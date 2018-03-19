#include <windows.h>
#include <math.h>
#include <process.h>

typedef struct
{
	HWND hwnd;
	int cxClient;
	int cyClient;
	int cyChar;
	BOOL bKill;
}
PARAMS, *PPARAMS;
LRESULT APIENTRY WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//正常注册窗口类，没有menu
	static TCHAR szAppName[]= TEXT("Multi2");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
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
		MessageBox(NULL, TEXT("Register Failed"), szAppName,MB_ICONERROR);
		return 0;
	}
	hwnd= CreateWindow(szAppName, TEXT("Multitasking Demo"), WS_OVERLAPPEDWINDOW, 
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

int CheckBottom(HWND hwnd, int cyClient, int cyChar, int iLine)
{
	if(iLine*cyChar+ cyChar>cyClient)
	{
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		iLine= 0;
	}
	return iLine;
}

//windiws1: Display increasing sequence of numbers
void Thread1(PVOID pvoid)
{
	HDC hdc;
	int iNum= 0;, iLine=0;
	PPARAMS pparams;
	TCHAR szBuffer[16];

	pparams= (PPARAMS)pvoid;

	while(!pparams->bKill)
	{
		if(iNum<0)
			iNum=0;
		iLine= CheckBottom(pparams->hwnd, pparams->cyClient, pparams->cyChar, iLine);
		hdc= GetDC(pparams->hwnd);
		TextOut(hdc, 0, iLine*pparams->cyChar, szBuffer,
			wsprintf(szBuffer,TEXT("%d"), iNum++));
		ReleaseDC(pparams->hwnd, hdc);
		iLine++;
	}
	_endTread();
}

LRESULT APIENTRY WndPorc1(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PARAMS params;
	switch(message)
	{
	case WM_CREATE:
		params.hwnd= hwnd;
		params.cyChar= HIWORD(GetDialogBaseUnits());
		_beginthread(Thread1, 0, &params);
		return 0;
	case WM_SIZE:
		params.cyClient= HIWORD(lParam);
		return 0;
	case WM_DESTROY:
		params.bKill= TRUE;
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//windows2: Display increasing sequence of prime numbers
void Thread2(PVOID pvoid)
{
	HDC hdc;
	int iNum= 1, iLine= 0, i, iSqrt;
	PPARAMS pparams;
	TCHAR szBuffer[16];
	pparams= (PPARAMS)pvoid;
	
	while(!pparams->bKill)
	{
		do
		{
			if(++iNum<0)
				iNum= 0;
			iSqrt= (int)sqrt(iNum);

			for(i=2; i<= iSqrt; i++)
				if(iNum%i==0)
					break;
		}
		while(i<= iSqrt);

		iLine= CheckBottom(pparams->hwnd, pparams->cyClient, pparams->cyChar, iLine);
		hdc= GetDC(pparams->hwnd);

		TextOut(hdc, 0, iLine*pparams->cyChar, szBuffer, 
			wsprintf(szBuffer, TEXT("%d"), iNum));

		ReleaseDC(pparams->hwnd, hdc);
		iLine++;
	}
	_endthread();
}

LRESULT APIENTRY WndProc2(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PARAMS params;

	switch(message)
	{
	case WM_CREATE:
		params.hwnd= hwnd;
		params.cyChar= HIWORD(GetDialogBaseUnits());
		_beginthread(Thread2, 0, &params);
		return 0;
	case WM_SIZE:
		params.cyClient= HIWORD(lParam);
		return 0;
	case WM_DESTROY:
		params.bKill= TRUE;
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//windows3: Display increasing sequence of Fibonacci numbers
void Thread3(PVOID pvoid)
{
	HDC hdc;
	int iNum= 0, iNext= 1, iLine=0, iTemp;
	PPARAMS pparams;
	TCHAR szBuffer[16];

	pparams= (PPARAMS)pvoid;

	while(!pparams->bKill)
	{
		if(iNum<0)
		{
			iNum= 0;
			iNext= 1;
		}
		iLine= CheckBottom(pparams->hwnd, pparams->cyClient, pparams->cyChar, iLine);

		hdc= GetDC(pparams->hwnd);

		TextOut(hdc, 0, iLine*pparams->cyChar, szBuffer, 
			wsprintf(szBuffer, TEXT("%d"), iNum));

		ReleaseDC(pparams->hwnd, hdc);

		iTemp= iNum;
		iNum= iNext;
		iNext+= iTemp;
		iLine++;
	}
	_endthread();
}

LRESULT APIENTRY WndProc3(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}