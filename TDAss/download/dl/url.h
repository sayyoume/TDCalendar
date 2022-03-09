// Implementation of the CURLComponents and CSplitURL classes.

#pragma once

#include <assert.h>
#include <wininet.h>
#include "../../JLHelper.h"

#pragma comment(lib, "wininet.lib")



// Class to wrap the Win32 URL_COMPONENTS structure
class CURLComponents : public URL_COMPONENTSW
{
public:	
	CURLComponents(void)
	{
		memset(this, 0, sizeof(URL_COMPONENTSW));
		dwStructSize = sizeof(URL_COMPONENTSW);
	}
};

// Class used to split a URL into component parts.
// Note: Uses WININET InternetCrackUrl function.
class CSplitURL
{
private:
	_tstring m_strScheme;
	INTERNET_SCHEME m_nScheme;
	_tstring m_strHostName;
	INTERNET_PORT m_nPort;
	_tstring m_strUserName;
	_tstring m_strPassword;
	_tstring m_strURLPath;
	_tstring m_strExtraInfo;
public:	
	CSplitURL(void)
		: m_nScheme(INTERNET_SCHEME_DEFAULT)
		, m_nPort(0)
	{
	}
	
	CSplitURL(LPCTSTR lpsz)
		: m_nScheme(INTERNET_SCHEME_DEFAULT)
		, m_nPort(0)
	{
		Split(lpsz);
	}

	~CSplitURL(void)
	{
	}

	// Split a URL into component parts
	bool Split(LPCTSTR lpsz)
	{
		// Be defensive
		assert(lpsz != NULL && *lpsz != L'\0');
		// Get the URL length
		DWORD dwLength = (DWORD)wcslen(lpsz);

		CURLComponents url;		
		// Fill structure
		url.lpszScheme = new wchar_t[dwLength];
        if (url.lpszScheme == NULL)
            return false;
        memset(url.lpszScheme,0,dwLength*sizeof(wchar_t));
		url.dwSchemeLength = dwLength;

		url.lpszHostName = new wchar_t[dwLength];
        if (url.lpszHostName == NULL)
            return false;
        memset(url.lpszHostName,0,dwLength*sizeof(wchar_t));
		url.dwHostNameLength = dwLength;

		url.lpszUserName = new wchar_t[dwLength];
        if (url.lpszUserName == NULL)
            return false;
        memset(url.lpszUserName,0,dwLength*sizeof(wchar_t));
		url.dwUserNameLength = dwLength;

		url.lpszPassword = new wchar_t[dwLength];
        if (url.lpszPassword == NULL)
            return false;
        memset(url.lpszPassword,0,dwLength*sizeof(wchar_t));
		url.dwPasswordLength = dwLength;

		url.lpszUrlPath = new wchar_t[dwLength];
        if (url.lpszUrlPath == NULL)
            return false;
        memset(url.lpszUrlPath,0,dwLength*sizeof(wchar_t));
		url.dwUrlPathLength = dwLength;

		url.lpszExtraInfo = new wchar_t[dwLength];
        if (url.lpszExtraInfo == NULL)
            return false;
        memset(url.lpszExtraInfo,0,dwLength*sizeof(wchar_t));
		url.dwExtraInfoLength = dwLength;
		// Split
		bool bRet = InternetCrackUrl(lpsz, 0, 0, &url) != FALSE;
		// Release buffers
		m_strScheme = url.lpszScheme;
        SAFEDELARRAY(url.lpszScheme);
		m_strHostName = url.lpszHostName;
        SAFEDELARRAY(url.lpszHostName);
		m_strUserName = url.lpszUserName;
        SAFEDELARRAY(url.lpszUserName);
		m_strPassword = url.lpszPassword;
        SAFEDELARRAY(url.lpszPassword);
		m_strURLPath = url.lpszUrlPath;
        SAFEDELARRAY(url.lpszUrlPath);
		m_strExtraInfo = url.lpszExtraInfo;
        SAFEDELARRAY(url.lpszExtraInfo);
		// Get the scheme/port
		m_nScheme = url.nScheme;
		m_nPort = url.nPort;

        
		// Done
		return bRet;
	}

	// Get the scheme number
	inline INTERNET_SCHEME GetScheme(void) const throw() { return m_nScheme; }
	// Get the port number
	inline INTERNET_PORT GetPort(void) const throw() { return m_nPort; }
	// Get the scheme name
	inline LPCTSTR GetSchemeName(void) const throw() { return m_strScheme.c_str(); }
	// Get the host name
	inline LPCTSTR GetHostName(void) const throw() { return m_strHostName.c_str(); }
	// Get the user name
	inline LPCTSTR GetUserName(void) const throw() { return m_strUserName.c_str(); }
	// Get the password
	inline LPCTSTR GetPassword(void) const throw() { return m_strPassword.c_str(); }
	// Get the URL path
	inline LPCTSTR GetURLPath(void) const throw() { return m_strURLPath.c_str(); }
	// Get the extra info
	inline LPCTSTR GetExtraInfo(void) const throw() { return m_strExtraInfo.c_str(); }
};
