#include <QtGui/QApplication>
#include <QtCore>
#include "skypecommon.h"

#ifdef Q_WS_WIN
//#ifdef Q_WS_WIN_RAW_API

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



#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

#include <windows.h>
#include <rpcdce.h>

HWND hInit_MainWindowHandle;
HINSTANCE hInit_ProcessHandle;
// char acInit_WindowClassName[128];
wchar_h acInit_WindowClassName[128];
HANDLE hGlobal_ThreadShutdownEvent;
bool volatile fGlobal_ThreadRunning=true;
UINT uiGlobal_MsgID_SkypeControlAPIAttach;
UINT uiGlobal_MsgID_SkypeControlAPIDiscover;
HWND hGlobal_SkypeAPIWindowHandle=NULL;
#if defined(_DEBUG)
bool volatile fGlobal_DumpWindowsMessages=true;
#else
bool volatile fGlobal_DumpWindowsMessages=false;
#endif
DWORD ulGlobal_PromptConsoleMode=0;
HANDLE volatile hGlobal_PromptConsoleHandle=NULL;

enum {
	SKYPECONTROLAPI_ATTACH_SUCCESS=0,								// Client is successfully attached and API window handle can be found in wParam parameter
	SKYPECONTROLAPI_ATTACH_PENDING_AUTHORIZATION=1,	// Skype has acknowledged connection request and is waiting for confirmation from the user.
    // The client is not yet attached and should wait for SKYPECONTROLAPI_ATTACH_SUCCESS message
	SKYPECONTROLAPI_ATTACH_REFUSED=2,								// User has explicitly denied access to client
	SKYPECONTROLAPI_ATTACH_NOT_AVAILABLE=3,					// API is not available at the moment. For example, this happens when no user is currently logged in.
    // Client should wait for SKYPECONTROLAPI_ATTACH_API_AVAILABLE broadcast before making any further
    // connection attempts.
	SKYPECONTROLAPI_ATTACH_API_AVAILABLE=0x8001
};

bool Global_Console_ReadRow( char *pacPromptBuffer, unsigned int uiMaxLength)
{
	HANDLE hConsoleHandle, hDuplicatedConsoleHandle;
	DWORD ulCharactersRead, ulConsoleMode;
	unsigned int uiNewLength;
	BOOL fReadConsoleResult;
	bool fReturnStatus;
	char cCharacter;

	fReturnStatus=false;
	while((hConsoleHandle=GetStdHandle(STD_INPUT_HANDLE))!=INVALID_HANDLE_VALUE)
		{
            if( DuplicateHandle( GetCurrentProcess(), hConsoleHandle,
                                 GetCurrentProcess(), &hDuplicatedConsoleHandle, 0, FALSE,
                                 DUPLICATE_SAME_ACCESS)==FALSE )
                break;
            GetConsoleMode( hDuplicatedConsoleHandle, &ulConsoleMode);
            SetConsoleMode( hDuplicatedConsoleHandle, ENABLE_LINE_INPUT|ENABLE_PROCESSED_INPUT|ENABLE_ECHO_INPUT);
            hGlobal_PromptConsoleHandle=hDuplicatedConsoleHandle;
            ulGlobal_PromptConsoleMode=ulConsoleMode;
            fReadConsoleResult=ReadConsole( hGlobal_PromptConsoleHandle,
                                            (LPVOID)pacPromptBuffer, uiMaxLength, &ulCharactersRead, NULL);
            if( hGlobal_PromptConsoleHandle==(HANDLE)0 )
                break;
            hGlobal_PromptConsoleHandle=(HANDLE)0;
            SetConsoleMode( hDuplicatedConsoleHandle, ulConsoleMode);
            CloseHandle(hDuplicatedConsoleHandle);
            if( fReadConsoleResult==FALSE || ulCharactersRead>uiMaxLength )
                break;
            pacPromptBuffer[ulCharactersRead]=0;
            uiNewLength=ulCharactersRead;
            while(uiNewLength!=0)
                {
                    cCharacter=pacPromptBuffer[uiNewLength-1];
                    if( cCharacter!='\r' && cCharacter!='\n' )
                        break;
                    uiNewLength--;
                }
            pacPromptBuffer[uiNewLength]=0;
            fReturnStatus=true;
            break;
		}
	if( fReturnStatus==false )
		pacPromptBuffer[0]=0;
	return(fReturnStatus);
}

void Global_Console_CancelReadRow(void)
{
	if( hGlobal_PromptConsoleHandle!=(HANDLE)0 )
		{
            SetConsoleMode( hGlobal_PromptConsoleHandle, ulGlobal_PromptConsoleMode);
            CloseHandle(hGlobal_PromptConsoleHandle);
            hGlobal_PromptConsoleHandle=(HANDLE)0;
		}
}

