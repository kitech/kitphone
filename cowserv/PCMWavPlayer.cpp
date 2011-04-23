#include <stdio.h>
#include "SidPCMInterface.hpp"
#include "SidPlatform.hpp"
#ifndef _WIN32
#include <sys/times.h>
#include <unistd.h>
#endif


#define INPUT_BUF_ARRAY_SIZE 	50 
//#define SAMPLE_RATE		44100
#define SAMPLE_RATE		16000
// 8kHz, 16kHz, 24kHz, 32kHz, 44.1kHz, 48kHz
#define CALLBACK_INTERVAL	20

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


class PCMWavPlayer : public SkypePCMInterface, public Sid::Thread{
    Sid::String current_device_guid;// Sid::String("0");
    Sid::String current_device_name;//Sid::String("DefaultDevice");
    Sid::String current_device_productID;
public:


	PCMWavPlayer(SkypePCMCallbackInterface* transport):
        input_started(0), output_started(0), notification_started(0),
        input_muted(0), output_muted(0),
        input_volume(100), output_volume(100),
        input_sampleRate(SAMPLE_RATE), 
        output_sampleRate(SAMPLE_RATE),notification_sampleRate(SAMPLE_RATE),
        numOfOutputChannels(1),
        input_p(0),
        m_transport(transport)
	{
        for(int i = 0 ; i < INPUT_BUF_ARRAY_SIZE; i++)
            {
                input_buf[i].resize(SAMPLE_RATE * CALLBACK_INTERVAL * 2 / 1000 );
                memset((char* )input_buf[i].data(), 0, SAMPLE_RATE * CALLBACK_INTERVAL * 2 / 1000);
            }
        current_device_guid = Sid::String("0");
        current_device_name = Sid::String("DefaultDevice");
        current_device_productID = Sid::String("");
	}
  
	~PCMWavPlayer() 
	{
	}
	
    //	SkypePCMInterface::SendTransport* get_pcmtransport(){ return m_transport; }

	virtual int Init()
	{ 
        this->wav_file_name = "/home/gzleo/ivr_record/SKYPE1.wav";
        this->wav_file_fp = fopen(this->wav_file_name, "rw");
        printf("Init\n"); 
        Sid::Thread::start("callbackthread");
        return 0;
	}

