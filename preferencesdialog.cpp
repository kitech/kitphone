// preferencesdialog.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-08-25 09:58:44 +0000
// Version: $Id$
// 
#include <QtCore>

#include "preferences.h"

#include "ui_preferencesdialog.h"
#include "preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    ,uiw(new Ui::PreferencesDialog())
{
    this->uiw->setupUi(this);

    // Setup buttons
    okButton = this->uiw->buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = this->uiw->buttonBox->button(QDialogButtonBox::Cancel);
    applyButton = this->uiw->buttonBox->button(QDialogButtonBox::Apply);
    helpButton = this->uiw->buttonBox->button(QDialogButtonBox::Help);

    /////////
    QObject::connect(this->uiw->listWidget, SIGNAL(currentRowChanged(int)),
                     this, SLOT(onChangeItem(int)));

    QListWidgetItem *item;
    for (int i = 0; i < this->uiw->listWidget->count(); ++i) {
        item = this->uiw->listWidget->item(i);
        QSize is = item->sizeHint();
        is.setHeight(35);
        item->setSizeHint(is);
    }
}

PreferencesDialog::~PreferencesDialog()
{

}

void PreferencesDialog::getData(Preferences *prefs)
{
    Q_ASSERT(prefs != NULL);

    prefs->use_tls = this->uiw->checkBox->isChecked();
    prefs->sip_protocol = this->uiw->comboBox_4->currentIndex();
    int nat_idx = this->uiw->comboBox_6->currentIndex();
    prefs->use_stun = (nat_idx == 1);
    prefs->use_turn = (nat_idx == 2);
    prefs->use_ice = (nat_idx == 3);
    prefs->stun_server = this->uiw->lineEdit->text().toStdString();

    prefs->use_codec = this->uiw->comboBox_5->currentText().toStdString();

    prefs->skype_mode = this->uiw->checkBox_2->isChecked();

}

void PreferencesDialog::setData(Preferences *prefs)
{
    Q_ASSERT(prefs != NULL);
    
    this->uiw->checkBox->setChecked(prefs->use_tls);
    this->uiw->comboBox_4->setCurrentIndex(prefs->sip_protocol);
    int nat_idx = 0;
    if (prefs->use_ice) nat_idx = 3;
    if (prefs->use_turn) nat_idx = 2;
    if (prefs->use_stun) nat_idx = 1;
    this->uiw->comboBox_6->setCurrentIndex(nat_idx);
    this->uiw->lineEdit->setText(QString::fromStdString(prefs->stun_server));

    int contains_use_codec = -1;
    for (int i = 0; i < prefs->media_codecs.size(); ++i) {
        this->uiw->comboBox_5->addItem(QString::fromStdString(prefs->media_codecs.at(i)));
        if (prefs->media_codecs.at(i) == prefs->use_codec) {
            contains_use_codec = i;
        }
    }

    if (prefs->use_codec.length() > 0) {
        if (contains_use_codec >= 0) {
            this->uiw->comboBox_5->setCurrentIndex(contains_use_codec);
        }
    }

    this->uiw->checkBox_2->setChecked(prefs->skype_mode);
}

void PreferencesDialog::onChangeItem(int idx)
{
    this->uiw->stackedWidget->setCurrentIndex(idx);
}

void PreferencesDialog::apply()
{

}

void PreferencesDialog::showHelp()
{

}


