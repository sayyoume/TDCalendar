
#pragma once

#include "../Ass.h"

#include <functional>
#include <thread>
#include <condition_variable>
#include <future>
#include <atomic>
#include <vector>
#include <queue>
#include <string>


using ReportTask = std::function<int(const std::wstring& url, const std::wstring& info, std::wstring& response)>;

struct ReportInfo
{
	std::wstring url;
	std::wstring info;

	ReportInfo()
	{
		this->url.empty();
		this->info.empty();
	}

	ReportInfo(const std::wstring& surl, const std::wstring& sinfo)
	{
		this->url = surl;
		this->info = sinfo;
	}
};

class  APDDLL_DECLSPEC CUGReportMgr final
{
public:
	CUGReportMgr(size_t size = 1);
	~CUGReportMgr();

	//void restart();
	void schedualTask();
	ReportInfo getPostInfo();

	/********************************************
	* 同步上报 （response 为返回值）
	*********************************************/
	//Mdata 用
	int postTask(const std::wstring& info, std::wstring& response);
	//其他上报如（DMP）
	int postTaskEx(const std::wstring& url, const std::wstring& info, std::wstring& response);

	/********************************************
	* 异步上报
	*********************************************/
	//Mdata 用
	int addTask(const std::wstring& info);
	//其他上报如（DMP）
	int addTaskEx(const std::wstring& url, const std::wstring& info);
	void shutdown();
private:
	// 线程池
	std::vector<std::shared_future<void>> m_pool;
	// 任务队列
	std::queue<ReportInfo> m_infos;
	// 同步
	std::mutex m_mtx;
	std::condition_variable m_cvtask;
	// 是否关闭提交
	std::atomic<bool> m_stop;

	ReportTask m_fun;

	std::wstring m_sUrl;
};