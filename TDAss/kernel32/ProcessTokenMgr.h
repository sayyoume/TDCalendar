
#pragma once

#include "../ass.h"
#include <string>

namespace TD{
	namespace Core{

		enum ProcessEnum
		{
			CurrentProcessToken_System=899,	//ϵͳȨ��
			CurrentProcessToken_Admin,		//����ԱȨ��
		};

		enum ProcessTargetEnum
		{
			TargetToken_Admin=59988,		//����ԱȨ��
			TargetToken_Normal,				//��׼Ȩ��
			TargetToken_Low,				//��Ȩ��
		};

		class UGSystemToken
		{
		public:
			UGSystemToken();
			~UGSystemToken(){}

			HANDLE CreateToken(ProcessTargetEnum iType);

		private:
			bool getExplorerIdFromCurrentSessionId(int iCurSessionId, unsigned int& retPid);
			bool getTokenInfo(HANDLE hToken, TOKEN_INFORMATION_CLASS Tic, PVOID *pTokenInfoOut, unsigned int* pdwOutSize);
			bool getTokenFromPid(unsigned int uiPid, _TOKEN_TYPE TokenType, HANDLE& hToken);
			bool getLasPid(unsigned int& retPid);
			bool enableTokenPrivilege(HANDLE hToken, LPCTSTR szPrivName, bool bEnable);
			PVOID getFromToken(HANDLE hToken, TOKEN_INFORMATION_CLASS tic);
			HANDLE createUserToken(HANDLE hBasicToken, PLUID pUsersession);

		private:
			typedef DWORD(WINAPI  *LoadWTSGetActiveConsoleSessionId)(VOID);
			LoadWTSGetActiveConsoleSessionId fWTSGetActiveConsoleSessionId;
			unsigned int m_uiLsaId;
		};

		class APDDLL_DECLSPEC UGAdminToken
		{
		public:
			HANDLE CreateToken(ProcessTargetEnum iType);
		};

		class  UGProcessTokenMgr
		{
		public:
			APDDLL_DECLSPEC UGProcessTokenMgr(){}
			APDDLL_DECLSPEC  ~UGProcessTokenMgr(){}

			//ֻ֧�ִӸ�Ȩ�޴�����Ȩ�޽��̣���֧�ִ������Լ�Ȩ�޸ߵĽ���
			 APDDLL_DECLSPEC  bool CreateProcess(ProcessEnum iCurTokenType, ProcessTargetEnum iTargetTokenType, const std::wstring& sPath, const std::wstring& sCmd);
			//����PROCESS_INFORMATION ���ݽṹ ���йر�
			 APDDLL_DECLSPEC  bool CreateProcess(ProcessEnum iCurTokenType, ProcessTargetEnum iTargetTokenType, const std::wstring& sPath, const std::wstring& sCmd, PROCESS_INFORMATION& pi);
		};

	}
}