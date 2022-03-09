
#ifndef __JTERSEREGOPT_H__
#define __JTERSEREGOPT_H__

#include <windows.h>
#include <string>
#include "..\Ass.h"

namespace JTerse{namespace core{

	class APDDLL_DECLSPEC  JTerseRegOpt
	{
	public:
		JTerseRegOpt(bool bIs64OS,HKEY hKey=HKEY_LOCAL_MACHINE,bool bIs64Key=false);
		virtual ~JTerseRegOpt();

		bool  read(const std::wstring& sKey, std::wstring& sRet);
		bool  read(const std::wstring& sKey, DWORD& dwRet);

		bool  write(const std::wstring& sKey, std::wstring& sValue);
		bool  write(const std::wstring& sKey, DWORD dwRet);
		bool open(const std::wstring& sKey);
		bool open(REGSAM samDesired,const std::wstring& sKey);
		bool createKey(const std::wstring& sSubKey);

		bool deleteKey(HKEY hKey,const std::wstring& sSubKey);
		bool deleteValue(const std::wstring& sValue);
	protected:
		HKEY m_hKey;
		DWORD dwDesired;
	};

	bool APDDLL_DECLSPEC fastCreateRegKey(HKEY hKey, const std::wstring& sSub, bool bIs64OS, bool bIs64Key = false);

	bool APDDLL_DECLSPEC fastReadRegValue(HKEY hKey, const std::wstring& sSubKey, const std::wstring& sKey, std::wstring& sRet, bool bIs64OS, bool bIs64Key = false);
	bool APDDLL_DECLSPEC fastReadRegValue(HKEY hKey, const std::wstring& sSubKey, const std::wstring& sKey, DWORD& dwRet, bool bIs64OS, bool bIs64Key = false);

	bool APDDLL_DECLSPEC fastWriteRegValue(HKEY hKey, const std::wstring& sSubKey, const std::wstring& sKey, std::wstring& sValue, bool bIs64OS, bool bIs64Key = false);
	bool APDDLL_DECLSPEC fastWriteRegValue(HKEY hKey, const std::wstring& sSubKey, const std::wstring& sKey, DWORD dwRet, bool bIs64OS, bool bIs64Key = false);

	bool APDDLL_DECLSPEC fastDeleteRegKey(HKEY hKey, const std::wstring& sSubKey, bool bIs64OS, bool bIs64Key = false);
	bool APDDLL_DECLSPEC fastDeleteRegValue(HKEY hKey, const std::wstring& sSubKey, const std::wstring& sKey, bool bIs64OS, bool bIs64Key = false);

}}

#endif