/*
Dialogbox��Ҫ����"resource.h"������ʹ��CreateDialog����Ҫ��ֻҪд�����־Ϳ�����
�����ĶԻ�����ؼǵ�����Ϊvisible����Ȼ��ʾ������
���ֵ���ʹ�ù��ĺ���SetScrollPos���봰����Ĺ������ؼ�һ����������λ��
δ���ֹ������þ�̬�ӿؼ����ֵĺ���SetDlgItemInt
*/
//-------------------------------------------

#include <windows.h>
//#include "resource.h"  ���÷�ģ̬�Ի��򲢲���Ҫ��Դͷ�ļ���������Ҫ����Դͷ�ļ��н��ж���
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ColorScrDlg(HWND, UINT, WPARAM, LPARAM);
HWND hDlgModeless;	//ȫ�ֱ�����������Ի�����

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("Colors2");
	MSG msg;
	HWND hwnd;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= CreateSolidBrush(0L);	//diff
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("wrong"), szAppName, MB_ICONERROR);
		return 0;
	}
	
	hwnd= CreateWindow(szAppName, TEXT("Color Scroll"),
		WS_OVERLAPPEDWINDOW| WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	
	hDlgModeless= CreateDialog(hInstance, TEXT("ColorScrDlg"), hwnd, ColorScrDlg);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		if(hDlgModeless==0||!IsDialogMessage(hDlgModeless, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		DeleteObject((HGDIOBJ)SetClassLong(hwnd, GCL_HBRBACKGROUND, 
			(LONG)GetStockObject(WHITE_BRUSH)));
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK ColorScrDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int iColor[3];
	HWND hwndParent, hCtrl;
	int iCtrlID, iIndex;
	switch(message)
	{
	case WM_INITDIALOG:
		for(iCtrlID= 10; iCtrlID< 13; iCtrlID++)
		{
			hCtrl= GetDlgItem(hDlg, iCtrlID);	//��ȡÿ���Ի����ӿؼ��ľ����ע�����ﲢ���ǽ������ӿؼ��Ѿ����ŶԻ���һͬ������
			SetScrollRange(hCtrl, SB_CTL, 0, 255, FALSE);
			SetScrollPos(hCtrl, SB_CTL, 0, FALSE);
		}
	    return TRUE;
	case WM_VSCROLL:	//�봰������ӿؼ�������һ�����Ի�������ӿؼ�������Ҳ��ֱ�ӷ�����Ϣ�������ڽ��д����
		//��ȡ�����ID�������ھ��
		hCtrl= (HWND)lParam;
		iCtrlID= GetWindowLong(hCtrl, GWL_ID);
		iIndex= iCtrlID- 10;
		hwndParent= GetParent(hDlg);
		

		switch(LOWORD(wParam))
		{
		case SB_PAGEDOWN:
			iColor[iIndex]+= 15;
		case SB_LINEDOWN:
			iColor[iIndex]= min(255, iColor[iIndex]+1);
			break;
		case SB_PAGEUP:
			iColor[iIndex]-=15;
		case SB_LINEUP:
			iColor[iIndex]= max(0, iColor[iIndex]- 1);
			break;
		case SB_TOP:
			iColor[iIndex]= 0;
			break;
		case SB_BOTTOM:
			iColor[iIndex]= 255;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			iColor[iIndex]= HIWORD(wParam);
			break;
		default:
			return FALSE;
		}
		SetScrollPos(hCtrl, SB_CTL, iColor[iIndex], TRUE);
		SetDlgItemInt(hDlg, iCtrlID+3, iColor[iIndex], FALSE);
		DeleteObject((HGDIOBJ)SetClassLong(hwndParent, GCL_HBRBACKGROUND, 
			(LONG)CreateSolidBrush(RGB(iColor[0], iColor[1], iColor[2]))));
		InvalidateRect(hwndParent, NULL, TRUE);
		return TRUE;
	}
	return FALSE;
}

