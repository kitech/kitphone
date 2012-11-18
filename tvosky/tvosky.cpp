// tvosky.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-01 10:10:54 +0800
// Version: $Id: tvosky.cpp 688 2011-01-13 15:04:33Z drswinghead $
// 

#include "main.h"
#include "cskype.h"

#include "utils.h"
#include "skypeutils.h"

#include "preference.h"

#include "buddybox.h"
#include "buddyitem.h"

#include "tvosky.h"
#include "ui_tvosky.h"

String inetAddr         = "127.0.0.1";
int portNum             = 8963;
String                  localname = "";
const char*             log_streams = 0;

String apptoken         = "xDbixo3eTW";
CSkype* gskype           = 0;
// CommandInvoker* invoker = 0;

#ifdef SKYPEKIT_SURFACE_RENDERING
#if defined(GL_WINDOW)
glWindow *preview_window=0;
glWindow *receive_window=0;
#elif defined(X11_WINDOW)
X11Window *preview_window=0;
X11Window *receive_window=0;
#endif
void window_run_events();
#endif


TvoSky::TvoSky(QWidget *parent)
    : QMainWindow(parent),
      uiw(new Ui::TvoSky())
{
    this->uiw->setupUi(this);

    ////// init members
    this->skproc = NULL;
    this->mNetAM = new QNetworkAccessManager();

    // connect signal
    QObject::connect(this->uiw->pushButton, SIGNAL(clicked()),
                     this, SLOT(onLoginSkype()));
    QObject::connect(this->uiw->pushButton_2, SIGNAL(clicked()),
                     this, SLOT(onCallPhone()));

    QObject::connect(this->uiw->listWidget_2, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
                     this, SLOT(onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));

    // FriendItemDelegate *idg = new FriendItemDelegate();
    // idg->lw = this->uiw->listWidget_2;
    // this->uiw->listWidget_2->setItemDelegate(idg);

    BuddyItem *item, *item2;
    // BuddyBox *bbox;

    // item = new BuddyItem("item11111");
    // this->uiw->listWidget_2->addItem(item);
    // // bbox = new BuddyBox(this->uiw->listWidget_2);
    // // this->uiw->listWidget_2->setItemWidget(item, bbox);
    // // bbox->show();
    // qDebug()<<"item count:"<<this->uiw->listWidget_2->count();
    // // this->uiw->listWidget_2->openPersistentEditor(item);
    // // idg->set_current_row(0);
    // // item->setSizeHint(QSize(100, 100)); // nice ...........
    // // item->setText("");

    // item2 = new BuddyItem("222222item");
    // this->uiw->listWidget_2->addItem(item2);

    // item2 = new BuddyItem("item 33333");
    // this->uiw->listWidget_2->addItem(item2);

    ////
    this->adjustStartupUi();
}

TvoSky::~TvoSky()
{
    this->stopSkypekitProcess();
}

void TvoSky::adjustStartupUi()
{
    QWidget *main_uiw = this->uiw->widget;
    QWidget *login_uiw = this->uiw->widget_2;

    main_uiw->setVisible(false);

    this->resize(235, 520);
}

void TvoSky::adjustLoggedUi()
{
    QWidget *main_uiw = this->uiw->widget;
    QWidget *login_uiw = this->uiw->widget_2;

    main_uiw->setVisible(true);
    login_uiw->setVisible(false);

    this->resize(620, 500);
}

void TvoSky::onLoginSkype()
{
    QTimer *timer;

    this->loginAnimateCycle = 0;
    this->uiw->label->setPixmap(QPixmap(":/icons/appointment-recurring.png"));
    timer = this->loginAnimateTimer = new QTimer();
    timer->setInterval(200);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(onLoginingAnimate()));
    timer->start();

    this->startSkypekitProcess();
}

bool TvoSky::startSkypekitProcess()
{
#if defined(WIN32)
    QString proc_path = QCoreApplication::applicationDirPath() + "/weskend.bin";
#else
    QString proc_path = QCoreApplication::applicationDirPath() + "/leskend.bin";
#endif
    QStringList args;

    args << "-p" << "8963"
         << "-f" << "nodb";

    this->skproc = new QProcess();
    QObject::connect(this->skproc, SIGNAL(error(QProcess::ProcessError)),
                     this, SLOT(onSkypekitBackendError(QProcess::ProcessError)));
    QObject::connect(this->skproc, SIGNAL(finished(int, QProcess::ExitStatus)),
                     this, SLOT(onSkypekitBackendFinished(int, QProcess::ExitStatus)));
    QObject::connect(this->skproc, SIGNAL(readyReadStandardError()),
                     this, SLOT(onSkypekitBackendReadyReadStandardError()));
    QObject::connect(this->skproc, SIGNAL(readyReadStandardOutput()),
                     this, SLOT(onSkypekitBackendReadyReadStandardOutput()));
    QObject::connect(this->skproc, SIGNAL(started()),
                     this, SLOT(onSkypekitBackendStarted()));
    QObject::connect(this->skproc, SIGNAL(stateChanged(QProcess::ProcessState)),
                     this, SLOT(onSkypekitBackendStateChanged(QProcess::ProcessState)));
    
    this->skproc->start(proc_path, args);

    return true;
}

