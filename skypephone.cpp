// kitpstn.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-10-20 17:20:22 +0800
// Version: $Id: skypephone.cpp 854 2011-04-26 15:08:58Z drswinghead $
// 

#include <QtCore>
#include <QtGui>

#include "ui_skypephone.h"
#include "skypephone.h"

#include "metauri.h"
#include "skype.h"
#include "skypetunnel.h"
#include "skypetracer.h"

#include "websocketclient.h"
#include "asyncdatabase.h"
#include "phonecontact.h"
#include "phonecontactproperty.h"

SkypePhone::SkypePhone(QWidget *parent)
    :QWidget(parent),
     uiw(new Ui::SkypePhone())
{
    this->uiw->setupUi(this);

    this->m_dialpanel_layout_index = 2;
    this->m_dialpanel_layout_item = this->layout()->itemAt(this->m_dialpanel_layout_index);
    // this->m_dialpanel_layout_item->widget()->setVisible(false);
    this->m_call_state_layout_index = 3;
    this->m_call_state_layout_item = this->layout()->itemAt(this->m_call_state_layout_index);
    // this->m_call_state_layout_item->widget()->setVisible(false);
    this->m_status_bar = nullptr;

    this->m_call_button_disable_count = 1;

    this->defaultPstnInit();
    this->m_adb = new AsyncDatabase();
    this->m_adb->start();
    
    QObject::connect(this->m_adb, SIGNAL(results(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)),
                     this, SLOT(onSqlExecuteDone(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)));


    //////
    this->m_call_button_disable_count.ref();
    QHostInfo::lookupHost("gw.skype.tom.com", this, SLOT(onCalcWSServByNetworkType(QHostInfo)));
    this->log_output(LT_USER, "检测网络类型...");
}

SkypePhone::~SkypePhone()
{
    this->m_adb->quit();
    this->m_adb->wait();
    delete this->m_adb;

    delete uiw;
}

void SkypePhone::paintEvent ( QPaintEvent * event )
{
    QWidget::paintEvent(event);
    // qDebug()<<"parintttttt"<<event<<event->type();
    if (this->first_paint_event) {
        this->first_paint_event = false;
		// QTimer::singleShot(50, this, SLOT(main_ui_draw_complete()));
    }
}
void SkypePhone::showEvent ( QShowEvent * event )
{
    QWidget::showEvent(event);
    // qDebug()<<"showwwwwwwwwwwww"<<event<<event->type();
}

void SkypePhone::init_status_bar(QStatusBar *bar)
{
    this->m_status_bar = bar;
}

// call only once
void SkypePhone::defaultPstnInit()
{
    this->mSkype = NULL;
    this->mtun = NULL;
    this->mSkypeTracer = NULL;

    // QObject::connect(this->uiw->checkBox_2, SIGNAL(stateChanged(int)),
    //                  this, SLOT(onInitPstnClient()));

    QObject::connect(this->uiw->toolButton_2, SIGNAL(clicked()),
                     this, SLOT(onShowDialPanel()));

    QObject::connect(this->uiw->pushButton, SIGNAL(clicked()),
                     this, SLOT(onShowSkypeTracer()));
    QObject::connect(this->uiw->pushButton_3, SIGNAL(clicked()),
                     this, SLOT(onConnectSkype()));
    // QObject::connect(this->mainUI.pushButton_2, SIGNAL(clicked()),
    //                  this, SLOT(onSendPackage()));
    QObject::connect(this->uiw->pushButton_4, SIGNAL(clicked()),
                     this, SLOT(onCallPstn()));
    QObject::connect(this->uiw->toolButton_10, SIGNAL(clicked()),
                     this, SLOT(onHangupPstn()));

    // QObject::connect(this->uiw->toolButton_6, SIGNAL(clicked()),
    //                  this, SLOT(onAddContact()));

    this->customAddContactButtonMenu();
}

void SkypePhone::customAddContactButtonMenu()
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

    this->uiw->toolButton_3->setMenu(add_contact_menu);
    this->uiw->toolButton_3->setDefaultAction(daction);
}

