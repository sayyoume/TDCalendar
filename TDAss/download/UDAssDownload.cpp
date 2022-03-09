#include "UDAssDownload.h"
#include "../fileutils.h"


namespace UGDOWNLOAD{

	UGDownload::UGDownload(const std::wstring& sUrl, std::wstring& sSave, UGITaskDownloadCallBack* pcallback)
	{
		m_sUrl = sUrl;
		m_sSave = sSave;
		m_pCallBack = pcallback;
	}


	UGDownload::~UGDownload()
	{
		if (m_pDObj != NULL)
		{
			m_pDObj->StopDownload();
			delete m_pDObj;
			m_pDObj = NULL;
		}
	}


	void UGDownload::OnDownloadUpdated(net::CHttpDownload* download)
	{
		//当前进度  下载速度 已下载 剩余大小
		if (m_pCallBack !=NULL)
		{
			m_pCallBack->downloadPropress(download->getProgress(), download->getRate(), download->getTransfered(), download->getMissingTransfer());
		}
	}

	void UGDownload::OnDownloadFinish(net::CHttpDownload* download)
	{
		if (m_pCallBack != NULL)
		{
			m_pCallBack->finish();
		}

	}

	void UGDownload::Run()
	{
		m_pDObj = new net::CHttpDownload();
		m_pDObj->SetObserver(this);
		m_pDObj->Download(m_sUrl,m_sSave, 3);

		delete m_pDObj;
		m_pDObj = NULL;
	}

}

