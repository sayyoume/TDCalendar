/************************************************************************/
/* 
    Date :   1/7/2016
	File :   HttpClient.cpp
    Author : Eadwin
    Funtion :  
	Version : 1.4
	Copyright (c) 2014-2015 Oasgames
*/
/************************************************************************/  
#include "HttpClient.h"
#include "../3rd/curl/curl.h"
#include "../StringConvertion.h"
#include <sys/stat.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wldap32.lib")


#ifdef _DEBUG
#pragma comment(lib,"libcurld.lib")
#else
#pragma comment(lib,"libcurl.lib")
#endif

CHttpClient::CHttpClient(void) :
m_bDebug(false)
{
	m_bRunning = false;
}

CHttpClient::~CHttpClient(void)
{
	Stop(true);
}

void CHttpClient::InitNetwork()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

void CHttpClient::ReleaseNetwork()
{
	curl_global_cleanup();
}

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	//if (itype == CURLINFO_TEXT)
	//{
	//	//printf("[TEXT]%s\n", pData);
	//}
	//else if (itype == CURLINFO_HEADER_IN)
	//{
	//	printf("[HEADER_IN]%s\n", pData);
	//}
	//else if (itype == CURLINFO_HEADER_OUT)
	//{
	//	printf("[HEADER_OUT]%s\n", pData);
	//}
	//else if (itype == CURLINFO_DATA_IN)
	//{
	//	printf("[DATA_IN]%s\n", pData);
	//}
	//else if (itype == CURLINFO_DATA_OUT)
	//{
	//	printf("[DATA_OUT]%s\n", pData);
	//}
	return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if (NULL == str || NULL == buffer)
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);
	return nmemb;
}

int CHttpClient::Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	/*curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);*/
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Post(const std::wstring & strUrl, const std::wstring & strPost, std::wstring & strResponse)
{
	int res;
	std::string sUrl, sPost, sResponse;
	if (strUrl.empty())
	{
		return CURLE_URL_MALFORMAT;
	}
	sUrl = base::SysWideToUTF8(strUrl);

	if (!strPost.empty())
	{
		sPost = base::SysWideToUTF8(strPost);
	}

	res = Post(sUrl, sPost, sResponse);

	if (res == 0 && !sResponse.empty()){
		strResponse = base::SysUTF8ToWide(sResponse);
	}

	return res;
}

int CHttpClient::ReportPost(const std::wstring & strUrl, const std::wstring & strPost, std::wstring & strResponse)
{

	std::string sUrl, sPost, sResponse;
	if (strUrl.empty())
	{
		return CURLE_URL_MALFORMAT;
	}
	sUrl = base::SysWideToUTF8(strUrl);

	if (!strPost.empty())
	{
		sPost = base::SysWideToUTF8(strPost);
	}

	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	struct curl_slist *headerlist = NULL;
	headerlist = curl_slist_append(headerlist, "Content-type:application/json");

	curl_easy_setopt(curl, CURLOPT_URL, sUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&sResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

	res = curl_easy_perform(curl);


	if (NULL != headerlist)
	{
		curl_slist_free_all(headerlist);
	}

	curl_easy_cleanup(curl);

	if (res == 0 && !sResponse.empty()){
		strResponse = base::SysUTF8ToWide(sResponse);
	}

	return res;
}

int CHttpClient::Get(const std::string & strUrl, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/

	// 设置重定向的最大次数  
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// 设置301、302跳转跟随location  
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	/*curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);*/
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}


int CHttpClient::Get(const std::wstring & strUrl, std::wstring & strResponse)
{
	int res;
	std::string sUrl, sResponse;
	if (strUrl.empty())
	{
		return CURLE_URL_MALFORMAT;
	}

	sUrl = base::SysWideToUTF8(strUrl);

	res = Get(sUrl, sResponse);

	if (!sResponse.empty()){
		strResponse = base::SysUTF8ToWide(sResponse);
	}
	return res;
}

int CHttpClient::Get(const std::wstring & strUrl, std::wstring & strResponse, bool bCheckProxy)
{
	return Get(strUrl, strResponse, bCheckProxy, 3L, 3L);
}


int CHttpClient::Get(const std::wstring & strUrl, std::wstring & strResponse, bool bCheckProxy, long lRequestTimeout, long lConnectTimeout)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	std::string sUrl, sResponse;
	char proxy_data[1024] = { 0 };
	char szIP[256] = { 0 };
	WORD dwPort = 0;
	sUrl = base::SysWideToUTF8(strUrl);

	if (bCheckProxy)
	{
		if (get_proxy_enable(szIP, 256, dwPort))
		{
			sprintf_s(proxy_data, 1024, "%s:%d", szIP, dwPort);
		}
		else
		{
			bCheckProxy = false;
		}
	}

	curl_easy_setopt(curl, CURLOPT_URL, sUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&sResponse);

	if (bCheckProxy)
	{
		res = curl_easy_setopt(curl, CURLOPT_PROXY, proxy_data);
	}

	// 设置重定向的最大次数  
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// 设置301、302跳转跟随location  
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, lConnectTimeout);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, lRequestTimeout);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (!sResponse.empty()){
		strResponse = base::SysUTF8ToWide(sResponse);
	}
	return res;
}


