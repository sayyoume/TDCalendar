
#pragma  once

#include "../Ass.h"
#include <string>

namespace Oasgames{

	class APDDLL_DECLSPEC MutexProcess
	{
	public:
		MutexProcess(const std::wstring& sProcessName, const std::wstring& sGuid);
		virtual ~MutexProcess(){}

		int init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpstrCmdLine, int nCmdShow);
	protected:
		virtual int run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpstrCmdLine, int nCmdShow){ return 0; }
		virtual void exitProcessClean(){}
		virtual void existProcess(){}
	protected:
		std::wstring				 m_sGuid;
		std::wstring				 m_sName;
	};
}
