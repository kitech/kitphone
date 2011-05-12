// sipaccountswindow.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:28:31 +0800
// Version: $Id: sipaccountswindow.cpp 876 2011-05-11 14:25:21Z drswinghead $
// 

#include "boost/signals2.hpp"

#include "simplelog.h"
#include "asyncdatabase.h"

#include "sipaccount.h"

#include "sipaccountswindow.h"
#include "ui_sipaccountswindow.h"

#include "sipaccountpropertieswindow.h"

SipAccountsWindow::SipAccountsWindow(boost::shared_ptr<AsyncDatabase> adb, QWidget *parent)
    : QDialog(parent)
    , uiw(new Ui::SipAccountsWindow)
{
    this->uiw->setupUi(this);

    this->m_list_inited = false;
    this->m_adb = adb;
    QObject::connect(this->m_adb.get(), SIGNAL(results(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)),
                     this, SLOT(onSqlExecuteDone(const QList<QSqlRecord>&, int, bool, const QString&, const QVariant&)));

    QObject::connect(this->uiw->pushButton_6, SIGNAL(clicked()),
                     this, SLOT(onClose()));
    QObject::connect(this->uiw->pushButton_5, SIGNAL(clicked()),
                     this, SLOT(onApply()));
    QObject::connect(this->uiw->pushButton_4, SIGNAL(clicked()),
                     this, SLOT(onNewAccount()));
    QObject::connect(this->uiw->pushButton_3, SIGNAL(clicked()),
                     this, SLOT(onRemoveAccount()));
    QObject::connect(this->uiw->pushButton_2, SIGNAL(clicked()),
                     this, SLOT(onModifyAccount()));
    QObject::connect(this->uiw->pushButton, SIGNAL(clicked()),
                     this, SLOT(onMakeDefaultAccount()));
    
    // this->reload();
    this->uiw->tableWidget->setColumnWidth(0, 60);
    this->uiw->tableWidget->setColumnWidth(1, 60);
    this->uiw->tableWidget->setColumnWidth(2, 130);
    this->uiw->tableWidget->setColumnWidth(3, 200);
    this->uiw->tableWidget->setColumnWidth(4, 200);
    this->uiw->tableWidget->setColumnWidth(5, 200);
    this->uiw->tableWidget->setColumnWidth(6, 100);

    QTimer::singleShot(20, this, SLOT(onGetAllAccounts()));
}

SipAccountsWindow::~SipAccountsWindow()
{

}

void SipAccountsWindow::onClose()
{
    this->reject();
}

void SipAccountsWindow::onApply()
{
    this->accept();
}

void SipAccountsWindow::onNewAccount()
{
    boost::shared_ptr<SqlRequest> req(new SqlRequest());

    SipAccount acc;
    boost::scoped_ptr<SipAccountPropertiesWindow> prop_win(new SipAccountPropertiesWindow(acc, this));
    if (prop_win->exec() == QDialog::Accepted) {
        acc = prop_win->getAccount();
        QDateTime nowtime = QDateTime::currentDateTime();

        req->mCbFunctor = boost::bind(&SipAccountsWindow::onNewAccountDone, this, _1);
        req->mCbObject = this;
        req->mCbSlot = SLOT(onNewAccountDone(boost::shared_ptr<SqlRequest>));
        req->mSql = QString("INSERT INTO kp_accounts (account_name, account_password, display_name, serv_addr, account_status, account_ctime, account_mtime) VALUES ('%1', '%2', '%3', '%4' , '%5', '%6', '%6')")
            .arg(acc.userName).arg(acc.password).arg(acc.userName)
            .arg(acc.domain).arg(0)
            .arg(nowtime.toString());
        req->mReqno = this->m_adb->execute(req->mSql);
        this->mRequests.insert(req->mReqno, req);

        // acc.save(acc);
    }
    // delete prop_win;

    // if (!acc.userName.isEmpty()) {
    //     this->reload();
    // }
}
void SipAccountsWindow::onRemoveAccount()
{
    int row;
    QString user_name;
    SipAccount acc;

    if (this->uiw->tableWidget->selectedItems().count() == 0) {
        return;
    }
    row = this->uiw->tableWidget->currentRow();
    qDebug()<<"current row:"<<row;
    user_name = this->uiw->tableWidget->item(row, 2)->text();

    int uid = this->uiw->tableWidget->item(row, 1)->text().toInt();

    boost::shared_ptr<SqlRequest> req(new SqlRequest());

    req->mCbId = uid;
    req->mCbFunctor = boost::bind(&SipAccountsWindow::onRemoveAccountDone, this, _1);
    req->mCbObject = this;
    req->mCbSlot = SLOT(onRemoveAccountDone(boost::shared_ptr<SqlRequest>));
    req->mSql = QString("DELETE FROM kp_accounts WHERE aid=%1").arg(uid);
    req->mReqno = this->m_adb->execute(req->mSql);
    this->mRequests.insert(req->mReqno, req);

    // acc.remove(user_name);
    // this->reload();

    // emit this->accountWantRemove(user_name);
}

