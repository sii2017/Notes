/*
这个程序一样做了个窗口，然后通过循环来使用SetDIBitsToDevice函数画出图像。
需要注意的是，WM_PAINT里面也有一个SetDIBitsToDevice让我一度很疑惑，最终了解到：
WM_COMMAND里的SetDIBitsToDevice是真正用来画图像的，WM_PAINT里面的SetDIBitsToDevice则是在调整大小的时候重绘图像
如果少了WM_COMMAND里的SetDIBitsToDevice则不会有图像，而如果少了WM_PAINT里的SetDIBitsToDevice则一开始有图像，调整窗口大小后没图像。
*/
#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("SeqDisp");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HACCEL hAccel;
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
	hwnd= CreateWindow(szAppName, TEXT("DIB Headers"), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	hAccel= LoadAccelerators(hInstance, szAppName);
	while(GetMessage(&msg, NULL, 0,0))
	{
		if(!TranslateAccelerator(hwnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BITMAPINFO* pbmi;
	static BYTE* pBits;
	static int cxDib, cyDib, cBits;
	static OPENFILENAME ofn;
	static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];
	static TCHAR szFilter[]= TEXT("Bitmap Files(*.BMP)\0*.BMP\0")
		TEXT("All Files(*.*)\0*.*\0\0");
	BITMAPFILEHEADER bmfh;
	BOOL bSuccess, bTopDown;
	DWORD dwBytesRead;
	HANDLE hFile;
	HDC hdc;
	HMENU hMenu;
	int iInfoSize, iRowLength, y, iBitsSize;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE:
		ofn.lStructSize= sizeof(OPENFILENAME);
		ofn.hInstance= NULL;
		ofn.lpstrFilter= szFilter;
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
		return 0;
	case WM_COMMAND:
		hMenu= GetMenu(hwnd);
		switch(LOWORD(wParam))
		{
		case IDM_FILE_OPEN:
			//Display file open dialog
			if(!GetOpenFileName(&ofn))
				return 0;
			//Get rid of old DIB
			if(pbmi)
			{
				free(pbmi);
				pbmi= NULL;
			}
			if(pBits)
			{
				free(pBits);
				pBits= NULL;
			}
			//Generate WM_PAINT message to erase background
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			//Open the file
			hFile= CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if(hFile== INVALID_HANDLE_VALUE)
			{
				MessageBox(hwnd, TEXT("Cannot open file."), szAppName, 
					MB_ICONWARNING| MB_OK);
				return 0;
			}
			//Read in the BITMAPFILEHEADER
			bSuccess= ReadFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);
			if(!bSuccess|| dwBytesRead!= sizeof(BITMAPFILEHEADER))
			{
				MessageBox(hwnd, TEXT("Cannot read file."), szAppName, 
					MB_ICONWARNING| MB_OK);
				CloseHandle(hFile);
				return 0;
			}
			//Check that it is a bitmap
			if(bmfh.bfType!= *(WORD*)"BM")
			{
				MessageBox(hwnd, TEXT("File is not a bitmap."), szAppName, 
					MB_ICONWARNING| MB_OK);
				CloseHandle(hFile);
				return 0;
			}
			//Allocate memory for header and bits
			iInfoSize= bmfh.bfOffBits- sizeof(BITMAPFILEHEADER);
			iBitsSize= bmfh.bfSize- bmfh.bfOffBits;

			pbmi= malloc(iInfoSize);
			pBits= malloc(iBitsSize);

			if(pbmi== NULL|| pBits==NULL)
			{
				MessageBox(hwnd, TEXT("Cannot allocate memory."), szAppName, 
					MB_ICONWARNING| MB_OK);
				if(pbmi)
					free(pbmi);
				if(pBits)
					free(pBits);
				CloseHandle(hFile);
				return 0;
			}
			//Read in the Information Header
			bSuccess= ReadFile(hFile, pbmi, iInfoSize, &dwBytesRead, NULL);
			if(!bSuccess|| (int)dwBytesRead!= iInfoSize)
			{
				MessageBox(hwnd, TEXT("Cannot read file."),
					szAppName, MB_ICONWARNING| MB_OK);
				if(pbmi)
					free(pbmi);
				if(pBits)
					free(pBits);
				CloseHandle(hFile);
				return 0;
			}
			//Get the DIB width and height
			bTopDown= FALSE;
			if(pbmi->bmiHeader.biSize== sizeof(BITMAPCOREHEADER))	//旧版是BITMAPCONREHEADER，新版是BITMAPINFOHEADER，根据大小来判断新的还是旧的
			{
				cxDib= ((BITMAPCOREHEADER*)pbmi)->bcWidth;
				cyDib= ((BITMAPCOREHEADER*)pbmi)->bcHeight;
				cBits= ((BITMAPCOREHEADER*)pbmi)->bcBitCount;
			}
			else
			{
				if(pbmi->bmiHeader.biHeight<0)
					bTopDown= TRUE;
				cxDib= pbmi->bmiHeader.biWidth;
				cyDib= abs(pbmi->bmiHeader.biHeight);
				cBits= pbmi->bmiHeader.biBitCount;

				if(pbmi->bmiHeader.biCompression!= BI_RGB&& pbmi->bmiHeader.biCompression!= BI_BITFIELDS)
				{
					MessageBox(hwnd, TEXT("File is compressed."), szAppName,
						MB_ICONWARNING| MB_OK);
					if(pbmi)
						free(pbmi);
					if(pBits)
						free(pBits);
					CloseHandle(hFile);
					return 0;
				}
			}
			//Get the row length
			iRowLength= ((cxDib*cBits+31)&~31)>>3;
			//Read and display
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			hdc= GetDC(hwnd);
			for(y=0; y<cyDib; y++)	//一行行显示
			{
				ReadFile(hFile, pBits+ y*iRowLength, iRowLength,&dwBytesRead, NULL);
				SetDIBitsToDevice(hdc, 0,0,cxDib, cyDib, 0,0,bTopDown?cyDib-y-1:y,
					1, pBits+y*iRowLength, pbmi, DIB_RGB_COLORS);
			}
			ReleaseDC(hwnd, hdc);
			CloseHandle(hFile);

			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			return 0;
		}
		break;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		if(pbmi&& pBits)	//当窗口调整大小的时候，这里的SetDIBitsToDevice重新绘图（一次性）。
			SetDIBitsToDevice(hdc, 0,0, cxDib, cyDib, 0,0,0,cyDib,pBits,pbmi, DIB_RGB_COLORS);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		if(pbmi)
			free(pbmi);
		if(pBits)
			free(pBits);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}