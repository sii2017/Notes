#include <windows.h>
#include <math.h>
#define NUM 1000
#define TWOPI  (2*3.14159)
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("LineDemo");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName=NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("wrong"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Line Demonstration"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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
	static int cxClient, cyClient;
	HDC hdc;
	PAINTSTRUCT ps;
	
	switch(message)
	{
	case WM_SIZE:
		cxClient= LOWORD(lParam);	//lParam低字节是client的x坐标
		cyClient= HIWORD(lParam);	//lParam高字节是client的y坐标
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		Rectangle(hdc, cxClient/8, cyClient/8, 7*cxClient/8, 7*cyClient/8);	//矩形

		MoveToEx(hdc, 0, 0, NULL);
		LineTo(hdc, cxClient, cyClient);	//从左上角到右下角的直线	

		MoveToEx(hdc, 0, cyClient, NULL);
		LineTo(hdc, cxClient, 0);	//从左下角到右上角的直线

		Ellipse(hdc, cxClient/8, cyClient/8, 7*cxClient/8, 7*cyClient/8);	//椭圆
		RoundRect(hdc, cxClient/4, cyClient/4, 3*cxClient/4, 3*cyClient/4, cxClient/4, cyClient/4);	//圆角矩形，最后两个参数用来在矩形内做椭圆来形成圆角

		EndPaint(hdc, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}