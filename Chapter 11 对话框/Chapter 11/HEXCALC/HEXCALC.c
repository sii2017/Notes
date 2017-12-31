/*
可能由于版本不同，这个程序我没有办法顺利打开，目前的状况是显示"hexcalc.dlg"无法打开
直接说需要注意的结论
1 有一些我们没法通过Developer Studio直接加入的内容，我们需要进入RC直接写入
2 HEXCALC.RC中有一行CLASS "HexCalc"，这目前是只能通过写入才能进去的，外面没有地方可以添加，我猜想MFC里面就不用这么麻烦了。
3 我们在HEXCALC中没有包含头文件"resource.h"，因为通常我们该文件用来提供所有子窗口控件定义的标识符IDM_XXX来给代码使用，
  但是在该程序中大部分操作我们并不用得到。
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
	wndclass.cbWndExtra= DLGWINDOWEXTRA;	//diff 如果使用这种类型的调用一定需要
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
