/*
创建一个增强型metafile，然后画到显示界面上并置于中心
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	//正常创建，没有menu
	static TCHAR szAppName[]= TEXT("EMF1");
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

	hwnd = CreateWindow (szAppName, TEXT ("Enhanced MetaFile Demo #1"),
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
	static HENHMETAFILE hemf;
	HDC hdc, hdcEMF;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		hdcEMF= CreateEnhMetaFile(NULL, NULL, NULL, NULL);//创建一个空的增强型metafile
		//这里的数字并不是真实的坐标，各数值加上相同的数字，他们也不会有区别
		Rectangle(hdcEMF, 100, 100, 200, 200);

		MoveToEx(hdcEMF, 100, 100, NULL);
		LineTo(hdcEMF, 200, 200);

		MoveToEx(hdcEMF, 200, 100, NULL);
		LineTo(hdcEMF, 100, 200);

		hemf= CloseEnhMetaFile(hdcEMF);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);//获取显示区域大小
		rect.left= rect.right/4;	//缩小rect
		rect.right= 3*rect.right/4;
		rect.top= rect.bottom/4;
		rect.bottom= 3*rect.bottom/4;

		PlayEnhMetaFile(hdc, hemf, &rect);	//投射到rect上

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteEnhMetaFile(hemf);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}