std::future<std::wstring> CHttpClient::Get_Async(const std::wstring & strUrl)
{
	return std::async(std::launch::async, [&]{
		std::wstring strResponse;
		Get(strUrl, strResponse);
		return strResponse;
	});
}

int CHttpClient::Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	/*curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);*/
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	// 设置重定向的最大次数  
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// 设置301、302跳转跟随location  
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}


void CHttpClient::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
}

#define USER_AGENT "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 Safari/537.36"

size_t write_fun(void *buffer, size_t size, size_t nmemb, void *pParam)
{
	FILE* fp = (FILE*)pParam;
	size_t nWrite = fwrite(buffer, size, nmemb, fp);

	return nWrite;
}

long get_local_file_len(const wchar_t * file_path)
{
	long len = 0;
	FILE * file = NULL;

	_wfopen_s(&file, file_path, L"rb");
	if (NULL != file)
	{
		fseek(file, 0, SEEK_END);
		len = ftell(file);

		fclose(file);
		file = NULL;
	}

	return len;
}

bool CHttpClient::get_proxy_enable(char * szIP, unsigned long uLen, unsigned short & dwPort)
{
	bool bRet = false;
	HKEY hKey = NULL;
	DWORD dwValue = { 0 };
	DWORD dwType = REG_DWORD;
	DWORD dwDataSize = sizeof(DWORD);

	DWORD dwTypeS = REG_SZ;
	DWORD dwSize = 256;
	char szValue[256] = { 0 };

	if (::RegOpenKeyA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", &hKey) == ERROR_SUCCESS)
	{
		do
		{
			if (::RegQueryValueExA(hKey, "ProxyEnable", NULL, &dwType, (LPBYTE)(&dwValue), &dwDataSize) != ERROR_SUCCESS)
			{
				break;
			}

			if (!dwValue)
			{
				break;
			}

			if (::RegQueryValueExA(hKey, "ProxyServer", NULL, &dwTypeS, (LPBYTE)szValue, &dwSize) != ERROR_SUCCESS)
			{
				break;
			}

			if (dwSize <= 0)
			{
				break;
			}

			char * pTail = strchr(szValue, ':');
			if (NULL != pTail)
			{
				char * temp = pTail;
				temp++;
				dwPort = atoi(temp);

				strcpy_s(pTail, strlen(pTail), "");

				strcpy_s(szIP, uLen, szValue);

				if (strstr(szIP, "http=") != NULL)
				{
					strcpy_s(szIP, uLen, szIP + 5);
				}

				if (dwPort <= 0 || strlen(szIP) == 0)
				{
					break;
				}

				bRet = TRUE;
			}

		} while (FALSE);

		::RegCloseKey(hKey);
	}

	return bRet;
}

#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3

static int xferinfo(void* p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	CHttpClient* pThis = (CHttpClient*)p;
	CURL_Progress *myp = &pThis->prog;

	CURL *pcurl = (CURL*)myp->curl;
	double curtime = 0;
	double downloadSpeed = 0;
	int iRet = 0;
	curl_easy_getinfo(pcurl, CURLINFO_TOTAL_TIME, &curtime);
	curl_easy_getinfo(pcurl, CURLINFO_SPEED_DOWNLOAD, &downloadSpeed);

	long retcode = 0;
	curl_easy_getinfo(pcurl, CURLINFO_RESPONSE_CODE, &retcode);
	if (retcode != 200)
	{
		return 0;
	}

	if ((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL)
	{
		myp->lastruntime = curtime;
	}

	if (myp->bStop)
		return 1;

	if (myp->func == nullptr)
		return 0;

	if (dltotal > 0)
	{
		double dpos = dlnow*100.0 / dltotal;
		if (dlnow > 0)
		{
			myp->func(pcurl, dpos, downloadSpeed, curtime, dlnow, dltotal);
		}
	}

	return 0;
}

int CHttpClient::get_taget_file_ex(const char * url, const wchar_t * file_path, const DownloadCB& func, bool bRange, bool bCheckProxy)
{
	bool ret_fun = false;

	CURL * pCurl = NULL;
	CURLcode code = CURLE_FAILED_INIT;

	FILE * file = NULL;
	//USER_PARAM user_p = { 0 };

	wchar_t wszGUID[256] = { 0 };
	char szGUID[256] = { 0 };

	char user_agent[1024] = { 0 };

	char rang_data[100] = { 0 };
	unsigned int down_load = 0;

	char proxy_data[1024] = { 0 };
	char szIP[256] = { 0 };
	WORD dwPort = 0;

	if (bCheckProxy)
	{
		if (get_proxy_enable(szIP, 256, dwPort))
		{
			sprintf_s(proxy_data, 1024, "%s:%d", szIP, dwPort);
		}
		else
		{
			bCheckProxy = false;
		}
	}

	if (bRange)
	{
		//获取已经下载的文件大小.
		down_load = get_local_file_len(file_path);
		sprintf_s(rang_data, 100, "%d- ", down_load);

		//追加模式打开.
		_wfopen_s(&file, file_path, L"ab");
	}
	else
	{
		_wfopen_s(&file, file_path, L"wb");
		//user_p.range = false;
	}

	//user_p.file = file;

	pCurl = curl_easy_init();
	if (NULL != pCurl &&
		NULL != file)
	{
		//WCharToChar(wszGUID,szGUID);
		strcpy_s(user_agent, USER_AGENT);

		struct curl_slist *headerlist = NULL;

		headerlist = curl_slist_append(headerlist, "Connection:keep-alive");

		code = curl_easy_setopt(pCurl, CURLOPT_URL, url);
		code = curl_easy_setopt(pCurl, CURLOPT_USERAGENT, user_agent);
		code = curl_easy_setopt(pCurl, CURLOPT_ENCODING, "gzip, deflate");
		curl_easy_setopt(pCurl, CURLOPT_MAXREDIRS, 10);
		curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1);

		if (bCheckProxy)
		{
			code = curl_easy_setopt(pCurl, CURLOPT_PROXY, proxy_data);
		}

		if (bRange)
		{
			code = curl_easy_setopt(pCurl, CURLOPT_RANGE, rang_data);
		}

		//user_p.buf_pos = 0;
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, file);
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, &write_fun);
		curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headerlist);
		//curl_easy_setopt(pCurl, CURLOPT_LOW_SPEED_LIMIT, 1L);
		//curl_easy_setopt(pCurl, CURLOPT_LOW_SPEED_TIME, 5L);
		//curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 5L);
		//curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 5L);


