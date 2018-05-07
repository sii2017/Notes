#include <windows.h>
struct
{
	int iStyle;
	TCHAR* szText;
}
button[]= 
{
	BS_PUSHBUTTON, TEXT("PUSHBUTTON"),
	BS_DEFPUSHBUTTON, TEXT("DEFPUSHBUTTON"),
	BS_CHECKBOX, TEXT("CHECKBOX"),
	BS_AUTOCHECKBOX, TEXT("AUTOCHECKBOX"),
	BS_RADIOBUTTON, TEXT("RADIABUTTON"),
	BS_3STATE, TEXT("3STATE"),
	BS_AUTO3STATE, TEXT("AUTO3STATE"),
	BS_GROUPBOX, TEXT("GROUPBOX"),
	BS_AUTORADIOBUTTON, TEXT("AUTORADIO"),
	BS_OWNERDRAW, TEXT("OWNERDRAW"),
};

#define NUM (sizeof button/sizeof button[0])
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("BtnLook");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG"), szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd= CreateWindow(szAppName, TEXT("Button Look"), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndButton[NUM];
	static RECT rect;
	static TCHAR szTop[]= TEXT("message wParam lParam"), 
		szUnd[]= TEXT("_______ ______ ______"),
		szFormat[]= TEXT("%-16s%04X-%04X %04X-%04X"), 
		szBuffer[50];
	static int cxChar, cyChar;
	HDC hdc;
	PAINTSTRUCT ps;
	int i;
	switch(message)
	{
	case WM_CREATE:
		cxChar= LOWORD(GetDialogBaseUnits());	//该函数返回系统的对话基本单位，该基本单位为系统字体字符的平均宽度和高度
		cyChar= HIWORD(GetDialogBaseUnits());

		for(i=0; i<NUM; i++)
			hwndButton[i]= CreateWindow(
			TEXT("button"), //类别名称
			button[i].szText, //窗口文字
			WS_CHILD| WS_VISIBLE| button[i].iStyle,	//窗口样式
			cxChar, //x坐标位置
			cyChar*(1+2*i), //y坐标位置
			20*cxChar, //宽度
			7*cyChar/4, //高度，当按钮的高度为文字高度的7/4倍看起来最好
			hwnd, //父窗口
			(HMENU)i, //子窗口id这个可以自己定义，这里直接使用12345...
			((LPCREATESTRUCT)lParam)->hInstance,	//**执行实体句柄
			NULL);	//附加参数
		return 0;
	case WM_SIZE:
		rect.left= 24* cxChar;
		rect.top= 2* cyChar;
		rect.right= LOWORD(lParam);
		rect.bottom= HIWORD(lParam);
		return 0;
	case WM_PAINT:
		InvalidateRect(hwnd, &rect, TRUE);

		hdc= BeginPaint(hwnd, &ps);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		SetBkMode(hdc, TRANSPARENT);

		TextOut(hdc, 24*cxChar, cyChar, szTop, lstrlen(szTop));
		TextOut(hdc, 24*cxChar, cyChar, szUnd, lstrlen(szUnd));

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DRAWITEM://一般此消息在控件需要重画时发出，在收到此消息之后控件才会执行重画。如果事先截获此消息，就可以用自己的代码来重画某一个控件
	case WM_COMMAND://如果这个消息是由子窗口控件产生，如button产生则：LOWORD(wParam): 控件ID。HIWORD(wParam): 通知码。lParam: 子窗口句柄。
		ScrollWindow(hwnd, 0, -cyChar, &rect, &rect);

		hdc= GetDC(hwnd);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

		TextOut(hdc, 24*cxChar, cyChar*(rect.bottom/cyChar-1), szBuffer, 
			wsprintf(szBuffer, szFormat, message==WM_DRAWITEM?TEXT("WM_DRAWITEM"):TEXT("WM_COMMAND"), 
			HIWORD(wParam), LOWORD(wParam), HIWORD(lParam), LOWORD(lParam)));

		ReleaseDC(hwnd, hdc);
		ValidateRect(hwnd, &rect);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}