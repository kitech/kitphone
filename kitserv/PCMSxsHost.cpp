#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include "SidPCMInterface.hpp"
#include "SidPlatform.hpp"
#include <errno.h>
#ifndef _WIN32
#include <sys/times.h>
#include <unistd.h>
#include <syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#endif

#include "resamplesubs.h"

#include "PCMSxsHost.h"

#include "rpc_command.h"
#include "kitserv.h"

static const char* deviceNames[] = {"INPUT_DEVICE", "OUTPUT_DEVICE", "NOTIFICATION_DEVICE"};

//#define SAMPLE_RATE		44100
// #define SAMPLE_RATE		16000
// 8kHz, 16kHz, 24kHz, 32kHz, 44.1kHz, 48kHz
#define SAMPLE_RATE_8K 8000
#define SAMPLE_RATE_16K 16000
#define SAMPLE_RATE_24K 24000
#define SAMPLE_RATE_32K 32000
#define SAMPLE_RATE_44_1K 44100
#define SAMPLE_RATE_48K  48000

#define CALLBACK_INTERVAL 20


int clocks_per_second = 0;
unsigned int get_milliseconds()
{
#ifdef _WIN32
	return GetTickCount();
#else
	if(clocks_per_second == 0) {
        
#ifdef _SC_CLK_TCK
        clocks_per_second = sysconf(_SC_CLK_TCK);
        if (clocks_per_second <= 0)
#endif
            clocks_per_second = CLOCKS_PER_SEC;
    }
	struct tms t;
	int rv = ((unsigned long long)(unsigned int)times(&t)) * 1000 / clocks_per_second;
	
	return rv;
#endif
}

PCMSxsHost::PCMSxsHost(SkypePCMCallbackInterface* transport, void *ctx)
    :
    input_started(0), output_started(0), notification_started(0),
    input_muted(0), output_muted(0),
    input_volume(100), output_volume(100),
    input_sampleRate(SAMPLE_RATE_24K), 
    output_sampleRate(SAMPLE_RATE_24K),notification_sampleRate(SAMPLE_RATE_24K),
    numOfOutputChannels(1),
    //    input_p(0),
    m_transport(transport)
{
    // for(int i = 0 ; i < INPUT_BUF_ARRAY_SIZE; i++) {
    //     input_buf[i].resize(SAMPLE_RATE * CALLBACK_INTERVAL * 2 / 1000 );
    //     memset((char* )input_buf[i].data(), 0, SAMPLE_RATE * CALLBACK_INTERVAL * 2 / 1000);
    // }
    current_device_guid = Sid::String("0");
    current_device_name = Sid::String("DefaultDevice");
    current_device_productID = Sid::String("");

    //// 
    this->m_ctx = ctx;
    this->media_mode = MEDIA_MODE_PLAYER;

}
  
PCMSxsHost::~PCMSxsHost() 
{
}
	
//	SkypePCMInterface::SendTransport* get_pcmtransport(){ return m_transport; }

int PCMSxsHost::Init()
{ 
    printf("Init\n"); 

    // put here ok? or put Start?
    memset(this->wav_file_name, 0, sizeof(this->wav_file_name));
    // this->wav_file_name = "/home/gzleo/ivr_record/SKYPE1.wav";
    snprintf(this->wav_file_name, sizeof(this->wav_file_name)-1, "/home/gzleo/ivr_record/SKYPE1.wav");

    memset(this->wav_file_name, 0, sizeof(this->wav_file_name));
    snprintf(this->wav_file_name, sizeof(this->wav_file_name)-1, "%s/SKYPE1.wav", getenv("HOME"));
    this->wav_file_fp = fopen(this->wav_file_name, "rw");

    this->input_rb = ringbuffer_create(512*1024);
    this->output_rb = ringbuffer_create(512*1024);

    Sid::Thread::start("callbackthread");
    return 0;
}

