#include <windows.h>
#include <wininet.h>
#include <process.h>
#include "resource.h"

#define WM_USER_CHECKFILES (WM_USER+1)
#define WM_USER_GETFILES (WM_USER+2)

#define FTRSERVER TEXT("ftp.cpetzold.com")
#define DIRECTORY TEXT("cpetzold.com/ProgWin/UpdDemo")
#define TEMPLATE TEXT("UD??????.TEXT")

typedef struct
{
	TCHAR* szFilename;
	char* szContents;
}
FILEINFO;

typedef struct
{
	int iNum;
	FILEINFO info[1];
}
FILELIST;

typedef struct
{
	BOOL bContinue;
	HWND hwnd;
}
PARAMS;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
VOID FtpThread(PVOID);
VOID ButtonSwitch(HWND, HWND, TCHAR*);
FILELIST* GetFileList(VOID);
int Compare(cocnst FILEINFO*, const FILEINFO*);

HINSTANCE hInst;
TCHAR szAppName[]= TEXT("UpdDemo");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//正常创建窗口，并且创建一个dialog
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	hInst= hInstance;

	wndclass.style= 0;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= NULL;
	wndclass.hbrBackground= GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register Wrong!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Update Demo with Anonymous FTP"), 
		WS_OVERLAPPEDWINDOW| WS_VSCROLL, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	//After window iss displayed, check if the latest file exists
	SendMessage(hwnd, WM_USER_CHECKFILES, 0, 0);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static FILELIST* plist;
	static int cxClient, cyClient, cxChar, cyChar;
	HDC hdc;
	int i;
	PAINTSTRUCT ps;
	SCROLLINFO si;
	SYSTEMTIME st;
	TCHAR szFilename[MAX_PATH];

	switch(message)
	{
	case WM_CREATE:
		cxChar= LOWORD(GetDialogBaseUnits());
		cyChar= HIWORD(GetDialogBaseUnits());
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);

		si.cbSize= sizeof(SCROLLINFO);
		si.fMask= SIF_RANGE|SIF_PAGE;
		si.nMin= 0;
		si.nMax= plist?plist->iNum-1:0;
		si.nPage= cyClient/cyChar;

		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		return 0;
	case WM_VSCROLL:
		si.cbSize= sizeof(SCROLLINFO);
		si.fMask= SIF_POS|SIF_RANGE|SIF_PAGE;
		GetScrollInfo(hwnd, SB_VERT, &si);

		switch(LOWORD(wParam))
		{
		case SB_LINEDOWN:
			si.nPos+=1; break;
		case SB_LINEUP:
			si.nPos-=1; break;
		case SB_PAGEDOWN:
			si.nPos+= si.nPage; break;
		case SB_PAGEUP:
			si.nPos-=si.nPage; break;
		case SB_THUMBPOSITION:
			si.nPos= HIWORD(wParam);
			break;
		default:
			return 0;
		}
		si.fMask= SIF_POS;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_USER_CHECKFILES:
		//Get the system date& form filename from year and month
		GetSystemTime(&st);
		wsprintf(szFilename, TEXT("UD%04i%02i.TXT"), st.wYear, st.wMonth);
		//Check if the file exists; if so, read all the files
		if(GetFileAtrtributes(szFilename)!= (DWORD)-1)
		{
			SendMessage(hwnd, WM_USER_GETFILES, 0, 0);
			return 0;
		}

		//Otherwise, get files from internet
		//But first check so we don't try to copy files to a cdrom
		if(GetDriveType(NULL)== DRIVE_CDROM)
		{
			Message(hwnd, TEXT("Cannot run this program from CDROM"), 
				szAppName, MB_OK|MB_ICONEXCLAMATION);
			return 0;
		}

		//Ask user if an Internet connection is desired
		if(IDYES== MessageBox(hwnd, TEXT("Update information from Internet?"),
			szAppName, MB_YESNO|MB_ICONQUESTION))
			DialogBox(hInst, szAppName, hwnd, DlgProc);
		
		SendMessage(hwnd, WM_USER_GETFILES, 0, 0);
		return 0;
	case WM_USER_GETFILES:
		SetCursor(LoadCursor(NULL, IDC_WAIT));
		ShowCursor(TRUE);

		plist= GetFileList();
		ShowCursor(FALSE);
		SetCursor(LoadCursor(NULL, IDC_ARROW));

		//Simulate a WM_SIZE message to alter scroll bar& repaint
		SendMessage(hwnd, WM_SIZE, 0, MAKELONG(cxClient, cyClient));
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hwnd, &ps);
		SetTextAlign(hdc, TA_UPDATECP);

		si.cbSize= sizeof(SCROLLINFO);
		si.fMask= SIF_POS;
		GetScrollInfo(hwnd, SB_VERT, &si);

		if(plist)
		{
			for(i=0; i<plist->iNum; i++)
			{
				MoveToEx(hdc, cxChar, (i-si.nPos)*cyChar, NULL);
				TextOut(hdc, 0, 0, plist->info[i].szFilename, 
					lstrlen(plist->info[i].szFilename));
				TextOut(hdc, 0, 0, TEXT(":"), 2);
				TextOutA(hdc, 0, 0, plist->info[i].szContents, 
					strlen(plist->info[i].szContents));
			}
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PARAMS params;
	switch(message)
	{
	case WM_INITDIALOG:
		params.bContinue= TRUE;
		params.hwnd= hwnd;

		_beginthread(FtpThread, 0, &params);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case INCANCEL:
			params.bContinue= FALSE;
			return TRUE;
		case IDOK:
			EndDialog(hwnd, 0);
			return TRUE;
		}
	}
	return FALSE;
}

