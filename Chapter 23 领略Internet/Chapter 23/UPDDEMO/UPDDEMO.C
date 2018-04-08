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