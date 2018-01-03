#include <windows.h>
#include <commdlg.h>
#include <tchar.h>
#define MAX_STRING_LEN 256
static TCHAR szFindText[MAX_STRING_LEN];
static TCHAR szReplText[MAX_STIRNG_LEN];

HWND PopFindFindDlg(HWND hwnd)