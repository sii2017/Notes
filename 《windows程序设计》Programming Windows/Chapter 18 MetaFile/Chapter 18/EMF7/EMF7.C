/*
EMF7使用EMF3程序建立的EMF3.EMF，所以在执行EMF7之前要执行EMF3程序建立MetaFile。
将emf3的内容读取后，以列举的方式进行一定的修改并且存入emf7中，最后显示出来
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	//正常创建，没有menu
	static TCHAR szAppName[]= TEXT("EMF7");
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

	hwnd = CreateWindow (szAppName, TEXT ("Enhanced MetaFile Demo #7"),
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

int CALLBACK EnhMetaFileProc(HDC hdc, HANDLETABLE* pHandleTable, CONST ENHMETARECORD* pEmfRecord,
							 int iHandles, LPARAM pData)
{
	HBRUSH hBrush;
	HPEN hPen;
	LOGBRUSH lb;
	//如果不是头或者尾，那么就按记录画进hdc（实际上是emf7里）
	if(pEmfRecord->iType!= EMR_HEADER&& pEmfRecord->iType!= EMR_EOF)
		PlayEnhMetaFileRecord(hdc, pHandleTable, pEmfRecord, iHandles);
	//如果是方块则改变一些颜色及线条大小
	if(pEmfRecord->iType== EMR_RECTANGLE)
	{
		hBrush= SelectObject(hdc, GetStockObject(NULL_BRUSH));
		lb.lbStyle= BS_SOLID;
		lb.lbColor= RGB(0,255,0);
		lb.lbHatch= 0;

		hPen= SelectObject(hdc, ExtCreatePen(PS_SOLID| PS_GEOMETRIC, 5, &lb, 0, NULL));
		Ellipse(hdc, 100, 100, 200, 200);
		DeleteObject(SelectObject(hdc, hPen));
		SelectObject(hdc, hBrush);
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ENHMETAHEADER emh;
	HDC hdc, hdcEMF;
	HENHMETAFILE hemfOld, hemf;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		//获取emf3的metafile句柄以及获取其表头
		hemfOld= GetEnhMetaFile(TEXT("..\\emf3\\emf3.emf"));
		GetEnhMetaFileHeader(hemfOld, sizeof(ENHMETAHEADER), &emh);
		//创建一个新的emf7
		hdcEMF= CreateEnhMetaFile(NULL, TEXT("emf7.emf"), NULL, 
			TEXT("EMF7\0EMF Demo #7\0"));
		//使用列举函数将emf3的内容画（拷贝）进emf7里
		EnumEnhMetaFile(hdcEMF, hemfOld, EnhMetaFileProc, NULL, (RECT*)& emh.rclBounds);
		//关闭打开的emf7的metafile句柄并且获得GDI句柄
		hemf= CloseEnhMetaFile(hdcEMF);
		//删除不需要的metafile句柄
		DeleteEnhMetaFile(hemfOld);
		DeleteEnhMetaFile(hemf);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		rect.left= rect.right/4;
		rect.right= 3*rect.right/4;
		rect.top= rect.bottom/4;
		rect.bottom= 3*rect.bottom/4;

		//获取emf7的句柄并且显示出来
		hemf= GetEnhMetaFile(TEXT("emf7.emf"));
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