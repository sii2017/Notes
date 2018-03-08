/*
这次metafile不是存在内存中而是直接存在磁盘里
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	//正常创建，没有menu
	static TCHAR szAppName[]= TEXT("EMF2");
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
		MessageBox(NULL, TEXT("Regiseter Failed"),szAppName, MB_ICONERROR) ;
		return 0;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Enhanced MetaFile Demo #2"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL) ;

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam,LPARAM lParam)
{
	HDC hdc, hdcEMF;
	HENHMETAFILE hemf;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		hdcEMF= CreateEnhMetaFile(NULL, TEXT("emf2.emf"), NULL, 
			TEXT("EMF2\OEMF Demo #2\0"));

		if(!hdcEMF)
			return 0;

		Rectangle(hdcEMF, 100, 100, 200, 200);
		MoveToEx(hdcEMF, 100, 100, NULL);
		LineTo(hdcEMF, 200, 200);

		MoveToEx(hdcEMF, 200, 100, NULL);
		LineTo(hdcEMF, 100, 200);

		hemf= CloseEnhMetaFile(hdcEMF);

		DeleteEnhMetaFile(hemf);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);

		rect.left= rect.right/4;
		rect.right= 3*rect.right/4;
		rect.top= rect.bottom/4;
		rect.bottom= 3*rect.bottom/4;

		//由于不在内存中没有句柄可以使用，则通过函数存取这个文件
		if(hemf= GetEnhMetaFile(TEXT("emf2.emf")))
		{
			PlayEnhMetaFile(hdc, hemf, &rect);
			DeleteEnhMetaFile(hemf);
			//对MetaFile的删除操作仅是释放了用以储存MetaFile的内存资源而已，磁盘MetaFile甚至在程序执行结束后还保留在磁盘上。
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}