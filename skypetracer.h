// skypetracer.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-07-01 19:18:37 +0800
// Version: $Id: skypetracer.h 154 2010-07-04 14:24:29Z drswinghead $
// 
#ifndef _SKYPETRACER_H_
#define _SKYPETRACER_H_

#include <QtCore>
#include <QtGui/QDialog>
#include <QtGui/QListWidgetItem>

// #include "ui_skypetracer.h"
namespace Ui {
    class SkypeTracer;
};

class SkypeTracer : public QDialog
{
    Q_OBJECT;
public:
    SkypeTracer(QWidget *parent = 0);
    virtual ~SkypeTracer();

public slots:
    void onCommandRequest(QString cmd);
    void onCommandResponse(QString skype_name, QString cmd);

signals:
    void commandRequest(QString cmd);

private slots:
    void onSendRequest();
    void onCommandTemplateSelected(QListWidgetItem *item);
    void onCompleteRequest(const QString &text);

private:
    // Ui::SkypeTracer uiw;
    Ui::SkypeTracer *uiw;
};

#endif /* _SKYPETRACER_H_ */
