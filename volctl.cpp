// volctl.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-07 12:30:54 +0800
// Version: $Id$
// 

/* * Standard includes */ 

#include <stdio.h>
#include <stdlib.h>

#include "volctl.h"

#ifndef WIN32

#include <sys/ioctl.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <sys/soundcard.h> 
#include <sys/types.h>
#include <sys/stat.h>

unsigned int GetVolume()
{
    int mixer_fd;
    
    mixer_fd = open("/dev/mixer", O_RDONLY, 0);

    if (mixer_fd <= 0) {
        fprintf(stderr, "open mixer read error\n");
        return -1;
    }

    int vol = -1; 
    if (ioctl(mixer_fd, SOUND_MIXER_READ_VOLUME, &vol) == -1) {
        fprintf(stderr, "get mixer volme error\n");
    } 
    close(mixer_fd);
    return vol;

    return -1;
}

bool SetVolume(unsigned int vol)
{
    int mixer_fd;
    
    mixer_fd = open("/dev/mixer", O_WRONLY, 0);

    if (mixer_fd <= 0) {
        fprintf(stderr, "open mixer read error\n");
        return false;
    }

    if (ioctl(mixer_fd, SOUND_MIXER_WRITE_VOLUME, &vol) == -1) {
        fprintf(stderr, "get mixer volme error\n");
    } 
    close(mixer_fd);
    return true;

    return false;
}
#endif

#ifdef WIN32
#define   _WIN32_WINNT   0x0500   
#include <windows.h>
#include <stdio.h>
//调节音量的函数需要winmm.lib
#include <mmsystem.h>
#define _WIN32_WINNT 0x0500
#pragma comment(lib,"winmm.lib")

#define interval 20   //最低音量和最高音量分20个值

bool GetVolumeControl(HMIXER hmixer ,long componentType,long ctrlType,MIXERCONTROL* mxc)
{
    MIXERLINECONTROLS mxlc;
    MIXERLINE mxl;
    mxl.cbStruct = sizeof(mxl);
    mxl.dwComponentType = componentType;
    if (!mixerGetLineInfo((HMIXEROBJ)hmixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE)) {
        mxlc.cbStruct = sizeof(mxlc);
        mxlc.dwLineID = mxl.dwLineID;
        mxlc.dwControlType = ctrlType;
        mxlc.cControls = 1;
        mxlc.cbmxctrl = sizeof(MIXERCONTROL);
        mxlc.pamxctrl = mxc;
        if(mixerGetLineControls((HMIXEROBJ)hmixer,&mxlc,MIXER_GETLINECONTROLSF_ONEBYTYPE))
            return 0;
        else
            return 1;
    }
    return 0;
}


unsigned GetVolumeValue(HMIXER hmixer ,MIXERCONTROL *mxc)
{
    MIXERCONTROLDETAILS mxcd;
    MIXERCONTROLDETAILS_UNSIGNED vol; vol.dwValue=0;
    mxcd.hwndOwner = 0;
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.dwControlID = mxc->dwControlID;
    mxcd.cbDetails = sizeof(vol);
    mxcd.paDetails = &vol;
    mxcd.cChannels = 1;
    if (mixerGetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_GETCONTROLDETAILSF_VALUE))
        return -1;
    return vol.dwValue;
}


bool SetVolumeValue(HMIXER hmixer ,MIXERCONTROL *mxc, long volume)
{
    MIXERCONTROLDETAILS mxcd;
    MIXERCONTROLDETAILS_UNSIGNED vol;vol.dwValue = volume;
    mxcd.hwndOwner = 0;
    mxcd.dwControlID = mxc->dwControlID;
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.cbDetails = sizeof(vol);
    mxcd.paDetails = &vol;
    mxcd.cChannels = 1;
    if (mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE))
        return 0;
    return 1;
}


//设置音量值
bool SetVolume(unsigned int vol)
{
    bool Result = false;   //用于判断函数是否执行成功
    MIXERCONTROL volCtrl;
    HMIXER hmixer;
    if (mixerOpen(&hmixer, 0, 0, 0, 0)) return 0;
    if (GetVolumeControl(hmixer, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
                         MIXERCONTROL_CONTROLTYPE_VOLUME, &volCtrl)) {
        int ChangeValue=volCtrl.Bounds.lMaximum/interval;
        int TotalValue=ChangeValue*vol;
        if (SetVolumeValue(hmixer,&volCtrl,TotalValue))
            Result=true;
    }   
    mixerClose(hmixer);
    return Result;
}

//返回音量值
unsigned int GetVolume()
{
    unsigned int rt = 0;
    MIXERCONTROL volCtrl;
    HMIXER hmixer; 
    if (mixerOpen(&hmixer, 0, 0, 0, 0)) return 0;
    if (!GetVolumeControl(hmixer, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
                          MIXERCONTROL_CONTROLTYPE_VOLUME, &volCtrl)) {
        return 0;
    }
    int ChangeValue = volCtrl.Bounds.lMaximum/interval; //每一次调用函数改变的音量值
    rt = GetVolumeValue(hmixer,&volCtrl)/ChangeValue;   //GetVolumeValue得到的是具体值
    mixerClose(hmixer);
    return rt;
}

#endif
