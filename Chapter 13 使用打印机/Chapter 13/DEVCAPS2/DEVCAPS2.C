#include <windows.h>
#include "resource.h"
//#define NUMLINES ((int)(sizeof devcaps/sizeof devcaps[0]))
//struct
//{
//	int iIndex;
//	TCHAR* szLabel;
//	TCHAR* szDesc;
//}
//devcaps[]=
//{
//	HORZSIZE, TEXT("HORZSIZE"), TEXT("Width in millimeters:"),
//	VERTSIZE, TEXT("VERTSIZE"), TEXT("Height in millimters:"),
//	HORZRES, TEXT("HORZRES"), TEXT("Width in pixels:"),
//	VERTRES, TEXT("VERTRES"), TEXT("Height in raster lines:"),
//	BITSPIXEL, TEXT("BITSPIXEL"), TEXT("Color bits per pixel:"),
//	PLANES, TEXT("PLANES"), TEXT("Number of color planes:"),
//	NUMBRUSHES, TEXT("NUMBRUSHES"), TEXT("Number of device brushes:"),
//	NUMPENS, TEXT("NUMPENS"), TEXT("Number of device pens:"),
//	NUMMARKERS, TEXT("NUMMARKERS"), TEXT("Number of device markers:"),
//	NUMFONTS, TEXT("NUMFONTS"), TEXT("Number if device fonts:"),
//	NUMCOLORS, TEXT("NUMCOLORS"), TEXT("Number of device colors:"),
//	PDEVICESIZE, TEXT("PDEVICESIZE"), TEXT("Size of device structure:"),
//	ASPECTX, TEXT("ASPECTX"), TEXT("Relative width of pixel:"),
//	ASPECTY, TEXT("ASPECTY"), TEXT("Relative height of pixel:"),
//	ASPECTXY, TEXT("ASPECTXY"), TEXT("Relative diagonal of pixel:"),
//	LOGPIXELSX, TEXT("LOGPIXELSX"), TEXT("Horizontal dots per inch:"),
//	LOGPIXELSY, TEXT("LOGPIXELSY"), TEXT("Vertical dots per inch:"),
//	SIZEPALETTE, TEXT("SIZEPALETTE"), TEXT("Number of palette entries:"),
//	NUMRESERVED, TEXT("NUMRESERVED"), TEXT("Reserved palete entries:"),
//	COLORRES, TEXT("COLORRES"), TEXT("Actual color resolution:")
//};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DoBasicInfo(HDC,HDC,int, int);
void DoOtherInfo(HDC, HDC, int, int);
void DoBitCodedCaps(HDC, HDC, int, int, int);

typedef struct
{
	int iMask;
	TCHAR* szDesc;
}
BITS;
#define IDM_DEVMODE 1000
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("DevCpas2");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	
	wndclass.style= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= szAppName;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG!"), szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd= CreateWindow(szAppName, NULL, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
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
	static TCHAR szDevice[32], szWindowText[64];
	static int cxChar, cyChar, nCurrentDevice= IDM_SCREEN, nCurrentInfo= IDM_BASIC;
	static DWORD dwNeeded, dwReturned;
	static PRINTER_INFO_4* pinfo4;
	static PRINTER_INFO_5* pinfo5;
	DWORD i;
	HDC hdc, hdcInfo;
	HMENU hMenu;
	HANDLE hPrint;
	PAINTSTRUCT ps;
	TEXTMETRIC tm;
	
	switch(message)
	{
	case WM_CREATE:
		hdc= GetDC(hwnd);	//获得当前hdc
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		GetTextMetrics(hdc, &tm);	//以hdc作为参数获取该hdc的相关字体信息保存在tm中
		cxChar= tm.tmAveCharWidth;
		cxCaps= (tm.tmPitchAndFamily & 1?3:2) * cxChar/2;
		cyChar= tm.tmHeight + tm.tmExternalLeading;
		ReleaseDC(hwnd, hdc);
		//fall through
	case WM_SETTINGCHANGE:
		hMenu= GetSubMenu(GetMenu(hwnd), 0);
		while(GetMenuItemCount(hMenu)>1)
			DeleteMenu(hMenu, 1, MF_BYPOSITION);
		if(GetVersion()&0x80000000)	//windows 98
		{
			EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 5, NULL, 0, &dwNeeded, &dwReturned);
			pinfo5= malloc(dwNeeded);
			EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 5, (PBYTE)pinfo5, 
				dwNeeded, &dwNeeded, &dwReturned);
			for(i=0;i<dwReturned; i++)
			{
				AppendMenu(hMenu, (i+1)%16?0:MF_MENUBARBREAK, i+1, pinfo5[i].pPrinterName);
			}
			free(pinfo5);
		}
		else	//window NT
		{
			EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
			pinfo4= malloc(dwNeeded);
			EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pinfo4, dwNeeded, 
				&dwNeeded, &dwReturned);
			for(i=0; i<dwReturned; i++)
			{
				AppendMenu(hMenu, (i+1)%16?0:MF_MENUBARBREAK, i+1, pinfo4[i].pPrinterName);
			}
			free(pinfo4);
		}
		AppendMenu(hMenu, MF_SEPARATOR, 0 ,NULL);
		AppendMenu(hMenu, 0, IDM_DEVMODE, TEXT("Properties"));
		wParam= IDM_SCREEN;
		//fall through
	case WM_COMMAND:
		hMenu= GetMenu(hwnd);
		if(LOWORD(wParam)== IDM_SCREEN|| //IDM_SCREEN& Printers
			LOWORD(wParam)< IDM_DEVMODE)
		{

	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		for(i=0; i<NUMLINES;++i)
		{
			TextOut(hdc, 0, cyChar*i, devcaps[i].szLabel, lstrlen(devcaps[i].szLabel));
			TextOut(hdc, 14*cxCaps, cyChar*i,devcaps[i].szDesc, lstrlen(devcaps[i].szDesc));
			SetTextAlign(hdc, TA_RIGHT | TA_TOP);		//第三列设置为 起始点为右边开始输出
			TextOut(hdc, 14*cxCaps + 35*cxChar, cyChar*i, szBuffer, wsprintf(szBuffer, TEXT("%5d"),
				GetDeviceCaps(hdc, devcaps[i].iIndex)));
			SetTextAlign(hdc, TA_LEFT | TA_TOP);	//第三列输出后再下一个循环开始输出第一列之前 再改回从左侧开始输出
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}