void FtpThread(PVOID parg)
{
	BOOL bSuccess;
	HINTERNET hIntSession, hFtpSession, hFind;
	HWND hwndStatus, hwndButton;
	PARAMS* pparams;
	TCHAR szBuffer[64];
	WIN32_FIND_DATA finddata;
	pparams= parg;
	hwndStatus= GetDlgItem(pparams->hwnd, IDC_STATUS);
	hwndButton= GetDlgItem(pparams->hwnd, IDCANCEL);

	//Open an internet session 
	hIntSession= InternetOpen(szAppName, INTERNET_OPEN_TYPE_PRECONFIG, NULL,
		NULL, INTERNET_FLAG_ASYNC);
	if(hInetSession== NULL)
	{
		wsprintf(szBuffer, TEXT("InternetOpen error %i"), GetLastError());
		ButtonSwitch(hwndStatus, hwndButton, szBuffer);
		_endthread();
	}

	SetWindowText(hwndStatus, TEXT("Internet session opened..."));

	//Check if user has pressed cancel
	if(!pparams->bContinue)
	{
		InternetCloseHandle(hIntSession);
		ButtonSwitch(hwndStatus, hwndButton, NULL);
		_endthread();
	}

	//Open an FTP session
	hFtpSession=  InternetConnect(hIntSession, FTPSERVER, INTERNET_DEFAULT_FTP_PORT,
		NULL, NULL, INTERNET_SERVICE_FTP, 0, 0);

	if(hFtpSession== NULL)
	{
		InternetCloseHandle(hIntSession);
		wsprintf(szBuffer, TEXT("InternetConnect error %i"), GetLastError());
		ButtonSwitch(hwndStatus, hwndButton, szBuffer);
		_endthread();
	}

	SetWindowText(hwndStatus, TEXT("FTP Session opened...."));
	//Check if user has pressed Cancel
	if(!pparams->bContinue)
	{
		InternetCloseHandle(hFtpSession);
		InternetCloseHandle(hIntSession);
		ButtonSwitch(hwndStatus, hwndButton, NULL);
		_endthread();
	}

	//Set the directory
	bSuccess= FtpSetCurrentDirectory(hFtpSession, DIRECTORY);
	if(!bSuccess)
	{
		InternetCloseHandle(hFtpSession);
		InternetCloseHandle(hIntSession);
		wsprintf(szBuffer, TEXT("Cannot setdirectory to %s"), DIRECTORY);
		ButtonSwitch(hwndStatus, hwndButton, szBuffer);
		_endthread();
	}

	SetWindowText(hwndStatus, TEXT("Directroy found..."));
	//Check if user has pressed Cancel
	if(!pparams->bContinue)
	{
		InternetCloseHandle(hFtpSession);
		InternetCloseHandle(hIntSession);
		ButtonSwitch(hwndStatus, hwndButton, NULL);
		_endthread(;
	}

	//Get the first file fitting the template
	hFind= FtpFineFirstFile(hFtpSession, TEMPLATE, &finddata, 0, 0);
	if(hFind== NULL)
	{
		InternetCloseHandle(hFtpSession);
		InternetCloseHandle(hIntSession);
		ButtonSwitch(hwndStatus, hwndButton, TEXT("Cannot find files"));
		_endthread();
	}

	do
	{
		//check if user has pressed cancel
		if(!pparams->bContinue)
		{
			InternetCloseHandle(hFind);
			InternetCloseHandle(hFtpSession);
			InternetCloseHandle(hIntSession);
			ButtonSwitch(hwndStatus, hwndButton, NULL);
			_endthread();
		}

		//Copy the file from internet to local hard disk, but fail if the filealready exists
		wsprintf(szBuffer, TEXT("Reading file %s..."), finddata.cFileName);
		SetWindowText(hwndStatus, szBuffer);
		FtpGetFile(hFtpSession, finddata.cFileName, finddata.cFileName, TRUE, 
			FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY, 0);
	}
	while(InternetFindNextFile(hFind, &finddata));
	InternetCloseHandle(hFind);
	InternetCloseHandle(hFtpSession);
	InternetCloseHandle(hIntSession);

	ButtonSwitch(hwndStatus, hwndButton, TEXT("Internet DownLoad Complete"));
}

VOID ButtonSwitch(HWND hwndStatus, HWND hwndButton, TCHAR* szText)
{
	if(szText)
		SetWindowText(hwndStatus, szText);
	else
		SetWindowText(hwndStatus, TEXT("Internet Session Cancelled"));
	SetWindowText(hwndButton, TEXT("OK"));
	SetWindowLong(hwndbutton, GWL_ID, IDOK);
}

FILELIST* GetFileList(void)
{
	DWORD dwRead;
	FILELIST* plist;
	HANDLE hFile, hFind;
	int iSize, iNum;
	WIN32_FIND_DATA finddata;

	hFind= FindFirstFile(TEMPLATE, &finddata);
	if(hFind== INVALID_HANDLE_VALUE)
		return NULL;
	plist= NULL;
	iNum= 0;

	do
	{
		hFile= CreateFile(finddata.cFileName, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, 0, NULL);

		if(hFile== INVALID_HANDLE_VALUE)
			continue;

		iSize= GetFileSize(hFile, NULL);

		if(iSize== (DWORD)-1)
		{
			CloseHandle(hFile);
			continue;
		}

		plist= realloc(plist, sizeof(FILELIST)+ iNum*sizeof(FILEINFO));//1623