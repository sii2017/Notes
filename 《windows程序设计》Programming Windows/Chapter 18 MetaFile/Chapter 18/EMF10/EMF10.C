/*
这个版本的直尺会随着窗口大小而缩放，但是是固定的纵横比
*/
#include <windows.h>
TCHAR szClass[]= TEXT("EMF10");
TCHAR szTitle[]= TEXT("EMF10:Enhanced MetaFile Demo# 10");

void CreateRoutine(HWND hwnd)
{
}

void PaintRoutine(HWND hwnd, HDC hdc, int cxArea, int cyArea)
{
	ENHMETAHEADER emh;
	float fScale;
	HENHMETAFILE hemf;
	int cxMms, cyMms, cxPix, cyPix, cxImage, cyImage;
	RECT rect;

	cxMms= GetDeviceCaps(hdc, HORZSIZE);
	cyMms= GetDeviceCaps(hdc, VERTSIZE);
	cxPix= GetDeviceCaps(hdc, HORZRES);
	cyPix= GetDeviceCaps(hdc, VERTRES);

	hemf= GetEnhMetaFile(TEXT ("..\\emf8\\emf8.emf"));

	GetEnhMetaFileHeader(hemf, sizeof(emh), &emh);
	cxImage= emh.rclFrame.right- emh.rclFrame.left;
	cyImage= emh.rclFrame.bottom- emh.rclFrame.top;

	cxImage= cxImage* cxPix/cxMms/100;
	cyImage= cyImage*cyPix/cyMms/100;

	fScale= min((float)cxArea/ cxImage, (float)cyArea/cyImage);

	cxImage= (int)(fScale* cxImage);
	cyImage= (int)(fScale* cyImage);

	rect.left= (cxArea - cxImage) / 2 ;
	rect.right = (cxArea + cxImage) / 2 ;
	rect.top = (cyArea - cyImage) / 2 ;
	rect.bottom = (cyArea + cyImage) / 2 ;
	PlayEnhMetaFile (hdc, hemf, &rect) ;
	DeleteEnhMetaFile (hemf) ;
}