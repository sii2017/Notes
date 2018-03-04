/*
使用PACKEDIB.C.H来获取DIB信息，并且绘图。
如果是8位就建立逻辑绘图板并且显现，不然则不。（按书上说会按照20标准色来显现，但是现在的显卡都是真彩色，如果默认显现的话可能是32位真彩色？）
由于8位位图已经不使用了，甚至画图软件都不能以这个规格保存，无法测试
除了使用调色盘索引以外，SHOWDIB6程序与SHOWDIB3相似。
*/
#include <windows.h>
#include "PackeDib.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("ShowDib6");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//正常winmain，带menu
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
		MessageBox(NULL, TEXT("Register falied"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Show DIB #6: Palette Indices"), WS_OVERLAPPEDWINDOW,
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

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam,LPARAM lParam)
{
	static BITMAPINFO* pPackedDib;
	static HPALETTE hPalette;
	static int cxClient, cyClient;
	static OPENFILENAME ofn;
	static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];
	static TCHAR szFilter[]= TEXT("Bitmap Files(*.BMP)\0*.bmp\0")
		TEXT("All Files(*.*)\0*.*\0\0");
	HDC hdc;
	int i, iNumColors;
	PAINTSTRUCT ps;
	WORD* pwIndex;

	switch(message)
	{
	case WM_CREATE:
		ofn.lStructSize= sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd ;
		ofn.hInstance = NULL ;
		ofn.lpstrFilter = szFilter ;
		ofn.lpstrCustomFilter = NULL ;
		ofn.nMaxCustFilter = 0 ;
		ofn.nFilterIndex = 0 ;
		ofn.lpstrFile = szFileName ;
		ofn.nMaxFile = MAX_PATH ;
		ofn.lpstrFileTitle = szTitleName ;
		ofn.nMaxFileTitle = MAX_PATH ;
		ofn.lpstrInitialDir = NULL ;
		ofn.lpstrTitle = NULL ;
		ofn.Flags = 0 ;
		ofn.nFileOffset = 0 ;
		ofn.nFileExtension = 0 ;
		ofn.lpstrDefExt = TEXT ("bmp") ;
		ofn.lCustData = 0 ;
		ofn.lpfnHook = NULL ;
		ofn.lpTemplateName = NULL ;
		return 0 ;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_FILE_OPEN:
			//Show the file open dialog box
			if(!GetOpenFileName(&ofn))
				return 0;
			//If there's an existing packed DIB, free the memory
			if(pPackedDib)
			{
				free(pPackedDib);
				pPackedDib= NULL;
			}
			//If there's an existing logical palette, delete it
			if(hPalette)
			{
				DeleteObject(hPalette);
				hPalette= NULL;
			}
			//Load the packedDib into memory
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			pPackedDib= PackedDibLoad(szFileName);
			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			if(pPackedDib)
			{
				//Create the palette from the DIB color table
				hPalette= PackedDibCreatePalette(pPackedDib);
				if(hPalette)
				{
					iNumColors= PackedDibGetNumColors(pPackedDib);
					pwIndex= (WORD*)PackedDibGetColorTablePtr(pPackedDib);
					for(i=0;i<iNumColors;i++)
						pwIndex[i]= (WORD)i;
				}
			}
			else
			{
				MessageBox(hwnd, TEXT("Cannot load DIB file."), szAppName,0);
			}
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}
		break;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		if(hPalette)
		{
			SelectPalette(hdc, hPalette, FALSE);
			RealizePalette(hdc);
		}
		if(pPackedDib)
			SetDIBitsToDevice(hdc, 0,0, PackedDibGetWidth(pPackedDib),
			PackedDibGetHeight(pPackedDib),0,0,0,
			PackedDibGetHeight(pPackedDib),PackedDibGetBitsPtr(pPackedDib),
			pPackedDib, DIB_PAL_COLORS);

		EndPaint(hwnd, &ps);
		return 0;
	case WM_QUERYNEWPALETTE:
		if(!hPalette)
			return FALSE;
		hdc= GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		InvalidateRect(hwnd, NULL, TRUE);

		ReleaseDC(hwnd, hdc);
		return TRUE;
	case WM_PALETTECHANGED:
		if(!hPalette||(HWND)wParam== hwnd)
			break;
		hdc= GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		UpdateColors(hdc);

		ReleaseDC(hwnd, hdc);
		break;
	case WM_DESTROY:
		if(pPackedDib)
			free(pPackedDib);
		if(hPalette)
			DeleteObject(hPalette);

		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}