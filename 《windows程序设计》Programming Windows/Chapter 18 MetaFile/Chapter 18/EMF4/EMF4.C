/*
通过内存设备内容将位图画（转移）到了metafile（的设备内容中）并保存在磁盘上
最后显示在了显示画面中
*/
#define OEMRESOURCE
#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	//正常创建，没有menu
	static TCHAR szAppName[]= TEXT("EMF4");
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

	hwnd = CreateWindow (szAppName, TEXT ("Enhanced MetaFile Demo #4"),
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
	BITMAP bm;
	HBITMAP hbm;
	HDC hdc, hdcEMF, hdcMem;
	HENHMETAFILE hemf;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		hdcEMF= CreateEnhMetaFile(NULL, TEXT("emf4.emf"), NULL,
			TEXT("EMF4\0EMF Demo #4\0"));

		hbm= LoadBitmap(NULL, MAKEINTRESOURCE(OBM_CLOSE));//1 先读取系统位图到句柄里
		GetObject(hbm, sizeof(BITMAP), &bm);	//2 从句柄获取信息到bm里
		hdcMem= CreateCompatibleDC(hdcEMF);	//3 创建与hedEMF相容的内存设备内容
		SelectObject(hdcMem, hbm);	//将位图句柄选入hdcMem中
		StretchBlt(hdcEMF, 100, 100, 100, 100, hdcMem, 0,0,
			bm.bmWidth, bm.bmHeight, SRCCOPY);	//将内存设备内容hdcMem中的画面以拉伸的方式画到metaFile的设备内容hdcEMF中
		DeleteObject(hbm);//删除已经使用好的句柄
		hemf= CloseEnhMetaFile(hdcEMF);//关闭已经在磁盘中保存好的metafile并获得句柄
		DeleteEnhMetaFile(hemf);//删除不用使用的句柄
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		rect.left= rect.right/4;
		rect.right= 3*rect.right/4;
		rect.top= rect.bottom/4;
		rect.bottom= 3*rect.bottom/4;

		hemf= GetEnhMetaFile(TEXT("emf4.emf"));
		
		PlayEnhMetaFile(hdc, hemf, &rect);
		DeleteEnhMetaFile(hemf);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}