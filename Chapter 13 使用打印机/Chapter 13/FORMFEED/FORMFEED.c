/*
通过两次测试返回值，完成了（虚拟？）打印
*/
#include <windows.h>
HDC GetPrinterDC(void);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static DOCINFO di= 
	{
		sizeof(DOCINFO), TEXT("FormFeed")
	};
	HDC hdcPrint= GetPrinterDC();
	if(hdcPrint!=NULL)
	{
		//首先呼叫StartDoc开始一个新的文件测试从StartDoc的返回值，当该值为正，才继续
		//di的第一个字段包含了结构的大小，在第二个字段包含了字符串「FormFeed」。
		//当文件正在被打印或者在等待打印时，这个字符串将出现在打印机任务队列中的「Document Name」列中。
		//通常，该字符串包含进行打印的应用程序名称和被打印的文件名称。（2个名称？）
		if(StartDoc(hdcPrint, &di)>0)
			//继续调用StartPage，紧接着调用EndPage
			//这一程序将打印机推进到新的一页，再次对传回值进行测试
			if(StartPage(hdcPrint)>0&& EndPage(hdcPrint)>0)
				EndDoc(hdcPrint);//最后，如果不出错，文件就结束：
		DeleteDC(hdcPrint);
	}
	return 0;
}

//由于该程序也需要之前的GETPPRNDC.C文件，我将那个程序复制到下面来

HDC GetPrinterDC(void)
{
	DWORD dwNeeded, dwReturned;
	HDC hdc;
	PRINTER_INFO_4* pinfo4;
	PRINTER_INFO_5* pinfo5;

	if(GetVersion()& 0x80000000)	//windows 98
	{
		EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, NULL, 0, 
			&dwNeeded, &dwReturned);
		pinfo5= malloc(dwNeeded);
		EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, (PBYTE)pinfo5, 
			dwNeeded, &dwNeeded, &dwReturned);
		hdc= CreateDC(NULL, pinfo5->pPrinterName, NULL, NULL);
		free(pinfo5);
	}
	else	//windows NT，实际上win10也进这里
	{
		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, 
			&dwNeeded, &dwReturned);
		pinfo4= malloc(dwNeeded);
		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pinfo4, 
			dwNeeded, &dwNeeded, &dwReturned);
		hdc= CreateDC(NULL, pinfo4->pPrinterName, NULL, NULL);
		free(pinfo4);
	}
	return hdc;
}