//���뻬�֣���Ӧ�������հ汾��


#include <windows.h>
#include "SYSMETS.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("SysMets");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor= LoadCursor(hInstance, IDC_ARROW);
	wndclass.hIcon= LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hInstance= hInstance;
	wndclass.lpfnWndProc= WndProc;
	wndclass.lpszClassName= szAppName;
	wndclass.lpszMenuName= NULL;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG"), szAppName, MB_ICONERROR);
		return 0;
	}
	
	hwnd= CreateWindow(szAppName, TEXT("Get System Metrics"), WS_OVERLAPPEDWINDOW|
		WS_VSCROLL| WS_HSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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
	int i,x,y,iVertPos, iHorzPos, iPaintBeg, iPaintEnd;
	PAINTSTRUCT ps;
	SCROLLINFO si;
	TCHAR szBuffer[10];
	TEXTMETRIC tm;
	//for mouse wheel logic
	static int iDeltaPerLine, iAccumDelta;
	ULONG ulScrollLines;
	
	switch(message)
	{
	case WM_CREATE:
		hdc= GetDC(hwnd);

		GetTextMetrics(hdc, &tm);
		cxChar= tm.tmAveCharWidth;
		cxCaps= (tm.tmPitchAndFamily&1?3:2)*cxChar/2;//����ǵȿ���ô��д��Ҳ��cxChar��������ǵȿ�����1.5����
		cyChar= tm.tmHeight+tm.tmExternalLeading;

		ReleaseDC(hwnd, hdc);	//Save the width of the three colums

		iMaxWidth= 40*cxChar + 22*cxCaps;
	case WM_SETTINGCHANGE://ϵͳ�������÷����ı�󣬶��㴰�ڻ��յ�
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);//SPI_GETWHEELSCROLLLINES�ɻ�ȡ����������
		if(ulScrollLines)//���ȡ����
			iDeltaPerLine= WHEEL_DELTA / ulScrollLines;//��ȡ���ι�����delta��ֵ
		else
			iDeltaPerLine= 0;
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		//set vertical scroll bar range and page size
		si.cbSize= sizeof(si);
		si.fMask= SIF_RANGE|SIF_PAGE;
		si.nMin= 0;
		si.nMax= NUMLINES -1;
		si.nPage= cyClient/cyChar;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		//set horizontal scroll bar range and page size
		si.cbSize= sizeof(si);
		si.fMask= SIF_RANGE|SIF_PAGE;
		si.nMin= 0;
		si.nMax= 2+iMaxWidth/cxChar;
		si.nPage= cxClient/cxChar;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		return 0;
	case WM_VSCROLL:
		//Get all the vertical scroll bar information
		si.cbSize= sizeof(si);
		si.fMask= SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT, &si);
		//Save the position for comparision later on 
		iVertPos= si.nPos;
		switch(LOWORD(wParam))
		{
		case SB_TOP:
			si.nPos= si.nMin;
			break;
		case SB_BOTTOM:
			si.nPos= si.nMax;
			break;
		case SB_LINEUP:
			si.nPos-= 1;
			break;
		case SB_LINEDOWN:
			si.nPos+= 1;
			break;
		case SB_PAGEUP:
			si.nPos-= si.nPage;
			break;
		case SB_PAGEDOWN:
			si.nPos+= si.nPage;
			break;
		case SB_THUMBTRACK:		//����϶�posʱ���϶����з�Ӧ
			si.nPos= si.nTrackPos;
			break;
		default:
			break;
		}
		//Set position and then retrieve it, Due to adjustments by windows it might not be the same as the value set.
		si.fMask= SIF_POS;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hwnd, SB_VERT, &si);
		//if the position changed, scroll the window and update
		if(si.nPos!=iVertPos)
		{
			ScrollWindow(hwnd, 0, cyChar*(iVertPos-si.nPos),NULL,NULL);
			UpdateWindow(hwnd);
		}
		return 0;
	case WM_HSCROLL:
		//Get all the vertical scroll var information
		si.cbSize= sizeof(si);
		si.fMask= SIF_ALL;
		//Save the position for comparison later on 
		GetScrollInfo(hwnd, SB_HORZ, &si);
		iHorzPos= si.nPos;
		switch(LOWORD(wParam))
		{
		case SB_LINELEFT:
			si.nPos-= 1;
			break;
		case SB_LINERIGHT:
			si.nPos+= 1;
			break;
		case SB_PAGELEFT:
			si.nPos-= si.nPage;
			break;
		case SB_PAGERIGHT:
			si.nPos+ si.nPage;
			break;
		case SB_THUMBPOSITION:
			si.nPos= si.nTrackPos;
			break;
		default:
			break;
		}
		//Set the posotion and then retrieve it. due to adjuestments by windows it might not be 
		//the same as the value set
		si.fMask= SIF_POS;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		GetScrollInfo(hwnd, SB_HORZ, &si);
		//if the position has changed, sroll the window
		if(si.nPos!=iHorzPos)
		{
			ScrollWindow(hwnd, cxChar*(iHorzPos- si.nPos),0,NULL,NULL);
		}
		return 0;
	case WM_KEYDOWN:	//������Ϣ��ת����Ϣ������Ĺ�����case
		switch(wParam)
		{
		case VK_HOME:
			SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0 );
			break;
		case VK_END:
			SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
			break;
		case VK_PRIOR:
			SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
			break;
		case VK_NEXT:
			SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
			break;
		case VK_UP:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
			break;
		case VK_DOWN:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
			break;
		case VK_LEFT:
			SendMessage(hwnd, WM_HSCROLL, SB_PAGEUP, 0);
			break;
		case VK_RIGHT:
			SendMessage(hwnd, WM_HSCROLL, SB_PAGEDOWN, 0);
			break;
		}
		return 0;
	case WM_MOUSEWHEEL:
		if(iDeltaPerLine==0)//�������0˵�������ܲ�֧�ֹ��֡�
			break;
		iAccumDelta += (short)HIWORD(wParam);	//120 or -120�� �����120��ÿһ����40����ô����һ���ܻ�3��
		while(iAccumDelta>=iDeltaPerLine)
		{
			SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
			iAccumDelta -= iDeltaPerLine;	//iDeltaPerLine�����һ�ε�ֵ��ѭ��ֱ��iAccumDelta����
		}
		while(iAccumDelta<= -iDeltaPerLine)
		{
			SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
			iAccumDelta += iDeltaPerLine;
		}
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		//Get vertical scroll bar position
		si.cbSize= sizeof(si);
		si.fMask= SIF_POS;
		GetScrollInfo(hwnd, SB_VERT, &si);
		iVertPos= si.nPos;
		//Get horizontal scroll bar position
		GetScrollInfo(hwnd, SB_HORZ, &si);
		iHorzPos= si.nPos;
		//Find painting limits  ????
		iPaintBeg= max(0, iVertPos+ps.rcPaint.top/cyChar);
		iPaintEnd= min(NUMLINES-1, iVertPos+ps.rcPaint.bottom/cyChar);

		for(i=iPaintBeg;i<=iPaintEnd;++i)
		{
			x=cxChar*(1-iHorzPos);
			y=cyChar*(i-iVertPos);
			
			TextOut(hdc, x, y, sysmetrics[i].szLabel, lstrlen(sysmetrics[i].szLabel));
			TextOut(hdc, x+22*cxCaps,y, sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));

			SetTextAlign(hdc, TA_RIGHT|TA_TOP);

			TextOut(hdc, x+22*cxCaps+40*cxChar,y, szBuffer, 
				wsprintf(szBuffer, TEXT("%5d"),
				GetSystemMetrics(sysmetrics[i].iIndex)));

			SetTextAlign(hdc, TA_LEFT|TA_TOP);
		}

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam,lParam);
}

/* �ܽ�
��ȡÿһ����Ҫ��delta����������40��Ȼ��һ����120���ܾ����С�
*/