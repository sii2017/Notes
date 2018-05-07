/*
GetDeviceCaps(hdc, SIZEPALETTE);返回值为0
原因为，只有在设备驱动器在RASTERCAPS索引中设置RC PALETFE位时该索引值才是有效的。且该索引值只能用于16位Windows的驱动器
这个版本较上一个版本，增加了逻辑调色板并且显现了它
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("SysPal2");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, PSTR szCmdLine, int iCmdShow)
{
	//这里并没有什么区别
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
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register falied"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("System Palette #2"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	if(!hwnd)
		return 0;

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

BOOL CheckDisplay(HWND hwnd)
{
	HDC hdc;
	int iPalSize;

	hdc= GetDC(hwnd);
	iPalSize= GetDeviceCaps(hdc, SIZEPALETTE);//我的这个电脑没法用噢
	ReleaseDC(hwnd, hdc);
	if(iPalSize!= 256)
	{
		MessageBox(hwnd, TEXT("This program requires that the video")
			TEXT("display mode have a 256-color palette."),
			szAppName, MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HPALETTE hPalette;
	static int cxClient, cyClient;
	HBRUSH hBrush;
	HDC hdc;
	int i,x,y;
	LOGPALETTE* plp;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		if(!CheckDisplay(hwnd))
			return -1;
		plp= malloc(sizeof(LOGPALETTE)+ 255*sizeof(PALETTEENTRY));
		plp->palVersion= 0x0300;//兼容windows3.0
		plp->palNumEntries= 256;
		for(i=0; i<256; i++)
		{
			plp->palPalEntry[i].peRed= i;
			plp->palPalEntry[i].peGreen= 0;
			plp->palPalEntry[i].peBlue=0;
			plp->palPalEntry[i].peFlags= PC_EXPLICIT;//逻辑调色盘项目的较低字组指定了一个硬件调色盘索引。此旗标允许应用程序显示硬件调色盘的内容
		}
		hPalette= CreatePalette(plp);
		free(plp);
		return 0;
	case WM_DISPLAYCHANGE:
		if(!CheckDisplay(hwnd))
			DestroyWindow(hwnd);
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);

		for(y=0; y<16; y++)
		{
			for(x=0;x<16;x++)
			{
				hBrush= CreateSolidBrush(PALETTEINDEX(16*y+x));
				SetRect(&rect, x*cxClient/16, y*cyClient/16,
					(x+1)*cxClient/16, (y+1)*cyClient/16);
				FillRect(hdc, &rect, hBrush);
				DeleteObject(hBrush);
			}
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_PALETTECHANGED:
		if((HWND)wParam!= hwnd)
			InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_DESTROY:
		DeleteObject(hPalette);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam,lParam);
}