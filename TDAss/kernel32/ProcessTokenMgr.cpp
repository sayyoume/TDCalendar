
#include "ProcessTokenMgr.h"

#pragma  comment(lib,"Secur32.lib")
#pragma  comment(lib,"Userenv.lib")

#include <UserEnv.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <Ntsecapi.h>
#include <Sddl.h>
#include <accctrl.h>
#include <Aclapi.h>
#include <Wtsapi32.h>

namespace TD{
	namespace Core{

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

		typedef struct
		{
			ULONG	Length;
			HANDLE  RootDirectory;
			PUNICODE_STRING	ObjectName;
			ULONG	Attributes;
			PSECURITY_DESCRIPTOR SecurityDescriptor;
			PSECURITY_QUALITY_OF_SERVICE SecurityQualityOfService;
		} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;


		typedef struct _PRIVILEGE_ENABLED
		{
			wchar_t* privilegeName;
			bool  enabledByDefault;
		}PRIVILEGE_ENABLED;

		PRIVILEGE_ENABLED defaultPrivilegesForAdmin [] = {
				{ L"SeIncreaseQuotaPrivilege", false },
				{ L"SeSecurityPrivilege", false },
				{ L"SeTakeOwnershipPrivilege", false },
				{ L"SeLoadDriverPrivilege", false },
				{ L"SeSystemProfilePrivilege", false },
				{ L"SeSystemtimePrivilege", false },
				{ L"SeProfileSingleProcessPrivilege", false },
				{ L"SeIncreaseBasePriorityPrivilege", false },
				{ L"SeCreatePagefilePrivilege", false },
				{ L"SeBackupPrivilege", false },
				{ L"SeRestorePrivilege", false },
				{ L"SeShutdownPrivilege", false },
				{ L"SeDebugPrivilege", false },
				{ L"SeSystemEnvironmentPrivilege", false },
				{ L"SeChangeNotifyPrivilege", true },
				{ L"SeRemoteShutdownPrivilege", false },
				{ L"SeUndockPrivilege", false },
				{ L"SeManageVolumePrivilege", false },
				{ L"SeImpersonatePrivilege", true },
				{ L"SeCreateGlobalPrivilege", true },
				{ L"SeIncreaseWorkingSetPrivilege", false },
				{ L"SeTimeZonePrivilege", false },
				{ L"SeCreateSymbolicLinkPrivilege", false }
		};

		typedef UINT(WINAPI * pfnZwCreateTokenType)(
			PHANDLE				TokenHandle,
			ACCESS_MASK			DesiredAccess,
			POBJECT_ATTRIBUTES	ObjectAttributes,
			TOKEN_TYPE			Type,
			PLUID					AuthenticationId,
			PLARGE_INTEGER		ExpirationTime,
			PTOKEN_USER			User,
			PTOKEN_GROUPS			Groups,
			PTOKEN_PRIVILEGES		Privileges,
			PTOKEN_OWNER			Owner,
			PTOKEN_PRIMARY_GROUP	PrimaryGroup,
			PTOKEN_DEFAULT_DACL	DefaultDacl,
			PTOKEN_SOURCE			Source
			);

		typedef BOOL(WINAPI* pfnWTSEnumerateSessions)(
			_In_   HANDLE hServer,
			_In_   DWORD Reserved,
			_In_   DWORD Version,
			_Out_  PWTS_SESSION_INFO *ppSessionInfo,
			_Out_  DWORD *pCount
			);

		typedef UINT(WINAPI * pfnRtlNtStatusToDosErrorType)(UINT dwError);

		pfnZwCreateTokenType			pfnZwCreateToken = NULL;
		pfnRtlNtStatusToDosErrorType	pfnRtlNtStatusToDosError = NULL;

		UGSystemToken::UGSystemToken()
		{
			fWTSGetActiveConsoleSessionId = (LoadWTSGetActiveConsoleSessionId) GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "WTSGetActiveConsoleSessionId");
			getLasPid(m_uiLsaId);
		}

