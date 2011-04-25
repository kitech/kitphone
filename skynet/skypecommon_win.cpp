/***************************************************************
 * skypeX11.cpp
 * @Author:      Jonathan Verner (jonathan.verner@matfyz.cz)
 * @License:     GPL v2.0 or later
 * @Created:     2008-05-14.
 * @Last Change: 2008-05-14.
 * @Revision:    $Id: skypecommon_win.cpp 180 2010-11-06 04:39:16Z drswinghead $
 * Description:
 * Usage:
 * TODO:
 * CHANGES:
 ***************************************************************/
#include <QtGui/QApplication>
#include <QtCore>
#include "skypecommon.h"

#ifdef Q_WS_WIN

enum {
  SKYPE_ATTACH_SUCCESS=0,
  SKYPE_TRY_NOW=0x8001,
  SKYPE_REFUSED=2,
  SKYPE_PENDING_AUTHORIZATION=1,
  SKYPE_TRY_AGAIN=3
};

UINT SkypeCommon::attachMSG = 0;
UINT SkypeCommon::discoverMSG = 0;
QWidget *SkypeCommon::mainWin = NULL;
WId SkypeCommon::main_window = 0;

SkypeCommon::SkypeCommon() { 
    if ( attachMSG == 0 || discoverMSG == 0 ) { 
        // attachMSG = RegisterWindowMessage((LPCWSTR)"SkypeControlAPIAttach");
        // discoverMSG = RegisterWindowMessage((LPCWSTR)"SkypeControlAPIDiscover");
        // attachMSG = RegisterWindowMessageA("SkypeControlAPIAttach");
        // discoverMSG = RegisterWindowMessageA("SkypeControlAPIDiscover");
        wchar_t *sa = L"SkypeControlAPIAttach";
        wchar_t *sb = L"SkypeControlAPIDiscover";
        attachMSG = RegisterWindowMessage(sa);
        discoverMSG = RegisterWindowMessage(sb);
    }
    if ( mainWin == NULL ) {
        mainWin = new QWidget();
        main_window = mainWin->winId();
    }

    connect( qApp, SIGNAL( winMessage( MSG *) ), this, SLOT( processWINMessage( MSG *) ) );
    skype_win=0;
    connected = false;
    refused = false;
    tryLater = false;
    // TimeOut = 10000;
    TimeOut = 1000 * 100;
}
SkypeCommon::~SkypeCommon()
{

}


bool SkypeCommon::sendMsgToSkype(const QString &message) {
    COPYDATASTRUCT copyData;
    QByteArray tmp;
    // qDebug()<<"SENDING MESSAGE:"<<message;

    if ( refused || tryLater ) return false;
    if ( ! connected ) return attachToSkype();
    if ( ! connected ) return false;
  
    tmp.append(message);


    copyData.dwData=0;
    copyData.lpData=tmp.data();
    copyData.cbData=tmp.size()+1;

    SendMessage( skype_win, WM_COPYDATA, (WPARAM) main_window, (LPARAM) &copyData );
    // qDebug()<<"MESSAGE SENT:"<<message<<" to"<<skype_win;

    return true;
}

bool SkypeCommon::attachToSkype() {
    if ( connected ) return true;
    if ( refused || tryLater ) return false;
    waitingForConnect = true;
    SendMessage( HWND_BROADCAST, discoverMSG, (WPARAM) main_window, 0 );
    QTimer *timer = new QTimer(this);
    QTimer::singleShot(TimeOut, this, SLOT(timeOut()));
    int result = localEventLoop.exec();
    waitingForConnect = false;
    return connected;
    return false;
}

void SkypeCommon::timeOut()
{
    if ( waitingForConnect ) localEventLoop.exit(1);
}


// for skype
extern bool eventHandled;
extern long eventResult;


void SkypeCommon::processWINMessage( MSG *msg )
{
    QByteArray tmp;
    char *data=NULL;
    COPYDATASTRUCT *copyData;
    // qDebug() << "ProcessWINMessage:" << msg->message;
    // application::eventHandled=true;
    //  application::eventResult=1;
    // qApp->eventHandled = true;
    // qApp->eventResult = 1;
    ::eventHandled = true;
    ::eventResult = 1;

    switch ( msg->message ) { 
    case WM_COPYDATA:
        if ( skype_win != (WId) msg->wParam ) {
		    qDebug() << "Message not from skype";
		    return;
        }
        copyData = (COPYDATASTRUCT *)msg->lParam;
        data = new char[ copyData->cbData ];
        data = qstrncpy( data, (char *) copyData->lpData, copyData->cbData );
        tmp.append(data);
        Q_ASSERT( data != NULL );
        delete data;
        // qDebug() << "WM_COPYDATA:" << tmp;
        emit newMsgFromSkype( tmp );
        return;
    default:
	    if ( msg->message == attachMSG ) {
            qDebug() << "Attach status";
            switch ( msg->lParam ) {
            case SKYPE_ATTACH_SUCCESS:
                connected=true;
                tryLater=false;
                skype_win = (WId) msg->wParam;
                qDebug() << "Attached to "<<skype_win;
                if ( waitingForConnect ) localEventLoop.quit();
                return;
            case SKYPE_TRY_NOW:
                qDebug() << "Try to attach now";
                tryLater=false;
                attachToSkype();
                return;
            case SKYPE_REFUSED:
                qDebug() << "Refused";
                refused=true;
                return;
            case SKYPE_PENDING_AUTHORIZATION:
                qDebug() << "Pending authorization";
                return;
            case SKYPE_TRY_AGAIN:
                qDebug() << "Try Again";
                tryLater=true;
                if ( waitingForConnect ) localEventLoop.quit();
                return;
            default: 
                qDebug() <<"WEIRD STATUS:"<<msg->lParam;
                return;
            }
		  
	    }
    }
    // application::eventHandled=false;
    // qApp->eventHandled = false;
    ::eventHandled = false;
}


// #include "SkypeCommon.moc"
#endif /* Q_WS_WIN */
