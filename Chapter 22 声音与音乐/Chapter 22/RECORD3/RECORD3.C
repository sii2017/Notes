#include <windows.h>
#include "resource.h"

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("Record3");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	if(-1== DialogBox(hInstance, TEXT("Record"), NULL, DlgProc))
	{
		MessageBox(NULL, TEXT("Create Wrong"), szAppName, MB_ICONERROR);
	}
	return 0;
}

BOOL mciExecute(LPCTSTR szCommand)
{
	MCIERROR error;
	TCHAR szErrorStr[1024];