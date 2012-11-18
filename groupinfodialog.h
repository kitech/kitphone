// groupinfodialog.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-28 15:28:57 +0800
// Version: $Id$
// 

#ifndef _GROUPINFODIALOG_H_
#define _GROUPINFODIALOG_H_

#include <QtGui>

namespace Ui {
    class GroupInfoDialog;
};

class GroupInfoDialog : public QDialog
{
    Q_OBJECT;
public:
    explicit GroupInfoDialog(QWidget *parent = 0);
    virtual ~GroupInfoDialog();

    QString groupName();

private:
    Ui::GroupInfoDialog *uiw;
};

#endif /* _GROUPINFODIALOG_H_ */
