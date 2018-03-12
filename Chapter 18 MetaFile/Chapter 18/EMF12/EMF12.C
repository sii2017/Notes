/*
使用了MM_HIMETRIC并更改了原点。
*/
#include <windows.h>
TCHAR szClass[]= TEXT("EMF12");
TCHAR szTitle[]= TEXT("EMF12:Enhanced MetaFile Demo #12");

//WM_CREATE
void DrawRuler(HDC hdc, int cx, int cy)
{
	int iAdj, i, iHeight;
	LOGFONT lf;
	TCHAR ch;

	iAdj= GetVersion()& 0x80000000?0:1;
	//BlackPen with 1-point wdith
	SelectObject(hdc, CreatePen(PS_SOLID, cx/72/6, 0));
	//Rectangle surrounding entrie pen (with adjustment)
	Rectangle(hdc, iAdj, iAdj, cx+ iAdj+ 1, cy+ iAdj+ 1);
	//Tick Marks
	for(i=0; i<96; i++)
	{
		if(i % 16 == 0) iHeight = cy / 2 ; // inches
		else if (i % 8 == 0) iHeight = cy / 3 ; // half inches
		else if (i % 4 == 0) iHeight = cy / 5 ; // quarter inches
		else if (i % 2 == 0) iHeight = cy / 8 ; // eighths
		else iHeight = cy / 12 ; // sixteenths


		MoveToEx(hdc, i*cx/96, cy, NULL);
		LineTo(hdc, i*cx/96, cy- iHeight);
	}
	//Create logical font
	FillMemory(&lf, sizeof(lf), 0);
	lf.lfHeight= cy/2;
	lstrcpy(lf.lfFaceName, TEXT("Times New Roman"));

	SelectObject(hdc, CreateFontIndirect(&lf));
	SetTextAlign(hdc, TA_BOTTOM| TA_CENTER);
	SetBkMode(hdc, TRANSPARENT);
	//Display numbers
	for(i=1; i<=5; i++)
	{
		ch= (TCHAR)(i+ '0');
		TextOut(hdc, i*cx/6, cy/2, &ch, 1);
	}
	//clean up
	DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
	DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
}

//WM_CREATE
void CreateRoutine(HWND hwnd)
{
	HDC hdcEMF;
	HENHMETAFILE hemf;
	int cxMms, cyMms, cyPix, cxPix, xDpi, yDpi;
	hdcEMF= CreateEnhMetaFile(NULL, TEXT("emf12.emf"), NULL, TEXT("EMF12\0EMF Demo #12\0"));

	cxMms= GetDeviceCaps(hdcEMF, HORZSIZE);
	cyMms= GetDeviceCaps(hdcEMF, VERTSIZE);
	cxPix= GetDeviceCaps(hdcEMF, HORZRES);
	cyPix= GetDeviceCaps(hdcEMF, VERTRES);

	xDpi= cxPix* 254/ cxMms/ 10;
	yDpi= cyPix* 254/ cyMms/ 10;

	DrawRuler(hdcEMF, 6*xDpi, yDpi);
	hemf= CloseEnhMetaFile(hdcEMF);
	DeleteEnhMetaFile(hemf);
}

void PaintRoutine(HWND hwnd, HDC hdc, int cxArea, int cyArea)
{
	ENHMETAHEADER emh;
	HENHMETAFILE hemf;
	POINT pt;
	int cxImage, cyImage;
	RECT rect;

	//new
	//EMF12中的PaintRoutine函数首先将映像方式设定为MM_HIMETRIC。像其它度量映像方式一样，y值沿着屏幕向上增长。然而，原点坐标仍在屏幕的左上角，这就意味显示区域内的y坐标值是负数。
	//为了纠正这个问题，程序呼叫SetViewportOrgEx将原点坐标设定在左下角。
	SetMapMode(hdc, MM_HIMETRIC);
	SetViewportOrgEx(hdc, 0, cyArea, NULL);
	pt.x= cxArea;
	pt.y= 0;

	DPtoLP(hdc, &pt, 1);
	hemf= GetEnhMetaFile(TEXT("emf12.emf"));
	GetEnhMetaFileHeader(hemf, sizeof(emh), &emh);
	cxImage= emh.rclFrame.right- emh.rclFrame.left;
	cyImage= emh.rclFrame.bottom- emh.rclFrame.top;

	rect.left= (pt.x- cxImage)/2;
	rect.top= (pt.y+ cyImage)/2;
	rect.right= (pt.x+ cxImage)/2;
	rect.bottom= (pt.y- cyImage)/2;

	PlayEnhMetaFile(hdc, hemf, &rect);
	DeleteEnhMetaFile(hemf);
}