// sipaccountswindow.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-11-20 17:28:31 +0800
// Version: $Id: sipaccountswindow.cpp 803 2011-03-29 03:46:13Z drswinghead $
// 

#include "sipaccount.h"

#include "sipaccountswindow.h"
#include "ui_sipaccountswindow.h"

#include "sipaccountpropertieswindow.h"

SipAccountsWindow::SipAccountsWindow(QWidget *parent)
    : QDialog(parent),
      uiw(new Ui::SipAccountsWindow)
{
    this->uiw->setupUi(this);

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
    
    this->reload();
    this->uiw->tableWidget->setColumnWidth(0, 60);
    this->uiw->tableWidget->setColumnWidth(1, 60);
    this->uiw->tableWidget->setColumnWidth(2, 130);
    this->uiw->tableWidget->setColumnWidth(3, 200);
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
    SipAccount acc;
    SipAccountPropertiesWindow *prop_win = new SipAccountPropertiesWindow();
    if (prop_win->exec() == QDialog::Accepted) {
        acc = prop_win->getAccount();

        acc.save(acc);
    }
    delete prop_win;

    if (!acc.userName.isEmpty()) {
        this->reload();
    }
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

    acc.remove(user_name);
    this->reload();

    emit this->accountWantRemove(user_name);
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
    user_name = this->uiw->tableWidget->item(row, 2)->text();

    SipAccountPropertiesWindow *prop_win = new SipAccountPropertiesWindow(user_name);
    if (prop_win->exec() == QDialog::Accepted) {
        acc = prop_win->getAccount();
        if (acc.userName != user_name) {
            acc.remove(user_name);
            emit this->accountWantRemove(user_name);
        }
        acc.save(acc);
    }
    delete prop_win;

    if (!acc.userName.isEmpty()) {
        this->reload();
    }
}

void SipAccountsWindow::onMakeDefaultAccount()
{
}

void SipAccountsWindow::reload()
{
    QTableWidgetItem *item;
    QVector<SipAccount> accs;
    SipAccount acc;
    QCheckBox *cbox;

    // this->uiw->tableWidget->clear();
    for (int i = this->uiw->tableWidget->rowCount() - 1; i >= 0 ; --i) {
        this->uiw->tableWidget->removeRow(i);    
    }
    accs = SipAccount().listAccounts();
    // qDebug()<<accs.count();
    for (int i = 0 ; i < accs.count() ; i++) {
        acc = accs.at(i);
        this->uiw->tableWidget->insertRow(i);
        cbox = new QCheckBox();
        this->uiw->tableWidget->setCellWidget(i, 0, cbox);
        cbox->setChecked(false);
        QObject::connect(cbox, SIGNAL(toggled(bool)),
                         this, SLOT(onSetLogin(bool)));

        this->uiw->tableWidget->setItem(i, 1, new QTableWidgetItem());
        this->uiw->tableWidget->item(i, 1)->setText(QString::number(i+1));
        item = this->uiw->tableWidget->item(i, 2);
        item = new QTableWidgetItem();
        item->setText(acc.userName);
        this->uiw->tableWidget->setItem(i, 2, item);
        item = this->uiw->tableWidget->item(i, 3);
        item = new QTableWidgetItem();
        item->setText(acc.domain);
        this->uiw->tableWidget->setItem(i, 3, item);
    }
}

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

    qDebug()<<"set login: "<<user_name<<checked;
}
