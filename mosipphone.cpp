// mosipphone.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-08-10 15:11:22 +0000
// Version: $Id: mosipphone.cpp 999 2011-09-17 14:25:17Z drswinghead $
// 

// #ifdef WIN32
// #include <winsock2.h>
// #else
// #include <arpa/inet.h>
// #endif

#include <QtCore>
#include <QtNetwork>

#include <boost/signals2.hpp>

#include "pjsip/sip_msg.h"

#include "boostbridge.h"
#include "sipengine.h"
#include "skycitengine.h"
#include "websocketclient2.h"
#include "security.h"

#include "simplelog.h"
#include "ui_mosipphone.h"
#include "mosipphone.h"

#include "sipaccount.h"
#include "sipaccountswindow.h"
#include "preferencesdialog.h"

#include "preferences.h"
#include "global.h"
#include "asyncdatabase.h"
#include "phonecontact.h"
#include "phonecontactproperty.h"
#include "groupinfodialog.h"
#include "contactmodel.h"
#include "callhistorymodel.h"
#include "intermessage.h"


MosipPhone::MosipPhone(QMainWindow *win, QWidget *parent)
    : QWidget(parent)
    ,main_win(win)
    ,uiw(new Ui::MosipPhone())
{
    this->uiw->setupUi(this);

    this->first_paint_event = true;

    QObject::connect(this->uiw->pushButton, SIGNAL(clicked()), 
                     this, SLOT(onManageSipAccounts()));
    QObject::connect(this->uiw->pushButton_2, SIGNAL(clicked()), 
                     this, SLOT(onShowPreferences()));

    this->m_contact_model = NULL;
    this->m_call_history_model = NULL;

    this->m_acc_id = PJSUA_INVALID_ID;
    this->m_call_id = PJSUA_INVALID_ID;
    this->wsc2 = NULL;

    this->init_client_ui_element();

    /////////////
    // QGraphicsOpacityEffect *oe = new QGraphicsOpacityEffect();
    // oe->setOpacity(0.3);
    // this->uiw->toolButton_5->setWindowOpacity(0.5);
    // this->uiw->toolButton_5->setGraphicsEffect(oe);
}

MosipPhone::~MosipPhone()
{
    delete uiw;
}

void MosipPhone::main_ui_draw_complete()
{
    qLogx()<<"draw complete, load dynamic data now";

    // prefs and translate
    Global::global_init();

    /// database
    this->m_adb = boost::shared_ptr<AsyncDatabase>(new AsyncDatabase());
    QObject::connect(this->m_adb.get(), SIGNAL(connected()), this, SLOT(onDatabaseConnected()));
    this->m_adb->start();
    // QTimer::singleShot(50, this->m_adb.get(), SLOT(start()));
    
    QObject::connect(this->m_adb.get(), SIGNAL(results(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)),
                     this, SLOT(onSqlExecuteDone(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)));


    this->m_contact_model = new ContactModel(this->m_adb);
    this->m_call_history_model = new CallHistoryModel(this->m_adb);

    // 需要延迟到会话参数加载完成决定初始化哪一个引擎，或者有没有办法让这些引擎都走动变不冲突呢？
    this->mbb = new BoostBridge();
    this->mse = NULL;
    // this->mse = SipEngine::instance();
    // this->mse->sip_engine_error.connect(boost::bind(&BoostBridge::on_sip_engine_error, this->mbb, _1));
    // this->mse->sip_engine_started.connect(boost::bind(&BoostBridge::on_sip_engine_started, this->mbb));
    // QObject::connect(this->mbb, SIGNAL(sip_engine_error(int)), 
    //                  this, SLOT(onon_mosip_engine_error(int)));
    // QObject::connect(this->mbb, SIGNAL(sip_engine_started()),
    //                  this, SLOT(onon_mosip_engine_started()));

    // // test
    // QObject::connect(this, SIGNAL(sig_test_q2b_rt()), this->mbb, SLOT(onon_test_q2b_signal_run_thread()));
    // this->mbb->my_test_q2b_rt.connect(boost::bind(&SipEngine::onon_test_q2b_rt, this->mse));

    // this->mse->start();

    // emit this->sig_test_q2b_rt();

    // another engine
    this->msce = SkycitEngine::instance();
    this->msce->start();

    this->mdyn_oe = new QGraphicsOpacityEffect();
    QObject::connect(this->uiw->toolButton_4, SIGNAL(clicked()),
                     this, SLOT(onShowDialPanel()));
    QObject::connect(this->uiw->toolButton, SIGNAL(clicked()),
                     this, SLOT(onShowLogPanel()));


    // QString user_account;
    // QVector<SipAccount> accs;//  = this->acc_list->loadAccounts(); TODO, replace by
    // for (int i = accs.count()-1; i >= 0; i--) {
    //     user_account = QString("%1@%2").arg(accs.at(i).userName).arg(accs.at(i).domain);
    //     this->uiw->comboBox_6->insertItem(0, user_account);
    // }
    // this->uiw->comboBox_6->setCurrentIndex(0);

    // this->uiw->comboBox_6->view()->setFixedWidth(280);
}

void MosipPhone::paintEvent ( QPaintEvent * event )
{
    QWidget::paintEvent(event);
    // qDebug()<<"parintttttt"<<event<<event->type();
    if (this->first_paint_event) {
        this->first_paint_event = false;
		QTimer::singleShot(50, this, SLOT(main_ui_draw_complete()));
    }
}
void MosipPhone::showEvent ( QShowEvent * event )
{
    QWidget::showEvent(event);
    // qDebug()<<"showwwwwwwwwwwww"<<event<<event->type();
}

void MosipPhone::onShowDialPanel()
{
    // this->onDynamicSetVisible(this->m_dialpanel_layout_item->widget(),
    //                           !this->m_dialpanel_layout_item->widget()->isVisible());    
}

void MosipPhone::onShowLogPanel()
{
    return;
    // this->onDynamicSetVisible(this->m_log_list_layout_item->widget(),
    //                           !this->m_log_list_layout_item->widget()->isVisible());

    // this->onDynamicSetVisible(this->uiw->widget,
    //                           !this->uiw->widget->isVisible());
    // QSize fsize, tsize, wsize;
    // if (this->uiw->widget->isVisible()) {
    //     fsize = this->main_win->size();
    //     wsize = this->uiw->widget->size();
    //     tsize = fsize;
    //     tsize.setWidth(fsize.width() - wsize.width() - 150);
    //     qLogx()<<fsize<<wsize<<tsize;
    // }
    // this->main_win->resize(tsize);
    // this->uiw->widget->setVisible(!this->uiw->widget->isVisible());

    // this->m_log_list_layout_item->widget()->setVisible(!this->m_log_list_layout_item->widget()->isVisible());
    // this->uiw->toolButton->setArrowType(this->uiw->widget->isVisible()
    //                                     ? Qt::LeftArrow : Qt::RightArrow);
}

void MosipPhone::onDynamicSetVisible(QWidget *w, bool visible)
{
    Q_ASSERT(w != NULL);

    this->mdyn_widget = w;
    this->mdyn_visible = visible;

    this->onDynamicSetVisible();
    // w->setWindowOpacity(0.5);

}

