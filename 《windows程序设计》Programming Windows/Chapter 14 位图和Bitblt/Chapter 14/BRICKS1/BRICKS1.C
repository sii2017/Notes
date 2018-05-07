/*
简单的用程序描述了一下
1 关于位图的制作
2 创建内存设备内容以及将位图拉入内存中
3 将内存设备内容的位图复制到hdc设备内容
4 画面真是瞎了我的眼噢
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("Bricks1");
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
		MessageBox(NULL, TEXT("Register Wrong"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("LoadBitmap Demo"), WS_OVERLAPPEDWINDOW, 
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap;
	static int cxClient, cyClient, cxSource, cySource;
	BITMAP bitmap;
	HDC hdc, hdcMem;
	HINSTANCE hInstance;
	int x,y;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE:
		hInstance= ((LPCREATESTRUCT)lParam)->hInstance;
		hBitmap= LoadBitmap(hInstance, TEXT("Bricks"));	//获取位图句柄
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);	//根据位图句柄获取位图信息
		cxSource= bitmap.bmWidth;	//保存位图长和宽
		cySource= bitmap.bmHeight;
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		hdcMem= CreateCompatibleDC(hdc);	//创建一个兼容hdc的内存设备内容
		SelectObject(hdcMem, hBitmap);	//将位图选入内存设备内容

		for(y=0; y<cyClient; y+= cySource)	//8个像素8个像素进行传输
			for(x=0; x<cxClient; x+= cxSource)
			{
				//将来源地hdcMem的位图复制到hdc设备内容中显示
				BitBlt(hdc, x, y, cxSource, cySource, hdcMem, 0,0, SRCCOPY);
			}

		DeleteDC(hdcMem);	//删除
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}