
#include "UGDBOpt.h"
#include <shlwapi.h>

namespace Oasgames{

	void UGDBOpt::setTableName(const std::wstring& sName)
	{
		TCHAR szPath[MAX_PATH];
		GetModuleFileName(NULL, szPath, MAX_PATH);
		PathRemoveFileSpec(szPath);
		PathAppend(szPath, _T("data"));
		PathAppend(szPath, sName.c_str());
		m_sDataPath = szPath;
	}


	void UGDBOpt::setModuleName(const std::wstring& sName)
	{
		TCHAR szPath[MAX_PATH];
		GetModuleFileName(NULL, szPath, MAX_PATH);
		PathRemoveFileSpec(szPath);
		PathAppend(szPath, sName.c_str());
		m_sDataPath = szPath;
	}


	void UGDBOpt::writeInt(const std::wstring& sKey, const std::wstring& sSubKey, unsigned int uiValue)
	{	
		wchar_t buff[256] = { 0 };
		swprintf_s(buff, L"%d", uiValue);
		WritePrivateProfileString(sKey.c_str(), sSubKey.c_str(), buff, m_sDataPath.c_str());
	}

	void UGDBOpt::writeInt64(const std::wstring& sKey, const std::wstring& sSubKey, unsigned long long ullValue)
	{
		wchar_t buff[256] = { 0 };
		swprintf_s(buff, L"%I64u", ullValue);
		WritePrivateProfileString(sKey.c_str(), sSubKey.c_str(), buff, m_sDataPath.c_str());
	}

	void UGDBOpt::writeBool(const std::wstring& sKey, const std::wstring& sSubKey, bool bValue)
	{
		wchar_t buff[256] = { 0 };
		swprintf_s(buff, L"%d", bValue);
		WritePrivateProfileString(sKey.c_str(), sSubKey.c_str(), buff, m_sDataPath.c_str());
	}

	void UGDBOpt::writeString(const std::wstring& sKey, const std::wstring& sSubKey, const std::wstring& sValue)
	{
		WritePrivateProfileString(sKey.c_str(), sSubKey.c_str(), sValue.c_str(), m_sDataPath.c_str());
	}

	bool UGDBOpt::readInt(const std::wstring& sKey, const std::wstring& sSubKey, unsigned int& retValue, const int nDef)
	{
		retValue = GetPrivateProfileInt(sKey.c_str(), sSubKey.c_str(), nDef, m_sDataPath.c_str());
		return true;
	}

	bool UGDBOpt::readInt64(const std::wstring& sKey, const std::wstring& sSubKey, unsigned long long& retValue)
	{
		TCHAR strtemp[256] = { 0 };
		GetPrivateProfileString(sKey.c_str(), sSubKey.c_str(), _T(""), strtemp, 256, m_sDataPath.c_str());
		retValue = _ttoi64(strtemp);

		return true;
	}

	bool UGDBOpt::readBool(const std::wstring& sKey, const std::wstring& sSubKey, bool bDef)
	{
		int nValue = GetPrivateProfileInt(sKey.c_str(), sSubKey.c_str(), bDef ? 1 : 0, m_sDataPath.c_str());
		return nValue ==1;
	}

	bool UGDBOpt::readString(const std::wstring& sKey, const std::wstring& sSubKey, std::wstring& retValue)
	{
		TCHAR strtemp[256] = { 0 };
		GetPrivateProfileString(sKey.c_str(), sSubKey.c_str(), _T(""), strtemp, 256, m_sDataPath.c_str());
		retValue = strtemp;

		return true;
	}

}