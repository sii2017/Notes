/*
BITMASK将窗口背景设为亮灰色。这样就确保我们能正确地屏蔽位图，而不只是偷懒将其涂成与背景色一样的白色。
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//窗口创建与平时大致一样，背景画刷颜色改成了灰色
	static TCHAR szAppName[]= TEXT("BitMask");
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
	wndclass.hbrBackground= (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register Wrong"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Bitmap Masking Demonstration"), WS_OVERLAPPEDWINDOW, 
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
	static HBITMAP hBitmapImag, hBitmapMask;
	static HINSTANCE hInstance;
	static int cxClient, cyClient, cxBitmap, cyBitmap;
	BITMAP bitmap;
	HDC hdc, hdcMemImag, hdcMemMask;
	int x,y;
	PAINTSTRUCT ps;
	
	switch(message)
	{
	case WM_CREATE:
		hInstance= ((LPCREATESTRUCT)lParam)->hInstance;
		hBitmapImag= LoadBitmap(hInstance, TEXT("Matthew"));	//获取句柄
		GetObject(hBitmapImag, sizeof(BITMAP), &bitmap);	//获取尺寸
		cxBitmap= bitmap.bmWidth;
		cyBitmap= bitmap.bmHeight;

		hdcMemImag= CreateCompatibleDC(NULL);	//创建内存

		SelectObject(hdcMemImag, hBitmapImag);	//位图选入内存

		hBitmapMask= CreateBitmap(cxBitmap, cyBitmap, 1, 1, NULL);	//创建mask位图
		hdcMemMask= CreateCompatibleDC(NULL);	//创建mask内存
		SelectObject(hdcMemMask, hBitmapMask);	//位图选入内存

		//mask内存中设置黑色画刷，并且填满位图
		SelectObject(hdcMemMask, GetStockObject(BLACK_BRUSH));
		Rectangle(hdcMemMask, 0,0, cxBitmap, cyBitmap);
		//mask中再用白色画刷画个椭圆
		SelectObject(hdcMemMask, GetStockObject(WHITE_BRUSH));
		Ellipse(hdcMemMask, 0,0, cxBitmap, cyBitmap);

		//SRCAND:通过使用AND(与)操作符来将源和目标矩形区域内的颜色合并。
		//用mask里创建的黑色周边白色空白的位图与hdcMemImag里的原位图进行合并。
		//白色的为1（true&&true= true），不会遮住原来的位图，黑色的为0（0&true=0），会遮住原来的位图
		BitBlt(hdcMemImag, 0, 0 ,cxBitmap, cyBitmap, hdcMemMask, 0,0, SRCAND);
		//修改好原原位图后删除两个内存空间
		DeleteDC(hdcMemImag);
		DeleteDC(hdcMemMask);
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);

		hdcMemImag= CreateCompatibleDC(hdc);
		SelectObject(hdcMemImag, hBitmapImag);//创建内存选入原位图

		hdcMemMask= CreateCompatibleDC(hdc);
		SelectObject(hdcMemMask, hBitmapMask);//创建内存选入mask位图

		x= (cxClient- cxBitmap)/2;
		y= (cyClient- cyBitmap)/2;

		BitBlt(hdc, x, y, cxBitmap, cyBitmap, hdcMemMask, 0, 0, 0x220326);
		BitBlt(hdc, x, y, cxBitmap, cyBitmap, hdcMemImag, 0, 0, SRCPAINT);	//SRCPAINT 通过or或来进行合并
		DeleteDC(hdcMemImag);
		DeleteDC(hdcMemMask);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteObject(hBitmapImag);
		DeleteObject(hBitmapMask);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}