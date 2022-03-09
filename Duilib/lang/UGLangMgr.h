
#ifndef __JTERSELANGMGR__H__
#define __JTERSELANGMGR__H__

#include "UGIXaml.h"
#include <map>

namespace UG{namespace Lang{

	class  UGLangMgr
	{
	public:
		static UGLangMgr* shared();

		bool loadLang(UG::Interface::UGIXaml* pXaml,const std::wstring& sLangFile,const std::wstring& sLang);
		std::wstring getText(unsigned int iTextId);
		bool addText(unsigned int iTextId,std::wstring& sText);
		bool removeText(unsigned int iTextId);
		void removeAll();

		std::map<unsigned int,std::wstring>* getTextPtr(){return &mapLangTexts;}
	private:
		std::map<unsigned int,std::wstring> mapLangTexts;

	private:
		UGLangMgr(){}
		~UGLangMgr(){}
	};
}}

#endif