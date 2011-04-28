// skypephone.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-18 21:23:19 +0800
// Version: $Id: skypephone.h 854 2011-04-26 15:08:58Z drswinghead $
// 

#ifndef _SKYPEPHONE_H_
#define _SKYPEPHONE_H_

#include <QtCore>
#include <QtNetwork>
#include <QtGui>
#include <QtSql>

#include "boost/shared_ptr.hpp"

class Skype;
class SkypeTunnel;
class SkypeTracer;

class WebSocketClient;
class AsyncDatabase;
class SqlRequest;

namespace Ui {
    class SkypePhone;
};

class SkypePhone : public QWidget
{
    Q_OBJECT;
public:
    explicit SkypePhone(QWidget *parent = 0);
    virtual ~SkypePhone();

    void init_status_bar(QStatusBar *bar);                         

public slots:    // pstn
    void defaultPstnInit();
    void onInitPstnClient();
    void onCallPstn();
    void onHangupPstn();
    void onShowSkypeTracer();

    void onShowDialPanel();
    void onShowLogPanel();
    void onAddContact();
    void onAddGroup();

    void onConnectSkype();
    void onConnectApp2App();

    void onSkypeConnected(QString user_name);
    void onSkypeRealConnected(QString user_name);

    void onSkypeUserStatus(QString str_status, int int_status);
    void onSkypeCallArrived(QString callerName, QString calleeName, int callID);

    void onWSConnected(QString path);
    void onWSError(int error, const QString &errmsg);
    void onWSDisconnected();
    void onWSMessage(QByteArray msg);

    void onCalcWSServByNetworkType(QHostInfo hi);
    void onNoticeUserStartup();

    void onDatabaseConnected();
    // database exec callbacks
    void onSqlExecuteDone(const QList<QSqlRecord> & results, int reqno, bool eret, 
                  const QString &estr, const QVariant &eval);
    bool onAddContactDone(boost::shared_ptr<SqlRequest> req);
    bool onAddGroupDone(boost::shared_ptr<SqlRequest> req);
    bool onAddCallHistoryDone(boost::shared_ptr<SqlRequest> req);
    bool onGetAllContactsDone(boost::shared_ptr<SqlRequest> req);
    bool onGetAllGroupsDone(boost::shared_ptr<SqlRequest> req);
    bool onGetAllHistoryDone(boost::shared_ptr<SqlRequest> req);

    void log_output(int type, const QString &log);

public:
    enum LOGTYPE {
        LT_USER = 0,
        LT_DEBUG = 1
    };

protected:
    bool first_paint_event;
    virtual void 	paintEvent ( QPaintEvent * event );
    virtual void 	showEvent ( QShowEvent * event );

    bool eventFilter(QObject *obj, QEvent *evt);

private:
    void customAddContactButtonMenu();

private: // pstn
    Skype *mSkype;
    SkypeTunnel *mtun;
    SkypeTracer *mSkypeTracer;

    QStatusBar *m_status_bar;
    int m_dialpanel_layout_index;
    QLayoutItem *m_dialpanel_layout_item;
    int m_call_state_layout_index;
    QLayoutItem *m_call_state_layout_item;
    int m_log_list_layout_index;
    QLayoutItem *m_log_list_layout_item;

    AsyncDatabase *m_adb;
    int m_curr_skype_call_id;
    QString m_curr_skype_call_peer;
    boost::shared_ptr<WebSocketClient> wscli;
    
    QAtomicInt m_call_button_disable_count; // 由于默认的按钮初始化状态是disabled
    static const int m_conn_ws_max_retry_times = 3;    
    int m_conn_ws_retry_times;
    QString m_ws_serv_ipaddr;

    //// sql reqno <---> sql reqclass
    QHash<int, boost::shared_ptr<SqlRequest> > mRequests;

private:
    Ui::SkypePhone *uiw;
};

#endif /* _SKYPEPHONE_H_ */
