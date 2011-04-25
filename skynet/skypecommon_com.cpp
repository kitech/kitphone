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

//#ifdef Q_WS_WIN_COM
#ifdef Q_WS_WIN

#include <QAxObject>

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

QAxObject *axo = NULL;

SkypeCommon::SkypeCommon() { 
    if ( attachMSG == 0 || discoverMSG == 0 ) { 
        // attachMSG = RegisterWindowMessage((LPCWSTR)"SkypeControlAPIAttach");
        // discoverMSG = RegisterWindowMessage((LPCWSTR)"SkypeControlAPIDiscover");
        // attachMSG = RegisterWindowMessageA("SkypeControlAPIAttach");
        // discoverMSG = RegisterWindowMessageA("SkypeControlAPIDiscover");

        // 还是得这种方法，应该什么编码的系统都行。
        wchar_t *sa = L"SkypeControlAPIAttach";
        wchar_t *sb = L"SkypeControlAPIDiscover";
        attachMSG = ::RegisterWindowMessage(sa);
        discoverMSG = ::RegisterWindowMessage(sb);
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
    TimeOut = 10000;
}
SkypeCommon::~SkypeCommon()
{

}


bool SkypeCommon::sendMsgToSkype(const QString &message) {

    Q_ASSERT(axo != NULL);
    QList<QVariant> vars;
    // vars<<100<<"PROTOCOL 100";
    vars << QVariant() << message;
    QAxObject *cmd_obj = axo->querySubObject("Command(int, QString, QString, bool, int)", vars);

    QVariant vret = axo->dynamicCall("SendCommand(IDispatch*)", cmd_obj->asVariant());
    qDebug()<<vret;

    // COPYDATASTRUCT copyData;
    // QByteArray tmp;
    qDebug()<<"SENDING MESSAGE:"<<message;

    // if ( refused || tryLater ) return false;
    // if ( ! connected ) return attachToSkype();
    // if ( ! connected ) return false;
  
    // tmp.append(message);


    // copyData.dwData=0;
    // copyData.lpData=tmp.data();
    // copyData.cbData=tmp.size()+1;

    // SendMessage( skype_win, WM_COPYDATA, (WPARAM) main_window, (LPARAM) &copyData );
    // qDebug()<<"MESSAGE SENT:"<<message<<" to"<<skype_win;

    return true;
}

QString com_dll_path() {
    return qApp->applicationDirPath() + "/Skype4COM.dll";
}

bool regsvr32_install()
{
    QString path = com_dll_path();

    QStringList args;
    // args << "/s" << "/i" << path;
    // args << "/s" << path;
    args << path;

    QProcess proc;
    proc.start("regsvr32", args);

    proc.waitForFinished();

    return true;
}

bool regsvr32_uninstall()
{
    QString path = com_dll_path();

    QStringList args;
    args << "/s" <<"/u" << path;

    QProcess proc;
    proc.start("regsvr32", args);

    proc.waitForFinished();

    return true;
}

bool SkypeCommon::attachToSkype() {
    
    int retry_times = 5;
    // 这种方式竟然又不能在线程中调用。在调用时界面直接没有响应了。
    do {
        // axo = new QAxObject("Skype4COM.Skype", 0);
        // 830690FC-BF2F-47A6-AC2D-330BCB402664
        // axo = new QAxObject("{830690FC-BF2F-47A6-AC2D-330BCB402664}", 0);
        axo = new QAxObject();
        QObject::connect(axo, SIGNAL(exception(int , const QString & , const QString & , const QString &)), 
                         this, SLOT(onComException(int , const QString & , const QString & , const QString &)));
        axo->setControl("{830690FC-BF2F-47A6-AC2D-330BCB402664}");
        if (axo->isNull()) {
            regsvr32_install();
        }
    } while (axo->isNull() && retry_times-- >= 0);
    if (axo->isNull()) {
        Q_ASSERT(!axo->isNull());
        return false;
    }
    
    // QObject::connect(axo, SIGNAL(Command(IDispatch*)), this, SLOT(onComCommand(IDispatch*)));
    QObject::connect(axo, SIGNAL(signal(const QString&,int,void *)),
                     this, SLOT(onComSignal(const QString&, int, void*)));

    // QString html_doc = axo->generateDocumentation();
    // QFile fp("a.html");
    // fp.open(QIODevice::WriteOnly);
    // fp.write(html_doc.toAscii());
    // fp.close();

    QVariant vret = axo->dynamicCall("Attach(int,bool)", QVariant(100));
    qDebug()<<axo->isNull()<<vret;

    return true;

    // if ( connected ) return true;
    // if ( refused || tryLater ) return false;
    // waitingForConnect = true;
    // SendMessage( HWND_BROADCAST, discoverMSG, (WPARAM) main_window, 0 );
    // QTimer *timer = new QTimer(this);
    // QTimer::singleShot(TimeOut, this, SLOT(timeOut()));
    // int result = localEventLoop.exec();
    // waitingForConnect = false;
    // return connected;
    // return false;
}

QString getIDispatchStringValue(IDispatch *pdisp, OLECHAR FAR* pname)
{
    Q_ASSERT(pdisp != NULL);
    QString str_val;

    DWORD   size;

    DISPID dispid;
    char *prop_name = "Command";
    // OLECHAR FAR* szMember = L"Command";
    // OLECHAR FAR* szMember = _com_util::ConvertStringToBSTR(prop_name);
    // _com_util::ConvertBSTRToString(szMember);
    // size = MultiByteToWideChar(CP_ACP, 0, (prop_name), -1, 0, 0);
    // OLECHAR FAR* szMember = SysAllocStringByteLen(prop_name, strlen(prop_name));
    OLECHAR FAR* szMember = pname;

    long hresult = pdisp->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid);
    // qDebug()<<hresult<<dispid;

    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    VARIANT varReault;
    VARIANT FAR *pVarResult = (VARIANT FAR*)&varReault;
    hresult = pdisp->Invoke(dispid, IID_NULL,
                           LOCALE_USER_DEFAULT,
                           DISPATCH_PROPERTYGET,
                           &dispparamsNoArgs, pVarResult, NULL, NULL);
    // qDebug()<<pVarResult<<hresult<<pVarResult->vt<<pVarResult->bstrVal;

    void    *strOut = NULL;
    BSTR strIn = pVarResult->bstrVal;
    // ANSI
    size = WideCharToMultiByte(CP_ACP, 0, (WCHAR *)((char *)strIn), -1, 0, 0, 0, 0);
    if (size > 1) {
        if ((strOut = GlobalAlloc(GMEM_FIXED, size))) {
            WideCharToMultiByte(CP_ACP, 0, (WCHAR *)((char *)strIn), -1, (char *)strOut, size, 0, 0);
        }

        qDebug()<<(char*)(strOut)<<QString((char*)strOut)<<size<<strlen((char*)strIn)
                <<hresult<<(pVarResult->vt == VT_BSTR)<<(pVarResult->vt == VT_EMPTY); // ok
    }
    str_val = QString((char*)(strOut));

    return str_val;
}

