// skaccount.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-17 09:16:00 +0800
// Version: $Id: skaccount.cpp 574 2010-11-17 05:23:57Z drswinghead $
// 

#include <QtCore>

#include "skaccount.h"

void Delay(int Sec)
{
#ifdef _WIN32
    Sleep(Sec * 1000);
#else
    sleep(Sec);
#endif
}

MyAccount::MyAccount(unsigned int oid, SERootObject* root) 
    : Account(oid, root), QObject(0)
{
    this->LoggedIn = false;
    this->LoggedOut = false;
}

MyAccount::~MyAccount()
{
}

MyAccount *MyAccount::self()
{
    return this;
}

// There are no separate events for different account properties -
// this callback will fire for all property changes.
void MyAccount::OnChange(int prop)
{
    // Here we are swicthing our global bools to inform our "main loop" of the
    // account status changes.
    qDebug()<<"account change event:"<<prop;
    if (prop == Account::P_STATUS) {
        Account::STATUS LoginStatus;
        GetPropStatus(LoginStatus);
        if (LoginStatus == Account::LOGGED_IN) {
            printf("Login complete.\n");
            this->LoggedIn = true;
            emit this->loginCompelete();
        } else if (LoginStatus == Account::LOGGED_OUT) {
            printf("Logout complete.\n");
            this->LoggedOut = true;
        }
    }
}
