#include <windows.h>
#include <math.h>
#define TWO_PI (2.0*3.14159)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	static TCHAR szAppName[]= TEXT("clover");
	MSG msg;
	HWND hwnd;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.lpfnWndProc= WndProc;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszClassName= szAppName;
	wndclass.lpszMenuName= NULL;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Draw a Clover"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HRGN hRgnClip;
	static int cxClient, cyClient;
	double fAngle, fRadius;
	HCURSOR hCursor;
	HDC hdc;
	HRGN hRgnTemp[6];
	int i;
	PAINTSTRUCT ps;
	switch(iMsg)
	{
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		hCursor= SetCursor(LoadCursor(NULL, IDC_WAIT));//显示等待的光标，现在的最新系统下是个圈圈，但是似乎由于运算速度过快，较少会出现。并保存之前的光标在句柄里
		ShowCursor(TRUE);//显示光标

		if(hRgnClip)
			DeleteObject(hRgnClip);

		hRgnTemp[0]= CreateEllipticRgn(0, cyClient/3, cxClient/2, 2*cyClient/3);
		hRgnTemp[1]= CreateEllipticRgn(cxClient/2, cyClient/3, cxClient, 2*cyClient/3);
		hRgnTemp[2]= CreateEllipticRgn(cxClient/3, 0, 2*cxClient/3, cyClient/2);
		hRgnTemp[3]= CreateEllipticRgn(cxClient/3, cyClient/2, 2*cxClient/3, cyClient);
		hRgnTemp[4]= CreateRectRgn(0,0,1,1);//初始化一个坐标
		hRgnTemp[5]= CreateRectRgn(0,0,1,1);
		hRgnClip= CreateRectRgn(0,0,1,1);
		//根据需要将区域连接起来
		CombineRgn(hRgnTemp[4], hRgnTemp[0], hRgnTemp[1], RGN_OR);
		CombineRgn(hRgnTemp[5], hRgnTemp[2], hRgnTemp[3], RGN_OR);
		CombineRgn(hRgnClip, hRgnTemp[4], hRgnTemp[5],RGN_XOR);

		for(i=0;i<6;i++)
			DeleteObject(hRgnTemp[i]);

		SetCursor(hCursor);//使用之前保存的光标句柄，并恢复
		ShowCursor(FALSE);
		return 0;

	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		
		SetViewportOrgEx(hdc, cxClient/2, cyClient/2, NULL);
		SelectClipRgn(hdc, hRgnClip);//将区域放进设备内容hdc中
		fRadius= _hypot(cxClient/2.0, cyClient/2.0);
		for(fAngle= 0.0; fAngle<TWO_PI; fAngle+= TWO_PI/360)
		{
			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, (int)(fRadius*cos(fAngle)+0.5), (int)(-fRadius*sin(fAngle)+0.5));
		}

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteObject(hRgnClip);//最后删除区域
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}