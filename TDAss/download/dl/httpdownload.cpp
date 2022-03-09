/************************************************************************/
/* 
	Date : 4/2/2014
    File : httpdownload.cpp
    Author : Eadwin Li
    Function : 
    Version : 1.0
    Copyright (c) 2012 - 2014 OASGAME Studio
*/
/************************************************************************/  

#include "HttpDownload.h"
#include "url.h"
#include <assert.h>
#include <shlwapi.h>
#include "../../JTerseUtilities.h"
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib")
#pragma warning(disable:4996)

#define __HTTP_VERB_GET					 L"GET"
#define __HTTP_VERB_POST                 L"POST"
#define __HTTP_ACCEPT_TYPE				 L"*/*"
#define __HTTP_POST_HEAD_FLAGS           L"InPost="
#define	__HTTP_POST_CONTENT_TYPE         L"Content-Type: application/x-www-form-urlencoded"

#define __HTTP_HEADER_BUF_SIZE			64
#define __HTTP_LOCATION_BUF_SIZE		1024
#define __HTTP_LASTMODIFIED_BUF_SIZE	64
#define __HTTP_READ_BUF_SIZE			(4*1024)

namespace net
{
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    BOOL GetIEVersion(LPTSTR lpszVersion, UINT uSize)
    {
        //////////////////////////////////////////////////////////////////////////
        wchar_t szFileName[MAX_PATH] = { 0 };
        HMODULE hModule = ::GetModuleHandle(L"WININET.DLL");
        DWORD dwHandle = 0;
        if (hModule == 0)
            return FALSE;
        DWORD dwLen = GetModuleFileName(hModule, szFileName, sizeof(szFileName) / sizeof(szFileName[0]));
        if (dwLen <= 0)
            return FALSE;

        dwLen = GetFileVersionInfoSize(szFileName, &dwHandle);
        if (dwLen <= 0)
            return FALSE;
        BYTE *pVersionInfo = new BYTE[dwLen];
        if (!::GetFileVersionInfo(szFileName, dwHandle, dwLen, pVersionInfo))
        {
            delete pVersionInfo;
            return FALSE;
        }

        VS_FIXEDFILEINFO *pFixed = NULL;
        UINT uLen = 0;
        if (!VerQueryValue(pVersionInfo, L"\\", (LPVOID*)&pFixed, &uLen))
        {
            delete pVersionInfo;

            return FALSE;
        }

        //wsprintf(lpszVersion, _T("MSIE %d.%d"), verInfo.dwMajorVersion, verInfo.dwMinorVersion);
        wsprintf(lpszVersion,
            L"MSIE %d.%d", 
            HIWORD(pFixed->dwFileVersionMS/*dwProductVersionMS*/),
            LOWORD(pFixed->dwFileVersionMS/*dwProductVersionMS*/));	

        delete pVersionInfo;

        return TRUE;
    }

    BOOL GetOSVersion(LPTSTR lpszVersion, UINT uSize)
    {
        OSVERSIONINFOEX osvi;
        BOOL bOsVersionInfoEx;

        // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
        // If that fails, try using the OSVERSIONINFO structure.

        ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

        if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
        {
            osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
            if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
            {
                *lpszVersion = L'\0';
                return FALSE;
            }
        }

        switch (osvi.dwPlatformId)
        {
            // Test for the Windows NT product family.
        case VER_PLATFORM_WIN32_NT:
            {
                wsprintf(lpszVersion, L"Windows NT %d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
            }

            break;

            // Test for the Windows 95 product family.
        case VER_PLATFORM_WIN32_WINDOWS:

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
            {
                wsprintf(lpszVersion, L"Windows 95");

            } 

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
            {
                wsprintf(lpszVersion, L"Windows 98");
            } 

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
            {
                wsprintf(lpszVersion, L"Windows Me");
            } 
            break;

        case VER_PLATFORM_WIN32s:

            wsprintf(lpszVersion, L"Windows");
            break;
        }
        return TRUE; 

    }
    CHttpDownload::CHttpDownload()
    {
        m_hInternet				= NULL;
        m_hConnect				= NULL;
        m_hRequest				= NULL;

        m_bServerAlter			= FALSE;
        m_bAuthorization		= FALSE;
        m_bProxyAuthorization	= FALSE;
        m_bUseProxy				= FALSE;
        m_bStop					= FALSE;

        m_nServerPort			= INTERNET_DEFAULT_HTTP_PORT;
        m_nProxyPort			= 0;
        m_dwLastError			= S_OK;
        m_Observer			    = NULL;
        m_nConnectType          = CONNECT_TYPE_IE;
        m_dwFileSize			= 0;
        m_dlstate               = DL_PREPARE;
		rate_					= 0.0;

        // _T("Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)")
        TCHAR szIEVer[MAX_PATH] = { 0 };
        GetIEVersion(szIEVer, MAX_PATH);
        TCHAR szOSVer[MAX_PATH] = { 0 };
        GetOSVersion(szOSVer, MAX_PATH);
        JTerse::core::FormatW(m_strUserAgent,L"Mozilla/4.0 (compatible; %s; %s; Rising)", szIEVer, szOSVer);
    }

    CHttpDownload::~CHttpDownload()
    {
        CloseHandles();
    }


    //////////////////////////////////////////////////////////////////////////////////
    //	��������BOOL Connect()
    //	��  ;����ʼ��������session��Http���ӣ��ڲ�ʹ�ã�
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ������
    //	����ֵ���ɹ���TRUE��ʧ��FALSE
    /////////////////////////////////////////////////////////////////////////////////
    BOOL CHttpDownload::Connect()
    {
        //DWORD dwTimeOut = 30*1000;
		DWORD dwTimeOut = m_dwTimeOut;

        if (!m_hInternet)
        {
            if (m_bUseProxy)
            {
                // ��pszProxyName��lpszProxyBypass Ϊ��IE7�᷵��ʧ��
                m_hInternet = InternetOpen(m_strUserAgent.data(), INTERNET_OPEN_TYPE_PROXY, TEXT("proxy"), TEXT("<local>"), 0L);
            }
            else
                m_hInternet = InternetOpen(m_strUserAgent.data(), m_nConnectType, NULL,	NULL, 0L);

            if (NULL == m_hInternet)
            {
                m_dwLastError = ::GetLastError();
                return FALSE;
            }

            INTERNET_PROXY_INFO proxyInfo;
            ZeroMemory(&proxyInfo, sizeof(INTERNET_PROXY_INFO));
            TCHAR szProxy[MAX_PATH] = { 0 };
            if (m_bUseProxy)
            {				
                proxyInfo.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
                wsprintf(szProxy, L"%s:%d", m_strProxyServer, m_nProxyPort );
                proxyInfo.lpszProxy = szProxy;

                if (!InternetSetOption(m_hInternet, INTERNET_OPTION_PROXY, &proxyInfo, sizeof(INTERNET_PROXY_INFO)))
                {
                    m_dwLastError = ::GetLastError();
                    CloseHandles();
                    return FALSE;
                }
            }

            if (!InternetSetOption(m_hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD)))
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return FALSE;
            }
        }

        if (!m_hConnect)
        {
            if (!m_bAuthorization)
            {
                m_hConnect = InternetConnect(m_hInternet,
                    m_strServerName.data(),
                    m_nServerPort,
                    NULL,
                    NULL,
                    INTERNET_SERVICE_HTTP,
                    0L,
                    NULL);
            }
            else
            {
                m_hConnect = InternetConnect(m_hInternet,
                    m_strServerName.data(),
                    m_nServerPort,
                    m_strUsername.data(),
                    m_strPassword.data(),
                    INTERNET_SERVICE_HTTP,
                    0L,
                    NULL);
            }

            if (NULL == m_hConnect)
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return FALSE;
            }

