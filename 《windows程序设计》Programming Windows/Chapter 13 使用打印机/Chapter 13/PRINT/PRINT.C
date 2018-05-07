#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL PrintMyPage(HWND);

extern HINSTANCE hInst;
extern TCHAR szAppName[];
extern TCHAR szCaption[];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	
	wndclass.style= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG!"), szAppName, MB_ICONERROR);
		return 0;
	}
	hInst= hInstance;
	hwnd= CreateWindow(szAppName, szCaption, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void PageGDICalls(HDC hdcPrn, int cxPage, int cyPage)
{
	static TCHAR szTextStr[]= TEXT("Hello, Printer!");
	Rectangle(hdcPrn, 0, 0, cxPage, cyPage);	//包围页面的矩形
	MoveToEx(hdcPrn, 0,0, NULL);
	LineTo(hdcPrn, cxPage, cyPage);	//对角线
	MoveToEx(hdcPrn, cxPage, 0, NULL);
	LineTo(hdcPrn, 0, cyPage);	//对角线

	SaveDC(hdcPrn);	//保存住现在的hdc，因为之后的操作会改变映射模式等等的内容

	SetMapMode(hdcPrn, MM_ISOTROPIC);
	SetWindowExtEx(hdcPrn, 1000, 1000, NULL);
	SetViewportExtEx(hdcPrn, cxPage/2, -cyPage/2, NULL);
	SetViewportOrgEx(hdcPrn, cxPage/2, cyPage/2, NULL);

	Ellipse(hdcPrn, -500, 500, 500, -500);
	SetTextAlign(hdcPrn, TA_BASELINE| TA_CENTER);
	TextOut(hdcPrn, 0, 0, szTextStr, lstrlen(szTextStr));
	RestoreDC(hdcPrn, -1);//恢复之前的hdc
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient;
	HDC hdc;
	HMENU hMenu;
	PAINTSTRUCT ps;
	BOOL ace, abe;
	switch(message)
	{
	case WM_CREATE:
		hMenu= GetSystemMenu(hwnd, FALSE);
		ace= AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		abe= AppendMenu(hMenu, 0, 1, TEXT("&Print"));	//这是加在系统菜单里，即最左上角
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if(wParam==1)	//Print设置的ID 就是1
		{
			if(!PrintMyPage(hwnd))
				MessageBox(hwnd, TEXT("Could not print page!"), szAppName, 
				MB_OK| MB_ICONEXCLAMATION);
			return 0;
		}
		break;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		
		PageGDICalls(hdc, cxClient, cyClient);

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}