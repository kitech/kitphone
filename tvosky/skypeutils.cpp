// skypeutils.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-14 11:13:03 +0800
// Version: $Id$
// 

#include "utils.h"

#include "cskype.h"
#include "skypeutils.h"

extern CSkype *gskype;

// static 
QString SkypeUtils::availablityIcon(int availability)
{
    QString icon_path;

    QString status_icons_name = "offline.png";
    switch (availability) {
    case Contact::UNKNOWN:
        break;
    case Contact::OFFLINE:
        status_icons_name = "offline.png";
        break;
    case Contact::INVISIBLE:
        status_icons_name = "invisible.png";
        break;
    case Contact::DO_NOT_DISTURB:
        status_icons_name = "away.png";
        break;
    case Contact::AWAY:
        status_icons_name = "idle.png";
        break;
    case Contact::SKYPE_ME:
        status_icons_name = "skypeme.png";
        break;
    case Contact::ONLINE:
        status_icons_name = "available.png";
        break;
    default:
        q_debug()<<"Unknown or uncare P_AVAILABILITY";
        break;
    };

    icon_path = QString(":/icons/status/") + status_icons_name;
    return icon_path;
}

