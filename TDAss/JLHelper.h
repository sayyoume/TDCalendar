
#pragma once

#include <string>

#ifndef MAX_PATHEX
#define MAX_PATHEX MAX_PATH*2
#endif

#define  JMakeAttribute(name,param,_type)\
    inline void set##name(_type param){\
    this->param = param;}\
    inline _type get##name(){ return this->param;}


#ifndef SAFEDEL
#define SAFEDEL(ptr) { \
    if(NULL != ptr)\
    {\
    delete ptr;\
    ptr = NULL;\
    }\
   }
#endif
#ifndef SAFEDELARRAY
#define SAFEDELARRAY(ptr) { \
    if(NULL != ptr)\
    {\
    delete[] ptr;\
    ptr = NULL;\
    }\
}
#endif

#ifdef UNICODE
typedef std::wstring	 _tstring;
#else
typedef std::string	 _tstring;
#endif

