/************************************************************************/
/* 
	Date : 4/2/2014
    File : httpdownload.h
    Author : Eadwin Li
    Function : 
    Version : 1.0
    Copyright (c) 2012 - 2014 OASGAME Studio
*/
/************************************************************************/  

#if !defined(_HTTPDOWNLOAD_H__)
#define _HTTPDOWNLOAD_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../JLHelper.h"
#include <sys/timeb.h>
#include <windows.h>
#include <Wininet.h>
#pragma comment(lib, "wininet.lib")

namespace net{

    BOOL _CreateDirectory(LPCTSTR lpFileName,
        BOOL    bIsFile,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes);

    enum HTTPDownloadStatus
    {
        DL_PREPARE = 0,
        DL_START,
        DL_WORKING,
        DL_COMPLETE,
    };

     class CHttpDownload;
    ////////////////////////////////////////////////////////////////////////////////
    //�ص���Ϣ
    class Observer {
    public:
        virtual void OnDownloadUpdated(CHttpDownload* download){}
        //virtual void OnDownloadStart(CHttpDownload* download){}
        //virtual void OnDownloadCancel(CHttpDownload* download){}
        virtual void OnDownloadFinish(CHttpDownload* download){}

    protected:
        virtual ~Observer() {}
    };

    // �ṹ����, ����������Ϣ
    typedef struct _tagHttpDownloadInfo
    {
        DWORDLONG	dwFileSize;					    // �ļ��ܵĴ�С
        DWORDLONG	dwFileDownloadedSize;		    // �Ѿ����ش�С
        _tstring	strFileName;				    // �ļ���
    }HTTPDOWNLOADINFO,*PHTTPDOWNLOADINFO;
    // �����Զ�����Ϣ�ṹ״̬����,��ӦHTTPDOWNLOADINFO.nStatusType
    enum
    {
        DOWNLOAD_STATUS_FILENAME			= 1, // �ļ���
        DOWNLOAD_STATUS_FILESIZE			= 2,	// �ļ���С
        DOWNLOAD_STATUS_DOWNLOADEDSIZE		= 3,	// �ļ��Ѿ����ش�С
        DOWNLOAD_STATUS_DOWNLOADCOMPLETE    = 4, // �������
    };

    // Download ��������ֵ
    enum
    {
        ERR_HTTP_SUCCESS				=	0x00000000,			// ���سɹ�
        ERR_HTTP_FAIL					=	0xFFFFFFFF,			// ����ʧ��
        ERR_HTTP_REDIRECT				=   0x00000001,			// �ض���HTTP
        ERR_HTTP_USER_STOP              =   0x00000002,			// �û�ֹͣ����(������StopDownload����)
        ERR_HTTP_INVALID_URL			=   0x80000001,         // ��ЧURL
        ERR_HTTP_DEST_FILENAME_EMPTY	=   0x80000002,			// ���ش洢�ļ�Ϊ��
        ERR_HTTP_CREATE_FILE_FAIL		=   0x80000003,			// ���������ļ�ʧ��
        ERR_HTTP_RENAME_FILE_FAIL       =   0x80000004,			// �����������ļ�ʧ��
        ERR_HTTP_SEND_TIMEOUT           =   0x80000005,			// ����ʱ
        ERR_HTTP_BUFFER_TOO_SMALL       =   0x80000006,			// buffer too samll
        ERR_HTTP_NOT_FIND				=   0x80000007,			// ���󲻴���
        ERR_HTTP_NET_ERROR              =   0x80000008,         // �������
        //HTTP_RESULT_CONNECT_FAIL    =   0x80000002,         // CONNECT Error 
        ERR_HTTP_RECEIVE_TIMEOUT        =   0x80000009,			// ����ʱ
        ERR_HTTP_FORBIDDEN_ACCESS       =   0x80000010,			// �ܾ�����
        ERR_HTTP_SERVER_ERROR           =   0x80000011,			// ����������internal server error����500
        ERR_HTTP_URL_OR_DATA_NULL           =   0x80000012,			// download�������Ϊ��
    };

