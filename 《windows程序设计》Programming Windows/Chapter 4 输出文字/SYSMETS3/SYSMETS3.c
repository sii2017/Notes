#include <windows.h>
#include "SYSMETS.h"

LRESULT CALLBACK WndProc(WHND, UINT, APARAM, LPARAM);
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("SysMets1");
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

	hwnd= CreateWindow(szAppName, TEXT("Get System Metrics No.1"), WS_OVERLAPPEDWINDOW|WS_VSCROLL
		|WS_HSCROLL,CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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
	static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;
	HDC hdc;
	int i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd;
	PAINTSTRUCT ps;
	SCROLLINFO si;
	TCHAR szBuffer[10];
	TEXTMETRIC tm;
	switch(message)
	{
		case WM_CREATE:
			hdc= GetDC(hwnd);
			GetTextMetrics(hdc, &tm);
			cxChar= tm.tmAveCharWidth;
			cxCaps= (tm.tmPitchAndFamily&1?3:2)* cxChar/2;
			cyChar= tm.tmHeight + tm.tmExternalLeading;
			ReleaseDC(hwnd, hdc);
			//Save the width of the three colums
			iMaxWidth= 40*cxChar + 22*cxCaps;
			return 0;
		case WM_SIZE:
			cxClient= LOWORD(lParam);
			cyClient= HIWORD(lParam);
			//Set vertical scroll bar range and page size
			si.cbSize= sizeof(si);
			si.fMask= SIF_RANGE | SIF_PAGE;
			si.nMin= 0;
			si.nMax= NUMLINES -1;			
			si.nPage= cyClient/cyChar;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			//Set horizontal scroll bar range and page size
			si.cbSize= sizeof(si);
			si.fMask= SIF_RANGE|SIF_PAGE;
			si.nMin= 0;
			si.nMax= 2+ iMaxWidth/cxChar;	
			si.nPage= cxClient/cxChar;
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			return 0;
		case WM_VSCROLL:
			//Get all the vertical bar information
			si.cbSize= sizeof(si);
			si.fMask= SIF_ALL;	//表示Get后将使用si结构的位置、页面大小等量
			GetScrollInfo(hwnd, SB_VERT, &si);
			//Save the position for comparison later on
			iVertPos= si.nPos;
			switch(LOWORD(wParam))
			{
			case SB_TOP:
				si.nPos = si.nMin;
				break;
			case SB_BOTTOM:
				si.nPos= si.nMax;
				break;
			case SB_LINEUP:
				si.nPos-= 1;
				break;
			case SB_LINEDOWN:
				si.nPos+=1;
				break;
			case SB_PAGEUP:
				si.nPos-=si.nPage;
				break;
			case SB_PAGEDOWN:
				si.nPos+=si.nPage;
				break;
			case SB_THUMBTRACK:		//鼠标拖动pos时，拖动即有反应
				si.nPos= si.nTrackPos;
				break;
			default:
				break;
			}
			//Set the position and then retrieve it. Due to adjustments
			//by Windows it may not be the same as the value set
			si.fMask= SIF_POS;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			GetScrollInfo(hwnd, SB_VERT, &si);
			//if the position has changed, scroll the window and update it
			if(si.nPos!= iVertPos)
			{
				ScrollWindow(hwnd, 0, cyChar *(iVertPos-si.nPos), NULL, NULL);
				UpdateWindow(hwnd);
			}
			return 0;
		case WM_HSCROLL:
			//Get all the vertiacal scroll bar information
			si.cbSize=  sizeof(si);
			si.fMask= SIF_ALL;		
			//Save the position for comparison later on
			GetScrollInfo(hwnd, SB_HORZ, &si);
			iHorzPos= si.nPos;
			switch(LOWORD(wParam))
			{
			case SB_LINELEFT:
				si.nPos-=1;
				break;
			case SB_LINERIGHT:
				si.nPos+=1;
				break;
			case SB_PAGELEFT:
				si.nPos-=si.nPage;	//是不是移动的距离？
				break;
			case SB_PAGERIGHT:
				si.nPos= si.nPage;
			case SB_THUMBPOSITION:		//鼠标拖动pos时放开才会有反应
				si.nPos= si.nTrackPos;
				break;
			default:
				break;
			}
			//Set the position and then retrieve it. Due to adjuest ments
			//by Windows it may not be the same as the value set
			si.fMask= SIF_POS;		//当flag为SIF_POS时，使用set能设置pos位置，使用get能获取pos位置
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			GetScrollInfo(hwnd, SB_HORZ, &si);
			//If the position has changed , scroll the window
			if(si.nPos!=iHorzPos)
			{
				ScrollWindow(hwnd, cxChar*(iHorzPos- si.nPos), 0, NULL, NULL);
			}
			return 0;
		case WM_PAINT:
			hdc= BeginPaint(hwnd, &ps);
			//Get vertical scroll bar position
			si.cbSize= sizeof(si);
			si.fMask= SIF_POS;
			GetScrollInfo(hwnd, SB_VERT, &si);
			iVertPos= si.nPos;
			//Get horizental scroll bar position
			GetScrollInfo(hwnd, SB_HORZ, &si);
			iHorzPos= si.nPos;
			//Find painting limits
			iPaintBeg= max(0, iVertPos + ps.rcPaint.top / cyChar);
			iPaintEnd= min(NUMLINES - 1, iVertPos + ps.rcPaint.bottom / cyChar);

			for(i=iPaintBeg;i<=iPaintEnd;i++)
			{
				x=cxChar*(1-iHorzPos);			//根据pos来设置xy
				y=cyChar*(i-iVertPos);
				TextOut(hdc, x, y, sysmetrics[i].szLabel,
					lstrlen(sysmetrics[i].szLabel)); 
				TextOut(hdc, x+22*cxCaps, y, sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));
				SetTextAlign(hdc, TA_RIGHT | TA_TOP);
				TextOut(hdc, x+22*cxCaps + 40*cxChar, y, szBuffer, wsprintf(szBuffer, TEXT("%5d"),
					GetSystemMetrics(sysmetrics[i].iIndex)));
				SetTextAlign(hdc, TA_LEFT | TA_TOP);
			}
			EndPaint(hwnd , &ps);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
		return DefWindowProc(hwnd, message, wParam, lParam);
}