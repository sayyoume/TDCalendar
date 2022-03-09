
#include "UGLangMgr.h"
#include <vector>

namespace UG{namespace Lang{

	UGLangMgr* UGLangMgr::shared()
	{
		static UGLangMgr gLangMgr;
		return &gLangMgr;
	}

	std::wstring UGLangMgr::getText(unsigned int iTextId)
	{
		std::map<unsigned int,std::wstring>::iterator it = mapLangTexts.find(iTextId);
		if (it != mapLangTexts.end())
			return it->second;

		return L"";
	}

	bool UGLangMgr::addText(unsigned int iTextId,std::wstring& sText)
	{
		std::map<unsigned int,std::wstring>::iterator it = mapLangTexts.find(iTextId);
		if (it != mapLangTexts.end())
			return false;

		mapLangTexts.insert(std::make_pair(iTextId,sText));
		return true;
	}

	bool UGLangMgr::removeText(unsigned int iTextId)
	{
		std::map<unsigned int,std::wstring>::iterator it = mapLangTexts.find(iTextId);
		if (it != mapLangTexts.end())
		{
			mapLangTexts.erase(it);
			return true;
		}

		return false;
	}

	void UGLangMgr::removeAll()
	{
		mapLangTexts.clear();
	}

	bool UGLangMgr::loadLang(UG::Interface::UGIXaml* pXaml,const std::wstring& sLangFile,const std::wstring& sLang)
	{
		if (!pXaml->open(sLangFile))
			return false;

		unsigned int iAddr;
		pXaml->readInt(iAddr);
		pXaml->seek(iAddr,SEEK_SET);
		pXaml->readInt(iAddr);

		std::vector<unsigned int> vec;

		for (int i=0; i< (int)iAddr; i++)
		{
			unsigned int iStart=0;
			unsigned int iLen=0;
			pXaml->readInt(iStart);
			pXaml->readInt(iLen);
			vec.push_back(iStart);
			vec.push_back(iLen);
		}

		bool bFind = false;
		int iSeekEn = 0;

		for (unsigned int j=0; j<(unsigned int)vec.size(); j+=2)
		{
			pXaml->seek(vec[j],SEEK_SET);
			std::wstring sKey;
			pXaml->readString(sKey);
			if (sKey == sLang)
			{
				bFind = true;
				break;
			}

			if (sKey == L"en")
				iSeekEn = vec[j];
		}

		if (!bFind)
		{
			pXaml->seek(iSeekEn,SEEK_SET);
			std::wstring sKey;
			pXaml->readString(sKey);
			int b=1;
		}

		unsigned int iLangSize=0;
		pXaml->readInt(iLangSize);


		for (unsigned int k=0; k< iLangSize; k++)
		{
			unsigned int iId=0;
			pXaml->readInt(iId);

			std::wstring s;
			pXaml->readString(s);

			mapLangTexts.insert(std::make_pair(iId,s));
		}

		pXaml->close();

		return iLangSize ?  true : false;
	}

}}