    // SetConnectType����
    enum
    {
        CONNECT_TYPE_IE                = 0,	// ʹ�ãɣ�����
        CONNECT_TYPE_DIRECT            = 1, // ֱ������
        CONNECT_TYPE_PROXY             = 3  // ʹ�ô���
    };

    //���ʱ�䣨���粻ͬ״̬��
    const long long DL_TimeSpan_MaxValue = 0x7FFFFFFFFFFFFFFF;
    const unsigned int DL_RetryDelay = 5000;

    //class CDataItem;
    class CHttpDownload  
    {
    public:
        //////////////////////////////////////////////////////////////////////////////////
        //	��������HRESULT Download(LPCTSTR lpszURL,
        //				LPCTSTR lpszSaveFileName
        //				BOOL    bAutoRedirect /*= FALSE*/)
        //	��  ;��������ڣ����ô˺�������ʼ���ع���
        //	��ȫ�ֱ�����Ӱ�죺��
        //	��  ����
        //		lpszURL			  : [in]�����ص�����URL
        //		lpszSaveFileName  : [in]���ر����ļ���
        //      bAutoRedirect     : [in]�Ƿ��Զ��ض���
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
        bool Download(LPCTSTR lpszURL, LPCTSTR lpszSaveFileName, 
            BOOL bAutoRedirect = FALSE, LPCTSTR lpszResetHost = NULL);


        //////////////////////////////////////////////////////////////////////////////////
        //	��������HRESULT Download(_tstring& szURL,
        //				_tstring& szSaveFileName��
        //				unsigned int    trytimes /*= FALSE*/)
        //	��  ;��������ڣ����ô˺�������ʼ���ع���
        //	��ȫ�ֱ�����Ӱ�죺��
        //	��  ����
        //		lpszURL			  : [in]�����ص�����URL
        //		lpszSaveFileName  : [in]���ر����ļ���(��Ҫ�õ��ض���URLʱ����ΪNULL)
        //      trytimes          : [in]���Դ���
        //		
        //  ֧���Զ��ض�������
        //	
        /////////////////////////////////////////////////////////////////////////////////
        bool Download(_tstring& szURL, _tstring& szSaveFileName, unsigned int trytimes);




        //////////////////////////////////////////////////////////////////////////////////
        //	��������HRESULT Download(_tstring& szURL,
        //				_tstring& szSaveFileName	/*= NULL*/��
        //				unsigned int    trytimes /*= FALSE*/)
        //	��  ;��������ڣ����ô˺�������ʼ���ع���
        //	��ȫ�ֱ�����Ӱ�죺��
        //	��  ����
        //		lpszURL			  : [in]�����ص�����URL
        //		lpszSaveFileName  : [in]���ر����ļ���(��Ҫ�õ��ض���URLʱ����ΪNULL)
        //      trytimes          : [in]���Դ���
        //		
        //  ֧���Զ��ض�������
        //	
        /////////////////////////////////////////////////////////////////////////////////
        bool Download(_tstring& szURL, HANDLE hMap, DWORDLONG d64StartPos, DWORDLONG d64EndPos);


        //////////////////////////////////////////////////////////////////////////////////
        //	��������void StopDownload()
        //	��  ;��ֹͣ����
        //	��ȫ�ֱ�����Ӱ�죺��
        //	��  ������
        //	����ֵ����
        /////////////////////////////////////////////////////////////////////////////////
        void    StopDownload();
        void    ContinueDownload();
        //////////////////////////////////////////////////////////////////////////////////
        //	��������BOOL SetObserver( Observer* lpob )
        //	��  ;�����ý���������Ϣ�Ĵ��ھ��
        //	��ȫ�ֱ�����Ӱ�죺��
        //	��  ����lpob : [in]����������Ϣ����ָ��
        //	����ֵ����
        /////////////////////////////////////////////////////////////////////////////////
        void	SetObserver(Observer* lpob);