static LRESULT APIENTRY SkypeAPITest_Windows_WindowProc(
                                                        HWND hWindow, UINT uiMessage, WPARAM uiParam, LPARAM ulParam)
{
	LRESULT lReturnCode;
	bool fIssueDefProc;

	lReturnCode=0;
	fIssueDefProc=false;
	switch(uiMessage)
		{
		case WM_DESTROY:
			hInit_MainWindowHandle=NULL;
			PostQuitMessage(0);
			break;
		case WM_COPYDATA:
			if( hGlobal_SkypeAPIWindowHandle==(HWND)uiParam )
				{
                    PCOPYDATASTRUCT poCopyData=(PCOPYDATASTRUCT)ulParam;
                    printf( "Message from Skype(%u): %.*s\n", poCopyData->dwData, poCopyData->cbData, poCopyData->lpData);
                    lReturnCode=1;
				}
			break;
		default:
			if( uiMessage==uiGlobal_MsgID_SkypeControlAPIAttach )
				{
                    switch(ulParam)
                        {
                        case SKYPECONTROLAPI_ATTACH_SUCCESS:
                            printf("!!! Connected; to terminate issue #disconnect\n");
                            hGlobal_SkypeAPIWindowHandle=(HWND)uiParam;
                            break;
                        case SKYPECONTROLAPI_ATTACH_PENDING_AUTHORIZATION:
                            printf("!!! Pending authorization\n");
                            break;
                        case SKYPECONTROLAPI_ATTACH_REFUSED:
                            printf("!!! Connection refused\n");
                            break;
                        case SKYPECONTROLAPI_ATTACH_NOT_AVAILABLE:
                            printf("!!! Skype API not available\n");
                            break;
                        case SKYPECONTROLAPI_ATTACH_API_AVAILABLE:
                            printf("!!! Try connect now (API available); issue #connect\n");
                            break;
                        }
                    lReturnCode=1;
                    break;
				}
			fIssueDefProc=true;
			break;
		}
	if( fIssueDefProc )
		lReturnCode=DefWindowProc( hWindow, uiMessage, uiParam, ulParam);
	if( fGlobal_DumpWindowsMessages )
		{
            printf( "WindowProc: hWindow=0x%08X, MainWindow=0x%08X, Message=%5u, WParam=0x%08X, LParam=0x%08X; Return=%ld%s\n",
                    hWindow, hInit_MainWindowHandle, uiMessage, uiParam, ulParam, lReturnCode, fIssueDefProc? " (default)":"");
		}
	return(lReturnCode);
}

bool Initialize_CreateWindowClass(void)
{
    // unsigned char *paucUUIDString;
    wchar_t *paucUUIDString;
	RPC_STATUS lUUIDResult;
	bool fReturnStatus;
	UUID oUUID;

	fReturnStatus=false;
	lUUIDResult=UuidCreate(&oUUID);
	hInit_ProcessHandle=(HINSTANCE)OpenProcess( PROCESS_DUP_HANDLE, FALSE, GetCurrentProcessId());
	if( hInit_ProcessHandle!=NULL && (lUUIDResult==RPC_S_OK || lUUIDResult==RPC_S_UUID_LOCAL_ONLY) )
		{
            if( UuidToString( &oUUID, &paucUUIDString)==RPC_S_OK )
                {
                    WNDCLASS oWindowClass;

                    strcpy_s( acInit_WindowClassName, "Skype-API-Test-");
                    strcat_s( acInit_WindowClassName, (char *)paucUUIDString);

                    oWindowClass.style=CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
                    oWindowClass.lpfnWndProc=(WNDPROC)&SkypeAPITest_Windows_WindowProc;
                    oWindowClass.cbClsExtra=0;
                    oWindowClass.cbWndExtra=0;
                    oWindowClass.hInstance=hInit_ProcessHandle;
                    oWindowClass.hIcon=NULL;
                    oWindowClass.hCursor=NULL;
                    oWindowClass.hbrBackground=NULL;
                    oWindowClass.lpszMenuName=NULL;
                    oWindowClass.lpszClassName=acInit_WindowClassName;

                    if( RegisterClass(&oWindowClass)!=0 )
                        fReturnStatus=true;

                    RpcStringFree(&paucUUIDString);
                }
		}
	if( fReturnStatus==false )
		CloseHandle(hInit_ProcessHandle),hInit_ProcessHandle=NULL;
	return(fReturnStatus);
}

void DeInitialize_DestroyWindowClass(void)
{
	UnregisterClass( acInit_WindowClassName, hInit_ProcessHandle);
	CloseHandle(hInit_ProcessHandle),hInit_ProcessHandle=NULL;
}

