/*
EMF5中有筛选的将metafile中的记录显示了出来，EMF6中更改了metafile中的记录。
但是在emf6中并不是更改原始记录，而是申请内存做了一个副本，先复制到副本，然后对副本进行操作。
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	//正常创建，没有menu
	static TCHAR szAppName[]= TEXT("EMF6");
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

	hwnd = CreateWindow (szAppName, TEXT ("Enhanced MetaFile Demo #6"),
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
	ENHMETARECORD* pEmfr;
	pEmfr= (ENHMETARECORD*)malloc(pEmfRecord->nSize);	//为复制提前申请一块内存
	CopyMemory(pEmfr, pEmfRecord, pEmfRecord->nSize);	//将原来的record复制到副本record里去
	//如果记录中是矩形，就改成圆形。。但是并没有更改数值
	if(pEmfr->iType== EMR_RECTANGLE)
		pEmfr->iType= EMR_ELLIPSE;
	PlayEnhMetaFileRecord(hdc, pHandleTable, pEmfr, iHandles);
	free(pEmfr);
	return TRUE;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	HENHMETAFILE hemf;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		rect.left= rect.right/4;
		rect.right= 3*rect.right/4;
		rect.top= rect.bottom/4;
		rect.bottom= 3*rect.bottom/4;

		hemf= GetEnhMetaFile(TEXT("..\\emf3\\emf3.emf"));
		EnumEnhMetaFile(hdc, hemf, EnhMetaFileProc, NULL, &rect);
		DeleteEnhMetaFile(hemf);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}