// sipaccountswindow.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:25:31 +0800
// Version: $Id: sipaccountswindow.h 875 2011-05-09 10:31:22Z drswinghead $
// 
#ifndef _SIPACCOUNTSWINDOW_H_
#define _SIPACCOUNTSWINDOW_H_

#include <QtCore>
#include <QtGui>

#include "boost/smart_ptr.hpp"

namespace Ui {
    class SipAccountsWindow;
}

class SipAccountsWindow : public QDialog
{
    Q_OBJECT;
public:
    explicit SipAccountsWindow(QWidget *parent = 0);
    virtual ~SipAccountsWindow();

public slots:
    void onClose();
    void onApply();

    void onNewAccount();
    void onRemoveAccount();
    void onModifyAccount();
    void onMakeDefaultAccount();

    void onSetLogin(bool checked);
public:
    void reload();

signals:
    void accountWantRegister(QString userName, bool reg);
    void accountWantRemove(QString userName);

private:
    Ui::SipAccountsWindow *uiw;
};

#endif /* _SIPACCOUNTSWINDOW_H_ */
