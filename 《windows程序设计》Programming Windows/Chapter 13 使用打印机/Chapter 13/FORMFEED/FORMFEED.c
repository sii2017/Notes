/*
ͨ�����β��Է���ֵ������ˣ����⣿����ӡ
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
		//���Ⱥ���StartDoc��ʼһ���µ��ļ����Դ�StartDoc�ķ���ֵ������ֵΪ�����ż���
		//di�ĵ�һ���ֶΰ����˽ṹ�Ĵ�С���ڵڶ����ֶΰ������ַ�����FormFeed����
		//���ļ����ڱ���ӡ�����ڵȴ���ӡʱ������ַ����������ڴ�ӡ����������еġ�Document Name�����С�
		//ͨ�������ַ����������д�ӡ��Ӧ�ó������ƺͱ���ӡ���ļ����ơ���2�����ƣ���
		if(StartDoc(hdcPrint, &di)>0)
			//��������StartPage�������ŵ���EndPage
			//��һ���򽫴�ӡ���ƽ����µ�һҳ���ٴζԴ���ֵ���в���
			if(StartPage(hdcPrint)>0&& EndPage(hdcPrint)>0)
				EndDoc(hdcPrint);//�������������ļ��ͽ�����
		DeleteDC(hdcPrint);
	}
	return 0;
}

//���ڸó���Ҳ��Ҫ֮ǰ��GETPPRNDC.C�ļ����ҽ��Ǹ������Ƶ�������

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
	else	//windows NT��ʵ����win10Ҳ������
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