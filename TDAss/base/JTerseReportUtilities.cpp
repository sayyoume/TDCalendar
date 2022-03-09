/************************************************************************/
/* 
    Date : 2/25/2013
	File : EREPORTUTILITIES.CPP
    Author : Eadwin Li
    Function :  
	Version : 1.0
	Copyright (c) 2012 - 2013 iWinPhone Studio
*/
/************************************************************************/  

#include <windows.h>
#include "JTerseReportUtilities.h"
#include "../safe/MD5.h"
#include "../JTerseUtilities.h"
#include <regex>

const std::wstring cskey = L"b8d375aff4439246bc04eedb9a0f6db9";

namespace JTerse{ namespace core{ namespace ReportLog{

	static tagLangAreaItem downloader_lang[] = 
	{   
		{LA_PT_BR,  			L"br",      L"pt"},
		{LA_TR_TR,				L"tr",      L"tr"},
		{LA_PL_PL,				L"pl",      L"pl"},
		{LA_EN_US,				L"us",		L"en"},
		{LA_VI_VN,				L"vn",      L"vi"},
		{LA_TH_TH,  		    L"th",      L"th"},
		{LA_ES_MX,				L"mx",      L"es"},
		{LA_ES_ES,				L"es",      L"es"},
		{LA_ES_AR,  			L"ar",      L"es"},
		{LA_ZH_CN,				L"cn",      L"zh"},
		{LA_SV_SE,				L"sw",      L"sv"},
		{LA_KO_KR,				L"kr",      L"ko"},
		{LA_AR_AE,				L"ae",      L"ar"},
		{LA_JA_JP,				L"jp",      L"ja"},
		{LA_NL_NL,				L"nl",      L"nl"},
		{LA_HI_IN,				L"in",      L"hi"},
		{LA_HU_HU,				L"hu",      L"hu"},
		{LA_ZH_TW,  		    L"tw",      L"zh"},
		{LA_DE_DE,				L"de",      L"de"},
		{LA_DE_AT,				L"at",      L"de"},
		{LA_EN_AU,				L"au",      L"en"},
		{LA_IT_IT,				L"it",      L"it"},
		{LA_AR_EG,				L"eg",      L"ar"},
		{LA_RO_RO,				L"ro",      L"ro"},
		{LA_AR_SA,				L"sa",      L"ar"},
		{LA_DA_DK,				L"dk",      L"da"},
		{LA_AR_MA,				L"ma",      L"ar"},
		{LA_EN_MY,				L"my",      L"en"},
		{LA_MM_MM,				L"mm",      L"mm"}
	};

	void ReportUtilities::GetNationLanguage(std::wstring& strOut, int iIndex)
	{
		int lcid = ::GetSystemDefaultLCID();

		for(int i = 0; i < countof(downloader_lang); i++)
		{
			if ((int)(downloader_lang[i].langID) == lcid)
			{
				switch (iIndex)
				{
				case 1:
					strOut = downloader_lang[i].langNationName;
                    return;
					break;
				case 2:
					strOut = downloader_lang[i].langLanName;
                    return;
					break;
				}
			}
		}
	}


	DWORD ReportUtilities::GetLCIDbyNation(std::wstring& strOut)
	{


		for(int i = 0; i < countof(downloader_lang); i++)
		{
			if (0 == wcscmp(downloader_lang[i].langNationName,strOut.data()))
			{
				return downloader_lang[i].langID;
			}
		}

		return 0;
	}



	std::wstring GetCheckSum(const std::wstring& oem,const std::wstring& uid,
		const std::wstring& pid,const std::wstring& version)
	{
		std::wstring sc;
		sc = oem;
		sc +=L"_"+cskey;
		sc +=L"_"+uid;
		sc +=L"_"+pid;
		sc +=L"_"+version;

		std::string checksum = ws2s(sc);

		wchar_t smd5[1024] = {0};

		::safe::MD5::md5(checksum.data(),smd5);

		sc = smd5;

		return sc;
	}


	BOOL GetNtVersionNumbers(DWORD&dwMajorVer, DWORD& dwMinorVer, DWORD& dwBuildNumber)
	{
		BOOL bRet = FALSE;
		HMODULE hModNtdll = NULL;
		if (hModNtdll = ::LoadLibraryW(L"ntdll.dll"))
		{
			typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
			pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
			pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
			if (pfRtlGetNtVersionNumbers)
			{
				pfRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer, &dwBuildNumber);
				dwBuildNumber &= 0x0ffff;
				bRet = TRUE;
			}

			::FreeLibrary(hModNtdll);
			hModNtdll = NULL;
		}

		return bRet;
	}


	bool Is64()
	{
		SYSTEM_INFO si;
		GetNativeSystemInfo(&si);
		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
			si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		{
			return true;
		}

		return false;
	}
}}}