
#include "MutexProcess.h"
#include <windows.h>

namespace Oasgames{

#define SD_INHERITHANDLE	0x00000001
#define SD_NORMALACCESS		0x00000002

	BOOL CreateSecurityAttributes(PSECURITY_ATTRIBUTES psa, DWORD dwFlag)
	{
		if (psa == NULL)
			return FALSE;

		psa->lpSecurityDescriptor = NULL;
		PSECURITY_DESCRIPTOR pSD = NULL;
		if (dwFlag & SD_NORMALACCESS)
		{
			pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
			if (pSD == NULL)
				goto ENDPOS;

			if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
				goto ENDPOS;

			if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE))
				goto ENDPOS;

			psa->lpSecurityDescriptor = pSD;
		}

		psa->bInheritHandle = (dwFlag & SD_INHERITHANDLE) ? TRUE : FALSE;
		psa->nLength = sizeof(SECURITY_ATTRIBUTES);

		return TRUE;

	ENDPOS:
		if (pSD != NULL)
			LocalFree(pSD);
		psa->lpSecurityDescriptor = NULL;

		return FALSE;
	}

	BOOL CleanSecurityAttributes(PSECURITY_ATTRIBUTES psa)
	{
		if (psa != NULL && psa->lpSecurityDescriptor != NULL)
		{
			LocalFree(psa->lpSecurityDescriptor);
			psa->lpSecurityDescriptor = NULL;
		}

		return TRUE;
	}

	HANDLE WINAPI MyCreateEvent(DWORD dwFlag, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName)
	{
		HANDLE hHandle = NULL;

		SECURITY_ATTRIBUTES sa;
		if (CreateSecurityAttributes(&sa, dwFlag))
		{
			hHandle = CreateEvent(&sa, bManualReset, bInitialState, lpName);

			CleanSecurityAttributes(&sa);
		}

		return hHandle;
	}

	HANDLE ProcessIsExist(const std::wstring& sName)
	{
		HANDLE hEvent = MyCreateEvent(SD_NORMALACCESS, FALSE, FALSE, sName.c_str());
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hEvent);

			return NULL;
		}

		return hEvent;
	}

	MutexProcess::MutexProcess(const std::wstring& sProcessName, const std::wstring& sGuid)
	{
		m_sName			= sProcessName;
		m_sGuid			= sGuid;
	}

	int MutexProcess::init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpstrCmdLine, int nCmdShow)
	{
		//L"Global\\TRAY{E9BE99D8-D697-435A-8EC1-4B7CB71A1A8C}"
		wchar_t buff[256] = { 0 };
		swprintf_s(buff, L"%s_%s", m_sName.c_str(), m_sGuid.c_str());
		
		HANDLE hHandle = ProcessIsExist(buff);
		if (hHandle == NULL)
		{
			existProcess();
			return 0;
		}

		int iRet = run(hInstance, hPrevInstance, lpstrCmdLine, nCmdShow);

		exitProcessClean();
		CloseHandle(hHandle);

		return iRet;
	}
}