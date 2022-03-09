#ifndef __UGREPORTBYSVC_H

#include <windows.h>
#include <string>
#include "../Ass.h"

namespace UGReport{

	class APDDLL_DECLSPEC ReportBySvc
	{
	public:

		static ReportBySvc* shared();
		// ֻ��eventʱ���ϱ�
		void sendData(const std::wstring &sEvent);
		/*
		sendDataEx �˺���֧�ֱ��
		sParamType �� Value ֻ��2�� status �� params
		sEvent     �¼�
		format     sParamType�� ���Ƽ�ֵ
		ʾ����sendDataEx(L"status",L"browserprotect",L"\"protect_type\":\"%s\"",L"db");
		*/
		void sendDataEx(const std::wstring &sParamType, const std::wstring &sEvent, LPCWSTR  format, ...);

		// ֪ͨ�����������
		void sendLocate(const std::wstring &wsLocate);

	};

}



#define __UGREPORTTYSVC_H
#endif //__UGREPORTTYSVC_H