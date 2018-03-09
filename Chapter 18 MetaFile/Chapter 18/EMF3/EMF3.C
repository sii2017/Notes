/*
本程序阐明了，metafile只会记录“会使用到设备内容句柄的GDI函数”并不会记录“不需要设备内容句柄的”函数
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	//正常创建，没有menu
	static TCHAR szAppName[]= TEXT("EMF3");
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

	hwnd = CreateWindow (szAppName, TEXT ("Enhanced MetaFile Demo #3"),
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
	LOGBRUSH lb;
	HDC hdc, hdcEMF;
	HENHMETAFILE hemf;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		//将metafile创建在磁盘中,由于metafile属于图元文件，所以返回的是HDC类型的句柄
		hdcEMF= CreateEnhMetaFile(NULL, TEXT("emf3.emf"), NULL, 
			TEXT("EMF3\0EMF Demo #3\0"));

		//CreateSolidBrush函数调用并不会被记录到metafile中但是selectobject函数会被记录进去
		SelectObject(hdcEMF, CreateSolidBrush(RGB(0,0,255)));
		lb.lbStyle= BS_SOLID;
		lb.lbColor= RGB(255,0,0);
		lb.lbHatch= 0;

		//ExtCreatePen函数调用不会被记录到metafile中，但是selectobject函数会被记录进去
		SelectObject(hdcEMF, ExtCreatePen(PS_SOLID|PS_GEOMETRIC, 5, &lb, 0, NULL));
		
		if(GetVersion()& 0x80000000)	//windows 98
			Rectangle(hdcEMF, 100, 100, 201, 201);
		else	//Windows NT
			Rectangle(hdcEMF, 101, 101, 202, 202);

		MoveToEx(hdcEMF, 100, 100, NULL);
		LineTo(hdcEMF, 200, 200);
		MoveToEx(hdcEMF, 200, 100, NULL);
		LineTo(hdcEMF, 100, 200);

		DeleteObject(SelectObject(hdcEMF, GetStockObject(BLACK_PEN)));
		DeleteObject(SelectObject(hdcEMF, GetStockObject(WHITE_BRUSH)));

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
		
		hemf= GetEnhMetaFile(TEXT("emf3.emf"));

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