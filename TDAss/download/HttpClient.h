/************************************************************************/
/*
Date :   1/7/2016
File :   CHttpClient.h
Author : Eadwin
Funtion :
Version : 1.4
Copyright (c) 2014-2015 Oasgames
*/
/************************************************************************/


#ifndef __HTTP_CURL_H__
#define __HTTP_CURL_H__

#include <string>
#include <future>
#include "../Ass.h"


#define CURL_BUF_LEN 1024*512

typedef struct Http_Param
{
	long len;
	char url_taget[1024];
}HTTP_PARAM, *LPHTTP_PARAM;

typedef struct  User_Param
{
	FILE * file;
	bool range;

	char buf[CURL_BUF_LEN];
	long buf_pos;
}USER_PARAM, *LPUSER_PARAM;

#define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES         6000
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3

struct Curl_Progress 
{
	double lastruntime;
	void *curl;
};

/**
回调函数说明
pcurl :  curl handle 可以控制 暂定 继续 （curl_easy_pause）
dPos :  当前下载进度
dSpeed:  平均下载速度 (bytes/second)
dTime：  下载时间
dlnow: 已下载大小
dltotal: 总大小

返回值：
   0 继续下载
   1 停止下载（不是暂停）
*/
using DownloadCB  = std::function < int(void* pcurl, double dPos, double dSpeed, double dTime, __int64 dlnow, __int64 dltotal) >;

struct CURL_Progress
{
	double lastruntime;
	void *curl;
	DownloadCB func;
	bool bStop;

	CURL_Progress()
	{
		bStop = false;
		func  = nullptr;
	}
};

class APDDLL_DECLSPEC CHttpClient
{
public:
	CHttpClient(void);
	~CHttpClient(void);

public:

	static void InitNetwork();
	static void ReleaseNetwork();

	/*返回值类型均为  CURLcode（int）   
	* 0 （CURLE_OK） 为正确
	* 非0 为异常，查对CURLcode 对应信息*/
	/**
	*HTTP POST请求
	*/
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);
	int Post(const std::wstring & strUrl, const std::wstring & strPost, std::wstring & strResponse);
	/*
	* HTTP Report POST
	*/
	int ReportPost(const std::wstring & strUrl, const std::wstring & strPost, std::wstring & strResponse);
	/**
	* HTTP GET请求
	*/
	int Get(const std::string & strUrl, std::string & strResponse);
	int Get(const std::wstring & strUrl, std::wstring & strResponse);
	int Get(const std::wstring & strUrl, std::wstring & strResponse, bool bCheckProxy);

	/*
	//lRequestTimeout: 请求超时时间 单位（秒）
	//lConnectTimeout: 连接超时时间 单位（秒）
	*/
	int Get(const std::wstring & strUrl, std::wstring & strResponse, bool bCheckProxy, long lRequestTimeout, long lConnectTimeout);

	//HTTP GET异步请求 返回 std::future
	std::future<std::wstring> Get_Async(const std::wstring & strUrl);

	/**
	* HTTPS POST请求,无证书版本
	*/
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL);

	/**
	* HTTPS GET请求,无证书版本
	*/
	int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);

	/**
	* HTTP download
	* Auto Check Proxy
	*/
	int DownFile(const std::wstring & wszFilePath, const std::wstring & szUrl, unsigned int nMaxTry = 3);


	/*
	* HTTP upload 
	* Auto Check Proxy
	* 支持 post form & put 2种方式  （1.3 版本）
	*/
	int UploadFile(const std::wstring & wszFilePath, const std::wstring & szUrl, bool bPost = true, unsigned int nMaxTry = 3);


	/************************************************************************/
	/* HTTP  Download with progress                                         */
	/************************************************************************/
	int DownFile(const std::wstring & wszFilePath, const std::wstring & szUrl, const DownloadCB& func, unsigned int nMaxTry = 3);

	void Stop(bool bWait);
	CURL_Progress	prog;
public:
	void SetDebug(bool bDebug);
	
protected:
	int Run(const std::wstring & wszFilePath, const std::wstring & szUrl, const DownloadCB& func, unsigned int nMaxTry);
private:
	bool			m_bDebug;

	int get_taget_file_ex(const char * url, const wchar_t * file_path, const DownloadCB& func, bool bRange = true, bool bCheckProxy = true);
	int put_taget_file(const char * url, const wchar_t * file_path, bool bPost = true, bool bCheckProxy = true);
	int get_taget_header(const char * url, HTTP_PARAM &httpParam, bool bCheckProxy = true);
	bool get_proxy_enable(char * szIP, unsigned long uLen, unsigned short & dwPort);
	bool			m_bRunning;
};

#endif
