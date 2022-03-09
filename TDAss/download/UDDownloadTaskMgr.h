#pragma once
#include "../Ass.h"
#include <deque>
#include <mutex>
#include <future>

namespace UGDOWNLOAD{

	class APDDLL_DECLSPEC CDownloadTaskBase
	{
	public:
		CDownloadTaskBase(){}
		virtual ~CDownloadTaskBase(){}
		virtual void Run() = 0;
	};

	class APDDLL_DECLSPEC CUGDownloadTaskMgr
	{
	public:
		CUGDownloadTaskMgr();
		virtual ~CUGDownloadTaskMgr();

		void AddTask(CDownloadTaskBase *pTask);
	protected:
		void RunTask();
	private:
		std::deque<CDownloadTaskBase*> m_dqTask;
		std::mutex m_mutex;
		std::future<void> m_pFuture;
		volatile bool m_bRun;
	};

}