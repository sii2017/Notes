#include <windows.h>
#include <commdlg.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsstance, PSTR szCmdLine, int iCmdShow)
{
	static CHOOSECOLOR cc;
	static COLORREF crCustColors[16];
	cc.lStructSize= sizeof(CHOOSECOLOR);
	cc.hwndOwner= NULL;
	cc.hInstance= NULL;
	cc.rgbResult= RGB(0x80, 0x80, 0x80);
	cc.lpCustColors= crCustColors;
	cc.Flags= CC_RGBINIT| CC_FULLOPEN;
	cc.lCustData= 0;
	cc.lpfnHook= NULL;
	cc.lpTemplateName= NULL;

	return ChooseColor(&cc);
}


/*
只用一个函数调用出来色彩板
*/