int PCMSxsHost::Start(int deviceType)
{
    if(deviceType == INPUT_DEVICE) {
        printf("Start INPUT_DEVICE\n");
        input_started = 1;
    }
    else if(deviceType == OUTPUT_DEVICE) {
        printf("Start OUTPUT_DEVICE\n");
        output_started = 1;
    }
    else if(deviceType == NOTIFICATION_DEVICE) {
        printf("Start NOTIFICATION_DEVICE\n");
        notification_started = 1;
    }

    rewind(this->wav_file_fp);
    this->switch_media_mode_player();

    return PCMIF_OK;
}

int PCMSxsHost::Stop(int deviceType){
    printf("Stop: %d\n", deviceType);
    if(deviceType == INPUT_DEVICE)	
        input_started = 0;
    if(deviceType == OUTPUT_DEVICE)
        output_started = 0;
    if(deviceType == NOTIFICATION_DEVICE)
        notification_started = 0;

    this->stop_clear_buffer();

    return PCMIF_OK;
}

int PCMSxsHost::GetDefaultDevice(int deviceType, Sid::String& guid, Sid::String& name, Sid::String& productID) 
{
    guid = Sid::String("0");
    name = Sid::String("DefaultDevice");
    productID = Sid::String("");
    return 0;
}
  
int PCMSxsHost::UseDefaultDevice(int deviceType) 
{
    printf("UseDefaultDevice: %d\n",deviceType);
    return PCMIF_OK;
}

int PCMSxsHost::GetCurrentDevice(int deviceType, Sid::String& guid, Sid::String& name, Sid::String& productID) 
{
    printf("GetCurrentDevice: %d\n",deviceType);
    guid = current_device_guid;
    name = current_device_name;
    productID = current_device_productID;
    return PCMIF_OK;
}
  
int PCMSxsHost::UseDevice(int deviceType, const Sid::String& guid) 
{
    printf("UseDevice: %d, %s\n",deviceType, (const char*)guid);
    return PCMIF_OK;
}
  
int PCMSxsHost::GetDeviceCount(int deviceType, uint& count) 
{
    count = 3;
    return PCMIF_OK;
}
  
int PCMSxsHost::GetDevices(int deviceType, Sid::List_String& guid, Sid::List_String& name, Sid::List_String& productID) 
{
    printf("GetDevices\n");
	  
    guid.append("guid0");
    guid.append("9");
    guid.append("guid10");

    name.append("DefaultDevice");
    name.append("Dev9");
    name.append("Dev10");

    productID.append("productID 1");
    productID.append("productID 2");
    productID.append("productID 3");

    return PCMIF_OK;
}
	
// int PCMSxsHost::GetVolumeParameters(int deviceType, unsigned int &range_min, unsigned int &range_max, unsigned int &volume, unsigned int &boost)
// {
//     range_min = 0; 
//     range_max = 100;
//     printf("GetVolumeParameters %d\n", deviceType);
//     if(deviceType == INPUT_DEVICE)
//         volume = input_volume;
//     else if (deviceType == OUTPUT_DEVICE)
//         volume = output_volume;
//     boost = 0;
//     return PCMIF_OK;
// }

int PCMSxsHost::GetVolumeParameters(int deviceType, unsigned int &range_min, unsigned int &range_max, unsigned int &volume, int &boost)
{
    range_min = 0; 
    range_max = 100;
    printf("GetVolumeParameters: %s\n", deviceNames[deviceType]);
    if(deviceType == INPUT_DEVICE)
        volume = input_volume;
    else if (deviceType == OUTPUT_DEVICE)
        volume = output_volume;

    // input boost is not supported in this example
    boost = -1;
    return PCMIF_OK;
}

int PCMSxsHost::SetVolume(int deviceType, unsigned int volume)
{
    printf("SetVolume %d %d\n", deviceType, volume);
    if(deviceType == INPUT_DEVICE)
        input_volume = volume;
    else if (deviceType == OUTPUT_DEVICE)
        output_volume = volume;
    return PCMIF_OK;
}
  
