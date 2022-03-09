
#pragma once

#include <windows.h>
#include <string>
#include "../download/UDReportMgr.h"
#include "../Ass.h"

#define UNINSTALLSTRING L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{CA42170D-90AC-4578-A858-C851D886B82B}"

namespace UD{

	const std::wstring sreport_host = L"vWN7GxtTYxc6WxNDF0M6Y3t7G5qB";					  //collect.mdata.cool
	const std::wstring sreport_url = L"tAsXFwcuenpjexsbU2MXOlsTQxdDOmN7ext6WxNDF0M6BwsH"; //http://collect.mdata.cool/mdata.php
	const std::wstring sreport_urlpath = L"2npbE0MXQzoHCwdX";							  ///mdata.php
	const std::wstring sreport_host_adc = L"xEMTYzpbE0MXQzpje3sb";						  //adc.mdata.cool
	const std::wstring sreport_url_adc = L"ogsXFwcuenpjexsbU2MXOiMTWwc6S3t6E1sHemdXI1tLFzoHCwfL";     //http://collect.bdmp.io/dmp/submit.php
	
	class APDDLL_DECLSPEC UGReport
	{
	public:
		static UGReport* shared();
		void sendReport(const std::wstring& sEvent, bool bImmediately,bool bAdc = true);
		void sendReport(const std::wstring& sEvent, const int id,const std::wstring & strReason,bool bImmediately);
		void sendReport(const std::wstring& sEvent);
		void doSend(const std::wstring& sUrl,const std::wstring& sData, bool bImmediately);
		void initData();
		//此函数不调 程序无法退出
		void shutdown();  

	public:

		std::wstring m_sChannel;
		std::wstring m_sSubChannel;
		std::wstring m_sVersion;
		std::wstring m_sNative;

	private:
		UGReport();
		~UGReport();

		CUGReportMgr m_reportMgr;

		std::wstring m_sUid;
		__int64 m_appid;
		unsigned long m_dwlcid;
		unsigned long m_dwlanid;
		std::wstring m_sUrl;
		std::wstring m_sHost;
		std::wstring m_sHostadc;
		std::wstring m_sUrladc;
		std::wstring m_sUrlga;
		std::wstring m_sGaID;
	};

}