// maybe called twice or more
void SkypePhone::onInitPstnClient()
{
    if (this->mtun != NULL) {
        delete this->mtun;
        this->mtun = NULL;
    }
    if (this->mSkype != NULL) {
        this->mSkype->disconnect();
        delete this->mSkype;
        this->mSkype = NULL;
    }
    this->mSkype = new Skype("karia2");
    this->mSkype->setRunAsClient();
    // this->mSkype->connectToSkype();
    // QObject::connect(this->mSkype, SIGNAL(skypeError(int, QString)),
    //                  this, SLOT(onSkypeError(int, QString)));

    QObject::connect(this->mSkype, SIGNAL(connected(QString)), this, SLOT(onSkypeConnected(QString)));
    QObject::connect(this->mSkype, SIGNAL(realConnected(QString)),
                     this, SLOT(onSkypeRealConnected(QString)));

    QObject::connect(this->mSkype, SIGNAL(UserStatus(QString,int)),
                     this, SLOT(onSkypeUserStatus(QString,int)));

    QObject::connect(this->mSkype, SIGNAL(newCallArrived(QString,QString,int)),
                     this, SLOT(onSkypeCallArrived(QString,QString,int)));

    this->mtun = new SkypeTunnel(this);
    this->mtun->setSkype(this->mSkype);
}

void SkypePhone::onShowSkypeTracer()
{
    if (this->mSkypeTracer == NULL) {
        this->mSkypeTracer = new SkypeTracer(this);
        QObject::connect(this->mSkype, SIGNAL(commandRequest(QString)),
                         this->mSkypeTracer, SLOT(onCommandRequest(QString)));
        QObject::connect(this->mSkype, SIGNAL(commandResponse(QString, QString)),
                         this->mSkypeTracer, SLOT(onCommandResponse(QString, QString)));
        QObject::connect(this->mSkypeTracer, SIGNAL(commandRequest(QString)),
                         this->mSkype, SLOT(onCommandRequest(QString)));
    }

    this->mSkypeTracer->setVisible(!this->mSkypeTracer->isVisible());
}

void SkypePhone::onConnectSkype()
{
    this->log_output(LT_USER, "正在连接Skype API。。。，");
    this->log_output(LT_USER, "请注意Skype客户端弹出的认证提示。");
    this->uiw->pushButton_3->setEnabled(false);
    this->onInitPstnClient();

    qDebug()<<"Skype name:"<<this->mSkype->handlerName();
}

void SkypePhone::onConnectApp2App()
{
    // QString skypeName = this->uiw->lineEdit->text();
    // this->mtun->setStreamPeer(skypeName);

    // QStringList contacts = this->mSkype->getContacts();
    // qDebug()<<skypeName<<contacts;

    // this->mSkype->newStream(skypeName);
    // this->mSkype->newStream("drswinghead");
}

void SkypePhone::onSkypeConnected(QString user_name)
{
    qDebug()<<"Waiting handler name:"<<user_name;
}

void SkypePhone::onSkypeRealConnected(QString user_name)
{
    qDebug()<<"Got handler name:"<<user_name;
    this->uiw->label_3->setText(user_name);

    // count == 0
    if (this->m_call_button_disable_count.deref() == false) {
        this->uiw->pushButton_4->setEnabled(true);
    }

    this->log_output(LT_USER, "连接Skype API成功，用户名：" + user_name);
}

void SkypePhone::onSkypeUserStatus(QString str_status, int int_status)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<str_status<<int_status;
    QPixmap icon = QPixmap(":/skins/default/status_offline.png");
    switch (int_status) {
    case SS_ONLINE:
    case SS_INVISIBLE:
    case SS_SKYPEME:
        icon = QPixmap(":/skins/default/status_online.png");
        break;
    case SS_AWAY:
        icon = QPixmap(":/skins/default/status_away.png");
        break;
    case SS_DND:
        icon = QPixmap(":/skins/default/status_dnd.png");
        break;
    case SS_NA:
        icon = QPixmap(":/skins/default/status_busy.png");
        break;
    case SS_OFFLINE:
        break;
    default:
        break;
    }

    this->uiw->label_14->setPixmap(icon);
    this->uiw->label_14->setToolTip(str_status);
}

void SkypePhone::onSkypeCallArrived(QString callerName, QString calleeName, int callID)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<callerName<<calleeName<<callID;
    this->m_curr_skype_call_id = callID;
    this->m_curr_skype_call_peer = calleeName;
}

