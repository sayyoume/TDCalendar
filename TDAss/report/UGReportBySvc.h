#ifndef __UGREPORTBYSVC_H

#include <windows.h>
#include <string>
#include "../Ass.h"

namespace UGReport{

	class APDDLL_DECLSPEC ReportBySvc
	{
	public:

		static ReportBySvc* shared();
		// 只有event时的上报
		void sendData(const std::wstring &sEvent);
		/*
		sendDataEx 此函数支持变参
		sParamType 的 Value 只有2个 status 和 params
		sEvent     事件
		format     sParamType的 名称及值
		示例：sendDataEx(L"status",L"browserprotect",L"\"protect_type\":\"%s\"",L"db");
		*/
		void sendDataEx(const std::wstring &sParamType, const std::wstring &sEvent, LPCWSTR  format, ...);

		// 通知服务更新语言
		void sendLocate(const std::wstring &wsLocate);

	};

}



#define __UGREPORTTYSVC_H
#endif //__UGREPORTTYSVC_H