int PCMSxsHost::SetInputBoost(int boost)
{
    printf("SetInputBoost %d\n", boost);
    return PCMIF_ERROR_PROP_NOT_SUPPORTED;
}

int PCMSxsHost::GetMute(int deviceType, int &muted)
{
    printf("GetMute %d\n", deviceType);
    if(deviceType == INPUT_DEVICE)
        muted = input_muted;
    else if (deviceType == OUTPUT_DEVICE)
        muted = output_muted;
    return PCMIF_OK; 
}

int PCMSxsHost::SetMute(int deviceType, int mute)
{
    printf("SetMute %d %d\n", deviceType, mute);
    if(deviceType == INPUT_DEVICE)
        input_muted = mute;
    else if (deviceType == OUTPUT_DEVICE)
        output_muted = mute;
    return PCMIF_OK;
}
	
int PCMSxsHost::GetSampleRateCount(int deviceType, uint& count)
{
    printf("GetSampleRateCount %d\n", deviceType); 

    // count = 1;
    count = 6;

    return PCMIF_OK;
}

int PCMSxsHost::GetSupportedSampleRates(int deviceType, Sid::List_uint& sampleRates)
{
    printf("GetSupportedSampleRates: %d\n", deviceType);

    sampleRates.append(SAMPLE_RATE_8K);
    sampleRates.append(SAMPLE_RATE_16K);
    sampleRates.append(SAMPLE_RATE_24K);
    sampleRates.append(SAMPLE_RATE_32K);
    sampleRates.append(SAMPLE_RATE_44_1K);
    sampleRates.append(SAMPLE_RATE_48K);

    return PCMIF_OK;
}

int PCMSxsHost::GetCurrentSampleRate(int deviceType, unsigned int &sampleRate)
{
    if(deviceType == INPUT_DEVICE)
        sampleRate = input_sampleRate;
    else if (deviceType == OUTPUT_DEVICE)
        sampleRate = output_sampleRate;
    else if (deviceType == NOTIFICATION_DEVICE)
        sampleRate = notification_sampleRate;
    else
        return PCMIF_ERROR_UNKNOWN_DEVICE;
    printf("GetCurrentSampleRate: %d %d\n", deviceType, sampleRate);
    return PCMIF_OK;
}

int PCMSxsHost::SetSampleRate(int deviceType, unsigned int sampleRate)
{
    printf("SetSampleRate: %d %d\n", deviceType, sampleRate);
    if(sampleRate != SAMPLE_RATE_8K
       && sampleRate != SAMPLE_RATE_16K
       && sampleRate != SAMPLE_RATE_24K
       && sampleRate != SAMPLE_RATE_32K
       && sampleRate != SAMPLE_RATE_44_1K
       && sampleRate != SAMPLE_RATE_48K)
        return PCMIF_ERROR; 
    if(deviceType == INPUT_DEVICE)
        input_sampleRate = sampleRate;
    else if (deviceType == OUTPUT_DEVICE)
        output_sampleRate = sampleRate;
    else if (deviceType == NOTIFICATION_DEVICE)
        notification_sampleRate = sampleRate;
    else
        return PCMIF_ERROR_UNKNOWN_DEVICE;

    // note bus thread
    ServContex *ctx = (ServContex*)this->m_ctx;
    QStringList strlist;
    strlist << QString::number(deviceType) << QString::number(sampleRate);
    ctx->post_schedule_cmd(RPCMD::PCMIF_SAMPLE_RATE_CHANGED, ctx, strlist);

    return PCMIF_OK;
}
	
int PCMSxsHost::SetNumberOfChannels(int deviceType, int numberOfChannels)
{
    printf("SetNumberOfOutputChannels %d\n", numberOfChannels);
    if(deviceType != OUTPUT_DEVICE)
        return PCMIF_ERROR_PROP_NOT_SUPPORTED;
    numOfOutputChannels = numberOfChannels;
    return PCMIF_OK;
}

