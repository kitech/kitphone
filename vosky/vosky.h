#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtNetwork>
#include <QMainWindow>

namespace Ui {
    class KitPhone;
}

class Skype;
class SkypeTunnel;
class SkypeTracer;

class PJSipEventThread : public QThread
{
    Q_OBJECT;
public: 
    PJSipEventThread(QObject *parent = 0);
    virtual ~PJSipEventThread();
    
    void run();

private:
    bool quit_loop;
    
};

class KitPhone : public QMainWindow
{
    Q_OBJECT;

public:
    explicit KitPhone(QWidget *parent = 0);
    virtual ~KitPhone();

public slots: // sip

public slots:    // pstn
    void defaultPstnInit();
    void onInitPstnClient();
    void onCallPstn();
    void onHangupPstn();
    void onShowSkypeTracer();
    
    void onConnectApp2App();

private:
    Ui::KitPhone *ui;

private: // sip

private: // pstn
    Skype *mSkype;
    SkypeTunnel *mtun;
    SkypeTracer *mSkypeTracer;
};

#endif // MAINWINDOW_H
