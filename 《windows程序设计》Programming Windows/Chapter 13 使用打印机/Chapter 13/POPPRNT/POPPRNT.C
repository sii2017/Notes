/*
该文件主要用来补足之前POPPAD中打印部分缺失的函数
就之前的几个项目来说，这里加入了打印实际需要的关于行数页数的计算。
*/
#include <windows.h>
#include <commdlg.h>
#include "resource.h"

BOOL bUserAbort;
HWND hDlgPrint;

BOOL CALLBACK PrintDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
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

BOOL CALLBACK AbortProc(HDC hPrinterDC, int iCode)
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

BOOL PopPrntPrintFile(HINSTANCE hInst, HWND hwnd, HWND hwndEdit, PTSTR szTitleName)
{
	static DOCINFO di=
	{
		sizeof(DOCINFO)
	};
	static PRINTDLG pd;
	BOOL bSuccess;
	int yChar, iCharsPerLine, iLinesPerPage, iTotalLines, iTotalPages, iPage, iLine, iLineNum;
	PTSTR pstrBuffer;
	TCHAR szJobName[64+ MAX_PATH];
	TEXTMETRIC tm;
	WORD iColCopy, iNoiColCopy;

	pd.lStructSize= sizeof(PRINTDLG);
	pd.hwndOwner= hwnd;
	pd.hDevMode= NULL;
	pd.hDevNames= NULL;
	pd.hDC= NULL;
	pd.Flags= PD_ALLPAGES| PD_COLLATE| PD_RETURNDC| PD_NOSELECTION;
	pd.nFromPage= 0;
	pd.nToPage= 0;
	pd.nMinPage= 0;
	pd.nMaxPage= 0;
	pd.nCopies= 1;
	pd.hInstance= NULL;
	pd.lCustData= 0L;
	pd.lpfnPrintHook= NULL;
	pd.lpfnSetupHook= NULL;
	pd.lpPrintTemplateName= NULL;
	pd.lpSetupTemplateName= NULL;
	pd.hPrintTemplate= NULL;
	pd.hSetupTemplate= NULL;

	if(!PrintDlg(&pd))	//显示打印对话框
		return TRUE;
	if(0== (iTotalLines= SendMessage(hwndEdit, EM_GETLINECOUNT, 0, 0)))	//获取总行数
		return TRUE;

	GetTextMetrics(pd.hDC, &tm);
	yChar= tm.tmHeight+ tm.tmExternalLeading;

	iCharsPerLine= GetDeviceCaps(pd.hDC, HORZRES)/ tm.tmAveCharWidth;	//每行的字数
	iLinesPerPage= GetDeviceCaps(pd.hDC, VERTRES)/ yChar;			//每页的行数
	iTotalPages= (iTotalLines+ iLinesPerPage-1)/ iLinesPerPage;			//总页数

	pstrBuffer= malloc(sizeof(TCHAR)*(iCharsPerLine+1));

	EnableWindow(hwnd, FALSE);	//使窗口停止
	bSuccess= TRUE;
	bUserAbort= FALSE;
	hDlgPrint= CreateDialog(hInst, TEXT("PrintDlgBox"), hwnd, PrintDlgProc);	//打开正在打印对话框
	
	SetDlgItemText(hDlgPrint, DC_FILENAME, szTitleName);
	SetAbortProc(pd.hDC, AbortProc);

	GetWindowText(hwnd, szJobName, sizeof(szJobName));
	di.lpszDocName= szJobName;
	if(StartDoc(pd.hDC, &di)>0)	//开始打印
	{
		//当使用者指定将副本逐份打印时，它将会起作用。
		for(iColCopy=0; iColCopy<((WORD)pd.Flags& PD_COLLATE?pd.nCopies:1); iColCopy++)
		{
			for(iPage= 0; iPage<iTotalPages; iPage++)
			{
				//当不对副本进行逐份打印时，它将起作用
				for(iNoiColCopy=0; iNoiColCopy<(pd.Flags& PD_COLLATE?1:pd.nCopies);iNoiColCopy++)
				{
					if(StartPage(pd.hDC)<0)
					{
						bSuccess= FALSE;
						break;
					}
					//for each page, print the lines
					for(iLine= 0; iLine<iLinesPerPage; iLine++)
					{
						iLineNum= iLinesPerPage* iPage+ iLine;
						if(iLineNum> iTotalLines)
							break;
						*(int*)pstrBuffer= iCharsPerLine;
						//用TextOut把这一行送到打印机设备内容中
						TextOut(pd.hDC, 0, yChar*iLine, pstrBuffer, 
							(int)SendMessage(hwndEdit, EM_GETLINE, (WPARAM)iLineNum, (LPARAM)pstrBuffer));
					}
					if(EndPage(pd.hDC<0))
					{
						bSuccess= FALSE;
						break;
					}
					if(bUserAbort)
						break;
				}
				if(!bSuccess|| bUserAbort)
					break;
			}
			if(!bSuccess|| bUserAbort)
				break;
		}
	}
	else
		bSuccess= FALSE;
	if(bSuccess)
		EndDoc(pd.hDC);
	if(!bUserAbort)
	{
		EnableWindow(hwnd, TRUE);
		DestroyWindow(hDlgPrint);
	}
	free(pstrBuffer);
	DeleteDC(pd.hDC);

	return bSuccess&& !bUserAbort;
}