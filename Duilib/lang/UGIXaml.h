
#ifndef __JTERSEIXAML_H__
#define __JTERSEIXAML_H__

#include <string>

namespace UG{namespace Interface{

	class UGIXaml
	{
	public:
		virtual bool open(const std::wstring& sFile)=0;
		virtual void close()=0; 
		virtual bool readDoubleByte(unsigned short& sResult)=0;
		virtual bool readDoubleByte2(short& sResult)=0;
		virtual bool readByte(unsigned char& cResult)=0;
		virtual	bool readInt( unsigned int& iResult)=0;
		virtual	bool readInt2(int& iResult)=0;
		virtual bool readString(std::wstring& swResult)=0;
		virtual bool readBool(bool& bResult)=0;
		virtual void seek(unsigned long ipos,int iorgin)=0;
		virtual unsigned long getFileSize() = 0;
		virtual void readData(void* buf, unsigned long iStart, unsigned long iLen) = 0;
	};
}}

#endif