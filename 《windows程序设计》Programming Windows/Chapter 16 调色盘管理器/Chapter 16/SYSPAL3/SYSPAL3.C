/*
GetDeviceCaps(hdc, SIZEPALETTE);返回值为0
原因为，只有在设备驱动器在RASTERCAPS索引中设置RC PALETFE位时该索引值才是有效的。且该索引值只能用于16位Windows的驱动器
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("SysPal3");
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

	hwnd= CreateWindow(szAppName, TEXT("System Palette #3"), WS_OVERLAPPEDWINDOW,
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
	static HBITMAP hBitmap;
	static int cxClient, cyClient;
	BYTE bits[256];
	HDC hdc, hdcMem;
	int i;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE:
		if(!CheckDisplay(hwnd))
			return -1;

		for(i=0; i<256; i++)
			bits[i]= i;
		hBitmap= CreateBitmap(16, 16, 1, 8, &bits);
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
		hdcMem= CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmap);

		StretchBlt(hdc, 0,0, cxClient, cyClient, hdcMem, 
			0,0,16,16, SRCCOPY);

		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}