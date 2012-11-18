#include <QtCore>

#include "xpapp.h"


XpApp::XpApp(int & argc, char ** argv)
    : QtSingleCoreApplication(argc, argv)
{

}

XpApp::~XpApp()
{

}


void XpApp::handleMessage(const QString &msg)
{
    if (this->isRunning()) {
        // i'am a client
        qDebug()<<">>> "<<msg.trimmed();
        if (msg == "rdone") {
            this->quit();
        }
    } else {
        // i'am a server instance
        qDebug()<<"I am running, you say:"<<msg;
    }
}

void XpApp::handleMessage(const QString &msg, int client_id)
{
    if (this->isRunning()) {
        // i'am a client
        qDebug()<<"app client recieved:"<<msg;
    } else {
        // i'am a server instance
        qDebug()<<"I am running, you "<<client_id<<" say:"<<msg;
    }
}

void XpApp::clientAck(int client_id)
{
    this->sendMessage("rdone", client_id);
}
