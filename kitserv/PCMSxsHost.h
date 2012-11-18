// PCMSxsHost.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-25 21:46:57 +0800
// Version: $Id$
// 


#ifndef _PCMSXSHOST_H_
#define _PCMSXSHOST_H_

#include <stdio.h>
#include "SidPCMInterface.hpp"
#include "SidPlatform.hpp"
#ifndef _WIN32
#include <sys/times.h>
#include <unistd.h>
#endif

#include "ringbuffer.h"

// #define INPUT_BUF_ARRAY_SIZE 	50 

class PCMSxsHost : public SkypePCMInterface, public Sid::Thread 
{
    Sid::String current_device_guid;// Sid::String("0");
    Sid::String current_device_name;//Sid::String("DefaultDevice");
    Sid::String current_device_productID;
public:

	PCMSxsHost(SkypePCMCallbackInterface* transport, void *ctx);
  
	~PCMSxsHost() ;
	
    //	SkypePCMInterface::SendTransport* get_pcmtransport(){ return m_transport; }

	virtual int Init();

	virtual int Start(int deviceType);

	virtual int Stop(int deviceType);

	virtual int GetDefaultDevice(int deviceType, Sid::String& guid, Sid::String& name, Sid::String& productID) ;
  
	virtual int UseDefaultDevice(int deviceType) ;

	virtual int GetCurrentDevice(int deviceType, Sid::String& guid, Sid::String& name, Sid::String& productID) ;
  
	virtual int UseDevice(int deviceType, const Sid::String& guid) ;
  
	virtual int GetDeviceCount(int deviceType, uint& count) ;
  
	virtual int GetDevices(int deviceType, Sid::List_String& guid, Sid::List_String& name, Sid::List_String& productID) ;
	
	// virtual int GetVolumeParameters(int deviceType, unsigned int &range_min, unsigned int &range_max, unsigned int &volume, unsigned int &boost);
    virtual int GetVolumeParameters(int deviceType, unsigned int &range_min, unsigned int &range_max, unsigned int &volume, int &boost);
  
	virtual int SetVolume(int deviceType, unsigned int volume);
  
	virtual int SetInputBoost(int boost);

	virtual int GetMute(int deviceType, int &muted);

    virtual int SetMute(int deviceType, int mute);
	
	virtual int GetSampleRateCount(int deviceType, uint& count);

	virtual int GetSupportedSampleRates(int deviceType, Sid::List_uint& sampleRates);

	virtual int GetCurrentSampleRate(int deviceType, unsigned int &sampleRate);

	virtual int SetSampleRate(int deviceType, unsigned int sampleRate);
	virtual int SetNumberOfChannels(int deviceType, int numberOfChannels);

	virtual int CustomCommand(const Sid::String& command, Sid::String& response);

	// void Run_old();

	void Run();
    void run_media_mode_player(int sleep_interval);
    void run_media_mode_sxs(int sleep_interval);
    void run_media_mode_sxs_share_rb(int sleep_interval);
    void run_media_mode_sxs_sock_pair(int sleep_interval);

    int input_append_frame(char *src, int len);

    int output_levy_frame(char *dest, int len);

    bool switch_media_mode_player();
    bool switch_media_mode_sxs();
    bool stop_clear_buffer();

private:
    int input_started, output_started, notification_started;
    int input_muted, output_muted;
    int input_volume, output_volume;
    int input_sampleRate, output_sampleRate, notification_sampleRate;
    //	  int inputSamples, outputSamples;
    int numOfOutputChannels;
    // Sid::Binary input_buf[INPUT_BUF_ARRAY_SIZE];
    // Sid::Binary output_buf;
    // int input_p;
    SkypePCMCallbackInterface* m_transport;

    Sid::Binary clear_buf;
    Sid::Binary xinput_buf;
    Sid::Binary xoutput_buf;
    Sid::Binary xresample_buf;
    ringbuffer_t *input_rb;
    ringbuffer_t *output_rb;

    char wav_file_name[256];
    FILE *wav_file_fp;

    enum { MEDIA_MODE_PLAYER = 1, MEDIA_MODE_SXS };
    int media_mode; // 
    void *m_ctx;

};

// replacement of original interface
// extern "C" SkypePCMInterface* SkypePCMInterfaceGet(SkypePCMCallbackInterface*);
// extern "C" void SkypePCMInterfaceRelease(SkypePCMInterface*);

SkypePCMInterface *SkypePCMInterfaceGetEx(SkypePCMCallbackInterface *transport, void *ctx);
void SkypePCMInterfaceReleaseEx(SkypePCMInterface* pcmif, void *ctx);

#endif /* _PCMSXSHOST_H_ */