bool TvoSky::stopSkypekitProcess()
{
    if (this->skproc != NULL) {
        this->skproc->kill();
    }
    
    return true;
}

bool TvoSky::appendLog(QString log)
{
    this->uiw->plainTextEdit->appendPlainText(log);
    return true;
}

void TvoSky::onLoginStateChanged(int prop)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<prop;

    CAccount *acc = (CAccount*)(sender());
    
    String value = acc->GetProp(prop);
    List_String dbg = acc->getPropDebug(prop, value);

    if (prop == Account::P_STATUS) {
        if ((Account::STATUS)acc->GetUintProp(Account::P_STATUS) == Account::LOGGED_IN) {
            // login done,
            this->loginAnimateTimer->stop();
            delete this->loginAnimateTimer;
            this->loginAnimateTimer = NULL;
            this->uiw->label->setPixmap(QPixmap(":/icons/network-connect.png"));

            // 
            ContactGroup::Ref cg;
            Contact::Refs contacts;
            ::gskype->GetHardwiredContactGroup(ContactGroup::ALL_KNOWN_CONTACTS, cg);
            cg->GetContacts(contacts);
            q_debug()<<"contact size:"<<contacts.size();

            this->adjustLoggedUi();
        }
    }
    if (prop == Account::P_AVAILABILITY) {
        int ava_idx = 8;
        switch (acc->GetUintProp(Account::P_AVAILABILITY)) {
        case Contact::UNKNOWN:
            break;
        case Contact::OFFLINE:
            ava_idx = 6;
            break;
        case Contact::INVISIBLE:
            ava_idx = 5;
            break;
        case Contact::DO_NOT_DISTURB:
            ava_idx = 4;
            break;
        case Contact::AWAY:
            ava_idx = 2;
            break;
        case Contact::SKYPE_ME:
            ava_idx = 1;
            break;
        case Contact::ONLINE:
            ava_idx = 0;
            break;
        default:
            q_debug()<<"Unknown or uncare P_AVAILABILITY";
            break;
        };
        this->uiw->comboBox->setCurrentIndex(ava_idx);
    }
}

void TvoSky::onLoginingAnimate()
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    QTimer *timer = (QTimer*)(sender());
    
    // animate rolling
    QTransform trans;
    QPixmap old_pixmap, new_pixmap;
    
    old_pixmap = QPixmap(":/icons/appointment-recurring.png");

    trans.rotateRadians(-5.0 * (++this->loginAnimateCycle));
    new_pixmap = old_pixmap.transformed(trans);

    this->uiw->label->setPixmap(new_pixmap);
    // new_pixmap.fill(this->uiw->label, 0, 0);
}

void TvoSky::onConversationListChanged(PReference *pconversation, int type, bool added)
{
    CConversation::Ref conv = pconversation->toConversation();
    delete pconversation; pconversation = NULL;

    // CConversation * conv_p = static_cast<CConversation*>(conv_ref.operator->());
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<conv_p<<type<<added;

    SEString identity, display_name;
    BuddyItem *item;
    QString  q_identity, q_display_name;

    if (type == Conversation::ALL_CONVERSATIONS) {
        conv->GetPropIdentity(identity);
        conv->GetPropDisplayname(display_name);
        q_identity = identity.data();
        q_display_name = gskype->codec->toUnicode(display_name.data());

        item = new BuddyItem("item11111");
        this->uiw->listWidget_2->addItem(item);

        item->setIdentity(q_identity);
        // item->setText(q_display_name);
        item->setData(Qt::DisplayRole, q_display_name);

        item->conv = conv;
        q_debug()<<item<<item->bbox;

        item->setIcon(QIcon(":/icons/status/offline.png"));
    }

    SEString country;
    ParticipantRefs participants;
    conv->GetParticipants(participants);
    for (unsigned int i = 0; i < participants.size(); i++) {
        participants[i]->GetPropLiveCountry(country);
        participants[i]->GetPropIdentity(identity);
        q_debug()<<participants.size()<<i<<QString(identity.data())<<QString(country.data());
    }
}

