#include <windows.h>
#include <commdlg.h>
#include "resource.h"
#define EDITID 1
#define UNTITLED TEXT("(untitled)")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

//Functions in POPFILE.C
void PopFileInitialize (HWND);
BOOL PopFileOpenDlg (HWND, PTSTR, PTSTR);
BOOL PopFileSaveDlg (HWND, PTSTR, PTSTR);
BOOL PopFileRead (HWND, PTSTR);
BOOL PopFileWrite (HWND, PTSTR);

//Function in POPFIND.C
HWND PopFindFindDlg (HWND);
HWND PopFindReplaceDlg (HWND);
BOOL PopFindFindText (HWND, int*, LPFINDREPLACE);
BOOL PopFindReplaceText (HWND, int*, LPFINDREPLACE);
BOOL PopFindNextText (HWND, int*);
BOOL PopFindValidFind (void);

//Functions in POPFONT.C
void PopFontInitialize (HWND);
BOOL PopFontChooseFont (HWND);
void PopFontSetFont (HWND);
void PopFontDeinitialize (void);

//Functions in POPPRNT.C
BOOL PopPrntPrintFile(HINSTANCE, HWND, HWND, PTSTR);

//Global variables
static HWND hDlgModeless;
static TCHAR szAppName[]= TEXT("PopPad");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsstance, PSTR szCmdLine, int iCmdShow)
{
	MSG msg;
	HWND hwnd;
	HACCEL hAccel;	//diff
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;	
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(hInstance, szAppName);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= szAppName;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("wrong"), szAppName, MB_ICONERROR);
		return 0;
	}
	
	hwnd= CreateWindow(szAppName, NULL, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, szCmdLine);

	ShowWindow(hwnd,iCmdShow);
	UpdateWindow(hwnd);
	hAccel= LoadAccelerators(hInstance, szAppName);	//diff ¿ì½Ý¼ü±í

	while(GetMessage(&msg, NULL, 0, 0))
	{
		if((hDlgModeless== NULL||!IsDialogMessage(hDlgModeless, &msg))
		{
			if(!TranslateAccelerator(hwnd, hAccel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return msg.wParam;
}

void DoCaption(HWND hwnd, TCHAR* szTitleName)
{
	TCHAR szCaption[64+MAX_PATH];
	wsprintf(szCaption, TEXT("%s-%s"), szAppName,
		szTitleName[0]? szTitleName: UNTITLED);
	SetWindowText(hwnd, szCaption);
}

void OkMessage(HWND hwnd, TCHAR* szMessage, TCHAR* szTitleName)
{
	TCHAR szBuffer[64+ MAX_PATH];
	wsprintf(szBuffer, szMessage, szTitleName[0]? szTitleName: UNTITLED);//?
	MessageBox(hwnd, szBuffer, szAppName, MB_OK| MB_ICONEXCLAMATION);
}

short AskAboutSave(HWND hwnd, TCHAR* szTitleName)
{
	TCHAR szBuffer[64+ MAX_PATH];
	int iReturn;
	wsprintf(szBuffer, TEXT("Save current changes in %s?"),
		szTitleName[0]? szTitleName: UNTITLED);
	iReturn= MessageBox(hwnd, szBuffer, szAppName, MB_YESNOCANCEL| MB_ICONQUESTION);
	if(iReturn== IDYES)
		if(!SendMessage(hwnd, WM_COMMAND, IDM_FILE_SAVE, 0))
			iReturn= IDCANCEL;
	return iReturn;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bNeedSave= FALSE;
	static HINSTANCE hInst;
	static HWND hwndEdit;
	static int iOffset;
	static TCHAR szFileName[MAX+PATH], szTitleName[MAX_PATH];
	static UINT messageFindReplace;
	int iSelBeg, iSelEnd, iEnable;
	LPFINDREPLACE pfr;

	switch(message)
	{
	case WM_CREATE:
		hInst= ((LPCREATESTRUCT)lParam)->hInstance;
		//Create the edit control child window
		hwndEdit= CreateWindow(TEXT("edit"), NULL, WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|
			WS_BORDER|ES_LEFT|ES_MULTILINE|ES_NOHIDESEL|ES_AUTOHSCROLL|ES_AUTOVSCROLL, 
			0,0,0,0, hwnd, (HMENU)EDITID, hInst, NULL);

		SendMessage(hwndEdit, EM_LIMITTEXT, 32000, 0L);
		//Initialize common dialog box stuff
		PopFileInitialize(hwnd);
		PopFontInitialize(hwndEdit);

		messageFindReplace= RegisterWindowMessage(FINDMSGSTRING);
		DoCaption(hwnd, szTtileName);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hwndEdit);
		reutrn 0;
	case WM_SIZE:
		MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
	case WM_INITMENUPOPUP:
		switch(lParam)
		{
		case 1:	//Edit menu
			//Enable Undo if edit control can do it
			EnableMenuItem((HMENU)wParam, IDM_EDIT_UNDO, 
				SendMessage(hwndEdit, EM_CANUNDO,0, 0L)? MF_ENABLED: MF_GRAYED);
			//Enable Paste if text is in the clopboard
			EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE, 
				IsClipboardFormatAvailable(CF_TEXT)? MF_ENABLED: MF_GRAYED);
			//Enable Cut, Copy, and Del if text is selected
			SendMessage(hwndEdit, EM_GETSEL, (WPARAM)&iSelBeg, (LPARAM)&iSelEnd);
			iEnable= iSelBeg != iSelEnd ? MF_ENABLED: MF_GRAYED;
			EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_EDIT_CLEAR, iEnable);
			break;
		case 2:	//Search menu
			//Enable Find, Next, and Replace if modeless
			//dialogs are not already active
			iEnable= hDlgModeless == NULL? MF_ENABLED: MF_GRAYED;
			EnableMenuItem((HMENU)wParam, IDM_SEARCH_FIND, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_SEARCH_NEXT, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_SEARCH_REPLACE, iEnable);
			break;
		}
		return 0;
	case WM_COMMAND: