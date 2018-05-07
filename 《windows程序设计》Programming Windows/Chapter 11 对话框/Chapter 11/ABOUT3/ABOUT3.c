#include <windows.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EllipPushWndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("About3");
	MSG msg;
	HWND hwnd;
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

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= EllipPushWndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= NULL;
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)(COLOR_BTNFACE + 1);	//COLOR_BTNFACE是按钮颜色
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= TEXT("EllipPush");

	RegisterClass(&wndclass);	//子控件类，类名在控件里可以进行使用
	
	hwnd= CreateWindow(szAppName, TEXT("About Box Demo Program"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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
	static HINSTANCE hInstance;
	switch(message)
	{
	case WM_CREATE:
		hInstance= ((LPCREATESTRUCT)lParam)->hInstance;	//先保存好句柄
		return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_APP_ABOUT:
			DialogBox(hInstance, TEXT("AboutBox"), hwnd, AboutDlgProc);
			return 0;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg,0);
			return TRUE;	//作为DialogBox的返回值
		}
		break;
	}
	return FALSE;
}

//给子控件注册的类专用的消息处理程序
LRESULT CALLBACK EllipPushWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szText[40];
	HBRUSH hBrush;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	switch(message)
	{
	case WM_PAINT:
		GetClientRect(hwnd, &rect);
		GetWindowText(hwnd, szText, sizeof(szText));	//获取标题文字

		hdc= BeginPaint(hwnd, &ps);

		hBrush= CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		hBrush= (HBRUSH)SelectObject(hdc, hBrush);
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));	//设置OK按钮的背景颜色以及文字颜色
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));

		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
		DrawText(hdc, szText, -1, &rect, DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		DeleteObject(SelectObject(hdc, hBrush));

		EndPaint(hwnd, &ps);
		return 0;
	case WM_KEYUP:
		if(wParam!= VK_SPACE)	//如果是空格则往下给WM_LBUTTONUP处理
			break;
	case WM_LBUTTONUP:		//发送IDOK消息给对话框消息处理程序以关闭对话框
		SendMessage(GetParent(hwnd), WM_COMMAND, 
			GetWindowLong(hwnd, GWL_ID), (LPARAM)hwnd);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


//About3较About2没有什么大的区别，仅仅是增加了对话框使用的子控件类