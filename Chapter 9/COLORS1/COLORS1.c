#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ScrollProc(HWND, UINT, WPARAM, LPARAM);//子控件滚动条消息处理

int idFocus;
WNDPROC OldScroll[3];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("Colors1");
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
	hwnd= CreateWindow(szAppName, TEXT("Color Scroll"), WS_OVERLAPPEDWINDOW, 
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
	static COLORREF crPrim[3]= {RGB(255,0,0), RGB(0,255,0), RGB(0,0,255)};
	static HBRUSH hBrush[3], hBrushStatic;
	static HWND hwndScroll[3], hwndLabel[3], hwndValue[3], hwndRect;
	static int color[3],cyChar;
	static RECT rcColor;
	static TCHAR* szColorLabel[]= {TEXT("Red"), TEXT("Green"), TEXT("Blue")};
	HINSTANCE hInstance;
	int i, cxClient, cyClient;
	TCHAR szBuffer[10];

	switch(message)
	{
	case WM_CREATE:		//一共使用了10个子窗口，其中6个静态文字框窗口，3个滚动条窗口，1个矩形框
		hInstance= (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		hwndRect= CreateWindow(TEXT("static"), NULL, WS_CHILD|WS_VISIBLE|SS_WHITERECT, 0, 0, 0, 0, 
			hwnd, (HMENU)9, hInstance, NULL);

		for(i=0; i<3; i++)
		{
			//the 3 scroll bars have IDs 0,1,2 with scroll bar ranges from 0 through 255
			hwndScroll[i]= CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD|WS_VISIBLE|WS_TABSTOP|SBS_VERT,
				0,0,0,0, hwnd, (HMENU)i, hInstance, NULL);
			//这里设置滚动条的参数与窗口滚动条的参数不同
			SetScrollRange(hwndScroll[i], SB_CTL, 0, 255, FALSE);
			SetScrollPos(hwndScroll[i], SB_CTL, 0, FALSE);
			//三个静态文字框，blue red green，子ID分别是345
			hwndLabel[i]= CreateWindow(TEXT("static"), szColorLabel[i], WS_CHILD|WS_VISIBLE|SS_CENTER,	//居中
				0,0,0,0, hwnd, (HMENU)(i+3), hInstance, NULL);
			//三个静态文字框，代表0-255的数字，起始为0，子ID是678
			hwndValue[i]= CreateWindow(TEXT("static"), TEXT("0"), WS_CHILD|WS_VISIBLE|SS_CENTER, 
				0,0,0,0, hwnd, (HMENU)(i+6), hInstance, NULL);

			OldScroll[i]= (WNDPROC)SetWindowLong(hwndScroll[i], GWL_WNDPROC, (LONG)ScrollProc);
			hBrush[i]= CreateSolidBrush(crPrim[i]);
		}
		hBrushStatic= CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));
		cyChar= HIWORD(GetDialogBaseUnits());
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		SetRect(&rcColor, cxClient/2, 0, cxClient, cyClient);
		MoveWindow(hwndRect, 0, 0, cxClient/2, cyClient, TRUE);

		for(i=0;i<3;i++)
		{
			MoveWindow(hwndScroll[i], (2*i+1)*cxClient/14, 2*cyChar, cxClient/14, cyClient-4*cyChar,TRUE);
			MoveWindow(hwndLabel[i], (4*i+1)*cxClient/28, cyChar/2, cxClient/7, cyChar, TRUE);
			MoveWindow(hwndValue[i], (4*i+1)*cxClient/28, cyClient-3*cyChar/2, cxClient/7, cyChar, TRUE);
		}

		SetFocus(hwnd);
		return 0;
	case WM_SETFOCUS:	//当父窗口获得输入焦点则把输入焦点交给滚动条子窗口，始终把键盘输入焦点给子窗口
		SetFocus(hwndScroll[idFocus]);
		return 0;
	case WM_VSCROLL:
		i= GetWindowLong((HWND)lParam, GWL_ID);	//子窗口id
		
		switch(LOWORD(wParam))
		{
		case SB_PAGEDOWN:
			color[i] += 15;
		case SB_LINEDOWN:
			color[i]= min(255, color[i] +1);
			break;
		case SB_PAGEUP:
			color[i] -= 15;
		case SB_LINEUP:
			color[i]= max(0, color[i] -1);
			break;
		case SB_TOP:
			color[i]= 0;
			break;
		case SB_BOTTOM:
			color[i]= 255;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			color[i]= HIWORD(wParam);
			break;
		default:
			break;
		}
		SetScrollPos(hwndScroll[i], SB_CTL, color[i], TRUE);
		wsprintf(szBuffer, TEXT("%i"), color[i]);
		SetWindowText(hwndValue[i], szBuffer);

		DeleteObject((HBRUSH)SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(color[0], color[1], color[2]))));	//设置新的画刷并且删除旧的画刷（返回值）
		InvalidateRect(hwnd, &rcColor, TRUE);
		return 0;
	case WM_CTLCOLORSCROLLBAR://************拦截用来改变滚动条控件的各种颜色*********************返回画刷给自动调用它的PAINT
		i= GetWindowLong((HWND)lParam, GWL_ID);
		return (LRESULT)hBrush[i];
	case WM_CTLCOLORSTATIC:
		i= GetWindowLong((HWND)lParam, GWL_ID);
		if(i>=3&& i<=8)	//static text controls
		{
			SetTextColor((HDC)wParam, crPrim[i%3]);
			SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNHIGHLIGHT));
			return (LRESULT)hBrushStatic;
		}
		break;
	case WM_SYSCOLORCHANGE:
		DeleteObject(hBrushStatic);
		hBrushStatic= CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));
		return 0;
	case WM_DESTROY:
		DeleteObject((HBRUSH)SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(WHITE_BRUSH)));

		for(i=0;i<3;i++)
			DeleteObject(hBrush[i]);

		DeleteObject(hBrushStatic);
		PostQuitMessage(0);
		return 0;
	}
	return  DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ScrollProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int id= GetWindowLong(hwnd, GWL_ID);
	switch(message)
	{
	case WM_KEYDOWN:
		if(wParam==VK_TAB)
			SetFocus(GetDlgItem(GetParent(hwnd), (id+(GetKeyState(VK_SHIFT)<0?2:1))%3));
		break;
	case WM_SETFOCUS:
		idFocus= id;
		break;
	}
	return CallWindowProc(OldScroll[id], hwnd, message, wParam, lParam);
}


//0 在窗口内建立三个子窗口滚动条控件，分别代表RGB的三种颜色，拖动这三种可以进行颜色的搭配。
//1 通过预定好的scrollbar和static这两个classname来创建子窗口
//2 通过movewindows来改变窗口的位置以及大小
//3 虽然滚动条控件也有自己的消息处理程序，但是滚动的消息仍旧是发给父窗口的
//4 SetClassLong来设置新的子控件背景画刷，并删除返回值即上一个旧的画刷句柄
//5 滚动条控件创建中放入WS_TABSTOP，可以使该滚动条拥有焦点时，在该滚动条的小方框上显示一个闪烁的灰色块