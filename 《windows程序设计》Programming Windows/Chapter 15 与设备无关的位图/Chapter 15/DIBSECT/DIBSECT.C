/*
DIBSECT除了呼叫CreateDIBSection而不是CreateDIBitmap之外，与DIBCONV程序相似。
*/
#include <windows.h>
#include <commdlg.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("DIBsect");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//常规，无快捷键，有menu
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc ;
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
		MessageBox(NULL, TEXT("Register Failed"), szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd= CreateWindow(szAppName, TEXT("DIB Section Display"), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

HBITMAP CreateDIBsectionFromDibFile(PTSTR szFileName)
{
	BITMAPFILEHEADER bmfh;
	BITMAPINFO* pbmi;
	BYTE* pBits;
	BOOL bSuccess;
	DWORD dwInfoSize, dwBytesRead;
	HANDLE hFile;
	HBITMAP hBitmap;
	//Open the file: read access, prohibit write access
	hFile= CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, 0, NULL);
	if(hFile== INVALID_HANDLE_VALUE)
		return NULL;
	//Read in the BITMAPFILEHEADER
	//1 先将DIB内容读取到bmfh中
	bSuccess= ReadFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);
	if(!bSuccess||(dwBytesRead!= sizeof(BITMAPFILEHEADER))||
		(bmfh.bfType= *(WORD*)"BM"))
	{
		CloseHandle(hFile);
		return NULL;
	}
	//Allocate memory for the BITMAPINFO structure& read it in
	dwInfoSize= bmfh.bfOffBits- sizeof(BITMAPFILEHEADER);
	pbmi= malloc(dwInfoSize);
	bSuccess= ReadFile(hFile, pbmi, dwInfoSize, &dwBytesRead, NULL);
	if(!bSuccess|| (dwBytesRead!= dwInfoSize))
	{
		free(pbmi);
		CloseHandle(hFile);
		return NULL;
	}
	//Create the DIB Section 
	//2 将pbmi作为参数给出，并且关联pBits指针
	hBitmap= CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	if(hBitmap== NULL)
	{
		free(pbmi);
		CloseHandle(hFile);
		return NULL;
	}
	//Read in the bitmap bits
	//3 将DIB中的图素存入pBits指针中
	ReadFile(hFile, pBits, bmfh.bfSize- bmfh.bfOffBits, &dwBytesRead, NULL);
	free(pbmi);
	CloseHandle(hFile);
	
	return hBitmap;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap;
	static int cxClient, cyClient;
	static OPENFILENAME ofn;
	static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];
	static TCHAR szFilter[]= TEXT("Bitmap File(*.BMP)\0*.BMP\0")
		TEXT("All File(*.*)\0*.*\0\0");
	BITMAP bitmap;
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE://打开对话框需要的一系列初始化
		ofn.lStructSize= sizeof(OPENFILENAME);
		ofn.hwndOwner= hwnd;
		ofn.hInstance= NULL;
		ofn.lpstrFilter= szFilter;
		ofn.lpstrCustomFilter= NULL;
		ofn.nMaxCustFilter= 0;
		ofn.nFilterIndex= 0;
		ofn.lpstrFile= szFileName;
		ofn.nMaxFile= MAX_PATH;
		ofn.lpstrFileTitle= szTitleName;
		ofn.nMaxFileTitle= MAX_PATH;
		ofn.lpstrInitialDir= NULL;
		ofn.lpstrTitle= NULL;
		ofn.Flags= 0;
		ofn.nFileOffset= 0;
		ofn.nFileExtension= 0;
		ofn.lpstrDefExt= TEXT("bmp");
		ofn.lCustData= 0;
		ofn.lpfnHook= NULL;
		ofn.lpTemplateName= NULL;
		return 0;
	case WM_SIZE://获取显示区域的长和宽
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_COMMAND://处理menu的按键
		switch(LOWORD(wParam))
		{
		case IDM_FILE_OPEN:
			//打开dialog box
			if(!GetOpenFileName(&ofn))
				return 0;
			//if there's an existing DIB, delete it 
			if(hBitmap)
			{
				DeleteObject(hBitmap);
				hBitmap= NULL;
			}
			//Crerate the DDB from the DIB
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			//hdc= GetDC(hwnd);
			hBitmap= CreateDIBsectionFromDibFile(szFileName);
			//ReleaseDC(hwnd, hdc);
			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			//Invalidate the client area for later update
			InvalidateRect(hwnd , NULL, TRUE);
			if(hBitmap== NULL)
			{
				MessageBox(hwnd, TEXT("Cannot load DIB file"), szAppName, 
					MB_OK||MB_ICONEXCLAMATION);
			}
			return 0;
		}
		break;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		if(hBitmap)
		{
			GetObject(hBitmap, sizeof(BITMAP), &bitmap);//选择前面由DIB位图转化的DDB位图
			hdcMem= CreateCompatibleDC(hdc);//创建内存设备内容
			SelectObject(hdcMem, hBitmap);//将之前的DDB位图选入内存设备内容
			BitBlt(hdc, 0,0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0,0, SRCCOPY);//将内存设备内容里的DDB位图画到hdc显示区域里
			DeleteDC(hdcMem);
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		if(hBitmap)
			DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}