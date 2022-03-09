
#include "JTerseRegOpt.h"
#include <assert.h>

namespace JTerse{namespace core{

	JTerseRegOpt::JTerseRegOpt(bool bIs64OS,HKEY hKey,bool bIs64Key)
	{
		dwDesired = 0;

		if (!bIs64Key)
		{
			if (bIs64OS)
				dwDesired |= KEY_WOW64_32KEY;
		}else
		{
			if (bIs64OS)
				dwDesired |= KEY_WOW64_64KEY;
		}

		m_hKey=hKey;
	}

	JTerseRegOpt::~JTerseRegOpt()
	{
		if(m_hKey)
		{
			RegCloseKey(m_hKey);
			m_hKey=NULL;
		}
	}

	bool JTerseRegOpt::open(const std::wstring& subKey)
	{
		assert(m_hKey);
		assert(subKey.c_str());

		HKEY hKey;
		LSTATUS ls = RegOpenKeyEx(m_hKey,subKey.c_str(),0L,KEY_ALL_ACCESS|dwDesired,&hKey);
		if (ls == ERROR_SUCCESS)
		{
			m_hKey = hKey;
			return true;
		}

		return false;
	}

	bool JTerseRegOpt::open(REGSAM samDesired,const std::wstring& subKey)
	{
		assert(m_hKey);
		assert(subKey.c_str());

		HKEY hKey;
		LSTATUS ls = RegOpenKeyEx(m_hKey,subKey.c_str(),0L,samDesired|dwDesired,&hKey);
		if (ls == ERROR_SUCCESS)
		{
			m_hKey = hKey;
			return true;
		}

		return false;
	}

	bool JTerseRegOpt::createKey(const std::wstring& sSubKey)
	{
		assert(m_hKey);
		assert(sSubKey.c_str());

		HKEY hKey;
		DWORD dw;
		LSTATUS ls=RegCreateKeyEx(m_hKey,sSubKey.c_str(),0L,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS|dwDesired,NULL,&hKey,&dw);

		if(ls == ERROR_SUCCESS)
		{
			m_hKey=hKey;
			return true;
		}

		return false;
	}

	bool JTerseRegOpt::read(const std::wstring& sSubKey,std::wstring& sRet)
	{
		assert(m_hKey);
		assert(sSubKey.c_str());
		assert(sRet.c_str());

		DWORD dwType;
		DWORD dwSize=512;
		wchar_t buff[512]={0};

		LSTATUS ls=RegQueryValueEx(m_hKey,sSubKey.c_str(),NULL,&dwType,(BYTE *)buff,&dwSize);

		if(ls == ERROR_SUCCESS)
		{
			sRet = buff;
			return true;
		}

		return false;
	}

	bool JTerseRegOpt::read(const std::wstring& sSubKey,DWORD& dwRet)
	{
		assert(m_hKey);
		assert(sSubKey.c_str());

		DWORD dwType;
		DWORD dwSize=sizeof(DWORD);
		DWORD dwDest;

		LSTATUS ls=RegQueryValueEx(m_hKey,sSubKey.c_str(),NULL,&dwType,(BYTE *)&dwDest,&dwSize);

		if(ls == ERROR_SUCCESS)
		{
			dwRet=dwDest;
			return true;
		}

		return false;
	}

	bool JTerseRegOpt::deleteKey(HKEY hKey,const std::wstring& sSubKey)
	{
		assert(hKey);
		assert(sSubKey.c_str());

		LSTATUS ls=RegDeleteKey(hKey,sSubKey.c_str());

		if(ls == ERROR_SUCCESS)
			return true;

		return false;
	}

	bool JTerseRegOpt::deleteValue(const std::wstring& sValue)
	{
		assert(m_hKey);
		assert(sValue.c_str());

		LSTATUS ls=RegDeleteValue(m_hKey,sValue.c_str());

		if(ls == ERROR_SUCCESS)
			return true;

		return false;
	}


	bool JTerseRegOpt::write(const std::wstring& sSubKey,std::wstring& sValue)
	{
		assert(m_hKey);
		assert(sSubKey.c_str());

		DWORD dwcbData = (sValue.length()+1)*sizeof(wchar_t);

		LSTATUS ls=RegSetValueEx(m_hKey,sSubKey.c_str(),0L,REG_SZ,(const BYTE *)sValue.c_str(),dwcbData);

		if(ls == ERROR_SUCCESS)
			return true;

		return false;
	}

	bool JTerseRegOpt::write(const std::wstring& sSubKey,DWORD dwRet)
	{
		assert(m_hKey);
		assert(sSubKey.c_str());

		LSTATUS ls=RegSetValueEx(m_hKey,sSubKey.c_str(),0L,REG_DWORD,(const BYTE *) &dwRet,sizeof(DWORD));

		if(ls == ERROR_SUCCESS)
			return true;

		return false;
	}

	bool fastReadRegValue(HKEY hKey,const std::wstring& sSubKey,const std::wstring& sKey,std::wstring& sRet,bool bIs64OS,bool bIs64Key)
	{
		JTerseRegOpt opt(bIs64OS,hKey,bIs64Key);
		if(!opt.open(KEY_READ,sSubKey))
		{
			return false;
		}

		return opt.read(sKey,sRet);
	}

	bool fastReadRegValue(HKEY hKey,const std::wstring& sSubKey,const std::wstring& sKey,DWORD& dwRet,bool bIs64OS,bool bIs64Key)
	{
		JTerseRegOpt opt(bIs64OS,hKey,bIs64Key);
		if(!opt.open(KEY_READ,sSubKey))
		{
			return false;
		}

		return opt.read(sKey,dwRet);
	}

	bool fastWriteRegValue(HKEY hKey,const std::wstring& sSubKey,const std::wstring& sKey,std::wstring& sRet,bool bIs64OS,bool bIs64Key)
	{
		JTerseRegOpt opt(bIs64OS,hKey,bIs64Key);
		if(!opt.open(sSubKey))
		{
			bool b = opt.createKey(sSubKey);
			if (!b)
				return false;
		}

		return opt.write(sKey,sRet);
	}

	bool fastWriteRegValue(HKEY hKey,const std::wstring& sSubKey,const std::wstring& sKey,DWORD dwRet,bool bIs64OS,bool bIs64Key)
	{
		JTerseRegOpt opt(bIs64OS,hKey,bIs64Key);
		if(!opt.open(sSubKey))
		{
			bool b = opt.createKey(sSubKey);
			if (!b)
				return false;
		}

		return opt.write(sKey,dwRet);
	}

	bool fastDeleteRegKey(HKEY hKey,const std::wstring& sSubKey,bool bIs64OS,bool bIs64Key)
	{
		JTerseRegOpt opt(bIs64OS,HKEY_LOCAL_MACHINE,bIs64Key);
		return opt.deleteKey(hKey,sSubKey);
	}

	bool fastDeleteRegValue(HKEY hKey,const std::wstring& sSubKey,const std::wstring& sKey,bool bIs64OS,bool bIs64Key)
	{
		JTerseRegOpt opt(bIs64OS,HKEY_LOCAL_MACHINE,bIs64Key);
		if(!opt.open(sSubKey))
		{
			return false;
		}

		return opt.deleteValue(sKey);
	}

	bool fastCreateRegKey(HKEY hKey,const std::wstring& sSubKey,bool bIs64OS,bool bIs64Key)
	{
		JTerseRegOpt opt(bIs64OS,HKEY_LOCAL_MACHINE,bIs64Key);
		return opt.createKey(sSubKey);
	}

}}