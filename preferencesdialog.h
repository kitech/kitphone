// preferencesdialog.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-08-25 09:58:32 +0000
// Version: $Id$
// 

#ifndef _PREFERENCESDIALOG_H_
#define _PREFERENCESDIALOG_H_

#include <QtGui>

class Preferences;

namespace Ui {
    class PreferencesDialog;
};

class PreferencesDialog : public QDialog
{
    Q_OBJECT;
public:
    explicit PreferencesDialog(QWidget *parent = 0);
    virtual ~PreferencesDialog();

    void getData(Preferences *prefs);
    void setData(Preferences *prefs);

protected slots:
    void onChangeItem(int idx);
    void apply();
    void showHelp();

private:
    Ui::PreferencesDialog *uiw;

    QPushButton * okButton;
    QPushButton * cancelButton;
    QPushButton * applyButton;
    QPushButton * helpButton;
    
};

#endif /* _PREFERENCESDIALOG_H_ */
