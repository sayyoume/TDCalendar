
#include "UDReportMgr.h"

#include "HttpClient.h"

using namespace std::placeholders;

CUGReportMgr::CUGReportMgr(size_t size)
	:m_stop{ false }, m_sUrl(L"http://collect.mdata.cool/mdata.php")
{
	size = 1;
	for (size_t i = 0; i < size; ++i)
	{
		std::future<void> m_future = std::async(std::launch::async, std::bind(&CUGReportMgr::schedualTask, this));
		m_pool.emplace_back(m_future.share());
	}

	m_fun = std::bind(&CUGReportMgr::postTaskEx, this, _1, _2, _3);
}

CUGReportMgr::~CUGReportMgr()
{

}

void CUGReportMgr::shutdown()
{
	this->m_stop.store(true);
	//this->m_cvtask.notify_all();

	for (auto& it : m_pool)
	{
		if (it.valid())
			it.wait();
	}
}

ReportInfo CUGReportMgr::getPostInfo()
{
	ReportInfo info;
	if (!m_infos.empty())
	{
		info = m_infos.front();
		m_infos.pop();
	}

	return info;
}

void CUGReportMgr::schedualTask()
{
	while (!m_stop.load())
	{
		std::unique_ptr<ReportInfo> pTemp = nullptr;
		m_mtx.lock();

		if (m_infos.size() >0)
		{
			pTemp = std::make_unique<ReportInfo>(m_infos.front());
			m_infos.pop();
		}

		m_mtx.unlock();

		if (pTemp != nullptr && !pTemp->info.empty() && !m_stop.load())
		{
			std::wstring response;
			m_fun(pTemp->url, pTemp->info, response);
		}

		pTemp = nullptr;

		Sleep(500);
		/*ReportInfo rinfo = getPostInfo();
		if (!rinfo.info.empty() && !m_stop.load())
		{
			std::wstring response;

			{
				wchar_t szBuf[2048] = L"";
				swprintf_s(szBuf, L"EVENT_: %s", rinfo.info.c_str());
				OutputDebugString(szBuf);
			}

			m_fun(rinfo.url, rinfo.info, response);
		}*/
	}
}


int CUGReportMgr::addTask(const std::wstring& info)
{
	return addTaskEx(m_sUrl, info);
}
 

int CUGReportMgr::addTaskEx(const std::wstring& url, const std::wstring& info)
{
	if (m_stop.load())
		return -1;
	ReportInfo rinfo(url, info);

	{
		//std::lock_guard<std::mutex> lock{ m_mtx };
		m_mtx.lock();
		m_infos.emplace(rinfo);
		m_mtx.unlock();
	}
	//m_cvtask.notify_one();
	return 0;
}


int CUGReportMgr::postTask(const std::wstring& info, std::wstring& response)
{

	return postTaskEx(m_sUrl, info, response);
}

int CUGReportMgr::postTaskEx(const std::wstring& url, const std::wstring& info, std::wstring& response)
{
	CHttpClient hc;
	return hc.ReportPost(url, info, response);
}