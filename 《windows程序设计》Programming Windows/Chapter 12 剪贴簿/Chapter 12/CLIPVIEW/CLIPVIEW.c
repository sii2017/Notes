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
		hwndNextViewer= SetClipboardViewer(hwnd);//设置为当前剪贴簿浏览器，并且记录前一个剪贴簿浏览器的句柄
		return 0;
	case WM_CHANGECBCHAIN://有某一个剪贴簿浏览器删除了自己，windows将该消息发给当前浏览器，并由当前剪贴簿浏览器逐个转发
		if((HWND)wParam== hwndNextViewer)//如果退出的剪贴簿浏览器是前一个剪贴簿浏览器，则把之前保存的“前一个剪贴簿浏览器”的句柄改成“前前个剪贴浏览器”
			hwndNextViewer= (HWND)lParam;
		else if(hwndNextViewer)
			SendMessage(hwndNextViewer, message, wParam, lParam);
		return 0;
	case WM_DRAWCLIPBOARD://当剪贴板数据发送变化时，窗口会依次收到这个消息。处理该消息挨个传下去就可以了，如果前一个剪贴板浏览器没有就不用传
		if(hwndNextViewer)
			SendMessage(hwndNextViewer, message, wParam, lParam);
		InvalidateRect(hwnd, NULL, TRUE);//当剪贴板有新内容就刷新页面显示它
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
			pGlobal= (PTSTR)GlobalLock(hGlobal);//锁住内存块来获取内容
			DrawText(hdc, pGlobal, -1, &rect, DT_EXPANDTABS);
			GlobalUnlock(hGlobal);//使用后解锁内存块
		}
		CloseClipboard();
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		ChangeClipboardChain(hwnd, hwndNextViewer);//退出剪贴簿浏览器链，让系统将自己的句柄与前一个保存的句柄以WM_CHANGECBCHAIN形式发出。
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}