void SipAccountsWindow::onModifyAccount()
{
    int row;
    QString user_name;
    SipAccount acc;

    if (this->uiw->tableWidget->selectedItems().count() == 0) {
        return;
    }
    row = this->uiw->tableWidget->currentRow();
    // qDebug()<<"current row:"<<row;
    acc = this->accountFromRow(row);

    boost::scoped_ptr<SipAccountPropertiesWindow> prop_win(new SipAccountPropertiesWindow(acc, this));
    // SipAccountPropertiesWindow *prop_win = new SipAccountPropertiesWindow(acc);
    if (prop_win->exec() == QDialog::Accepted) {
        acc = prop_win->getAccount();

        QDateTime nowtime = QDateTime::currentDateTime();

        boost::shared_ptr<SqlRequest> req(new SqlRequest());        
        req->mCbId = acc.uid;
        req->mCbFunctor = boost::bind(&SipAccountsWindow::onModifyAccountDone, this, _1);
        req->mCbObject = this;
        req->mCbSlot = SLOT(onModifyAccountDone(boost::shared_ptr<SqlRequest>));
        req->mSql = QString("UPDATE kp_accounts SET account_name='%1', account_password='%2', display_name='%3', serv_addr='%4', account_status='%5', account_mtime='%6' WHERE aid=%7")
            .arg(acc.userName).arg(acc.password).arg(acc.userName)
            .arg(acc.domain).arg(0)
            .arg(nowtime.toString()).arg(acc.uid);
        req->mReqno = this->m_adb->execute(req->mSql);
        this->mRequests.insert(req->mReqno, req);

        // if (acc.userName != user_name) {
        //     acc.remove(user_name);
        //     emit this->accountWantRemove(user_name);
        // }
        // acc.save(acc);
    }
    // delete prop_win;

    // if (!acc.userName.isEmpty()) {
    //     this->reload();
    // }
}

void SipAccountsWindow::onMakeDefaultAccount()
{
}

// depcreated
// void SipAccountsWindow::reload()
// {
//     QTableWidgetItem *item;
//     QVector<SipAccount> accs;
//     SipAccount acc;
//     QCheckBox *cbox;

//     // this->uiw->tableWidget->clear();
//     for (int i = this->uiw->tableWidget->rowCount() - 1; i >= 0 ; --i) {
//         this->uiw->tableWidget->removeRow(i);    
//     }
//     accs = SipAccount().listAccounts();
//     // qDebug()<<accs.count();
//     for (int i = 0 ; i < accs.count() ; i++) {
//         acc = accs.at(i);
//         this->uiw->tableWidget->insertRow(i);
//         cbox = new QCheckBox();
//         this->uiw->tableWidget->setCellWidget(i, 0, cbox);
//         cbox->setChecked(false);
//         QObject::connect(cbox, SIGNAL(toggled(bool)),
//                          this, SLOT(onSetLogin(bool)));

