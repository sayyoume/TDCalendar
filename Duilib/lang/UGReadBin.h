
#ifndef __JTERSEREADBIN_H__
#define __JTERSEREADBIN_H__

#include "UGIXaml.h"

namespace UG{namespace Lang{

	class UGReadBin : public UG::Interface::UGIXaml
	{
	public:

		//override
		virtual bool open(const std::wstring& sFile);
		virtual void close(); 
		virtual bool readDoubleByte(unsigned short& sResult);
		virtual bool readDoubleByte2(short& sResult);
		virtual bool readByte(unsigned char& cResult);
		virtual	bool readInt( unsigned int& iResult);
		virtual	bool readInt2(int& iResult);
		virtual bool readString(std::wstring& swResult);
		virtual bool readBool(bool& bResult);
		virtual void seek(unsigned long ipos,int iorgin);
		virtual unsigned long getFileSize();
		virtual void readData(void* buf,unsigned long iStart,unsigned long iLen);

	private:
		FILE* m_pFile;
	};

}}


#endif