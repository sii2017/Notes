/*
这是一个画图的程序。
创建一个屏幕最大尺寸的位图，然后将他作为“阴影位图”
所谓“阴影位图”的意思是在位图上画图，然后投射（复制）到实际的显示页面上去
Q1 为什么不直接画在显示区域，因为这个位图远大于显示的区域，所以就算画到显示区域外了，当我们拖动窗口也能看到之前看不到的
Q2 既然是画在位图上能不能删除画在hdc上的代码，试过但是不行，lineto可能是刷新显示区域的关键。
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//窗口创建与平时大致一样，加入了判断hwnd有没有成功生成
	static TCHAR szAppName[]= TEXT("Sketch");
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

	hwnd= CreateWindow(szAppName, TEXT("Sketch"), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL);

	//new
	if(hwnd== NULL)
	{
		MessageBox(NULL, TEXT("Not enough memory to create bitmap!"), szAppName, MB_ICONERROR);
		return 0;
	}

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg, NULL, 0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void GetLargestDisplayMode(int* pcxBitmap, int* pcyBitmap)
{
	DEVMODE devmode;
	int iModeNum= 0;
	*pcxBitmap= *pcyBitmap= 0;

	ZeroMemory(&devmode, sizeof(DEVMODE));	//用0填充一片内存区域
	devmode.dmSize= sizeof(DEVMODE);

	while(EnumDisplaySettings(NULL, iModeNum++, &devmode))//得到显示设备所有的图形模式信息，一台显示器一般都支持多种分辨率，++是获取下一个分辨率。
	{
		//刚获得的分辨率和当前保存的分辨率比较，取较大的一个
		*pcxBitmap= max(*pcxBitmap, (int)devmode.dmPelsWidth);
		*pcyBitmap= max(*pcyBitmap, (int)devmode.dmPelsHeight);
	}
}

LRESULT CALLBACK WndProc ( HWND hwnd, UINT message, WPARAM wParam,LPARAM lParam)
{
	static BOOL fLeftButtonDown, fRightButtonDown;
	static HBITMAP hBitmap;
	static HDC hdcMem;
	static int cxBitmap, cyBitmap, cxClient, cyClient, xMouse, yMouse;
	HDC hdc;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE:
		GetLargestDisplayMode(&cxBitmap, &cyBitmap);	//获取显示最大的尺寸

		hdc= GetDC(hwnd);
		//创建兼容性位图和内存设备内容
		hBitmap= CreateCompatibleBitmap(hdc, cxBitmap, cyBitmap);
		hdcMem= CreateCompatibleDC(hdc);
		ReleaseDC(hwnd, hdc);

		if(!hBitmap)	//no memory for bitmap
		{
			DeleteDC(hdcMem);
			return -1;
		}
		SelectObject(hdcMem, hBitmap);	//将位图选入内存设备内容
		PatBlt(hdcMem, 0, 0, cxBitmap, cyBitmap, WHITENESS);	//画个白框
		return 0;
	case WM_SIZE:
		//获取显示区域大小
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_LBUTTONDOWN:
		if(!fRightButtonDown)
			SetCapture(hwnd);
		xMouse= LOWORD(lParam);
		yMouse= HIWORD(lParam);
		fLeftButtonDown= TRUE;
		return 0;
	case WM_LBUTTONUP:
		//抬起后不再追踪鼠标以及bool设为false
		if(fLeftButtonDown)
			SetCapture(NULL);
		fLeftButtonDown=FALSE;
		return 0;
	case WM_RBUTTONDOWN:
		if(!fLeftButtonDown)
			SetCapture(hwnd);
		xMouse= LOWORD(lParam);
		yMouse= HIWORD(lParam);
		fRightButtonDown= TRUE;
		return 0;
	case WM_RBUTTONUP:
		if(fRightButtonDown)
			SetCapture(NULL);
		fRightButtonDown= FALSE;
		return 0;
	case WM_MOUSEMOVE:
		if(!fLeftButtonDown&& !fRightButtonDown)
			return 0;
		hdc= GetDC(hwnd);
		SelectObject(hdc, GetStockObject(fLeftButtonDown?BLACK_PEN:WHITE_PEN));
		SelectObject(hdcMem, GetStockObject(fLeftButtonDown?BLACK_PEN:WHITE_PEN));
		MoveToEx(hdc, xMouse, yMouse, NULL);
		MoveToEx(hdcMem, xMouse, yMouse, NULL);

		xMouse= (short)LOWORD(lParam);
		yMouse= (short)HIWORD(lParam);

		LineTo(hdc, xMouse, yMouse);
		LineTo(hdcMem, xMouse, yMouse);

		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		BitBlt(hdc, 0,0, cxClient, cyClient, hdcMem, 0,0, SRCCOPY);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}