        //////////////////////////////////////////////////////////////////////////////////
        //	��������LPCTSTR GetRedirectURL()
        //	��  ;���õ��ض����ģգң�
        //	��ȫ�ֱ�����Ӱ�죺��
        //	��  ������
        //	����ֵ��URL�ַ���
        //  ˵��  : ������Download�󷵻�ֵΪHTTP_RES_REDIRECT���ٵ��ô˺����õ��ض���URL
        /////////////////////////////////////////////////////////////////////////////////
        LPCTSTR  GetRedirectURL();

        //////////////////////////////////////////////////////////////////////////////////
        //	��������BOOL SetProxy( LPCTSTR lpszProxyServer,
        //					USHORT nProxyPort, 
        //					BOOL bUseProxy /* =TRUE */,
        //					BOOL bProxyAuthorization /* =FALSE */,
        //					LPCTSTR lpszProxyUsername /* =NULL */,
        //					LPCTSTR lpszProxyPassword /* =NULL */ )
        //	��  ;�����ô�����������ʲ���
        //	��ȫ�ֱ�����Ӱ�죺��
        //	��  ����
        //		lpszProxyServer		: [in]�����������
        //      nProxyPort			: [in]����������˿�
        //		bUseProxy			: [in]�Ƿ�ʹ�ô���(��ֵΪFALSEʱ��ǰ����������������)
        //		bProxyAuthorization : [in]����������Ƿ���Ҫ�����֤
        //		lpszProxyUsername   : [in]��������������֤�û���
        //		lpszProxyUsername   : [in]��������������֤����
        //	����ֵ���ɹ���TRUE��ʧ��FALSE
        /////////////////////////////////////////////////////////////////////////////////
        BOOL SetProxy(LPCTSTR lpszProxyServer,
            USHORT nProxyPort, 
            BOOL bUseProxy = TRUE,
            BOOL bProxyAuthorization = FALSE,
            LPCTSTR lpszProxyUsername = NULL,
            LPCTSTR lpszProxyPassword = NULL);

        //////////////////////////////////////////////////////////////////////////////////
        //	��������void SetAuthorization(
        //				LPCTSTR lpszUsername,
        //				LPCTSTR lpszPassword,
        //				BOOL bAuthorization/* = TRUE*/ ) 
        //	��  ;������WWW��֤��Ϣ(���ʱ�������ҳ��ʱ��Ҫ)
        //	��ȫ�ֱ�����Ӱ�죺��
        //	��  ����
        //		lpszUsername   : [in]����ҳ����û���
        //		lpszPassword   : [in]����
        //		bAuthorization : [in]�Ƿ���Ҫ��֤(��ֵΪFALSEʱ��ǰ����������������)
        //	����ֵ��void
        ////////////////////////////////////////////////////////////////////////////////
        void SetAuthorization(LPCTSTR lpszUsername,LPCTSTR lpszPassword,BOOL bAuthorization = TRUE);

        //////////////////////////////////////////////////////////////////////////////////
        //	��������void SetConnectType(INT nType /*= CONNECT_TYPE_IE*/) 
        //	��  ;��������������
        //	��ȫ�ֱ�����Ӱ�죺��
        //	��  ����
        //		nType   : [in]���ͣ�CONNECT_TYPE_IE/* 0 - IE; 1 - ֱ������; 3 - Proxy*/��
        //	����ֵ��void
        ////////////////////////////////////////////////////////////////////////////////
        void SetConnectType(INT nType = CONNECT_TYPE_IE/* 0 - IE; 1 - ֱ������; 3 - Proxy*/) { m_nConnectType = nType; }

        DWORD GetLastError(){return m_dwLastError;}

        DWORDLONG GetFileSize() {return m_dwFileSize;}