void TvoSky::onContactStateChanged(int prop)
{
    BuddyItem *item = 0;
    BuddyBox *bbox = 0;
    CContact::Ref contact = ((CContact*)(sender()))->ref();
    SEString skype_name;
    Contact::AVAILABILITY availability;

    if (prop == Contact::P_AVAILABILITY) {
        contact->GetPropSkypename(skype_name);
        for (int i = this->uiw->listWidget_2->count() - 1 ; i >= 0; --i) {
            item = (BuddyItem*)this->uiw->listWidget_2->item(i);
            q_debug()<<item->identity();
            if (item->identity() == QString(skype_name.data())) {
                break;
            }
            item = 0;
        }

        contact->GetPropAvailability(availability);
        QString status_icons_name = SkypeUtils::availablityIcon(availability);

        if (item && !item->isSelected()) {
            item->setIcon(QIcon(status_icons_name));
        } else if (item && item == this->uiw->listWidget_2->currentItem()) {
            bbox = (BuddyBox*)this->uiw->listWidget_2->itemWidget(item);
            bbox->setAvailability(availability);
        }
    }
}

void TvoSky::onCurrentItemChanged ( QListWidgetItem * current, QListWidgetItem * previous)
{
    QDateTime btime, etime;
    btime = QDateTime::currentDateTime();

    int rowno = this->uiw->listWidget_2->currentRow();

    CConversation::Ref conv;
    SEString identity, display_name;
    BuddyItem *citem, *pitem;
    BuddyBox *bbox = 0;

    q_debug()<<current;
    citem = (BuddyItem*)current;
    {
        bbox = new BuddyBox();
        conv = citem->conv;
        // Q_ASSERT(conv != NULL);
        // Q_ASSERT(conv.present());

        bbox->setConversation(conv);
        q_debug()<<"";
    }
    qDebug()<<bbox;

    etime = QDateTime::currentDateTime();
    q_debug()<<"here"<<btime.msecsTo(etime);

    current->setSizeHint(QSize(100, 100));
    this->uiw->listWidget_2->setItemWidget(current, bbox);
    current->setData(Qt::DisplayRole, "");
    current->setIcon(QIcon());

    etime = QDateTime::currentDateTime();
    q_debug()<<"here"<<btime.msecsTo(etime);

    if (previous) {
        bbox = (BuddyBox*)this->uiw->listWidget_2->itemWidget(previous);
        int availability = bbox->availability();

        // 这两个调用都会把widget给删掉啊
        this->uiw->listWidget_2->setItemWidget(previous, 0);
        // this->uiw->listWidget_2->removeItemWidget(previous);
        // previous->setText("dsfsdfsdfioi" + QString::number(qrand()));
        previous->setSizeHint(QSize(100, 15));
        pitem = (BuddyItem*)previous;
        pitem->conv->GetPropDisplayname(display_name);
        previous->setData(Qt::DisplayRole, gskype->codec->toUnicode(display_name.data()));
        previous->setIcon(QIcon(SkypeUtils::availablityIcon(availability)));
    }

    etime = QDateTime::currentDateTime();
    q_debug()<<"here"<<btime.msecsTo(etime);
}

bool TvoSky::onCallSkype(QString target)
{
    CConversation::Ref convs;
    SEStringList CallTargets;
    ParticipantRefs ConvParticipantList;
    SEString MyCallTarget;
    int ret;
    
    MyCallTarget = "yat-sen";
    MyCallTarget = target.toAscii().data();
    CallTargets.append(MyCallTarget);
    ret = ::gskype->GetConversationByParticipants(CallTargets, convs, true, true);
    qDebug()<<"get convs:"<<ret;

    ret = convs->GetParticipants(ConvParticipantList, Conversation::ALL);
    qDebug()<<"get participant:"<<ret;

    for (unsigned int i = 0; i < ConvParticipantList.size(); i++) {
        if (ConvParticipantList[i]->GetProp(Participant::P_IDENTITY) == MyCallTarget) {
            qDebug()<<"Calling "<<(const char*)MyCallTarget;
            ret = ConvParticipantList[i]->Ring(MyCallTarget, false, 0, 10, false);
            break;
        }  
    }    
    return true;
}

void TvoSky::onCallPhone()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;

    QString sendUrl;
    QNetworkRequest req;

    QString num = this->uiw->comboBox_2->currentText();
    QString router = "liuguangzhao01";
    // this->mycall = true;
    sendUrl = QString("http://202.108.12.211/ftsearch/skype_gateway.php?func=notepair&caller_name=%1&callee_phone=%2&rand=%3")
        .arg(this->uiw->lineEdit->text()).arg(num).arg(qrand());
    
    req.setUrl(sendUrl);
    QNetworkReply *rep = this->mNetAM->get(req);
    this->mPresendResult.insert(rep, QByteArray());
    QObject::connect(rep, SIGNAL(finished()), this, SLOT(onPresendFinished()));
    QObject::connect(rep, SIGNAL(readyRead()), this, SLOT(onPresendReadyRead()));
    rep->setProperty("router", router);
    rep->setProperty("caller_name", this->uiw->lineEdit->text());
    rep->setProperty("callee_phone", num);
}

