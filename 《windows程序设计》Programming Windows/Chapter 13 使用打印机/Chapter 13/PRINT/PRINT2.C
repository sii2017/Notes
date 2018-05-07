/*
PRINT2程序在PRINT1中增加了一个放弃程序和必要的支持－呼叫AbortProc函数
并呼叫EnableWindow两次（第一次阻止窗口接受输入消息，第二次启用窗口）。
*/
//#include <windows.h>
//HDC GetPrinterDC(void);	//in GETPRNDC.C
//void PageGDICalls(HDC, int, int);	//in PRINT.C
//
//HINSTANCE hInst;
//TCHAR szAppName[]= TEXT("Print2");
//TCHAR szCaption[]= TEXT("Print Program 2(Abort Pricedure)");
//
//BOOL CALLBACK AbortProc(HDC hdcPrn, int iCode)
//{
//	MSG msg;
//	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//	{
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//	return TRUE;
//}
//
//BOOL PrintMyPage(HWND hwnd)
//{
//	static DOCINFO di= 
//	{
//		sizeof(DOCINFO), TEXT("Print2: Printing")
//	};
//	BOOL bSuccess= TRUE;
//	HDC hdcPrn;
//	int xPage, yPage;
//
//	if(NULL== (hdcPrn= GetPrinterDC()))	//获取打印机的hdc设备内容
//		return FALSE;
//	xPage= GetDeviceCaps(hdcPrn, HORZRES);	//通过hdc获取长和宽，这不是纸面的全部大小，只是可打印区域
//	yPage= GetDeviceCaps(hdcPrn, VERTRES);
//
//	EnableWindow(hwnd, FALSE);
//	SetAbortProc(hdcPrn, AbortProc);
//
//	if(StartDoc(hdcPrn, &di)> 0)	//第一步验证
//	{	
//		if(StartPage(hdcPrn)>0)		//第二步验证
//		{
//			PageGDICalls(hdcPrn, xPage, yPage);
//			//当GDI得到EndPage后，对打印页中由设备驱动程序定义的每个输出带,GDI都将该metafile送入设备驱动程序中。
//			//然后，GDI将打印机驱动程序建立的打印输出储存到一个文件中。
//			//在EndPage呼叫期间，GDI模块调用之前设定的放弃程序（取消程序）
//			if(EndPage(hdcPrn)>0)	
//				EndDoc(hdcPrn);		//结束打印？这里已经打印好了吗？
//			else
//				bSuccess= FALSE;
//		}
//	}
//	else
//		bSuccess= FALSE;
//
//	EnableWindow(hwnd, TRUE);
//	DeleteDC(hdcPrn);
//	return bSuccess;
//}