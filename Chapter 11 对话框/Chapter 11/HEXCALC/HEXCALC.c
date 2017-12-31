/*
�������ڰ汾��ͬ�����������û�а취˳���򿪣�Ŀǰ��״������ʾ"hexcalc.dlg"�޷���
ֱ��˵��Ҫע��Ľ���
1 ��һЩ����û��ͨ��Developer Studioֱ�Ӽ�������ݣ�������Ҫ����RCֱ��д��
2 HEXCALC.RC����һ��CLASS "HexCalc"����Ŀǰ��ֻ��ͨ��д����ܽ�ȥ�ģ�����û�еط�������ӣ��Ҳ���MFC����Ͳ�����ô�鷳�ˡ�
3 ������HEXCALC��û�а���ͷ�ļ�"resource.h"����Ϊͨ�����Ǹ��ļ������ṩ�����Ӵ��ڿؼ�����ı�ʶ��IDM_XXX��������ʹ�ã�
  �����ڸó����д󲿷ֲ������ǲ����õõ���
 */
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("HexCalc");
	MSG msg;
	HWND hwnd;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= DLGWINDOWEXTRA;	//diff ���ʹ���������͵ĵ���һ����Ҫ
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(hInstance, szAppName);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)(COLOR_BTNFACE+1);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("wrong"), szAppName, MB_ICONERROR);
		return 0;
	}
	
	hwnd= CreateDialog(hInstance, szAppName, 0, NULL);

	ShowWindow(hwnd, iCmdShow);
	//UpdateWindow(hwnd);	//diff
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void ShowNumber(HWND hwnd, UINT iNumber)
{
	TCHAR szBuffer[20];
	wsprintf(szBuffer, TEXT("%X"), iNumber);
	SetDlgItemText(hwnd, VK_ESCAPE, szBuffer);
}

DWORD CalcIt(UINT iFirstNum, int iOperation, UINT iNum)
{
	switch(iOperation)
	{
	case '=': return iNum;
	case '+': return iFirstNum + iNum;
	case '-': return iFirstNum- iNum;
	case '*': return iFirstNum* iNum;
	case '&': return iFirstNum& iNum;
	case '|': return iFirstNum| iNum;
	case '^': return iFirstNum^ iNum;
	case '<': return iFirstNum << iNum ;
	case '>': return iFirstNum >> iNum ;
	case '/': return iNum ? iFirstNum / iNum: MAXDWORD ;
	case '%': return iNum ? iFirstNum % iNum: MAXDWORD ;
	default : return 0 ;
	}
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam,LPARAM lParam)
{
	static BOOL bNewNumber= TRUE;
	static int iOperation= '=';
	static UINT iNumber, iFirstNum;
	HWND hButton;

	switch(message)
	{
	case WM_KEYDOWN:
		if(wParam!= VK_LEFT)
			break;
		wParam= VK_BACK;
		//fall through
	case WM_CHAR:
		if((wParam=(WPARAM)CharUpper((TCHAR*)wParam))==VK_RETURN)
			wParam= '=';
		if(hButton= GetDlgItem(hwnd, wParam))
		{
			SendMessage(hButton, BM_SETSTATE, 1, 0);
			Sleep(100);
			SendMessage(hButton, BM_SETSTATE, 0, 0);
		}
		else
		{
			MessageBeep(0);
			break;
		}
		//fall through
	case WM_COMMAND:
		SetFocus(hwnd);
		if(LOWORD(wParam)== VK_BACK)
			ShowNumber(hwnd, iNumber/= 16);
		else if(LOWORD(wParam)== VK_ESCAPE)
			ShowNumber(hwnd, iNumber=0);
		else if(isxdigit(LOWORD(wParam)))
		{
			if(bNewNumber)
			{
				iFirstNum= iNumber;
				iNumber= 0;
			}
			bNewNumber= FALSE;
			if(iNumber<= MAXDWORD>>4)
				ShowNumber(hwnd, iNumber= 16* iNumber+ wParam- (isdigit(wParam)?'0':'A'-10));
			else
				MessageBeep(0);
		}
		else
		{
			if(!bNewNumber)
				ShowNumber(hwnd,iNumber= CalcIt(iFirstNum, iOperation, iNumber));
			bNewNumber= TRUE;
			iOperation= LOWORD(wParam);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
