/*
整个屏幕出现方块形的马赛克
*/
#include <windows.h>
#define NUM 300
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//窗口创建不一样
	static int iKeep[NUM][4];
	HDC hdcScr, hdcMem;
	int cx, cy;
	HBITMAP hBitmap;
	HWND hwnd;
	int i,j,x1,y1,x2,y2;

	if(LockWindowUpdate(hwnd= GetDesktopWindow()))	//防止其它程序更新整个屏幕
	{
		//获得整个屏幕的设备内容
		hdcScr= GetDCEx(hwnd, NULL, DCX_CACHE| DCX_LOCKWINDOWUPDATE);
		hdcMem= CreateCompatibleDC(hdcScr);
		cx= GetSystemMetrics(SM_CXSCREEN)/10;
		cy= GetSystemMetrics(SM_CYSCREEN)/10;
		hBitmap= CreateCompatibleBitmap(hdcScr, cx, cy);//获取屏幕图片除以十来建立位图

		SelectObject(hdcMem, hBitmap);//位图选入内存
		srand((int)GetCurrentTime());

		for(i=0; i<2; i++)
			for(j=0; j<NUM; j++)
			{
				if(i==0)
				{
					iKeep[j][0]= x1= cx*(rand()%10);
					iKeep[j][1]= y1= cy*(rand()%10);
					iKeep[j][2]= x2= cx*(rand()%10);
					iKeep[j][3]= y2= cy*(rand()%10);
				}
				else
				{
					x1= iKeep[NUM-1-j][0];
					y1= iKeep[NUM-1-j][1];
					x2= iKeep[NUM-1-j][2];
					y2= iKeep[NUM-1-j][3];
				}
				//第一次将从第一个坐标点开始的矩形复制到内存设备内容。
				//第二次BitBlt将从第二坐标点开始的矩形复制到第一点开始的位置。
				//第三次将内存设备内容中的矩形复制到第二个坐标点开始的区域。
				BitBlt(hdcMem, 0, 0, cx, cy, hdcScr, x1, y1, SRCCOPY);
				BitBlt(hdcScr, x1, y1, cx, cy, hdcScr, x2, y2, SRCCOPY);
				BitBlt(hdcScr, x2, y2, cx, cy, hdcMem, 0, 0, SRCCOPY);
				Sleep(10);
			}

		DeleteDC(hdcMem);
		ReleaseDC(hwnd, hdcScr);
		DeleteObject(hBitmap);

		LockWindowUpdate(NULL);	//解除桌面锁定
	}
	return FALSE;
}