#if LIBCURL_VERSION_NUM >= 0x072000
		prog.lastruntime = 0;
		prog.curl = pCurl;
		prog.func = func;
		curl_easy_setopt(pCurl, CURLOPT_XFERINFOFUNCTION, xferinfo);
		curl_easy_setopt(pCurl, CURLOPT_XFERINFODATA, this);
#endif

		code = curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 0L);

		code = curl_easy_perform(pCurl);

		long retcode = 0;
		curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &retcode);
		if ((code == CURLE_OK) && retcode == 200)
		{
			ret_fun = true;
		}
		else
		{
			if (code == CURLE_OK)
			{
				code = (CURLcode)retcode;
			}
			
		}

		//处理剩余的数据.
		/*if (user_p.buf_pos > 0)
		{
		fwrite(user_p.buf, 1, user_p.buf_pos, file);
		}*/

		if (NULL != headerlist)
		{
			curl_slist_free_all(headerlist);
		}
	}

	if (NULL != file)
	{
		fflush(file);

		fclose(file);
		file = NULL;
	}

	if (NULL != pCurl)
	{
		curl_easy_cleanup(pCurl);
		pCurl = NULL;
	}

	return int(code);
}

int CHttpClient::get_taget_header(const char * url, HTTP_PARAM &httpParam, bool bCheckProxy)
{
	CURL * pCurl = NULL;
	CURLcode code = CURLE_FAILED_INIT;

	wchar_t wszGUID[256] = { 0 };
	char szGUID[256] = { 0 };

	char user_agent[1024] = { 0 };

	char proxy_data[1024] = { 0 };
	char szIP[256] = { 0 };
	WORD dwPort = 0;


	if (bCheckProxy)
	{
		if (get_proxy_enable(szIP, 256, dwPort))
		{
			sprintf_s(proxy_data, 1024, "%s:%d", szIP, dwPort);
		}
		else
		{
			bCheckProxy = false;
		}
	}

	pCurl = curl_easy_init();
	if (NULL != pCurl)
	{
		strcpy_s(user_agent, USER_AGENT);

		code = curl_easy_setopt(pCurl, CURLOPT_URL, url);
		code = curl_easy_setopt(pCurl, CURLOPT_USERAGENT, user_agent);
		code = curl_easy_setopt(pCurl, CURLOPT_NOBODY, 1);

		if (bCheckProxy)
		{
			code = curl_easy_setopt(pCurl, CURLOPT_PROXY, proxy_data);
		}

		code = curl_easy_setopt(pCurl, CURLOPT_ENCODING, "gzip, deflate");

		code = curl_easy_perform(pCurl);

		double sz = 0;
		char * url_jump;

		if (CURLE_OK == code)
		{

			code = curl_easy_getinfo(pCurl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &sz);
			code = curl_easy_getinfo(pCurl, CURLINFO_REDIRECT_URL, &url_jump);

			httpParam.len = (long)sz;
			if (url_jump != NULL  && strlen(url_jump) > 0)
			{
				strcpy_s(httpParam.url_taget, url_jump);
			}
			else
			{
				strcpy_s(httpParam.url_taget, url);
			}

		}
	}

	if (NULL != pCurl)
	{
		curl_easy_cleanup(pCurl);
		pCurl = NULL;
	}

	return code;
}