void MosipPhone::onDynamicSetVisible()
{
    int curr_opacity = 100;
    char *pname = "dyn_opacity";
    QVariant dyn_opacity = this->mdyn_widget->property(pname);

    if (dyn_opacity.isValid()) {
        curr_opacity = dyn_opacity.toInt();
        if (this->mdyn_visible == true) {
            curr_opacity += 10;
            this->mdyn_widget->setProperty(pname, QVariant(curr_opacity));
            if (curr_opacity == 100) {
                this->mdyn_oe->setOpacity(1.0);
                this->mdyn_widget->setGraphicsEffect(this->mdyn_oe);
                this->mdyn_widget->setVisible(true);
            } else {
                // this->mdyn_widget->setWindowOpacity(curr_opacity*1.0/100);
                this->mdyn_oe->setOpacity(curr_opacity*1.0/100);
                this->mdyn_widget->setGraphicsEffect(this->mdyn_oe);
                if (curr_opacity == 10) {
                    this->mdyn_widget->setVisible(true);
                }
                QTimer::singleShot(50, this, SLOT(onDynamicSetVisible()));
            }
        } else {
            curr_opacity -= 10;
            this->mdyn_widget->setProperty(pname, QVariant(curr_opacity));
            if (curr_opacity == 0) {
                this->mdyn_widget->setVisible(false);
            } else {
                // this->mdyn_widget->setWindowOpacity(curr_opacity*1.0/100);
                this->mdyn_oe->setOpacity(curr_opacity*1.0/100);
                this->mdyn_widget->setGraphicsEffect(this->mdyn_oe);
                QTimer::singleShot(50, this, SLOT(onDynamicSetVisible()));
            }
        }
    } else {
        if (this->mdyn_visible == true) {
            curr_opacity = 0;
            this->mdyn_widget->setProperty(pname, QVariant(curr_opacity));
        } else {
            curr_opacity = 100;
            this->mdyn_widget->setProperty(pname, QVariant(curr_opacity));
        }
        QTimer::singleShot(1, this, SLOT(onDynamicSetVisible()));
    }
    
}


void MosipPhone::init_client_ui_element()
{
    QObject::connect(this->uiw->pushButton_5, SIGNAL(clicked()), this, SLOT(onCallSipNew()));
    // QObject::connect(this->uiw->pushButton_5, SIGNAL(clicked()), this, SLOT(onCallHangupButtonClicked()));
    QObject::connect(this->uiw->pushButton_8, SIGNAL(clicked()), this, SLOT(onHangupSipNew()));

    QObject::connect(this->uiw->comboBox_4, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(onSelectedUserAccountChanged(int)));

    QObject::connect(this->uiw->toolButton_14, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_15, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_16, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_17, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_18, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_19, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_20, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_21, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_22, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_23, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_24, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));
    QObject::connect(this->uiw->toolButton_25, SIGNAL(clicked()), this, SLOT(onDigitButtonClicked()));

}


void MosipPhone::onManageSipAccounts()
{
    SipAccountsWindow *acc_win = new SipAccountsWindow(this->m_adb, this);
    QObject::connect(acc_win, SIGNAL(accountWantRegister(QString, QString, QString, bool)),
                     this, SLOT(onRegisterAccount(QString, QString, QString, bool)));
    // QObject::connect(acc_win, SIGNAL(accountWantRemove(QString)),
    //                  this, SLOT(onRemoveAccount(QString)));
    QObject::connect(acc_win, SIGNAL(accountAdded(SipAccount&)),
                     this, SLOT(onAccountAdded(SipAccount &)));
    QObject::connect(acc_win, SIGNAL(accountRemoved(SipAccount&)),
                     this, SLOT(onAccountRemoved(SipAccount &)));
    if (acc_win->exec() == QDialog::Accepted) {

    }
    delete acc_win;
}

void MosipPhone::onShowPreferences()
{
    bool should_save = false;
    PreferencesDialog *dlg = new PreferencesDialog(this);
    dlg->setData(Global::pref);
    if (dlg->exec() == QDialog::Accepted) {
        should_save = true;
        dlg->getData(Global::pref);
    }
    delete dlg;

    if (should_save) {
        this->savePreferences(Global::pref);
    }
}

bool MosipPhone::onPostLoadPreferencesDone()
{
    return true;
}

/*
 */
void MosipPhone::onRegisterAccount(QString display_name, QString user_name, const QString serv_addr, bool reg)
{
    bool bret = false;
    // SipAccount sip_acc;
    // QString acc_uri;
    QList<QSqlRecord> recs;
    QString sql;

    sql = QString("SELECT * FROM kp_accounts WHERE account_name='%1' AND serv_addr='%2' AND display_name='%3'")
        .arg(user_name).arg(serv_addr).arg(display_name);
    this->m_adb->syncExecute(sql, recs);
    qLogx()<<recs<<sql;
    
    CmdRegister cmd;
    cmd.display_name = display_name.toStdString();
    cmd.user_name = user_name.toStdString();
    cmd.password = recs.at(0).value("account_password").toString().toStdString();
    cmd.sip_server = serv_addr.toStdString();
    cmd.unregister = !reg;

    std::string jcstr = InterMessage().jpack_message(cmd);

    if (this->wsc2 != NULL) {
        bret = this->wsc2->sendMessage(jcstr);
        qLogx()<<bret;
    } else {
        qLogx()<<"wsc not connected now.enqueue request.";
        this->cmds_queue_before_connected.append(jcstr);
    }
}

void MosipPhone::onRemoveAccount(QString user_name, const QString serv_addr)
{
    // pjsua_acc_id acc_id;
    // pj_status_t status;

    // acc_id = this->_find_account_from_pjacc(user_name, serv_addr);
    // if (acc_id != -1) {
    //     status = pjsua_acc_set_registration(acc_id, 0);
    //     status = pjsua_acc_del(acc_id);
    // } else {
    //     qLogx()<<"Can not find pjsip account:"<<user_name<<serv_addr;
    // }
}

bool MosipPhone::setEngineEnable(int engine_index, bool enable)
{

    return true;
}

// engine
void MosipPhone::onCallState(const CmdCallState &cmd_cs)
{
    qLogx()<<"";

    this->uiw->label_4->setText(QString::fromStdString(cmd_cs.state_text));
    if (cmd_cs.state == PJSIP_INV_STATE_CONFIRMED) {
        this->uiw->pushButton_8->setEnabled(true);
    } else if (cmd_cs.state == PJSIP_INV_STATE_DISCONNECTED) {
        this->m_call_id = PJSUA_INVALID_ID;

        // QPushButton *btn = this->uiw->pushButton_5;
        // btn->setText(tr("Call ..."));
        // btn->setEnabled(true);
        this->onSwitchCallStatePannel(false);
    }
}

// void MosipPhone::on1_call_state(pjsua_call_id call_id, pjsip_event *e, pjsua_call_info *pci)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
//     // pjsua_call_info ci;
 
//     PJ_UNUSED_ARG(e);
 
//     // pjsua_call_get_info(call_id, &ci);
//     PJ_LOG(3, (__FILE__, "Call %d state=%.*s", call_id,
//               (int)pci->state_text.slen,
//                PJSTR2A(pci->state_text)));

//     if (pci->state == PJSIP_INV_STATE_CONFIRMED) {
//         log_output(LT_USER, QString("呼叫已经建立：%1").arg(this->m_curr_call_id));
//     }

//     if (pci->state == PJSIP_INV_STATE_DISCONNECTED) {
//         log_output(LT_USER, QString("呼叫结束：%1").arg(this->m_curr_call_id));
//         this->m_curr_call_id = PJSUA_INVALID_ID;

//         QPushButton *btn = this->uiw->pushButton_5;
//         btn->setText(tr("Call ..."));
//         btn->setEnabled(true);
//     }

//     free(pci);
// }


void MosipPhone::onCallMediaState(const CmdCallMediaState &cmd_cms)
{
    qLogx()<<"";
}


// void MosipPhone::on1_call_media_state(pjsua_call_id call_id, pjsua_call_info *pci)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
//     // pjsua_call_info ci;
//     pj_status_t status;
 
//     // pjsua_call_get_info(call_id, &ci);
 
//     if (pci->media_status == PJSUA_CALL_MEDIA_ACTIVE) {
//         // When media is active, connect call to sound device.
//         pjsua_conf_connect(pci->conf_slot, 0);
//         pjsua_conf_connect(0, pci->conf_slot);

