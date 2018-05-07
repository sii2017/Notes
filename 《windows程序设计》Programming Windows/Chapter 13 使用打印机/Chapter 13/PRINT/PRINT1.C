/*
该程序中我们加入了打印的相关内容，获取打印机的设备内容，获取纸张的可打印范围，然后进行打印任务。
由于本电脑没有连接打印机，按下打印后显示的是错误。
*/
//#include <windows.h>
//HDC GetPrinterDC(void);	//in GETPRNDC.C
//void PageGDICalls(HDC, int, int);	//in PRINT.C
//
//HINSTANCE hInst;
//TCHAR szAppName[]= TEXT("Print1");
//TCHAR szCaption[]= TEXT("Print Program 1");
//
//BOOL PrintMyPage(HWND hwnd)
//{
//	static DOCINFO di= 
//	{
//		sizeof(DOCINFO), TEXT("Print1: Printing")
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
//	if(StartDoc(hdcPrn, &di)> 0)	//第一步验证
//	{	
//		if(StartPage(hdcPrn)>0)		//第二步验证
//		{
//			PageGDICalls(hdcPrn, xPage, yPage);
//			if(EndPage(hdcPrn)>0)	//这也是第二步验证，拆开了。原来是在一起&的
//				EndDoc(hdcPrn);		//结束打印？这里已经打印好了吗？
//			else
//				bSuccess= FALSE;
//		}
//	}
//	else
//		bSuccess= FALSE;
//
//	DeleteDC(hdcPrn);
//	return bSuccess;
//}