void TvoSky::onPresendFinished()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<sender()<<QDateTime::currentDateTime();

    QString router;
    QString arouter; // auto router from server
    QString caller_name;
    QString callee_phone;
    QStringList resp_lines;
    
    QNetworkReply *rep = (QNetworkReply*)sender();
    QByteArray ba = this->mPresendResult.value(rep);

    router = rep->property("router").toString();
    caller_name = rep->property("caller_name").toString();
    callee_phone = rep->property("callee_phone").toString();
    
    this->mPresendResult.remove(rep);

    resp_lines = QString(ba).trimmed().split('\n');
    qDebug()<<"presend response:"<<ba<<rep->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    // qDebug()<<resp_lines;
    switch(rep->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
    case 200:
        if (ba.startsWith("200 ")) {
            if (resp_lines.count() >= 3) {
                arouter = resp_lines.at(2).trimmed();
            }
            if (!arouter.isEmpty()) {
                this->onCallSkype(arouter);
            } else {
                this->onCallSkype(router);
            }
        } else {
            qDebug()<<"presend call pair server faild:";
        }
        break;
    default:
        qDebug()<<"presend call pair network faild:";
        break;
    }
}

void TvoSky::onPresendReadyRead()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<sender();

    QNetworkReply *rep = (QNetworkReply*)sender();
    QByteArray ba = rep->readAll();

    this->mPresendResult[rep] += ba;
}


void TvoSky::onSkypekitBackendStarted()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    ::gskype = new CSkype();
    ::gskype->main_win = this;
    QObject::connect(::gskype, SIGNAL(conversationListChanged(PReference *, int, bool)),
                     this, SLOT(onConversationListChanged(PReference *, int, bool)));

    // QString login_name = this->uiw->lineEdit->text();
    // QString login_pwd = this->uiw->lineEdit_2->text();    

    AsyncSkyRunner *runner = new AsyncSkyRunner();
    runner->login_name = this->uiw->lineEdit->text();
    runner->login_password = this->uiw->lineEdit_2->text();
    runner->cmdno = AsyncSkyRunner::AR_LOGIN;
    runner->skywin = this;
    runner->start();
}

void TvoSky::onSkypekitBackendFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<exitCode<<exitStatus;
}

void TvoSky::onSkypekitBackendError(QProcess::ProcessError error)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<error;
}

void TvoSky::onSkypekitBackendReadyReadStandardError()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    QByteArray ba = this->skproc->readAllStandardError();
    this->appendLog(QString(ba));
}

void TvoSky::onSkypekitBackendReadyReadStandardOutput()
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__;
    QByteArray ba = this->skproc->readAllStandardOutput();
    this->appendLog(QString(ba));
}

void TvoSky::onSkypekitBackendStateChanged( QProcess::ProcessState newState)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<newState;
}


/////////////////////////////////////////
/////////////
/////////////////////////////////////////
AsyncSkyRunner::AsyncSkyRunner(QObject *parent)
    : QThread(parent)
{
}
AsyncSkyRunner::~AsyncSkyRunner()
{
}

void AsyncSkyRunner::run()
{
    if (this->cmdno == AR_LOGIN) {
        Sid::TransportInterface::Status status;
        status = ::gskype->init((const char*)apptoken, (const char*)inetAddr, portNum, log_streams);
        if (status != Sid::TransportInterface::OK) {
            printf("\n::: Error connecting to skypekit, enter 'r' to reconnect...\n");
            // this->appendLog("\n::: Error connecting to skypekit, enter 'r' to reconnect...\n");
            return;
        }
        
        ::gskype->start();

        // mobileSetup();

        Sid::String version;
        ::gskype->GetVersionString(version);
        printf("%s\n\n", (const char*) version);
        // this->appendLog((const char*) version);

        SEString MyAccountName = this->login_name.toAscii().data();
        bool ok = ::gskype->GetAccount(MyAccountName, ::gskype->activeAccount);
        Q_ASSERT(ok);
        // move to skype
        // QObject::connect(::gskype->activeAccount->self(), SIGNAL(loginStateChanged(int)),
        //                  this->skywin, SLOT(onLoginStateChanged(int)));

        SEString MyAccountPwd = this->login_password.toAscii().data();
        ok = ::gskype->activeAccount->LoginWithPassword(MyAccountPwd, false, true);
        // this->appendLog(QString("logining skype: %1\n").arg(login_name));
    }
}
