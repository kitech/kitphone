// skaccount.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-17 09:15:43 +0800
// Version: $Id: skaccount.h 574 2010-11-17 05:23:57Z drswinghead $
// 

#ifndef _SKACCOUNT_H_
#define _SKACCOUNT_H_

#ifdef _WIN32
#include "Windows.h"
#else
#include <unistd.h>
#endif

#include <QtCore>

#include "skype-embedded_2.h"
// #include <sidg_skylibrefs.hpp>

//---------------------------------------------------------------------------------------
// Subclassing Account
class MyAccount : public QObject, public Account
{
    Q_OBJECT;
public:
    typedef DRef<MyAccount, Account> Ref;
    MyAccount(unsigned int oid, SERootObject* root);
    virtual ~MyAccount();
    MyAccount *self();

    void OnChange(int prop);
private:
    // Simple globals for the main loop. We set those two bools from within
    // Account::OnChange callback, checking for Account status property changes.
    bool LoggedIn;
    bool LoggedOut;

signals:
    void loginCompelete();
};



#endif /* _SKACCOUNT_H_ */
