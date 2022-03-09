#include "TdHelp.h"

TdHelp::TdHelp()
{

}

TdHelp::~TdHelp()
{

}


std::string TdHelp::ws2s(const std::wstring& wide, unsigned int CodePage)
{
	int wide_length = static_cast<int>(wide.length());
	if (wide_length == 0)
		return std::string();

	// Compute the length of the buffer we'll need.
	int charcount = WideCharToMultiByte(CodePage, 0, wide.data(), wide_length,
		NULL, 0, NULL, NULL);
	if (charcount == 0)
		return std::string();

	std::string mb;
	mb.resize(charcount);
	WideCharToMultiByte(CodePage, 0, wide.data(), wide_length,
		&mb[0], charcount, NULL, NULL);

	return mb;
}


std::wstring TdHelp::s2ws(const std::string& mb, unsigned int CodePage)
{
	if (mb.empty())
		return std::wstring();

	int mb_length = static_cast<int>(mb.length());
	// Compute the length of the buffer.
	int charcount = MultiByteToWideChar(CodePage, 0,
		mb.data(), mb_length, NULL, 0);
	if (charcount == 0)
		return std::wstring();

	std::wstring wide;
	wide.resize(charcount);
	MultiByteToWideChar(CodePage, 0, mb.data(), mb_length, &wide[0], charcount);

	return wide;
}

void TdHelp::ReplaceString(std::wstring& str, const std::wstring& src, const std::wstring& des)
{
	int pos = -1;
	int curPos = 0;
	while (-1 != (pos = str.find(src, curPos)))
	{
		str.replace(pos, src.size(), des);
		curPos = pos + des.size();
	}
}
