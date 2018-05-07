/*
增加了生成对话框以及处理对话框消息的程序。增加了一个bool值来判断是否取消了打印。
*/
#include <windows.h>
HDC GetPrinterDC(void);	//in GETPRNDC.C
void PageGDICalls(HDC, int, int);	//in PRINT.C

HINSTANCE hInst;
TCHAR szAppName[]= TEXT("Print3");
TCHAR szCaption[]= TEXT("Print Program 3(Dialog Box)");

BOOL bUserAbort;
HWND hDlgPrint;

//处理打印对话框的消息处理程序
BOOL CALLBACK PrintDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
		SetWindowText(hDlg, szAppName);
		EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);	//让退出键无效
		return TRUE;
	case WM_COMMAND:
		bUserAbort= TRUE;
		EnableWindow(GetParent(hDlg), TRUE);
		DestroyWindow(hDlg);
		hDlgPrint= NULL;
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK AbortProc(HDC hdcPrn, int iCode)
{
	MSG msg;
	while(!bUserAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(!hDlgPrint|| !IsDialogMessage(hDlgPrint, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return !bUserAbort;
}

BOOL PrintMyPage(HWND hwnd)
{
	static DOCINFO di= 
	{
		sizeof(DOCINFO), TEXT("Print2: Printing")
	};
	BOOL bSuccess= TRUE;
	HDC hdcPrn;
	int xPage, yPage;

	if(NULL== (hdcPrn= GetPrinterDC()))	//获取打印机的hdc设备内容
		return FALSE;
	xPage= GetDeviceCaps(hdcPrn, HORZRES);	//通过hdc获取长和宽，这不是纸面的全部大小，只是可打印区域
	yPage= GetDeviceCaps(hdcPrn, VERTRES);

	EnableWindow(hwnd, FALSE);
	bUserAbort= FALSE;
	hDlgPrint= CreateDialog(hInst, TEXT("PrintDlgBox"), hwnd, PrintDlgProc);
	SetAbortProc(hdcPrn, AbortProc);

	if(StartDoc(hdcPrn, &di)> 0)	//第一步验证
	{	
		if(StartPage(hdcPrn)>0)		//第二步验证
		{
			PageGDICalls(hdcPrn, xPage, yPage);
			//当GDI得到EndPage后，对打印页中由设备驱动程序定义的每个输出带,GDI都将该metafile送入设备驱动程序中。
			//然后，GDI将打印机驱动程序建立的打印输出储存到一个文件中。
			//在EndPage呼叫期间，GDI模块调用之前设定的放弃程序（取消程序）
			if(EndPage(hdcPrn)>0)	
				EndDoc(hdcPrn);		//结束打印？这里已经打印好了吗？
			else
				bSuccess= FALSE;
		}
	}
	else
		bSuccess= FALSE;

	if(!bUserAbort)
	{
		EnableWindow(hwnd, TRUE);
		DeleteDC(hdcPrn);
	}
	DeleteDC(hdcPrn);
	return bSuccess&&!bUserAbort;
}