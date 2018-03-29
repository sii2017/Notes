/*
需要在项目的properties中linker的addition depends中添加WINMM.lib才能正确调用mci相关的函数。
lib和dll应该都在c盘的固定路径里
*/
#include <windows.h>
#include "resource.h"

#define ID_TIMER 1
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]=  TEXT("TestMci");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	if(-1== DialogBox(hInstance, szAppName, NULL, DlgProc))
	{
		MessageBox(NULL, TEXT("Something Wrong"), szAppName, MB_ICONERROR);
	}
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEdit;
	int iCharBeg, iCharEnd, iLineBeg, iLineEnd, iChar, iLine, iLength;
	MCIERROR error;		//实际上就是DWORD
	RECT rect;
	TCHAR szCommand[1024], szReturn[1024], szError[1024], szBuffer[32];

	switch(message)
	{
	case WM_INITDIALOG:
		//Center the windows on screen
		GetWindowRect(hwnd, &rect);
		SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN)- rect.right+ rect.left)/2, 
			(GetSystemMetrics(SM_CYSCREEN)- rect.bottom+ rect.top)/2, 
			0,0, SWP_NOZORDER| SWP_NOSIZE);

		hwndEdit= GetDlgItem(hwnd, IDC_MAIN_EDIT);
		SetFocus(hwndEdit);
		return FALSE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			SendMessage(hwndEdit, EM_GETSEL, (WPARAM)&iCharBeg, (LPARAM)&iCharEnd);
			iLineBeg= SendMessage(hwndEdit, EM_LINEFROMCHAR, iCharBeg, 0);
			iLineEnd= SendMessage(hwndEdit, EM_LINEFROMCHAR, iCharEnd, 0);

			for(iLine= iLineBeg; iLine<= iLineEnd; iLine++)
			{
				*(WORD*)szCommand= sizeof(szCommand)/ sizeof(TCHAR);
				iLength= SendMessage(hwndEdit, EM_GETLINE, iLine, 
					(LPARAM)szCommand);
				szCommand[iLength]= '\0';
				 if(iLength==0)
					 continue;

				 error= mciSendString(szCommand, szReturn, sizeof(szReturn)/ sizeof(TCHAR), hwnd);

				 SetDlgItemText(hwnd, IDC_RETURN_STRING, szReturn);

				 mciGetErrorString(error, szError, sizeof(szError)/ sizeof(TCHAR));

				 SetDlgItemText(hwnd, IDC_ERROR_STRING, szError);
			}
			iChar= SendMessage(hwndEdit, EM_LINEINDEX, iLineEnd, 0);
			iChar+= SendMessage(hwndEdit, EM_SETSEL, iChar, iChar);

			SendMessage(hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)TEXT("\r\n"));
			SetFocus(hwndEdit);

			return TRUE;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		case IDC_MAIN_EDIT:
			if(HIWORD(wParam)== EN_ERRSPACE)
			{
				MessageBox(hwnd, TEXT("Error control out of space."),
					szAppName, MB_OK| MB_ICONINFORMATION);
				return TRUE;
			}
			break;
		}
		break;
	case MM_MCINOTIFY:
		EnableWindow(GetDlgItem(hwnd, IDC_NOTIFY_MESSAGE), TRUE);
		wsprintf(szBuffer, TEXT("Device ID= %i"), lParam);
		SetDlgItemText(hwnd, IDC_NOTIFY_ID, szBuffer);
		EnableWindow(GetDlgItem(hwnd, IDC_NOTIFY_ID), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_NOTIFY_SUCCESSFUL), 
			wParam& MCI_NOTIFY_SUCCESSFUL);
		EnableWindow(GetDlgItem(hwnd, IDC_NOTIFY_SUPERSEDED), 
			wParam& MCI_NOTIFY_SUPERSEDED);
		EnableWindow(GetDlgItem(hwnd, IDC_NOTIFY_ABORTED), 
			wParam& MCI_NOTIFY_ABORTED);
		EnableWindow(GetDlgItem(hwnd, IDC_NOTIFY_FAILURE),
			wParam& IDC_NOTIFY_FAILURE);

		SetTimer(hwnd, ID_TIMER, 5000, NULL);
		return TRUE;
	case WM_TIMER:
		KillTimer(hwnd, ID_TIMER);

		EnableWindow(GetDlgItem (hwnd, IDC_NOTIFY_MESSAGE), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_NOTIFY_ID), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_NOTIFY_SUCCESSFUL), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_NOTIFY_SUPERSEDED), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_NOTIFY_ABORTED), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_NOTIFY_FAILURE), FALSE) ;
		return TRUE ;
	case WM_SYSCOMMAND:
		switch(LOWORD(wParam))
		{
		case SC_CLOSE:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