		bool UGSystemToken::getTokenFromPid(unsigned int uiPid, _TOKEN_TYPE TokenType, HANDLE& hToken)
		{
			HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, uiPid);
			if (!hProcess)
				return false;

			HANDLE hOriToken = NULL;

			if (::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hOriToken))
			{
				if (DuplicateTokenEx(hOriToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenType, &hToken))
				{
					::CloseHandle(hOriToken);
					return true;
				}

				::CloseHandle(hOriToken);
			}

			::CloseHandle(hProcess);

			return true;
		}

		bool UGSystemToken::getTokenInfo(HANDLE hToken, TOKEN_INFORMATION_CLASS Tic, PVOID *pTokenInfoOut, unsigned int* pdwOutSize)
		{
			DWORD	dwSize = 0;
			PVOID	pv = NULL;

			bool bRet = true;

			if (NULL == pTokenInfoOut)
				bRet = false;

			else if (!GetTokenInformation(hToken, Tic, 0, 0, &dwSize)
				&& GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				bRet = false;

			else if (NULL == (pv = malloc(dwSize)))
				bRet = false;

			else if (!GetTokenInformation(hToken, Tic, pv, dwSize, &dwSize))
			{
				free(pv);
				*pTokenInfoOut = 0;
				bRet = false;
			}
			else
			{
				*pTokenInfoOut = pv;

				if (pdwOutSize)
					*pdwOutSize = dwSize;

				bRet = true;
			}

			return bRet;
		}

		bool UGSystemToken::getExplorerIdFromCurrentSessionId(int iCurSessionId, unsigned int& retPid)
		{
			HANDLE hToken = NULL;
			bool bRet = false;
			PROCESSENTRY32 pe32 = { 0 };
			DWORD dwProcessSessionId = 0;
			PVOID pTokenUserBuf = NULL;
			unsigned int size = 0;

			HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hProcessSnap == INVALID_HANDLE_VALUE)
				return false;

			pe32.dwSize = sizeof(PROCESSENTRY32);

			HANDLE hProcess = NULL;

			if (::Process32First(hProcessSnap, &pe32))
			{
				do
				{
					if (hProcess != NULL)
					{
						::CloseHandle(hProcess);
						hProcess = NULL;
					}

					if (hToken != NULL)
					{
						::CloseHandle(hToken);
						hToken = NULL;
					}

					if (0 != _tcsicmp(pe32.szExeFile, L"explorer.exe"))
						continue;

					if (FALSE == ::ProcessIdToSessionId(pe32.th32ProcessID, &dwProcessSessionId) || dwProcessSessionId != iCurSessionId)
						continue;

					if (NULL == (hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID)))
						continue;

					if (FALSE == ::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
						continue;

					pTokenUserBuf = NULL;

					if (FALSE == getTokenInfo(hToken, TokenUser, &pTokenUserBuf, &size))
						continue;

					BYTE adminSID[SECURITY_MAX_SID_SIZE];
					DWORD dwSize = sizeof(adminSID);
					CreateWellKnownSid(WinLocalSystemSid, NULL, adminSID, &dwSize);

					if (!EqualSid(((PTOKEN_USER) pTokenUserBuf)->User.Sid, adminSID))
					{
						retPid = pe32.th32ProcessID;
						bRet = true;

						free(pTokenUserBuf);
						pTokenUserBuf = NULL;
						break;
					}

					free(pTokenUserBuf);
					pTokenUserBuf = NULL;

				} while (::Process32Next(hProcessSnap, &pe32));
			}

			if (hProcess != NULL)
			{
				::CloseHandle(hProcess);
				hProcess = NULL;
			}

			if (hToken != NULL)
			{
				::CloseHandle(hToken);
				hToken = NULL;
			}

			if (hProcessSnap != INVALID_HANDLE_VALUE)
			{
				::CloseHandle(hProcessSnap);
			}

			return bRet;
		}

		bool UGSystemToken::getLasPid(unsigned int& retPid)
		{
			retPid = -1;

			HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hProcessSnap == INVALID_HANDLE_VALUE)
				return false;

			PROCESSENTRY32 pe32 = { 0 };
			pe32.dwSize = sizeof(PROCESSENTRY32);

			bool bRet = false;
			if (::Process32First(hProcessSnap, &pe32))
			{
				do
				{
					if (_tcsicmp(pe32.szExeFile, L"lsass.exe") == 0)
					{
						retPid = pe32.th32ProcessID;
						bRet = true;
						break;
					}

				} while (::Process32Next(hProcessSnap, &pe32));
			}

			if (hProcessSnap != INVALID_HANDLE_VALUE)
				CloseHandle(hProcessSnap);

			return bRet;
		}

		bool UGSystemToken::enableTokenPrivilege(HANDLE hToken, LPCTSTR szPrivName, bool bEnable)
		{
			if (NULL == hToken || NULL == szPrivName)
				return FALSE;

			TOKEN_PRIVILEGES Tp = { 0 };
			LUID	Luid = { 0 };

			if (!LookupPrivilegeValue(NULL, szPrivName, &Luid))
				return FALSE;

			Tp.PrivilegeCount = 1;
			Tp.Privileges[0].Luid = Luid;
			Tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

			return AdjustTokenPrivileges(hToken, FALSE, &Tp, sizeof(Tp), NULL, NULL) ? true : false;
		}

		PVOID UGSystemToken::getFromToken(HANDLE hToken, TOKEN_INFORMATION_CLASS tic)
		{
			DWORD n;
			BOOL rv = GetTokenInformation(hToken, tic, 0, 0, &n);
			if ((rv == FALSE) && (GetLastError() != ERROR_INSUFFICIENT_BUFFER))
				return 0;

			PBYTE p = new BYTE[n];
			if (GetTokenInformation(hToken, tic, p, n, &n) == FALSE)
			{
				free(p);
				return 0;
			}

			return p;
		}

		HANDLE UGSystemToken::createUserToken(HANDLE hBasicToken, PLUID pUsersession)
		{
			HANDLE retToken = NULL;

			SECURITY_QUALITY_OF_SERVICE sqos = { sizeof(sqos), SecurityIdentification, SECURITY_STATIC_TRACKING, FALSE };
			OBJECT_ATTRIBUTES oa = { sizeof(oa), 0, 0, 0, 0, &sqos };
			PTOKEN_STATISTICS stats = PTOKEN_STATISTICS(getFromToken(hBasicToken, TokenStatistics));
			LUID authid = stats->AuthenticationId;

			DWORD iTokenGroupsLen = 0;
			TOKEN_GROUPS* Groups = NULL;

			GetTokenInformation(hBasicToken, TokenGroups, NULL, 0, &iTokenGroupsLen);

			PSID adminPsid = NULL;
			if (!ConvertStringSidToSidW(L"S-1-5-32-544", &adminPsid))
				return NULL;

			PSID pIntegrityLevelSid = NULL;

			if (!ConvertStringSidToSidW(L"S-1-16-12288", &pIntegrityLevelSid))
				return NULL;

			PSID systemSid = NULL;
			if (!ConvertStringSidToSidW(L"S-1-5-114", &systemSid))
				return NULL;

			if (iTokenGroupsLen != 0)
			{
				Groups = (TOKEN_GROUPS*) malloc(iTokenGroupsLen);

				if (!GetTokenInformation(hBasicToken, TokenGroups, Groups, iTokenGroupsLen, &iTokenGroupsLen))
				{

				}

				bool hasAdmin = false;

				for (DWORD i = 0; i < Groups->GroupCount; i++)
				{
					if (Groups->Groups[i].Attributes & SE_GROUP_USE_FOR_DENY_ONLY)
					{
						Groups->Groups[i].Attributes = (SE_GROUP_OWNER | SE_GROUP_MANDATORY | SE_GROUP_ENABLED);
						//Groups->Groups[i].Attributes = (SE_GROUP_MANDATORY | SE_GROUP_ENABLED);
					}

					if (EqualSid(Groups->Groups[i].Sid, systemSid))
					{
						DWORD dw = Groups->Groups[i].Attributes;
						Groups->Groups[i].Attributes = (SE_GROUP_MANDATORY | SE_GROUP_ENABLED);
					}

					if (Groups->Groups[i].Attributes & SE_GROUP_INTEGRITY)
					{
						Groups->Groups[i].Sid = pIntegrityLevelSid;
					}
					if (EqualSid(Groups->Groups[i].Sid, adminPsid))
					{
						hasAdmin = true;
					}
				}

				if (!hasAdmin)
				{
					PTOKEN_GROUPS newGroups = (PTOKEN_GROUPS) malloc(iTokenGroupsLen + sizeof(SID_AND_ATTRIBUTES));
					for (DWORD i = 0; i < Groups->GroupCount; i++)
					{
						newGroups->Groups[i].Sid = Groups->Groups[i].Sid;
						newGroups->Groups[i].Attributes = Groups->Groups[i].Attributes;
					}
					newGroups->GroupCount = Groups->GroupCount + 1;
					newGroups->Groups[Groups->GroupCount].Sid = adminPsid;
					newGroups->Groups[Groups->GroupCount].Attributes = (SE_GROUP_OWNER | SE_GROUP_MANDATORY | SE_GROUP_ENABLED);
					free(Groups);
					Groups = newGroups;
				}
			}

			LUID_AND_ATTRIBUTES privileges[sizeof(defaultPrivilegesForAdmin) / sizeof(PRIVILEGE_ENABLED)];
			LUID luid;
			memset(&luid, 0, sizeof(LUID));
			for (int i = 0; i < sizeof(privileges) / sizeof(LUID_AND_ATTRIBUTES); i++)
			{
				if (!LookupPrivilegeValue(NULL, defaultPrivilegesForAdmin[i].privilegeName, &luid))
				{

				}
				else
				{
					memcpy(&privileges[i].Luid, &luid, sizeof(LUID));
					if (defaultPrivilegesForAdmin[i].enabledByDefault)
					{
						privileges[i].Attributes = (SE_PRIVILEGE_ENABLED | SE_PRIVILEGE_ENABLED_BY_DEFAULT);
					}
					else
					{
						privileges[i].Attributes = 0;
					}
				}
			}

			PTOKEN_PRIVILEGES token_privileges = NULL;
			token_privileges = (PTOKEN_PRIVILEGES) malloc(sizeof(DWORD) + sizeof(privileges));

			token_privileges->PrivilegeCount = sizeof(privileges) / sizeof(LUID_AND_ATTRIBUTES);
			memcpy(token_privileges->Privileges, privileges, sizeof(privileges));

			//prepare token owner

			TOKEN_OWNER token_owner;
			token_owner.Owner = adminPsid;

			//prepare DACL 

			PACL pacl;
			//	PSECURITY_DESCRIPTOR psd;
			PACL newACL;
			EXPLICIT_ACCESS ea;
			PTOKEN_DEFAULT_DACL ptoken_default_dacl;

			ptoken_default_dacl = (PTOKEN_DEFAULT_DACL) getFromToken(hBasicToken, TokenDefaultDacl);
			pacl = ptoken_default_dacl->DefaultDacl;

			memset(&ea, 0, sizeof(EXPLICIT_ACCESS));
			ea.grfAccessPermissions = GENERIC_ALL;
			ea.grfAccessMode = GRANT_ACCESS;
			ea.grfInheritance = NO_INHERITANCE;
			ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
			ea.Trustee.ptstrName = (wchar_t*) adminPsid;


			DWORD EntryCount;
			EXPLICIT_ACCESS *ExplicitEntries;

			DeleteAce(pacl, 0);
			GetExplicitEntriesFromAcl(pacl, &EntryCount, &ExplicitEntries);

			SetEntriesInAcl(2, ExplicitEntries, NULL, &newACL);
			SetEntriesInAcl(1, &ea, newACL, &newACL);

			ptoken_default_dacl->DefaultDacl = newACL;

			authid.HighPart = pUsersession->HighPart;
			authid.LowPart = pUsersession->LowPart;

			DWORD	dwCreateResult = 0;

			if (pfnZwCreateToken == 0 || pfnRtlNtStatusToDosError == 0)
			{
				HMODULE hNtdllMod = GetModuleHandle(TEXT("ntdll.dll"));
				pfnZwCreateToken = (pfnZwCreateTokenType) GetProcAddress(hNtdllMod, "ZwCreateToken");
				pfnRtlNtStatusToDosError = (pfnRtlNtStatusToDosErrorType) GetProcAddress(hNtdllMod, "RtlNtStatusToDosError");
			}

			if (pfnZwCreateToken == 0 || pfnRtlNtStatusToDosError == 0)
				return NULL;

			dwCreateResult = pfnZwCreateToken(
				&retToken,
				TOKEN_ALL_ACCESS,
				&oa,
				TokenPrimary,
				(PLUID) &authid,
				(PLARGE_INTEGER) &stats->ExpirationTime,
				(PTOKEN_USER) getFromToken(hBasicToken, TokenUser),
				Groups,
				token_privileges,
				&token_owner/*(PTOKEN_OWNER) getFromToken(hBasicToken, TokenOwner)*/,
				PTOKEN_PRIMARY_GROUP(getFromToken(hBasicToken, TokenPrimaryGroup)),
				ptoken_default_dacl,
				PTOKEN_SOURCE(getFromToken(hBasicToken, TokenSource))//&source
				);

			DWORD dwLastError = pfnRtlNtStatusToDosError(dwCreateResult);
			SetLastError(dwLastError);

			if (NT_SUCCESS(dwCreateResult))
			{
				if (retToken != 0)
				{
					if (Groups != NULL)
						free(Groups);

					if (pIntegrityLevelSid != NULL)
						LocalFree(pIntegrityLevelSid);

					if (token_privileges)
						free(token_privileges);

					TOKEN_ORIGIN* token_orign = (TOKEN_ORIGIN*) getFromToken(hBasicToken, (TOKEN_INFORMATION_CLASS) TokenOrigin);
					SetTokenInformation(retToken, (TOKEN_INFORMATION_CLASS) TokenOrigin, token_orign, sizeof(TOKEN_ORIGIN));
					return retToken;
				}
			}

			if (Groups != NULL)
				free(Groups);

			return NULL;
		}

		HANDLE UGSystemToken::CreateToken(ProcessTargetEnum iType)
		{
			unsigned int dwSessionId = 0;
			// 			if (fWTSGetActiveConsoleSessionId != NULL)
			// 				dwSessionId = fWTSGetActiveConsoleSessionId();

			{
				WTS_SESSION_INFO *sessionInfo = NULL;
				pfnWTSEnumerateSessions fnWTSEnumerateSessions = NULL;
				HMODULE hModWtsapi = LoadLibrary(L"Wtsapi32.dll");
				fnWTSEnumerateSessions = (pfnWTSEnumerateSessions) GetProcAddress(hModWtsapi, "WTSEnumerateSessionsW");
				if (NULL != fnWTSEnumerateSessions)
				{
					DWORD sessionInfoCount;
					BOOL result = fnWTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &sessionInfo, &sessionInfoCount);

					int num = 0;
					for (unsigned int i = 0; i < sessionInfoCount; ++i)
					{
						if ((sessionInfo[i].State == WTSActive))
						{
							dwSessionId = sessionInfo[i].SessionId;
							break;
						}
					}
					FreeLibrary(hModWtsapi);
				}
			}

			unsigned int dwExplorerId = 0;
			if (!getExplorerIdFromCurrentSessionId(dwSessionId, dwExplorerId))
				return NULL;

			HANDLE hExplorerToken = NULL;
			if (!getTokenFromPid(dwExplorerId, TokenPrimary, hExplorerToken))
				return NULL;

			HANDLE hLsaToken = NULL;

			bool bNotXp = true;
			if (bNotXp)
			{
				if (m_uiLsaId == -1)
					getLasPid(m_uiLsaId);

				//lsass.exe pid
				if (m_uiLsaId == -1)
					return NULL;

				if (!getTokenFromPid(m_uiLsaId, TokenPrimary, hLsaToken))
					return NULL;

				//enable create_token
				enableTokenPrivilege(hLsaToken, SE_CREATE_TOKEN_NAME, true);
				ImpersonateLoggedOnUser(hLsaToken);
			}

			HANDLE hToken = NULL;
			HANDLE hTokenBack = NULL;

			PSECURITY_LOGON_SESSION_DATA sData = NULL;
			NTSTATUS status;
			ULONG count;
			PLUID luidList = NULL;

			status = LsaEnumerateLogonSessions(&count, &luidList);

			if (!NT_SUCCESS(status))
				return NULL;

			/*TCHAR *sUserName = NULL;
			DWORD dwSize = 0;
			if (WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, dwSessionId, WTSUserName, &sUserName, &dwSize))
			{
			WTSFreeMemory(sUserName);
			}*/

			TOKEN_ELEVATION_TYPE tet;

			for (int i = 0; i < (int)count; i++)
			{
				status = LsaGetLogonSessionData(&luidList[i], &sData);
				if (!NT_SUCCESS(status))
					break;

				if (sData->UserName.Buffer != NULL)
				{
					if (sData->Session == dwSessionId)
					{
						HANDLE hTempToken = createUserToken(hExplorerToken, &luidList[i]);

						DWORD Size = 0;
						if (GetTokenInformation(hTempToken, (TOKEN_INFORMATION_CLASS) TokenElevationType, &tet, sizeof(TOKEN_ELEVATION_TYPE), &Size))
						{
							if (tet == TokenElevationTypeFull)
							{
								hToken = hTempToken;
								LsaFreeReturnBuffer(sData);
								break;
							}
							else if (tet == TokenElevationTypeDefault)
							{
								if (hTokenBack != nullptr)
								{
									CloseHandle(hTempToken);
									break;
								}

								hTokenBack = hTempToken;
							}
							else
							{
								CloseHandle(hTempToken);
							}
						}
						else
						{
							hToken = hTempToken;
							LsaFreeReturnBuffer(sData);
							break;
						}
					}
				}

				LsaFreeReturnBuffer(sData);
			}
			if (hToken == nullptr)
			{
				hToken = hTokenBack;
			}

			if (hToken)
			{
				DWORD	dwSessionIdLen = sizeof(DWORD);
				if (0 == SetTokenInformation(hToken, TokenSessionId, &dwSessionId, dwSessionIdLen))
				{

				}
			}

			if (hLsaToken)
			{
				::RevertToSelf();
				::CloseHandle(hLsaToken);
			}

			if (hExplorerToken)
				::CloseHandle(hExplorerToken);

			if (hToken != nullptr)
			{
				std::wstring sSid = L"";

				switch (iType)
				{
				case TargetToken_Admin:
					{
						break;
					}
				case TargetToken_Normal:
					{
						sSid = L"S-1-16-8192";
						break;
					}
				case TargetToken_Low:
					{
						sSid = L"S-1-16-4096";
						break;
					}
				}

				if (!sSid.empty())
				{
					do
					{
						PSID  pIntegritySid = nullptr;

						TOKEN_MANDATORY_LABEL til = { 0 };

						if (!ConvertStringSidToSid(sSid.c_str(), &pIntegritySid))
							break;

						til.Label.Attributes = SE_GROUP_INTEGRITY;
						til.Label.Sid = pIntegritySid;

						if (!::SetTokenInformation(hToken, (TOKEN_INFORMATION_CLASS) TokenIntegrityLevel, &til, sizeof(TOKEN_MANDATORY_LABEL)))
						{
							CloseHandle(hToken);
							hToken = nullptr;
							break;
						}

						if (pIntegritySid)
						{
							::LocalFree((HLOCAL) pIntegritySid);
							pIntegritySid = NULL;
						}

					} while (0);
				}
			}

			return hToken;
		}

		bool UGProcessTokenMgr::CreateProcess(ProcessEnum iCurTokenType, ProcessTargetEnum iTargetTokenType, const std::wstring& sPath, const std::wstring& sCmd)
		{
			PROCESS_INFORMATION pi;
			bool bRet = CreateProcess(iCurTokenType, iTargetTokenType, sPath, sCmd, pi);

			if (bRet)
			{
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}

			return bRet;
		}

		bool UGProcessTokenMgr::CreateProcess(ProcessEnum iCurTokenType, ProcessTargetEnum iTargetTokenType, const std::wstring& sPath, const std::wstring& sCmd, PROCESS_INFORMATION& pi)
		{
			HANDLE hNewToken = nullptr;

			switch (iCurTokenType)
			{
			case CurrentProcessToken_System:
				{
					UGSystemToken obj;
					hNewToken = obj.CreateToken(iTargetTokenType);
					break;
				}
			case CurrentProcessToken_Admin:
				{
					UGAdminToken obj;
					hNewToken = obj.CreateToken(iTargetTokenType);
					break;
				}
			}

			if (!hNewToken)
				return false;

			LPVOID pEnv = NULL;
			BOOL bRet = CreateEnvironmentBlock(&pEnv, hNewToken, FALSE);

			STARTUPINFO si = { sizeof(STARTUPINFO) };
			si.lpDesktop = L"WinSta0\\Default";
			si.wShowWindow = SW_SHOWNORMAL;
			si.dwFlags = STARTF_USESHOWWINDOW;
			

			LPTSTR lpszTempCmdLine = (LPTSTR) sCmd.c_str();

			wchar_t runBuf[4096] = { 0 };
			swprintf_s(runBuf, L"%s %s", sPath.c_str(), lpszTempCmdLine);

			DWORD dwCreateFlag = CREATE_NEW_CONSOLE;
			if (bRet)
			{
				dwCreateFlag |= CREATE_UNICODE_ENVIRONMENT;
			}
			else
			{
				dwCreateFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_SEPARATE_WOW_VDM;
			}

			BOOL bResult = CreateProcessAsUser(hNewToken, NULL, runBuf, NULL, NULL, FALSE, dwCreateFlag, bRet ? pEnv : NULL, NULL, &si, &pi);

			CloseHandle(hNewToken);
			if (bRet)
				DestroyEnvironmentBlock(pEnv);

			if (bResult)
				return true;

			return false;
		}

		HANDLE UGAdminToken::CreateToken(ProcessTargetEnum iType)
		{
			HANDLE hToken = nullptr;
			HANDLE hProcess = GetCurrentProcess();

			if (!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
			{
				CloseHandle(hProcess);
				return nullptr;
			}

			HANDLE hDupToken = nullptr;
			if (!::DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, 0, SecurityImpersonation, TokenPrimary, &hDupToken))
			{
				CloseHandle(hToken);
				CloseHandle(hProcess);
				return nullptr;
			}

			std::wstring sSid = L"";

			switch (iType)
			{
			case TargetToken_Admin:
				{
					break;
				}
			case TargetToken_Normal:
				{
					sSid = L"S-1-16-8192";
					break;
				}
			case TargetToken_Low:
				{
					sSid = L"S-1-16-4096";
					break;
				}
			}

			if (!sSid.empty())
			{
				do 
				{
					PSID  pIntegritySid = nullptr;

					TOKEN_MANDATORY_LABEL til = { 0 };

					if (!ConvertStringSidToSid(sSid.c_str(), &pIntegritySid))
						break;

					til.Label.Attributes = SE_GROUP_INTEGRITY;
					til.Label.Sid = pIntegritySid;

					if (!::SetTokenInformation(hDupToken, (TOKEN_INFORMATION_CLASS) TokenIntegrityLevel, &til, sizeof(TOKEN_MANDATORY_LABEL)))
					{
						CloseHandle(hDupToken);
						hDupToken = nullptr;
						break;
					}

					if (pIntegritySid)
					{
						::LocalFree((HLOCAL) pIntegritySid);
						pIntegritySid = NULL;
					}

				} while (0);
			}
			
			CloseHandle(hToken);
			CloseHandle(hProcess);

			return hDupToken;
		}

	}
}