
#include <windows.h>
#include "fileutils.h"
#include <Shlwapi.h>
#include <tchar.h>
#include <Shlobj.h>

#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Shell32.lib")


namespace file
{
	bool pathFileExists(const wchar_t* fileName)
	{
		return (GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES);
	}

	bool pathFileExists(const std::wstring& fileName)
	{
		return pathFileExists(fileName.c_str());
	}

	std::wstring getProgramRunDir()
	{   
		wchar_t exeFullPath[MAX_PATH];
		std::wstring strPath = L"";

		GetModuleFileName(NULL,exeFullPath,MAX_PATH);
		strPath=(std::wstring)exeFullPath;
		int pos = strPath.find_last_of('\\', strPath.length());
		return strPath.substr(0, pos+1);
	}

	void removeUninstall(const std::wstring& keyName)
	{
		if (keyName.empty())
		{
			return;
		}
		HKEY hkey = NULL;
		int iRet=0;

		iRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",0,KEY_ALL_ACCESS,&hkey);

		if (iRet == ERROR_SUCCESS)
		{
			SHDeleteKey(hkey,keyName.c_str());
			RegCloseKey(hkey);
		}
	}

	bool writeUninstall(const std::wstring& sDisplayName, const std::wstring& sPath, const std::wstring& sParam, const std::wstring& sKeyName, const std::wstring& sVersion, const std::wstring& sMainKey)
	{
		HKEY hKey; 
		hKey = NULL;

		std::wstring srsubkey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
		if (sMainKey.empty())
			srsubkey += sDisplayName;
		else
			srsubkey += sMainKey;

		HRESULT lRet = RegCreateKeyEx( HKEY_LOCAL_MACHINE, srsubkey.c_str(),0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

		if(lRet != ERROR_SUCCESS)  
			return false;

		lRet = RegSetValueEx(hKey, L"DisplayName", 0L, REG_SZ,(CONST BYTE*) sDisplayName.c_str(), sDisplayName.length()*2 + 1);
		if(lRet!=ERROR_SUCCESS) 
		{
			RegCloseKey(hKey); 
			return false; 
		}

		std::wstring szuninstall = sPath + L" " + sParam;
		lRet = RegSetValueEx(hKey, L"UninstallString", 0L, REG_SZ,(CONST BYTE*) szuninstall.c_str(), szuninstall.length()*2 + 1);
		
		if(lRet!=ERROR_SUCCESS) 
		{
			RegCloseKey(hKey); 
			return false; 
		}

		lRet = RegSetValueEx(hKey, L"DisplayIcon", 0L, REG_SZ,(CONST BYTE*) sPath.c_str(), sPath.length()*2 + 1);
		if(lRet!=ERROR_SUCCESS) 
		{
			RegCloseKey(hKey); 
			return false; 
		}

		lRet = RegSetValueEx(hKey, L"Publisher", 0L, REG_SZ,(CONST BYTE*) sKeyName.c_str(), sKeyName.length()*2 + 1);
		if(lRet!=ERROR_SUCCESS) 
		{
			RegCloseKey(hKey); 
			return false; 
		}

		lRet = RegSetValueEx(hKey, L"DisplayVersion", 0L, REG_SZ, (CONST BYTE*)sVersion.c_str(), sVersion.length() * 2 + 1);
		if (lRet != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return false;
		}

		RegCloseKey(hKey); 
		return true;
	}

	void delSelf(std::wstring sDir)
	{
		//采用批处理
		SHELLEXECUTEINFO ExeInfo;
		TCHAR     ExePath[MAX_PATH] = {0};
		TCHAR     ParamPath[MAX_PATH] = {0};
		TCHAR     ComposePath[MAX_PATH] = {0};

		GetModuleFileName(NULL,ExePath,MAX_PATH);
		{
			sDir=(std::wstring)ExePath;
			int pos = sDir.find_last_of('\\', sDir.length());
			sDir=  sDir.substr(0, pos);
		}

		GetShortPathName(ExePath,ExePath,MAX_PATH);

		GetEnvironmentVariable(_T("COMSPEC"),ComposePath,MAX_PATH);

		_tcscpy_s(ParamPath,_T("/c del "));
		_tcscat_s(ParamPath,ExePath);
		_tcscat_s(ParamPath,_T(" > nul"));

		ZeroMemory(&ExeInfo,sizeof(ExeInfo));
		ExeInfo.cbSize = sizeof(ExeInfo);
		ExeInfo.hwnd = 0;  
		ExeInfo.lpVerb = _T("Open");    
		ExeInfo.lpFile = ComposePath;    
		ExeInfo.lpParameters = ParamPath; 
		ExeInfo.nShow = SW_HIDE;     
		ExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS; 

		if (ShellExecuteEx(&ExeInfo))
		{
			SetPriorityClass(ExeInfo.hProcess,IDLE_PRIORITY_CLASS);
			//设置本程序进程基本为实时执行，快速退出。
			SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
			//通知资源管理器，本程序删除
			SHChangeNotify(SHCNE_DELETE,SHCNF_PATH,ExePath,NULL);
		}
	}
}
