
#pragma once

#include <windows.h>
#include <functional>
#include <future>
#include "..\Ass.h"

using namespace std::placeholders;

namespace Oasgames{
	namespace core{
	
		class APDDLL_DECLSPEC UGPipeServer
		{
		public:
			UGPipeServer();
			virtual ~UGPipeServer();

			//int(const std::wstring& sRecvData,std::wstring& sAnswer)
			//返回int 为等待时间，返回的sAnswer要发送给对方的数据
			bool listen(const std::wstring& sName, const std::wstring& sGuid, const std::function<int(const std::wstring& sRecvData,std::wstring& sAnswer)>& func);
			bool listen2(const std::wstring& sName, const std::wstring& sGuid, const std::function<int(const std::wstring& sRecvData)>& func);
			void stop();
		private:
			BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo);
		private:
			bool m_bStop;
			std::future<void> m_future;
			std::wstring sPipeName;
		};

		//等待对方回答
		bool APDDLL_DECLSPEC  sendPipeData(const std::wstring& sData, const std::wstring& sPipeName,
			const std::wstring& sGuid, const std::function<void(const std::wstring& sAnswer, bool bAnswer)>& func, int iWaitTimer);

		bool APDDLL_DECLSPEC  sendPipeData(const std::wstring& sData, const std::wstring& sPipeName, const std::wstring& sGuid);
	}
}