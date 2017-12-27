#include <windows.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
int iCurrentColor= IDC_BLACK, iCurrentFigure= IDC_RECT;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("About2");
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

void PaintWindow(HWND hwnd, int iColor, int iFigure)//这里的hwnd实际上是对话框控件IDC_PAINT的，并不是本体
{
	static COLORREF crColor[8]= 
	{ RGB(0,0,0), RGB(0,0,255), RGB(0,255,0), RGB(0,255,255), 
	RGB(255,0,0), RGB(255,0,255), RGB(255,255,0), RGB(255,255,255)};
	HBRUSH hBrush;
	HDC hdc;
	RECT rect;

	hdc= GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	hBrush= CreateSolidBrush(crColor[iColor- IDC_BLACK]);	//制作新的画刷获取句柄
	hBrush= (HBRUSH)SelectObject(hdc, hBrush);	//应该刚刚的句柄并且将之前的句柄赋值回去

	if(iFigure==IDC_RECT)
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	else
		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
	DeleteObject(SelectObject(hdc, hBrush));	//画刷用完就删
	ReleaseDC(hwnd, hdc);
}

void PaintTheBlock(HWND hCtrl, int iColor, int iFigure)
{
	InvalidateRect(hCtrl, NULL, TRUE);
	UpdateWindow(hCtrl);
	PaintWindow(hCtrl, iColor, iFigure);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE:
		hInstance= ((LPCREATESTRUCT)lParam)->hInstance;
		return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_APP_ABOUT:
			if(DialogBox(hInstance, TEXT("AboutBox"), hwnd, AboutDlgProc))
				InvalidateRect(hwnd, NULL, TRUE);	//在DialogBox关闭后，才会有返回值进入到这里
			return 0;
		}
		break;
	case WM_PAINT:
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);

		PaintWindow(hwnd, iCurrentColor, iCurrentFigure);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hCtrlBlock;
	static int iColor, iFigure;

	switch(message)
	{
	case WM_INITDIALOG:
		iColor= iCurrentColor;
		iFigure= iCurrentFigure;

		CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, iColor);
		CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, iFigure);

		hCtrlBlock= GetDlgItem(hDlg, IDC_PAINT);//用对话框句柄和控件ID来取得一个对话框控件的窗口句柄

		SetFocus(GetDlgItem(hDlg, iColor));
		return FALSE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			iCurrentColor= iColor;
			iCurrentFigure= iFigure;
			EndDialog(hDlg, TRUE);	//第二个参数与OK相关
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);	//第二个参数与cancel相关
			return TRUE;

		case IDC_BLACK:
		case IDC_RED:
		case IDC_GREEN:
		case IDC_YELLOW:
		case IDC_BLUE:
		case IDC_MAGENTA:
		case IDC_CYAN:
		case IDC_WHITE:
			iColor= LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, LOWORD(wParam));
			PaintTheBlock(hCtrlBlock, iColor, iFigure);
			return TRUE;
		case IDC_RECT:
		case IDC_ELLIPSE:
			iFigure= LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, LOWORD(wParam));
			PaintTheBlock(hCtrlBlock, iColor, iFigure);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//这个程序没什么太大的难点，在ABOUT1的基础上，增加了两个组，一个组负责选择颜色，另一个组是选择形状。选择好以后在区域里显示出样子，按确定后打印出来