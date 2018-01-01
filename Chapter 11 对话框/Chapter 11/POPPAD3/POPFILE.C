#include <windows.h>
#include <commdlg.h>

static OPENFILENAME ofn;
void PopFileInitialize(HWND hwnd)
{
	static TCHAR szFilter[]= 