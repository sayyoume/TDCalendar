
#pragma once

#include "..\Ass.h"
#include <string>

namespace Oasgames{

	//opt config
	class APDDLL_DECLSPEC UGDBOpt
	{
	public:
		void setTableName(const std::wstring& sName);
		void setModuleName(const std::wstring& sName);
		void writeInt(const std::wstring& sKey, const std::wstring& sSubKey, unsigned int uiValue);
		void writeInt64(const std::wstring& sKey, const std::wstring& sSubKey, unsigned long long ullValue);
		void writeBool(const std::wstring& sKey, const std::wstring& sSubKey, bool bValue);
		void writeString(const std::wstring& sKey, const std::wstring& sSubKey, const std::wstring& sValue);
		bool readInt(const std::wstring& sKey, const std::wstring& sSubKey, unsigned int& retValue, const int nDef = 0);
		bool readInt64(const std::wstring& sKey, const std::wstring& sSubKey, unsigned long long& retValue);
		bool readBool(const std::wstring& sKey, const std::wstring& sSubKey, bool bDef = false);
		bool readString(const std::wstring& sKey, const std::wstring& sSubKey, std::wstring& retValue);

	private:
		std::wstring m_sDataPath; //ÅäÖÃÎÄ¼þiniÂ·¾¶
	};

}