//         // test port_info
//         pjsua_conf_port_info cpi;
//         pjsua_conf_get_port_info(pci->conf_slot, &cpi);
//         qLogx()<<"conf port info: port number="<<cpi.slot_id<<",name='"<<PJSTR2Q(cpi.name)
//                 <<"', chan cnt="<<cpi.channel_count
//                 <<", clock rate="<<cpi.clock_rate;

//         // pjsua_conf_connect(ci.conf_slot, pjsua_recorder_get_conf_port(g_rec_id));

//         pjmedia_transport *mtp = pjsua_call_get_media_transport(call_id);
//         pjmedia_session *ms = pjsua_call_get_media_session(call_id);
//         pjmedia_port *mp = NULL;
//         unsigned int stream_count = 100;
//         pjmedia_stream_info msi[100];
//         pjmedia_frame from_lang_frame;
//         int i = 0;
//         unsigned short n_port = 0;

//         // pjsua_switcher_get_port(g_rec_id, &mp);
//         // n_port = pjsua_switcher_get_net_port(mp);
//         qLogx()<<"got wav server port :"<<n_port;
//         // wc->setPort(n_port);
//         // wc->arun();
        
//         if (ms == NULL) {
//             qLogx()<<"pjmedia_session is null.\n";
//         } else {
//             status = pjmedia_session_enum_streams(ms, &stream_count, msi);
//             qLogx()<<"enum stream count:"<<stream_count;
            
//             status = pjmedia_session_get_port(ms, 0, &mp);
//             Q_ASSERT(status == PJ_SUCCESS);

//             QFile wfp("/tmp/arec.wav");
//             if (!wfp.open(QIODevice::ReadWrite | QIODevice::Unbuffered)) {
//                 exit(-1);
//             }
//             // why kitphone: ../src/pjmedia/plc_common.c：106：pjmedia_plc_generate: 断言“plc && frame”失败。
//             // while ((status = pjmedia_port_get_frame(mp, &from_lang_frame)) == PJ_SUCCESS
//             //        && i ++ < 10000) {
//             //     if (from_lang_frame.size > 0) {
//             //         wfp.write((char*)(from_lang_frame.buf), from_lang_frame.size);
//             //     } else {
//             //         qDebug()<<"got 0 frame. stop.";
//             //         break;
//             //     }
//             // }
//             wfp.close();
            
//         }
//     }

//     // pjsua_dump(PJ_TRUE);
//     // pjsua_call_dump(call_id, ...);

//     free(pci);
// }

// void MosipPhone::on1_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
//     pj_status_t status;
//     pjsua_call_info ci;
 
//     PJ_UNUSED_ARG(acc_id);
//     PJ_UNUSED_ARG(rdata);
 
//     pjsua_call_get_info(call_id, &ci);
 
//     PJ_LOG(3,(__FILE__, "Incoming call from %.*s!!",
//               (int)ci.remote_info.slen,
//               PJSTR2A(ci.remote_info)));
 
//     /* Automatically answer incoming calls with 200/OK */
//     status = pjsua_call_answer(call_id, 200, NULL, NULL);

// }

void MosipPhone::onRegisterState(const CmdRegState &cmd_rs)
{
    qLogx()<<"";
    QString sb_msg;

    if (cmd_rs.status == PJSIP_SC_OK) {
        sb_msg = QString(tr("Register OK %1")).arg(cmd_rs.acc_uri.c_str());
    } else {
        sb_msg = QString(tr("Register ERR %1")).arg(cmd_rs.acc_uri.c_str());
    }

    this->m_acc_id = cmd_rs.acc_id;

    this->main_win->statusBar()->showMessage(sb_msg);
}

void MosipPhone::onCodecList(const CmdCodecList &cmd_cl)
{
    this->sip_codecs = cmd_cl.codecs;

    for (int i = 0; i < this->sip_codecs.size(); i ++) {
        QString codec_idname = QString::fromStdString(this->sip_codecs.at(i));
        this->uiw->comboBox->insertItem(this->uiw->comboBox->count(), codec_idname);
    }
    if (Global::pref != NULL) {
        Global::pref->setCodecs(this->sip_codecs);
    } else {
        Global::global_init();
        Global::pref->setCodecs(this->sip_codecs);
    }
}

// void MosipPhone::onCallHangupButtonClicked()
// {
//     QPushButton *btn = static_cast<QPushButton*>(sender());
//     if (this->m_call_id == PJSUA_INVALID_ID) {
//         // should new call
//         btn->setEnabled(false);
//         btn->setIcon(QIcon(":/skins/default/phone_32.png"));
//         btn->setText(tr("Calling"));
//         this->onCallSipNew();
//     } else {
//         // should hangup call
//         btn->setEnabled(false);
//         btn->setIcon(QIcon(":/skins/default/hangup_20.png"));
//         btn->setText(tr("Hanguping"));
//         this->onHangupSipNew();
//     }
// }

void MosipPhone::onSwitchCallStatePannel(bool calling)
{
    if (calling) {
        this->uiw->pushButton_8->setEnabled(false);
        this->uiw->stackedWidget->setCurrentIndex(1);
    } else {
        this->uiw->stackedWidget->setCurrentIndex(0);
    }
}