int PCMSxsHost::CustomCommand(const Sid::String& command, Sid::String& response)
{
    printf("CustomCommand %s\n", (char*)command.data());
    if(command == "PING")
        response = "PONG";
    else
        response = "NOT SUPPORTED";
    return PCMIF_OK;
}

// void PCMSxsHost::Run_old()
// {
//     int sleep_interval = CALLBACK_INTERVAL;
//     unsigned int stream_time;
//     stream_time = get_milliseconds();

//     while(1) {

//         if(stream_time > get_milliseconds()) {
//             Sleep(sleep_interval);
//             continue;
//         }

//         stream_time += sleep_interval;
	    
//         input_p ++;
//         if(input_p >= INPUT_BUF_ARRAY_SIZE || input_p < 0)
//             input_p = 0;

//         int samples = output_sampleRate * sleep_interval / 1000;
	    
//         if(output_started)
//             m_transport->OutputDeviceReady(samples,output_sampleRate,numOfOutputChannels,output_buf);
//         else {
//             output_buf.resize(samples* 2 * numOfOutputChannels);
//             memset((char*)output_buf.data(), 0, output_buf.size());
//         }
	    
//         if(input_started) {
//             unsigned int samples_returned = output_buf.size() /( 2 * numOfOutputChannels );

//             if(input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].size() != samples_returned * 2)
//                 input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].resize(samples_returned * 2);
    
//             if(input_muted) {
//                 memset((char*)input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].data(), 0, input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].size());
//             }
//             else if(numOfOutputChannels > 1) {
//                 short* ob = (short*) output_buf.data();
//                 short* ib = (short*) input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].data();
//                 for(unsigned int i = 0 ; i < samples_returned; i ++) {
//                     ib[i] = ob[i * numOfOutputChannels];
//                 }
//                 fprintf(stdout, "numOfOutputChannels > 1, how can i do?\n");
//             }
//             else {
//                 input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)] = output_buf;
//             }

//             int input_samples_returned = input_buf[(input_p + 1) % INPUT_BUF_ARRAY_SIZE].size() / 2;
//             m_transport->InputDeviceReady(input_samples_returned,input_sampleRate,1, input_buf[(input_p + 1) % INPUT_BUF_ARRAY_SIZE]);
//         }
//     }
// }

void PCMSxsHost::Run()
{
    int sleep_interval = CALLBACK_INTERVAL;
    unsigned int stream_time;
    int iret;

    fprintf(stdout, "media host thread: %d\n", syscall(__NR_gettid));

    stream_time = get_milliseconds();
    while(1) {
        // fprintf(stdout, "media host thread: %d\n", syscall(__NR_gettid));
        if(stream_time > get_milliseconds()) {
            Sleep(sleep_interval);
            continue;
        }

        stream_time += sleep_interval;

        if (this->media_mode == MEDIA_MODE_SXS) {
            this->run_media_mode_sxs(sleep_interval);
        } else if (this->media_mode == MEDIA_MODE_PLAYER) {
            this->run_media_mode_player(sleep_interval);
        }
    }
}

