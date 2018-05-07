#include <windows.h>
#define ID_TIMER 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID CALLBACK TimerProc(HWND, UINT, UINT, DWORD);	//参数为句柄，message，定时器ID，秒数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("Beeper2");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor= LoadCursor(hInstance, IDC_ARROW);
	wndclass.hIcon= LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hInstance= hInstance;
	wndclass.lpfnWndProc= WndProc;
	wndclass.lpszClassName= szAppName;
	wndclass.lpszMenuName= NULL;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG"), szAppName, MB_ICONERROR);
		return 0;
	}
	
	hwnd= CreateWindow(szAppName, TEXT("Beeper2 Timer Demo"), WS_OVERLAPPEDWINDOW|
		WS_VSCROLL| WS_HSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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
	switch(message)
	{
	case WM_CREATE:
		SetTimer(hwnd, ID_TIMER, 1000, TimerProc);	//设定定时器
		return 0;
	//case WM_TIMER:
	//	MessageBeep(-1);	//发出蜂鸣声
	//	fFlipFlop=!fFlipFlop;	//根据bool值每1秒改变颜色
	//	InvalidateRect(hwnd,NULL, FALSE);
	//	return 0;
	//case WM_PAINT:
	//	hdc= BeginPaint(hwnd, &ps);

	//	GetClientRect(hwnd, &rc);
	//	hBrush= CreateSolidBrush(fFlipFlop ? RGB(255,0,0): RGB(0,0,255));
	//	FillRect(hdc, &rc, hBrush);

	//	EndPaint(hwnd, &ps);
	//	DeleteObject(hBrush);
	//	return 0;
	case WM_DESTROY:
		KillTimer(hwnd, ID_TIMER);	//在WM_DESTROY中要终止所有的定时器
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

VOID CALLBACK TimerProc(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
	static BOOL fFlipFlop= FALSE;
	HBRUSH hBrush;
	HDC hdc;
	RECT rc;

	MessageBeep(-1);  //发出蜂鸣声
	fFlipFlop=!fFlipFlop;  //根据bool值每1秒改变颜色

	GetClientRect(hwnd, &rc);
	hdc= GetDC(hwnd);
	hBrush= CreateSolidBrush(fFlipFlop ? RGB(255,0,0): RGB(0,0,255));
	FillRect(hdc, &rc, hBrush);
	ReleaseDC(hwnd, hdc);
	DeleteObject(hBrush);
}

//Tips
//与BEEPER1相比将一些窗口消息的内容移植到了这个timerproc中，由于不是WM_PAINT消息所以使用了GetDC。