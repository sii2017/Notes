/*
由于加载的是纯资源dll，所以没有lib文件，只有dll
因此，在properties中的link/input/addition depends中，不需要输入XXX.lib
在文件目录下只要复制进dll文件就可以了
*/
#include <windows.h>

LRESULT CALLBACK WndProc(HWND,  UINT , WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("ShowBit");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//正常建立窗口,没有menu
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

	hwnd= CreateWindow(szAppName, TEXT("Show Bitmaps from BITLIB (Press Key)"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL);

	if(!hwnd)
		return 0;
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void DrawBitmap(HDC hdc, int xStart, int yStart, HBITMAP hBitmap)
{
	BITMAP bm;
	HDC hMemDC;
	POINT pt;

	hMemDC= CreateCompatibleDC(hdc);
	SelectObject(hMemDC, hBitmap);
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	pt.x= bm.bmWidth;
	pt.y= bm.bmHeight;

	BitBlt(hdc, xStart, yStart, pt.x, pt.y, hMemDC, 0, 0, SRCCOPY);
	DeleteDC(hMemDC);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hLibrary;
	static int iCurrent= 1;
	HBITMAP hBitmap;
	HDC hdc;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE:
		if((hLibrary= LoadLibrary(TEXT("BITLIB.DLL")))== NULL)	//通过读取dll获取dll的句柄
		{
			MessageBox(hwnd, TEXT("Can not load BITLIB.DLL"), szAppName, 0);
			return -1;
		}
		return 0;
	case WM_CHAR:	//随便按什么键触发
		if(hLibrary)
		{
			iCurrent++;
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);

		if(hLibrary)
		{
			hBitmap= LoadBitmap(hLibrary, MAKEINTRESOURCE(iCurrent));	//第一个参数为句柄

			if(!hBitmap)	//如果图片用完了，那么重新回到1进行循环
			{
				iCurrent= 1;
				hBitmap= LoadBitmap(hLibrary, MAKEINTRESOURCE(iCurrent));
			}
			if(hBitmap)
			{
				DrawBitmap(hdc, 0, 0, hBitmap);
				DeleteObject(hBitmap);
			}
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		if(hLibrary)
			FreeLibrary(hLibrary);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}