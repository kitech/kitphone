#include <QtCore>
#include <QtGui>

#include "vosky.h"
#include "ui_vosky.h"


KitPhone::KitPhone(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KitPhone)
{
    ui->setupUi(this);

    this->defaultPstnInit();
}

KitPhone::~KitPhone()
{
    delete ui;
}


////////////////////////////////
PJSipEventThread::PJSipEventThread(QObject *parent)
    : QThread(parent)
{
    this->quit_loop = false;
}

PJSipEventThread::~PJSipEventThread()
{
    
}

void PJSipEventThread::run()
{
}

