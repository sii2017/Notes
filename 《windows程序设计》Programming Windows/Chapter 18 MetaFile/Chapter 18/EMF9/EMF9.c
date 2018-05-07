/*
这个版本与EMF8的区别在于，emf8使用了rclBounds来确定大小，emf9使用了rclFrame来确定直尺大小
*/
#include <windows.h>
#include <string.h>

TCHAR szClass[]=  TEXT("EMF9");
TCHAR szTitle[]= TEXT("EMF9:Enhanced MetaFile Demo #9");

void CreateRoutine(HWND hwnd)
{
	//使用了EMF8的metafile，所以不需要再创建一个新的了。
}

void PaintRoutine(HWND hwnd, HDC hdc, int cxArea, int cyArea)
{
	ENHMETAHEADER emh;
	HENHMETAFILE hemf;
	int cxMms, cyMms, cxPix, cyPix, cxImage, cyImage;
	RECT rect;

	cxMms= GetDeviceCaps(hdc, HORZSIZE);
	cyMms= GetDeviceCaps(hdc, VERTSIZE);
	cxPix= GetDeviceCaps(hdc, HORZRES);
	cyPix= GetDeviceCaps(hdc, VERTRES);

	hemf= GetEnhMetaFile(TEXT("..\\emf8\\emf8.emf"));
	GetEnhMetaFileHeader(hemf, sizeof(emh), &emh);
	cxImage= emh.rclFrame.right- emh.rclFrame.left;
	cyImage= emh.rclFrame.bottom- emh.rclFrame.top;

	cxImage= cxImage*cxPix/ cxMms/ 100;
	cyImage= cyImage*cyPix/ cyMms/ 100;

	rect.left= (cxArea- cxImage)/2;
	rect.right= (cxArea+ cxImage)/2;
	rect.top= (cyArea- cyImage)/2;
	rect.bottom= (cyArea + cyImage)/2;

	PlayEnhMetaFile(hdc, hemf, &rect);
	DeleteEnhMetaFile(hemf);
}