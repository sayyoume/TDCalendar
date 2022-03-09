
#include "UGLang.h"
#include "UGReadBin.h"
#include <winnls.h>

namespace UG{
	namespace Lang{

		std::wstring UG::Lang::LoadLang(LPCTSTR lpszLangFile,LPCTSTR lplangue)
		{
			std::wstring sLang = L"";
			if (lplangue == NULL || _tcsclen(lplangue) == 0 )
			{
				DWORD lcid = ::GetSystemDefaultLCID();

				switch (lcid)
				{
				case 1055:
				{
					sLang = L"tr";
					break;
				}
 				case 1046:
 				{
					sLang = L"pt";
 					break;
 				}
				case 3082:
				case 2058:
 				{
					sLang = L"es";
					break;
				}
				case 2052:
				{
					sLang = L"zh";
					break;
				}
				default:
					sLang = L"en";
					break;
				}
			}
			else
			{
				sLang = lplangue;
			}
			
			UGReadBin readBin;
			UG::Lang::UGLangMgr::shared()->removeAll();
			UG::Lang::UGLangMgr::shared()->loadLang(&readBin, lpszLangFile, sLang);
			return sLang;
		}

		void LoadLang2(LPCTSTR lpszLangFile, LPCTSTR lplangue)
		{
			std::wstring sLang = L"";

			if (lplangue == NULL || _tcsclen(lplangue) == 0)
			{
				DWORD lcid = ::GetSystemDefaultLCID();

				switch (lcid)
				{
				case 1055:
				{
					sLang = L"tr";
					break;
				}
// 				case 1046:
// 				{
// 					sLang = L"pt";
// 					break;
// 				}
// 				case 1066:
// 				{
// 					sLang = L"vi";
// 					break;
// 				}
				case 2052:
				{
					sLang = L"zh";
					break;
				}
				default:
					sLang = L"en";
					break;
				}
			}
			else
			{
				sLang = lplangue;
			}

			UGReadBin readBin;
			UG::Lang::UGLangMgr::shared()->loadLang(&readBin, lpszLangFile, sLang);
		}

		std::wstring getLang(int id)
		{
			return UG::Lang::UGLangMgr::shared()->getText(id);
		}
	}
}


