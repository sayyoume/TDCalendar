
#include "UDReport.h"
#include "../base/JTerseUid.h"
#include "../JTerseUtilities.h"
#include "../Reg/JTerseRegOpt.h"
#include "../safe/StrEncrypt.h"
#include <Shlwapi.h>
#include "../base/JTerseReportUtilities.h"

namespace UD{

	UGReport* UGReport::shared()
	{
		static UGReport objReport;
		return &objReport;
	}

	UGReport::UGReport()
	{
		wchar_t szGuid[MAX_PATH] = {0};
		JTerse::core::ReportLog::getGUID(szGuid, MAX_PATH);
		m_sUid  = szGuid;
		m_appid = 2835396161;
		m_sVersion = L"2.6.31.0";
		m_sGaID = L"UA-76562005-3";

	}

	UGReport::~UGReport()
	{
		
	}

	void UGReport::sendReport(const std::wstring& sEvent, bool bImmediately, bool bDmp)
	{
		wchar_t buff[1024]={0};

		swprintf_s(buff, L"{\"event\":\"%s\",\"channel\":\"%s\",\"subchannel\":\"%s\",\"version\":\"%s\",\"locale\":%d,\"uuid\":\"%s\",\"appid\":%I64d,\"params\":{\"lan\":%d}}",
			sEvent.c_str(), m_sChannel.c_str(), m_sSubChannel.c_str(), m_sVersion.c_str(), m_dwlcid, m_sUid.c_str(), m_appid, m_dwlanid);

		doSend(m_sUrl,buff, bImmediately);

		if (bDmp)
		{
			wchar_t buff1[1024] = { 0 };

			swprintf_s(buff1, L"{\"event\":\"%s\",\"channel\":\"%s\",\"subchannel\":\"%s\",\"version\":\"%s\",\"locale\":%d,\"uuid\":\"%s\",\"appid\":%I64d,\"params\":{\"lan\":%d,\"locale\":%d}}",
				sEvent.c_str(), m_sChannel.c_str(), m_sSubChannel.c_str(), m_sVersion.c_str(), m_dwlcid, m_sUid.c_str(), m_appid, m_dwlanid, m_dwlcid);
			doSend(m_sUrladc,buff1, bImmediately);


			wchar_t buff2[1024] = { 0 };

			swprintf_s(buff2, L"v=1&t=event&tid=%s&cid=%s&ec=%s&ea=%s&el=%s&ev=%d", m_sGaID.c_str(),
				m_sUid.c_str(), m_sChannel.c_str(), sEvent.c_str(), m_sVersion.c_str(), m_dwlanid);
			doSend(m_sUrlga, buff2, true);
		}
			
	}

	void UGReport::sendReport(const std::wstring& sEvent, const int id, const std::wstring & strReason,bool bImmediately)
	{
		wchar_t buff[1024] = { 0 };
		swprintf_s(buff, L"{\"event\":\"%s\",\"channel\":\"%s\",\"subchannel\":\"%s\",\"version\":\"%s\",\"locale\":%d,\"uuid\":\"%s\",\"appid\":%I64d,\"params\":{\"reasonid\":\"%d\",\"anyreason\":\"%s\",\"lan\":%d}}",
			sEvent.c_str(), m_sChannel.c_str(), m_sSubChannel.c_str(), m_sVersion.c_str(), m_dwlcid, m_sUid.c_str(), m_appid, id, strReason.c_str(), m_dwlanid);

		doSend(m_sUrl,buff, bImmediately);

		wchar_t buff1[1024] = { 0 };
		swprintf_s(buff1, L"{\"event\":\"%s\",\"channel\":\"%s\",\"subchannel\":\"%s\",\"version\":\"%s\",\"locale\":%d,\"uuid\":\"%s\",\"appid\":%I64d,\"params\":{\"lan\":%d,\"locale\":%d}}",
			sEvent.c_str(), m_sChannel.c_str(), m_sSubChannel.c_str(), m_sVersion.c_str(), m_dwlcid, m_sUid.c_str(), m_appid, m_dwlanid, m_dwlcid);
		doSend(m_sUrladc, buff1, bImmediately);

		wchar_t buff2[1024] = { 0 };

		swprintf_s(buff2, L"v=1&t=event&tid=%s&cid=%s&ec=%s&ea=%s&el=%s&ev=%d", m_sGaID.c_str(),
			m_sUid.c_str(), m_sChannel.c_str(), sEvent.c_str(), m_sVersion.c_str(), m_dwlanid);
		doSend(m_sUrlga, buff2, bImmediately);
	}

