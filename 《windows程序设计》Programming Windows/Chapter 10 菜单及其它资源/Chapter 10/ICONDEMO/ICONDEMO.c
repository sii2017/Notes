#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(WHND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szAppName[]= TEXT("IconDemo");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));	//new 这次使用自定义图标
	//以前是wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);	//以前使用的是系统的标准图标
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("wrong"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Icon Demo"), WS_OVERLAPPEDWINDOW,
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
	static HICON hIcon;
	static int cxIcon, cyIcon, cxClient, cyClient;
	HDC hdc;
	HINSTANCE hInstance;
	PAINTSTRUCT ps;
	int x, y;

	switch(message)
	{
	case WM_CREATE:
		hInstance= ((LPCREATESTRUCT)lParam)->hInstance;
		hIcon= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));	//获得图标的句柄以供使用
		cxIcon= GetSystemMetrics(SM_CXICON);	//获得图标的大小
		cyIcon= GetSystemMetrics(SM_CYICON);
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		for(y=0; y<cyClient; y+=cyIcon)
			for(x=9; x<cxClient;x+=cxIcon)
				DrawIcon(hdc, x, y, hIcon);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}