bool Initialize_CreateMainWindow(void)
{
	hInit_MainWindowHandle=CreateWindowEx(WS_EX_APPWINDOW|WS_EX_WINDOWEDGE,
                                          acInit_WindowClassName, "", WS_BORDER|WS_SYSMENU|WS_MINIMIZEBOX,
                                          CW_USEDEFAULT, CW_USEDEFAULT, 128, 128, NULL, 0, hInit_ProcessHandle, 0);
	
	return(hInit_MainWindowHandle!=NULL? true:false);
}

void DeInitialize_DestroyMainWindow(void)
{
	if( hInit_MainWindowHandle!=NULL )
		DestroyWindow(hInit_MainWindowHandle),hInit_MainWindowHandle=NULL;
}

void Global_MessageLoop(void)
{
	MSG oMessage;

	while(GetMessage( &oMessage, 0, 0, 0)!=FALSE)
		{
            TranslateMessage(&oMessage);
            DispatchMessage(&oMessage);
		}
}

void __cdecl Global_InputProcessingThread(void *)
{
	static char acInputRow[1024];
	bool fProcessed;

    if( SendMessageTimeout( HWND_BROADCAST, uiGlobal_MsgID_SkypeControlAPIDiscover, (WPARAM)hInit_MainWindowHandle, 0, SMTO_ABORTIFHUNG, 1000, NULL)!=0 )
		{
            while(Global_Console_ReadRow( acInputRow, sizeof(acInputRow)-1))
                {
                    if( _stricmp( acInputRow, "#quit")==0 ||
                        _stricmp( acInputRow, "#exit")==0 )
                        break;
                    fProcessed=false;
                    if( _stricmp( acInputRow, "#dbgon")==0 )
                        {
                            printf( "SkypeControlAPIAttach=%u, SkypeControlAPIDiscover=%u, hInit_MainWindowHandle=0x%08lX\n",
                                    uiGlobal_MsgID_SkypeControlAPIAttach, uiGlobal_MsgID_SkypeControlAPIDiscover, hInit_MainWindowHandle);
                            fGlobal_DumpWindowsMessages=true,fProcessed=true;
                        }
                    if( _stricmp( acInputRow, "#dbgoff")==0 )
                        fGlobal_DumpWindowsMessages=false,fProcessed=true;
                    if( _stricmp( acInputRow, "#connect")==0 )
                        {
                            SendMessageTimeout( HWND_BROADCAST, uiGlobal_MsgID_SkypeControlAPIDiscover, (WPARAM)hInit_MainWindowHandle, 0, SMTO_ABORTIFHUNG, 1000, NULL);
                            fProcessed=true;
                        }
                    if( _stricmp( acInputRow, "#disconnect")==0 )
                        {
                            hGlobal_SkypeAPIWindowHandle=NULL;
                            printf("!!! Disconnected\n");
                            fProcessed=true;
                        }
                    if( fProcessed==false && hGlobal_SkypeAPIWindowHandle!=NULL )
                        {
                            COPYDATASTRUCT oCopyData;

                            // send command to skype
                            oCopyData.dwData=0;
                            oCopyData.lpData=acInputRow;
                            oCopyData.cbData=strlen(acInputRow)+1;
                            if( oCopyData.cbData!=1 )
                                {
                                    if( SendMessage( hGlobal_SkypeAPIWindowHandle, WM_COPYDATA, (WPARAM)hInit_MainWindowHandle, (LPARAM)&oCopyData)==FALSE )
                                        {
                                            hGlobal_SkypeAPIWindowHandle=NULL;
                                            printf("!!! Disconnected\n");
                                        }
                                }
                        }
                }
		}
	SendMessage( hInit_MainWindowHandle, WM_CLOSE, 0, 0);
	SetEvent(hGlobal_ThreadShutdownEvent);
	fGlobal_ThreadRunning=false;
}


SkypeCommon::SkypeCommon() { 
    if ( attachMSG == 0 || discoverMSG == 0 ) { 
        // attachMSG = RegisterWindowMessage((LPCWSTR)"SkypeControlAPIAttach");
        // discoverMSG = RegisterWindowMessage((LPCWSTR)"SkypeControlAPIDiscover");

        // 这种写法在unicode模式的win下可用，到中文cp936编码的系统上就不行了。
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

    QObject::connect( qApp, SIGNAL( winMessage( MSG *) ), this, SLOT( processWINMessage( MSG *) ) );
    skype_win=0;
    connected = false;
    refused = false;
    tryLater = false;
    // TimeOut = 10000;
    TimeOut = 1000 * 120;
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

    QTimer::singleShot(TimeOut, this, SLOT(timeOut()));
    
    int result = localEventLoop.exec();
    waitingForConnect = false;
    return connected;
    return false;
}

void SkypeCommon::timeOut()
{
    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<(this->TimeOut/1000);
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
                if ( waitingForConnect )  localEventLoop.quit();
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
