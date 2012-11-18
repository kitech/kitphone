// kitphone.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:46:16 +0800
// Version: $Id: kitphone.h 995 2011-09-16 09:51:17Z drswinghead $
// 
#ifndef _KITPHONE_H_
#define _KITPHONE_H_

#include <QtCore>
#include <QtGui>
#include <QtNetwork>
#include <QMainWindow>

class QStatusBar;
class SkypePhone;
class MosipPhone;

namespace Ui {
    class KitPhone;
}

class KitPhone : public QMainWindow
{
    Q_OBJECT;
public:
    explicit KitPhone(QWidget *parent = 0);
    virtual ~KitPhone();

public slots:
    void onQuitApp();
    void onShowAbout();
    void onSwitchPhoneMode();

protected:
    virtual void 	paintEvent ( QPaintEvent * event );
    virtual void 	showEvent ( QShowEvent * event );
    
private:
    void custom_status_bar();

private:
    Ui::KitPhone *uiw;

    SkypePhone *uiw_skype;
    MosipPhone *uiw_sip;

    QStatusBar *esb;
    QToolButton *stb; // status toolbutton
    QToolButton *mtb; // mode switch button
    QToolButton *htb; // help toolbutton
};

#endif /* _KITPHONE_H_ */
