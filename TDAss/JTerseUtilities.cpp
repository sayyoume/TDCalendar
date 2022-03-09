
#include "JTerseUtilities.h"
#include <windows.h>
#include <Sensapi.h>
#include <Wininet.h>
#include "zip/unzip.h"
#include "fileutils.h"
#include <TlHelp32.h>
#include <Shlobj.h>
#include <regex> 
#include <process.h>
#include <Psapi.h>
#include <comutil.h>
#include <Strsafe.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <mscat.h>
#include <Softpub.h>
#pragma warning(disable:4996)

#pragma comment(lib,"Sensapi.lib")
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"Version.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Psapi.lib")

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wintrust")


namespace JTerse{namespace core{

	bool CheckNetworkAlive(std::wstring& sCheckUrl)
	{
		DWORD dwCheckType = NETWORK_ALIVE_WAN;

		if (IsNetworkAlive((LPDWORD)&dwCheckType)
			&& InternetCheckConnection(sCheckUrl.c_str(), FLAG_ICC_FORCE_CONNECTION, NULL))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool getFileSizeEx(std::wstring sFilePath, int* nSize)
	{
		HANDLE hFile = ::CreateFile(sFilePath.c_str(), FILE_READ_EA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

		if (hFile != INVALID_HANDLE_VALUE)  
		{
			*nSize = ::GetFileSize(hFile, NULL);
			CloseHandle(hFile);
			return true;
		}

		return false;
	}

	std::wstring StringTrim(const std::wstring &source, const std::wstring &targets)
	{
		std::wstring::size_type start = 0;
		std::wstring::size_type end = 0;
		for (start = 0; start < source.size(); start++)
		{
			bool bIsTarget = false;
			for (std::wstring::size_type i = 0; i < targets.size(); i++)
			{
				if (source[start] == targets[i])
				{
					bIsTarget = true;
					break;
				}
			}
			if (!bIsTarget)
			{
				break;
			}
		}

		for (end = source.size() - 1; (int)end >= 0; end--)
		{
			bool bIsTarget = false;
			for (std::wstring::size_type i = 0; i < targets.size(); i++)
			{
				if (source[end] == targets[i])
				{
					bIsTarget = true;
					break;
				}
			}
			if (!bIsTarget)
			{
				break;
			}
		}

		std::wstring result = L"";
		if (end >= start && start < source.size() && end >= 0)
		{
			result = source.substr(start, end-start+1);
		}

		return result;
	}

	std::string ws2s(const std::wstring& wide, unsigned int CodePage)
	{
		int wide_length = static_cast<int>(wide.length());
		if (wide_length == 0)
			return std::string();

		// Compute the length of the buffer we'll need.
		int charcount = WideCharToMultiByte(CodePage, 0, wide.data(), wide_length,
			NULL, 0, NULL, NULL);
		if (charcount == 0)
			return std::string();

		std::string mb;
		mb.resize(charcount);
		WideCharToMultiByte(CodePage, 0, wide.data(), wide_length,
			&mb[0], charcount, NULL, NULL);

		return mb;
	}


	std::wstring s2ws( const std::string& mb,unsigned int CodePage)
	{
		if (mb.empty())
			return std::wstring();

		int mb_length = static_cast<int>(mb.length());
		// Compute the length of the buffer.
		int charcount = MultiByteToWideChar(CodePage, 0,
			mb.data(), mb_length, NULL, 0);
		if (charcount == 0)
			return std::wstring();

		std::wstring wide;
		wide.resize(charcount);
		MultiByteToWideChar(CodePage, 0, mb.data(), mb_length, &wide[0], charcount);

		return wide;
	}

	bool unZip(const std::wstring& sSrc,const std::wstring& sDstDir)
	{
		HZIP hZip = OpenZip(sSrc.c_str(),NULL);

		if (hZip != NULL)
		{
			bool bResult = false;
			ZRESULT  zr = SetUnzipBaseDir(hZip,sDstDir.c_str());
			if(zr != ZR_OK)
			{
				CloseZip(hZip);
				return false;
			}

			ZIPENTRY ze;
			zr = GetZipItem(hZip,-1,&ze);
			if (zr != ZR_OK)
			{
				 CloseZip(hZip);
				 return false;
			}
			
			int numitems = ze.index;

			for (int i=0; i<numitems; i++)
			{
				zr=GetZipItem(hZip,i,&ze);
				zr=UnzipItem(hZip,i,ze.name);

				/*if(zr != ZR_OK)
				{
					CloseZip(hZip);
					return false;
				}*/
			}

			CloseZip(hZip);
			return true;
		}

		return false;
	}

	bool killProcessByName(const std::wstring& sname,bool wait)
	{
		HANDLE   hSnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0   ); 
		PROCESSENTRY32   procentry;
		DWORD pid;
		bool bret = true;
		procentry.dwSize	=sizeof(PROCESSENTRY32); 
		BOOL  bFlag=Process32First(   hSnapShot,   &procentry   ) ; 
		while( bFlag ) 
		{ 
			if(_wcsicmp(procentry.szExeFile, sname.c_str())==0) 
			{
				pid=procentry.th32ProcessID;
				bret =KillProcessById(pid,0,wait);

			}
			bFlag=Process32Next(hSnapShot,&procentry); 
		} 

		return bret;
	}


    bool killProcessByNameEx(const std::wstring& sname,bool wait)
    {
        HANDLE   hSnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0   ); 
        PROCESSENTRY32   procentry;
        DWORD dwpid = _getpid();
        DWORD pid;
        bool bret = true;
        procentry.dwSize	=sizeof(PROCESSENTRY32); 
        BOOL  bFlag=Process32First(   hSnapShot,   &procentry   ) ; 
        while( bFlag ) 
        { 
            if(_wcsicmp(procentry.szExeFile, sname.c_str())==0) 
            {
                pid=procentry.th32ProcessID;
                if (pid != dwpid)
                {
                    bret =KillProcessById(pid,0,wait);
                }
            }
            bFlag=Process32Next(hSnapShot,&procentry); 
        } 

        return bret;

    }