void SkypePhone::onCallPstn()
{
    if (sender() != NULL) {
        // init call by user
        this->m_conn_ws_retry_times = this->m_conn_ws_max_retry_times;
    } else {
        // retry call by robot
        if (--this->m_conn_ws_retry_times >= 0) {
            qDebug()<<"retry conn ws: "<< (this->m_conn_ws_max_retry_times-this->m_conn_ws_retry_times);
        } else {
            qDebug()<<"retry conn ws exceed max retry:"<<this->m_conn_ws_max_retry_times;
            return;
        }
    }
    // 检测号码有效性
    
    // 设置呼叫状态
    // this->uiw->pushButton_4->setEnabled(false);
    this->uiw->label_5->setText(this->uiw->comboBox_3->currentText());

    Q_ASSERT(!this->m_ws_serv_ipaddr.isEmpty());
    // QString wsuri = "ws://202.108.12.212:80/" + this->mSkype->handlerName() + "/";
    QString wsuri = QString("ws://%1:80/%2/").arg(this->m_ws_serv_ipaddr).arg(this->mSkype->handlerName());
    this->wscli = boost::shared_ptr<WebSocketClient>(new WebSocketClient(wsuri));
    QObject::connect(this->wscli.get(), SIGNAL(onConnected(QString)), this, SLOT(onWSConnected(QString)));
    QObject::connect(this->wscli.get(), SIGNAL(onDisconnected(QString)), this, SLOT(onWSDisconnected(QString)));
    QObject::connect(this->wscli.get(), SIGNAL(onWSMessage(QByteArray)), this, SLOT(onWSMessage(QByteArray)));
    QObject::connect(this->wscli.get(), SIGNAL(onEror(int, const QString&)), 
                     this, SLOT(onEror(int, const QString&)));
    bool ok = this->wscli->connectToServer();
    Q_ASSERT(ok);

   // QString num = this->uiw->comboBox_3->currentText();
    
   //  SkypePackage sp;
   //  sp.seq = SkypeCommand::nextID().toInt();
   //  sp.type = SkypePackage::SPT_GW_SELECT;
   //  sp.data = num;

   //  QString spStr = sp.toString();
   //  qDebug()<<spStr;
   //  // bool ok = this->mSkype->sendPackage(this->uiw->lineEdit->text(), spStr);
   //  // if (!ok) {
   //  //     qDebug()<<"maybe stream dosn't created, try create stream first";
   //  // }
    
   //  this->mtun->setPhoneNumber(num);
   //  // QString callee_name = QString("%1,%2").arg("liuguangzhao01").arg(num);
   //  QString callee_name = QString("%1").arg("liuguangzhao01");
   //  this->mSkype->callFriend(callee_name);
}

void SkypePhone::onHangupPstn()
{
    this->mSkype->setCallHangup(QString::number(this->m_curr_skype_call_id));
}

void SkypePhone::onShowDialPanel()
{
    // this->m_dialpanel_layout_item->widget()->setVisible(false);
    if (this->m_dialpanel_layout_item->widget()->isVisible()) {
        
    }
    this->m_dialpanel_layout_item->widget()->setVisible(!this->m_dialpanel_layout_item->widget()->isVisible());
}

void SkypePhone::onAddContact()
{

    PhoneContact pc;
    PhoneContactProperty *pcp = new PhoneContactProperty();
    boost::shared_ptr<SqlRequest> req(new SqlRequest());

    if (pcp->exec() == QDialog::Accepted) {
        
    }

    req->mCbFunctor = boost::bind(&SkypePhone::onAddContactDone, this, _1);
    req->mCbObject = this;
    req->mCbSlot = SLOT(onAddContactDone(boost::shared_ptr<SqlRequest>));
    req->mSql = "INSERT INTO kp_contacts (group_id,phone_number) VALUES (1332332345, '234中中56789043')";
    req->mReqno = this->m_adb->execute(req->mSql);
    this->mRequests.insert(req->mReqno, req);
    
    
}

void SkypePhone::onWSConnected(QString path)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<path;

    // 
    QString num = this->uiw->comboBox_3->currentText();
    QString cmd = QString("101$%1$%2").arg(this->mSkype->handlerName()).arg(num);
    this->wscli->sendMessage(cmd.toAscii());
}

void SkypePhone::onWSError(int error, const QString &errmsg)
{
    qDebug()<<error<<errmsg;
}

