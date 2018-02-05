/*
将字符串占满屏幕或者缩小平铺
根据字符串的大小建立一个空的位图并且选入一个内存。然后在内存里（的位图上）输出字符串
*/
#include <windows.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//窗口创建与平时一样，没有特别的，这个版本有menu
	static TCHAR szAppName[]= TEXT("HelloBit");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= szAppName;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register Wrong"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("HelloBit"), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
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

LRESULT CALLBACK WndProc ( HWND hwnd, UINT message, WPARAM wParam,LPARAM lParam)
{
	static HBITMAP hBitmap;
	static HDC hdcMem;
	static int cxBitmap, cyBitmap, cxClient, cyClient, iSize= IDM_BIG;
	static TCHAR* szText= TEXT("Hello,world!");
	HDC hdc;
	HMENU hMenu;
	int x,y;
	PAINTSTRUCT ps;
	SIZE size;

	switch(message)
	{
	case WM_CREATE:
		hdc= GetDC(hwnd);
		hdcMem= CreateCompatibleDC(hdc);	//创建兼容性内存设备内容
		GetTextExtentPoint32(hdc, szText, lstrlen(szText), &size);//确定字符串的图素尺寸
		cxBitmap= size.cx;
		cyBitmap= size.cy;
		hBitmap= CreateCompatibleBitmap(hdc, cxBitmap, cyBitmap);//根据长和宽创建创建与指定的设备环境相关的设备兼容的位图
		ReleaseDC(hwnd, hdc);

		SelectObject(hdcMem, hBitmap);   //将位图选入内存，这时候内存作为另外个隐藏的显示设备，并且平铺着一张空的位图
		TextOut(hdcMem, 0,0, szText, lstrlen(szText));	//在内存（这个显示设备内容）输出字符串，写在了位图上
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_COMMAND:
		hMenu= GetMenu(hwnd);
		switch(LOWORD(wParam))
		{
		case IDM_BIG:
		case IDM_SMALL:
			CheckMenuItem(hMenu, iSize, MF_UNCHECKED);
			iSize= LOWORD(wParam);
			CheckMenuItem(hMenu, iSize, MF_CHECKED);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		switch(iSize)	//根据静态iSize来分辨上一个按钮按的是哪个
		{
		case IDM_BIG://如果是大，就从来源内存复制（并拉伸）到目的显示设备内容中
			StretchBlt(hdc, 0, 0, cxClient, cyClient, hdcMem, 
				0, 0, cxBitmap, cyBitmap, SRCCOPY);
			break;
		case IDM_SMALL://如果是小，则平铺位图
			for(y=0; y<cyClient; y+=cyBitmap)
				for(x=0; x<cxClient; x+=cxBitmap)
				{
					BitBlt(hdc, x, y, cxBitmap, cyBitmap, hdcMem, 0, 0, SRCCOPY);
				}
				break;
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		//删除内存和位图句柄
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
