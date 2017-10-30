#include <windows.h>
#define BUFFER(x,y) *(pBuffer + y*cxBuffer + x)

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
	case WM_INPUTLANGCHANGE://���뷨�����ı�,���������Է����ı�������½���CREATE�ȡ���������һ��KEYVIEW1������
		dwCharSet= wParam;
	case WM_CREATE:
		hdc= GetDC(hwnd);
		SelectObject(hdc, CreateFont(0,0,0,0,0,0,0,0,
			dwCharSet, 0,0,0,FIXED_PITCH, NULL));
		GetTextMetrics(hdc, &tm);
		cxChar= tm.tmAveCharWidth;
		cyChar= tm.tmHeight;
		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		ReleaseDC(hwnd, hdc);
		//fall through
	case WM_SIZE:
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
				BUFFER(x,y)= '';
		xCaret= 0;
		yCaret= 0;

		if(hwnd== GetFocus())
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
			xCaret=cxBuffer-1;	//�������һ
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
			//д������

	case WM_KEYUP:
	case WM_CHAR:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
		//Rearange storage array
		for(i=cLinesMax-1;i>0;i--)
		{
			pmsg[i]= pmsg[i-1];
		}
		//Store new message
		pmsg[0].hwnd= hwnd;
		pmsg[0].message= message;
		pmsg[0].wParam= wParam;
		pmsg[0].lParam= lParam;

		cLines= min(cLines+1, cLinesMax);
		//Scroll up the display
		ScrollWindow(hwnd, 0, -cyChar, &rectScroll, &rectScroll);
		break;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		SelectObject(hdc, CreateFont(0,0,0,0,0,0,0,0,
			dwCharSet, 0,0,0,FIXED_PITCH, NULL));	//��������
		SetBkMode(hdc, TRANSPARENT);	//ͨ���������趨Ϊ͸����ʹ��������������ص���ʾ����������������»����ص���ʾ��
		//SetBkMode(hdc, OPAQUE);
		TextOut(hdc, 0, 0, szTop, lstrlen(szTop));
		TextOut(hdc, 0, 0, szUnd, lstrlen(szUnd));

		for(i=0;i<min(cLines, cyClient/cyChar-1);i++)
		{
			iType= pmsg[i].message==WM_CHAR||
				pmsg[i].message==WM_SYSCHAR||
				pmsg[i].message==WM_DEADCHAR||
				pmsg[i].message==WM_SYSDEADCHAR;

			GetKeyNameText(pmsg[i].lParam, szKeyName, sizeof(szKeyName)/sizeof(TCHAR));

			TextOut(hdc, 0, (cyClient/cyChar-1-i)*cyChar, szBuffer, 
				wsprintf(szBuffer, szFormat[iType], szMessage[pmsg[i].message- WM_KEYFIRST],
				pmsg[i].wParam, (PTSTR)(iType?TEXT(" "):szKeyName),
				(TCHAR)(iType?pmsg[i].wParam:' '),
				LOWORD(pmsg[i].lParam),
				HIWORD(pmsg[i].lParam)&0xFF,
				0x01000000&pmsg[i].lParam?szYes:szNo,
				0x20000000&pmsg[i].lParam?szYes:szNo,
				0x40000000&pmsg[i].lParam?szDown:szUp,
				0x80000000&pmsg[i].lParam?szUp:szDown));
		}
		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));	//�������ó�ϵͳ���岢��ͨ������ֵɾ����һ���Զ����Լ���gdi����
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//�ܽ�
//SetBkMode(hdc, TRANSPARENT);ʹ�������ı���Ϊ͸����������������������Ի����ص���
//wsprintf����������Ҫ������д����szBuffer���棬�ַ����˴�С�����ǳ��ο���ʱ��̫������⡣