// sipaccountswindow.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:25:31 +0800
// Version: $Id: sipaccountswindow.h 581 2010-11-21 15:03:14Z drswinghead $
// 
#ifndef _SIPACCOUNTSWINDOW_H_
#define _SIPACCOUNTSWINDOW_H_

#include <QtCore>
#include <QtGui>

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