        _tstring GetObjectName(){return m_strObjectName;}

        DWORDLONG GetFileSizeByUrl(LPCTSTR strUrl);

        bool GetFileInfoByUrl(LPCTSTR strUrl,bool& bAsseptRage,_tstring& strContentype,_tstring& sFileName,DWORDLONG& dlFileSize);

        //��ǰ�Ƿ����ڹ���
        bool IsWorking();

        //��ȡ����
        double getProgress();

        //��ȡ�����ٶ�
        double getRate();

        //��ȡ������
        __int64 getTransfered();

        //��ȡʣ��ʱ��
        __time64_t getTimeLeft();

        //ʣ���С
        __int64 CHttpDownload::getMissingTransfer();

		// ���ó�ʱ��
		void SetTimerOut(DWORD dwTimeOut){ m_dwTimeOut = dwTimeOut; }

		DWORD GetTimerOut(){ return m_dwTimeOut; }

		_tstring GetSavePath(){ return m_strSavePath; }
    public:
        CHttpDownload();
        virtual ~CHttpDownload();
        BOOL ParseUrl(LPCTSTR lpszUrl);
    protected:

        BOOL Connect();
        BOOL HttpDateToSystemTime(LPCTSTR lpszTime, SYSTEMTIME& st);
        BOOL CloseHandles();
        BOOL Notify(HTTPDownloadStatus msg);
        void IncreaseStartPosition(__int64 size);
        void BeginWork();

    protected:
        HINTERNET m_hInternet;					// internet open handle
        HINTERNET m_hConnect;					// internet connection handle
        HINTERNET m_hRequest;					// internet request handle

        // ��������������
        _tstring            m_strUserAgent;         // UserAgent (IE version;os version)
        _tstring		    m_strServerName;		// ��������(hostname,���磺www.rising.com.cn)
        INTERNET_PORT       m_nServerPort;			// �������˿�
        BOOL			    m_bAuthorization;		// ���������Ƿ���Ҫ�����֤
        _tstring		    m_strUsername;			// ����������½�û���
        _tstring	        m_strPassword;			// ����������½����
        _tstring		    m_strObjectName;		// ���URL·��(���磺/register/pcver/AutoUpgradePad/ver2005/RSAGENT/NewVer/RsAgent_exe.zip)
		_tstring			m_strSavePath;			// ����·��

        // ��Դ����������
        _tstring			m_strProxyServer;		// �����������
        INTERNET_PORT	    m_nProxyPort;			// ����������˿�
        BOOL			    m_bUseProxy;			// �Ƿ�ʹ�ô��������
        BOOL			    m_bProxyAuthorization;	// ����������Ƿ���Ҫ�����֤
        _tstring			m_strProxyUsername;     // �������������½�û���
        _tstring			m_strProxyPassword;		// �������������½����

        BOOL			    m_bStop;				// �û���ֹ����������
        BOOL			    m_bServerAlter;         // �������ʱ���ڶ���URL��ַ��HostName�Ƿ�ı�

        _tstring            m_strRedirectURL;       // �ض����ַ
        Observer*			m_Observer;             // ����������Ϣ��

        DWORD			    m_dwLastError;          // �洢�ڲ�������
        DWORD               m_nConnectType;
        DWORDLONG		    m_dwFileSize;

        HTTPDownloadStatus  m_dlstate;              //��ǰ״̬
        HTTPDOWNLOADINFO    m_httpDownInfo;         //��ǰ��Ϣ
        bool started_;                              //�Ƿ�ʼ
        double rate_;                               //�����ٶ�
        __int64 start_;                             //�𲽴�С
        __time64_t left_;                           //ʣ���С
        __timeb64 lastReception_;                   //�ϴμ�¼ʱ��

		DWORD m_dwTimeOut = 30 * 1000;				// ��ʱʱ��
    };
}



#endif // !defined(_HTTPDOWNLOAD_H__)