//////
void MosipPhone::onCallSipNew()
{
    pjsua_acc_id acc_id;
    pj_status_t status;

    // check call state
    if (this->m_call_id != PJSUA_INVALID_ID) {
        this->log_output(LT_USER, QString(tr("正在呼叫中：%1，请不要重复呼叫。")).arg(this->m_call_id));
        Q_ASSERT(this->m_call_id == PJSUA_INVALID_ID);
        return;
    }

    this->onSwitchCallStatePannel(true);

    //////
    // set codec
    // pjsua_codec_info cids[100];
    // unsigned int cid_cnt = 100;
    // QString selected_codec;
    // char tbuf[200];
    // pjsua_transport_info tpi;

    // selected_codec = this->uiw->comboBox->currentText();
    // status = pjsua_enum_codecs(cids, &cid_cnt);
    // for (int i = 0; i < cid_cnt; i++) {
    //     if (PJSTR2Q(cids[i].codec_id) == selected_codec) {
    //         status = pjsua_codec_set_priority(&cids[i].codec_id, 200);
    //         qLogx()<<"Using codec for this call,"<<selected_codec;
    //     } else {
    //         status = pjsua_codec_set_priority(&cids[i].codec_id, 0);
    //     }
    // }

    // switch (this->uiw->comboBox_2->currentIndex()) {
    // case 0:
    //     // UDP mode
    //     status = pjsua_transport_set_enable(this->m_tcp_tp_id, PJ_FALSE);
    //     status = pjsua_transport_set_enable(this->m_udp_tp_id, PJ_TRUE);
    //     status = pjsua_transport_get_info(this->m_udp_tp_id, &tpi);
    //     qLogx()<<"Transport real host:port,"<<PJSTR2Q(tpi.local_name.host)<<tpi.local_name.port;
    //     break;
    // case 1:
    //     // TCP mode
    //     status = pjsua_transport_set_enable(this->m_tcp_tp_id, PJ_TRUE);
    //     status = pjsua_transport_set_enable(this->m_udp_tp_id, PJ_FALSE);
    //     status = pjsua_transport_get_info(this->m_tcp_tp_id, &tpi);
    //     qLogx()<<"Transport real host:port,"<<PJSTR2Q(tpi.local_name.host)<<tpi.local_name.port;
    //     break;
    // default:
    //     Q_ASSERT(1==2);
    //     break;
    // }
    // qLogx()<<"Using transport: "<< this->uiw->comboBox_2->currentText();

    SipAccount acc;
    QList<QSqlRecord> recs;
    QString display_name = this->uiw->comboBox_4->currentText();
    QString callee_phone = this->uiw->comboBox_7->currentText();
    QString sip_server;// 

    // // sip_server = "202.108.29.234:4060";
    // // sip_server = "202.108.29.229:4060";
    // // sip_server = user_account.split("@").at(1);

    QString sql = QString("SELECT * FROM kp_accounts WHERE display_name='%1' LIMIT 1").arg(display_name);
    this->m_adb->syncExecute(sql, recs);
    qLogx()<<recs<<sql;
    
    acc = SipAccount::fromSqlRecord(recs.at(0));

    sip_server = acc.domain;

    // //////////
    // acc_id = this->_find_account_from_pjacc(acc.userName, acc.domain);
    // if (acc_id == PJSUA_INVALID_ID) {
    //     acc_id = this->_create_sip_account(acc.userName, acc.domain);
    // }
    if (acc_id == PJSUA_INVALID_ID) {
        // this->uiw->pushButton_5->setText(tr("Call ..."));
        // this->uiw->pushButton_5->setEnabled(true);
        this->onSwitchCallStatePannel(false);
        return;
    }

    bool bret = false;
    CmdMakeCall cmd_mc;
    cmd_mc.caller_name = acc.userName.toStdString();
    cmd_mc.callee_phone = callee_phone.toStdString();
    cmd_mc.sip_server = sip_server.toStdString();
    cmd_mc.caller_ipaddr = "Unknown caller ipaddd";
    cmd_mc.acc_id = this->m_acc_id;
    cmd_mc.call_id = this->m_call_id;

    if (Global::pref != NULL) {
        cmd_mc.use_tls = Global::pref->use_tls;
        cmd_mc.use_stun = Global::pref->use_stun;
        cmd_mc.use_turn = Global::pref->use_turn;
        cmd_mc.use_ice = Global::pref->use_ice;
        cmd_mc.stun_server = Global::pref->stun_server;
        cmd_mc.use_codec = Global::pref->use_codec;
    }

    std::string cmdstr = InterMessage().jpack_message(cmd_mc);

    if (this->wsc2 != NULL) {
        bret = this->wsc2->sendMessage(cmdstr);
        qLogx()<<bret;
    } else {
        qLogx()<<"wsc not connected now.";
        // this->cmds_queue_before_connected.append(jcstr);
    }

    // // char *sipu = "<SIP:99008668056013552776960@122.228.202.105:4060;transport=UDP>";
    // char *sipu = strdup(QString("<SIP:%1@%2;transport=UDP>")
    //                     .arg(callee_phone).arg(sip_server) .toAscii().data());
    // qLogx()<<"call peer: "<<sipu;
    // // char *sipu = "<SIP:99008668056013552776960@202.108.29.234:5060;transport=UDP>";
    // pj_str_t uri = pj_str(sipu);

    // // status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
    // // if (status != PJ_SUCCESS) {
    // //     if (status == 469996) {
    // //         this->log_output(LT_USER, "无法打开声音设备。");
    // //     } else {
    // //         pjsua_perror(__FILE__, "Error making call", status);
    // //         //error_exit("Error making call", status);
    // //     }
    // // }

    // int reqno = this->m_invoker->invoke_make_call(acc_id, QString(sipu));

    // free(sipu);

    qLogx()<<"oncall slot returned";

    //////// 改变界面
    // this->uiw->label_5->setText(callee_phone);
}

void MosipPhone::onHangupSipNew()
{
    Q_ASSERT(this->m_call_id != PJSUA_INVALID_ID);
    // QPushButton *btn = this->uiw->pushButton_5;
    // btn->setText(tr("Hanguping"));
    // btn->setEnabled(false);
    bool bret = false;

    CmdHangupCall cmd_hup;
    cmd_hup.reason_no = 123456;
    cmd_hup.reason_text = "User push hangup button";
    cmd_hup.call_id = this->m_call_id;

    std::string cmdstr = InterMessage().jpack_message(cmd_hup);

    if (this->wsc2 != NULL) {
        bret = this->wsc2->sendMessage(cmdstr);
        qLogx()<<bret;
    } else {
        qLogx()<<"wsc not connected now.";
        // this->cmds_queue_before_connected.append(jcstr);
    }   

    this->uiw->pushButton_8->setEnabled(false);
    // this->onSwitchCallStatePannel(false);
}

void MosipPhone::onSipAnswered()
{
    // log_output(LT_USER, QString("呼叫已经建立：%1").arg(this->m_curr_call_id));
}

void MosipPhone::onSipDisconnected()
{
    // log_output(LT_USER, QString("呼叫结束：%1").arg(this->m_curr_call_id));
    // this->m_curr_call_id = PJSUA_INVALID_ID;
}

// pjsua_acc_id MosipPhone::_create_sip_account(QString user_name, QString serv_addr)
// {
//     pj_status_t status;
//     pjsua_acc_id acc_id = -1;
//     char tbuf[200] = {0};

//     /* Register to SIP server by creating SIP account. */

//     SipAccount acc;
//     QList<QSqlRecord> recs;
//     pjsua_acc_config cfg;
//     // QString display_name = this->uiw->comboBox_4->currentText();
//     QString caller_from_domain = this->_get_sip_from_domain();

//     // if (this->uiw->comboBox_4->currentIndex() == -1) {
//     //      qLogx()<<"select a host please";
//     //      return acc_id;
//     // }

//     // QString sql = QString("SELECT * FROM kp_accounts WHERE display_name='%1' LIMIT 1").arg(display_name);
//     // this->m_adb->syncExecute(sql, recs);
//     // qLogx()<<recs<<sql;
    
//     // acc = SipAccount::fromSqlRecord(recs.at(0));
//     caller_from_domain = acc.domain;

//     pjsua_acc_config_default(&cfg);
//     // cfg.id = pj_str(SIP_USER "<sip:" SIP_USER "@" SIP_DOMAIN ">");
//     // cfg.id = pj_str(SIP_USER " <sip:" SIP_USER "@"  "192.168.15.53:5678>");
//     // cfg.reg_uri = pj_str("sip:" SIP_DOMAIN); // if no reg_uri, it will no auth register to server, and call ok
//     memset(tbuf, 0, sizeof(tbuf));
//     QString reg_uri_str = QString("%1 <sip:%1@%2>").arg(user_name)
//         .arg(caller_from_domain);
//     strncpy(tbuf, reg_uri_str.toAscii().data(), sizeof(tbuf)-1);
//     qLogx()<<reg_uri_str<<tbuf;
//     cfg.id = pj_str(tbuf);

//     // cfg.reg_timeout = 800000000;
//     // cfg.publish_enabled = PJ_FALSE;
//     // cfg.auth_pref.initial_auth = 0; // no use
//     // cfg.reg_retry_interval = 0;
//     cfg.cred_count = 1;
//     cfg.cred_info[0].realm = pj_str("*");
//     cfg.cred_info[0].scheme = pj_str("digest");
//     cfg.cred_info[0].username = pj_str(acc.userName.toAscii().data());
//     cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
//     cfg.cred_info[0].data = pj_str(SIP_PASSWD);

//     status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
//     if (status != PJ_SUCCESS) {
//         pjsua_perror(__FILE__, "Error adding account", status);
//         //   error_exit("Error adding account", status);
//     }

//     return acc_id;
// }

// TODO add stuned ip if avalible
// QString MosipPhone::_get_sip_from_domain()
// {
//     QString from_domain;

//     QString ipaddr;
//     QHostAddress addr;
//     QList<QHostAddress> addr_list;
//     QList<QString> addr_str_list;
    
//     addr_list = QNetworkInterface::allAddresses();
//     // qDebug()<<addr_list;
//     for (int i = 0 ; i < addr_list.count() ; i ++) {
//         addr_str_list.append(addr_list.at(i).toString());
//     }
//     // qSort(list.begin(), list.end(), qGreater<int>());
//     qSort(addr_str_list.begin(), addr_str_list.end(), qGreater<QString>());

