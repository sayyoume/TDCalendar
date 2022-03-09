
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
	* ͬ���ϱ� ��response Ϊ����ֵ��
	*********************************************/
	//Mdata ��
	int postTask(const std::wstring& info, std::wstring& response);
	//�����ϱ��磨DMP��
	int postTaskEx(const std::wstring& url, const std::wstring& info, std::wstring& response);

	/********************************************
	* �첽�ϱ�
	*********************************************/
	//Mdata ��
	int addTask(const std::wstring& info);
	//�����ϱ��磨DMP��
	int addTaskEx(const std::wstring& url, const std::wstring& info);
	void shutdown();
private:
	// �̳߳�
	std::vector<std::shared_future<void>> m_pool;
	// �������
	std::queue<ReportInfo> m_infos;
	// ͬ��
	std::mutex m_mtx;
	std::condition_variable m_cvtask;
	// �Ƿ�ر��ύ
	std::atomic<bool> m_stop;

	ReportTask m_fun;

	std::wstring m_sUrl;
};