// Added the pragma line below to work with Microsoft Visual Studio C++ 2005 Express edition
// Skype:TheUberOverLord
#pragma comment(lib, "Rpcrt4.lib")
// tab size: 2
// following special commands are recognized and handled by this client
//   #quit
//   #exit
//     terminate client
//   #dbgon
//     turn on debug printing of windows messages
//   #dbgoff
//     turn off debug printing of windows messages
//   #connect
//     discover skype api
//   #disconnect
//     terminate connection to skype api
// all other commands are sent "as is" directly to Skype
// (no UTF-8 translation is done at present)

#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

#include <windows.h>
#include <rpcdce.h>

HWND hInit_MainWindowHandle;
HINSTANCE hInit_ProcessHandle;
char acInit_WindowClassName[128];
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
	unsigned char *paucUUIDString;
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

void main(void)
	{
	// create window class
	//   create dummy/hidden window for processing messages
	//     run message loop thread
	//       do application control until exit
	//       exit: send QUIT message to our own window
	//             wait until thred terminates
	//   destroy main window
	// destroy window class
	uiGlobal_MsgID_SkypeControlAPIAttach=RegisterWindowMessage("SkypeControlAPIAttach");
	uiGlobal_MsgID_SkypeControlAPIDiscover=RegisterWindowMessage("SkypeControlAPIDiscover");
	if( uiGlobal_MsgID_SkypeControlAPIAttach!=0 && uiGlobal_MsgID_SkypeControlAPIDiscover!=0 )
		{
		if( Initialize_CreateWindowClass() )
			{
			if( Initialize_CreateMainWindow() )
				{
				hGlobal_ThreadShutdownEvent=CreateEvent( NULL, TRUE, FALSE, NULL);
				if( hGlobal_ThreadShutdownEvent!=NULL )
					{
					if( _beginthread( &Global_InputProcessingThread, 64*1024, NULL)!=(unsigned long)-1 )
						{
						Global_MessageLoop();
						Global_Console_CancelReadRow();
						WaitForSingleObject( hGlobal_ThreadShutdownEvent, INFINITE);
						}
					CloseHandle(hGlobal_ThreadShutdownEvent);
					}
				DeInitialize_DestroyMainWindow();
				}
			DeInitialize_DestroyWindowClass();
			}
		}
	}
