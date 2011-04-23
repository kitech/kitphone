// buddybox.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-12 12:42:41 +0800
// Version: $Id$
// 

#include "utils.h"

#include "cskype.h"

#include "skype-object.h"
#include "helper-functions.h"

/** global variables */
extern CSkype* gskype;

#include "skypeutils.h"

#include "ui_buddybox.h"
#include "buddybox.h"


BuddyBox::BuddyBox(QWidget *parent)
    : QWidget(parent)
    , uiw(new Ui::BuddyBox())
{
    this->uiw->setupUi(this);
    this->conv = CConversation::Ref(0);
}

BuddyBox::~BuddyBox()
{
    q_debug()<<"";
}

void BuddyBox::setConversation(Conversation::Ref &conv)
{
    SEString identity, display_name, country, mood_text;
    Conversation::LOCAL_LIVESTATUS status;
    CContact::Ref contact;
    Contact::AVAILABILITY availability;

    uint last_time, time_zone;

    this->conv = conv;

    conv->GetPropIdentity(identity);
    conv->GetPropDisplayname(display_name);
    conv->GetPropLocalLivestatus(status);
    conv->GetPropLastActivityTimestamp(last_time);

    this->setIdentity(QString(identity.data()));
    this->setDisplayname(gskype->codec->toUnicode(display_name.data()));
    this->setStatus(status);

    QString user_time_pos = QDateTime::fromTime_t(last_time).toString("h:m");
    this->uiw->label_2->setText(user_time_pos);
    // country is in contact

    QDateTime btime, etime;
    btime = QDateTime::currentDateTime();

    // contact = gskype->find_contact(identity); // time cost very much, why???
    contact = CContact::Ref(0);

    etime = QDateTime::currentDateTime();
    q_debug()<<"here2"<<btime.msecsTo(etime);

    if (contact.present()) {
        contact->GetPropCountry(country);

        contact->GetPropTimezone(time_zone);
        user_time_pos += QString("(%1),%2").arg(time_zone).arg(country.data());
        this->uiw->label_2->setText(user_time_pos);

        contact->GetPropMoodText(mood_text);
        this->setSignText(gskype->codec->toUnicode(mood_text.data()));

        contact->GetPropAvailability(availability);
        this->setAvailability(availability);
    } else {
    }
}

void BuddyBox::setIdentity(QString identity)
{
    this->identity = identity;
}

void BuddyBox::setDisplayname(QString display_name)
{
    this->display_name = display_name;
    this->uiw->toolButton_4->setText(this->display_name);
}

void BuddyBox::setStatus(int status)
{
    this->status = status;
    this->uiw->label->setText(QString("S:") + QString::number(this->status)); 
}

void BuddyBox::setSignText(QString sign_text)
{
    this->sign_text = sign_text;
    this->uiw->plainTextEdit->setPlainText(this->sign_text);
}

void BuddyBox::setAvailability(int availability)
{
    this->_availability = availability;
    QString status_icons_name = SkypeUtils::availablityIcon(availability);
    this->uiw->toolButton_4->setIcon(QIcon(status_icons_name));
}
