#ifndef _XPAPP_H_
#define _XPAPP_H_


#include <QCoreApplication>
// #include <QtSingleCoreApplication>  // add include_once in this file
#include <qtsinglecoreapplication.h>

class XpApp : public QtSingleCoreApplication
{
    Q_OBJECT;

public:
    XpApp(int & argc, char ** argv);
    virtual ~XpApp();

public slots:
    void handleMessage(const QString &msg);
    void handleMessage(const QString &msg, int client_id);

    void clientAck(int client_id);
private:
    
};


#endif