struct input {
	FILE *in;
	size_t bytes_read; /* count up */
	CURL *hnd;
};

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	struct input *i = (input*)userp;
	size_t retcode = fread(ptr, size, nmemb, i->in);
	i->bytes_read += retcode;
	return retcode;
}

int CHttpClient::put_taget_file(const char * url, const wchar_t * file_path, bool bPost, bool bCheckProxy)
{

	CURL *pCurl;
	CURLcode res = CURLE_FAILED_INIT;
	struct _stat32 file_info;
	FILE *file, *filetemp;
	struct input in_data;
	char user_agent[1024] = { 0 };
	_wfopen_s(&file, file_path, L"rb");
	wchar_t file_temp[MAX_PATH] = { 0 };
	wcscpy_s(file_temp, file_path);
	wcscat_s(file_temp, L".tmp");
	_wfopen_s(&filetemp, file_temp, L"wb");
	/* to get the file size */
	if (_fstat32(_fileno(file), &file_info) != 0) {

		return 1; /* can't continue */
	}
	std::string sfilepath = base::SysWideToUTF8(file_path);
	pCurl = curl_easy_init();
	if (NULL != pCurl &&
		NULL != file) {

		struct curl_slist *headerlist = NULL;

		wchar_t fname[MAX_PATH] = { 0 };
		wchar_t ext[MAX_PATH] = { 0 };
		_wsplitpath_s(file_path, NULL, 0, NULL, 0, fname, MAX_PATH, ext, MAX_PATH);
		wcscat_s(fname, ext);
		if (wcslen(fname) > 0)
		{
			wchar_t file_name[MAX_PATH] = { 0 };
			wcscpy_s(file_name, L"FileName:");
			wcscat_s(file_name, fname);

			std::string sfilename = base::SysWideToUTF8(file_name);


			headerlist = curl_slist_append(headerlist, sfilename.c_str());
		}


		strcpy_s(user_agent, USER_AGENT);
		/* upload to this place */
		curl_easy_setopt(pCurl, CURLOPT_URL, url);

		//curl_easy_setopt(pCurl, CURLOPT_POST, 1);

		/* write to this file */
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, filetemp);

		curl_easy_setopt(pCurl, CURLOPT_READFUNCTION, read_callback);

		curl_easy_setopt(pCurl, CURLOPT_USERAGENT, user_agent);

		struct curl_httppost* post = NULL;
		struct curl_httppost* last = NULL;

		if (bPost)
		{
			curl_formadd(&post, &last, CURLFORM_COPYNAME, "filename", CURLFORM_COPYCONTENTS, fname, CURLFORM_END);
			curl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILE, sfilepath.data(), CURLFORM_END);
			curl_formadd(&post, &last, CURLFORM_COPYNAME, "submit", CURLFORM_COPYCONTENTS, "send", CURLFORM_END);
			curl_easy_setopt(pCurl, CURLOPT_HTTPPOST, post);
		}
		else
			curl_easy_setopt(pCurl, CURLOPT_UPLOAD, 1L);

		in_data.in = file;
		in_data.hnd = pCurl;

		curl_easy_setopt(pCurl, CURLOPT_READDATA, &in_data);

		curl_easy_setopt(pCurl, CURLOPT_INFILESIZE_LARGE,
			(curl_off_t)file_info.st_size);


		curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headerlist);

		/* enable verbose for easier tracing */
		curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(pCurl);

		if (NULL != post)
		{
			curl_formfree(post);
			//curl_formfree(last);
		}



		if (NULL != headerlist)
		{
			curl_slist_free_all(headerlist);
		}

	}

	if (NULL != file)
	{
		fclose(file);
		file = NULL;
	}

	if (NULL != filetemp)
	{
		fflush(filetemp);
		fclose(filetemp);
		filetemp = NULL;
	}

	::DeleteFile(file_temp);
	curl_easy_cleanup(pCurl);

	return res;
}

