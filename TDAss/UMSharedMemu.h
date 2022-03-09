#pragma once

#include "Ass.h"

namespace UM
{

	class APDDLL_DECLSPEC UMSharedMemu
	{
	public:
		static UMSharedMemu* shared();
		static bool  isNeedReboot();
		static void  rebootSystem();
		static void  setAutoRun(std::wstring& sKeyName);
		static void  clearAutoRun(std::wstring& sKeyName);
		static bool uninstall();
	public:
		UMSharedMemu();
	};
}

