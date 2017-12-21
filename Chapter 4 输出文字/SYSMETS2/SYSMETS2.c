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

	hwnd= CreateWindow(szAppName, TEXT("Get System Metrics No.2"), WS_OVERLAPPEDWINDOW|WS_VSCROLL	//�����˴�ֱ������ģʽ�� HSCROLLΪˮƽ������
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
		cyChar= tm.tmHeight+tm.tmExternalLeading;		//ÿ�������������һ��
		ReleaseDC(hwnd, hdc);

		SetScrollRange(hwnd, SB_VERT,0,NUMLINES-1,FALSE);
		SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		for(i=0;i<NUMLINES; ++i)
		{
			y= cyChar*(i-iVscrollPos);	//y�Ĵ�С����pos���仯
			TextOut(hdc, 0, y, sysmetrics[i].szLabel, lstrlen(sysmetrics[i].szLabel));
			TextOut(hdc, 22*cxCaps, y,sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));
			
			SetTextAlign(hdc, TA_RIGHT|TA_TOP);			//�ڵ����д�ӡ֮ǰ�ı�hdcʹ֮�Ҷ���
			TextOut(hdc, 22*cxCaps+40*cxChar, y, szBuffer, wsprintf(szBuffer, TEXT("%5d"),
				GetSystemMetrics(sysmetrics[i].iIndex)));
			SetTextAlign(hdc, TA_LEFT|TA_TOP);		//��ӡ�õ������Ժ����������ǰ���У��������ֲ�����
		}
		
		EndPaint(hwnd, &ps);
		return 0;
	case WM_SIZE:
		cyClient= HIWORD(lParam);	//����ֻ��ȡ�߶ȱ仯���߶ȱ仯������ʾ�����������𣬵��ǿ�ȱ仯
		return 0;
	case WM_VSCROLL:			//1 �յ���������Ϣ
		switch(LOWORD(wParam))	//2 �ӹ�������Ϣ�ĵ��ֽ�wParam��ȡ֪ͨ����д���
		{
		case SB_LINEUP:
			iVscrollPos -= 1;	//3 ����֪ͨ��ı�pos��ֵ
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
		case SB_THUMBPOSITION:	//��������ſ������״̬
			iVscrollPos= HIWORD(wParam);
			break;
		default:
			break;
		}
		iVscrollPos= max(0, min(iVscrollPos, NUMLINES-1));		//��max��min��������λ��ȷ���ڷ�Χ��
		if(iVscrollPos!= GetScrollPos(hwnd, SB_VERT))		//����ʱ��iVscrollPos��֮ǰ��Pos���бȽϣ������ͬ�����pos���ҽ���ʾ������Ч��
		{
			SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);	//4 ��Ϣ�иı���pos����ֵ������ͨ�������ı�pos�����λ��
			InvalidateRect(hwnd, NULL, TRUE);		//5 ����ˢ�� ����ʹwindows����WM_PAINT��Ϣ
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}