/*
要在链接依赖里面加上WIMM.LIB、
这个程序主要介绍并使用了mciSendCommand函数，通过发送不同的消息，我们执行了录音，播放等一系列事情，仅仅只是需要发送消息即可
这个方式比之前的record1要方便许多。
*/
#include <windows.h>
#include "resource.h"

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("Record2");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	if(-1== DialogBox(hInstance, TEXT("Record"), NULL, DlgProc))
	{
		MessageBox(NULL, TEXT("Create Failed"), szAppName, MB_ICONERROR);
	}
	return 0;
}

void ShowError(HWND hwnd, DWORD dwError)
{
	TCHAR szErrorStr[1024];
	mciGetErrorString(dwError, szErrorStr, sizeof(szErrorStr)/ sizeof(TCHAR));
	MessageBeep(MB_ICONEXCLAMATION);
	MessageBox(hwnd, szErrorStr, szAppName, MB_OK| MB_ICONEXCLAMATION);
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bRecording, bPlaying, bPaused;
	static TCHAR szFileName[]= TEXT("record2.wav");
	static WORD wDeviceID;
	DWORD dwError;
	MCI_GENERIC_PARMS mciGeneric;
	MCI_OPEN_PARMS mciOpen;
	MCI_PLAY_PARMS mciPlay;
	MCI_RECORD_PARMS mciRecord;
	MCI_SAVE_PARMS mciSave;

	switch(message)
	{
	case WM_COMMAND:
		switch(wParam)
		{
		case IDC_RECORD_BEG:	//录音开始
			//先删除已经存在的录音
			DeleteFile(szFileName);
			//发送mci消息来打开设备。与record1的waveInOpen累死，但是更便捷。
			mciOpen.dwCallback= 0;
			mciOpen.wDeviceID= 0;
			mciOpen.lpstrDeviceType= TEXT("waveaudio");//说明设备型态
			mciOpen.lpstrElementName= TEXT("");
			mciOpen.lpstrAlias= NULL;
			dwError= mciSendCommand(0, MCI_OPEN, MCI_WAIT| MCI_OPEN_TYPE| MCI_OPEN_ELEMENT, 
				(DWORD)(LPMCI_OPEN_PARMS)&mciOpen);	
			if(dwError!=0)
			{
				ShowError(hwnd, dwError);
				return TRUE;
			}
			//Save the Device ID
			wDeviceID= mciOpen.wDeviceID;	//成功打开设备后可以获得设备ID，保存后之后使用
			//发送mci消息来开始录音
			mciRecord.dwCallback= (DWORD)hwnd;
			mciRecord.dwFrom= 0;
			mciRecord.dwTo= 0;
			mciSendCommand(wDeviceID, MCI_RECORD, MCI_NOTIFY, 
				(DWORD)(LPMCI_RECORD_PARMS)&mciRecord);
			//Enable and disable buttons
			EnableWindow(GetDlgItem(hwnd, IDC_RECORD_BEG), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), FALSE);
			SetFocus (GetDlgItem (hwnd, IDC_RECORD_END)) ;
			bRecording= TRUE;
			return TRUE;
		case IDC_RECORD_END:	//按下end后，将收到该消息
			//发送mci消息来停止
			mciGeneric.dwCallback= 0;
			mciSendCommand(wDeviceID, MCI_STOP, MCI_WAIT, 
				(DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);
			//发送mci消息来保存文件
			mciSave.dwCallback= 0;
			mciSave.lpfilename= szFileName;
			mciSendCommand(wDeviceID, MCI_SAVE, MCI_WAIT| MCI_SAVE_FILE, 
				(DWORD)(LPMCI_SAVE_PARMS)&mciSave);
			//发送mci消息来关闭设备
			mciSendCommand(wDeviceID, MCI_CLOSE, MCI_WAIT,
				(DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);
			//Enable andd disable buttons
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), TRUE);
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), TRUE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), FALSE);
			SetFocus (GetDlgItem (hwnd, IDC_PLAY_BEG)) ;
			bRecording= FALSE;
			return TRUE;
		case IDC_PLAY_BEG:	//播放录音
			//发送mci消息打开设备
			mciOpen.dwCallback= 0;
			mciOpen.wDeviceID= 0;
			mciOpen.lpstrDeviceType= NULL;
			mciOpen.lpstrElementName= szFileName;	//指定文件名
			mciOpen.lpstrAlias= NULL;
			dwError= mciSendCommand(0, MCI_OPEN, MCI_WAIT|MCI_OPEN_ELEMENT, 
				(DWORD)(LPMCI_OPEN_PARMS)&mciOpen);

			if(dwError!=0 )
			{
				ShowError(hwnd, dwError);
				return TRUE;
			}
			//Save the Device ID
			wDeviceID= mciOpen.wDeviceID;
			//发送mci消息播放录音
			mciPlay.dwCallback= (DWORD)hwnd;
			mciPlay.dwFrom= 0;
			mciPlay.dwTo= 0;
			mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY, 
				(DWORD)(LPMCI_PLAY_PARMS)&mciPlay);
			//Enable and disable buttons
			EnableWindow(GetDlgItem(hwnd, IDC_RECORD_BEG), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), TRUE);
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), TRUE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), FALSE);
			SetFocus (GetDlgItem (hwnd, IDC_PLAY_BEG)) ;
		case IDC_PLAY_PAUSE:	//暂停，同上也是发送mci消息
			if(!bPaused)
			{
				mciGeneric.dwCallback= 0;
				mciSendCommand(wDeviceID, MCI_PAUSE, MCI_WAIT,
					(DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);
				SetDlgItemText(hwnd, IDC_PLAY_PAUSE, TEXT("Resume"));
				bPaused= TRUE;
			}
			else
			{
				mciPlay.dwCallback= (DWORD)hwnd;
				mciPlay.dwFrom= 0;
				mciPlay.dwTo= 0;
				mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY, 
					(DWORD)(LPMCI_PLAY_PARMS)&mciPlay);
				SetDlgItemText(hwnd, IDC_PLAY_PAUSE, TEXT("Pause"));
				bPaused= FALSE;
			}
			return TRUE;
		case IDC_PLAY_END:	//结束，分别发送mci消息停止及关闭设备
			//Stop and close
			mciGeneric.dwCallback= 0;
			mciSendCommand(wDeviceID, MCI_STOP, MCI_WAIT, 
				(DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);
			mciSendCommand(wDeviceID, MCI_CLOSE, MCI_WAIT, 
				(DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);
			//Enable and disable buttons
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), FALSE);
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), FALSE);
			SetFocus (GetDlgItem (hwnd, IDC_PLAY_BEG)) ;
			bPlaying= FALSE;
			bPaused= FALSE;
			return TRUE;
		}
		break;
	case MM_MCINOTIFY:	//播放时，在声音文件结束后接收到MM_MCINOTIFY消息
		switch(wParam)
		{
		case MCI_NOTIFY_SUCCESSFUL:	//当播放结束或录音结束后发送mci消息
			if(bPlaying)
				SendMessage(hwnd, WM_COMMAND, IDC_PLAY_END, 0);
			if(bRecording)
				SendMessage(hwnd, WM_COMMAND, IDC_RECORD_END, 0);
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