//         this->uiw->tableWidget->setItem(i, 1, new QTableWidgetItem());
//         this->uiw->tableWidget->item(i, 1)->setText(QString::number(i+1));
//         item = this->uiw->tableWidget->item(i, 2);
//         item = new QTableWidgetItem();
//         item->setText(acc.userName);
//         this->uiw->tableWidget->setItem(i, 2, item);
//         item = this->uiw->tableWidget->item(i, 3);
//         item = new QTableWidgetItem();
//         item->setText(acc.domain);
//         this->uiw->tableWidget->setItem(i, 3, item);
//     }
// }

void SipAccountsWindow::onSetLogin(bool checked)
{
    qDebug()<<__FILE__<<__LINE__<<checked;
    QWidget *cbox = static_cast<QWidget*>(sender());
    QWidget *tbox;
    QString user_name;

    for (int i = 0 ; i < this->uiw->tableWidget->rowCount(); i ++) {
        tbox = this->uiw->tableWidget->cellWidget(i, 0);
        if (cbox == tbox) {
            // 
            user_name = this->uiw->tableWidget->item(i, 2)->text();
            emit this->accountWantRegister(user_name, checked);
            break;
        }
    }

    qLogx()<<"set login: "<<user_name<<checked;
}

SipAccount SipAccountsWindow::accountFromRow(int row)
{
    SipAccount acc;

    acc.uid = this->uiw->tableWidget->item(row, 1)->text().toInt();
    acc.userName = this->uiw->tableWidget->item(row, 2)->text();
    acc.displayName = acc.userName;
    acc.domain = this->uiw->tableWidget->item(row, 3)->text();
    acc.password = this->uiw->tableWidget->item(row, 6)->text();

    return acc;
}

void SipAccountsWindow::onGetAllAccounts()
{
    boost::shared_ptr<SqlRequest> req(new SqlRequest());


    req->mCbFunctor = boost::bind(&SipAccountsWindow::onNewAccountDone, this, _1);
    req->mCbObject = this;
    req->mCbSlot = SLOT(onNewAccountDone(boost::shared_ptr<SqlRequest>));
    req->mSql = QString("SELECT * FROM kp_accounts");
    req->mReqno = this->m_adb->execute(req->mSql);
    this->mRequests.insert(req->mReqno, req);
}

void SipAccountsWindow::onGetAccountById(int uid)
{
    boost::shared_ptr<SqlRequest> req(new SqlRequest());

    req->mCbFunctor = boost::bind(&SipAccountsWindow::onGetAllAccountsDone, this, _1);
    req->mCbObject = this;
    req->mCbSlot = SLOT(onGetAllAccountsDone(boost::shared_ptr<SqlRequest>));
    req->mSql = QString("SELECT * FROM kp_accounts WHERE aid=%1").arg(uid);
    req->mReqno = this->m_adb->execute(req->mSql);
    this->mRequests.insert(req->mReqno, req);
}

bool SipAccountsWindow::onGetAllAccountsDone(boost::shared_ptr<SqlRequest> req)
{
    qLogx()<<req<<req->mRet<<req->mReqno;
    this->onAccountListArrived(req->mResults);

    this->mRequests.remove(req->mReqno);
    return true;
}

