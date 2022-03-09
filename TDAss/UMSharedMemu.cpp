#include "UMSharedMemu.h"
#include <windows.h>
#include <winsvc.h>
#include <string>
#include "Reg\JTerseRegOpt.h"
#include "JTerseUtilities.h"
#include "fileutils.h"

#pragma warning(disable:4996)

namespace UM
{

	UMSharedMemu::UMSharedMemu()
	{
	}

	UMSharedMemu* UMSharedMemu::shared()
	{
		static UMSharedMemu ug;
		return &ug;
	}

	bool UMSharedMemu::isNeedReboot()
	{
		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;
		std::wstring sServiceName{ L"memudrv" };
		unsigned long ulRunSate = 0;
		bool bReboot = false;

		hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (hSCM != NULL)
		{
			hService = OpenService(hSCM, sServiceName.c_str(), SC_MANAGER_ALL_ACCESS);
			if (hService)
			{
				SERVICE_STATUS svState;
				memset(&svState, 0, sizeof(SERVICE_STATUS));
				BOOL bState = QueryServiceStatus(hService, &svState);
				if (bState)
				{
					ulRunSate = svState.dwCurrentState;
				}
			}
		}
		
		if (ulRunSate == 3)
		{
			bReboot = true;
		}

		return bReboot;
	}

	void UMSharedMemu::rebootSystem()
	{
		HANDLE   hToken;
		TOKEN_PRIVILEGES   tkp;

		//获取系统信息
		OSVERSIONINFO   osvi;
		memset(&osvi, 0, sizeof(osvi));

		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (GetVersionEx(&osvi) == 0)
			return;

		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return;

		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;     //   one   privilege   to   set 
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
		ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, 0);
	}

	void UMSharedMemu::setAutoRun(std::wstring& sKeyName)
	{
		wchar_t sFile[MAX_PATH] = L"";
		GetModuleFileName(NULL, sFile, MAX_PATH);
		std::wstring sValue{ sFile };

		HKEY   hKey;
		std::wstring sKeyItem = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	
		long lRet;
		lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sKeyItem.c_str(), 0, KEY_WRITE, &hKey);
		if (lRet != ERROR_SUCCESS)
		{
			//注册表打开失败
			return ;
		}

		bool bRetValue = false;
		lRet = RegSetValueEx(hKey, sKeyName.c_str(), 0, REG_SZ, (LPBYTE)sValue.c_str(), sValue.length()*sizeof(wchar_t));
		if (ERROR_SUCCESS == lRet)
		{
			bRetValue = true;
		}

		if (hKey)
		{
			RegCloseKey(hKey);
			hKey = nullptr;
		}
		return ;

	}	

	void UMSharedMemu::clearAutoRun(std::wstring& sKeyName)
	{
		long lRet;
		HKEY   hKey;
		std::wstring sKeyItem = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

		lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sKeyItem.c_str(), 0, KEY_WRITE, &hKey);
		if (lRet != ERROR_SUCCESS)
		{
			//注册表打开失败
			return ;
		}

		bool bRetValue = false;

		lRet = RegDeleteValue(hKey, sKeyName.c_str());
		if (lRet == ERROR_SUCCESS)
		{
			bRetValue = true;
		}

		RegCloseKey(hKey);
		hKey = NULL;

		return;

	}

	bool UMSharedMemu::uninstall()
	{
		//卸载
		std::wstring sMenuOldPath = L"";
		std::wstring sInstallMemu{ L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"};
		JTerse::core::fastReadRegValue(HKEY_LOCAL_MACHINE, sInstallMemu, L"MEmu_Path", sMenuOldPath, false, false);

		std::wstring sMenuPath = sMenuOldPath;

		int nPos = sMenuPath.find_last_of(L"\\");
		if (nPos != -1)
		{
			sMenuPath = sMenuPath.substr(0, nPos);
		}
		sMenuPath = sMenuPath + L"\\Microvirt\\MEmu\\Uninstall";
		std::wstring sUninstallMenu1 = sMenuPath + L"\\uninstall.exe";

		std::wstring sMenuPathCmd = L" /U:\"" + sMenuPath + L"\\uninstall.xml" + L"\" /S";

		bool bState = file::pathFileExists(sUninstallMenu1);
		if (bState)
		{
			JTerse::core::LaunchProcess(sUninstallMenu1, sMenuPathCmd, true);
		}
        
		return bState;
	}


}