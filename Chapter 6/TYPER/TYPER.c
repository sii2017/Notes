#include <windows.h>
#define BUFFER(x,y) *(pBuffer + y*cxBuffer + x)//pBuffer的开头的地址+行数乘以一行字符的数量+不到一行字符的数量

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("Typer");
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

	hwnd= CreateWindow(szAppName, TEXT("Typing Program"),
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
	static DWORD dwCharSet= DEFAULT_CHARSET;
	static int cxClient, cyClient, cxChar, cyChar, cxBuffer, cyBuffer, xCaret, yCaret;
	static TCHAR*pBuffer= NULL;
	HDC hdc;
	int x,y,i;
	PAINTSTRUCT ps;
	TEXTMETRIC tm;

	switch(message)
	{
	case WM_INPUTLANGCHANGE://输入法发生改变,即输入语言发生改变后则重新进行CREATE等。这是与上一个KEYVIEW1的区别
		dwCharSet= wParam;
	case WM_CREATE:
		//自定字体，然后获得该字体的宽和高，删除并定义回原来的字体
		hdc= GetDC(hwnd);
		SelectObject(hdc, CreateFont(0,0,0,0,0,0,0,0,
			dwCharSet, 0,0,0,FIXED_PITCH, NULL));
		GetTextMetrics(hdc, &tm);
		cxChar= tm.tmAveCharWidth;
		cyChar= tm.tmHeight;
		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		ReleaseDC(hwnd, hdc);
		//fall through
	case WM_SIZE:	//如果最大化最小化甚至调整窗口，内容都会消失并且初始化
		if(message== WM_SIZE)
		{
			cxClient= LOWORD(lParam);
			cyClient= HIWORD(lParam);
		}
		//Calculate windows size
		cxBuffer= max(1,cxClient/cxChar);
		cyBuffer= max(1,cyClient/cyChar);
		//allocate memory for buffer and clear it
		if(pBuffer!=NULL)
			free(pBuffer);
		pBuffer= (TCHAR*)malloc(cxBuffer*cyBuffer*sizeof(TCHAR));
		for(y=0;y<cyBuffer;y++)
			for(x=0;x<cxBuffer;x++)
				BUFFER(x,y)= ' ';
		xCaret= 0;	//如果这里设置为0
		yCaret= 0;

		if(hwnd== GetFocus())	//判断是否获得焦点
			SetCaretPos(xCaret*cxChar, yCaret*cyChar);
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_SETFOCUS:
		//create and show caret
		CreateCaret(hwnd, NULL, cxChar, cyChar);
		SetCaretPos(xCaret*cxChar, yCaret*cyChar);
		ShowCaret(hwnd);
		return 0;
	case WM_KILLFOCUS:
		//hide and destroy
		HideCaret(hwnd);
		DestroyCaret();
		return 0;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_HOME:
			xCaret= 0;
			break;
		case VK_END:
			xCaret=cxBuffer-1;	//最大距离减一
			break;
		case VK_PRIOR:
			yCaret= 0;
			break;
		case VK_NEXT:
			yCaret=cyBuffer-1;
			break;
		case VK_LEFT:
			xCaret= max(xCaret-1,0);
			break;
		case VK_RIGHT:
			xCaret= min(xCaret+1,cxBuffer-1);
			break;
		case VK_UP:
			yCaret= max(yCaret-1,0);
			break;
		case VK_DOWN:
			yCaret= min(yCaret+1,cyBuffer-1);
			break;
		case VK_DELETE://删除光标目前所在的字符，这个功能现在已经不太用了2017-10-31
			for(x=xCaret;x<cxBuffer-1;x++)
				BUFFER(x,yCaret)=BUFFER(x+1,yCaret);
			BUFFER(cxBuffer-1,yCaret)= ' ';
			HideCaret(hwnd);
			hdc= GetDC(hwnd);
			SelectObject(hdc, CreateFont(0,0,0,0,0,0,0,0,dwCharSet,0,0,0,FIXED_PITCH,NULL));
			TextOut(hdc, xCaret*cxChar, yCaret*cyChar,&BUFFER(xCaret, yCaret), cxBuffer-xCaret);
			DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
			ReleaseDC(hwnd, hdc);
			ShowCaret(hwnd);
			break;
		}
		SetCaretPos(xCaret*cxChar, yCaret*cyChar);
		return 0;
	case WM_CHAR:
		for(i=0;i<(int)LOWORD(lParam);i++)
		{
			switch(wParam)
			{
			case '\b'://backspace
				if(xCaret>0)
				{
					xCaret--;
					SendMessage(hwnd, WM_KEYDOWN, VK_DELETE, 1);
				}
				break;
			case '\t'://tab
				do{ 
					SendMessage(hwnd, WM_CHAR, ' ', 1);
				}
				while(xCaret%8!=0);
				break;
			case '\n'://line feed
				if(++yCaret==cyBuffer)
					yCaret==0;
				break;
			case '\r'://carriage return 
				xCaret=0;
				if(++yCaret==cyBuffer)
					yCaret=0;
				break;
			case '\x1B'://escape
				for(y=0;y<cyBuffer;y++)
					for(x=0;x<cxBuffer;x++)
						BUFFER(x,y)=' ';
				xCaret=0;
				yCaret=0;

				InvalidateRect(hwnd, NULL, FALSE);
				break;
			default://character codes
				BUFFER(xCaret,yCaret)=(TCHAR)wParam;
				HideCaret(hwnd);
				hdc=GetDC(hwnd);
				SelectObject(hdc, CreateFont(0,0,0,0,0,0,0,0,dwCharSet,0,0,0,FIXED_PITCH,NULL));
				TextOut(hdc, xCaret*cxChar, yCaret*cyChar,&BUFFER(xCaret, yCaret), 1);
				DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
				ReleaseDC(hwnd, hdc);
				ShowCaret(hwnd);

				if(++xCaret==cxBuffer)
				{
					xCaret=0;
					if(++yCaret==cyBuffer)
						yCaret=0;
				}
				break;
			}
		}
		SetCaretPos(xCaret*cxChar, yCaret*cyChar);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		SelectObject(hdc, CreateFont(0,0,0,0,0,0,0,0,
			dwCharSet, 0,0,0,FIXED_PITCH, NULL));	//创建字体
		for(y=0;y<cyBuffer;y++)
			TextOut(hdc, 0, y*cyChar, &BUFFER(0,y), cxBuffer);
		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));	//重新设置成系统字体并且通过返回值删除上一个自定义自己的gdi对象
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//总结
//这是个简单的文字编辑器，对于部分位移键可以使光标移动，根据光标的位置进行输入，根据光标的位置进行一些功能键比如空格回车以及删除。
//xCaret yCaret 就是光标的坐标，当然如果用他当作坐标进行输入分别需要乘以cxChar和cyChar作为字体的长度宽度
//尚不知到自定义字体的含义。