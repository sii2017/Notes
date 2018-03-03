/*
与GRAYS2相似
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("Grays3");
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

	hwnd= CreateWindow(szAppName, TEXT("Shades of Gray #3"), WS_OVERLAPPEDWINDOW,
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
	static HPALETTE hPalette;
	static int cxClient, cyClient;
	HBRUSH hBrush;
	HDC hdc;
	int i;
	LOGPALETTE* plp;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		//Set up a LOGPALETTE struture and create a palette
		//下面用到了65种颜色，所以除了初始的LOGPALETTE种已经包含了一个PALETTEENTRY，再加上另外申请的64个一共为65个
		plp= malloc(sizeof(LOGPALETTE)+ 64*sizeof(PALETTEENTRY));
		plp->palVersion= 0x0300;//表示兼容windows3.0
		plp->palNumEntries= 65;//表示调色盘中的项目数

		//初始化调色盘项目
		for(i=0; i<65; i++)
		{
			plp->palPalEntry[i].peRed= (BYTE)min(255,4*i);
			plp->palPalEntry[i].peGreen= (BYTE)min(255, 4*i);
			plp->palPalEntry[i].peBlue= (BYTE)min(255, 4*i);
			plp->palPalEntry[i].peFlags= 0;
		}
		hPalette= CreatePalette(plp);
		free(plp);
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		//Select and realize the palette in the device context
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		//Draw the fountain of grays
		for(i=0; i<65; i++)
		{
			rect.left= i*cxClient/65;
			rect.top= 0;
			rect.right= (i+1)*cxClient/65;
			rect.bottom= cyClient;
			/*hBrush= CreateSolidBrush(RGB(min(255,4*i), min(255, 4*i),
				min(255, 4*i)));*/
			//这里改为使用了索引，因为调色板中原本就有65个颜色，所以索引就是0-64，一个个使用过去
			hBrush= CreateSolidBrush(PALETTEINDEX(i));//new
			FillRect(hdc, &rect, hBrush);
			DeleteObject(hBrush);
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_QUERYNEWPALETTE:
		if(!hPalette)
			return FALSE;
		hdc= GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);//将逻辑调色板选入设备内容
		RealizePalette(hdc);//将逻辑调色板加载到系统调色板
		InvalidateRect(hwnd, NULL, TRUE);

		ReleaseDC(hwnd, hdc);
		return TRUE;
	case WM_PALETTECHANGED:
		//如果逻辑调色板还没有生成，或者wParam当前活动窗口句柄是本程序的句柄
		//这个消息是意味着调色板被改变了，如果是本程序的句柄则说明是我们自己改变的不用做处理
		//如果当前活动窗口句柄不是本程序，而是其他程序那么就要做一些操作了
		if(!hPalette|| (HWND)wParam== hwnd)
			break;
		hdc= GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		UpdateColors(hdc);

		ReleaseDC(hwnd, hdc);
		break;
	case WM_DESTROY:
		DeleteObject(hPalette);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}