/*
稍显复杂的录音程序。代码方面并不复杂，只是各个消息交错比较难记。85行应该是个小错误。
需要注意，尽管没有提示，但是如同TESTMCI
需要在项目的properties中linker的addition depends中添加WINMM.lib才能正确调用mci相关的函数。
lib和dll应该都在c盘的固定路径里   
调用顺序是waveOutOpen -> waveOutPrepareHeader ->waveOutWrite
*/
#include <windows.h>
#include "resource.h"

#define INP_BUFFER_SIZE 16384
BOOL CALLBACK DlgProc(HWND, UINT , WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("Record1");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	if(-1== DialogBox(hInstance, TEXT("Record"), NULL, DlgProc))
	{
		MessageBox(NULL, TEXT("Create Failed"), szAppName, MB_ICONERROR);
	}
	return 0;
}

void ReverseMemory(BYTE* pBuffer, int iLength)
{
	BYTE b;
	int i;

	for(i=0; i<iLength/2; i++)
	{
		b=pBuffer[i];
		pBuffer[i]= pBuffer[iLength-i-1];
		pBuffer[iLength-i-1]= b;
	}
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bRecording, bPlaying, bReverse, bPaused, bEnding, bTerminating;
	static DWORD dwDataLength, dwRepetitions= 1;
	static HWAVEIN hWaveIn;
	static HWAVEOUT hWaveOut;
	static PBYTE pBuffer1, pBuffer2, pSaveBuffer, pNewBuffer;
	static PWAVEHDR pWaveHdr1, pWaveHdr2;
	static TCHAR szOpenError[]= TEXT("Error opening waveform audio!");
	static TCHAR szMemError[]= TEXT("Error allocating memory!");
	static WAVEFORMATEX waveform;

	switch(message)
	{
	case WM_INITDIALOG:
		pWaveHdr1= malloc(sizeof(WAVEHDR));
		pWaveHdr2= malloc(sizeof(WAVEHDR));
		
		pSaveBuffer= malloc(1);
		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_RECORD_BEG:
			pBuffer1= malloc(INP_BUFFER_SIZE);
			pBuffer2= malloc(INP_BUFFER_SIZE);

			if(!pBuffer1|| !pBuffer2)
			{
				if(pBuffer1) free(pBuffer1);
				if(pBuffer2) free(pBuffer2);

				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox(hwnd, szMemError, szAppName, MB_ICONEXCLAMATION|MB_OK);
				return TRUE;
			}
			//Open waveform audio for input
			waveform.wFormatTag= WAVE_FORMAT_PCM;
			waveform.nChannels= 1;
			waveform.nSamplesPerSec= 11025;
			waveform.nAvgBytesPerSec= 11025;
			waveform.nBlockAlign= 1;
			waveform.wBitsPerSample= 8;
			waveform.cbSize= 0;

			//通过调用waveOutOpen函数，SINEWAVE打开波形声音设备以便输出
			//这里的代码应该有问题，判断应该像上一个代码一个当它的返回值等于error才进入if内部
			//虽然查阅英语原版和中文版书都是如此，但是这样逻辑是不不通的。
			if(waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD)hwnd, 0, CALLBACK_WINDOW))
			{
				free(pBuffer1);
				free(pBuffer2);
				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox(hwnd, szOpenError, szAppName, MB_ICONEXCLAMATION|MB_OK);
			}

			pWaveHdr1->lpData= pBuffer1;
			pWaveHdr1->dwBufferLength= INP_BUFFER_SIZE;
			pWaveHdr1->dwBytesRecorded= 0;
			pWaveHdr1->dwUser=0;
			pWaveHdr1->dwFlags= 0;
			pWaveHdr1->dwLoops= 1;
			pWaveHdr1->lpNext= NULL;
			pWaveHdr1->reserved= 0;
			waveInPrepareHeader(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));	//该函数将在音频媒体中播放pWaveHdr1指定的数据

			pWaveHdr2->lpData= pBuffer2;
			pWaveHdr2->dwBufferLength= INP_BUFFER_SIZE;
			pWaveHdr2->dwBytesRecorded= 0;
			pWaveHdr2->dwUser= 0;
			pWaveHdr2->dwFlags= 0;
			pWaveHdr2->dwLoops= 1;
			pWaveHdr2->lpNext= NULL;
			pWaveHdr2->reserved= 0;
			waveInPrepareHeader(hWaveIn, pWaveHdr2, sizeof(WAVEHDR));	//该函数将在音频媒体中播放pWaveHdr2指定的数据
			
			return TRUE;
		case IDC_RECORD_END:
			bEnding= TRUE;
			waveInReset(hWaveIn);//waveInReset函数使录音停止，并产生MM_WIM_DATA消息
			return TRUE;
		case IDC_PLAY_BEG:	//开始录音
			waveform.wFormatTag= WAVE_FORMAT_PCM;
			waveform.nChannels= 1;
			waveform.nSamplesPerSec= 11025;
			waveform.nAvgBytesPerSec= 11025;
			waveform.nBlockAlign= 1;
			waveform.wBitsPerSample= 8;
			waveform.cbSize= 0;

			if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveform, 
				(DWORD)hwnd, 0, CALLBACK_WINDOW))
			{
				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox(hwnd, szOpenError, szAppName, MB_ICONEXCLAMATION|MB_OK);
			}
			return TRUE;
		case IDC_PLAY_PAUSE:	//暂停或者继续，并且更改按键显示。
			//Pause or restart output
			if(!bPaused)
			{
				waveOutPause(hWaveOut);
				SetDlgItemText(hwnd, IDC_PLAY_PAUSE, TEXT("Resume"));
				bPaused= TRUE;
			}
			else
			{
				waveOutRestart(hWaveOut);
				SetDlgItemText(hwnd, IDC_PLAY_PAUSE, TEXT("Pause"));
				bPaused= FALSE;
			}
			return TRUE;
		case IDC_PLAY_END:	//结束。重置变量
			//Reset output for close preparation
			bEnding= TRUE;
			waveOutReset(hWaveOut);
			return TRUE;
		case IDC_PLAY_REV:	//逆向播放录音
			//Reverse save buffer and play
			bReverse= TRUE;
			ReverseMemory(pSaveBuffer, dwDataLength);	//该函数把储存缓冲区里的数据按字节顺序反向
			SendMessage(hwnd, WM_COMMAND, IDC_PLAY_BEG, 0);
			return TRUE;
		case IDC_PLAY_REP:	//Repeat重复播放
			//Set infinite repetitions and play
			dwRepetitions= -1;
			SendMessage(hwnd, WM_COMMAND, IDC_PLAY_BEG, 0);
			return TRUE;
		case IDC_PLAY_SPEED:	//加速播放，仅仅将nSamplesPerSec和nAvgBytesPerSec翻倍，即可按原来流程播放
			//Open waveform audio for fast output
			waveform.wFormatTag= WAVE_FORMAT_PCM;
			waveform.nChannels= 1;
			waveform.nSamplesPerSec= 22050;
			waveform.nAvgBytesPerSec= 22050;
			waveform.nBlockAlign= 1;
			waveform.wBitsPerSample= 8;
			waveform.cbSize= 0;
			if(waveOutOpen(&hWaveOut, 0, &waveform, (DWORD)hwnd, 0, CALLBACK_WINDOW))
			{
				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox(hwnd, szOpenError, szAppName, MB_ICONEXCLAMATION| MB_OK);
			}
			return TRUE;
		}
		break;
	case MM_WIM_OPEN:	//当打开了音频设备后触发
		//Shrink down the save buffer
		pSaveBuffer= realloc(pSaveBuffer, 1);
		//Enable and disable buttons
		EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), TRUE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_REV), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_REP), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_SPEED), FALSE) ;
		SetFocus (GetDlgItem (hwnd, IDC_RECORD_END)) ;

		//Add the buffers
		waveInAddBuffer(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));
		waveInAddBuffer(hWaveIn, pWaveHdr2, sizeof(WAVEHDR));

		//Begin sampling
		bRecording= TRUE;
		bEnding= FALSE;
		dwDataLength= 0;
		waveInStart(hWaveIn);	//开始录音
		return TRUE;
	case MM_WIM_DATA:	//由waveInAddBuffer触发
		//Reallocate save buffer memory
		pNewBuffer= realloc(pSaveBuffer, dwDataLength+ ((PWAVEHDR)lParam)->dwBytesRecorded);

		if(pNewBuffer== NULL)
		{
			waveInClose(hWaveIn);
			MessageBeep(MB_ICONEXCLAMATION);
			MessageBox(hwnd, szMemError, szAppName, 
				MB_ICONEXCLAMATION| MB_OK);
			return TRUE;
		}
		pSaveBuffer= pNewBuffer;
		CopyMemory(pSaveBuffer+ dwDataLength, ((PWAVEHDR)lParam)->lpData,
			((PWAVEHDR)lParam)->dwBytesRecorded);
		dwDataLength+= ((PWAVEHDR)lParam)->dwBytesRecorded;

		if(bEnding)
		{
			waveInClose(hWaveIn);
			return TRUE;
		}

		//Send out a new buffer
		waveInAddBuffer(hWaveIn, (PWAVEHDR)lParam, sizeof(WAVEHDR));
		return TRUE;
	case MM_WIM_CLOSE:
		//Free the buffer memory
		waveInUnprepareHeader(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));
		waveInUnprepareHeader(hWaveIn, pWaveHdr2, sizeof(WAVEHDR));
		free(pBuffer1);
		free(pBuffer2);

		//Enable the disable buttons
		EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), TRUE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), FALSE) ;
		SetFocus (GetDlgItem (hwnd, IDC_RECORD_BEG)) ;
		if (dwDataLength > 0)
		{
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), FALSE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), FALSE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_REP), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_REV), TRUE) ;
			EnableWindow (GetDlgItem (hwnd, IDC_PLAY_SPEED), TRUE) ;
			SetFocus (GetDlgItem (hwnd, IDC_PLAY_BEG)) ;
		}
		bRecording = FALSE ;

		if (bTerminating)
			SendMessage (hwnd, WM_SYSCOMMAND, SC_CLOSE, 0L) ;
		return TRUE ;
	case MM_WOM_OPEN:	//播放声音
		// Enable and disable buttons
		EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), TRUE);
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), TRUE);
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_REP), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_REV), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_SPEED), FALSE) ;
		SetFocus (GetDlgItem (hwnd, IDC_PLAY_END)) ;

		//Set up header
		pWaveHdr1->lpData= pSaveBuffer;
		pWaveHdr1->dwBufferLength= dwDataLength;
		pWaveHdr1->dwBytesRecorded= 0;
		pWaveHdr1->dwUser= 0;
		pWaveHdr1->dwFlags= WHDR_BEGINLOOP| WHDR_ENDLOOP;
		pWaveHdr1->dwLoops= dwRepetitions;
		pWaveHdr1->lpNext= NULL;
		pWaveHdr1->reserved= 0;

		//Prepare and write
		waveOutPrepareHeader(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));
		waveOutClose(hWaveOut);
		return TRUE;
	case MM_WOM_DONE:	//播放完储存缓冲区里的所有数据后产生
		waveOutUnprepareHeader(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));
		waveOutClose(hWaveOut);
		return TRUE;
	case MM_WOM_CLOSE:	//关闭
		//Enable and disable buttons
		EnableWindow (GetDlgItem (hwnd, IDC_RECORD_BEG), TRUE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_RECORD_END), TRUE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_BEG), TRUE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_PAUSE), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_END), FALSE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_REV), TRUE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_REP), TRUE) ;
		EnableWindow (GetDlgItem (hwnd, IDC_PLAY_SPEED), TRUE) ;
		SetFocus (GetDlgItem (hwnd, IDC_PLAY_BEG)) ;
		SetDlgItemText (hwnd, IDC_PLAY_PAUSE, TEXT ("Pause")) ;
		bPaused= FALSE;
		dwRepetitions= 1;
		bPlaying= FALSE;

		if(bReverse)
		{
			ReverseMemory(pSaveBuffer, dwDataLength);
			bReverse= FALSE;
		}

		if(bTerminating)
			SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0L);
		return TRUE;
	case WM_SYSCOMMAND:
		switch(LOWORD(wParam))
		{
		case SC_CLOSE:
			if(bRecording)
			{
				bTerminating= TRUE;
				bEnding= TRUE;
				waveInReset(hWaveIn);
				return TRUE;
			}
			if(bPlaying)
			{
				bTerminating= TRUE;
				bEnding= TRUE;
				waveOutReset(hWaveOut);
				return TRUE;
			}
			free(pWaveHdr1);
			free(pWaveHdr2);
			free(pSaveBuffer);
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
