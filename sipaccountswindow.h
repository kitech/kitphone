// sipaccountswindow.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:25:31 +0800
// Version: $Id: sipaccountswindow.h 876 2011-05-11 14:25:21Z drswinghead $
// 
#ifndef _SIPACCOUNTSWINDOW_H_
#define _SIPACCOUNTSWINDOW_H_

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "boost/smart_ptr.hpp"

#include "sipaccount.h"

class AsyncDatabase;
class SqlRequest;

namespace Ui {
    class SipAccountsWindow;
}

class SipAccountsWindow : public QDialog
{
    Q_OBJECT;
public:
    explicit SipAccountsWindow(boost::shared_ptr<AsyncDatabase> adb, QWidget *parent = 0);
    virtual ~SipAccountsWindow();

public slots:
    void onClose();
    void onApply();

    void onNewAccount();
    void onRemoveAccount();
    void onModifyAccount();
    void onMakeDefaultAccount();

    void onSetLogin(bool checked);

    void onGetAllAccounts();
    void onGetAccountById(int uid);

    bool onNewAccountDone(boost::shared_ptr<SqlRequest> req);
    bool onRemoveAccountDone(boost::shared_ptr<SqlRequest> req);
    bool onModifyAccountDone(boost::shared_ptr<SqlRequest> req);
    bool onMakeDefaultAccountDone(boost::shared_ptr<SqlRequest> req);
    bool onGetAllAccountsDone(boost::shared_ptr<SqlRequest> req);

    bool onAccountListArrived(const QList<QSqlRecord> & results);

    // database exec callbacks
    void onSqlExecuteDone(const QList<QSqlRecord> & results, int reqno, bool eret, 
                          const QString &estr, const QVariant &eval);

public:
    // void reload();

private:
    SipAccount accountFromRow(int row);    

signals:
    void accountWantRegister(QString userName, bool reg);
    // void accountWantRemove(QString userName);
    void accountAdded(SipAccount &acc);
    // void accountModified(SipAccount &acc); // 删掉，添加两步
    void accountRemoved(SipAccount &acc);

private:
    Ui::SipAccountsWindow *uiw;

    boost::shared_ptr<AsyncDatabase> m_adb;
    //// sql reqno <---> sql reqclass
    QHash<int, boost::shared_ptr<SqlRequest> > mRequests;

    bool m_list_inited;
};

#endif /* _SIPACCOUNTSWINDOW_H_ */
