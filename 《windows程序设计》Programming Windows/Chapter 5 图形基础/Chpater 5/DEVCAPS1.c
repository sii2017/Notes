#include <windows.h>
#define NUMLINES ((int)(sizeof devcaps/sizeof devcaps[0]))
struct
{
	int iIndex;
	TCHAR* szLabel;
	TCHAR* szDesc;
}
devcaps[]=
{
	HORZSIZE, TEXT("HORZSIZE"), TEXT("Width in millimeters:"),
	VERTSIZE, TEXT("VERTSIZE"), TEXT("Height in millimters:"),
	HORZRES, TEXT("HORZRES"), TEXT("Width in pixels:"),
	VERTRES, TEXT("VERTRES"), TEXT("Height in raster lines:"),
	BITSPIXEL, TEXT("BITSPIXEL"), TEXT("Color bits per pixel:"),
	PLANES, TEXT("PLANES"), TEXT("Number of color planes:"),
	NUMBRUSHES, TEXT("NUMBRUSHES"), TEXT("Number of device brushes:"),
	NUMPENS, TEXT("NUMPENS"), TEXT("Number of device pens:"),
	NUMMARKERS, TEXT("NUMMARKERS"), TEXT("Number of device markers:"),
	NUMFONTS, TEXT("NUMFONTS"), TEXT("Number if device fonts:"),
	NUMCOLORS, TEXT("NUMCOLORS"), TEXT("Number of device colors:"),
	PDEVICESIZE, TEXT("PDEVICESIZE"), TEXT("Size of device structure:"),
	ASPECTX, TEXT("ASPECTX"), TEXT("Relative width of pixel:"),
	ASPECTY, TEXT("ASPECTY"), TEXT("Relative height of pixel:"),
	ASPECTXY, TEXT("ASPECTXY"), TEXT("Relative diagonal of pixel:"),
	LOGPIXELSX, TEXT("LOGPIXELSX"), TEXT("Horizontal dots per inch:"),
	LOGPIXELSY, TEXT("LOGPIXELSY"), TEXT("Vertical dots per inch:"),
	SIZEPALETTE, TEXT("SIZEPALETTE"), TEXT("Number of palette entries:"),
	NUMRESERVED, TEXT("NUMRESERVED"), TEXT("Reserved palete entries:"),
	COLORRES, TEXT("COLORRES"), TEXT("Actual color resolution:")
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("DevCpas1");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	
	wndclass.style= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG!"), szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd= CreateWindow(szAppName,TEXT("Device Capabilities"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
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
	static int cxChar, cxCaps, cyChar;
	TCHAR szBuffer[10];
	HDC hdc;
	int i;
	PAINTSTRUCT ps;
	TEXTMETRIC tm;
	
	switch(message)
	{
	case WM_CREATE:
		hdc= GetDC(hwnd);	//��õ�ǰhdc
		GetTextMetrics(hdc, &tm);	//��hdc��Ϊ������ȡ��hdc�����������Ϣ������tm��
		cxChar= tm.tmAveCharWidth;
		cxCaps= (tm.tmPitchAndFamily & 1?3:2) * cxChar/2;
		cyChar= tm.tmHeight + tm.tmExternalLeading;
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		for(i=0; i<NUMLINES;++i)
		{
			TextOut(hdc, 0, cyChar*i, devcaps[i].szLabel, lstrlen(devcaps[i].szLabel));
			TextOut(hdc, 14*cxCaps, cyChar*i,devcaps[i].szDesc, lstrlen(devcaps[i].szDesc));
			SetTextAlign(hdc, TA_RIGHT | TA_TOP);		//����������Ϊ ��ʼ��Ϊ�ұ߿�ʼ���
			TextOut(hdc, 14*cxCaps + 35*cxChar, cyChar*i, szBuffer, wsprintf(szBuffer, TEXT("%5d"),
				GetDeviceCaps(hdc, devcaps[i].iIndex)));
			SetTextAlign(hdc, TA_LEFT | TA_TOP);	//���������������һ��ѭ����ʼ�����һ��֮ǰ �ٸĻش���࿪ʼ���
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}