void SkypeCommon::onComCommand(IDispatch *pcmd)
{
    qDebug()<<__FILE__<<__LINE__<<pcmd;
    
    DISPID dispid;
    OLECHAR FAR* szMember = L"Command";
    long hresult = pcmd->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid);
    qDebug()<<hresult<<dispid;

    DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
    VARIANT varReault;
    VARIANT FAR *pVarResult = (VARIANT FAR*)&varReault;
    hresult = pcmd->Invoke(dispid, IID_NULL,
                           LOCALE_USER_DEFAULT,
                           DISPATCH_PROPERTYGET,
                           &dispparamsNoArgs, pVarResult, NULL, NULL);
    qDebug()<<pVarResult<<hresult<<pVarResult->vt<<pVarResult->bstrVal;

    DWORD   size;
    void    *strOut;
    BSTR strIn = pVarResult->bstrVal;
    // ANSI
    size = WideCharToMultiByte(CP_ACP, 0, (WCHAR *)((char *)strIn), -1, 0, 0, 0, 0);
    if ((strOut = GlobalAlloc(GMEM_FIXED, size))) {
        WideCharToMultiByte(CP_ACP, 0, (WCHAR *)((char *)strIn), -1, (char *)strOut, size, 0, 0);
    }

    qDebug()<<(char*)(strOut); // ok
}

