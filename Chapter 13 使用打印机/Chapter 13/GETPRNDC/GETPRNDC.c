/*
����ļ�����û�����а�����������Ҳû��NT��98�ˡ�
������win10��2018-01-25 17:35:05
�Ǿ��������ɡ�

GetVersion��������ȷ������ִ����ʲô����ϵͳ��
��������EnumPrinters���Σ�һ��ȡ��������ṹ�Ĵ�С��һ������ṹ
��Windows 98�ϣ�����ʹ��PRINTER_INFO_5�ṹ����Windows NT�ϣ�����ʹ��PRINTER_INFO_4�ṹ��
����win10������ʲô��
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