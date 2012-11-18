// aboutdialog.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2012 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-18 22:29:06 +0800
// Version: $Id$
// 

#ifndef _ABOUTDIALOG_H_
#define _ABOUTDIALOG_H_

#include <QtCore>
#include <QtGui>

namespace Ui {
    class AboutDialog;
};

class AboutDialog : public QDialog
{
    Q_OBJECT;
public:
    explicit AboutDialog(QWidget *parent = 0);
    virtual ~AboutDialog();

private:
    Ui::AboutDialog *uiw;
};

#endif /* _ABOUTDIALOG_H_ */
