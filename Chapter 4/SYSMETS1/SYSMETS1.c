#include <windows.h>
#include "SYSMETS.h"

LRESULT CALLBACK WndProc(WHND, UINT, APARAM, LPARAM);
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("SysMets1");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW|CS_VREDRAW;	//这两个style意味着当窗口的大小水平或者垂直方向发生任何变化，windows都会强行刷新页面发送一个WM_PAINT消息
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL,TEXT("This program require Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Get System Metrics No.1"), WS_OVERLAPPEDWINDOW ,CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg, NULL, 0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxChar, cxCaps, cyChar, cyClient, iVscrollPos;
	HDC hdc;
	int i,y;
	PAINTSTRUCT ps;
	TCHAR szBuffer[10];
	TEXTMETRIC tm;
	switch(message)
	{
	case WM_CREATE:
		hdc= GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		cxChar= tm.tmAveCharWidth;
		cxCaps= (tm.tmPitchAndFamily&1?3:2)*cxChar/2;
		cyChar= tm.tmHeight+tm.tmExternalLeading;		//每隔多少像素输出一行
		ReleaseDC(hwnd, hdc);

		SetScrollRange(hwnd, SB_VERT,0,NUMLINES-1,FALSE);
		SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		for(i=0;i<NUMLINES; ++i)
		{
			TextOut(hdc, 0, cyChar*i, sysmetrics[i].szLabel, lstrlen(sysmetrics[i].szLabel));
			TextOut(hdc, 22*cxCaps, cyChar*i,sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));
			
			SetTextAlign(hdc, TA_RIGHT|TA_TOP);			//在第三列打印之前改变hdc使之右对齐
			TextOut(hdc, 22*cxCaps+40*cxChar, cyChar*i, szBuffer, wsprintf(szBuffer, TEXT("%5d"),
				GetSystemMetrics(sysmetrics[i].iIndex)));
			SetTextAlign(hdc, TA_LEFT|TA_TOP);		//打印好第三列以后重新左对齐前两列，还有这种操作。
		}
		
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}