//     if (addr_str_list.count() == 0) {
//         ipaddr = "0.0.0.0";
//     } else if (addr_str_list.count() == 1) {
//         // must be 127.0.0.1
//         ipaddr = addr_str_list.at(0);
//     } else {
//         for (int i = 0 ; i < addr_str_list.count(); i ++) {
//             // addr = addr_list.at(i);
//             ipaddr = addr_str_list.at(i);
//             if (ipaddr.indexOf(":") != -1) {
//                 // ipv6 addr
//                 ipaddr = QString();
//                 continue;
//             } else {
//                 if (ipaddr.startsWith("127.0")) {
//                     ipaddr = QString();
//                     continue;
//                 } else if (!ipaddr.startsWith("172.24.")
//                            &&!ipaddr.startsWith("192.168.")
//                            &&!ipaddr.startsWith("10.10.")) {
//                     // should a big ip addr
//                     qDebug()<<"break big ip";
//                     break;
//                 } else if (ipaddr.startsWith("172.24.")) {
//                     qDebug()<<"break 172.2";
//                     break;
//                 } else if (ipaddr.startsWith("10.10.")) {
//                     break;
//                 } else if (ipaddr.startsWith("192.168.")) {
//                     break;
//                 } else {
//                     // do not want go here
//                     Q_ASSERT(1 == 2);
//                     break;
//                 }
//             }
//             ipaddr = QString();
//         }
//     }

//     pjsua_transport_config cfg;
//     pjsua_transport_info tinfo;
//     if (this->uiw->comboBox_2->currentIndex() == 0) {
//         // UDP mode
//         pjsua_transport_get_info(this->m_udp_tp_id, &tinfo);
//     } else {
//         // TCP mode
//         pjsua_transport_get_info(this->m_tcp_tp_id, &tinfo);
//     }
//     qLogx()<<"Using transport: "<< this->uiw->comboBox_2->currentText();

//     unsigned short port = ntohs(tinfo.local_addr.ipv4.sin_port);
    
//     from_domain = QString("KP-%1-%2:%2").arg(ipaddr).arg(port);
//     // from_domain = QString("%1:%2").arg(ipaddr).arg(port);
//     return from_domain;
// }

void MosipPhone::onSelectedUserAccountChanged(int idx)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<idx;
    // qLogx()<<this->uiw->comboBox_6->currentText();

    // TODO
    // 得到该账号的注册状态，更新对应的状态图标
    // QString sip_uri;// = this->uiw->comboBox_6->currentText();
    SipAccount acc;
    QList<QSqlRecord> recs;
    QString display_name, user_name, serv_addr;
    display_name = this->uiw->comboBox_4->currentText();
    QString sql = QString("SELECT * FROM kp_accounts WHERE display_name='%1' LIMIT 1").arg(display_name);
    this->m_adb->syncExecute(sql, recs);
    qLogx()<<recs<<sql;
    
    acc = SipAccount::fromSqlRecord(recs.at(0));
    user_name = acc.userName;
    display_name = acc.displayName;
    serv_addr = acc.domain;

    this->onRegisterAccount(display_name, user_name, serv_addr, true);

}

void MosipPhone::onDigitButtonClicked()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<(sender());
    QToolButton *btn = static_cast<QToolButton*>(sender());

    QString digit;
    QHash<QToolButton*, char> bdmap;
    bdmap[this->uiw->toolButton_14] = '0';
    bdmap[this->uiw->toolButton_15] = '1';
    bdmap[this->uiw->toolButton_16] = '2';
    bdmap[this->uiw->toolButton_17] = '3';
    bdmap[this->uiw->toolButton_18] = '4';
    bdmap[this->uiw->toolButton_19] = '5';
    bdmap[this->uiw->toolButton_20] = '6';
    bdmap[this->uiw->toolButton_21] = '7';
    bdmap[this->uiw->toolButton_22] = '8';
    bdmap[this->uiw->toolButton_23] = '9';
    bdmap[this->uiw->toolButton_24] = '*';
    bdmap[this->uiw->toolButton_25] = '#';

    Q_ASSERT(bdmap.contains(btn));

    QString num = this->uiw->comboBox_7->currentText();
    this->uiw->comboBox_7->setEditText(num + bdmap[btn]);
    
    // if (in call state) it's dtmf digit
    // if (this->m_curr_call_id != PJSUA_INVALID_ID) {
        
    // }
}

// void MosipPhone::on2_pjsua_start_done(int seqno, pj_status_t rstatus)
// {
//     qLogx()<<seqno<<rstatus;

//     pj_status_t status;

//     // NON-standard SIP Extension
//     pjsip_cfg()->endpt.allow_port_in_fromto_hdr = PJ_TRUE;

//     status = pjsua_detect_nat_type();
//     if (status != PJ_SUCCESS) {
//         pjsua_perror(__FILE__, "Error starting pjsua", status);
//         qLogx()<<status;
//     }
   
//     pjsua_codec_info infos[20];
//     unsigned int info_count;

//     info_count = 20;
    
//     status = pjsua_enum_codecs(infos, &info_count);
//     if (status == PJ_SUCCESS) {
//         for (int i = 0; i < info_count; i ++) {
//             QString codec_idname = PJSTR2Q(infos[i].codec_id);
//             qLogx()<<"codec info:"<<"p="<<infos[i].priority<<" id="<<PJSTR2Q(infos[i].codec_id);
//             this->uiw->comboBox->insertItem(this->uiw->comboBox->count(), codec_idname);
//         }
//     }


//     pjmedia_aud_dev_info auids[128];
//     pjmedia_snd_dev_info sndids[128];
//     unsigned int auid_count = 128;
//     unsigned int sndid_count = 128;

//     status = pjsua_enum_aud_devs(auids, &auid_count);
//     qDebug()<<"found aud dev count:"<<auid_count;
//     status = pjsua_enum_snd_devs(sndids, &sndid_count);
//     qDebug()<<"found snd dev count:"<<sndid_count;

//     for (int i = 0 ; i < sndid_count; i ++) {
//         QString name;
//         qDebug()<<"aud:"<<QString(auids[i].name)<<" snd:"<<QString(sndids[i].name);
//     }
    
//     int cap_dev = -1, pb_dev = -1;
//     status = pjsua_get_snd_dev(&cap_dev, &pb_dev);
//     qDebug()<<"curr snd dev:"<<"status="<<status<<" cap="<<cap_dev<<" pb="<<pb_dev;
//     qDebug()<<"snd ok?"<<pjsua_snd_is_active();

//     // status = pjsua_set_snd_dev(0, 0);
//     qDebug()<<"snd ok?"<<pjsua_snd_is_active();

//     // 现在再来注册用户吧
//     if (this->uiw->comboBox_4->count() > 0) {
//         this->onSelectedUserAccountChanged(0);
//     }
// }

// void MosipPhone::on2_make_call_done(int seqno, pj_status_t rstatus, pjsua_call_id call_id)
// {
//     qLogx()<<seqno<<rstatus<<call_id;

//     QPushButton *btn = this->uiw->pushButton_5;

//     if (rstatus != PJ_SUCCESS) {
//         btn->setText(tr("Call ..."));
//         btn->setEnabled(true);
//         pjsua_perror(__FILE__, "Error make call", rstatus);
//         this->log_output(LT_USER, QString(tr("Error make call: %1")).arg(rstatus));
//     } else {
//         this->m_curr_call_id = call_id;
//         btn->setText(tr("Hangup"));
//         btn->setEnabled(true);
//     }
// }

/*
  如果返回空，则确定用户输入的号码格式不正确。
 */
QString MosipPhone::_reformat_call_phone_number(const QString &user_phone_number)
{
    QString phone_number;
    QString phone_prefix = "9900866";

    if (!user_phone_number.startsWith("*0")
        && !user_phone_number.startsWith(phone_prefix)) {
        return phone_number;
    }

    if (user_phone_number.length() < 10
        && user_phone_number.length() > 21) {
        return phone_number;
    }

    for (int i = 0; i < user_phone_number.size(); ++i) {
        if (user_phone_number.at(i) >= QChar('0') && user_phone_number.at(i) <= QChar('9')) {
        } else {
            return phone_number;
        }
    }

    // *0 OR 99xxxx
    if (user_phone_number.startsWith(phone_prefix)) {
        phone_number = user_phone_number;
    } else {
        phone_number = phone_prefix + QString::number(8010+qrand()%90)
            + user_phone_number.right(user_phone_number.length()-1);
    }

    return phone_number;
}

QString MosipPhone::_get_input_phone_number()
{
    QString user_phone_number;
    QString phone_number;

    phone_number = user_phone_number = this->uiw->comboBox_7->currentText();
    phone_number = this->_reformat_call_phone_number(user_phone_number);

    if (phone_number.isEmpty()) {
        qLogx()<<"Invalid phone number format."<<user_phone_number;
    }

    return phone_number;
}

bool MosipPhone::_update_current_account_status(int acc_id)
{
    
    return 0;
}

void MosipPhone::customAddContactButtonMenu()
{
    QAction *action;
    QAction *daction;
    QMenu *add_contact_menu = new QMenu(this);

    daction = new QAction(QIcon(":/skins/default/addcontact.png"), tr("Add Contact"), this);
    add_contact_menu->addAction(daction);
    // add_contact_menu->setDefaultAction(action);

    QObject::connect(daction, SIGNAL(triggered()),
                     this, SLOT(onAddContact()));


    action = new QAction(tr("Import Contacts"), this);
    add_contact_menu->addAction(action);

    action = new QAction(tr("Export Contacts"), this);
    add_contact_menu->addAction(action);

    add_contact_menu->addSeparator();

    action = new QAction(tr("New Group"), this);
    add_contact_menu->addAction(action);
    QObject::connect(action, SIGNAL(triggered()), this, SLOT(onAddGroup()));

    // this->uiw->toolButton_6->setMenu(add_contact_menu);
    // this->uiw->toolButton_6->setDefaultAction(daction);
}


void MosipPhone::initContactViewContextMenu()
{
    QAction *action;

    this->m_contact_view_ctx_menu = new QMenu(this);

    action = new QAction(tr("&Call..."), this);
    this->m_contact_view_ctx_menu->addAction(action);
    
    this->m_contact_view_ctx_menu->addSeparator();

    action = new QAction(tr("&Edit Contact"), this);
    this->m_contact_view_ctx_menu->addAction(action);
    QObject::connect(action, SIGNAL(triggered()), this, SLOT(onModifyContact()));

    action = new QAction(tr("&Delete Contact"), this);
    this->m_contact_view_ctx_menu->addAction(action);

    this->m_contact_view_ctx_menu->addSeparator();

    action = new QAction(tr("&Delete Group"), this);
    this->m_contact_view_ctx_menu->addAction(action);

    // QObject::connect(this->uiw->treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
    //                  this, SLOT(onShowContactViewMenu(const QPoint &)));
}

void MosipPhone::onAddContact()
{
    boost::shared_ptr<SqlRequest> req(new SqlRequest());
    // boost::shared_ptr<PhoneContact> pc;
    PhoneContact *pc;
    boost::scoped_ptr<PhoneContactProperty> pcp(new PhoneContactProperty(this));

    if (pcp->exec() == QDialog::Accepted) {
        pc = pcp->contactInfo();

        req->mCbFunctor = boost::bind(&MosipPhone::onAddContactDone, this, _1);
        req->mCbObject = this;
        req->mCbSlot = SLOT(onAddContactDone(boost::shared_ptr<SqlRequest>));
        // req->mSql = QString("INSERT INTO kp_contacts (group_id,phone_number) VALUES (%1, '%2')")
        //     .arg(pc->mGroupId).arg(pc->mPhoneNumber);
        req->mSql = QString("INSERT INTO kp_contacts (group_id,display_name,phone_number) VALUES (IFNULL((SELECT gid FROM kp_groups  WHERE group_name='%1'),3), '%2', '%3')")
            .arg(pc->mGroupName).arg(pc->mUserName).arg(pc->mPhoneNumber);
        req->mReqno = this->m_adb->execute(req->mSql);
        this->mRequests.insert(req->mReqno, req);

        qDebug()<<req->mSql;
    } else {

    }
}

void MosipPhone::onModifyContact()
{
    boost::shared_ptr<SqlRequest> req(new SqlRequest());
    // boost::shared_ptr<PhoneContact> pc;
    PhoneContact * pc;
    boost::scoped_ptr<PhoneContactProperty> pcp(new PhoneContactProperty(this));
    QItemSelectionModel *ism;// = this->uiw->treeView->selectionModel();

    if (!ism->hasSelection()) {
        return;
    }

    QModelIndex cidx, idx;
    cidx = ism->currentIndex();
    idx = ism->model()->index(cidx.row(), 0, cidx.parent());

    if (this->m_contact_model->hasChildren(idx)) {
        // group node
        return ;
    }
    
    ContactInfoNode *cnode = static_cast<ContactInfoNode*>(idx.internalPointer());
    pcp->setContactInfo(cnode->pc);

    if (pcp->exec() == QDialog::Accepted) {
        pc = pcp->contactInfo();

        req->mCbId = pc->mContactId;
        req->mCbFunctor = boost::bind(&MosipPhone::onModifyContactDone, this, _1);
        req->mCbObject = this;
        req->mCbSlot = SLOT(onModifyContactDone(boost::shared_ptr<SqlRequest>));
        req->mSql = QString("UPDATE kp_contacts SET group_id = (SELECT gid FROM kp_groups WHERE group_name='%1'), display_name='%2', phone_number='%3' WHERE cid='%4'")
            .arg(pc->mGroupName).arg(pc->mUserName).arg(pc->mPhoneNumber).arg(pc->mContactId);
        req->mReqno = this->m_adb->execute(req->mSql);
        this->mRequests.insert(req->mReqno, req);

        qDebug()<<req->mSql;
    } else {

    }
}

void MosipPhone::onAddGroup() 
{
    QString group_name;
    boost::shared_ptr<SqlRequest> req(new SqlRequest());
    boost::scoped_ptr<GroupInfoDialog> gidlg(new GroupInfoDialog(this));

    if (gidlg->exec() == QDialog::Accepted) {
        group_name = gidlg->groupName();

        req->mCbFunctor = boost::bind(&MosipPhone::onAddGroupDone, this, _1);
        req->mCbObject = this;
        req->mCbSlot = SLOT(onAddGroupDone(boost::shared_ptr<SqlRequest>));
        req->mSql = QString("INSERT INTO kp_groups (group_name) VALUES ('%1')")
            .arg(group_name);
        req->mReqno = this->m_adb->execute(req->mSql);
        this->mRequests.insert(req->mReqno, req);
    }
}


void MosipPhone::onShowContactViewMenu(const QPoint &pos)
{
    ContactInfoNode *cin = NULL;
    QModelIndex idx;// = this->uiw->treeView->indexAt(pos);
    // qDebug()<<idx<<idx.parent();
    if (idx.parent().isValid()) {
        // leaf contact node
    } else {

    }
    // this->m_contact_view_ctx_menu->popup(this->uiw->treeView->mapToGlobal(pos));
}


void MosipPhone::onDatabaseConnected()
{
    // 加载联系人信息，加载呼叫历史记录信息
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    boost::shared_ptr<SqlRequest> req1(new SqlRequest());
    boost::shared_ptr<SqlRequest> req2(new SqlRequest());
    boost::shared_ptr<SqlRequest> req3(new SqlRequest());
    boost::shared_ptr<SqlRequest> req4(new SqlRequest());

    // QTreeView *ctv;// = this->uiw->treeView; // contact 联系人表
    // QTreeView *htv;// = this->uiw->treeView_2; // 呼叫历史表

    // return;

    // ctv->setHeaderHidden(true);
    // ctv->setModel(this->m_contact_model);
    // ctv->setColumnHidden(2, false);
    // ctv->setAnimated(true);
    // // ctv->setIndentation(20);
    // ctv->setColumnWidth(0, 120);
    // ctv->setColumnWidth(1, 160);
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<ctv->columnWidth(0)
    //         <<ctv->columnWidth(1);

    // htv->setHeaderHidden(true);
    // htv->setModel(this->m_call_history_model);
    // htv->setColumnWidth(0, 100);
    // htv->setColumnWidth(1, 160);
    
    {
        // get accounts list
        // req4->mCbFunctor = boost::bind(&MosipPhone::onGetAllAccountsDone, this, _1);
        // req4->mCbObject = this;
        // req4->mCbSlot = SLOT(onGetAllAccountsDone(boost::shared_ptr<SqlRequest>));
        // req4->mSql = QString("SELECT * FROM kp_accounts WHERE 1=1 ORDER BY aid DESC");
        // req4->mReqno = this->m_adb->execute(req4->mSql);
        // this->mRequests.insert(req4->mReqno, req4);
    }

    // {
    //     // get contacts list
    //     req1->mCbFunctor = boost::bind(&MosipPhone::onGetAllContactsDone, this, _1);
    //     req1->mCbObject = this;
    //     req1->mCbSlot = SLOT(onGetAllContactsDone(boost::shared_ptr<SqlRequest>));
    //     req1->mSql = QString("SELECT * FROM kp_groups,kp_contacts WHERE kp_contacts.group_id=kp_groups.gid");
    //     req1->mReqno = this->m_adb->execute(req1->mSql);
    //     this->mRequests.insert(req1->mReqno, req1);
    // }

    // {
    //     // get groups list
    //     req2->mCbFunctor = boost::bind(&MosipPhone::onGetAllGroupsDone, this, _1);
    //     req2->mCbObject = this;
    //     req2->mCbSlot = SLOT(onGetAllGroupsDone(boost::shared_ptr<SqlRequest>));
    //     req2->mSql = QString("SELECT * FROM kp_groups");
    //     req2->mReqno = this->m_adb->execute(req2->mSql);
    //     this->mRequests.insert(req2->mReqno, req2);
    // }

    // {
    //     // get history list
    //     req3->mCbFunctor = boost::bind(&MosipPhone::onGetAllGroupsDone, this, _1);
    //     req3->mCbObject = this;
    //     req3->mCbSlot = SLOT(onGetAllGroupsDone(boost::shared_ptr<SqlRequest>));
    //     req3->mSql = QString("SELECT * FROM kp_histories ORDER BY call_ctime DESC");
    //     req3->mReqno = this->m_adb->execute(req3->mSql);
    //     this->mRequests.insert(req3->mReqno, req3);
    // }

    /////

    this->loadPreferences(Global::pref);
}

void MosipPhone::onSqlExecuteDone(const QList<QSqlRecord> & results, int reqno, bool eret, const QString &estr, const QVariant &eval)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<reqno; 
    
    QObject *cb_obj = NULL;
    const char *cb_slot = NULL;
    boost::function<bool(boost::shared_ptr<SqlRequest>)> cb_functor;
    boost::shared_ptr<SqlRequest> req;
    bool bret = false;
    // QGenericReturnArgument qret;
    // QGenericArgument qarg;
    bool qret;
    QMetaMethod qmethod;
    char raw_method_name[32] = {0};

    if (this->mRequests.contains(reqno)) {
        req = this->mRequests[reqno];
        req->mRet = eret;
        req->mErrorString = estr;
        req->mExtraValue = eval;
        req->mResults = results;

        // 实现方法太多，还要随机使用一种方法，找麻烦
        if (qrand() % 2 == 1) {
            cb_functor = req->mCbFunctor;
            bret = cb_functor(req);
        } else {
            cb_obj = req->mCbObject;
            cb_slot = req->mCbSlot;

            qDebug()<<"qinvoke:"<<cb_obj<<cb_slot;
            // get method name from SLOT() signature: 1onAddContactDone(boost::shared_ptr<SqlRequest>)
            for (int i = 0, j = 0; i < strlen(cb_slot); ++i) {
                if (cb_slot[i] >= '0' && cb_slot[i] <= '9') {
                    continue;
                }
                if (cb_slot[i] == '(') break;
                Q_ASSERT(j < sizeof(raw_method_name));
                raw_method_name[j++] = cb_slot[i];
            }
            Q_ASSERT(strlen(raw_method_name) > 0);
            Q_ASSERT(cb_obj->metaObject()->indexOfSlot(raw_method_name) != -1);
            bret = QMetaObject::invokeMethod(cb_obj, raw_method_name,
                                             Q_RETURN_ARG(bool, qret),
                                             Q_ARG(boost::shared_ptr<SqlRequest>, req));
            // qmethod = cb_obj->metaObject()->method(cb_obj->metaObject()->indexOfSlot(SLOT(onAddContactDone(boost::shared_ptr<SqlRequest>))));
            // bret = qmethod.invoke(cb_obj, Q_RETURN_ARG(bool, qret),
            //                        Q_ARG(boost::shared_ptr<SqlRequest>, req));
            // qDebug()<<cb_obj->metaObject()->indexOfSlot(cb_slot);
        }
    }
}

bool MosipPhone::onAddContactDone(boost::shared_ptr<SqlRequest> req)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<req->mReqno;    

    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<req->mExtraValue<<req->mErrorString<<req->mRet; 

    QList<QSqlRecord> recs;
    int ncid;

    if (!req->mRet) {
        this->log_output(LT_USER, "添加联系人出错：" + req->mErrorString);
    } else {
        ncid = req->mExtraValue.toInt();
        recs = req->mResults;

        this->m_contact_model->onContactsRetrived(recs.at(0).value("group_id").toInt(), recs);
    }
    
    this->mRequests.remove(req->mReqno);

    return true;
}

bool MosipPhone::onModifyContactDone(boost::shared_ptr<SqlRequest> req)
{
    
    this->m_contact_model->onContactModified(req->mCbId);
    return true;
}

bool MosipPhone::onAddGroupDone(boost::shared_ptr<SqlRequest> req)
{
    QList<QSqlRecord> recs;
    int ngid;

    if (!req->mRet) {
        this->log_output(LT_USER, "添加联系人组失败：" + req->mErrorString);
    } else {
        ngid = req->mExtraValue.toInt();
        recs = req->mResults;

        this->m_contact_model->onGroupsRetrived(recs);
    }

    qDebug()<<recs<<req->mExtraValue;

    this->mRequests.remove(req->mReqno);
    return true;
}

bool MosipPhone::onAddCallHistoryDone(boost::shared_ptr<SqlRequest> req)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<req->mReqno;

    this->m_call_history_model->onNewCallHistoryArrived(req->mResults);    
    
    this->mRequests.remove(req->mReqno);
    return true;
}


bool MosipPhone::onGetAllContactsDone(boost::shared_ptr<SqlRequest> req)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<req->mReqno;

    this->mRequests.remove(req->mReqno);
    return true;
}

bool MosipPhone::onGetAllGroupsDone(boost::shared_ptr<SqlRequest> req)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<req->mReqno;

    this->mRequests.remove(req->mReqno);
    return true;
}

bool MosipPhone::onGetAllHistoryDone(boost::shared_ptr<SqlRequest> req)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<req->mReqno;


    this->mRequests.remove(req->mReqno);
    return true;
}

bool MosipPhone::onGetAllAccountsDone(boost::shared_ptr<SqlRequest> req)
{
    qLogx()<<req->mReqno;

    QList<QSqlRecord> results = req->mResults;
    QSqlRecord rec;
    QString user_name, display_name;
    QString serv_addr;
    QString user_account;
    SipAccount acc;

    for (int i = results.count() - 1; i >= 0; --i) {
        rec = results.at(i);
        
        acc = SipAccount::fromSqlRecord(rec);
        display_name = acc.displayName;
        user_name = acc.userName;
        serv_addr = acc.domain;
        user_account = QString("%1@%2").arg(user_name).arg(serv_addr);

        this->uiw->comboBox_4->insertItem(0, display_name);
    }

    this->uiw->comboBox_4->setCurrentIndex(0);

    this->uiw->comboBox_4->view()->setFixedWidth(60);

    if (results.count() == 1) {
        this->onSelectedUserAccountChanged(0);
    }

    this->mRequests.remove(req->mReqno);


    return true;
}