            if (ERROR_SUCCESS != ::InternetAttemptConnect(NULL))
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return FALSE;
            }

            if (!InternetSetOption(m_hConnect, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD)))
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return FALSE;
            }

            if (!InternetSetOption(m_hConnect, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD)))
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return FALSE;
            }

            if (m_bProxyAuthorization)
            {
                if (!InternetSetOption(m_hConnect, 
                    INTERNET_OPTION_PROXY_USERNAME,
                    (LPVOID)(LPCTSTR)m_strProxyUsername.data(),
                    m_strProxyUsername.length() + 1))
                {
                    m_dwLastError = ::GetLastError();
                    CloseHandles();
                    return FALSE;
                }

                if (!InternetSetOption(m_hConnect, 
                    INTERNET_OPTION_PROXY_PASSWORD,
                    (LPVOID)(LPCTSTR)m_strProxyPassword.data(),
                    m_strProxyPassword.length() + 1))
                {
                    m_dwLastError = ::GetLastError();
                    CloseHandles();
                    return FALSE;
                }
            }
        }

        return TRUE;
    }

    //////////////////////////////////////////////////////////////////////////////////
    //	��������BOOL _CreateDirectory(LPCTSTR lpFileName,
    //					   BOOL    bIsFile,
    //					   LPSECURITY_ATTRIBUTES lpSecurityAttributes)
    //	��  ;�������༶·��(�ڲ�ʹ��)
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ����
    //		lpFileName		: ��������Ŀ¼
    //		bIsFile			: lpFileName�Ƿ�ΪĿ¼�����ļ���TRUEΪ�ļ�����FALSEΪĿ¼��
    //      lpSecurityAttributes : NULL
    //	����ֵ���ɹ���TRUE��ʧ��FALSE
    /////////////////////////////////////////////////////////////////////////////////
    BOOL _CreateDirectory(LPCTSTR lpFileName,
        BOOL    bIsFile,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes)
    {

        BOOL	fRet = TRUE;
        TCHAR	szFileName[MAX_PATH] = { 0 };
        TCHAR*	lpChr = NULL;
        TCHAR*	lpHead = NULL;

        assert(lpFileName != NULL && *lpFileName != L'\0');
        if (lpFileName == NULL || lstrlen(lpFileName) <= 0)
            return FALSE;

        if (lstrlen(lpFileName) > sizeof(szFileName))
        {
            SetLastError(ERROR_BUFFER_OVERFLOW);
            return FALSE;
        }

        lstrcpyn(szFileName, lpFileName, sizeof(szFileName)-1);
        if (bIsFile)
            PathRemoveFileSpec(szFileName);
        lpHead = PathSkipRoot(szFileName);
        if (lstrlen(lpHead) == 0)
        {
            CreateDirectory(szFileName, lpSecurityAttributes);
            return TRUE;
        }

        while (true)
        {
            lpChr = wcschr(lpHead, L'\\');
            if (NULL == lpChr)
            {
                fRet &= CreateDirectory(szFileName, lpSecurityAttributes);
                if (!fRet && (GetLastError() == ERROR_ALREADY_EXISTS))
                    fRet = TRUE;
                break;
            }
            else
            {
                *lpChr = NULL;
                fRet &= CreateDirectory(szFileName, lpSecurityAttributes);
                if (!fRet && (GetLastError() == ERROR_ALREADY_EXISTS))
                    fRet = TRUE;
                *lpChr = L'\\';
                lpHead = lpChr+1;
            }
        }
        return fRet;
    }


    //////////////////////////////////////////////////////////////////////////////////
    //	��������HRESULT Download(LPCTSTR lpszURL,
    //				LPCTSTR lpszSaveFileName	/*= NULL*/,
    //				BOOL bAutoRedirect /*= FALSE*/)
    //	��  ;��������ڣ����ô˺�������ʼ���ع���
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ����
    //		lpszDownloadUrl   : �����ص�����URL
    //		lpszSaveFileName  : ���ر����ļ���(��Ҫ�õ��ض���URLʱ����Ϊ·��)	
    //      bAutoRedirect     : �Ƿ��Զ��ض���
    //		lpszResetHost	  : [in]�����Ҫ����HTTPЭ��ͷ��Hostʱ���˲�����Ч��
    //			ΪNULL��ʾ����Ҫ����HTTPЭ��ͷ��Host�����lpszURL��ָ�����Ƿ�����
    //			��IP��ַʱ���õ��������
    //	����ֵ��HRESULT
    //		ERR_HTTP_USER_STOP		: �û�ֹͣ����(������StopDownload����)
    //		ERR_HTTP_FAIL			: ����ʧ��
    //		ERR_HTTP_SUCCESS		: ���سɹ�
    //		ERR_HTTP_REDIRECT		: �ض���HTTP
    //		ERR_HTTP_INVALID_URL	: ��ЧURL
    //      ERR_HTTP_DEST_FILENAME_EMPTY : ���ش洢�ļ�Ϊ��
    //      ERR_HTTP_CREATE_FILE_FAIL : ���������ļ�ʧ��
    //      ERR_HTTP_RENAME_FILE_FAIL : �����������ļ�ʧ��
    //      ERR_HTTP_SEND_TIMEOUT     : ����ʱ
    /////////////////////////////////////////////////////////////////////////////////
    bool CHttpDownload::Download( LPCTSTR lpszURL, LPCTSTR lpszSaveFileName /* = NULL */, 
        BOOL bAutoRedirect /*= FALSE*/, LPCTSTR lpszResetHost/* = NULL*/)
    {
        // 	_ASSERT(pData);
        // 	_ASSERT(lpszURL);
		if (lpszURL == NULL || lpszSaveFileName == NULL)
        {
            m_dwLastError = ERR_HTTP_URL_OR_DATA_NULL;
            return false;
        }
        m_strRedirectURL.clear();		// ����ض����ַ

        // �õ����ص���ʱ�ļ�������ԭ���ļ��Ļ����ϼ�.rs

        _tstring strTmpFile,strDlFile;
		strDlFile = lpszSaveFileName;

		if (NULL != lpszSaveFileName && strDlFile.rfind(L"\\") != strDlFile.length()-1)
        {
			if (m_bStop)
			{
				strTmpFile = L"";
			}else
			{
				 strTmpFile = lpszSaveFileName;
			}

			if (!strTmpFile.empty())
			{
				strTmpFile += L".rs";
			}
        }
        

        // ����URL�Ƿ�Ϸ�
        if (!ParseUrl(lpszURL))
        {
            m_dwLastError = ERR_HTTP_INVALID_URL;
            return false;
        }

        if (m_bServerAlter)
            CloseHandles();

        // ��������ֹ
        if (m_bStop)
        {
            CloseHandles();
            m_dwLastError = ERR_HTTP_USER_STOP;
            return false;
        }

        // ��ʼ����Ϣ�ṹ
        m_httpDownInfo.dwFileSize = 0;
        m_httpDownInfo.dwFileDownloadedSize = 0;
        m_httpDownInfo.strFileName = m_strObjectName.substr(m_strObjectName.rfind(L'/') + 1);

        // ����ô��ڷ��������ļ�����Ϣ
        if (!strTmpFile.empty())
        {
            Notify(DL_PREPARE);
        }

        // ��ʼ��WinINet�������������
        if (!Connect())
        {
            m_dwLastError = ERR_HTTP_FAIL;
            return false;
        }

        // ��������ֹ
        if (m_bStop)
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }

        static LPCTSTR szAcceptType[]= { __HTTP_ACCEPT_TYPE, 0 };
        DWORD dwOpenFlags = INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_KEEP_CONNECTION |INTERNET_FLAG_RELOAD;

        if (!bAutoRedirect)
            dwOpenFlags |= INTERNET_FLAG_NO_AUTO_REDIRECT;

        // Open Request
        m_hRequest = HttpOpenRequest(m_hConnect, __HTTP_VERB_GET,
            m_strObjectName.data(), HTTP_VERSION, NULL,
            szAcceptType, dwOpenFlags, NULL);
        // ERROR return
        if (NULL == m_hRequest)
        {
            m_dwLastError = ::GetLastError();
            CloseHandles();
            return false;
        }

        wchar_t szHost[272] = {0};
        if (lpszResetHost != NULL) 
        {
            // �滻Httpͷ��Hostֵ
            lstrcpy(szHost, L"Host: ");
            lstrcat(szHost, lpszResetHost);
            //lstrcat(szHost, "\r\n");
            HttpAddRequestHeaders(m_hRequest, szHost, -1, HTTP_ADDREQ_FLAG_ADD );
        }

        TCHAR szHeader[__HTTP_HEADER_BUF_SIZE+1] = { 0 };
        BOOL bSendRange = FALSE;

        SYSTEMTIME stLocalFile;						// ���ش��ڴ������ļ�ʱ��
        SYSTEMTIME stNetFile;						// �������������ļ�ʱ��

        // Ĭ��ȡ��ǰʱ��
        GetLocalTime(&stNetFile);

        // �����ش��ڴ�������ʱ�ļ�����õ��ļ���Сд��Htppͷ��Range��
        HANDLE hFileDown = NULL;
        if (!strTmpFile.empty())
        {
            WIN32_FIND_DATA finder = {0};
            HANDLE hFileDown = FindFirstFile(strTmpFile.data(), &finder);
            if (hFileDown != INVALID_HANDLE_VALUE)
            {
                FindClose(hFileDown);
                hFileDown = NULL;
                FileTimeToSystemTime(&finder.ftLastWriteTime, &stLocalFile);

                m_httpDownInfo.dwFileDownloadedSize  = ((DWORDLONG)finder.nFileSizeHigh << 32) | (DWORDLONG)finder.nFileSizeLow;

                if (m_httpDownInfo.dwFileDownloadedSize > 0 )
                {
                    _tstring strSize;
                    JTerse::core::FormatW(strSize,L"%llu", m_httpDownInfo.dwFileDownloadedSize);
                    wsprintf(szHeader, L"Range: bytes=%s-", strSize.data());
                    bSendRange = TRUE;
                }
                else
                {
                    SetFileAttributes(strTmpFile.data(), FILE_ATTRIBUTE_NORMAL);
                    DeleteFile(strTmpFile.data());
                }
            }	
        }

        // ��������ֹ
        if (m_bStop)
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }

        // SEND REQUEST
        if (!::HttpSendRequest(m_hRequest, szHeader, lstrlen(szHeader),	NULL, 0))
        {
            m_dwLastError = ::GetLastError();
            CloseHandles();	
            return false;
        }	

        // ��������ֹ
        if (m_bStop)
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }

        DWORD dwCode = 0;
        DWORD dwSize = sizeof(DWORD) ;
        if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL))
        {
            m_dwLastError = ::GetLastError();
            CloseHandles();
            return false;
        }

        BOOL bSendAgain = FALSE;
        switch (dwCode)
        {
            // �ض���
        case HTTP_STATUS_REDIRECT:
        case HTTP_STATUS_MOVED:
            {
                TCHAR* pszLocation = new TCHAR[__HTTP_LOCATION_BUF_SIZE + 1];
                ZeroMemory(pszLocation, __HTTP_LOCATION_BUF_SIZE +1 );
                dwSize = __HTTP_LOCATION_BUF_SIZE;
                if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_LOCATION, pszLocation, &dwSize, NULL))
                {
                    delete pszLocation;
                    m_dwLastError = ::GetLastError();
                    CloseHandles();

                    return false;
                }

                m_strRedirectURL = pszLocation;

                m_dwLastError = ERR_HTTP_REDIRECT;
                CloseHandles();
                delete pszLocation;

                return false;
            }
            // 200 OK
        case HTTP_STATUS_OK:
            {
                if (strTmpFile.empty())
                {
                    m_dwLastError = ERR_HTTP_DEST_FILENAME_EMPTY;
                    CloseHandles();
                    return false;
                }
                else
                {
                    // ����ɾ���ϴ������жϵ��ļ�
                    DeleteFile(strTmpFile.data());
                }
            }
            break;
        case HTTP_STATUS_NOT_FOUND:
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return false;
            }
            break;

        case HTTP_STATUS_FORBIDDEN:
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return false;
            }
            // �ϵ����󷵻�
        case HTTP_STATUS_PARTIAL:
        case HTTP_STATUS_PARTIAL_CONTENT:
            {
                TCHAR szLastModified[__HTTP_LASTMODIFIED_BUF_SIZE + 1] = { 0 };
                dwSize = __HTTP_LASTMODIFIED_BUF_SIZE;
                if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_LAST_MODIFIED, szLastModified, &dwSize, NULL))
                {
                    m_dwLastError = ::GetLastError();
                    DeleteFile(strTmpFile.data());
                    bSendAgain = TRUE;
                    m_httpDownInfo.dwFileDownloadedSize = 0;
                    CloseHandles();
                    return false;
                }
                HttpDateToSystemTime(szLastModified, stNetFile);

                if (stNetFile.wSecond != stLocalFile.wSecond
                    || stNetFile.wMinute != stLocalFile.wMinute
                    || stNetFile.wHour != stLocalFile.wHour
                    || stNetFile.wDay != stLocalFile.wDay
                    || stNetFile.wMonth != stLocalFile.wMonth
                    || stNetFile.wYear != stLocalFile.wYear)
                {
                    // ɾ���ϴ������жϵ��ļ�
                    DeleteFile(strTmpFile.data());
                    bSendAgain = TRUE;
                    m_httpDownInfo.dwFileDownloadedSize = 0;
                }
            }
            break;
        case HTTP_STATUS_SERVER_ERROR:
            {
                m_dwLastError = ::GetLastError();
                return false;
            }		
            break;
        default:
            if (bSendRange)
            {
                bSendAgain = TRUE;
                break;
            }
            else
            {
                m_dwLastError = ::GetLastError();
                return false;
            }
        }

        // ��������ֹ
        if (m_bStop)
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }

        // ���·�������
        if (bSendAgain)
        {
            InternetCloseHandle(m_hRequest);
            m_hRequest = HttpOpenRequest(m_hConnect, __HTTP_VERB_GET,
                m_strObjectName.data(), HTTP_VERSION, NULL,
                szAcceptType, dwOpenFlags, NULL);		
            if (NULL == m_hRequest)
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return false;
            }

            if (lpszResetHost != NULL)
                HttpAddRequestHeaders(m_hRequest, szHost, -1, HTTP_ADDREQ_FLAG_COALESCE_WITH_SEMICOLON);

            if (m_bStop)
            {
                m_dwLastError = ERR_HTTP_USER_STOP;
                CloseHandles();
                return false;
            }
            // SEND REQUEST
            if (!::HttpSendRequest(m_hRequest, NULL, 0L, NULL,0L))
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return false;
            }

            if (m_bStop)
            {
                m_dwLastError = ERR_HTTP_USER_STOP;
                CloseHandles();
                return false;
            }

            dwSize = sizeof(DWORD);
            if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL))
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return false;
            }

            if (dwCode != HTTP_STATUS_OK)
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return false;
            }
        }

        TCHAR szLastModified[__HTTP_LASTMODIFIED_BUF_SIZE + 1] = { 0 };
        dwSize = __HTTP_LASTMODIFIED_BUF_SIZE;
        // �õ��������ļ�������޸�ʱ��,���ܻ�ò���
        if (HttpQueryInfo(m_hRequest, HTTP_QUERY_LAST_MODIFIED, szLastModified, &dwSize, NULL))
        {
            HttpDateToSystemTime(szLastModified, stNetFile);
        }

        DWORDLONG dwContentLength = 0;
        dwSize = sizeof(DWORDLONG);
        BOOL bGotFileSize = TRUE;

        // �õ��������ļ��Ĵ�С
        if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwContentLength, &dwSize, NULL))
        {
            // ��Щ���������������ش��󣬵����������ļ�����ʱĬ��200K
            dwContentLength = 200 * 1024;
            m_httpDownInfo.dwFileDownloadedSize = 0;
            bGotFileSize = FALSE;
        }

        //httpDown.dwFileSize = dwContentLength;
        // �̵������õ��Ĵ�СΪʣ�µĴ�С
        m_httpDownInfo.dwFileSize = dwContentLength + m_httpDownInfo.dwFileDownloadedSize;
        m_dwFileSize = m_httpDownInfo.dwFileSize;

        // �����ش��ڷ��������ļ���С��Ϣ
        if (!strTmpFile.empty())
        {
            m_dlstate = DL_START;
            BeginWork();
            Notify(m_dlstate);
        }

        if (m_bStop)
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }
        // ��������Ŀ¼
        if (FALSE == _CreateDirectory(strTmpFile.data(), TRUE, NULL))
        {
            m_dwLastError = GetLastError();
            CloseHandles();
            return false;
        }
        // ����������ʱ�ļ�
        hFileDown = CreateFile(strTmpFile.data(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFileDown == INVALID_HANDLE_VALUE)
        {
            m_dwLastError = GetLastError();
            CloseHandles();
            return false;
        }

        /*if (pData)
        {
        pData->SetTmpFile(strTmpFile);
        }*/

        FILETIME ftWrite;			
        SystemTimeToFileTime(&stNetFile, &ftWrite);
        SetFileTime(hFileDown, NULL, NULL, &ftWrite);

        if (bSendAgain)
        {
            SetFilePointer(hFileDown, 0L, 0L, FILE_BEGIN);
            SetEndOfFile(hFileDown);
        }
        else
        {
            SetFilePointer(hFileDown, 0L, 0L, FILE_END);
        }

        // �����ļ�дʱ��
        CHAR* pszData = new CHAR[__HTTP_READ_BUF_SIZE + 1]; // ���ļ�������
        DWORD dwRead = 0;

        //httpDown.nStatusType = DOWNLOAD_STATUS_DOWNLOADEDSIZE;

        DWORD dwStartTicks = ::GetTickCount();
        DWORD dwCurrentTicks = dwStartTicks;

        DWORDLONG dwLastTotalBytes = m_httpDownInfo.dwFileDownloadedSize;
        DWORD dwLastPercentage = 0;
        do 
        {
            ZeroMemory(pszData, __HTTP_READ_BUF_SIZE + 1);
            if (m_bStop)
            {
                // ��������ֹ������
                delete pszData;
                pszData = NULL;
                CloseHandles();

                FlushFileBuffers(hFileDown);
                SetEndOfFile(hFileDown);

                // ���������ص��ļ����޸�ʱ���޸�Ϊ��������ļ�ʱ��һ��
                SystemTimeToFileTime(&stNetFile, &ftWrite);
                SetFileTime(hFileDown, NULL, NULL, &ftWrite);
                CloseHandle(hFileDown);
                hFileDown = NULL;

                m_dwLastError = ERR_HTTP_USER_STOP;
                return false;
            }

            // ��ʱ����������
            if (!InternetReadFile(m_hRequest, pszData, __HTTP_READ_BUF_SIZE, &dwRead))
            {
                m_dwLastError = ::GetLastError();
                // ��������ֹ������
                delete pszData;
                pszData = NULL;

                m_dwLastError = ::GetLastError();
                CloseHandles();

                FlushFileBuffers(hFileDown);
                SetEndOfFile(hFileDown);

                // ���������ص��ļ����޸�ʱ���޸�Ϊ��������ļ�ʱ��һ��
                SystemTimeToFileTime(&stNetFile, &ftWrite);
                SetFileTime(hFileDown, NULL, NULL, &ftWrite);
                CloseHandle(hFileDown);
                hFileDown = NULL;

                return false;
            }

            m_httpDownInfo.dwFileDownloadedSize += dwRead;

            // ������ߴ��ڷ������ش�С��Ϣ
            m_dlstate = DL_WORKING;
            Notify(m_dlstate);

            //if (pData)
            //{
            //	pData->UpdateDataDuringTransfer(dwStartTicks, dwCurrentTicks, httpDown.dwFileDownloadedSize, dwLastTotalBytes, 
            //				dwLastPercentage, bGotFileSize, m_dwFileSize);
            //}

            if (dwRead > 0)
            {
                DWORD dwWrited = 0;
                WriteFile(hFileDown, pszData, dwRead, &dwWrited, NULL);
            }
        }while (dwRead > 0);

        //httpDown.nStatusType = DOWNLOAD_STATUS_DOWNLOADCOMPLETE;
        m_dlstate = DL_WORKING;
        Notify(m_dlstate);

        // û���������
        if ( m_httpDownInfo.dwFileDownloadedSize < m_httpDownInfo.dwFileSize )
        {
            CloseHandles();

            delete pszData;
            pszData = NULL;

            FlushFileBuffers(hFileDown);
            SetEndOfFile(hFileDown);

            // ���������ص��ļ����޸�ʱ���޸�Ϊ��������ļ�ʱ��һ��
            SystemTimeToFileTime(&stNetFile, &ftWrite);
            SetFileTime(hFileDown, NULL, NULL, &ftWrite);
            CloseHandle(hFileDown);	
            hFileDown = NULL;
            m_dwLastError = ERR_HTTP_FAIL;
            return false;
        }

        // httpDown.dwFileDownloadedSize = httpDown.dwFileSize;
        //Notify(WM_HTTPDOWN_NOTIFY_MSG, 0L, (LPARAM)&httpDown);

        TCHAR szConnection[__HTTP_HEADER_BUF_SIZE + 1] = { 0 };
        dwSize = __HTTP_HEADER_BUF_SIZE;
        if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_CONNECTION, szConnection, &dwSize, NULL))
        {
       //     m_dwLastError = ::GetLastError();
            CloseHandles();
        }
        else
        {
            if ( 0 ==lstrcmpi(szConnection, L"close") )
            {
                CloseHandles();
            }
            else
            {
                InternetCloseHandle(m_hRequest);
                m_hRequest = NULL;
            }
        }	

        delete pszData;
        pszData = NULL;

        FlushFileBuffers(hFileDown);
        SetEndOfFile(hFileDown);

        // ���������ص��ļ����޸�ʱ���޸�Ϊ��������ļ�ʱ��һ��
        SystemTimeToFileTime(&stNetFile, &ftWrite);
        SetFileTime(hFileDown, NULL, NULL, &ftWrite);
        CloseHandle(hFileDown);	
        hFileDown = NULL;

        // ��ָ�����ش洢�ļ����ļ����ڣ�ɾ�����ļ����������ļ�����ʱ�ļ�Ϊָ���洢�ļ�

		if (!m_bStop)
		{
			if (PathFileExists(lpszSaveFileName))
			{
				SetFileAttributes(lpszSaveFileName, FILE_ATTRIBUTE_NORMAL);
				DeleteFile(lpszSaveFileName);
			}
			if (!MoveFile(strTmpFile.data(), lpszSaveFileName))
			{
				m_dwLastError = ERR_HTTP_RENAME_FILE_FAIL;
				return false;
			}
			m_strSavePath = lpszSaveFileName;
		}

        //pData->SetState(IS_SUCCESS);

        m_dlstate = DL_COMPLETE;
        Notify(m_dlstate);
        return true;
    }




    bool CHttpDownload::Download(_tstring& szURL, _tstring& szSaveFileName, unsigned int trytimes)
    {
        unsigned int currentTry = 0;
        do
        {
            m_dwLastError = 0;


            if (Download(szURL.c_str(),szSaveFileName.c_str(),FALSE,NULL))
                return true;

            m_dwLastError = this->GetLastError();

            //�û��Ƴ�
            if (m_bStop)
                break;

            if (m_dwLastError != ERR_HTTP_REDIRECT)
            {
                currentTry++;
            }
            else
            {
                szURL = m_strRedirectURL;
                _tstring::size_type spos = m_strRedirectURL.rfind(L"/");
                if (spos != std::wstring::npos)
                {
                    _tstring sFileName = m_strRedirectURL.substr(spos+1,m_strRedirectURL.length() - spos);
                    _tstring::size_type spos1 = szSaveFileName.rfind(L"\\");
                    if (spos1 != _tstring::npos)
                    {
                        _tstring sPathName = szSaveFileName.substr(0,spos1);
                        sPathName.append(L"\\");
                        sPathName.append(sFileName);
                        szSaveFileName = sPathName;
                    }

                }
            }

			if (currentTry < trytimes)
            {
				if (m_dwLastError != ERR_HTTP_REDIRECT)
				{
					Sleep(DL_RetryDelay);
				}
                
            }
            else
                break;

        }
        while (true);

        return false;
    }



    bool CHttpDownload::Download(_tstring& szURL, HANDLE hMap, DWORDLONG d64StartPos, DWORDLONG d64EndPos)
    {
        if (szURL.empty())
        {
            m_dwLastError = ERR_HTTP_URL_OR_DATA_NULL;
            return false;
        }
        m_strRedirectURL.clear();		// ����ض����ַ

        if (NULL == hMap)
        {
            m_dwLastError = ERR_HTTP_URL_OR_DATA_NULL;
            return false;
        }

       
        

        // ����URL�Ƿ�Ϸ�
        if (!ParseUrl(szURL.data()))
        {
            m_dwLastError = ERR_HTTP_INVALID_URL;
            return false;
        }

        if (m_bServerAlter)
            CloseHandles();

        // ��������ֹ
        if (m_bStop)
        {
            CloseHandles();
            m_dwLastError = ERR_HTTP_USER_STOP;
            return false;
        }

        // ��ʼ����Ϣ�ṹ
        m_httpDownInfo.dwFileSize = 0;
        m_httpDownInfo.dwFileDownloadedSize = 0;
        m_httpDownInfo.strFileName = m_strObjectName.substr(m_strObjectName.rfind(L'/') + 1);

        // ����ô��ڷ��������ļ�����Ϣ
         Notify(DL_PREPARE);
        

        // ��ʼ��WinINet�������������
        if (!Connect())
        {
            m_dwLastError = ERR_HTTP_FAIL;
            return false;
        }

        // ��������ֹ
        if (m_bStop)
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }

        static LPCTSTR szAcceptType[]= { __HTTP_ACCEPT_TYPE, 0 };
        DWORD dwOpenFlags = INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION;


        // Open Request
        m_hRequest = HttpOpenRequest(m_hConnect, __HTTP_VERB_GET,
            m_strObjectName.data(), HTTP_VERSION, NULL,
            szAcceptType, dwOpenFlags, NULL);
        // ERROR return
        if (NULL == m_hRequest)
        {
            m_dwLastError = ::GetLastError();
            CloseHandles();
            return false;
        }


        TCHAR szHeader[__HTTP_HEADER_BUF_SIZE+1] = { 0 };

        //SYSTEMTIME stLocalFile;						// ���ش��ڴ������ļ�ʱ��
        SYSTEMTIME stNetFile;						// �������������ļ�ʱ��

        // Ĭ��ȡ��ǰʱ��
        GetLocalTime(&stNetFile);

        // �����ش��ڴ�������ʱ�ļ�����õ��ļ���Сд��Htppͷ��Range��
       
        
        if (d64StartPos != 0)
        {
            _tstring strSizeStart;
            JTerse::core::FormatW(strSizeStart,L"%llu", d64StartPos);
            if (d64EndPos == 0)
            {
                wsprintf(szHeader, L"Range: bytes=%s-", strSizeStart.data());
            }
            else
            {
                _tstring strSizeEnd;
                JTerse::core::FormatW(strSizeEnd,L"%llu", d64EndPos);
                 wsprintf(szHeader, L"Range: bytes=%s-%s", strSizeStart.data(),strSizeEnd.data());
            }
        }


        // ��������ֹ
        if (m_bStop)
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }

        // SEND REQUEST
        if (!::HttpSendRequest(m_hRequest, szHeader, lstrlen(szHeader),	NULL, 0))
        {
            m_dwLastError = ::GetLastError();
            CloseHandles();	
            return false;
        }	

        // ��������ֹ
        if (m_bStop)
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }

        DWORD dwCode = 0;
        DWORD dwSize = sizeof(DWORD) ;
        if (!HttpQueryInfo (m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL))
        {
            m_dwLastError = ::GetLastError();
            CloseHandles();
            return false;
        }

        BOOL bSendAgain = FALSE;
        switch (dwCode)
        {
            // 200 OK
        case HTTP_STATUS_OK:
            {
                
            }
            break;
        case HTTP_STATUS_NOT_FOUND:
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return false;
            }
            break;

        case HTTP_STATUS_FORBIDDEN:
            {
                m_dwLastError = ::GetLastError();
                CloseHandles();
                return false;
            }
            // �ϵ����󷵻�
        case HTTP_STATUS_PARTIAL:
        case HTTP_STATUS_PARTIAL_CONTENT:
            {
                m_dwLastError = ::GetLastError();
                return false;
            }
            break;
        case HTTP_STATUS_SERVER_ERROR:
            {
                m_dwLastError = ::GetLastError();
                return false;
            }		
            break;
        default:
                m_dwLastError = ::GetLastError();
                return false;
        }

        // ��������ֹ
        if (m_bStop)
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }


        TCHAR szLastModified[__HTTP_LASTMODIFIED_BUF_SIZE + 1] = { 0 };
        dwSize = __HTTP_LASTMODIFIED_BUF_SIZE;
        // �õ��������ļ�������޸�ʱ��,���ܻ�ò���
        if (HttpQueryInfo(m_hRequest, HTTP_QUERY_LAST_MODIFIED, szLastModified, &dwSize, NULL))
        {
            HttpDateToSystemTime(szLastModified, stNetFile);
        }

        DWORDLONG dwContentLength = 0;
        dwSize = sizeof(DWORDLONG);
        BOOL bGotFileSize = TRUE;

        // �õ��������ļ��Ĵ�С
        if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwContentLength, &dwSize, NULL))
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }

        //httpDown.dwFileSize = dwContentLength;
        // �̵������õ��Ĵ�СΪʣ�µĴ�С
        m_httpDownInfo.dwFileSize = dwContentLength + m_httpDownInfo.dwFileDownloadedSize;
        m_dwFileSize = m_httpDownInfo.dwFileSize;

        // �����ش��ڷ��������ļ���С��Ϣ

        m_dlstate = DL_START;
        BeginWork();
        Notify(m_dlstate);
        

        if (m_bStop)
        {
            m_dwLastError = ERR_HTTP_USER_STOP;
            CloseHandles();
            return false;
        }
       

        /*if (pData)
        {
        pData->SetTmpFile(strTmpFile);
        }*/

        //FILETIME ftWrite;			
        //SystemTimeToFileTime(&stNetFile, &ftWrite);
        //SetFileTime(hFileDown, NULL, NULL, &ftWrite);

        /*if (bSendAgain)
        {
            SetFilePointer(hFileDown, 0L, 0L, FILE_BEGIN);
            SetEndOfFile(hFileDown);
        }
        else
        {
            SetFilePointer(hFileDown, 0L, 0L, FILE_END);
        }*/

        // �����ļ�дʱ��
        CHAR* pszData = new CHAR[__HTTP_READ_BUF_SIZE + 1]; // ���ļ�������
        DWORD dwRead = 0;


        DWORDLONG dwLastTotalBytes = m_httpDownInfo.dwFileDownloadedSize;
        DWORD dwLastPercentage = 0;
        do 
        {
            ZeroMemory(pszData, __HTTP_READ_BUF_SIZE + 1);
            if (m_bStop)
            {
                // ��������ֹ������
                delete pszData;
                pszData = NULL;
                CloseHandles();

                m_dwLastError = ERR_HTTP_USER_STOP;
                return false;
            }

            // ��ʱ����������
            if (!InternetReadFile(m_hRequest, pszData, __HTTP_READ_BUF_SIZE, &dwRead))
            {
                m_dwLastError = ::GetLastError();
                // ��������ֹ������
                delete pszData;
                pszData = NULL;
                CloseHandles();

                return false;
            }

            m_httpDownInfo.dwFileDownloadedSize += dwRead;

            // ������ߴ��ڷ������ش�С��Ϣ
            m_dlstate = DL_WORKING;
            Notify(m_dlstate);


            if (dwRead > 0)
            {
                //DWORD dwWrited = 0;
               // WriteFile(hFileDown, pszData, dwRead, &dwWrited, NULL);
            }
        }while (dwRead > 0);

        m_dlstate = DL_WORKING;
        Notify(m_dlstate);



        // û���������
        if ( m_httpDownInfo.dwFileDownloadedSize < m_httpDownInfo.dwFileSize )
        {
            CloseHandles();

            delete pszData;
            pszData = NULL;

            //FlushFileBuffers(hFileDown);
            //SetEndOfFile(hFileDown);

            //// ���������ص��ļ����޸�ʱ���޸�Ϊ��������ļ�ʱ��һ��
            //SystemTimeToFileTime(&stNetFile, &ftWrite);
            //SetFileTime(hFileDown, NULL, NULL, &ftWrite);
            //CloseHandle(hFileDown);	

            //hFileDown = NULL;


            m_dwLastError = ERR_HTTP_FAIL;
            return false;
        }

        // httpDown.dwFileDownloadedSize = httpDown.dwFileSize;
        //Notify(WM_HTTPDOWN_NOTIFY_MSG, 0L, (LPARAM)&httpDown);

        TCHAR szConnection[__HTTP_HEADER_BUF_SIZE + 1] = { 0 };
        dwSize = __HTTP_HEADER_BUF_SIZE;
        if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_CONNECTION, szConnection, &dwSize, NULL))
        {
        //    m_dwLastError = ::GetLastError();
            CloseHandles();
        }
        else
        {
            if ( 0 ==lstrcmpi(szConnection, L"close") )
            {
                CloseHandles();
            }
            else
            {
                InternetCloseHandle(m_hRequest);
                m_hRequest = NULL;
            }
        }	

        delete pszData;
        pszData = NULL;

        //FlushFileBuffers(hFileDown);
        //SetEndOfFile(hFileDown);


        // ��ָ�����ش洢�ļ����ļ����ڣ�ɾ�����ļ����������ļ�����ʱ�ļ�Ϊָ���洢�ļ�
        /*if (PathFileExists(lpszSaveFileName))
        {
            SetFileAttributes(lpszSaveFileName, FILE_ATTRIBUTE_NORMAL);
            DeleteFile(lpszSaveFileName);
        }
        if (!MoveFile(strTmpFile.data(), lpszSaveFileName))
        {
            m_dwLastError = ERR_HTTP_RENAME_FILE_FAIL;
            return false;
        }*/


        m_dlstate = DL_COMPLETE;
        Notify(m_dlstate);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////////////
    //	��������BOOL ParseUrl(LPCTSTR lpszUrl)
    //	��  ;����������URL�ĺϷ��Լ����URL(�ڲ�ʹ��)
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ����
    //		lpszUrl		: ����ֵ�URL
    //	����ֵ���ɹ���TRUE��ʧ��FALSE
    /////////////////////////////////////////////////////////////////////////////////
    BOOL CHttpDownload::ParseUrl(LPCTSTR lpszUrl)
    {
        if (NULL == lpszUrl || lstrlen(lpszUrl) <=0)
            return FALSE;

        CSplitURL url;

        if (!url.Split(lpszUrl))
        {
            return FALSE;
        }

        _tstring strHostName;
        strHostName = url.GetHostName();

        // �鿴��������ַ�Ƿ�ı�
        if (JTerse::core::compareNoCase(m_strServerName,strHostName) != 0)
        {
            m_bServerAlter = TRUE;
        }
        else
        {
            m_bServerAlter = FALSE;
        }
        m_strServerName = strHostName;

        /*
        CString strUserName, strPassword
        strUserName = url.GetUserName();
        strPassword = url.GetPassword();
        if (!strUserName.IsEmpty())
        {
        SetAuthorization(strUserName, strPassword);
        }

        //*/
        m_nServerPort = url.GetPort() ? url.GetPort() :INTERNET_DEFAULT_HTTP_PORT;

        JTerse::core::FormatW(m_strObjectName,L"%s%s", url.GetURLPath(), url.GetExtraInfo()); 

        return TRUE;
    }

    // �ַ���ת��Ϊ���ִ�(�ڲ�ʹ��)
    static int IntVal(_tstring strVal)
    {
        int nVal = 0;
        //strVal.TrimLeft();
        JTerse::core::LTrim(strVal);
        for(unsigned int ndx = 0; ndx < strVal.length(); ++ndx)
            nVal = nVal*10 + strVal[ndx] - L'0';

        return nVal;
    }

    // ת���·�Ϊ����(�ڲ�ʹ��)
    static int MonthFromStr(const _tstring& str)
    {
        LPCTSTR aMonths[] = {
            L"xxx", L"jan", L"feb", L"mar", L"apr", L"may", L"jun",
            L"jul", L"aug", L"sep", L"oct", L"nov", L"dec" };
        int nMonth = 1;
        for(nMonth = 1; nMonth <= 12; ++nMonth)
        {
            if (JTerse::core::compareNoCase(str,aMonths[nMonth]) == 0)
                break;
        }

        return nMonth;
    }


    //////////////////////////////////////////////////////////////////////////////////
    //	��������BOOL HttpDateToSystemTime(LPCTSTR lpszTime, SYSTEMTIME &st)
    //	��  ;��ת������ʱ�䵽ϵͳʱ��(�ڲ�ʹ��)
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ����
    //		lpszTime		: HTTP��ʽϵͳʱ��
    //      st			    : ��ת����ϵͳʱ��
    //	����ֵ���ɹ���TRUE��ʧ��FALSE
    //  ˵����// Dow, dd Mon year hh:mm:ss GMT
    /////////////////////////////////////////////////////////////////////////////////
    BOOL CHttpDownload::HttpDateToSystemTime(LPCTSTR lpszTime, SYSTEMTIME &st)
    {
        ZeroMemory(&st, sizeof(SYSTEMTIME));

        _tstring strHttp = lpszTime;
        // assume we couldn't get a good time conversion....
        BOOL bOk = FALSE;

        int ndx = 0;
        switch(strHttp[3])
        {
        case ',':
            // read RFC-1123 (preferred)....
            st.wDay = IntVal(strHttp.substr(5,2));
            st.wMonth = MonthFromStr(strHttp.substr(8,3));
            st.wYear = IntVal(strHttp.substr(12,4));
            st.wHour = IntVal(strHttp.substr(17,2));
            st.wMinute = IntVal(strHttp.substr(20,2));
            st.wSecond = IntVal(strHttp.substr(23,2));
            break;
        case ' ':
            // read ANSI-C time format....
            st.wDay = IntVal(strHttp.substr(8,2));
            st.wMonth = MonthFromStr(strHttp.substr(4,3));
            st.wYear = IntVal(strHttp.substr(20,4));
            st.wHour = IntVal(strHttp.substr(11,2));
            st.wMinute = IntVal(strHttp.substr(14,2));
            st.wSecond = IntVal(strHttp.substr(17,2));
            break;
        default:
            if ((ndx = strHttp.find(L", ")) != -1)
            {
                st.wDay = IntVal(strHttp.substr(ndx+2,2));
                st.wMonth = MonthFromStr(strHttp.substr(ndx+5,3));
                st.wYear = IntVal(strHttp.substr(ndx+9,2));
                st.wHour = IntVal(strHttp.substr(ndx+12,2));
                st.wMinute = IntVal(strHttp.substr(ndx+15,2));
                st.wSecond = IntVal(strHttp.substr(ndx+18,2));
                // add the correct century....
                st.wYear += (st.wYear > 50)?1900:2000;
            }
            break;
        }
        // if year not zero....
        if (st.wYear != 0)
        {
            return TRUE;
        }
        return FALSE;
    }

    //////////////////////////////////////////////////////////////////////////////////
    //	��������BOOL SetProxy(LPCTSTR lpszProxyServer,
    //					USHORT nProxyPort, 
    //					BOOL bUseProxy /* =TRUE */,
    //					BOOL bProxyAuthorization /* =FALSE */,
    //					LPCTSTR lpszProxyUsername /* =NULL */,
    //					LPCTSTR lpszProxyPassword /* =NULL */)
    //	��  ;�����ô�����������ʲ���
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ����
    //		lpszProxyServer		: �����������
    //      nProxyPort			: ����������˿�
    //		bUseProxy			: �Ƿ�ʹ�ô���(��ֵΪFALSEʱ��ǰ����������������)
    //		bProxyAuthorization : ����������Ƿ���Ҫ�����֤
    //		lpszProxyUsername   : ��������������֤�û���
    //		lpszProxyUsername   : ��������������֤����
    //	����ֵ���ɹ���TRUE��ʧ��FALSE
    /////////////////////////////////////////////////////////////////////////////////
    BOOL CHttpDownload::SetProxy(LPCTSTR lpszProxyServer,
        USHORT nProxyPort, 
        BOOL bUseProxy /* =TRUE */,
        BOOL bProxyAuthorization /* =FALSE */,
        LPCTSTR lpszProxyUsername /* =NULL */,
        LPCTSTR lpszProxyPassword /* =NULL */)
    {
        m_bUseProxy				= bUseProxy;
        m_strProxyServer		= lpszProxyServer;
        m_nProxyPort			= nProxyPort;
        m_bProxyAuthorization	= bProxyAuthorization;
        m_strProxyUsername		= lpszProxyUsername;
        m_strProxyPassword		= lpszProxyPassword;

        return TRUE;
    }

    //////////////////////////////////////////////////////////////////////////////////
    //	��������void SetAuthorization(
    //				LPCTSTR lpszUsername,
    //				LPCTSTR lpszPassword,
    //				BOOL bAuthorization/* = TRUE*/) 
    //	��  ;������WWW��֤��Ϣ(���ʱ�������ҳ��ʱ��Ҫ)
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ����
    //		lpszUsername   : ����ҳ����û���
    //		lpszPassword   : ����
    //		bAuthorization : �Ƿ���Ҫ��֤(��ֵΪFALSEʱ��ǰ����������������)
    //	����ֵ��void
    ////////////////////////////////////////////////////////////////////////////////
    void CHttpDownload::SetAuthorization(LPCTSTR lpszUsername,LPCTSTR lpszPassword,BOOL bAuthorization/* = TRUE*/)
    {
        if(bAuthorization && lpszUsername != NULL)
        {
            m_bAuthorization = TRUE;
            m_strUsername	 = lpszUsername;
            m_strPassword	 = lpszPassword;
        }
        else
        {
            m_bAuthorization = FALSE;
            m_strUsername	 = L"";
            m_strPassword	 = L"";
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    //	��������BOOL CloseHandles(void)
    //	��  ;���ر������������е�INTERNET������ڲ�ʹ�ã�
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ������
    //	����ֵ���ɹ�-TRUE��ʧ��-FALSE
    /////////////////////////////////////////////////////////////////////////////////
    BOOL CHttpDownload::CloseHandles()
    {
        if (m_hRequest)
        {
            InternetCloseHandle(m_hRequest);
            m_hRequest = NULL;
        }

        if (m_hConnect)
        {
            InternetCloseHandle(m_hConnect);
            m_hConnect = NULL;
        }

        if (m_hInternet)
        {
            InternetCloseHandle(m_hInternet);
            m_hInternet = NULL;
        }

        return TRUE;
    }

    //////////////////////////////////////////////////////////////////////////////////
    //	��������BOOL SetNotifyHwnd(HWND hNotifyWnd)
    //	��  ;�����ý���������Ϣ�Ĵ��ھ��
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ����hNotifyWnd : ����������Ϣ�Ĵ��ھ��
    //	����ֵ���Ϸ�����Ϊ-TRUE������-FALSE
    /////////////////////////////////////////////////////////////////////////////////
    void CHttpDownload::SetObserver(Observer* lpob)
    {
        // hNotifyWnd ����Ч���ھ��
        if (lpob != NULL)
        {
            m_Observer = lpob;
        }

    }

    //////////////////////////////////////////////////////////////////////////////////
    //	��������void StopDownload()
    //	��  ;��ֹͣ����
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ������
    //	����ֵ����
    /////////////////////////////////////////////////////////////////////////////////
    void CHttpDownload::StopDownload()
    {
        m_bStop = TRUE;
    }

    //////////////////////////////////////////////////////////////////////////////////
    //	��������LPCTSTR GetRedirectURL()
    //	��  ;���õ��ض����ģգң�
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ������
    //	����ֵ��URL�ַ���
    //  ˵��  : ������Download�󷵻�ֵΪERR_HTTP_REDIRECT���ٵ��ô˺����õ��ض���URL
    /////////////////////////////////////////////////////////////////////////////////
    LPCTSTR CHttpDownload::GetRedirectURL()
    {
        return m_strRedirectURL.data();
    }

    //////////////////////////////////////////////////////////////////////////////////
    // 
    // 
    BOOL CHttpDownload::Notify(HTTPDownloadStatus msg)
    {
        if (m_Observer != NULL)
        {
            switch (msg)
            {
            case DL_WORKING:
                {
                    m_Observer->OnDownloadUpdated(this);
                }
                break;
            case DL_COMPLETE:
                {
                    m_Observer->OnDownloadFinish(this);
                }
                break;
            default:
               break;
            }
        }

        return TRUE;
    }

    void CHttpDownload::ContinueDownload()
    {
        m_bStop = FALSE;
    }

    DWORDLONG CHttpDownload::GetFileSizeByUrl( LPCTSTR strUrl )
    {
        if (strUrl == NULL)
        {
            return ERR_HTTP_URL_OR_DATA_NULL;
        }

        // ����URL�Ƿ�Ϸ�
        if (!ParseUrl(strUrl))
        {
            return ERR_HTTP_INVALID_URL;
        }

        // ��ʼ��WinINet�������������
        if (!Connect())
        {
            return ERR_HTTP_FAIL;
        }

        static LPCTSTR szAcceptType[]= { __HTTP_ACCEPT_TYPE, 0 };
        DWORD dwOpenFlags = INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION;


        // Open Request
        m_hRequest = HttpOpenRequest(m_hConnect, __HTTP_VERB_GET,
            m_strObjectName.data(), HTTP_VERSION, NULL,
            szAcceptType, dwOpenFlags, NULL);
        // ERROR return
        if (NULL == m_hRequest)
        {
            m_dwLastError = ::GetLastError();
            CloseHandles();
            //pData->SetState(IS_FAIL);
            return ERR_HTTP_FAIL;
        }

        // SEND REQUEST
        if (!::HttpSendRequest(m_hRequest, NULL, NULL,	NULL, 0))
        {
            m_dwLastError = ::GetLastError();
            CloseHandles();	

            switch (m_dwLastError)
            {
            case ERROR_INTERNET_CANNOT_CONNECT:
            case ERROR_INTERNET_NAME_NOT_RESOLVED:
                return ERR_HTTP_NET_ERROR;
                break;
            case ERROR_INTERNET_TIMEOUT:
                return ERR_HTTP_SEND_TIMEOUT;
            default:
                {
                }
                return ERR_HTTP_FAIL;;
            }
        }	


        DWORDLONG dwContentLength = 0;
        DWORD dwSize = sizeof(DWORDLONG);
        BOOL bGotFileSize = TRUE;

        // �õ��������ļ��Ĵ�С
        if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwContentLength, &dwSize, NULL))
        {
            // ��Щ���������������ش��󣬵����������ļ�����ʱĬ��200K
            return NULL;
        }

        return dwContentLength;
    }

    bool CHttpDownload::GetFileInfoByUrl(LPCTSTR strUrl,bool& bAsseptRage,_tstring& strContentype,_tstring& sFileName,DWORDLONG& dlFileSize)
    {
        bool bRet = false;
        if (strUrl == NULL)
        {
            m_dwLastError = ERR_HTTP_URL_OR_DATA_NULL;
            return false;
        }

        // ����URL�Ƿ�Ϸ�
        if (!ParseUrl(strUrl))
        {
            m_dwLastError = ERR_HTTP_INVALID_URL;
            return false;
        }

        // ��ʼ��WinINet�������������
        if (!Connect())
        {
            m_dwLastError = ERR_HTTP_FAIL;
            return false;
        }

        HINTERNET hInternetUrl = InternetOpenUrlW(m_hInternet,
            strUrl,
            L"Accept: */*",
            -1,
            INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_AUTO_REDIRECT,
            NULL);

        if(hInternetUrl == NULL)
        {
            return FALSE;
        }

        wchar_t szStatus[1024]={NULL};
        DWORD dwBufferLen = sizeof(szStatus);

        ::HttpQueryInfoW(hInternetUrl,HTTP_QUERY_STATUS_CODE,szStatus,&dwBufferLen,NULL);

        switch (_wtoi(szStatus))
        {
        case 302:
            dwBufferLen = sizeof(szStatus);
            ::HttpQueryInfoW(hInternetUrl,HTTP_QUERY_LOCATION,szStatus,&dwBufferLen,NULL);
             m_strRedirectURL = szStatus;
             //�޸ı����ļ���
             {
                 _tstring::size_type spos = m_strRedirectURL.rfind(L"/");
                 if (spos != std::wstring::npos)
                 {
                     sFileName = m_strRedirectURL.substr(spos+1,m_strRedirectURL.length() - spos);
                 }
             }
            bRet = GetFileInfoByUrl(szStatus,bAsseptRage,strContentype,sFileName,dlFileSize);
            break;

        case 200:
            {
                bAsseptRage = false;
                DWORD dwSize = sizeof(DWORDLONG);
               

                do 
                {
                    if (!::HttpQueryInfoW(hInternetUrl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dlFileSize, &dwSize, 0))
                    {
                        m_dwLastError = GetLastError();
                        bRet = false;
                        break;
                    }


                    dwBufferLen = sizeof(szStatus);
                    if (!::HttpQueryInfoW(hInternetUrl, HTTP_QUERY_ACCEPT_RANGES , szStatus, &dwBufferLen, 0))
                    {
                        m_dwLastError = GetLastError();
                        bRet = false;
                        break;
                    }

                    if(wcscmp(szStatus,L"bytes") == 0)
                        bAsseptRage = true;

                    dwBufferLen = sizeof(szStatus);
                    if (!::HttpQueryInfoW(hInternetUrl, HTTP_QUERY_CONTENT_TYPE , szStatus, &dwBufferLen, 0))
                    {
                        m_dwLastError = GetLastError();
                        bRet = false;
                        break;
                    }

                    strContentype = szStatus;
                    

                } while (false);
                 
               

            }
            break;
        default:
            break;
        }

        InternetCloseHandle(hInternetUrl);
        //CloseHandles();
        return bRet;
    }



    void CHttpDownload::IncreaseStartPosition(__int64 size)
    {

        __timeb64 tsnow;
        _ftime64_s(&tsnow);

        __time64_t t_sec,ts;
        unsigned short t_ms;

        m_httpDownInfo.dwFileDownloadedSize += size;

        if (started_)
        {
            //�����
            t_sec=tsnow.time - lastReception_.time;
            //�������
            t_ms=tsnow.millitm - lastReception_.millitm;
            ts=t_sec*1000+t_ms;

			if (ts < 1000)
            {
                return;
            }

            // ÿ���ֽ���
            rate_ = ((double)(m_httpDownInfo.dwFileDownloadedSize - start_)) / ((double)(ts/1000));

            if (rate_ > 0.0)
            {
                left_ = (__time64_t)(getMissingTransfer() / rate_);
            }
            else
            {
                left_ = DL_TimeSpan_MaxValue;
            }
        }
        else
        {
            start_ = m_httpDownInfo.dwFileDownloadedSize;
            lastReception_ = tsnow;
            started_ = true;
        }

    }


    void CHttpDownload::BeginWork()
    {
        start_ = m_httpDownInfo.dwFileDownloadedSize;
        //��¼��ʼʱ��
        _ftime64_s(&lastReception_);
        started_ = true;
    }


    //////////////////////////////////////////////////////////////////////////////////
    //	��������bool IsWorking()
    //	��  ;���õ��ض����ģգң�
    //	��ȫ�ֱ�����Ӱ�죺��
    //	��  ������
    //	����ֵ��bool work or not
    //  ˵��  : 
    /////////////////////////////////////////////////////////////////////////////////
    bool CHttpDownload::IsWorking()
    {
        return (m_dlstate == DL_START ||
            m_dlstate == DL_WORKING);
    }

    //��ȡ����
    double CHttpDownload::getProgress()
    {
        return (m_httpDownInfo.dwFileSize <= 0? 0: ((double)getTransfered() / (double)m_httpDownInfo.dwFileSize * 100.0f));
    }

    //��ȡ�����ٶ�
    double CHttpDownload::getRate()
    {
        if (m_dlstate == DL_WORKING)
        {
            IncreaseStartPosition(0);
            return rate_;
        }
        else
        {
            return 0;
        }
    }


    //��ȡ������
    __int64 CHttpDownload::getTransfered()
    {
        return m_httpDownInfo.dwFileDownloadedSize; 
    }

    //��ȡʣ��ʱ��
    __time64_t CHttpDownload::getTimeLeft()
    {
        double dRate = this->getRate();
        if (dRate == 0)
            return DL_TimeSpan_MaxValue;

        __int64 missingTransfer = getMissingTransfer();
       

        return (__time64_t)((double)missingTransfer / dRate);
    }

    //ʣ���С
    __int64 CHttpDownload::getMissingTransfer()
    {
        return (m_httpDownInfo.dwFileSize <= 0? 0: m_httpDownInfo.dwFileSize - m_httpDownInfo.dwFileDownloadedSize);
    }
}

