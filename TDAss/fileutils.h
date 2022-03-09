
#pragma once

#include <string>
#include "Ass.h"

namespace file
{
	// Returns true if the given path exists on the local filesystem,
	// false otherwise.
	bool APDDLL_DECLSPEC pathFileExists(const wchar_t* fileName);
	bool APDDLL_DECLSPEC pathFileExists(const std::wstring& fileName);
	std::wstring APDDLL_DECLSPEC getProgramRunDir();
	void APDDLL_DECLSPEC removeUninstall(const std::wstring& keyName);
	bool APDDLL_DECLSPEC writeUninstall(const std::wstring& sDisplayName, const std::wstring& sPath, const std::wstring& sParam, const std::wstring& sKeyName, const std::wstring& sVersion = L"", const std::wstring& sMainKey = L"");
	void APDDLL_DECLSPEC delSelf(std::wstring sDir);
}