	void UGReport::sendReport(const std::wstring& sEvent)
	{
		wchar_t buff[1024] = { 0 };
		swprintf_s(buff, L"{\"event\":\"%s\",\"channel\":\"%s\",\"subchannel\":\"%s\",\"version\":\"%s\",\"locale\":%d,\"uuid\":\"%s\",\"appid\":%I64d,,\"params\":{\"lan\":%d}}",
			sEvent.c_str(), m_sChannel.c_str(), m_sSubChannel.c_str(), m_sVersion.c_str(), m_dwlcid, m_sUid.c_str(), m_appid, m_dwlanid);

		doSend(m_sUrl,buff, true);

		wchar_t buff2[1024] = { 0 };

		swprintf_s(buff2, L"v=1&t=event&tid=%s&cid=%s&ec=%s&ea=%s&el=%s&ev=%d", m_sGaID.c_str(),
			m_sUid.c_str(), m_sChannel.c_str(), sEvent.c_str(), m_sVersion.c_str(), m_dwlanid);
		doSend(m_sUrlga, buff2, true);
	}


	void UGReport::doSend(const std::wstring& sUrl, const std::wstring& sData, bool bImmediately)
	{
		std::wstring sResponse;
		if (bImmediately)
			m_reportMgr.postTaskEx(sUrl,sData, sResponse);
		else
			m_reportMgr.addTaskEx(sUrl,sData);
	}

	void UGReport::initData()
	{
		JTerse::core::fastReadRegValue(HKEY_LOCAL_MACHINE, UNINSTALLSTRING,	L"sChannel", m_sChannel, false, false);
		if (m_sChannel.empty())
			m_sChannel = L"simcake";
		
		JTerse::core::fastReadRegValue(HKEY_LOCAL_MACHINE, UNINSTALLSTRING,	L"sSubChannel", m_sSubChannel, false, false);
		if (m_sChannel.empty())
			m_sChannel = L"simcake";

		JTerse::core::fastReadRegValue(HKEY_LOCAL_MACHINE, UNINSTALLSTRING,	L"DisplayVersion", m_sVersion, false, false);

		std::wstring sIniPath = L"";
		wchar_t czLang[256] = { 0 };
		JTerse::core::fastReadRegValue(HKEY_LOCAL_MACHINE, UNINSTALLSTRING,	L"DisplayIcon", sIniPath, false, false);

		int ipos = sIniPath.rfind('\\');
		if (-1 != ipos)
		{
			sIniPath = sIniPath.substr(0, ipos + 1) + L"data\\ugconfig.ini";
			GetPrivateProfileString(L"APDMAIN", L"languange", L"", czLang, 256, sIniPath.c_str());
		}

		m_dwlcid =  ::GetUserDefaultLCID();
		m_dwlanid = ::GetUserDefaultLangID();

		std::wstring sLang(czLang);
		if (!sLang.empty())
		{
			m_sNative = sLang;
		}
 		else
 		{
 			JTerse::core::ReportLog::ReportUtilities::GetNationLanguage(m_sNative, 1);
 		}
		
		m_sUrl = safe::CStrEncrypt::shared()->WStringDecrypt(sreport_url);
		m_sHost = safe::CStrEncrypt::shared()->WStringDecrypt(sreport_host);
		
		m_sUrladc = safe::CStrEncrypt::shared()->WStringDecrypt(sreport_url_adc);
		m_sHostadc = safe::CStrEncrypt::shared()->WStringDecrypt(sreport_host_adc);

		m_sUrlga = L"http://www.google-analytics.com/collect";
		
	}

	void UGReport::shutdown()
	{
		m_reportMgr.shutdown();
	}

}