void PCMSxsHost::run_media_mode_player(int sleep_interval)
{
    int iret = 0;

    int samples = output_sampleRate * sleep_interval / 1000;
	    
    if(output_started) {
        m_transport->OutputDeviceReady(samples,output_sampleRate,numOfOutputChannels,xoutput_buf);
    } else {
        xoutput_buf.resize(samples* 2 * numOfOutputChannels);
        memset((char*)xoutput_buf.data(), 0, xoutput_buf.size());
    }
	    
    if(input_started) {
        unsigned int samples_returned = xoutput_buf.size() /( 2 * numOfOutputChannels );

        if(xinput_buf.size() != samples_returned * 2)
            xinput_buf.resize(samples_returned * 2);
    
        if(input_muted) {
            memset((char*)xinput_buf.data(), 0, xinput_buf.size());
        } else if(numOfOutputChannels > 1) {
            short* ob = (short*) xoutput_buf.data();
            short* ib = (short*) xinput_buf.data();
            for(unsigned int i = 0 ; i < samples_returned; i ++) {
                ib[i] = ob[i * numOfOutputChannels];
            }
            fprintf(stdout, "numOfOutputChannels > 1, how can i do?\n");
        } else {
            // fread(output_buf.data(), 1, samples_returned, this->wav_file_fp);
            if (feof(this->wav_file_fp)) {
                rewind(this->wav_file_fp);
            }
            fread(xinput_buf.data(), 1, xinput_buf.size(), this->wav_file_fp);
        }
        // double factor = rate_out * 1.0 / rate_in;
        double factor = this->output_sampleRate * 1.0 / SAMPLE_RATE_16K;
        char large_filter = 0;
        char high_quality = 1;
        int input_samples_returned = xinput_buf.size() / 2;
        if (SAMPLE_RATE_16K == this->output_sampleRate) {
            m_transport->InputDeviceReady(input_samples_returned,input_sampleRate,1, xinput_buf);
        } else {
            // m_transport->InputDeviceReady(input_samples_returned,input_sampleRate,1, xinput_buf);
            xresample_buf.resize(xinput_buf.size());
            memset(xresample_buf.data(), 0, xresample_buf.size());
            res_Resample((int16_t*)xinput_buf.data(), (int16_t*)xresample_buf.data(),
                         factor, xinput_buf.size(), large_filter, high_quality);
            m_transport->InputDeviceReady(input_samples_returned,input_sampleRate,1, xresample_buf);
            // resample后语音不太清晰
        }
    }
}

void PCMSxsHost::run_media_mode_sxs(int sleep_interval)
{
    // fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);
    if (1) {
        this->run_media_mode_sxs_sock_pair(sleep_interval);
    } else {
        // old method
        this->run_media_mode_sxs_share_rb(sleep_interval);
    }
}

void PCMSxsHost::run_media_mode_sxs_share_rb(int sleep_interval)
{
    // fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    int iret = 0;
    int out_samples = output_sampleRate * sleep_interval / 1000;
	    
    if(output_started) {
        m_transport->OutputDeviceReady(out_samples, output_sampleRate, 
                                       numOfOutputChannels, xoutput_buf);
        // free some space if buffer full
        if (ringbuffer_write_space(this->output_rb) < xoutput_buf.size()) {
            // fprintf(stdout, "pcmhost output buffer is full, clear some data\n");
            if (this->clear_buf.size() < xoutput_buf.size()) {
                // clear all 
                this->clear_buf.resize(ringbuffer_read_space(this->output_rb));
            }
            iret = ringbuffer_read(this->output_rb, clear_buf.data(), xoutput_buf.size());
        }
        iret = ringbuffer_write(this->output_rb, xoutput_buf.data(), xoutput_buf.size());
    } else {
        xoutput_buf.resize(out_samples* 2 * numOfOutputChannels);
        memset((char*)xoutput_buf.data(), 0, xoutput_buf.size());
    }

    iret = ringbuffer_read_space(this->input_rb);
    if(input_started && iret >= out_samples * 2) {
        //  fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);
        unsigned int in_samples = out_samples;
        if (xinput_buf.size() != in_samples *2) {
            xinput_buf.resize(in_samples * 2);
        }

        iret = ringbuffer_read(this->input_rb, xinput_buf.data(), in_samples * 2);

        if (input_muted) {
            // drop data if muted
            memset((char*)xinput_buf.data(), 0, xinput_buf.size());
            // fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);
        } else {
            // memset((char*)xinput_buf.data(), 0, xinput_buf.size());
            // fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);
        }
        // fprintf(stdout, "%s %d %s sample %d, rate %d, size %d\n", __FILE__, __LINE__, __FUNCTION__,
        //         in_samples, input_sampleRate, xinput_buf.size());
        this->m_transport->InputDeviceReady(in_samples, input_sampleRate, 1, xinput_buf);
    }
}

