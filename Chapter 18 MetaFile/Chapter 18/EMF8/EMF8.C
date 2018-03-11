/*
EMF1-7中显示的metafile图像都可以由拉伸显示窗口的大小来任意改变长宽比，但是从MEF8开始其中显示的直尺的大小及长宽高是不随窗口的改变而改变的。
EMF1-7中的WinMain是在EMFx.c中的，之后将会统一放在EMF.C中，而EFMx.c中则是用来放一些特定的不同的代码。
*/
#include <windows.h>
TCHAR szClass[]= TEXT("EMF8");
TCHAR szTitle[]= TEXT("EMF8:Enhanced MetaFile Demo #8");

//WM_CREATE
void DrawRuler(HDC hdc, int cx, int cy)
{
	int iAdj, i, iHeight;
	LOGFONT lf;
	TCHAR ch;

	iAdj= GetVersion()& 0x80000000?0:1;
	//Black pen with 1-point width
	SelectObject(hdc, CreatePen(PS_SOLID, cx/72/6, 0));
	//Rectangle sruuounding entire pen(with adjustment)
	Rectangle(hdc, iAdj, iAdj, cx+ iAdj+ 1, cy+ iAdj+ 1);
	//Tick Marks
	for(i=0;i<96;i++)
	{
		if(i%16==0)
			iHeight= cy/2;	//inches
		else if(i%8== 0)
			iHeight= cy/3;	//half inches
		else if(i%4== 0)
			iHeight= cy/5;	//quarter inches
		else if(i%2== 0)
			iHeight= cy/8;	//eights
		else
			iHeight= cy/12;	//sixteenths
	
		MoveToEx(hdc, i*cx/96, cy, NULL);
		LineTo(hdc, i*cx/96, cy-iHeight);
	}
	//Create logical font
	FillMemory(&lf, sizeof(lf), 0);
	lf.lfHeight= cy/2;
	lstrcpy(lf.lfFaceName, TEXT("Times New Roman"));

	SelectObject(hdc, CreateFontIndirect(&lf));
	SetTextAlign(hdc, TA_BOTTOM| TA_CENTER);
	SetBkMode(hdc, TRANSPARENT);
	//Display numbers
	for(i=i; i<=5; i++)
	{
		ch= (TCHAR)(i+'0');
		TextOut(hdc, i*cx/6, cy/2, &ch, 1);
	}
	//Clean up
	DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
	DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
}

//WM_CREATE
void CreateRoutine(HWND hwnd)
{
	HDC hdcEMF;
	HENHMETAFILE hemf;
	int cxMms, cyMms, cxPix, cyPix, xDpi, yDpi;

	//创建MetaFile文件在磁盘上
	hdcEMF= CreateEnhMetaFile(NULL, TEXT("emf8.emf"), NULL, TEXT("EMF8\0EMF Demo #8\0"));
	if(hdcEMF== NULL)
		return;
	cxMms= GetDeviceCaps(hdcEMF, HORZSIZE);
	cyMms= GetDeviceCaps(hdcEMF, VERTSIZE);
	cxPix= GetDeviceCaps(hdcEMF, HORZRES);
	cyPix= GetDeviceCaps(hdcEMF, VERTRES);

	xDpi= cxPix* 254/ cxMms/ 10;
	yDpi= cyPix* 254/ cyMms/ 10;

	//在MetaFile中画一把直尺
	DrawRuler(hdcEMF, 6*xDpi, yDpi);
	hemf= CloseEnhMetaFile(hdcEMF);	//关闭指定的增强型图元文件设备场景，并将新建的图元文件返回一个句柄

	DeleteEnhMetaFile(hemf);
}

void PaintRoutine(HWND hwnd, HDC hdc, int cxArea, int cyArea)
{
	ENHMETAHEADER emh;
	HENHMETAFILE hemf;
	int cxImage, cyImage;
	RECT rect;

	hemf= GetEnhMetaFile(TEXT("emf8.emf"));
	GetEnhMetaFileHeader(hemf, sizeof(emh), &emh);
	//rclBounds以图素为单位给出图像的大小。由于他之前依据的是显示器大小，所以它是不变的。
	cxImage= emh.rclBounds.right- emh.rclBounds.left;
	cyImage= emh.rclBounds.bottom- emh.rclBounds.top;

	//由于这四个坐标依托与rclBounds是固定的，所以当我们拖动显示窗口，直尺不会随之变大变小。
	rect.left= (cxArea- cxImage)/2;
	rect.right= (cxArea+ cxImage)/2;
	rect.top= (cyArea- cyImage)/2;
	rect.bottom= (cyArea+ cyImage)/2;

	PlayEnhMetaFile(hdc, hemf, &rect);	//将metafile打印出来
	DeleteEnhMetaFile(hemf);
}