	virtual int Start(int deviceType)
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
        return PCMIF_OK;
	}

	virtual int Stop(int deviceType){
        printf("Stop: %d\n", deviceType);
        if(deviceType == INPUT_DEVICE)	
            input_started = 0;
        if(deviceType == OUTPUT_DEVICE)
            output_started = 0;
        if(deviceType == NOTIFICATION_DEVICE)
            notification_started = 0;
        return PCMIF_OK;
	}

	virtual int GetDefaultDevice(int deviceType, Sid::String& guid, Sid::String& name, Sid::String& productID) 
	{
        guid = Sid::String("0");
        name = Sid::String("DefaultDevice");
        productID = Sid::String("");
        return 0;
	}
  
	virtual int UseDefaultDevice(int deviceType) 
	{
        printf("UseDefaultDevice: %d\n",deviceType);
        return PCMIF_OK;
	}

	virtual int GetCurrentDevice(int deviceType, Sid::String& guid, Sid::String& name, Sid::String& productID) 
	{
        printf("GetCurrentDevice: %d\n",deviceType);
        guid = current_device_guid;
        name = current_device_name;
        productID = current_device_productID;
        return PCMIF_OK;
	}
  
	virtual int UseDevice(int deviceType, const Sid::String& guid) 
	{
        printf("UseDevice: %d, %s\n",deviceType, (const char*)guid);
        return PCMIF_OK;
	}
  
	virtual int GetDeviceCount(int deviceType, uint& count) 
	{
        count = 3;
        return PCMIF_OK;
	}
  
	virtual int GetDevices(int deviceType, Sid::List_String& guid, Sid::List_String& name, Sid::List_String& productID) 
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
	
	virtual int GetVolumeParameters(int deviceType, unsigned int &range_min, unsigned int &range_max, unsigned int &volume, unsigned int &boost)
	{
        range_min = 0; 
        range_max = 100;
        printf("GetVolumeParameters %d\n", deviceType);
        if(deviceType == INPUT_DEVICE)
            volume = input_volume;
        else if (deviceType == OUTPUT_DEVICE)
            volume = output_volume;
        boost = 0;
        return PCMIF_OK;
	}
  
	virtual int SetVolume(int deviceType, unsigned int volume)
	{
        printf("SetVolume %d %d\n", deviceType, volume);
        if(deviceType == INPUT_DEVICE)
            input_volume = volume;
        else if (deviceType == OUTPUT_DEVICE)
            output_volume = volume;
        return PCMIF_OK;
	}
  
	virtual int SetInputBoost(int boost)
	{
        printf("SetInputBoost %d\n", boost);
        return PCMIF_ERROR_PROP_NOT_SUPPORTED;
	}

	virtual int GetMute(int deviceType, int &muted)
	{
        printf("GetMute %d\n", deviceType);
        if(deviceType == INPUT_DEVICE)
            muted = input_muted;
        else if (deviceType == OUTPUT_DEVICE)
            muted = output_muted;
        return PCMIF_OK; 
	}

	virtual int SetMute(int deviceType, int mute)
	{
        printf("SetMute %d %d\n", deviceType, mute);
        if(deviceType == INPUT_DEVICE)
            input_muted = mute;
        else if (deviceType == OUTPUT_DEVICE)
            output_muted = mute;
        return PCMIF_OK;
	}
	
	virtual int GetSampleRateCount(int deviceType, uint& count)
	{
        printf("GetSampleRateCount %d\n", deviceType); 
        count = 4;
        return PCMIF_OK;
	}

	virtual int GetSupportedSampleRates(int deviceType, Sid::List_uint& sampleRates)
	{
        printf("GetSupportedSampleRates: %d\n", deviceType);
        sampleRates.append(SAMPLE_RATE);
        sampleRates.append(24000);
        sampleRates.append(32000);
        sampleRates.append(48000); //         sampleRates.append(44100);
        return PCMIF_OK;
	}

	virtual int GetCurrentSampleRate(int deviceType, unsigned int &sampleRate)
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

	virtual int SetSampleRate(int deviceType, unsigned int sampleRate)
	{
        printf("SetSampleRate: %d %d\n", deviceType, sampleRate);
        if(sampleRate != SAMPLE_RATE)
            return PCMIF_ERROR; 
        if(deviceType == INPUT_DEVICE)
            input_sampleRate = sampleRate;
        else if (deviceType == OUTPUT_DEVICE)
            output_sampleRate = sampleRate;
        else if (deviceType == NOTIFICATION_DEVICE)
            notification_sampleRate = sampleRate;
        else
            return PCMIF_ERROR_UNKNOWN_DEVICE;
        return PCMIF_OK;
	}
	
	virtual int SetNumberOfChannels(int deviceType, int numberOfChannels)
	{
        printf("SetNumberOfOutputChannels %d\n", numberOfChannels);
        if(deviceType != OUTPUT_DEVICE)
            return PCMIF_ERROR_PROP_NOT_SUPPORTED;
        numOfOutputChannels = numberOfChannels;
        return PCMIF_OK;
	}

	virtual int CustomCommand(const Sid::String& command, Sid::String& response)
	{
        printf("CustomCommand %s\n", (char*)command.data());
		if(command == "PING")
			response = "PONG";
		else
			response = "NOT SUPPORTED";
		return PCMIF_OK;
	}

	void Run()
	{
        int sleep_interval = CALLBACK_INTERVAL;
        unsigned int stream_time;
        stream_time = get_milliseconds();

        while(1) {

            if(stream_time > get_milliseconds()) {
                Sleep(sleep_interval);
                continue;
            }

            stream_time += sleep_interval;
	    
            input_p ++;
            if(input_p >= INPUT_BUF_ARRAY_SIZE || input_p < 0)
                input_p = 0;

            int samples = output_sampleRate * sleep_interval / 1000;
	    
            if(output_started)
                m_transport->OutputDeviceReady(samples,output_sampleRate,numOfOutputChannels,output_buf);
            else {
                output_buf.resize(samples* 2 * numOfOutputChannels);
                memset((char*)output_buf.data(), 0, output_buf.size());
            }
	    
            if(input_started) {
                unsigned int samples_returned = output_buf.size() /( 2 * numOfOutputChannels );

                if(input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].size() != samples_returned * 2)
                    input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].resize(samples_returned * 2);
    
                if(input_muted) {
                    memset((char*)input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].data(), 0, input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].size());
                }
                else if(numOfOutputChannels > 1) {
                    short* ob = (short*) output_buf.data();
                    short* ib = (short*) input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].data();
                    for(unsigned int i = 0 ; i < samples_returned; i ++) {
                        ib[i] = ob[i * numOfOutputChannels];
                    }
                    fprintf(stdout, "numOfOutputChannels > 1, how can i do?\n");
                }
                else {
                    // fread(output_buf.data(), 1, samples_returned, this->wav_file_fp);
                    // input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)] = output_buf;
                    if (feof(this->wav_file_fp)) {
                        rewind(this->wav_file_fp);
                    }
                    fread(input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].data(), 1, 
                          input_buf[(input_p % INPUT_BUF_ARRAY_SIZE)].size(), this->wav_file_fp);
                }

                int input_samples_returned = input_buf[(input_p + 1) % INPUT_BUF_ARRAY_SIZE].size() / 2;
                m_transport->InputDeviceReady(input_samples_returned,input_sampleRate,1, input_buf[(input_p + 1) % INPUT_BUF_ARRAY_SIZE]);
                }
        }
	}

private:
    int input_started, output_started, notification_started;
    int input_muted, output_muted;
    int input_volume, output_volume;
    int input_sampleRate, output_sampleRate, notification_sampleRate;
    //	  int inputSamples, outputSamples;
    int numOfOutputChannels;
    Sid::Binary input_buf[INPUT_BUF_ARRAY_SIZE];
    Sid::Binary output_buf;
    int input_p;
    SkypePCMCallbackInterface* m_transport;

    char *wav_file_name;
    FILE *wav_file_fp;
};

SkypePCMInterface* SkypePCMInterfaceGet(SkypePCMCallbackInterface* transport)
{
    return new PCMWavPlayer(transport);
}
void SkypePCMInterfaceRelease(SkypePCMInterface* pcmif)
{
    delete pcmif;
}

