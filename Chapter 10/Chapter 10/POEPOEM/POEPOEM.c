#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInst;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szAppName[16], szCaption[64], szErrMsg[64];
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	LoadString(hInstance, IDS_APPNAME, szAppName, sizeof(szAppName)/sizeof(TCHAR));
	LoadString(hInstance, IDS_CAPTION, szCaption, sizeof(szCaption)/sizeof(TCHAR));

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(hInstance, szAppName);	//new 这次使用自定义图标
	//以前是wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);	//以前使用的是系统的标准图标
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		LoadStringA(hInstance, IDS_APPNAME, (char*)szAppName, sizeof(szAppName));//这里重新加载了一遍
		LoadStringA(hInsatnce, IDS_ERRMSG, (char*)szErrMsg, sizeof(szErrMsg));
		MessageBoxA(NULL, (char*)szErrMsg, (char*)szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, szCaption, WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static char *pText;
	static HGLOBAL hResource;
	static HWND hScroll;
	static int iPosition, cxChar, cyChar, cyClient, iNumLines, xScroll;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	TEXTMETRIC tm;

	switch(message)
	{
	case WM_CREATE:
		hdc= GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		cxChar= tm.tmAveCharWidth;
		cyChar= tm.tmHeight+ tm.tmExternalLeading;
		ReleaseDC(hwnd, hdc);

		xScroll=  GetSystemMetrics(SM_CXVSCROLL);
		hScroll= CreteaWindow(TEXT("scrollbar"), NULL, WS_CHILD|WS_VISIBLE|SBS_VERT,
			0,0,0,0, hwnd, (HMENU)1, hInst, NULL);

		//LoadResource
		hResource= LoadResource(hInst, FindResource(hInst, TEXT("AnnabelLee"), TEXT("TEXT")));
		pText= (char*)LockResource(hResource);
		iNumLines= 0;

		while(*pText!='\\' && *pText!='\0')
		{
			if(*pText== '\n')
				iNumLines++;
			pText= AnsiNext(pText);
		}
		*pText= '\0';