void MosipPhone::onAccountAdded(SipAccount &acc)
{
    QString user_account;
    user_account = QString("%1@%2").arg(acc.userName).arg(acc.domain);
    // this->uiw->comboBox_6->insertItem(0, user_account);
    // this->uiw->comboBox_6->setCurrentIndex(0);
}

void MosipPhone::onAccountRemoved(SipAccount &acc)
{
    QString user_account;
    QString tmp_account;

    user_account = QString("%1@%2").arg(acc.userName).arg(acc.domain);

    // for (int i = 0; i < this->uiw->comboBox_6->count(); i++) {
    //     tmp_account = this->uiw->comboBox_6->itemText(i);
    //     if (tmp_account == user_account) {
    //         this->uiw->comboBox_6->removeItem(i);
    //         break;
    //     }
    // }
}

void MosipPhone::onon_mosip_engine_error(int eno)
{
    qLogx()<<eno;
}

void MosipPhone::onon_mosip_engine_started()
{
    qLogx()<<"";
    this->wsc2 = new WebSocketClient2("WSS://127.0.0.1:18080/hahaha/");
    QObject::connect(this->wsc2, SIGNAL(onConnected(QString)), this, SLOT(onon_ws_client_connected(QString)));
    QObject::connect(this->wsc2, SIGNAL(onError()), this, SLOT(onon_ws_client_error()));
    QObject::connect(this->wsc2, SIGNAL(onDisconnected()), this, SLOT(onon_ws_client_disconnected()));
    QObject::connect(this->wsc2, SIGNAL(onWSMessage(QByteArray)), this, SLOT(onon_ws_client_message(QByteArray)));

    this->wsc2->connectToServer();
}


void MosipPhone::onon_ws_client_connected(QString rpath)
{
    qLogx()<<rpath;

    QPushButton *btn = this->uiw->pushButton_5;
    btn->setEnabled(true);

    int cnter = 0;
    std::string jcstr;
    while (this->cmds_queue_before_connected.count() > 0) {
        qLogx()<<++cnter;
        jcstr = this->cmds_queue_before_connected.at(0);
        this->cmds_queue_before_connected.remove(0);

        
        this->wsc2->sendMessage(jcstr);
    }
}

void MosipPhone::onon_ws_client_disconnected()
{
    qLogx()<<"";
}

void MosipPhone::onon_ws_client_error()
{
    qLogx()<<"";
}

void MosipPhone::onon_ws_client_message(QByteArray msg)
{
    qLogx()<<""<<msg;
    bool bret;
    std::string semsg;

    semsg = std::string(msg.data(), msg.length());

    bret = this->process_ctrl_message(semsg);

}

bool MosipPhone::process_ctrl_message(const std::string &msg)
{
    qLogx()<<msg.c_str();
    int cmd_id;
    int msg_id;

    CmdRegister cmd_reg;
    CmdRegState cmd_rs;
    CmdCallState cmd_cs;
    CmdCallMediaState cmd_cms;
    CmdCodecList cmd_cl;

    cmd_id = InterMessage().jpack_cmdid(msg);

    switch (cmd_id) {
    case IPC_NO_REG_STATE:
        InterMessage().junpack_message(cmd_rs, msg);
        this->onRegisterState(cmd_rs);
        break;
    case IPC_NO_CALL_STATE:
        InterMessage().junpack_message(cmd_cs, msg);
        this->onCallState(cmd_cs);
        break;
    case IPC_NO_CALL_MEDIA_STATE:
        InterMessage().junpack_message(cmd_cms, msg);
        this->onCallMediaState(cmd_cms);
        break;
    case IPC_NO_CODEC_LIST:
        InterMessage().junpack_message(cmd_cl, msg);
        this->onCodecList(cmd_cl);
        break;
    default:
        qLogx()<<"Unsupported backend cmd:"<<cmd_id;
        break;
    }

    return true;
}


// TODO 所有明文字符串需要使用翻译方式获取，而不是直接写在源代码中
// log is utf8 codec
void MosipPhone::log_output(int type, const QString &log)
{
    QListWidgetItem *witem = nullptr;
    QString log_time = QDateTime::currentDateTime().toString("hh:mm:ss");

    int debug = 1;

    QTextCodec *u8codec = QTextCodec::codecForName("UTF-8");
    QString u16_log = log_time + " " + u8codec->toUnicode(log.toAscii());

    if (type == LT_USER) {
        // TODO 怎么确定是属于呼叫日志呢。恐怕还是得在相应的地方执行才行。
        // this->uiw->label_11->setText(u16_log);
        witem = new QListWidgetItem(QIcon(":/skins/default/info.png"), u16_log);
        this->uiw->listWidget->addItem(witem);
    } else if (type == LT_DEBUG && debug) {
        witem = new QListWidgetItem(QIcon(":/skins/default/info.png"), u16_log);
        this->uiw->listWidget->addItem(witem);
    } else {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<type<<log;
    }

    // 清除多余日志
    static int max_log_count = 30;
    if (debug == 1) {
        max_log_count += 200;
    }
    if (this->uiw->listWidget->count() > max_log_count) {
        int rm_count = this->uiw->listWidget->count() - max_log_count;
        // 从最老的日志开始删除
        for (int i = 0; i < rm_count; i++) {
            witem = this->uiw->listWidget->takeItem(i);
            delete witem;
        }
        // 从最新的开始
        // for (int i = rm_count - 1; i >= 0; i--) {
        //     witem = this->uiw->listWidget->takeItem(max_log_count+i);
        //     delete witem;
        // }
    }

    qLogx()<<type<<u16_log;
}


////////////////////////////////
// PJSipEventThread::PJSipEventThread(QObject *parent)
//     : QThread(parent)
// {
//     this->quit_loop = false;
// }

// PJSipEventThread::~PJSipEventThread()
// {
    
// }

// void PJSipEventThread::run()
// {
//     pj_thread_desc initdec;
//     pj_thread_t* thread = 0;
//     pj_status_t status;
//     int evt_cnt = 0;

//     qLogx()<<"ready register pjsip thread by Qt";
//     if (!pj_thread_is_registered()) {
//         status = pj_thread_register("PjsipPollThread_run", initdec, &thread);
//         if (status != PJ_SUCCESS) {
//             qLogx()<<"pj_thread_register faild:"<<status;
//             return;
//         }
//     }
//     PJ_CHECK_STACK();
//     qLogx()<<"registerred pjsip thread:"<<thread;

//     this->dump_info(thread);
//     // while(isStackInit()) {
//     //     pjsua_handle_events(10);
//     // }
//     qLogx()<<"enter pjsua thread loop supported by Qt";
//     while (!quit_loop) {
//         evt_cnt = pjsua_handle_events(20);
//         // qDebug()<<"pjsua pool event in thread supported by Qt, proccess count:"<<evt_cnt;
//         if (qrand() % 100 == 1) {
//             fprintf(stdout, "."); fflush(stdout);
//         }
//     }
// }

// void PJSipEventThread::dump_info(pj_thread_t *thread)
// {
//     Q_ASSERT(thread != NULL);

//     qLogx()<<"pj_thread_is_registered:"<<pj_thread_is_registered();
//     qLogx()<<"pj_thread_get_prio:"<<pj_thread_get_prio(thread);
//     qLogx()<<"pj_thread_get_prio_min:"<<pj_thread_get_prio_min(thread);
//     qLogx()<<"pj_thread_get_prio_max:"<<pj_thread_get_prio_max(thread);
//     qLogx()<<"pj_thread_get_name:"<<pj_thread_get_name(thread);
//     qLogx()<<"pj_getpid:"<<pj_getpid();
// }