void SkypeCommon::onComSignal(const QString & name, int argc, void * argv)
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<name;
    IDispatch *pdisp = NULL;
    VARIANTARG *params = (VARIANTARG*)argv;
    QString str_val;

    if (name.startsWith("Command(")) {
        Q_ASSERT(argc == 1);
        pdisp = params[argc-1].pdispVal;
        str_val = getIDispatchStringValue(pdisp, L"Command");
        // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Get cmd val:"<<str_val;        
    } else if (name.startsWith("Reply(")) {
        Q_ASSERT(argc == 1);
        pdisp = params[argc-1].pdispVal;
        str_val = getIDispatchStringValue(pdisp, L"Command");
        if (str_val.length() > 0) {
            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Get repl val:"<<str_val;        
        }
    }
}

void SkypeCommon::onComException(int code, const QString & source, const QString & desc, const QString & help)
{
    qDebug()<<code<<source<<desc<<help;
}

void SkypeCommon::timeOut()
{
    // if ( waitingForConnect ) localEventLoop.exit(1);
}


// for skype
extern bool eventHandled;
extern long eventResult;


void SkypeCommon::processWINMessage( MSG *msg )
{

    return;
    // QByteArray tmp;
    // char *data=NULL;
    // COPYDATASTRUCT *copyData;
    // // qDebug() << "ProcessWINMessage:" << msg->message;
    // // application::eventHandled=true;
    // //  application::eventResult=1;
    // // qApp->eventHandled = true;
    // // qApp->eventResult = 1;
    // ::eventHandled = true;
    // ::eventResult = 1;

    // switch ( msg->message ) { 
    // case WM_COPYDATA:
    //     if ( skype_win != (WId) msg->wParam ) {
	// 	    qDebug() << "Message not from skype";
	// 	    return;
    //     }
    //     copyData = (COPYDATASTRUCT *)msg->lParam;
    //     data = new char[ copyData->cbData ];
    //     data = qstrncpy( data, (char *) copyData->lpData, copyData->cbData );
    //     tmp.append(data);
    //     Q_ASSERT( data != NULL );
    //     delete data;
    //     qDebug() << "WM_COPYDATA:" << tmp;
    //     emit newMsgFromSkype( tmp );
    //     return;
    // default:
	//     if ( msg->message == attachMSG ) {
    //         qDebug() << "Attach status";
    //         switch ( msg->lParam ) {
    //         case SKYPE_ATTACH_SUCCESS:
    //             connected=true;
    //             tryLater=false;
    //             skype_win = (WId) msg->wParam;
    //             qDebug() << "Attached to "<<skype_win;
    //             if ( waitingForConnect ) localEventLoop.quit();
    //             return;
    //         case SKYPE_TRY_NOW:
    //             qDebug() << "Try to attach now";
    //             tryLater=false;
    //             attachToSkype();
    //             return;
    //         case SKYPE_REFUSED:
    //             qDebug() << "Refused";
    //             refused=true;
    //             return;
    //         case SKYPE_PENDING_AUTHORIZATION:
    //             qDebug() << "Pending authorization";
    //             return;
    //         case SKYPE_TRY_AGAIN:
    //             qDebug() << "Try Again";
    //             tryLater=true;
    //             if ( waitingForConnect ) localEventLoop.quit();
    //             return;
    //         default: 
    //             qDebug() <<"WEIRD STATUS:"<<msg->lParam;
    //             return;
    //         }
		  
	//     }
    // }
    // // application::eventHandled=false;
    // // qApp->eventHandled = false;
    // ::eventHandled = false;
}


// #include "SkypeCommon.moc"
#endif /* Q_WS_WIN */
