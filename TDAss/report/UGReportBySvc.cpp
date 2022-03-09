
#include "UGReportBySvc.h"
#include <UGMQ.h>
#include <UGSerialize.h>

namespace UGReport{


	ReportBySvc* ReportBySvc::shared()
	{
		static ReportBySvc re;
		return &re;
	}

	void ReportBySvc::sendData(const std::wstring &sEvent)
	{
		std::wstring strName = L"SimcakeSvc_";
		std::wstring strGUID = L"{1FBBEA96-B21B-48E5-895A-21F116B13A02}";
		int iType = 10001;
		std::wstring sType = L"event";
		std::wstring strEvent = sEvent;

		Oasgames::Core::UGSerialize serial;
		serial << sType << strEvent;
		Oasgames::Core::UGMQ::FastSend(strName, strGUID, iType, serial.str());
	}

	void ReportBySvc::sendDataEx(const std::wstring &sParamType, const std::wstring &sEvent, LPCWSTR format, ...)
	{
		//std::wstring ws = L"report&" + sParamType + L"&" + sEvent + L"&";
		std::wstring sType = sParamType;
		std::wstring sMsg = L"";
		std::wstring strEvent = sEvent;

		va_list	args;
		va_start(args, format);
		int len = _vscwprintf(format, args);
		if (len > 0) {
			PWSTR buf = (PWSTR)malloc(sizeof(WCHAR)*(len + 1));
			if (buf) {
				len = vswprintf_s(buf, len + 1, format, args);
				sMsg = sMsg + (std::wstring)buf;
				free(buf);
			}
			va_end(args);
		}

		std::wstring strName = L"SimcakeSvc_";
		std::wstring strGUID = L"{1FBBEA96-B21B-48E5-895A-21F116B13A02}";
		int iType = 10001;
		
		Oasgames::Core::UGSerialize serial;
		serial << sType << sEvent << sMsg;
		Oasgames::Core::UGMQ::FastSend(strName, strGUID, iType, serial.str());

	}

	void ReportBySvc::sendLocate(const std::wstring &wsLocate)
	{
		std::wstring strName = L"SimcakeSvc_";
		std::wstring strGUID = L"{1FBBEA96-B21B-48E5-895A-21F116B13A02}}";
		int iType = 10001;
		std::wstring sType = L"locale";
		std::wstring sMsg = wsLocate;

		Oasgames::Core::UGSerialize serial;
		serial << sType << sMsg;
		Oasgames::Core::UGMQ::FastSend(strName, strGUID, iType, serial.str());

	}

}

