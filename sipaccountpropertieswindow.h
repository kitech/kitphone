// sipaccountpropertieswindow.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:33:36 +0800
// Version: $Id: sipaccountpropertieswindow.h 581 2010-11-21 15:03:14Z drswinghead $
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
    explicit SipAccountPropertiesWindow(QString uname = QString(), QWidget *parent = 0);
    virtual ~SipAccountPropertiesWindow();

    SipAccount getAccount();
    bool setAccount(QString uname);

private:
    Ui::SipAccountPropertiesWindow *uiw;
};

#endif /* _SIPACCOUNTPROPERTIESWINDOW_H_ */