bool SipAccountsWindow::onAccountListArrived(const QList<QSqlRecord> & results)
{
    SipAccount acc;
    QSqlRecord rec;

    // qLogx()<<results;
    for (int i = 0; i < results.count(); ++i) {
        rec = results.at(i);
        acc = SipAccount::fromSqlRecord(rec);
        int row_count = this->uiw->tableWidget->rowCount();
        QCheckBox *cbox;
        QTableWidgetItem *item;

        this->uiw->tableWidget->insertRow(row_count);
        cbox = new QCheckBox();
        this->uiw->tableWidget->setCellWidget(row_count, 0, cbox);
        cbox->setChecked(false);
        QObject::connect(cbox, SIGNAL(toggled(bool)),
                         this, SLOT(onSetLogin(bool)));

        this->uiw->tableWidget->setItem(row_count, 1, new QTableWidgetItem());
        // this->uiw->tableWidget->item(row_count, 1)->setText(QString::number(row_count+1));
        this->uiw->tableWidget->item(row_count, 1)->setText(QString::number(acc.uid));
        item = this->uiw->tableWidget->item(row_count, 2);
        item = new QTableWidgetItem();
        item->setText(acc.userName);
        this->uiw->tableWidget->setItem(row_count, 2, item);
        item = this->uiw->tableWidget->item(row_count, 3);
        item = new QTableWidgetItem();
        item->setText(acc.domain);
        this->uiw->tableWidget->setItem(row_count, 3, item);

        item = new QTableWidgetItem();
        item->setText(acc.ctime);
        this->uiw->tableWidget->setItem(row_count, 4, item);

        item = new QTableWidgetItem();
        item->setText(acc.mtime);
        this->uiw->tableWidget->setItem(row_count, 5, item);

        item = new QTableWidgetItem();
        item->setText(acc.password);
        this->uiw->tableWidget->setItem(row_count, 6, item);

        if (this->m_list_inited == true) {
            emit this->accountAdded(acc);
        }
        // acc.dump();
    }

    if (!this->m_list_inited) {
        this->m_list_inited = true;
    }

    return true;
}

bool SipAccountsWindow::onNewAccountDone(boost::shared_ptr<SqlRequest> req)
{
    qLogx()<<req;

    QSqlRecord rec;
    QList<QSqlRecord> results = req->mResults;

    bool bok = this->onAccountListArrived(results);

    // rec = results.at(0);
    // SipAccount acc = SipAccount::fromSqlRecord(rec);
    // emit this->accountAdded(acc);

    this->mRequests.remove(req->mReqno);

    return true;
}

bool SipAccountsWindow::onRemoveAccountDone(boost::shared_ptr<SqlRequest> req)
{
    qLogx()<<req<<req->mRet<<req->mReqno;

    QWidget *tbox;
    QString user_name;
    QTableWidgetItem *item;
    SipAccount acc;

    for (int i = 0 ; i < this->uiw->tableWidget->rowCount(); i ++) {
        // tbox = this->uiw->tableWidget->cellWidget(i, 0);
        // if (cbox == tbox) {
        //   
        item = this->uiw->tableWidget->item(i, 1);
        if (req->mCbId == item->text().toInt()) {
            acc = this->accountFromRow(i);
            this->uiw->tableWidget->removeRow(i);
            emit this->accountRemoved(acc);
            break;
        }
    }

    return true;
}

bool SipAccountsWindow::onModifyAccountDone(boost::shared_ptr<SqlRequest> req)
{
    QWidget *tbox;
    QString user_name;
    QTableWidgetItem *item;
    SipAccount acc;

    for (int i = 0 ; i < this->uiw->tableWidget->rowCount(); i ++) {
        // tbox = this->uiw->tableWidget->cellWidget(i, 0);
        // if (cbox == tbox) {
        //   
        item = this->uiw->tableWidget->item(i, 1);
        if (req->mCbId == item->text().toInt()) {
            acc = this->accountFromRow(i);
            this->uiw->tableWidget->removeRow(i);
            emit this->accountRemoved(acc);
            break;
        }
    }

    this->mRequests.remove(req->mReqno);

    this->onGetAccountById(req->mCbId);

    return true;
}

bool SipAccountsWindow::onMakeDefaultAccountDone(boost::shared_ptr<SqlRequest> req)
{

    return true;
}

void SipAccountsWindow::onSqlExecuteDone(const QList<QSqlRecord> & results, int reqno, bool eret, const QString &estr, const QVariant &eval)
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
