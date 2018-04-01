/*
需要注意，尽管没有提示，但是如同TESTMCI
需要在项目的properties中linker的addition depends中添加WINMM.lib才能正确调用mci相关的函数。
lib和dll应该都在c盘的固定路径里   
调用顺序是waveOutOpen -> waveOutPrepareHeader ->waveOutWrite
*/
#include <windows.h>
#include <math.h>
#include "resource.h"

#define SAMPLE_RATE  11025
#define FREQ_MIN  20
#define FREQ_MAX 5000
#define FREQ_INIT  440 
#define OUT_BUFFER_SIZE  4096
#define PI  3.14159

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("SineWave");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//建立对话框
	if(-1== DialogBox(hInstance, szAppName, NULL, DlgProc))
	{
		MessageBox(NULL, TEXT("Something Wrong"), szAppName, MB_ICONERROR);
	}
	return 0;
}

VOID FillBuffer(PBYTE pBuffer, int iFreq)
{
	static double fAngle;	//静态变量将会自动初始化。
	int i;

	for(i=0; i<OUT_BUFFER_SIZE; i++)
	{
		pBuffer[i]= (BYTE)(127+ 127*sin(fAngle));
		fAngle+= 2*PI*iFreq/SAMPLE_RATE;
		/*
		但是每周期的样本数可能带有小数，因此在使用时这种方法并不是很好。每个周期的尾部都会有间断。   
		使它正常工作的关键是保留一个静态的「相位角」变量。此角初始化为0。第一个样本是0度正弦。
		随后，相位角增加一个值，该值等于2π乘以频率再除以取样频率。用此相位角作为第二个样本，并且按此方法继续。一旦相位角超过2π弧度，则减去2π弧度，而不要把相位角再初始化为0。
		*/
		if(fAngle> 2*PI)
			fAngle-= 2*PI;
	}
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bShutOff, bClosing;
	static HWAVEOUT hWaveOut;
	static HWND hwndScroll;
	static int iFreq= FREQ_INIT;
	static PBYTE pBuffer1, pBuffer2;
	static PWAVEHDR pWaveHdr1, pWaveHdr2;
	static WAVEFORMATEX waveformat;
	int iDummy;

	switch(message)
	{
	case WM_INITDIALOG:
		hwndScroll= GetDlgItem(hwnd, IDC_SCROLL);
		SetScrollRange(hwndScroll, SB_CTL, FREQ_MIN, FREQ_MAX, FALSE);	//设置滚动条范围
		SetScrollPos(hwndScroll, SB_CTL, FREQ_INIT, TRUE);	//设置滚动条初始位置
		SetDlgItemInt(hwnd, IDC_TEXT, FREQ_INIT, FALSE);
		return TRUE;
	case WM_HSCROLL:		//控制滚动条的基本操作
		switch(LOWORD(wParam))
		{
		case SB_LINELEFT:
			iFreq-= 1; break;
		case SB_LINERIGHT: 
			iFreq+=1; break;
		case SB_PAGELEFT:
			iFreq/=2; break;
		case SB_PAGERIGHT:
			iFreq*= 2; break;
		case SB_THUMBTRACK:
			iFreq= HIWORD(wParam);
			break;
		case SB_TOP:
			GetScrollRange(hwndScroll, SB_CTL, &iFreq, &iDummy);
			break;
		case SB_BOTTOM:
			GetScrollRange(hwndScroll, SB_CTL, &iDummy, &iFreq);
			break;
		}
		iFreq= max(FREQ_MIN, min(FREQ_MAX, iFreq));
		SetScrollPos(hwndScroll, SB_CTL, iFreq, TRUE);
		SetDlgItemInt(hwnd, IDC_TEXT, iFreq, FALSE);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_ONOFF:
			if(hWaveOut== NULL)	//唯一的一个按键
			{
				pWaveHdr1= malloc(sizeof(WAVEHDR));
				pWaveHdr2= malloc(sizeof(WAVEHDR));
				pBuffer1= malloc(OUT_BUFFER_SIZE);
				pBuffer2= malloc(OUT_BUFFER_SIZE);

				if(!pWaveHdr1|| !pWaveHdr2|| !pBuffer1|| !pBuffer2)	//如果申请失败则释放，并发出提示
				{
					if(!pWaveHdr1) free(pWaveHdr1);
					if(!pWaveHdr2) free(pWaveHdr2);
					if(!pBuffer1) free(pBuffer1);
					if(!pBuffer2) free(pBuffer2);

					MessageBeep(MB_ICONEXCLAMATION);
					MessageBox(hwnd, TEXT("Error allocating memory!"), szAppName, 
						MB_ICONEXCLAMATION| MB_OK);
					return TRUE;
				}
				bShutOff= FALSE;

				//Open waveform audio for output
				waveformat.wFormatTag= WAVE_FORMAT_PCM;
				waveformat.nChannels= 1;
				waveformat.nSamplesPerSec= SAMPLE_RATE;
				waveformat.nAvgBytesPerSec= SAMPLE_RATE;
				waveformat.nBlockAlign= 1;
				waveformat.wBitsPerSample= 8;
				waveformat.cbSize= 0;

				//通过调用waveOutOpen函数，SINEWAVE打开波形声音设备以便输出
				if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveformat, (DWORD)hwnd, 0, CALLBACK_WINDOW)!=	//打开wav失败 这个也是很有趣，不等于noerror，即有error
					MMSYSERR_NOERROR)
				{
					free(pWaveHdr1);
					free(pWaveHdr2);
					free(pBuffer1);
					free(pBuffer2);

					hWaveOut= NULL;
					MessageBeep(MB_ICONEXCLAMATION);
					MessageBox(hwnd, TEXT("Error opening waveform audio device!"), 
						szAppName, MB_ICONEXCLAMATION| MB_OK);
					return TRUE;
				}

				//Set up headers and prepare them
				pWaveHdr1->lpData= pBuffer1;
				pWaveHdr1->dwBufferLength= OUT_BUFFER_SIZE;
				pWaveHdr1->dwBytesRecorded= 0;
				pWaveHdr1->dwUser= 0;
				pWaveHdr1->dwFlags= 0;
				pWaveHdr1->dwLoops= 1;
				pWaveHdr1->lpNext= NULL;
				pWaveHdr1->reserved= 0;

				//该函数将在音频媒体中播放pWaveHdr1指定的数据
				waveOutPrepareHeader(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));

				pWaveHdr2->lpData= pBuffer2;
				pWaveHdr2->dwBufferLength= OUT_BUFFER_SIZE;
				pWaveHdr2->dwBytesRecorded= 0;
				pWaveHdr2->dwUser= 0;
				pWaveHdr2->dwFlags= 0;
				pWaveHdr2->dwLoops= 1;
				pWaveHdr2->lpNext= NULL;
				pWaveHdr2->reserved= 0;

				//该函数将在音频媒体中播放pWaveHdr2指定的数据
				waveOutPrepareHeader(hWaveOut, pWaveHdr2, sizeof(WAVEHDR));
			}
			else	//if turning off waveform, reset waveform audio
			{
				bShutOff= TRUE;
				waveOutReset(hWaveOut);		//产生一条MM_WOM_DONE消息
			}
			return TRUE;
		}
		break;
	case MM_WOM_OPEN:
		//这里调用了两次waveOutWrite可能与波形的频率什么有关，当我注释掉一个以后，声音就会有间断，而两个一起播放则是连续的声音。
		SetDlgItemText(hwnd, IDC_ONOFF, TEXT("Turn Off"));
		//Send 2 buffers to waveform output device
		FillBuffer(pBuffer1, iFreq);
		waveOutWrite(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));
		FillBuffer(pBuffer2, iFreq);
		waveOutWrite(hWaveOut, pWaveHdr2, sizeof(WAVEHDR));
		return TRUE;
		//Message genereated when a buffer is finished
	case MM_WOM_DONE:		//由用户选择关闭按键的waveOutReset函数产生
		if(bShutOff)
		{
			waveOutClose(hWaveOut);	//产生一条MM_WOM_CLOSE消息
			return TRUE;
		}
		//Fill and send out a new buffer
		FillBuffer(((PWAVEHDR)lParam)->lpData, iFreq);
		waveOutWrite(hWaveOut, (PWAVEHDR)lParam, sizeof(WAVEHDR));
		return TRUE;
	case MM_WOM_CLOSE:		//由MM_WOM_DONE中的waveOutClose产生
		waveOutUnprepareHeader(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));	//清除由 waveOutPrepareHeader 完成的准备	
		waveOutUnprepareHeader(hWaveOut, pWaveHdr2, sizeof(WAVEHDR));
		free(pWaveHdr1);
		free(pWaveHdr2);
		free(pBuffer1);
		free(pBuffer2);

		hWaveOut= NULL;
		SetDlgItemText(hwnd, IDC_ONOFF, TEXT("Turn On"));
		if(bClosing)
			EndDialog(hwnd, 0);
		return TRUE;
	case WM_SYSCOMMAND:
		switch(wParam)
		{
		case SC_CLOSE:
			if(hWaveOut!= NULL)
			{
				bShutOff= TRUE;
				bClosing= TRUE;

				waveOutReset(hWaveOut);
			}
			else
				EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}