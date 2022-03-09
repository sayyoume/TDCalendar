#pragma  once
#include "../Ass.h"
#include "dl/httpdownload.h"
#include "UDDownloadTaskMgr.h"


namespace UGDOWNLOAD{

	class APDDLL_DECLSPEC UGITaskDownloadCallBack
	{
	public:
		//当前进度  下载速度 已下载 剩余大小
		virtual void downloadPropress(double fCurPos, double iSpeed, __int64 iCurSize, __int64 iFileSize) = 0;
		virtual void finish() = 0;
	};

	class  APDDLL_DECLSPEC UGDownload : public net::Observer, public CDownloadTaskBase
	{
	public:
		UGDownload(const std::wstring& sUrl, std::wstring& sSave, UGITaskDownloadCallBack* pcallback);
		~UGDownload();
		virtual void Run() override;
	private:
		void OnDownloadUpdated(net::CHttpDownload* download);
		void OnDownloadFinish(net::CHttpDownload* download);
	private:
		net::CHttpDownload* m_pDObj;
		UGITaskDownloadCallBack* m_pCallBack;
		std::wstring m_sUrl;
		std::wstring m_sSave;

	};
}
