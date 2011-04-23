// tvosky.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-01 10:08:49 +0800
// Version: $Id: tvosky.h 780 2011-03-20 15:08:48Z drswinghead $
// 


#ifndef _TVOSKY_H_
#define _TVOSKY_H_

#include <QtCore>
// #include <QtNetwork> // can not include like this, conflict with skp-kit's X509 impl
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtGui>

#include "main.h"

namespace Ui {
    class TvoSky;
};

class PReference;
class Conversation;

class TvoSky : public QMainWindow
{
    Q_OBJECT;
public:
    explicit TvoSky(QWidget *parent = 0);
    virtual ~TvoSky();

public slots:
    // user send
    void onLoginSkype();
    bool startSkypekitProcess();
    bool stopSkypekitProcess();
    bool appendLog(QString log);

    // program send
    void onLoginStateChanged(int prop);
    void onLoginingAnimate();  // 
    void onConversationListChanged(PReference *pconversation, int type, bool added);
    void onContactStateChanged(int prop);

    void onSkypekitBackendStarted();
    void onSkypekitBackendFinished( int exitCode, QProcess::ExitStatus exitStatus);
    void onSkypekitBackendError(QProcess::ProcessError error);
    void onSkypekitBackendReadyReadStandardError();
    void onSkypekitBackendReadyReadStandardOutput();
    void onSkypekitBackendStateChanged( QProcess::ProcessState newState);

    void onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    // other
    bool onCallSkype(QString target);
    void onCallPhone();

    void onPresendFinished();
    void onPresendReadyRead();

private:
    void adjustStartupUi();
    void adjustLoggedUi();

private:
    Ui::TvoSky *uiw;

    QProcess *skproc;
    Q_PID skpid;
    QTimer *loginAnimateTimer;
    int loginAnimateCycle;

    QNetworkAccessManager *mNetAM;
    QHash<QNetworkReply*, QByteArray> mPresendResult;
};

class AsyncSkyRunner: public QThread
{
    Q_OBJECT;
public:
    explicit AsyncSkyRunner(QObject *parent = 0);
    virtual ~AsyncSkyRunner();

    void run();

public:
    enum {AR_LOGIN};
    TvoSky *skywin;
    QString login_name;
    QString login_password;
    int cmdno;
};

#endif /* _TVOSKY_H_ */