	bool killProcessByPath(const std::wstring& spath, bool wait)
	{
		HANDLE   hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32   procentry;
		DWORD dwpid = _getpid();
		DWORD pid;
		bool bret = false;
		TCHAR szPath[MAX_PATH] = { 0 };
		procentry.dwSize = sizeof(PROCESSENTRY32);
		BOOL  bFlag = Process32First(hSnapShot, &procentry);
		while (bFlag)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procentry.th32ProcessID);
			if (hProcess != NULL)
			{
				if (GetModuleFileNameEx(hProcess, NULL, szPath, MAX_PATH) > 0)
				{
					if (_wcsicmp(szPath, spath.c_str()) == 0)
					{
						pid = procentry.th32ProcessID;
						if (pid != dwpid)
						{
							bret = KillProcess(hProcess, 0, wait);
						}
					}
				}
				CloseHandle(hProcess);
			}
			
			bFlag = Process32Next(hSnapShot, &procentry);
		}

		return bret;
	}

	bool KillProcessById(DWORD process_id, int exit_code,bool wait) 
	{
		HANDLE process = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE,
			FALSE,  // Don't inherit handle
			process_id);
		if (!process) {
			return false;
		}
		bool ret = KillProcess(process, exit_code,wait);
		CloseHandle(process);
		return ret;
	}


	bool KillProcess(HANDLE process, int exit_code, bool wait) {
		bool result = (TerminateProcess(process, exit_code) != FALSE);
		if (result && wait) {
			// The process may not end immediately due to pending I/O
			WaitForSingleObject(process, 60 * 1000);
		}
		return result;
	}



	bool LaunchProcess(const std::wstring& sFilePath,const std::wstring& sCmdline,bool wait) 
	{
			STARTUPINFO startup_info = {};
			PROCESS_INFORMATION process_info;
			bool bret = false;
			std::wstring cmdline = sFilePath;
			cmdline.append(L" ");
			cmdline.append(sCmdline);
			if (CreateProcessW(NULL,const_cast<wchar_t*>(cmdline.c_str()), NULL, NULL, false, 0, NULL,
				NULL, &startup_info, &process_info)) 
			{
				if (wait)
				{
					WaitForSingleObject(process_info.hProcess, INFINITE);
				}
				
				CloseHandle(process_info.hThread);
				CloseHandle(process_info.hProcess);
				bret = true;
			}

			return bret;
	}



	void waitExecute(const std::wstring& sFilePath,const std::wstring& sParam, DWORD dwTime , bool bTimeoutKill)
	{
		if (!file::pathFileExists(sFilePath.c_str()))
			return;

		SHELLEXECUTEINFO ShellInfo;

		memset(&ShellInfo, 0, sizeof(ShellInfo));
		ShellInfo.cbSize = sizeof(ShellInfo);
		ShellInfo.hwnd = NULL;
		ShellInfo.lpVerb = L"open";
		ShellInfo.lpFile = sFilePath.c_str();
		ShellInfo.nShow = SW_HIDE;
		ShellInfo.lpParameters = sParam.c_str();
		ShellExecuteEx(&ShellInfo);


		WaitForSingleObject(ShellInfo.hProcess,dwTime);

		
	}


	void Execute(const std::wstring& sFilePath,const std::wstring& sParam,bool bWait,const std::wstring& sdir)
	{
		if (!file::pathFileExists(sFilePath.c_str()))
			return;

		SHELLEXECUTEINFO ShellInfo;
		std::wstring strPath = L"";
		memset(&ShellInfo, 0, sizeof(ShellInfo));
		ShellInfo.cbSize = sizeof(ShellInfo);
		ShellInfo.hwnd = NULL;
		ShellInfo.lpVerb = L"open";
		ShellInfo.lpFile = sFilePath.c_str();
		ShellInfo.nShow = SW_HIDE;
		ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShellInfo.lpParameters = sParam.c_str();
		if (sdir.empty())
		{
			wchar_t exeFullPath[MAX_PATH];

			GetModuleFileName(NULL,exeFullPath,MAX_PATH);
			strPath=(std::wstring)exeFullPath;
			int pos = strPath.find_last_of('\\', strPath.length());
			strPath = strPath.substr(0, pos);

			ShellInfo.lpDirectory = strPath.c_str();
		}
		else
		{
			ShellInfo.lpDirectory = sdir.c_str();
		}

		ShellExecuteEx(&ShellInfo);

		if (bWait)
			WaitForSingleObject(ShellInfo.hProcess,600000);
	}

	bool GetFileVersion(const std::wstring& sPath,std::wstring& sVersion)
	{
		int nMaxPathName = 4096; // Max length of file name/path
		wchar_t pBuffer[100] = {0};
		//UINT nItemLength;
		void* pData;
		bool bRet = false;
		DWORD dwInfoSize, dwHandle,dwErr;

		if (!file::pathFileExists(sPath))
		{
			return bRet;
		}

		// Get File Version Info size
		dwInfoSize = ::GetFileVersionInfoSizeW(sPath.data(), &dwHandle);

		if (dwInfoSize > 0)
		{
			VS_FIXEDFILEINFO * pFileInfo;
			pData = new wchar_t[dwInfoSize+1];
			ZeroMemory(pData,(dwInfoSize+1)*sizeof(wchar_t));
			if (::GetFileVersionInfoW(sPath.data(), dwHandle, dwInfoSize, pData))
			{
				//if (::VerQueryValueW(pData, L"\\", (LPVOID*)&pFileInfo, &nItemLength))
				if (::VerQueryValueW(pData, L"\\", (LPVOID*)&pFileInfo, (PUINT)&dwInfoSize))
				{
// 					swprintf_s(pBuffer, L"%d.%d.%d.%d",
// 						pFileInfo->dwFileVersionMS >> 16,
// 						pFileInfo->dwFileVersionMS & 0xFFFF,
// 						pFileInfo->dwFileVersionLS >> 16,
// 						pFileInfo->dwFileVersionLS & 0xFFFF);
					WORD wBuf[4];
					wBuf[0] = HIWORD(pFileInfo->dwFileVersionMS);
					wBuf[1] = LOWORD(pFileInfo->dwFileVersionMS);
					wBuf[2] = HIWORD(pFileInfo->dwFileVersionLS);
					wBuf[3] = LOWORD(pFileInfo->dwFileVersionLS);

					swprintf_s(pBuffer, L"%d.%d.%d.%d",
						wBuf[0],
						wBuf[1],
						wBuf[2],
						wBuf[3]);

					sVersion = pBuffer;
					bRet = true;
				}
			}
				// Delete the data buffer
			else
			{
				sVersion = L"1.0.0.1";
				dwErr = GetLastError();
			}
			delete [] pData;
		}
		else
		{
			sVersion = L"1.0.0.1";
		}
		// Get rid of the allocated string buffer
		return bRet;
	}

	void GetProductVersion(std::wstring& sVersion)
	{
		UINT nItemLength;
		void* pData;
		DWORD dwInfoSize, dwHandle;
		wchar_t buff[MAX_PATH] = { 0 };

		::GetModuleFileNameW(NULL, buff, sizeof(buff));

		// Get File Version Info size
		dwInfoSize = ::GetFileVersionInfoSizeW(buff, &dwHandle);

		if (dwInfoSize > 0)
		{
			VS_FIXEDFILEINFO * pFileInfo;
			pData = new wchar_t[dwInfoSize];
			ZeroMemory(pData,dwInfoSize*sizeof(wchar_t));
			if (::GetFileVersionInfoW(buff, dwHandle, dwInfoSize, pData))
				if (::VerQueryValueW(pData, L"\\", (LPVOID*)&pFileInfo, &nItemLength))
				{
					swprintf_s(buff, L"%d.%d.%d.%d",
						pFileInfo->dwProductVersionMS >> 16,
						pFileInfo->dwProductVersionMS & 0xFFFF,
						pFileInfo->dwProductVersionLS >> 16,
						pFileInfo->dwProductVersionLS & 0xFFFF);

						sVersion = buff;
				}
				// Delete the data buffer
				delete [] pData;
		}
		else
		{
			sVersion = L"1.0.0.1";
		}

		return;
	}

	std::wstring GetFileDescription(LPCTSTR strFilePath)
	{
		std::wstring szVersion;

		//获取版本信息大小
		DWORD dwSize = GetFileVersionInfoSize(strFilePath, NULL);
		if (dwSize == 0)
			return L"";

		char *pBuf = new char[dwSize + 1];
		if (pBuf == NULL)
			return L"";

		memset(pBuf, 0, dwSize + 1);

		//获取版本信息
		DWORD dwRtn = GetFileVersionInfo(strFilePath, NULL, dwSize, pBuf);
		if (dwRtn == 0)
			goto cleanup;

		struct LANGANDCODEPAGE {
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;

		// Read the list of languages and code pages.
		UINT cbTranslate = 0;
		if (!VerQueryValue(pBuf, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &cbTranslate))
			goto cleanup;

		LPVOID lpBuffer = NULL;
		UINT uLen = 0;

		// Read the file description for each language and code page.
		for (size_t i = 0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++)
		{
			TCHAR szSubBlock[64];
			swprintf_s(szSubBlock, 64,
				TEXT("\\StringFileInfo\\%04x%04x\\FileDescription"),
				lpTranslate[i].wLanguage,
				lpTranslate[i].wCodePage);

			//// Retrieve file description for language and code page "i". 
			if (VerQueryValue(pBuf, szSubBlock, &lpBuffer, &uLen))
			{
				szVersion = (TCHAR*)lpBuffer;
				break;
			}
		}

	cleanup:
		if (pBuf != NULL)
			delete pBuf;

		return szVersion;
	}

	typedef struct tagSEARCHSDFILEPARAM
	{
		DWORD dwFlag;
		DWORD dwFuncID;

		union
		{
			struct COPYDIRPARAM
			{
				LPCTSTR lpszSrcPath;
				LPCTSTR lpszDesPath;
			}CopyDirParam;

			struct MOVEDIRPARAM
			{
				LPCTSTR lpszSrcPath;
				LPCTSTR lpszDesPath;
			}MoveDirParam;
		}param;
	}SEARCHSDFILEPARAM,*PSEARCHSDFILEPARAM;

	bool deleteDirectory(std::wstring srcDir)
	{
		HANDLE	handle; 
		WIN32_FIND_DATA    FindFileData;
		std::wstring srcTempDir=L"";

		if(srcDir[srcDir.length() - 1] == '\\')
		{
			srcDir = srcDir.substr(0,srcDir.length()-1);
		}

		srcTempDir = srcDir;
		srcTempDir.append(L"\\*");

		handle = FindFirstFile(srcTempDir.c_str(),&FindFileData); 
		if(handle == INVALID_HANDLE_VALUE) 
		{ 
			FindClose(handle);
			return  false; 
		}

		srcTempDir  = (srcDir  + L"\\");

		while(true) 
		{ 
			if(!FindNextFile(handle,&FindFileData))
			{
				break; 
			}

			if(wcscmp(FindFileData.cFileName,L".") != 0 && wcscmp(FindFileData.cFileName,L"..") !=0) 
			{ 
				if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) 
				{ 
					deleteDirectory(srcTempDir + FindFileData.cFileName);
				} 
				else 
				{ 
					DeleteFile((srcTempDir + FindFileData.cFileName).c_str());
				} 
			} 
		} 

		FindClose(handle);
		RemoveDirectory(srcDir.c_str());

		return true;
	}

	bool parseSeparator(const std::wstring& sSrc,wchar_t sTag,std::vector<std::wstring>& ret)
	{
		int iCount = (int)sSrc.size();
		if (iCount <=0)
			return false;

		int iCurIndex = 0;
		std::wstring sData=L"";

		while (iCurIndex < iCount)
		{
			wchar_t c = sSrc[iCurIndex];

			if (c == sTag)
			{
				ret.push_back(sData);
				sData=L"";
			}else
			{
				sData.push_back(c);
			}

			iCurIndex++;
		}

		if (!sData.empty())
		{
			ret.push_back(sData);
		}

		if (ret.size() != 0)
			return true;

		return false;
	}

	bool parseDNS(const std::wstring& sAddr,std::vector<std::wstring>& retVec)
	{
		std::string sUrl = ws2s(sAddr);
		WSADATA sockData;                           
		hostent *dnsInfo;                     
		
		if(WSAStartup(0x0101, &sockData) != 0)
		{
			WSACleanup();
			return false;
		}

		dnsInfo = gethostbyname(sUrl.c_str());

		if(!dnsInfo)
		{
			WSACleanup();
			return false;
		}

		for(int i = 0; dnsInfo->h_addr_list[i]; i++)
		{
			std::string s1 = inet_ntoa(*((struct in_addr *)dnsInfo->h_addr_list[i]));
			retVec.push_back(s2ws(s1));
		}

		WSACleanup();

		return true;
	}

	bool ReadKeyValueW(HKEY hKey,LPCWSTR lpSubKey,LPCWSTR lpValueName,DWORD dwType,LPBYTE lpValue,DWORD& dwLength,DWORD dwDesired)
	{
		bool bRet=false;

		HKEY hKEY=NULL;

		if (wcslen(lpValueName) < 1)
		{
			if(RegOpenKeyExW(hKey,lpSubKey,0,KEY_READ|dwDesired,&hKEY)!=ERROR_SUCCESS)
				goto ENDPOS;
		}
		else
		{
			if(RegOpenKeyExW(hKey,lpSubKey,0,KEY_QUERY_VALUE|KEY_READ|dwDesired,&hKEY)!=ERROR_SUCCESS)
				goto ENDPOS;

			if(RegQueryValueExW(hKEY,lpValueName,NULL,&dwType,lpValue,&dwLength)!=ERROR_SUCCESS)
				goto ENDPOS;
		}


		bRet=true;
ENDPOS:
		if(hKEY!=NULL)
			RegCloseKey(hKEY);

		return bRet;
	}


	bool ReadKeyValue64(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,__int64 *lpValue)
	{
		bool bRet=false;

		HKEY hKEY=NULL;
		DWORD dwValueLength=sizeof(__int64);
		DWORD dwType=REG_QWORD; 

	
		if(RegOpenKeyEx(hKey,lpSubKey,0,KEY_READ|KEY_WOW64_32KEY,&hKEY)!=ERROR_SUCCESS)
			goto ENDPOS;
		

		if(RegQueryValueEx(hKEY,lpValueName,NULL,&dwType,(LPBYTE)lpValue,&dwValueLength)!=ERROR_SUCCESS)
			goto ENDPOS;

		bRet=TRUE;

ENDPOS:

		if(hKEY!=NULL)
			RegCloseKey(hKEY);

		return bRet;
	}



    bool WriteKeyValueW(HKEY hKey,LPCWSTR lpSubKey,LPCWSTR lpValueName,DWORD dwType,const BYTE* lpValue,DWORD& dwLength,DWORD dwDesired)
    {
        bool bRet=false;

        HKEY hKEY=NULL;

        if(RegOpenKeyExW(hKey,lpSubKey,0,KEY_READ|KEY_WRITE|dwDesired,&hKEY)!=ERROR_SUCCESS)
            goto ENDPOS;


        if(RegSetValueExW(hKEY,lpValueName,NULL,dwType,lpValue,dwLength)!=ERROR_SUCCESS)
            goto ENDPOS;

        bRet=true;

ENDPOS:

        if(hKEY!=NULL)
            RegCloseKey(hKEY);

        return bRet;
    }


	bool getSignature2(const std::wstring& sExePath, std::wstring& retSignature)
	{
		HCERTSTORE hStore = nullptr;
		HCRYPTMSG hMsg = nullptr;
		PCMSG_SIGNER_INFO pSignerInfo = nullptr;
		PCMSG_SIGNER_INFO pCounterSignerInfo = nullptr;
		PCCERT_CONTEXT pCertContext = nullptr;
		LPTSTR szName = nullptr;

		bool bResult = false;

		do
		{
			BOOL fResult;
			DWORD dwEncoding, dwContentType, dwFormatType;

			DWORD dwSignerInfo;
			CERT_INFO CertInfo;

			fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE, sExePath.c_str(), CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED, CERT_QUERY_FORMAT_FLAG_BINARY,
				0, &dwEncoding, &dwContentType, &dwFormatType, &hStore, &hMsg, nullptr);

			if (!fResult)
				break;

			fResult = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, nullptr, &dwSignerInfo);
			if (!fResult)
				break;

			pSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);
			if (!pSignerInfo)
				break;

			fResult = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, (PVOID)pSignerInfo, &dwSignerInfo);
			if (!fResult)
				break;

			CertInfo.Issuer = pSignerInfo->Issuer;
			CertInfo.SerialNumber = pSignerInfo->SerialNumber;
			pCertContext = CertFindCertificateInStore(hStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_SUBJECT_CERT, (PVOID)&CertInfo, nullptr);

			if (!pCertContext)
				break;

			DWORD dwData;

			dwData = pCertContext->pCertInfo->SerialNumber.cbData;
			if (!(dwData = CertGetNameString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG, nullptr, nullptr, 0)))
				break;

			szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
			if (!szName)
				break;

			if (!(CertGetNameString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG, nullptr, szName, dwData)))
				break;

			LocalFree(szName);
			szName = nullptr;

			if (!(dwData = CertGetNameString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr, nullptr, 0)))
				break;

			szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
			if (!szName)
				break;

			if (!(CertGetNameString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr, szName, dwData)))
				break;

			retSignature = szName;

			bResult = true;

			break;

		} while (true);

		if (pSignerInfo != nullptr)
			LocalFree(pSignerInfo);

		if (pCounterSignerInfo != nullptr)
			LocalFree(pCounterSignerInfo);

		if (pCertContext != nullptr)
			CertFreeCertificateContext(pCertContext);

		if (hStore != nullptr)
			CertCloseStore(hStore, 0);

		if (hMsg != nullptr)
			CryptMsgClose(hMsg);

		return bResult;
	}

	bool bVerifyEmbeddedSignature(const std::wstring& sExePath, std::wstring& retSignature)
	{
		LONG lStatus;
		GUID WintrustVerifyGuid = WINTRUST_ACTION_GENERIC_VERIFY_V2;
		GUID DriverActionGuid = DRIVER_ACTION_VERIFY;
		HANDLE hFile;
		DWORD dwHash;
		BYTE bHash[100];
		HCATINFO hCatInfo;
		HCATADMIN hCatAdmin;

		WINTRUST_DATA wd = { 0 };
		WINTRUST_FILE_INFO wfi = { 0 };
		WINTRUST_CATALOG_INFO wci = { 0 };

		////set up structs to verify files with cert signatures
		memset(&wfi, 0, sizeof(wfi));
		wfi.cbStruct = sizeof(WINTRUST_FILE_INFO);
		wfi.pcwszFilePath = sExePath.c_str();
		wfi.hFile = NULL;
		wfi.pgKnownSubject = NULL;

		memset(&wd, 0, sizeof(wd));
		wd.cbStruct = sizeof(WINTRUST_DATA);
		wd.dwUnionChoice = WTD_CHOICE_FILE;
		wd.pFile = &wfi;
		wd.dwUIChoice = WTD_UI_NONE;
		wd.fdwRevocationChecks = WTD_REVOKE_NONE;
		wd.dwStateAction = 0;
		wd.dwProvFlags = WTD_SAFER_FLAG;
		wd.hWVTStateData = NULL;
		wd.pwszURLReference = NULL;
		wd.pPolicyCallbackData = NULL;
		wd.pSIPClientData = NULL;
		wd.dwUIContext = 0;

		lStatus = WinVerifyTrust(NULL, &WintrustVerifyGuid, &wd);

		////if failed, try to verify using catalog files
		if (lStatus != ERROR_SUCCESS)
		{
			//open the file
			hFile = CreateFileW(sExePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
				return FALSE;

			dwHash = sizeof(bHash);
			if (!CryptCATAdminCalcHashFromFileHandle(hFile, &dwHash, bHash, 0))
			{
				CloseHandle(hFile);
				return FALSE;
			}

			//Create a string form of the hash (used later in pszMemberTag)
			LPWSTR pszMemberTag = new WCHAR[dwHash * 2 + 1];
			// 				for (DWORD dw = 0; dw < dwHash; ++dw)
			// 				{
			// 					swprintf_s(&pszMemberTag[dw * 2], (dwHash * 2) + 1, L"%02X", bHash[dw]);
			// 				}

			for (unsigned int i = 0; i < dwHash; i++)
				StringCchPrintfW(&pszMemberTag[i * 2], (dwHash * 2) + 1, L"%02X", bHash[i]);

			if (!CryptCATAdminAcquireContext(&hCatAdmin, &DriverActionGuid, 0))
			{
				CloseHandle(hFile);
				return FALSE;
			}

			//find the catalog which contains the hash
			hCatInfo = CryptCATAdminEnumCatalogFromHash(hCatAdmin, bHash, dwHash, 0, NULL);

			if (hCatInfo)
			{
				CATALOG_INFO ci = { 0 };
				CryptCATCatalogInfoFromContext(hCatInfo, &ci, 0);

				memset(&wci, 0, sizeof(wci));
				wci.cbStruct = sizeof(WINTRUST_CATALOG_INFO);
				wci.pcwszCatalogFilePath = ci.wszCatalogFile;
				wci.pcwszMemberFilePath = sExePath.c_str();
				wci.pcwszMemberTag = pszMemberTag;

				memset(&wd, 0, sizeof(wd));
				wd.cbStruct = sizeof(WINTRUST_DATA);
				wd.dwUnionChoice = WTD_CHOICE_CATALOG;
				wd.pCatalog = &wci;
				wd.dwUIChoice = WTD_UI_NONE;
				wd.fdwRevocationChecks = WTD_STATEACTION_VERIFY;
				wd.dwProvFlags = 0;
				wd.hWVTStateData = NULL;
				wd.pwszURLReference = NULL;
				wd.pPolicyCallbackData = NULL;
				wd.pSIPClientData = NULL;
				wd.dwUIContext = 0;

				lStatus = WinVerifyTrust(NULL, &WintrustVerifyGuid, &wd);
				if (lStatus != 0 && wd.pCatalog->pcwszCatalogFilePath)
				{
					if (getSignature2(wd.pCatalog->pcwszCatalogFilePath, retSignature))
						lStatus = 0;
				}

				CryptCATAdminReleaseCatalogContext(hCatAdmin, hCatInfo, 0);
			}


			CryptCATAdminReleaseContext(hCatAdmin, 0);
			delete[] pszMemberTag;
			CloseHandle(hFile);
		}

		if (lStatus != ERROR_SUCCESS)
			return false;
		else
			return true;
	}

	int IsCharacterToStrip(int character)
	{
		return 0 == character || '\t' == character || '\n' == character || '\r' == character;
	}

	void StripString(std::wstring& stringArg)
	{
		stringArg.erase(remove_if(stringArg.begin(), stringArg.end(), IsCharacterToStrip), stringArg.end());
	}

	bool GetFileInfo(const std::wstring& szFilename, std::wstring& companyName/*, std::wstring& fileDescription, std::wstring& fileVersion, std::wstring& productVersion*/)
	{
		DWORD dummy;
		DWORD dwSize;
		LPVOID pvVariable = NULL;
		unsigned int iVariableLength = 0;
		unsigned int iTranslations = 0;
		WCHAR pszDest[256];

		struct LANGANDCODEPAGE
		{
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;

		//			fileDescription = L"";
		companyName = L"";

		dwSize = GetFileVersionInfoSize(szFilename.c_str(), &dummy);
		if (dwSize == 0)
			return false;

		std::vector<BYTE> data(dwSize);

		if (!GetFileVersionInfo(szFilename.c_str(), NULL, dwSize, &data[0]))
			return false;

		if (!VerQueryValue(&data[0], L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &iTranslations))
			return false;

		if (FAILED(StringCchPrintf(pszDest, 256, L"\\StringFileInfo\\%04x%04x\\FileDescription", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage)))
			return false;
		if (!VerQueryValue(&data[0], pszDest, &pvVariable, &iVariableLength))
			return false;
		//			fileDescription = std::wstring((WCHAR *)pvVariable, iVariableLength);
		//			StripString(fileDescription);

		if (FAILED(StringCchPrintf(pszDest, 256, L"\\StringFileInfo\\%04x%04x\\CompanyName", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage)))
			return false;
		if (!VerQueryValue(&data[0], pszDest, &pvVariable, &iVariableLength))
			return false;
		companyName = std::wstring((WCHAR *)pvVariable, iVariableLength);
		StripString(companyName);

		if (FAILED(StringCchPrintf(pszDest, 256, L"\\StringFileInfo\\%04x%04x\\FileVersion", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage)))
			return false;
		if (!VerQueryValue(&data[0], pszDest, &pvVariable, &iVariableLength))
			return false;
		//			fileVersion = std::wstring((WCHAR *)pvVariable, iVariableLength);
		//			StripString(fileVersion);

		if (FAILED(StringCchPrintf(pszDest, 256, L"\\StringFileInfo\\%04x%04x\\ProductVersion", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage)))
			return false;
		if (!VerQueryValue(&data[0], pszDest, &pvVariable, &iVariableLength))
			return false;
		//			productVersion = std::wstring((WCHAR *)pvVariable, iVariableLength);
		//			StripString(productVersion);

		return true;
	}

	std::wstring getBrowserPath()
	{
		HKEY hKey;
		wchar_t szBuffer[1080] = {0};
		wchar_t *next_token1 = NULL;
		unsigned long lSize = sizeof(wchar_t)*1080 + 1;
		if( RegOpenKeyEx( HKEY_CLASSES_ROOT, L"http\\shell\\open\\command", 0, KEY_READ, &hKey ) == 0 ) 
		{
			if( RegQueryValueEx( hKey, NULL, NULL, NULL, (unsigned char*)szBuffer, &lSize ) == 0 ) 
			{
				RegCloseKey( hKey ); 
				if (wcslen(szBuffer) == 0)
				{
					return L"";
				}
				return std::wstring(wcstok_s( szBuffer, L"\"",&next_token1));
			}

			RegCloseKey( hKey );
		}

		return L"";
	}

	std::wstring getFolder(LinkType type)
	{
		std::wstring path = L"";
		wchar_t buff[MAX_PATH] = {0};

		switch (type)
		{
		case LNKTYPE_DESKTOP:
			{
				::SHGetSpecialFolderPath(NULL, buff, CSIDL_DESKTOP, FALSE);
				path = buff;
			}
			break;
		case LNKTYPE_QUICKLAUNCH:
			{
				::SHGetSpecialFolderPath(NULL, buff, CSIDL_APPDATA, FALSE);

				OSVERSIONINFO osInfo = {0};
				osInfo.dwOSVersionInfoSize = sizeof(osInfo);
				::GetVersionEx(&osInfo);
				path = std::wstring(buff) + L"\\Microsoft\\Internet Explorer\\Quick Launch";
			}
			break;
		case LNKTYPE_STARTMENU:
			{
				::SHGetSpecialFolderPath(NULL, buff, CSIDL_PROGRAMS, FALSE);
				path = buff;
			}
			break;
		case LNKTYPE_ALLDESKTOP:
			{
				::SHGetSpecialFolderPath(NULL, buff, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
				path = buff;
			}
			break;
		case LNKTYPE_COMMON_PROGRAMS:
		   {
			    ::SHGetSpecialFolderPath(NULL, buff, CSIDL_COMMON_PROGRAMS, FALSE);
				path = buff;
		   }
		   break;
		case LNKTYPE_TASKBIN:
		   {
			   SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_DEFAULT, buff);
			   path = std::wstring(buff) + L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar";
		   }
		   break;
		default:
			break;
		}

		return path;
	}

	void createLinkIcon(const std::wstring& sDstFile,const std::wstring& sSavePath,const std::wstring& sParam,const std::wstring& sIcon,const std::wstring& sDec)
	{
		if(SUCCEEDED(::CoInitialize(NULL)))
		{
			IShellLink *pLink  = NULL;
			if(SUCCEEDED(::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,IID_IShellLink, (void **)&pLink)))
			{
				pLink->SetPath(sDstFile.c_str());
				pLink->SetArguments(sParam.c_str());

				if(!sDec.empty())
				{
					pLink->SetDescription(sDec.c_str());
				}

				if (!sIcon.empty())
				{
					pLink->SetIconLocation(sIcon.c_str(), 0);
				}
				else
				{
					pLink->SetIconLocation(sDstFile.c_str(), 0);
				}


				IPersistFile *pPersistFile = NULL;

				if(SUCCEEDED(pLink->QueryInterface(IID_IPersistFile, (void **)&pPersistFile)))
				{
					pPersistFile->Save(sSavePath.c_str(), TRUE);
					pPersistFile->Release();
				}

				pLink->Release();
			}

			::CoUninitialize();
		}
	}

	HRESULT createLinkIconEx(const std::wstring& sDstFile, const std::wstring& sSavePath, const std::wstring& sParam, const std::wstring& sIcon, const std::wstring& sDec, int nIndex)
	{
		HRESULT hr = S_FALSE;
		if (SUCCEEDED(::CoInitialize(NULL)))
		{
			IShellLink *pLink = NULL;
			if (SUCCEEDED(::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&pLink)))
			{
				hr = pLink->SetPath(sDstFile.c_str());
				hr = pLink->SetArguments(sParam.c_str());

				if (!sDec.empty())
				{
					hr = pLink->SetDescription(sDec.c_str());
				}

				if (!sIcon.empty())
				{
					hr = pLink->SetIconLocation(sIcon.c_str(), 0);
				}
				else
				{
					hr = pLink->SetIconLocation(sDstFile.c_str(), nIndex);
				}


				IPersistFile *pPersistFile = NULL;

				if (SUCCEEDED(pLink->QueryInterface(IID_IPersistFile, (void **)&pPersistFile)))
				{
					hr = pPersistFile->Save(sSavePath.c_str(), TRUE);
					pPersistFile->Release();
				}

				pLink->Release();
			}

			::CoUninitialize();
		}
		return hr;
	}


	void FormatA(std::string& sbuffer,LPCSTR lpszFormat,...)
	{
		char tcBuf[MAX_FORMAT_SINGLEBUFFER] = {0};

		va_list arglist;
		va_start(arglist,lpszFormat);

		vsprintf_s(tcBuf,lpszFormat,arglist);
		sbuffer = tcBuf;

	}

	void FormatW(std::wstring& swbuffer,LPCWSTR lpszFormat,...)
	{
		wchar_t tcBuf[MAX_FORMAT_SINGLEBUFFER] = {0};

		va_list arglist;
		va_start(arglist,lpszFormat);

		vswprintf_s(tcBuf,lpszFormat,arglist);
		swbuffer = tcBuf;
	}


	void LTrim(std::string& str) 
	{ 
		if (str.find_first_not_of(" \n\r\t")!=std::string::npos){
			str = str.substr(str.find_first_not_of(" \n\r\t"));
		}
		else
			str.clear();
	} 

	void RTrim(std::string& str) 
	{ 
		if (str.find_first_not_of(" \n\r\t")!=std::string::npos){
			str = str.substr(0,str.find_last_not_of(" \n\r\t")+1); 
		}
		else
			str.clear();
	} 


	void LTrim(std::wstring& str) 
	{ 
		if (str.find_first_not_of(L" \n\r\t")!=std::wstring::npos){
			str = str.substr(str.find_first_not_of(L" \n\r\t"));
		}
		else
			str.clear();
	} 

	void RTrim(std::wstring& str) 
	{ 
		if (str.find_first_not_of(L" \n\r\t") != std::wstring::npos){
			str = str.substr(0, str.find_last_not_of(L" \n\r\t") + 1);
		}
		else
			str.clear();
	} 


	void trim(std::string& str) 
	{ 
		LTrim(str); 
		RTrim(str);
	}

	void trim(std::wstring& str)
	{
		LTrim(str); 
		RTrim(str);
	}


    // stringתСд
    _tstring strToLower(const _tstring &str)
    {
        _tstring strTmp = str;
        transform(strTmp.begin(),strTmp.end(),strTmp.begin(),tolower);
        return strTmp;
    }

    // string.compareNoCase
    int compareNoCase(const _tstring &strA,const _tstring &strB)
    {
        _tstring str1 = strToLower(strA);
        _tstring str2 = strToLower(strB);
        return str1.compare(str2);
    }


    DWORD CharToNum(TCHAR c)
    {
        if(c>=L'0' && c<=L'9')
            return c- L'0';
        else if(c>=L'a' && c<= L'f')
            return c- L'a'+10;
        else if(c>= L'A' && c<= L'F')
            return c-L'A'+10;

        return 0;
    }

    DWORD StringToNum(wchar_t *sz)
    {
        DWORD dwRet=0;
        for(DWORD i=0;i<wcslen(sz);i++)
            dwRet=(dwRet<<4)+CharToNum(sz[i]);

        return dwRet;
    }

    BOOL CreateSecurityAttributes(PSECURITY_ATTRIBUTES psa,DWORD dwFlag)
    {
        if(psa==NULL)
            return FALSE;

        psa->lpSecurityDescriptor=NULL;
        PSECURITY_DESCRIPTOR pSD=NULL;
        if(dwFlag & MCE_NORMALACCESS)
        {
            pSD=(PSECURITY_DESCRIPTOR)LocalAlloc(LPTR,SECURITY_DESCRIPTOR_MIN_LENGTH);
            if(pSD==NULL)
                goto ENDPOS; 

            if(!InitializeSecurityDescriptor(pSD,SECURITY_DESCRIPTOR_REVISION))
                goto ENDPOS; 

            if(!SetSecurityDescriptorDacl(pSD,TRUE,(PACL)NULL,FALSE))
                goto ENDPOS; 

            psa->lpSecurityDescriptor=pSD;
        }

        psa->bInheritHandle=(dwFlag & MCE_INHERITHANDLE) ? TRUE : FALSE;
        psa->nLength=sizeof(SECURITY_ATTRIBUTES);

        return TRUE;

ENDPOS:
        if(pSD!=NULL)
            LocalFree(pSD);
        psa->lpSecurityDescriptor=NULL;

        return FALSE;
    }

    BOOL CleanSecurityAttributes(PSECURITY_ATTRIBUTES psa)
    {
        if(psa!=NULL && psa->lpSecurityDescriptor!=NULL)
        {
            LocalFree(psa->lpSecurityDescriptor);
            psa->lpSecurityDescriptor=NULL;
        }

        return TRUE;
    }

    HANDLE WINAPI MyCreateEvent(DWORD dwFlag,BOOL bManualReset,BOOL bInitialState,LPCTSTR lpName)
    {
        HANDLE hHandle=NULL;

        SECURITY_ATTRIBUTES sa;
        if(CreateSecurityAttributes(&sa,dwFlag))
        {
            hHandle=CreateEvent(&sa,bManualReset,bInitialState,lpName);

            CleanSecurityAttributes(&sa);
        }

        return hHandle;
    }

    HANDLE WINAPI MyCreateMutex(DWORD dwFlag,BOOL bInitialOwner,LPCTSTR lpName)
    {
        HANDLE hHandle=NULL;

        SECURITY_ATTRIBUTES sa;
        if(CreateSecurityAttributes(&sa,dwFlag))
        {
            hHandle=CreateMutex(&sa,bInitialOwner,lpName);

            CleanSecurityAttributes(&sa);
        }

        return hHandle;
    }

	void copyDir(const std::wstring& wsDestPath, const std::wstring& wsSourPath)
	{
		WIN32_FIND_DATA  FindData;
		HANDLE hFind = NULL;
		std::wstring wsTempDestPath = wsDestPath;
		std::wstring wsTempSourPath = wsSourPath;
		std::wstring wsTempFindPath = wsSourPath;
		BOOL bFind = true;
		wsTempFindPath += L"\\*.*";
		wsTempDestPath += L"\\";
		wsTempSourPath += L"\\";
		hFind = FindFirstFile(wsTempFindPath.c_str(), &FindData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return;
		}
		while (bFind)
		{
			if (_wcsicmp(L".", FindData.cFileName) != 0 && _wcsicmp(L"..", FindData.cFileName) != 0)
			{
				if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				{
					if (_wcsicmp(L".", FindData.cFileName) != 0 && _wcsicmp(L"..", FindData.cFileName) != 0)
					{
						std::wstring wsNewPath = wsTempDestPath + (std::wstring)FindData.cFileName;
						CreateDirectory(wsNewPath.c_str(), NULL);
						copyDir(wsNewPath, wsTempSourPath + (std::wstring)FindData.cFileName);
					}
				}
				else
				{
					std::wstring wsNewSour = wsTempSourPath + (std::wstring)FindData.cFileName;
					std::wstring wsNewDest = wsTempDestPath + (std::wstring)FindData.cFileName;

					if (!CopyFile(wsNewSour.c_str(), wsNewDest.c_str(), false))
					{
						DWORD dw = GetTickCount();
						wchar_t buf[50] = { 0 };
						swprintf_s(buf, L"%d", dw);
						std::wstring s = wsNewDest + std::wstring(buf);
						MoveFileEx(wsNewDest.c_str(), s.c_str(), MOVEFILE_REPLACE_EXISTING);
						MoveFileEx(s.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
						CopyFile(wsNewSour.c_str(), wsNewDest.c_str(), false);
					}
				}
			}

			bFind = FindNextFile(hFind, &FindData);
		}
		if (hFind != NULL)
		{
			FindClose(hFind);
			hFind = NULL;
		}
	}

	

}}