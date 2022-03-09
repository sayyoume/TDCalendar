
#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include "MainDlg.h"
#include "TDCalendar.h"
#include <algorithm>
#include <shlwapi.h>
#include "resource.h"
#include <time.h>
#include "TdHelp.h"
#include <shlobj.h>



bool  FreeResFile(HINSTANCE hInstance, DWORD dwResName, LPCWSTR lpResType, LPCWSTR lpFilePathName)
{
	HRSRC hResID = ::FindResource(hInstance, MAKEINTRESOURCE(dwResName), lpResType);
	HGLOBAL hRes = ::LoadResource(hInstance, hResID);
	LPVOID pRes = ::LockResource(hRes);
	if (pRes == NULL)
	{
		return FALSE;
	}
	DWORD dwResSize = ::SizeofResource(hInstance, hResID);
	HANDLE hResFile = CreateFile(lpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hResFile)
	{
		return FALSE;
	}
	DWORD dwWritten = 0;
	WriteFile(hResFile, pRes, dwResSize, &dwWritten, NULL);
	CloseHandle(hResFile);
	UnlockResource(pRes);
	FreeResource(hRes);
	return (dwResSize == dwWritten);
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{	
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;

	ULONG_PTR pGDIToken = NULL;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&pGDIToken, &gdiplusStartupInput, NULL);


	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	DuiLib::CPaintManagerUI::SetResourcePath(DuiLib::CPaintManagerUI::GetInstancePath() + _T("skin"));

	MainDlg* pFrame = new MainDlg();
	if (pFrame == NULL) return 0;
	
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHieght = GetSystemMetrics(SM_CYSCREEN);
	
	pFrame->Create(NULL, _T(""), UI_WNDSTYLE_FRAME , WS_EX_TOOLWINDOW | WS_EX_TOPMOST, nScreenWidth - 360, nScreenHieght - 680, 400, 640);
	pFrame->SetIcon(IDI_ICON1);

	DuiLib::CPaintManagerUI::MessageLoop();
	::CoUninitialize();
	GdiplusShutdown(pGDIToken);
	return 0;
}