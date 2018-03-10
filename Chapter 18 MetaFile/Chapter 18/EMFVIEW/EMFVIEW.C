/*
使用到了上几章中的调色盘
*/
#include <windows.h>
#include <commdlg.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("EmfView");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	//文件名为全局，并且有快捷键和menu
	HACCEL hAccel;
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= szAppName;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Regiseter Failed"),szAppName, MB_ICONERROR) ;
		return 0;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Enhanced MetaFile Viewer"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL) ;

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	hAccel= LoadAccelerators(hInstance, szAppName);

	while(GetMessage(&msg, NULL, 0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//从metafile中获取信息建立逻辑调色盘
HPALETTE CreatePaletteFromMetaFile(HENHMETAFILE hemf)
{
	HPALETTE hPalette;
	int iNum;
	LOGPALETTE* plp;

	if(!hemf)
		return NULL;
	if(0== (iNum= GetEnhMetaFilePaletteEntries(hemf, 0, NULL)))
		return NULL;
	plp= malloc(sizeof(LOGPALETTE)+(iNum-1)*sizeof(PALETTEENTRY));//LOGPALETTE中包含一个PALETTEENTR，所以申请的PALETTEENTRY中为iNum-1个
	plp->palVersion= 0x0300;//支持windows NT 3.0
	plp->palNumEntries= iNum;

	GetEnhMetaFilePaletteEntries(hemf, iNum, plp->palPalEntry);
	hPalette= CreatePalette(plp);
	free(plp);
	return hPalette;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static DOCINFO di= {sizeof(DOCINFO), TEXT("EmfView:Printing")};
	static HENHMETAFILE hemf;
	static OPENFILENAME ofn;
	static PRINTDLG printdlg= {sizeof(PRINTDLG)};
	static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];
	static TCHAR szFilter[]= TEXT("Enhanced MetaFiless(*.EMF)\0*.emf\0")
		TEXT("All Files(*.*)\0*.*\0\0");
	BOOL bSuccess;
	ENHMETAHEADER header;
	HDC hdc, hdcPrn;
	HENHMETAFILE hemfCopy;
	HMENU hMenu;
	HPALETTE hPalette;
	int i, iLength, iEnable;
	PAINTSTRUCT ps;
	RECT rect;
	PTSTR pBuffer;

	switch(message)
	{
	case WM_CREATE:
		//Initialize OPENFILENAME structure
		ofn.lStructSize = sizeof (OPENFILENAME) ;
		ofn.hwndOwner = hwnd ;
		ofn.hInstance = NULL ;
		ofn.lpstrFilter = szFilter ;
		ofn.lpstrCustomFilter = NULL ;
		ofn.nMaxCustFilter = 0 ;
		ofn.nFilterIndex = 0 ;
		ofn.lpstrFile = szFileName ;
		ofn.nMaxFile = MAX_PATH ;
		ofn.lpstrFileTitle = szTitleName ;
		ofn.nMaxFileTitle = MAX_PATH ;
		ofn.lpstrInitialDir = NULL ;
		ofn.lpstrTitle = NULL ;
		ofn.Flags = 0 ;
		ofn.nFileOffset = 0 ;
		ofn.nFileExtension = 0 ;
		ofn.lpstrDefExt = TEXT ("emf") ;
		ofn.lCustData = 0 ;
		ofn.lpfnHook = NULL ;
		ofn.lpTemplateName = NULL ;
		return 0 ;
	case WM_INITMENUPOPUP:
		hMenu= GetMenu(hwnd);
		iEnable = hemf ? MF_ENABLED : MF_GRAYED ;
		EnableMenuItem (hMenu, IDM_FILE_SAVE_AS, iEnable) ;
		EnableMenuItem (hMenu, IDM_FILE_PRINT, iEnable) ;
		EnableMenuItem (hMenu, IDM_FILE_PROPERTIES, iEnable) ;
		EnableMenuItem (hMenu, IDM_EDIT_CUT, iEnable) ;
		EnableMenuItem (hMenu, IDM_EDIT_COPY, iEnable) ;
		EnableMenuItem (hMenu, IDM_EDIT_DELETE, iEnable) ;
		EnableMenuItem (hMenu, IDM_EDIT_PASTE,
			IsClipboardFormatAvailable(CF_ENHMETAFILE) ? MF_ENABLED : MF_GRAYED) ;
		return 0 ;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_FILE_OPEN:	//打开文件窗口，与之前相同
			//Show the file Open dialog box
			ofn.Flags= 0;
			if(!GetOpenFileName(&ofn))
				return 0;
			//If there's an existing EMF, get rid of it
			if(hemf)
			{
				DeleteEnhMetaFile(hemf);
				hemf= NULL;
			}
			//Load the EMF into memory
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			hemf= GetEnhMetaFile(szFileName);
			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			//Invalidate the client area for later update
			InvalidateRect(hwnd, NULL, TRUE);
			if(hemf== NULL)
			{
				MessageBox(hwnd, TEXT("Cannot load MetaFile"), szAppName, 
					MB_ICONEXCLAMATION| MB_OK);
			}
			return 0;
		case IDM_FILE_SAVE_AS://保存文件
			if(!hemf)
				return 0;
			//Show the file save dialog box
			ofn.Flags= OFN_OVERWRITEPROMPT;
		    if(!GetSaveFileName(&ofn))
				return 0;
			//Save the EMF to disk file
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			hemfCopy= CopyEnhMetaFile(hemf, szFileName);
			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			if(hemfCopy)
			{
				DeleteEnhMetaFile(hemf);
				hemf= hemfCopy;
			}
			else
				MessageBox(hwnd, TEXT("Cannot save MetaFile"), szAppName, 
				MB_ICONEXCLAMATION| MB_OK);
			return 0;
		case IDM_FILE_PRINT://打印，与之前类似，StartDoc和EndDoc之间由之前的基础GDI函数换成了metafile专用的PlayEnhMetaFile
			//Show the print dialog box and get printer DC
			printdlg.Flags= PD_RETURNDC| PD_NOPAGENUMS| PD_NOSELECTION;
			if(!PrintDlg(&printdlg))
				return 0;
			if(NULL==(hdcPrn=printdlg.hDC))
			{
				MessageBox(hwnd, TEXT("Cannot obtain printer DC"), szAppName, 
					MB_ICONEXCLAMATION| MB_OK);
				return 0;
			}
			//Get size of printable area of page
			rect.left= 0;
			rect.right= GetDeviceCaps(hdcPrn, HORZRES);
			rect.top= 0;
			rect.bottom= GetDeviceCaps(hdcPrn, VERTRES);
			bSuccess= FALSE;
			//Play the EMF to the printer
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			if((StartDoc(hdcPrn, &di)>0) && (StartPage(hdcPrn)>0))
			{
				PlayEnhMetaFile(hdcPrn, hemf, &rect);
				if(EndPage(hdcPrn)>0)
				{
					bSuccess= TRUE;
					EndDoc(hdcPrn);
				}
			}
			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			DeleteDC(hdcPrn);

			if(!bSuccess)
				MessageBox(hwnd, TEXT("Could not print MetaFile"), szAppName, 
				MB_ICONEXCLAMATION| MB_OK);
			return 0;
		case IDM_FILE_PROPERTIES://获取信息并且写入缓冲pBuffer中再以message展示
			if(!hemf)
				return 0;
			iLength= GetEnhMetaFileDescription(hemf, 0, NULL);
			pBuffer= malloc((iLength+256)*sizeof(TCHAR));
			GetEnhMetaFileHeader(hemf, sizeof(ENHMETAHEADER), &header);
			//Format header file information
			i= wsprintf(pBuffer, TEXT("Bounds= (%i, %i)to(%i, %i)pixels\n"),
				header.rclBounds.left, header.rclBounds.top,
				header.rclBounds.right, header.rclBounds.bottom);
			i+=wsprintf(pBuffer+i, TEXT("Frame= (%i, %i)to(%i,%i)mms\n"),
				header.rclFrame.left, header.rclFrame.top,
				header.rclFrame.right, header.rclFrame.bottom);
			i+= wsprintf(pBuffer+i, TEXT ("Resolution = (%i, %i) pixels")
				TEXT (" = (%i, %i) mms\n"),
				header.szlDevice.cx, header.szlDevice.cy,
				header.szlMillimeters.cx,
				header.szlMillimeters.cy) ;
			i += wsprintf (pBuffer + i,
				TEXT ("Size = %i, Records = %i, ")
				TEXT ("Handles = %i, Palette entries = %i\n"),
				header.nBytes, header.nRecords,
				header.nHandles, header.nPalEntries) ;
			//Inclue the MetaFile description, if present
			if(iLength)
			{
				i += wsprintf (pBuffer + i, TEXT ("Description = ")) ;
				GetEnhMetaFileDescription (hemf, iLength, pBuffer + i) ;
				pBuffer [lstrlen (pBuffer)] = '\t' ;
			}
			MessageBox (hwnd, pBuffer, TEXT ("MetaFile Properties"), MB_OK) ;
			free (pBuffer) ;
			return 0 ;
		case IDM_EDIT_COPY:
		case IDM_EDIT_CUT:
			if(!hemf)
				return 0;
			//Transfer MetaFile copy to the clipboard
			hemfCopy= CopyEnhMetaFile(hemf, NULL);
			OpenClipboard(hwnd);//打开剪贴簿
			EmptyClipboard();	//清空剪贴簿
			SetClipboardData(CF_ENHMETAFILE, hemfCopy);//放入剪贴簿
			CloseClipboard();	//关闭剪贴簿

			if(LOWORD(wParam)== IDM_EDIT_COPY)	//如果是复制则结束，如果是剪贴则继续下面的命令删除原来的内容
				return 0;
			//fall through if IDM_EDIT_CUT
		case IDM_EDIT_DELETE:
			if(hemf)
			{
				DeleteEnhMetaFile(hemf);
				hemf= NULL;
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;
		case IDM_EDIT_PASTE://从剪贴簿里活动内容
			OpenClipboard(hwnd);
			hemfCopy= GetClipboardData(CF_ENHMETAFILE);
			CloseClipboard();
			if(hemfCopy&&hemf)
			{
				DeleteEnhMetaFile(hemf);
				hemf= NULL;
			}
			hemf= CopyEnhMetaFile(hemfCopy, NULL);	//将获取的内容放入hemf中，供WM_PAINT中绘制
			
			InvalidateRect(hwnd, NULL, TRUE);	//令区域无效
			return 0;
		case IDM_APP_ABOUT:
			MessageBox(hwnd, TEXT("Enhanced MetaFile Viewer\n")
				TEXT("(c)Charles Petzold, 1998"),szAppName, MB_OK);
			return 0;
		case IDM_APP_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0L);
			return 0;
		}
		break;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		if(hemf)
		{
			if(hPalette= CreatePaletteFromMetaFile(hemf))//从位图中获得调色板
			{
				SelectPalette(hdc, hPalette, FALSE);//更换调色板
			}
			GetClientRect(hwnd, &rect);
			PlayEnhMetaFile(hdc, hemf, &rect);

			if(hPalette)
				DeleteObject(hPalette);
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_QUERYNEWPALETTE://当顶层窗口或它的子窗口具有输入焦点时收到该消息
		if(!hemf|| !(hPalette= CreatePaletteFromMetaFile(hemf)))
			return FALSE;
		hdc= GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		InvalidateRect(hwnd, NULL, FALSE);

		DeleteObject(hPalette);
		ReleaseDC(hwnd, hdc);
		return TRUE;
	case WM_PALETTECHANGED://当调色板更改后收到的消息
		if((HWND)wParam== hwnd)
			break;
		if(!hemf||!(hPalette= CreatePaletteFromMetaFile(hemf)))
			break;
		hdc= GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		UpdateColors(hdc);

		DeleteObject(hPalette);
		ReleaseDC(hwnd, hdc);
		break;
	case WM_DESTROY:
		if(hemf)
			DeleteEnhMetaFile(hemf);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}