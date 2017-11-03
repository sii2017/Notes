#include <windows.h>
#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);
int idFocus= 0;
TCHAR szChildClass[]= TEXT("Checker4_Child");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("Check4");
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

	//for child register
	wndclass.lpfnWndProc= ChildWndProc;
	wndclass.cbWndExtra= sizeof(long);
	wndclass.hIcon= NULL;
	wndclass.lpszClassName= szChildClass;

	RegisterClass(&wndclass);

	hwnd= CreateWindow(szAppName, TEXT("Check4 Mouse Hit-Test Demo"),
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
	static HWND hwndChild[DIVISIONS][DIVISIONS];
	int cxBlock,cyBlock,x,y;

	switch(message)
	{
	case WM_CREATE:
		for(x=0;x<DIVISIONS;x++)
			for(y=0;y<DIVISIONS;y++)
				hwndChild[x][y]= CreateWindow(szChildClass, NULL, WS_CHILDWINDOW|WS_VISIBLE, 0,0,0,0, hwnd, (HMENU)(y<<8|x), (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);	//��ͬһ���ط����������Ӵ���
		return 0;
	case WM_SIZE:
		cxBlock= LOWORD(lParam)/DIVISIONS;
		cyBlock= HIWORD(lParam)/DIVISIONS;
		for(x=0; x<DIVISIONS; x++)	//���Ӵ��ڷֱ��ƶ�������λ��
			for(y=0; y<DIVISIONS; y++)
				MoveWindow(hwndChild[x][y], x*cxBlock, y*cyBlock, cxBlock, cyBlock, TRUE);
		return 0;
	case WM_LBUTTONDOWN:
		MessageBeep(0);
		return 0;
	case WM_SETFOCUS:
		SetFocus(GetDlgItem(hwnd, idFocus));	//д������
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	 
	switch(message)
	{
	case WM_CREATE:
		SetWindowLong(hwnd, 0, 0);
		return 0;
	case WM_LBUTTONDOWN:
		SetWindowLong(hwnd, 0, 1^GetWindowLong(hwnd, 0));
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		Rectangle(hdc, 0,0, rect.right, rect.bottom);

		if(GetWindowLong(hwnd, 0))
		{
			MoveToEx(hdc, 0,0, NULL);
			LineTo(hdc, rect.right, rect.bottom);
			MoveToEx(hdc, 0, rect.bottom, NULL);
			LineTo(hdc, rect.right, 0);
		}
		EndPaint(hwnd, &ps);
		return 0;
	}
		return DefWindowProc(hwnd, message, wParam, lParam);
}

//�ܽ᣺
//�������ִ���ע�����ͣ������ڽ����Ժ����Ӵ��ڣ����ƶ��Ӵ��ڵ�λ�á