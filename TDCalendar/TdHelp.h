#pragma  once
#include <string>
#include <windows.h>


class TdHelp
{
public:
	TdHelp();
	~TdHelp();

	std::string ws2s(const std::wstring& wide, unsigned int CodePage = CP_UTF8);
	std::wstring s2ws(const std::string& mb, unsigned int CodePage = CP_UTF8);

	void ReplaceString(std::wstring& str, const std::wstring& src, const std::wstring& des); //×Ö·û´®Ìæ»»  std::wstring sTemp = L:abd,ino:  replacestring(sTemp,L",",L".");
public:


};