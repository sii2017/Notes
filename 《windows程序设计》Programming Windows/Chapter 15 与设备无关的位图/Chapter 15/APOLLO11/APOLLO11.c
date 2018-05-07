/*
这个程序需要特定的图片，我，没，有。
代码层面并没有什么难点
*/
#include <windows.h>
#include "dibfile.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("Apollo11");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//正常注册 毫无不同
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
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register Failed"), szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd= CreateWindow(szAppName, TEXT("Apollo 11"), WS_OVERLAPPEDWINDOW, 
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

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam,LPARAM lParam)
{
	static BITMAPFILEHEADER *pbmfh[2];
	static BITMAPINFO *pbmi[2];
	static BYTE *pBits[2];
	static int cxClient, cyClient, cxDib[2], cyDib[2];
	HDC hdc;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE:
		pbmfh[0]= DibLoadImage(TEXT("Apollo11.bmp"));
		pbmfh[1]= DibLoadImage(TEXT("ApolloTD.bmp"));

		if(pbmfh[0]== NULL || pbmfh[1]== NULL)
		{
			MessageBox(hwnd, TEXT("Cannot load DIB file"), szAppName, 0);
			return 0;
		}
		//Get pointers to the info structure& the bits
		pbmi[0]= (BITMAPINFO*)(pbmfh[0]+1);
		pbmi[1]= (BITMAPINFO*)(pbmfh[1]+1);
		pBits[0]= (BYTE*)pbmfh[0]+ pbmfh[0]->bfOffBits;
		pBits[1]= (BYTE*)pbmfh[1]+ pbmfh[1]->bfOffBits;

		//Get the DIB width and height(assume BITMAPINFOHEADER)
		cxDib[0]= pbmi[0]->bmiHeader.biWidth;
		cxDib[1]= pbmi[1]->bmiHeader.biWidth;

		cyDib[0]= abs(pbmi[0]->bmiHeader.biHeight);
		cyDib[1]= abs(pbmi[1]->bmiHeader.biHeight);
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);

		//Bottom-up DIB full size
		SetDIBitsToDevice(hdc, 0, cyClient/4, cxDib[0], cyDib[0], 0,0,0,
			cyDib[0], pBits[0], pbmi[0], DIB_RGB_COLORS);
		//Bottom-up DIB patial
		SetDIBitsToDevice(hdc, 240, cyClient/4, 80, 166, 80, 60, 0, cyDib[0],
			pBits[0], pbmi[0], DIB_RGB_COLORS);
		//Top-down DIB full size
		SetDIBitsToDevice(hdc, 340, cyClient/4, cxDib[0], cyDib[0], 0,0,0,
			cyDib[0], pBits[0], pbmi[0], DIB_RGB_COLORS);
		//Top-down DIB partial
		SetDIBitsToDevice(hdc, 580, cyClient/4, 80, 166, 80, 60, 0, cyDib[1],
			pBits[1], pbmi[1], DIB_RGB_COLORS);

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		if(pbmfh[0])
			free(pbmfh[0]);
		if(pbmfh[1])
			free(pbmfh[1]);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}