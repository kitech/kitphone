// sipaccountpropertieswindow.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:33:36 +0800
// Version: $Id: sipaccountpropertieswindow.h 876 2011-05-11 14:25:21Z drswinghead $
// 

#ifndef _SIPACCOUNTPROPERTIESWINDOW_H_
#define _SIPACCOUNTPROPERTIESWINDOW_H_

#include <QtCore>
#include <QtGui>

#include "sipaccount.h"

namespace Ui {
    class SipAccountPropertiesWindow;
};

class SipAccountPropertiesWindow : public QDialog
{
    Q_OBJECT;
public:
    // explicit SipAccountPropertiesWindow(QString uname = QString(), QWidget *parent = 0);
    explicit SipAccountPropertiesWindow(SipAccount &acc, QWidget *parent = 0);
    virtual ~SipAccountPropertiesWindow();

    SipAccount getAccount();
    // bool setAccount(QString uname);
    bool setAccount();

private:
    Ui::SipAccountPropertiesWindow *uiw;
    SipAccount macc;
};

#endif /* _SIPACCOUNTPROPERTIESWINDOW_H_ */