int CHttpClient::DownFile(const std::wstring & wszFilePath, const std::wstring & szUrl, unsigned int nMaxTry)
{
	return DownFile(wszFilePath, szUrl, nullptr, nMaxTry);
}

int CHttpClient::DownFile(const std::wstring & wszFilePath, const std::wstring & szUrl, const DownloadCB& func, unsigned int nMaxTry)
{
	prog.bStop = false;
	m_bRunning = true;
	auto iRet = Run(wszFilePath, szUrl, func, nMaxTry);
	m_bRunning = false;
	prog.bStop = true;

	return iRet;
}

int CHttpClient::Run(const std::wstring & wszFilePath, const std::wstring & szUrl, const DownloadCB& func, unsigned int nMaxTry)
{
	int iRet = CURLE_FAILED_INIT;

	BOOL bFindJumpUrl = FALSE;
	bool bCheckProxy = false;
	HTTP_PARAM httpData = { 0 };

	std::string surl = base::SysWideToUTF8(szUrl);
	UINT itry = nMaxTry;
	//首页head 查找信息.
	do
	{
		iRet = get_taget_header(surl.c_str(), httpData, bCheckProxy);
		if (iRet == CURLE_OK)
		{
			bFindJumpUrl = TRUE;
			break;
		}
		else
		{
			bCheckProxy = !bCheckProxy;
			Sleep(100);
		}
	} while (itry--);

	//再次 下载真正的数据包.
	if (bFindJumpUrl &&
		strlen(httpData.url_taget) > 3)
	{
		itry = nMaxTry;

		do
		{
			iRet = get_taget_file_ex(httpData.url_taget, wszFilePath.data(), func, false, bCheckProxy);
			if (iRet == CURLE_OK || iRet == CURLE_ABORTED_BY_CALLBACK)
			{
				break;

			}
			else
			{
				bCheckProxy = !bCheckProxy;
				Sleep(1000);
			}
		} while (itry--);
	}

	return iRet;
}

void CHttpClient::Stop(bool bWait)
{
	prog.bStop = true;

	if (bWait)
	{
		while (m_bRunning)
			Sleep(10);
	}
}


int CHttpClient::UploadFile(const std::wstring & wszFilePath, const std::wstring & szUrl, bool bPost, unsigned int nMaxTry)
{
	int iRet = CURLE_FAILED_INIT;

	if (wszFilePath.empty())
	{
		return iRet;
	}

	bool bCheckProxy = false;
	HTTP_PARAM httpData = { 0 };

	std::string surl = base::SysWideToUTF8(szUrl);
	UINT itry = nMaxTry;

	//上传数据包.
	if (szUrl.length() > 3)
	{
		itry = nMaxTry;

		do
		{
			iRet = put_taget_file(surl.c_str(), wszFilePath.data(), bPost, bCheckProxy);
			if (iRet == CURLE_OK || iRet == CURLE_ABORTED_BY_CALLBACK)
			{
				break;

			}
			else
			{
				bCheckProxy = !bCheckProxy;
				Sleep(1000);
			}
		} while (itry--);
	}

	return iRet;
}
