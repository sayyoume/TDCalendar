
#pragma  once

#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <map>


#ifdef TDASS_EXPORTS
#define APDDLL_DECLSPEC __declspec(dllexport)
#else
#ifdef TDASS_LIB
#define UILIB_API
#else
#define APDDLL_DECLSPEC __declspec(dllimport)
#endif // UILIB_LIB
#endif // UILIB_EXPORTS

#pragma  warning(disable:4251)