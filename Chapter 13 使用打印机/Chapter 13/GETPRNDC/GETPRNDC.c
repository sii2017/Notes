/*
这个文件好像没法运行啊，而且现在也没有NT和98了。
现在是win10，2018-01-25 17:35:05
那就先这样吧。

GetVersion函数用来确定程序执行在什么操作系统下
函数调用EnumPrinters两次，一次取得它所需结构的大小，一次填入结构
在Windows 98上，函数使用PRINTER_INFO_5结构；在Windows NT上，函数使用PRINTER_INFO_4结构。
那在win10上填入什么？
*/
#include <windows.h>
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
	else	//windows NT
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