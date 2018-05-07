/*
除了惯例要加上的WINMM.lib（支持PlaySound），还要加上comctl32.lib（支持InitCommonControlsEx）
*/
#include <windows.h>
#include <commctrl.h>

//ID values for 3 child windows
#define ID_TIMEPICK 0
#define ID_CHECKBOX 1
#define ID_PUSHBTN 2

//Timer ID
#define ID_TIMER 1

//Number of 100-nanosecond increments 
#define FTTICKSPERHOUR (60*60*(LONGLONG)10000000)

#define SAMPRATE 11025
#define NUMSAMPS (3*SAMPRATE)
#define HALFSAMPS (NUMSAMPS/2)

typedef struct
{
	char chRiff[4];
	DWORD dwRiffSize;
	char chWave[4];
	char chFmt[4];
	DWORD dwFmtSize;
	PCMWAVEFORMAT pwf;
	char chData[4];
	DWORD dwDataSize;
	BYTE byData[0];
}
WAVEFORM;

//The window proc and the subclass proc
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SubProc(HWND, UINT, WPARAM ,LPARAM);

WNDPROC SubbedProc[3];
HWND hwndFocus;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("WakeUp");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= 0;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)(1+COLOR_BTNFACE);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Regtister Wrong!"), szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd= CreateWindow(szAppName, szAppName, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, 
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
	static HWND hwndDTP, hwndCheck, hwndPush;
	static WAVEFORM waveform= {
		"RIFF", NUMSAMPS+0x24, "WAVE", "fmt", sizeof(PCMWAVEFORMAT), 1, 1, SAMPRATE, 
		SAMPRATE, 1, 8, "data", NUMSAMPS
	};
	static WAVEFORM *pwaveform;
	FILETIME ft;
	HINSTANCE hInstance;
	INITCOMMONCONTROLSEX icex;
	int i, cxChar, cyChar;
	LARGE_INTEGER li;
	SYSTEMTIME st;

	switch(message)
	{
	case WM_CREATE:
		hInstance= (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		icex.dwSize= sizeof(icex);
		icex.dwICC= ICC_DATE_CLASSES;
		//如果一个运行在 Windows XP 上的应用程序清单指定要使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，则需要 InitCommonControlsEx()。否则，将无法创建窗口。
		InitCommonControlsEx(&icex);

		pwaveform= malloc(sizeof(WAVEFORM)+ NUMSAMPS);
		*pwaveform= waveform;

		for(i=0; i<HALFSAMPS; i++)
		{
			if(i%600<300)
				if(i%16<8)
					pwaveform->byData[i]= 25;
				else
					pwaveform->byData[i]= 230;
			else if(i%8<4)
				pwaveform->byData[i]= 25;
			else
				pwaveform->byData[i]= 230;
		}

		cxChar= LOWORD(GetDialogBaseUnits());
		cyChar= HIWORD(GetDialogBaseUnits());

		SetWindowPos(hwnd, NULL, 0, 0, 42*cxChar, 10*cyChar/3+2*GetSystemMetrics(SM_CYBORDER)
			+ GetSystemMetrics(SM_CYCAPTION), SWP_NOMOVE| SWP_NOZORDER| SWP_NOACTIVATE);

		//create the 3 child windows
		//这里创建窗口使用了DATETIMEPICK_CLASS，这是称为「Date-Time Picker」的通用控件
		hwndDTP= CreateWindow(DATETIMEPICK_CLASS, TEXT(""), WS_BORDER|WS_CHILD|WS_VISIBLE|DTS_TIMEFORMAT, 
			2*cxChar, cyChar, 12*cxChar, 4*cyChar/3, hwnd, (HMENU)ID_TIMEPICK, hInstance, NULL);
		hwndCheck= CreateWindow(TEXT("Button"), TEXT("Set Alarm"), WS_CHILD|WS_VISIBLE| BS_AUTOCHECKBOX, 
			16*cxChar, cyChar, 12*cxChar, 4*cyChar/3, hwnd, (HMENU)ID_CHECKBOX, hInstance, NULL);
		hwndPush= CreateWindow(TEXT("Button"), TEXT("Trun Off"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_DISABLED, 
			28*cxChar, cyChar, 12*cxChar, 4*cyChar/3, hwnd, (HMENU)ID_PUSHBTN, hInstance, NULL);

		hwndFocus= hwndDTP;
		//subclass the 3 child windows
		SubbedProc[ID_TIMEPICK]= (WNDPROC)SetWindowLong(hwndDTP, GWL_WNDPROC, (LONG)SubProc);
		SubbedProc[ID_CHECKBOX]= (WNDPROC)SetWindowLong(hwndCheck, GWL_WNDPROC, (LONG)SubProc);
		SubbedProc[ID_PUSHBTN]= (WNDPROC)SetWindowLong(hwndPush, GWL_WNDPROC, (LONG)SubProc);

		GetLocalTime(&st);
		SystemTimeToFileTime(&st, &ft);
		li= *(LARGE_INTEGER*)&ft;
		li.QuadPart+= 9*FTTICKSPERHOUR;
		ft= *(FILETIME*)&li;
		FileTimeToSystemTime(&ft, &st);
		st.wMinute= st.wSecond= st.wMilliseconds= 0;
		SendMessage(hwndDTP, DTM_SETSYSTEMTIME, 0, (LPARAM)&st);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hwndFocus);
		return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_CHECKBOX:
			if(SendMessage(hwndCheck, BM_GETCHECK, 0, 0))
			{
				SendMessage(hwndDTP, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				SystemTimeToFileTime(&st, &ft);
				li= *(LARGE_INTEGER*)&ft;
				GetLocalTime(&st);
				SystemTimeToFileTime(&st, &ft);
				li.QuadPart-= ((LARGE_INTEGER*)&ft)->QuadPart;

				while(li.QuadPart<0)
					li.QuadPart+= 24*FTTICKSPERHOUR;
				li.QuadPart%= 24*FTTICKSPERHOUR;

				SetTimer(hwnd, ID_TIMER, (int)(li.QuadPart/10000), 0);
			}
			else
				KillTimer(hwnd, ID_TIMER);
			return 0;
		case ID_PUSHBTN:
			PlaySound(NULL, NULL, 0);
			SendMessage(hwndCheck, BM_SETCHECK, 0, 0);
			EnableWindow(hwndDTP, TRUE);
			EnableWindow(hwndCheck, TRUE);
			EnableWindow(hwndPush, FALSE);
			SetFocus(hwndDTP);
			return 0;
		}
		return 0;
	case WM_NOTIFY:
		switch(wParam)
		{
		case ID_TIMEPICK:
			switch(((NMHDR*)lParam)->code)
			{
			case DTN_DATETIMECHANGE:
				if(SendMessage(hwndCheck, BM_GETCHECK, 0, 0))
				{
					KillTimer(hwnd, ID_TIMER);
					SendMessage(hwndCheck, BM_SETCHECK, 0 ,0);
				}
				return 0;
			}
		}
		return 0;
	case WM_TIMER:
		KillTimer(hwnd, ID_TIMER);
		PlaySound((PTSTR)pwaveform, NULL, SND_MEMORY|SND_LOOP|SND_ASYNC);

		EnableWindow (hwndDTP, FALSE) ;
		EnableWindow (hwndCheck, FALSE) ;
		EnableWindow (hwndPush, TRUE) ;

		hwndFocus= hwndPush;
		ShowWindow(hwnd, SW_RESTORE);
		SetForegroundWindow(hwnd);
		return 0;

	case WM_DESTROY:
		free(pwaveform);

		if(IsWindowEnabled(hwndPush))
			PlaySound(NULL,NULL, 0);
		if(SendMessage(hwndCheck, BM_GETCHECK, 0,0))
			KillTimer(hwnd, ID_TIMER);

		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK SubProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int idNext, id= GetWindowLong(hwnd, GWL_ID);
	switch(message)
	{
	case WM_CHAR:
		if(wParam== '\t')
		{
			idNext= id;
			do
			idNext= (idNext+ (GetKeyState(VK_SHIFT)<0?2:1))%3;
			while(!IsWindowEnabled(GetDlgItem(GetParent(hwnd), idNext)));
			SetFocus(GetDlgItem(GetParent(hwnd), idNext));
			return 0;
		}
		break;
	case WM_SETFOCUS:
		hwndFocus= hwnd;
		break;
	}
	return CallWindowProc(SubbedProc[id], hwnd, message, wParam, lParam);
}