void PCMSxsHost::run_media_mode_sxs_sock_pair(int sleep_interval)
{
    // fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    int iret = 0;
    int out_samples = output_sampleRate * sleep_interval / 1000;
    ServContex *ctx = (ServContex*)this->m_ctx;
    int fd = ctx->m_sock_pair_sv[1];
    char rbuf[5120] = {0};
	    
    if(output_started) {
        m_transport->OutputDeviceReady(out_samples, output_sampleRate, 
                                       numOfOutputChannels, xoutput_buf);
        iret = ::write(fd, xoutput_buf.data(), xoutput_buf.size());
        assert(iret == xoutput_buf.size());
        // // free some space if buffer full
        // if (ringbuffer_write_space(this->output_rb) < xoutput_buf.size()) {
        //     // fprintf(stdout, "pcmhost output buffer is full, clear some data\n");
        //     if (this->clear_buf.size() < xoutput_buf.size()) {
        //         // clear all 
        //         this->clear_buf.resize(ringbuffer_read_space(this->output_rb));
        //     }
        //     iret = ringbuffer_read(this->output_rb, clear_buf.data(), xoutput_buf.size());
        // }
        // iret = ringbuffer_write(this->output_rb, xoutput_buf.data(), xoutput_buf.size());
    } else {
        xoutput_buf.resize(out_samples* 2 * numOfOutputChannels);
        memset((char*)xoutput_buf.data(), 0, xoutput_buf.size());
    }

    if(input_started) {
        //  fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);
        unsigned int in_samples = out_samples;
        if (xinput_buf.size() != in_samples *2) {
            xinput_buf.resize(in_samples * 2);
        }

        // iret = ringbuffer_read(this->input_rb, xinput_buf.data(), in_samples * 2);
        // 实现非阻塞读取，如果没有可读取数据，则不需要发送数据而已。
#ifdef HAVE_SOCK_NONBLOCK
        iret = ::read(fd, rbuf, in_samples * 2);
#else
    retry_select:
        fd_set rd_fd_set;
        FD_ZERO(&rd_fd_set);
        FD_SET(fd, &rd_fd_set);

        struct timeval timev;
        timev.tv_sec = 0;
        timev.tv_usec = 1000 * 2; // 2ms, 最多可阻塞等待2ms
        
        iret = ::select(fd + 1, &rd_fd_set, 0, 0, &timev);
        if (iret < 0) {
            // error
            if (errno == EINTR) {
                goto retry_select;
            } else {
                assert(1 == 2);
            }
        } else if (iret == 0) {
            // timeout, no readable fd
        } else {
            iret = ::read(fd, rbuf, in_samples * 2);
        }
#endif
        
        if (iret < 0) {
            return;
        } else if (iret == 0) {
            return;
        } else {
            memcpy(xinput_buf.data(), rbuf, in_samples * 2);
            if (input_muted) {
                // drop data if muted
                memset((char*)xinput_buf.data(), 0, xinput_buf.size());
                // fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);
            } else {
                // memset((char*)xinput_buf.data(), 0, xinput_buf.size());
                // fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);
            }
            // fprintf(stdout, "%s %d %s sample %d, rate %d, size %d\n", __FILE__, __LINE__, __FUNCTION__,
            //         in_samples, input_sampleRate, xinput_buf.size());
            this->m_transport->InputDeviceReady(in_samples, input_sampleRate, 1, xinput_buf);
        }
    }
}

/*
  可能在挂断后，socketpair套接字缓存仍有缓存的数据，
  这部分数据可能是没有播放完的，在这清理掉，防止下次通话时播放给别的通话了。
  如，您呼叫的用户暂时无法...，在这停止了，
  但下次呼叫，会首先听到“接听...."
 */
