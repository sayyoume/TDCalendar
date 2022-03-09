#pragma once

#include "Ass.h"

namespace UD{
	namespace log{
		APDDLL_DECLSPEC void InitLog(std::wstring sFileName = L"APPDATER.log");
		
#ifdef __cplusplus
		extern "C"
		{
#endif
			// 参数定义同JTERSE_InitProgramLog
			void JTERSE_DeleteProgramLog(LPCTSTR pProjectName = NULL, LPCTSTR pLogFilePath = NULL);

			// JTERSE_InitProgramLog函数的参数定义
			// DWORD dwMask的定义
#define JTERSE_LOG_MASK_ALL		0xFFFFFFFF

// DWORD dwFlag的定义
#define JTERSE_LOG_DELETE_MASK		0x0000000F	// 控制日志的大小
#define JTERSE_LOG_DELETE_NO		0x00000000	// 不自动删除日志文件，dwMaxLogFileSize/dwMaxReserveSize参数无任何意义
#define JTERSE_LOG_DELETE_INIT		0x00000001	// 在初始化的时候根据文件大小来删除日志文件
#define JTERSE_LOG_DELETE_CLOSE		0x00000002	// 在关闭的时候根据文件大小来删除日志文件
#define JTERSE_LOG_DELETE_WRITE		0x00000004	// 在写日志的时候根据文件大小来删除日志文件

#define JTERSE_LOG_RELEASE			0x80000000	// Release版本也输出日志，默认只有Debug版本输出日志
#define JTERSE_LOG_SAFE				0x40000000	// 日志文件是否加密
#define JTERSE_LOG_PROJECTNAME		0x20000000	// 日志文件是否显示项目名称
#define JTERSE_LOG_TIME				0x10000000	// 日志文件是否显示时间戳

#define JTERSE_FLAG_INIT			(JTERSE_LOG_DELETE_NO | JTERSE_LOG_PROJECTNAME | JTERSE_LOG_TIME)

#define __JTERSE_LOG_ENABLE__

#ifdef __JTERSE_LOG_ENABLE__

			// DWORD dwMask 掩码，控制哪些日志需要真正写入
			// DWORD dwFlag 标志位
			// DWORD dwLogBufSize 日志缓冲区的大小，0为不使用缓冲，-1为使用默认缓冲大小(100K)，其它为实际缓冲区的大小
			// DWORD dwMaxLogFileSize 日志文件的最大尺寸(单位：KB)，0代表不自动删除日志文件
			// DWORD dwMaxReserveSize 日志文件的最大保留尺寸(单位：KB)，0代表全部删除，不保留
			// LPCTSTR pProjectName 项目名称，如果为NULL，取exe文件名
			// LPCTSTR pLogFilePath 日志文件路径，可以为绝对路径和相对路径，如果为相对路径，前面添加exe所在的目录，如果为NULL，则为exe所在的目录+log\\+pProjectName.log
			
			void APDDLL_DECLSPEC JTERSE_InitProgramLog(DWORD dwMask = JTERSE_LOG_MASK_ALL, DWORD dwFlag = JTERSE_FLAG_INIT, DWORD dwLogBufSize = -1, DWORD dwMaxLogFileSize = 0, DWORD dwMaxReserveSize = 0, LPCTSTR pProjectName = NULL, LPCTSTR pLogFilePath = NULL);
			void APDDLL_DECLSPEC JTERSE_InitProgramLogEx(DWORD dwMask = JTERSE_LOG_MASK_ALL, LPCTSTR pProjectName = NULL, LPCTSTR pLogFilePath = NULL);

			void APDDLL_DECLSPEC JTERSE_CloseProgramLog();

			void APDDLL_DECLSPEC JTERSE_FlushProgramLog();	// 把缓存的日志写入文件

			void APDDLL_DECLSPEC JTERSE_WriteProgramLogString(DWORD dwMask, LPCTSTR lpszLogText);
			void APDDLL_DECLSPEC JTERSE_WriteProgramLogBin(DWORD dwMask, LPCTSTR lpszFront, LPCTSTR lpszBack, LPCTSTR lpszBuf, DWORD uBufLength);
			void APDDLL_DECLSPEC JTERSE_WriteProgramLog(DWORD dwMask, LPCTSTR lpszFormat, ...);
			void APDDLL_DECLSPEC JTERSE_WriteProgramLogNoMask(LPCTSTR lpszFormat, ...);

#define JTERSE_WriteProgramLogStringNoMask(lpszLogText)						JTERSE_WriteProgramLogString(JTERSE_LOG_MASK_ALL,lpszLogText)
#define JTERSE_WriteProgramLogBinNoMask(lpszFront,lpszBack,lpszBuf,nBufLength)	JTERSE_WriteProgramLogBin(JTERSE_LOG_MASK_ALL,lpszFront,lpszBack,lpszBuf,nBufLength)

#else

#define InitLog __noop
#define JTERSE_InitProgramLog														__noop
#define JTERSE_CloseProgramLog()													__noop
#define JTERSE_FlushProgramLog()													__noop
#define JTERSE_WriteProgramLogString(dwMask,lpszLogText)							__noop
#define JTERSE_WriteProgramLogBin(dwMask,lpszFront,lpszBack,lpszBuf,nBufLength)	__noop
#define JTERSE_WriteProgramLog														__noop
#define JTERSE_WriteProgramLogNoMask												__noop

#define JTERSE_WriteProgramLogStringNoMask(lpszLogText)							__noop
#define JTERSE_WriteProgramLogBinNoMask(lpszFront,lpszBack,lpszBuf,nBufLength)		__noop

#endif

#ifdef __cplusplus
		}
#endif

}}
