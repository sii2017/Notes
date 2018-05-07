/*
mciExecute曾经是API函数，但是现在已经不是了，所以手动将其实现出来。
与record2的区别是将所有的mciSendCommand替换成了mciExecute加以字符串命令。
如果需要实际使用需要了解所有的字符串命令。
但是现在可能已经不适用这个模式了。
*/
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

//这个函数曾经是API函数，但是现在已经不是了，所以手动将其实现出来。
BOOL mciExecute(LPCTSTR szCommand)
{
	MCIERROR error;
	TCHAR szErrorStr[1024];

	//以命令字符串的形式发送。曾经使用，现在应该已经不常用了。
	if(error= mciSendString(szCommand, NULL, 0, NULL))
	{
		mciGetErrorString(error, szErrorStr, sizeof(szErrorStr)/sizeof(TCHAR));
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(NULL, szErrorStr, TEXT("MCI Error"), MB_OK| MB_ICONEXCLAMATION);
	}
	return error== 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bRecording, bPlaying, bPaused;
	switch(message)
	{
	case WM_COMMAND:
		switch(wParam)
		{
		case IDC_RECORD_BEG:
			//Delete existing waveform file
			DeleteFile(TEXT("record3.wav"));
			//Open waveform audio and record
			if(!mciExecute(TEXT("open new type waveaudio alias mysound")))	//将mysound作为别名
				return TRUE;
			mciExecute(TEXT("record mysound"));
			//Enable and disable buttons
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), FALSE);
			SetFocus (GetDlgItem (hwnd, IDC_RECORD_END)) ;
			bRecording= TRUE;
			return TRUE;
		case IDC_RECORD_END:
			//Stop save and close recording
			mciExecute(TEXT("stop mysound"));
			mciExecute(TEXT("save mysound record3.wav"));
			mciExecute(TEXT("close mysound"));
			//Enable and disable buttons
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), FALSE);
			SetFocus (GetDlgItem (hwnd, IDC_RECORD_END)) ;
			bRecording= FALSE;
			return TRUE;
		case IDC_PLAY_BEG:
			//Open waveform audio and play
			if(!mciExecute(TEXT("open record3.wav alias mysound")))
				return TRUE;
			mciExecute(TEXT("play mysound"));
			//Enable and disable buttons
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), TRUE) ;
			SetFocus (GetDlgItem (hwnd, IDC_PLAY_END)) ;
			bPlaying= TRUE;
			return TRUE;
		case IDC_PLAY_PAUSE:
			if(!bPaused)
			{
				mciExecute(TEXT("pause mysound"));
				SetDlgItemText(hwnd, IDC_PLAY_PAUSE, TEXT("Resume"));
				bPaused= TRUE;
			}
			else
			{
				mciExecute(TEXT("play mysound"));
				SetDlgItemText(hwnd, IDC_PLAY_PAUSE, TEXT("Pause"));
				bPaused= FALSE;
			}
			return TRUE;
		case IDC_PLAY_END:
			//Stop and close
			mciExecute(TEXT("stop mysound"));
			mciExecute(TEXT("close mysound"));
			//Enable and disable buttons
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), FALSE);
			SetFocus (GetDlgItem (hwnd, IDC_PLAY_BEG)) ;
			bPlaying=  FALSE;
			bPaused= FALSE;
			return TRUE;
		}
		break;
	case WM_SYSCOMMAND:
		switch(wParam)
		{
		case SC_CLOSE:
			if(bRecording)
				SendMessage(hwnd, WM_COMMAND, IDC_RECORD_END, 0L);
			if(bPlaying)
				SendMessage(hwnd, WM_COMMAND, IDC_PLAY_END, 0L);
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}