bool PCMSxsHost::stop_clear_buffer()
{
    ServContex *ctx = (ServContex*)this->m_ctx;
    int fd = ctx->m_sock_pair_sv[1];
    char rbuf[5120];
    int iret = 0;
    int counter = 0;

    while (counter ++ < 100) {
#ifdef HAVE_SOCK_NONBLOCK
        iret = ::read(fd, rbuf, sizeof(rbuf));
#else
    retry_select:
        fd_set rd_fd_set;
        FD_ZERO(&rd_fd_set);
        FD_SET(fd, &rd_fd_set);

        struct timeval timev;
        timev.tv_sec = 0;
        timev.tv_usec = 1000 * 2; // 2ms, 最多可阻塞等待2ms
        
        iret = ::select(fd + 1, &rd_fd_set, 0, 0, &timev);
        if (iret < 0) {
            // error
            if (errno == EINTR) {
                goto retry_select;
            } else {
                assert(1 == 2);
            }
        } else if (iret == 0) {
            // timeout, no readable fd
        } else {
            iret = ::read(fd, rbuf, sizeof(rbuf));
        }
#endif
        
        if (iret < 0) {
            break;
        } else if (iret == 0) {
            break;
        } else {
            continue;
        }
    }

    return true;
}

int PCMSxsHost::input_append_frame(char *src, int len)
{
    int ilen = 0;

    fprintf(stdout, "%s %d %s \n", __FILE__, __LINE__, __FUNCTION__);

    ilen = ringbuffer_write(this->input_rb, src, len);

    // fprintf(stdout, "pmc host debug info: %d, \n", this->input_sampleRate);
    // xinput_buf.resize(len);
    // memcpy(xinput_buf.data(), src, len);
    // this->m_transport->InputDeviceReady(len/2, input_sampleRate, 1, xinput_buf);

    return ilen;
}

int PCMSxsHost::output_levy_frame(char *dest, int len)
{
    int ilen = 0;

    ilen = ringbuffer_read_space(this->output_rb);

    if (ilen >= len) {
        ilen = ringbuffer_read(this->output_rb, dest, len);
        return ilen;
    }

    return 0;
}

bool PCMSxsHost::switch_media_mode_player()
{
    fprintf(stdout, "change pcm host to player mode.....\n");
    if (this->media_mode != MEDIA_MODE_PLAYER) {
        this->media_mode = MEDIA_MODE_PLAYER;
    }
    return true;
}

bool PCMSxsHost::switch_media_mode_sxs()
{
    fprintf(stdout, "change pcm host to sxs mode.....\n");
    if (this->media_mode != MEDIA_MODE_SXS) {
        this->media_mode = MEDIA_MODE_SXS;
    }

    return true;
}

// depcreated
SkypePCMInterface* SkypePCMInterfaceGet(SkypePCMCallbackInterface* transport)
{
    fprintf(stdout, "Error, this function is depcreated by project. relax.\n");
    return new PCMSxsHost(transport, 0);
}

// depcreated
void SkypePCMInterfaceRelease(SkypePCMInterface* pcmif)
{
    fprintf(stdout, "Error, this function is depcreated by project. relax.\n");
    delete pcmif;
}

// 扩展的带外带参数的对应函数
SkypePCMInterface * SkypePCMInterfaceGetEx(SkypePCMCallbackInterface *transport, void *ctx)
{
    SkypePCMInterface *pcmif = NULL;

    pcmif = new PCMSxsHost(transport, ctx);

    return pcmif;
}

void SkypePCMInterfaceReleaseEx(SkypePCMInterface* pcmif, void *ctx)
{
    assert(pcmif != NULL);
    assert(ctx != NULL);

    ((ServContex *)ctx)->pcmif = NULL;
    delete pcmif;
}

