#pragma once

#include "Ass.h"

namespace UD{
	namespace log{
		APDDLL_DECLSPEC void InitLog(std::wstring sFileName = L"APPDATER.log");
		
#ifdef __cplusplus
		extern "C"
		{
#endif
			// ��������ͬJTERSE_InitProgramLog
			void JTERSE_DeleteProgramLog(LPCTSTR pProjectName = NULL, LPCTSTR pLogFilePath = NULL);

			// JTERSE_InitProgramLog�����Ĳ�������
			// DWORD dwMask�Ķ���
#define JTERSE_LOG_MASK_ALL		0xFFFFFFFF

// DWORD dwFlag�Ķ���
#define JTERSE_LOG_DELETE_MASK		0x0000000F	// ������־�Ĵ�С
#define JTERSE_LOG_DELETE_NO		0x00000000	// ���Զ�ɾ����־�ļ���dwMaxLogFileSize/dwMaxReserveSize�������κ�����
#define JTERSE_LOG_DELETE_INIT		0x00000001	// �ڳ�ʼ����ʱ������ļ���С��ɾ����־�ļ�
#define JTERSE_LOG_DELETE_CLOSE		0x00000002	// �ڹرյ�ʱ������ļ���С��ɾ����־�ļ�
#define JTERSE_LOG_DELETE_WRITE		0x00000004	// ��д��־��ʱ������ļ���С��ɾ����־�ļ�

#define JTERSE_LOG_RELEASE			0x80000000	// Release�汾Ҳ�����־��Ĭ��ֻ��Debug�汾�����־
#define JTERSE_LOG_SAFE				0x40000000	// ��־�ļ��Ƿ����
#define JTERSE_LOG_PROJECTNAME		0x20000000	// ��־�ļ��Ƿ���ʾ��Ŀ����
#define JTERSE_LOG_TIME				0x10000000	// ��־�ļ��Ƿ���ʾʱ���

#define JTERSE_FLAG_INIT			(JTERSE_LOG_DELETE_NO | JTERSE_LOG_PROJECTNAME | JTERSE_LOG_TIME)

#define __JTERSE_LOG_ENABLE__

#ifdef __JTERSE_LOG_ENABLE__

			// DWORD dwMask ���룬������Щ��־��Ҫ����д��
			// DWORD dwFlag ��־λ
			// DWORD dwLogBufSize ��־�������Ĵ�С��0Ϊ��ʹ�û��壬-1Ϊʹ��Ĭ�ϻ����С(100K)������Ϊʵ�ʻ������Ĵ�С
			// DWORD dwMaxLogFileSize ��־�ļ������ߴ�(��λ��KB)��0�����Զ�ɾ����־�ļ�
			// DWORD dwMaxReserveSize ��־�ļ���������ߴ�(��λ��KB)��0����ȫ��ɾ����������
			// LPCTSTR pProjectName ��Ŀ���ƣ����ΪNULL��ȡexe�ļ���
			// LPCTSTR pLogFilePath ��־�ļ�·��������Ϊ����·�������·�������Ϊ���·����ǰ�����exe���ڵ�Ŀ¼�����ΪNULL����Ϊexe���ڵ�Ŀ¼+log\\+pProjectName.log
			
			void APDDLL_DECLSPEC JTERSE_InitProgramLog(DWORD dwMask = JTERSE_LOG_MASK_ALL, DWORD dwFlag = JTERSE_FLAG_INIT, DWORD dwLogBufSize = -1, DWORD dwMaxLogFileSize = 0, DWORD dwMaxReserveSize = 0, LPCTSTR pProjectName = NULL, LPCTSTR pLogFilePath = NULL);
			void APDDLL_DECLSPEC JTERSE_InitProgramLogEx(DWORD dwMask = JTERSE_LOG_MASK_ALL, LPCTSTR pProjectName = NULL, LPCTSTR pLogFilePath = NULL);

			void APDDLL_DECLSPEC JTERSE_CloseProgramLog();

			void APDDLL_DECLSPEC JTERSE_FlushProgramLog();	// �ѻ������־д���ļ�

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
