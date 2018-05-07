/*
由于本项目需要用到这里的函数，所以复制黏贴过来，没有修改，与GETPRNDC中的一致。
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
		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0,		//给dwNeeded dwReturned填值
			&dwNeeded, &dwReturned);
		pinfo4= malloc(dwNeeded);								//分配内存
		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pinfo4, //给pinfo4填值
			dwNeeded, &dwNeeded, &dwReturned);
		hdc= CreateDC(NULL, pinfo4->pPrinterName, NULL, NULL);	//通过pinfo4获取hdc
		free(pinfo4);											//释放pinfo4
	}
	return hdc;
}