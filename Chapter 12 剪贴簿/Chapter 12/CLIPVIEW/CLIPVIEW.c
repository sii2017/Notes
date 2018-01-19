#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("ClipView");
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
		MessageBox(NULL, TEXT("Regist Wrong"), szAppName, MB_ICONERROR);
		return 0;
	}
	
	hwnd= CreateWindow(szAppName, TEXT("Simple Clipboard Viewer"), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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
	static HWND hwndNextViewer;
	HGLOBAL hGlobal;
	HDC hdc;
	PTSTR pGlobal;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		hwndNextViewer= SetClipboardViewer(hwnd);//����Ϊ��ǰ����������������Ҽ�¼ǰһ��������������ľ��
		return 0;
	case WM_CHANGECBCHAIN://��ĳһ�������������ɾ�����Լ���windows������Ϣ������ǰ����������ɵ�ǰ��������������ת��
		if((HWND)wParam== hwndNextViewer)//����˳��ļ������������ǰһ������������������֮ǰ����ġ�ǰһ����������������ľ���ĳɡ�ǰǰ�������������
			hwndNextViewer= (HWND)lParam;
		else if(hwndNextViewer)
			SendMessage(hwndNextViewer, message, wParam, lParam);
		return 0;
	case WM_DRAWCLIPBOARD://�����������ݷ��ͱ仯ʱ�����ڻ������յ������Ϣ���������Ϣ��������ȥ�Ϳ����ˣ����ǰһ�������������û�оͲ��ô�
		if(hwndNextViewer)
			SendMessage(hwndNextViewer, message, wParam, lParam);
		InvalidateRect(hwnd, NULL, TRUE);//���������������ݾ�ˢ��ҳ����ʾ��
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		OpenClipboard(hwnd);
#ifdef UNICODE
		hGlobal= GetClipboardData(CF_UNICODETEXT);
#else
		hGlobal= GetClipboardData(CF_TEXT);
#endif
		if(hGlobal!=NULL)
		{
			pGlobal= (PTSTR)GlobalLock(hGlobal);//��ס�ڴ������ȡ����
			DrawText(hdc, pGlobal, -1, &rect, DT_EXPANDTABS);
			GlobalUnlock(hGlobal);//ʹ�ú�����ڴ��
		}
		CloseClipboard();
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		ChangeClipboardChain(hwnd, hwndNextViewer);//�˳������������������ϵͳ���Լ��ľ����ǰһ������ľ����WM_CHANGECBCHAIN��ʽ������
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}