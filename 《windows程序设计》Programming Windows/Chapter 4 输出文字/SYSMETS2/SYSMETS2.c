#include <windows.h>
#include "SYSMETS.h"

LRESULT CALLBACK WndProc(WHND, UINT, APARAM, LPARAM);
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("SYSMETS2");
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
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL,TEXT("This program require Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Get System Metrics No.2"), WS_OVERLAPPEDWINDOW|WS_VSCROLL	//增加了垂直滚动条模式， HSCROLL为水平滚动条
		,CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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
	static int cxChar, cxCaps, cyChar, cyClient, iVscrollPos;
	HDC hdc;
	int i,y;
	PAINTSTRUCT ps;
	TCHAR szBuffer[10];
	TEXTMETRIC tm;
	switch(message)
	{
	case WM_CREATE:
		hdc= GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		cxChar= tm.tmAveCharWidth;
		cxCaps= (tm.tmPitchAndFamily&1?3:2)*cxChar/2;
		cyChar= tm.tmHeight+tm.tmExternalLeading;		//每隔多少像素输出一行
		ReleaseDC(hwnd, hdc);

		SetScrollRange(hwnd, SB_VERT,0,NUMLINES-1,FALSE);
		SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		for(i=0;i<NUMLINES; ++i)
		{
			y= cyChar*(i-iVscrollPos);	//y的大小跟着pos来变化
			TextOut(hdc, 0, y, sysmetrics[i].szLabel, lstrlen(sysmetrics[i].szLabel));
			TextOut(hdc, 22*cxCaps, y,sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));
			
			SetTextAlign(hdc, TA_RIGHT|TA_TOP);			//在第三列打印之前改变hdc使之右对齐
			TextOut(hdc, 22*cxCaps+40*cxChar, y, szBuffer, wsprintf(szBuffer, TEXT("%5d"),
				GetSystemMetrics(sysmetrics[i].iIndex)));
			SetTextAlign(hdc, TA_LEFT|TA_TOP);		//打印好第三列以后重新左对齐前两列，还有这种操作。
		}
		
		EndPaint(hwnd, &ps);
		return 0;
	case WM_SIZE:
		cyClient= HIWORD(lParam);	//这里只获取高度变化，高度变化可以显示的行数有区别，但是宽度变化
		return 0;
	case WM_VSCROLL:			//1 收到滚动条消息
		switch(LOWORD(wParam))	//2 从滚动条消息的低字节wParam获取通知码进行处理
		{
		case SB_LINEUP:
			iVscrollPos -= 1;	//3 根据通知码改变pos的值
			break;
		case SB_LINEDOWN:
			iVscrollPos += 1;
			break;
		case SB_PAGEUP:
			iVscrollPos -= cyClient/cyChar;
			break;
		case SB_PAGEDOWN:
			iVscrollPos += cyClient/cyChar;
			break;
		case SB_THUMBPOSITION:	//仅仅处理放开鼠标后的状态
			iVscrollPos= HIWORD(wParam);
			break;
		default:
			break;
		}
		iVscrollPos= max(0, min(iVscrollPos, NUMLINES-1));		//用max和min宏来调整位置确保在范围内
		if(iVscrollPos!= GetScrollPos(hwnd, SB_VERT))		//把临时的iVscrollPos和之前的Pos进行比较，如果不同则更新pos并且将显示区域无效化
		{
			SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);	//4 消息中改变了pos的数值，这里通过函数改变pos滑块的位置
			InvalidateRect(hwnd, NULL, TRUE);		//5 立刻刷新 这条使windows发出WM_PAINT消息
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}