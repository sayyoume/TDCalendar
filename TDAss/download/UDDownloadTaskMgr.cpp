#include "UDDownloadTaskMgr.h"
#include <windows.h>


namespace UGDOWNLOAD{

	CUGDownloadTaskMgr::CUGDownloadTaskMgr()
		:m_bRun(false)
	{
	}


	CUGDownloadTaskMgr::~CUGDownloadTaskMgr()
	{
		//m_bRun = false;
		if (m_pFuture.valid())
			m_pFuture.get();

		assert(m_dqTask.empty());
	}

	void CUGDownloadTaskMgr::AddTask(CDownloadTaskBase *pTask)
	{
		if (pTask == NULL) return;

		m_mutex.lock();
		m_dqTask.push_back(pTask);
		bool bRun = m_bRun;
		m_mutex.unlock();

		// 线程还在运行，就不用再创建了
		if (bRun) return;

		RunTask();
	}

	void CUGDownloadTaskMgr::RunTask()
	{
		// 如果上一个线程还没有彻底退出,等待结束
		if (m_pFuture.valid())
			m_pFuture.get();

		m_bRun = true;
		m_pFuture = std::async(std::launch::async, [&]
		{
			while (m_bRun)
			{
				CDownloadTaskBase *pTask = NULL;
				m_mutex.lock();
				m_bRun = !m_dqTask.empty();
				if (m_bRun)
				{
					pTask = m_dqTask.front();
					m_dqTask.pop_front();
				}
				m_mutex.unlock();

				if (pTask == NULL) break;

				pTask->Run();
				delete pTask;
			}
		});
	}
}