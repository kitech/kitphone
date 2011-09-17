// mosipphone.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-08-10 15:08:21 +0000
// Version: $Id: mosipphone.h 999 2011-09-17 14:25:17Z drswinghead $
// 
#ifndef _MOSIPPHONE_H_
#define _MOSIPPHONE_H_

#include <QtCore>
#include <QtNetwork>
#include <QtGui>
#include <QtSql>

#include "boost/signals2.hpp"
#include "boost/smart_ptr.hpp"

#include "intermessage.h"

class SipAccount;
class AsyncDatabase;
class ContactModel;
class CallHistoryModel;
class SqlRequest;
class SipEngine;
class SkycitEngine;
class BoostBridge;
class WebSocketClient2;
class Preferences;

namespace Ui {
    class MosipPhone;
};

// 还要让它处理异步来自主界面线程的sip调用
// class PJSipEventThread : public QThread
// {
//     Q_OBJECT;
// public: 
//     explicit PJSipEventThread(QObject *parent = 0);
//     virtual ~PJSipEventThread();
    
//     void run();

//     void dump_info(pj_thread_t *thread);

// private:
//     bool quit_loop;
// };

class MosipPhone : public QWidget
{
    Q_OBJECT;
public:
    explicit MosipPhone(QMainWindow *win, QWidget *parent = 0);
    virtual ~MosipPhone();

public slots:
    // ui
    void main_ui_draw_complete();

    void init_client_ui_element();
    void onManageSipAccounts();
    void onShowPreferences();
    void onRegisterAccount(QString display_name, QString user_name, const QString serv_addr, bool reg);
    void onRemoveAccount(QString user_name, const QString serv_addr);

    bool setEngineEnable(int engine_index, bool enable);
    // void onCallHangupButtonClicked();
    void onSwitchCallStatePannel(bool calling);
    void onCallSipNew();
    void onHangupSipNew();
    void onSipAnswered();
    void onSipDisconnected();

    void onRegisterState(const CmdRegState &cmd_rs);
    void onCallState(const CmdCallState &cmd_cs);
    void onCallMediaState(const CmdCallMediaState &cmd_cms);
    void onCodecList(const CmdCodecList &cmd_cl);

    void onSelectedUserAccountChanged(int idx);
    void onDigitButtonClicked();
    
    void onon_mosip_engine_error(int eno);
    void onon_mosip_engine_started();

    void onon_ws_client_connected(QString rpath);
    void onon_ws_client_disconnected();
    void onon_ws_client_error();
    void onon_ws_client_message(QByteArray msg);


    void onDatabaseConnected();
    // database exec callbacks
    void onSqlExecuteDone(const QList<QSqlRecord> & results, int reqno, bool eret, 
                  const QString &estr, const QVariant &eval);

    void onAddContact();
    void onModifyContact();
    void onAddGroup();

    void onShowDialPanel();
    void onShowLogPanel();
    void onDynamicSetVisible(QWidget *w, bool visible);
    void onDynamicSetVisible();

    void customAddContactButtonMenu();
    void initContactViewContextMenu();
    void onShowContactViewMenu(const QPoint &pos);

    bool onAddContactDone(boost::shared_ptr<SqlRequest> req);
    bool onModifyContactDone(boost::shared_ptr<SqlRequest> req);
    bool onAddGroupDone(boost::shared_ptr<SqlRequest> req);
    bool onAddCallHistoryDone(boost::shared_ptr<SqlRequest> req);

    bool onGetAllContactsDone(boost::shared_ptr<SqlRequest> req);
    bool onGetAllGroupsDone(boost::shared_ptr<SqlRequest> req);
    bool onGetAllHistoryDone(boost::shared_ptr<SqlRequest> req);
    bool onGetAllAccountsDone(boost::shared_ptr<SqlRequest> req);

    void onAccountAdded(SipAccount &acc);
    void onAccountRemoved(SipAccount &acc);

    // impl 
    bool savePreferences(Preferences *prefs);
    bool loadPreferences(Preferences *prefs);
    bool onSavePreferencesDone(boost::shared_ptr<SqlRequest> req);
    bool onLoadPreferencesDone(boost::shared_ptr<SqlRequest> req);

    void log_output(int type, const QString &log);

public:
    enum LOGTYPE {
        LT_USER = 0,
        LT_DEBUG = 1
    };

private:
    // pjsua_acc_id _create_sip_account(QString user_name, QString serv_adder);
    // QString _get_sip_from_domain();
    // int _find_account_from_pjacc(QString user_name, const QString serv_addr);
    QString _reformat_call_phone_number(const QString &user_phone_number);
    QString _get_input_phone_number();
    bool _update_current_account_status(int acc_id);

    bool process_ctrl_message(const std::string &msg);

protected:
    bool first_paint_event;
    virtual void 	paintEvent ( QPaintEvent * event );
    virtual void 	showEvent ( QShowEvent * event );

signals:
    void sig_test_q2b_rt();

private:

    ///////////////////////
    QMainWindow *main_win;
    QWidget *mdyn_widget;
    bool mdyn_visible;
    QGraphicsOpacityEffect *mdyn_oe;

    // QStatusBar *m_status_bar;
    int m_dialpanel_layout_index;
    QLayoutItem *m_dialpanel_layout_item;
    int m_call_state_layout_index;
    QLayoutItem *m_call_state_layout_item;
    int m_log_list_layout_index;
    QLayoutItem *m_log_list_layout_item;

    QMenu *m_contact_view_ctx_menu;

    // AsyncDatabase *m_adb;
    boost::shared_ptr<AsyncDatabase> m_adb;
    //// sql reqno <---> sql reqclass
    QHash<int, boost::shared_ptr<SqlRequest> > mRequests;

    ContactModel *m_contact_model;

    CallHistoryModel *m_call_history_model;

    int m_acc_id;
    int m_call_id;
    std::vector<std::string> sip_codecs;

    SipEngine *mse;
    SkycitEngine *msce;
    BoostBridge *mbb;

    WebSocketClient2 *wsc2;
    QVector<std::string> cmds_queue_before_connected;
private:
    Ui::MosipPhone *uiw;
};



#endif /* _MOSIPPHONE_H_ */