void SkypePhone::onWSDisconnected()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
}

void SkypePhone::onWSMessage(QByteArray msg)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<msg;
    
    QString router_name;
    QStringList tmps;
    QStringList fields = QString(msg).split("$");
    QString log_msg;

    switch (fields.at(0).toInt()) {
    case 102:
        Q_ASSERT(fields.at(1) == this->mSkype->handlerName());
        tmps = fields.at(2).split("\n");
        if (tmps.at(0).trimmed() == "200 OK") {
            Q_ASSERT(tmps.at(1).trimmed().toInt() >= 1);
            router_name = tmps.at(2).trimmed();
            
            this->mSkype->callFriend(router_name);
        }
        break;
    case 108:
        log_msg = "通话结束。";
        // log_msg = QString("<p><img src='%1'/> %2</p>").arg(":/skins/default/info.png").arg(log_msg);
        // this->uiw->plainTextEdit->appendPlainText(log_msg);
        // this->uiw->plainTextEdit->appendHtml(log_msg);
        this->log_output(LT_USER, log_msg);
        break;
    case 118:
        log_msg = QString("对方已挂机，代码:%1").arg(fields.at(5));
        // log_msg = QString("<p><img src='%1'/> %2</p>").arg(":/skins/default/info.png").arg(log_msg);
        // this->uiw->plainTextEdit->appendPlainText(log_msg);
        // this->uiw->plainTextEdit->appendHtml(log_msg);
        this->log_output(LT_USER, log_msg);
        break;
    default:
        qDebug()<<"Unknwon ws msg:"<<msg;
        break;
    }

}

// TODO 在网络情况不好的情况下，确实有可能好3,5秒之后这个才返回
// 所以有必要在呼叫的时候检测这个值是否已经取到，要是没有取到，还需要等待操作完成。
// 需要两个值还控制呼叫按钮是否可用，有有点复杂了。
void SkypePhone::onCalcWSServByNetworkType(QHostInfo hi)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<hi.addresses();

    QString log = "网络类型：";
    QString ws_serv_ipaddr;
    QList<QHostAddress> addrs = hi.addresses();
    
    if (addrs.count() > 0) {
        if (addrs.at(0).toString() == "202.108.15.80") {
            ws_serv_ipaddr = "202.108.15.81";
            log += "网通";
        } else if (addrs.at(0).toString() == "211.100.41.6") {
            ws_serv_ipaddr = "211.100.41.7";
            log += "电信";
        } else {
            qDebug()<<"You network is strange enought.";
            log += "网络异常，检测结果不准确";
            Q_ASSERT(1==2);
        }
        this->m_ws_serv_ipaddr = ws_serv_ipaddr;
    } else {
        qDebug()<<"Can not resolve IP for :" << hi.hostName();
        log += "网络异常，无法检测到网络类型";
    }

    // for test
    QFile fp(QApplication::applicationDirPath() + "/kitphone.ini");
    if (fp.exists()) {
        fp.open(QIODevice::ReadOnly);
        QByteArray ba = fp.readAll();
        fp.close();

        if (ba.startsWith("2")) {
            this->m_ws_serv_ipaddr = ba.trimmed();
            log += "(测试)";            
        }
    }

    // count == 0
    if (this->m_call_button_disable_count.deref() == false) {
        this->uiw->pushButton_4->setEnabled(true);
    }
    qDebug()<<"All in all, the notice server is:"<<this->m_ws_serv_ipaddr;
    this->log_output(LT_USER, log);
}

void SkypePhone::onNoticeUserStartup()
{
    
}

void SkypePhone::onSqlExecuteDone(const QList<QSqlRecord> & results, int reqno, bool eret, const QString &estr, const QVariant &eval)
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

bool SkypePhone::onAddContactDone(boost::shared_ptr<SqlRequest> req)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<req->mReqno;    

    

    return true;
}

// log is utf8 codec
void SkypePhone::log_output(int type, const QString &log)
{
    QListWidgetItem *witem = nullptr;
    QString log_time = QDateTime::currentDateTime().toString("hh:mm:ss");

    int debug = 1;

    QTextCodec *u8codec = QTextCodec::codecForName("UTF-8");
    QString u16_log = log_time + " " + u8codec->toUnicode(log.toAscii());

    if (type == LT_USER) {
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
}
