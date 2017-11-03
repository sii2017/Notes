#include <windows.h>
#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("Check1");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszClassName= szAppName;
	wndclass.lpszMenuName= NULL;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Check1 Mouse Hit-Test Demo"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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
	static BOOL fState[DIVISIONS][DIVISIONS];
	static int cxBlock,cyBlock;
	HDC hdc;
	int x,y;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_SIZE://将当前的窗口大小平均的分为五行五列
		cxBlock= LOWORD(lParam)/DIVISIONS;	
		cyBlock= HIWORD(lParam)/DIVISIONS;	
		return 0;
	case WM_LBUTTONDOWN://当鼠标进行点击时，在当前空格划叉
		x= LOWORD(lParam)/cxBlock;	//当前的像素除以行数等于在第几行
		y= HIWORD(lParam)/cyBlock;
		if(x<DIVISIONS && y<DIVISIONS)
		{
			fState[x][y]^=1;	//fState[x][y]= fState[x][y]^1，通过异或1进行标记
			rect.left= x*cxBlock;
			rect.top= y*cyBlock;
			rect.right= (x+1)*cxBlock;
			rect.bottom= (y+1)*cyBlock;

		    InvalidateRect(hwnd, &rect, FALSE);
		}
		else
			MessageBeep(0);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);

		for(x=0;x<DIVISIONS;x++)
		{
			for(y=0;y<DIVISIONS;y++)
			{
				Rectangle(hdc, x*cxBlock, y*cyBlock, (x+1)*cxBlock, (y+1)*cyBlock);

				if(fState[x][y])//如果是标记的区域，则划交叉线
				{
					MoveToEx(hdc, x*cxBlock, y*cyBlock, NULL);
					LineTo(hdc, (x+1)*cxBlock, (y+1)*cyBlock);
					MoveToEx(hdc, x*cxBlock, (y+1)*cyBlock, NULL);
					LineTo(hdc, (x+1)*cxBlock, y*cyBlock);
				}
			}
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//总结：
//将显示区域平均分为五乘五的方格，进行点击的时候判断在哪个方格并进行标记，然后在WM_PAINT中划交叉斜线。