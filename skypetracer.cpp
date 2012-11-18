// skypetracer.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-07-01 19:18:54 +0800
// Version: $Id: skypetracer.cpp 154 2010-07-04 14:24:29Z drswinghead $
// 

#include <QtGui>

#include "skypetracer.h"

#include "ui_skypetracer.h"

SkypeTracer::SkypeTracer(QWidget *parent)
    : QDialog(parent)
{
    this->uiw = new Ui::SkypeTracer();
    this->uiw->setupUi(this);

    QObject::connect(this->uiw->comboBox, SIGNAL(editTextChanged(const QString &)),
                     this, SLOT(onCompleteRequest(const QString &)));
    QObject::connect(this->uiw->pushButton_8, SIGNAL(clicked()),
                     this, SLOT(onSendRequest()));
    QObject::connect(this->uiw->listWidget, SIGNAL(itemClicked(QListWidgetItem *)),
                     this, SLOT(onCommandTemplateSelected(QListWidgetItem *)));
}

SkypeTracer::~SkypeTracer()
{
    delete this->uiw;
}

void SkypeTracer::onCommandRequest(QString cmd)
{
    qDebug()<<__FILE__<<__LINE__<<cmd;
    this->uiw->textBrowser->append("<P><FONT COLOR=blue>" + cmd.trimmed() + "</FONT></P>");
}

void SkypeTracer::onCommandResponse(QString skype_name, QString cmd)
{
    qDebug()<<__FILE__<<__LINE__<<cmd;
    this->uiw->textBrowser->append("<P> -) " + cmd.trimmed() + "</P>");
}

void SkypeTracer::onSendRequest()
{
    QString cmd = this->uiw->comboBox->currentText();
    emit commandRequest(cmd);
}

void SkypeTracer::onCommandTemplateSelected(QListWidgetItem *item)
{
    QString str = item->text();
    this->uiw->comboBox->setEditText(str);
    int ltPos = str.indexOf(str, '<');
    int gtPos = str.indexOf(str, '>');

    this->uiw->comboBox->setFocus();
    if (ltPos >= 0 && gtPos >= 0) {
        QLineEdit *le = this->uiw->comboBox->lineEdit();
        le->setSelection(ltPos, gtPos);
    }

}

void SkypeTracer::onCompleteRequest(const QString &text)
{
    QListWidgetItem *item;
    for (int i = 0 ; i < this->uiw->listWidget->count(); ++i) {
        item = this->uiw->listWidget->item(i);
        if (item->text().startsWith(text)) {
            QObject::disconnect(this->uiw->listWidget, SIGNAL(itemClicked(QListWidgetItem *)),
                                this, SLOT(onCommandTemplateSelected(QListWidgetItem *)));
            this->uiw->listWidget->setCurrentRow(i);
            QObject::connect(this->uiw->listWidget, SIGNAL(itemClicked(QListWidgetItem *)),
                             this, SLOT(onCommandTemplateSelected(QListWidgetItem *)));
            break;
        }
    }
}
