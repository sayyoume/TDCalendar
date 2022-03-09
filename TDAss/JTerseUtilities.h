
#ifndef __JTERSEUTILITIES_H__
#define __JTERSEUTILITIES_H__

#include "Ass.h"

#include <string>
#include <windows.h>
#include <vector>
#include "JLHelper.h"

namespace JTerse{namespace core{

	#define MAX_FORMAT_SINGLEBUFFER	(1024*10)

	enum LinkType
	{
		LNKTYPE_DESKTOP,
		LNKTYPE_QUICKLAUNCH,
		LNKTYPE_STARTMENU,
		LNKTYPE_TASKBIN,
		LNKTYPE_ALLDESKTOP,
		LNKTYPE_COMMON_PROGRAMS,
	};

	const DWORD kProcessKilledExitCode = 1;

	void APDDLL_DECLSPEC FormatA(std::string& sbuffer, LPCSTR lpszFormat, ...);
	void APDDLL_DECLSPEC FormatW(std::wstring& swbuffer, LPCWSTR lpszFormat, ...);

	void APDDLL_DECLSPEC trim(std::string& str);
	void APDDLL_DECLSPEC trim(std::wstring& str);

	void APDDLL_DECLSPEC LTrim(std::string& str);
	void APDDLL_DECLSPEC LTrim(std::wstring& str);

	void APDDLL_DECLSPEC RTrim(std::string& str);
	void APDDLL_DECLSPEC RTrim(std::wstring& str);

	int APDDLL_DECLSPEC compareNoCase(const _tstring &strA, const _tstring &strB);

	bool APDDLL_DECLSPEC CheckNetworkAlive(std::wstring& sCheckUrl);
	bool APDDLL_DECLSPEC getFileSizeEx(std::wstring sFilePath, int* nSize);
	std::wstring APDDLL_DECLSPEC StringTrim(const std::wstring &source, const std::wstring &targets);
	std::string APDDLL_DECLSPEC ws2s(const std::wstring& wide, unsigned int CodePage = CP_UTF8);
	std::wstring APDDLL_DECLSPEC s2ws(const std::string& mb, unsigned int CodePage = CP_UTF8);
	bool APDDLL_DECLSPEC unZip(const std::wstring& sSrc, const std::wstring& sDstDir);
	bool APDDLL_DECLSPEC KillProcess(HANDLE process, int exit_code, bool wait = false);
	bool APDDLL_DECLSPEC killProcessByName(const std::wstring& sname, bool wait = false);
	bool APDDLL_DECLSPEC killProcessByNameEx(const std::wstring& sname, bool wait = false);
	bool APDDLL_DECLSPEC killProcessByPath(const std::wstring& spath, bool wait = false);
	bool APDDLL_DECLSPEC KillProcessById(DWORD process_id, int exit_code, bool wait = false);
	bool APDDLL_DECLSPEC LaunchProcess(const std::wstring& sFilePath, const std::wstring& sCmdline, bool wait);
	void APDDLL_DECLSPEC waitExecute(const std::wstring& sFilePath, const std::wstring& sParam, DWORD dwTime, bool bTimeoutKill = false);
	void APDDLL_DECLSPEC Execute(const std::wstring& sFilePath, const std::wstring& sParam, bool bWait = false, const std::wstring& sdir = L"");
	bool APDDLL_DECLSPEC GetFileVersion(const std::wstring& sPath, std::wstring& sVersion);
	void APDDLL_DECLSPEC GetProductVersion(std::wstring& sVersion);
	std::wstring APDDLL_DECLSPEC GetFileDescription(LPCTSTR strFilePath);
	bool APDDLL_DECLSPEC deleteDirectory(std::wstring srcDir);
	void APDDLL_DECLSPEC copyDir(const std::wstring& wsDest, const std::wstring& wsSour);
	bool APDDLL_DECLSPEC parseSeparator(const std::wstring& sSrc, wchar_t sTag, std::vector<std::wstring>& ret);
	bool APDDLL_DECLSPEC parseDNS(const std::wstring& sAddr, std::vector<std::wstring>& retVec);
	bool APDDLL_DECLSPEC ReadKeyValueW(HKEY hKey, LPCWSTR lpSubKey, LPCWSTR lpValueName, DWORD dwType, LPBYTE lpValue, DWORD& dwLength, DWORD dwDesired);
	bool APDDLL_DECLSPEC ReadKeyValue64(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, __int64 *lpValue);
	bool APDDLL_DECLSPEC WriteKeyValueW(HKEY hKey, LPCWSTR lpSubKey, LPCWSTR lpValueName, DWORD dwType, const BYTE* lpValue, DWORD& dwLength, DWORD dwDesired);

	bool APDDLL_DECLSPEC getSignature2(const std::wstring& sExePath, std::wstring& retSignature);
	bool APDDLL_DECLSPEC bVerifyEmbeddedSignature(const std::wstring& sExePath, std::wstring& retSignature);
	bool APDDLL_DECLSPEC GetFileInfo(const std::wstring& szFilename, std::wstring& companyName/*, std::wstring& fileDescription, std::wstring& fileVersion, std::wstring& productVersion*/);

	std::wstring APDDLL_DECLSPEC getBrowserPath();
	std::wstring APDDLL_DECLSPEC getFolder(LinkType type);
	void APDDLL_DECLSPEC createLinkIcon(const std::wstring& sDstFile, const std::wstring& sSavePath, const std::wstring& sParam, const std::wstring& sIcon, const std::wstring& sDec);
	HRESULT APDDLL_DECLSPEC createLinkIconEx(const std::wstring& sDstFile, const std::wstring& sSavePath, const std::wstring& sParam, const std::wstring& sIcon, const std::wstring& sDec, int nIndex = 0);

	DWORD APDDLL_DECLSPEC StringToNum(wchar_t *sz);


#define MCE_INHERITHANDLE		0x00000001		// ¼Ì³Ð
#define MCE_NORMALACCESS		0x00000002		// ¿ª·Å·ÃÎÊ

	APDDLL_DECLSPEC HANDLE WINAPI  MyCreateEvent(DWORD dwFlag, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName);
	APDDLL_DECLSPEC HANDLE WINAPI  MyCreateMutex(DWORD dwFlag, BOOL bInitialOwner, LPCTSTR lpName);
}}

#endif