
#include "UGReadBin.h"
#include <windows.h>

namespace UG{namespace Lang{

	std::wstring ascllToUnicode( const std::string& s_src)
	{
		int iLen = s_src.size();
		int iSize = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s_src.c_str(), iLen, 0, 0);
		if(iSize <= 0) return std::wstring(L"");

		WCHAR *pwszDst = new WCHAR[iSize+1];
		if( NULL == pwszDst) return std::wstring(L"");
		MultiByteToWideChar(CP_UTF8, 0,(LPCSTR)s_src.c_str(), iLen, pwszDst, iSize);
		pwszDst[iSize] = 0;
		if( pwszDst[0] == 0xFEFF) // skip Oxfeff
			for(int i = 0; i < iSize; i ++) 
				pwszDst[i] = pwszDst[i+1]; 
		std::wstring s(pwszDst);
		delete pwszDst;

		return s;
	}

	bool UGReadBin::open(const std::wstring& sFile)
	{
		//m_pFile = _wfopen(sFile.c_str(),L"rb");
		_wfopen_s(&m_pFile,sFile.c_str(),L"rb");
		return m_pFile ? true : false;
	}

	void UGReadBin::close()
	{
		fclose(m_pFile);
	}

	bool UGReadBin::readDoubleByte(unsigned short& sResult)
	{
		return fread((void*)&sResult,sizeof(unsigned short),1,m_pFile) ? true : false;
	}

	bool UGReadBin::readDoubleByte2(short& sResult)
	{
		return fread((void*)&sResult,sizeof(short),1,m_pFile) ? true : false;
	}

	bool UGReadBin::readByte(unsigned char& cResult)
	{
		return fread((void*)&cResult,sizeof(unsigned char),1,m_pFile) ? true : false;
	}

	bool UGReadBin::readInt( unsigned int& iResult)
	{
		return fread((void*)&iResult,sizeof(unsigned int),1,m_pFile) ? true : false;
	}

	bool UGReadBin::readInt2(int& iResult)
	{
		return fread((void*)&iResult,sizeof(int),1,m_pFile) ? true : false;
	}

	bool UGReadBin::readString(std::wstring& swResult)
	{
		unsigned short usSize;
		readDoubleByte(usSize);
		if (usSize == 0)
		{
			swResult=L"";
			return true;
		}

		char* buff = new char[usSize];
		memset(buff,0,usSize);
		int b = fread(buff,usSize-1,1,m_pFile);

		unsigned char by;
		bool bResult = readByte(by) ? true : false;

		std::string s = buff;
		s.push_back(by);

		delete[] buff;

		if (bResult)
		{
			swResult = ascllToUnicode(s);
		}

		return bResult;
	}

	bool UGReadBin::readBool(bool& bResult)
	{
		return fread((void*)&bResult,sizeof(bool),1,m_pFile) ? true : false;
	}

	void UGReadBin::seek(unsigned long ipos,int iorgin)
	{
		fseek(m_pFile,ipos,iorgin);
	}

	unsigned long  UGReadBin::getFileSize()
	{
		fseek(m_pFile, 0, SEEK_END);
		return ftell(m_pFile);
	}

	void UGReadBin::readData(void* buf,unsigned long iStart,unsigned long iLen)
	{
		fseek(m_pFile,iStart,SEEK_SET);
		fread(buf,iLen,1,m_pFile);
	}

}}