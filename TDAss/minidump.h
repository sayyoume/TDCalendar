#pragma once

#include <windows.h>
#include <DbgHelp.h>
#include <stdlib.h>
#include <string>
#include <Shlobj.h>
#include <math.h>
#include <time.h>
#include <Shlwapi.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "shlwapi.lib")

#ifndef _M_IX86
#error "The following code only works for x86!"
#endif

inline BOOL IsDataSectionNeeded(const WCHAR* pModuleName)
{
	if (pModuleName == 0)
	{
		return FALSE;
	}

	WCHAR szFileName[_MAX_FNAME] = L"";
	_wsplitpath_s(pModuleName, NULL, 0, NULL, 0, szFileName, _MAX_FNAME, NULL, 0);

	if (wcscmp(szFileName, L"ntdll") == 0)
		return TRUE;

	return FALSE;
}

inline BOOL CALLBACK MiniDumpCallback(PVOID                            pParam,
	const PMINIDUMP_CALLBACK_INPUT   pInput,
	PMINIDUMP_CALLBACK_OUTPUT        pOutput)
{
	if (pInput == 0 || pOutput == 0)
		return FALSE;

	switch (pInput->CallbackType)
	{
	case ModuleCallback:
		if (pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
			if (!IsDataSectionNeeded(pInput->Module.FullPath))
				pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
	case IncludeModuleCallback:
	case IncludeThreadCallback:
	case ThreadCallback:
	case ThreadExCallback:
		return TRUE;
	default:;
	}

	return FALSE;
}

inline void CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR strFileName)
{
	HANDLE hFile = CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pep;
		mdei.ClientPointers = NULL;

		MINIDUMP_CALLBACK_INFORMATION mci;
		mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE) MiniDumpCallback;
		mci.CallbackParam = 0;

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, (pep != 0) ? &mdei : 0, NULL, &mci);

		CloseHandle(hFile);
	}
}

std::wstring sDumapDir = L"";

LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	CreateMiniDump(pExceptionInfo, sDumapDir.c_str());
	return EXCEPTION_EXECUTE_HANDLER;
}

void DisableSetUnhandledExceptionFilter()
{
	void* addr = (void*) GetProcAddress(LoadLibrary(L"kernel32.dll"),"SetUnhandledExceptionFilter");

	if (addr)
	{
		unsigned char code[16];
		int size = 0;

		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC2;
		code[size++] = 0x04;
		code[size++] = 0x00;

		DWORD dwOldFlag, dwTempFlag;
		VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
	}
}

void InitMinDump()
{
	/*wchar_t temp[MAX_PATH] = { 0 };
	GetTempPath(MAX_PATH, temp);*/
	wchar_t buff[256] = { 0 };
	GetModuleFileName(NULL, buff, 256);
	std::wstring sFileName = buff;
	PathRemoveFileSpec(buff);
	int npos = sFileName.find_last_of(L"\\");
	if (npos != -1)
	{
		sFileName = sFileName.substr(npos + 1, sFileName.length());

		npos = sFileName.find_last_of(L".");
		if (npos != -1)
		{
			sFileName = sFileName.substr(0, npos);
		}
		time_t rawTime;
		time(&rawTime);

		struct tm today = { 0 };
		localtime_s(&today, &rawTime);

		wchar_t wTime[256] = { 0 };
		swprintf_s(wTime, L"%d%d%d%d%d", today.tm_year + 1900, today.tm_mon + 1, today.tm_mday, today.tm_hour, today.tm_min);
		sFileName.append(wTime);
		sFileName.append(L".dump");
	}
	sDumapDir = buff;
	sDumapDir = sDumapDir.append(L"\\dump\\");
	CreateDirectory(sDumapDir.c_str(), NULL);
	sDumapDir = sDumapDir.append(sFileName);

	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	//DisableSetUnhandledExceptionFilter();
}