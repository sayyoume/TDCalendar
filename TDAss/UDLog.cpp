
#define _CRT_SECURE_NO_WARNINGS 
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <windows.h>

#include <time.h>
#include <TCHAR.h>
#include <io.h>
#include <stdio.h>
#include <mutex>


#include "UDLog.h"

namespace UD{namespace log{

#define FLAG_ISSET(flag,value)					(((flag) & (value))==(value))

#define MAX_LOGPATHLENGTH		520
#define MAX_LOG_BUFFER			(1024*100)
#define MAX_LOG_SINGLEBUFFER	(1024*10)

	static void JTERSE_GetProgramLogPath(LPCTSTR pProjectName,LPCTSTR pLogFilePath,TCHAR* lpszPathOut)
	{
		if(pProjectName==NULL && pLogFilePath==NULL)
			return;

		TCHAR path_buffer[MAX_LOGPATHLENGTH];
		if(::GetModuleFileName(NULL,path_buffer,MAX_LOGPATHLENGTH)==0)
			return;

		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];
		_tsplitpath(path_buffer,drive,dir,fname,ext);

		if(pLogFilePath==NULL)
		{
			_tcscpy(lpszPathOut,drive);
			_tcscat(lpszPathOut,dir);
			_tcscat(lpszPathOut,_T("log\\"));
			_tcscat(lpszPathOut,pProjectName);
			_tcscat(lpszPathOut,_T(".LOG"));
		}
		else if(_tcslen(pLogFilePath)>=2 && (pLogFilePath[1]==_T(':') || (pLogFilePath[0]==_T('\\') && pLogFilePath[1]==_T('\\'))))
		{
			_tcscpy(lpszPathOut,pLogFilePath);
		}
		else
		{
			_tcscpy(lpszPathOut,drive);
			_tcscat(lpszPathOut,dir);
			_tcscat(lpszPathOut,pLogFilePath);
		}
	}

	void JTERSE_DeleteProgramLog(LPCTSTR pProjectName,LPCTSTR pLogFilePath)
	{
		if(pProjectName==NULL && pLogFilePath==NULL)
			return;

		TCHAR tcLogFilePath[MAX_LOGPATHLENGTH];
		JTERSE_GetProgramLogPath(pProjectName,pLogFilePath,tcLogFilePath);

		SetFileAttributes(tcLogFilePath,FILE_ATTRIBUTE_NORMAL);
		DeleteFile(tcLogFilePath);
	}
	void InitLog(std::wstring sFileName)
	{
		wchar_t buff[1024] = { 0 };
		GetEnvironmentVariable(L"TEMP", buff, 1024);
		std::wstring sLog = buff;
		sLog.append(L"\\UG\\");
		sLog.append(sFileName);
		JTERSE_InitProgramLog(JTERSE_LOG_MASK_ALL, JTERSE_FLAG_INIT | JTERSE_LOG_RELEASE, 0, 0, 0, NULL, sLog.data());
	}

#ifdef __JTERSE_LOG_ENABLE__
	
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	typedef struct tagJTERSE_PROGRAMLOGPARAM
	{
		BOOL m_bInit;								// 是否正确初始化
		DWORD m_dwMask;								// 掩码
		DWORD m_dwFlag;								// 标志位
		DWORD m_dwMaxLogFileSize;					// 日志文件的最大尺寸(单位：KB)，0代表不自动删除日志文件
		DWORD m_dwMaxReserveSize;					// 日志文件的最大保留尺寸(单位：KB)，0代表全部删除，不保留
		DWORD m_dwInitTickCount;					// 初始时间
		TCHAR *m_Buffer;							// 缓冲区
		DWORD m_dwBufferPos;						// 缓冲区当前位置
		DWORD m_dwMaxBufferSize;					// 缓冲区最大值
		TCHAR m_strLogFilePath[MAX_LOGPATHLENGTH];	// 日志文件路径
		TCHAR m_strProjectName[MAX_LOGPATHLENGTH];	// 项目名称
	}JTERSE_PROGRAMLOGPARAM,*PJTERSE_PROGRAMLOGPARAM;

	static JTERSE_PROGRAMLOGPARAM m_GlobalParam={0};

	std::mutex m_gLogMutex;

#define JTERSE_LOG_Init				(m_GlobalParam.m_bInit)
#define JTERSE_LOG_dwMask				(m_GlobalParam.m_dwMask)
#define JTERSE_LOG_dwFlag				(m_GlobalParam.m_dwFlag)
#define JTERSE_LOG_dwMaxLogFileSize	(m_GlobalParam.m_dwMaxLogFileSize)
#define JTERSE_LOG_dwMaxReserveSize	(m_GlobalParam.m_dwMaxReserveSize)
#define JTERSE_LOG_dwInitTickCount		(m_GlobalParam.m_dwInitTickCount)
#define JTERSE_LOG_Buffer				(m_GlobalParam.m_Buffer)
#define JTERSE_LOG_dwBufferPos			(m_GlobalParam.m_dwBufferPos)
#define JTERSE_LOG_dwMaxBufferSize		(m_GlobalParam.m_dwMaxBufferSize)
#define JTERSE_LOG_strLogFilePath		(m_GlobalParam.m_strLogFilePath)
#define JTERSE_LOG_strProjectName		(m_GlobalParam.m_strProjectName)

	static void JTERSE_WriteFile(HANDLE hFile,LPVOID pBuf,DWORD dwLength)
	{
		DWORD dwRet=0;
		WriteFile(hFile,pBuf,dwLength,&dwRet,NULL);
	}

	static void JTERSE_SafeWriteFile(HANDLE hFile,LPVOID pBuf,DWORD dwLength)
	{
		LPBYTE p=(LPBYTE)pBuf;
		BYTE bSeed=0x5A;

		for(DWORD i=0;i<dwLength;i++)
		{
			if(p[i]!=0 && p[i]!=bSeed)
				p[i]^=bSeed;
		}

		JTERSE_WriteFile(hFile,p,dwLength);
	}

	static void JTERSE_WriteFileBuf(HANDLE hFile,LPVOID pBuf,DWORD dwBufLength)
	{
		if(FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_SAFE))
			JTERSE_SafeWriteFile(hFile,pBuf,dwBufLength);
		else
			JTERSE_WriteFile(hFile,pBuf,dwBufLength);
	}

#define JTERSE_WriteFileString(hFile,lpszLog)	JTERSE_WriteFileBuf(hFile,(LPBYTE)(lpszLog),(lpszLog)==NULL ? 0 : _tcslen(lpszLog)*sizeof(TCHAR))

	static void JTERSE_TruncLogFile(HANDLE hFile)
	{
		DWORD dwFileSize=GetFileSize(hFile,NULL);

		if(JTERSE_LOG_dwMaxLogFileSize!=0 && dwFileSize>JTERSE_LOG_dwMaxLogFileSize)
		{
			// 文件大小满足要求
			if(JTERSE_LOG_dwMaxReserveSize>0 && JTERSE_LOG_dwMaxReserveSize<JTERSE_LOG_dwMaxLogFileSize)
			{
				// 保留部分数据
				SetFilePointer(hFile,dwFileSize-JTERSE_LOG_dwMaxReserveSize,0,FILE_BEGIN);

				BYTE *pBuf=new BYTE[JTERSE_LOG_dwMaxReserveSize];
				if(pBuf==NULL)
				{
					SetFilePointer(hFile,0,0,FILE_END);
					return;
				}

				DWORD dwRead=0;
				DWORD dwRet;

				while(dwRead<JTERSE_LOG_dwMaxReserveSize)
				{
					if(!ReadFile(hFile,pBuf+dwRead,JTERSE_LOG_dwMaxReserveSize-dwRead,&dwRet,NULL))
						break;
					if(dwRet>=0)
						dwRead+=dwRet;
				}

				if(dwRead<JTERSE_LOG_dwMaxReserveSize)
				{
					delete[] pBuf;
					SetFilePointer(hFile,0,0,FILE_END);
					return;
				}

				SetFilePointer(hFile,0,0,FILE_BEGIN);

#ifdef _UNICODE
				WORD format=0xFEFF;			// little-endian UNICODE
				JTERSE_WriteFileBuf(hFile,(LPBYTE)&format,sizeof(WORD));
#endif	// _UNICODE

				JTERSE_WriteFileBuf(hFile,pBuf,JTERSE_LOG_dwMaxReserveSize);

				delete[] pBuf;
			}
			else
			{
				SetFilePointer(hFile,0,0,FILE_BEGIN);

#ifdef _UNICODE
				WORD format=0xFEFF;			// little-endian UNICODE
				JTERSE_WriteFileBuf(hFile,(LPBYTE)&format,sizeof(WORD));
#endif	// _UNICODE
			}

			SetEndOfFile(hFile);
		}
	}

	static void JTERSE_WriteLogFile(DWORD dwFlag,LPCTSTR lpszLog)
	{
		if(!JTERSE_LOG_Init)
			return;

		SetFileAttributes(JTERSE_LOG_strLogFilePath,FILE_ATTRIBUTE_NORMAL);

		HANDLE hFile=CreateFile(JTERSE_LOG_strLogFilePath,GENERIC_WRITE | GENERIC_READ,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile==INVALID_HANDLE_VALUE)
			return;

		if((dwFlag & JTERSE_LOG_dwFlag & JTERSE_LOG_DELETE_INIT)==JTERSE_LOG_DELETE_INIT ||
			(dwFlag & JTERSE_LOG_dwFlag & JTERSE_LOG_DELETE_CLOSE)==JTERSE_LOG_DELETE_CLOSE ||
			(dwFlag & JTERSE_LOG_dwFlag & JTERSE_LOG_DELETE_WRITE)==JTERSE_LOG_DELETE_WRITE)
		{
			JTERSE_TruncLogFile(hFile);
		}

		SetFilePointer(hFile,0,0,FILE_END);

#ifdef _UNICODE
		DWORD dwFileSize=GetFileSize(hFile,NULL);
		if(dwFileSize==0)
		{
			WORD format=0xFEFF;			// little-endian UNICODE
			JTERSE_WriteFileBuf(hFile,(LPBYTE)&format,sizeof(WORD));
		}
#endif	// _UNICODE

		JTERSE_WriteFileString(hFile,lpszLog);

		CloseHandle(hFile);
	}

	BOOL MyCreateDirectory(LPCTSTR lpszPath)
	{
		TCHAR tcPath[MAX_PATH];
		_tcscpy_s(tcPath,MAX_PATH,lpszPath);

		for(DWORD i=0;i<_tcslen(tcPath);i++)
		{
			if(tcPath[i]==_T('/'))
				tcPath[i]=_T('\\');
		}

		TCHAR *p=tcPath+3;

		while(TRUE)
		{
			p=_tcsstr(p,_T("\\"));

			if(p==NULL)
			{
//			CreateDirectory(strPath,NULL);
				break;
			}
			else
			{
				*p=_T('\0');
				CreateDirectory(tcPath,NULL);
				*p=_T('\\');
				p++;
			}
		}

		return TRUE;
	}

    void JTERSE_InitProgramLogEx(DWORD dwMask,LPCTSTR pProjectName,LPCTSTR pLogFilePath)
    {
        JTERSE_InitProgramLog(dwMask,JTERSE_FLAG_INIT,0,5*1024*1024,2*1024*1024,pProjectName,pLogFilePath);
    }



	void JTERSE_InitProgramLog(DWORD dwMask,DWORD dwFlag,DWORD dwLogBufSize,DWORD dwMaxLogFileSize,DWORD dwMaxReserveSize,LPCTSTR pProjectName,LPCTSTR pLogFilePath)
	{
		if(JTERSE_LOG_Init)
			return;

#if !defined(_DEBUG)
		if(!FLAG_ISSET(dwFlag,JTERSE_LOG_RELEASE))
		{
			ZeroMemory(&m_GlobalParam,sizeof(m_GlobalParam));
			JTERSE_LOG_Init=FALSE;
			return;
		}
#endif

		if(dwMaxReserveSize>=dwMaxLogFileSize)
			dwMaxReserveSize=0;

		TCHAR tcProgramFilePath[MAX_LOGPATHLENGTH]={0};
		if(::GetModuleFileName(NULL,tcProgramFilePath,MAX_LOGPATHLENGTH)==0)
			return;

		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];
		_tsplitpath(tcProgramFilePath,drive,dir,fname,ext);

		ZeroMemory(&m_GlobalParam,sizeof(m_GlobalParam));
		JTERSE_LOG_dwMask=dwMask;
		JTERSE_LOG_dwFlag=dwFlag;
		JTERSE_LOG_dwMaxLogFileSize=dwMaxLogFileSize;
		JTERSE_LOG_dwMaxReserveSize=dwMaxReserveSize;
		JTERSE_LOG_dwInitTickCount=GetTickCount();

		if(pProjectName==NULL)
			_tcscpy(JTERSE_LOG_strProjectName,fname);
		else
			_tcscpy(JTERSE_LOG_strProjectName,pProjectName);

		JTERSE_GetProgramLogPath(JTERSE_LOG_strProjectName,pLogFilePath,JTERSE_LOG_strLogFilePath);

		MyCreateDirectory(JTERSE_LOG_strLogFilePath);

		if(dwLogBufSize==0)
		{
			JTERSE_LOG_Buffer=NULL;
			JTERSE_LOG_dwBufferPos=0;
			JTERSE_LOG_dwMaxBufferSize=0;
		}
		else
		{
			if(dwLogBufSize==(DWORD)-1)
				JTERSE_LOG_dwMaxBufferSize=MAX_LOG_BUFFER;
			else
			{
				JTERSE_LOG_dwMaxBufferSize=dwLogBufSize;
				if(JTERSE_LOG_dwMaxBufferSize>10*1024*1024)
					JTERSE_LOG_dwMaxBufferSize=10*1024*1024;
			}
			JTERSE_LOG_Buffer=new TCHAR[JTERSE_LOG_dwMaxBufferSize];
			JTERSE_LOG_dwBufferPos=0;
			if(JTERSE_LOG_Buffer==NULL)
				return;
		}

		JTERSE_LOG_Init=TRUE;

		m_gLogMutex.lock();

		TCHAR tcBuf[MAX_LOG_SINGLEBUFFER];
		_stprintf(tcBuf,_T("\r\n\r\n"));

		if(FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_PROJECTNAME))
			_stprintf(tcBuf+_tcslen(tcBuf),_T("Project Name: %s\r\n"),JTERSE_LOG_strProjectName);

		_stprintf(tcBuf+_tcslen(tcBuf),_T("Project Path<%u.%u>: %s\r\n"),GetCurrentProcessId(),GetCurrentThreadId(),tcProgramFilePath);

		if(FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_TIME))
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			_stprintf(tcBuf+_tcslen(tcBuf),_T("Start Time: %10d %04d-%02d-%02d %02d:%02d:%02d\r\n"),JTERSE_LOG_dwInitTickCount,
				st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		}

		_stprintf(tcBuf+_tcslen(tcBuf),_T("\r\n"));

		JTERSE_WriteLogFile(JTERSE_LOG_DELETE_INIT | JTERSE_LOG_DELETE_WRITE,tcBuf);

		m_gLogMutex.unlock();
	}

	void JTERSE_CloseProgramLog()
	{
		if(JTERSE_LOG_Init)
		{
			m_gLogMutex.lock();

			if(JTERSE_LOG_Buffer!=NULL && JTERSE_LOG_dwBufferPos>0)
			{
				JTERSE_WriteLogFile(JTERSE_LOG_DELETE_WRITE,JTERSE_LOG_Buffer);
				JTERSE_LOG_dwBufferPos=0;
			}

			TCHAR tcProgramFilePath[MAX_LOGPATHLENGTH]={0};
			if(::GetModuleFileName(NULL,tcProgramFilePath,MAX_LOGPATHLENGTH)==0)
				return;

			TCHAR tcBuf[MAX_LOG_SINGLEBUFFER];
			tcBuf[0]=_T('\0');

			if(FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_PROJECTNAME))
				_stprintf(tcBuf+_tcslen(tcBuf),_T("\r\nProject Name: %s\r\n"),JTERSE_LOG_strProjectName);

			_stprintf(tcBuf+_tcslen(tcBuf),_T("Project Path<%u.%u>: %s\r\n"),GetCurrentProcessId(),GetCurrentThreadId(),tcProgramFilePath);

			if(FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_TIME))
			{
				SYSTEMTIME st;
				GetLocalTime(&st);
				_stprintf(tcBuf+_tcslen(tcBuf),_T("End Time: %10d %04d-%02d-%02d %02d:%02d:%02d\r\n"),GetTickCount(),
					st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			}

			JTERSE_WriteLogFile(JTERSE_LOG_DELETE_CLOSE | JTERSE_LOG_DELETE_WRITE,tcBuf);

			if(JTERSE_LOG_Buffer!=NULL)
			{
				delete[] JTERSE_LOG_Buffer;
				JTERSE_LOG_Buffer=NULL;
			}
			JTERSE_LOG_dwBufferPos=0;

			m_gLogMutex.unlock();
		}

		if(JTERSE_LOG_Init)
		{
			JTERSE_LOG_Init=FALSE;
		}
	}

	void JTERSE_FlushProgramLog()	// 把缓存的日志写入文件
	{
		if(!JTERSE_LOG_Init)
			return;

		m_gLogMutex.lock();

		JTERSE_WriteLogFile(JTERSE_LOG_DELETE_WRITE,JTERSE_LOG_Buffer);
		JTERSE_LOG_dwBufferPos=0;
		m_gLogMutex.unlock();
	}

	void JTERSE_WriteProgramLogString(DWORD dwMask,LPCTSTR lpszLogText)
	{
		if(!JTERSE_LOG_Init)
			return;

		if(!FLAG_ISSET(JTERSE_LOG_dwMask,dwMask))
			return;

		TCHAR tcBuf[MAX_LOG_SINGLEBUFFER];

		if(FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_PROJECTNAME) && FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_TIME))
		{
			DWORD uTick=GetTickCount();
			_stprintf(tcBuf,_T("%10d %10.3f %s:: %s\r\n"),uTick,((double)uTick-JTERSE_LOG_dwInitTickCount)/CLOCKS_PER_SEC,JTERSE_LOG_strProjectName,lpszLogText);
		}
		else if(FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_PROJECTNAME) && !FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_TIME))
		{
			_stprintf(tcBuf,_T("%s:: %s\r\n"),JTERSE_LOG_strProjectName,lpszLogText);
		}
		else if(!FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_PROJECTNAME) && FLAG_ISSET(JTERSE_LOG_dwFlag,JTERSE_LOG_TIME))
		{
			DWORD uTick=GetTickCount();
			_stprintf(tcBuf,_T("%10d %10.3f %s\r\n"),uTick,((double)uTick-JTERSE_LOG_dwInitTickCount)/CLOCKS_PER_SEC,lpszLogText);
		}
		else
		{
			_stprintf(tcBuf,_T("%s\r\n"),lpszLogText);
		}

		m_gLogMutex.lock();

		if(JTERSE_LOG_Buffer!=NULL)
		{
			if(JTERSE_LOG_dwBufferPos+_tcslen(tcBuf)*sizeof(TCHAR)<JTERSE_LOG_dwMaxBufferSize)
			{
				memcpy(JTERSE_LOG_Buffer+JTERSE_LOG_dwBufferPos,tcBuf,_tcslen(tcBuf)*sizeof(TCHAR));
				JTERSE_LOG_dwBufferPos+=_tcslen(tcBuf);
				JTERSE_LOG_Buffer[JTERSE_LOG_dwBufferPos]=_T('\0');
			}
			else
			{
				JTERSE_WriteLogFile(JTERSE_LOG_DELETE_WRITE,JTERSE_LOG_Buffer);
				JTERSE_LOG_dwBufferPos=0;

				JTERSE_WriteLogFile(JTERSE_LOG_DELETE_WRITE,tcBuf);
			}
		}
		else
			JTERSE_WriteLogFile(JTERSE_LOG_DELETE_WRITE,tcBuf);

		m_gLogMutex.unlock();
	}

	inline TCHAR Hex(char x)
	{
		if(x>=0 && x<=9)
			return x+_T('0');
		else if(x>=10 && x<=16)
			return x-10+_T('A');

		return _T(' ');
	}

	void JTERSE_WriteProgramLogBin(DWORD dwMask,LPCTSTR lpszFront,LPCTSTR lpszBack,LPCTSTR lpszBuf,DWORD uBufLength)
	{
		if(!JTERSE_LOG_Init)
			return;

		if(!FLAG_ISSET(JTERSE_LOG_dwMask,dwMask))
			return;

		TCHAR tcBuf[MAX_LOG_SINGLEBUFFER];
		tcBuf[0]=_T('\0');

		if(lpszFront!=NULL)
		{
			_tcscat(tcBuf,lpszFront);
		}

		if((lpszBuf!=NULL) && (uBufLength>0))
		{
			TCHAR *pText;

			DWORD uNewBufLen;
#ifdef _UNICODE
			uNewBufLen=uBufLength*5;
#else
			uNewBufLen=uBufLength*3;
#endif	// _UNICODE

			pText=(TCHAR*)malloc(uNewBufLen*sizeof(TCHAR));
			if(pText==NULL)
				return;

			DWORD uPos;
			for(DWORD i=0;i<uBufLength;i++)
			{
#ifdef _UNICODE
				uPos=i*5;
				pText[uPos+0]=Hex((lpszBuf[i]>>12) & 0x0F);
				pText[uPos+1]=Hex((lpszBuf[i]>>8) & 0x0F);
				pText[uPos+2]=Hex((lpszBuf[i]>>4) & 0x0F);
				pText[uPos+3]=Hex(lpszBuf[i] & 0x0F);
				pText[uPos+4]=_T(' ');
#else
				uPos=i*3;
				pText[uPos+0]=Hex((lpszBuf[i]>>4) & 0x0F);
				pText[uPos+1]=Hex(lpszBuf[i] & 0x0F);
				pText[uPos+2]=_T(' ');
#endif	// _UNICODE
			}

#ifdef _UNICODE
			pText[uPos+4]=_T('\0');
#else
			pText[uPos+2]=_T('\0');
#endif	// _UNICODE

			_tcscat(tcBuf,pText);

			free(pText);
		}

		if(lpszBack!=NULL)
		{
			_tcscat(tcBuf,lpszBack);
		}

		JTERSE_WriteProgramLogString(dwMask,tcBuf);
	}

	void JTERSE_WriteProgramLog(DWORD dwMask,LPCTSTR lpszFormat,...)
	{
		if(!JTERSE_LOG_Init)
			return;

		if(!FLAG_ISSET(JTERSE_LOG_dwMask,dwMask))
			return;

		TCHAR tcBuf[MAX_LOG_SINGLEBUFFER];

		va_list arglist;
		va_start(arglist,lpszFormat);

		_vstprintf(tcBuf,lpszFormat,arglist);

		JTERSE_WriteProgramLogString(dwMask,tcBuf);
	}

	void JTERSE_WriteProgramLogNoMask(LPCTSTR lpszFormat,...)
	{
		if(!JTERSE_LOG_Init)
			return;

		TCHAR tcBuf[MAX_LOG_SINGLEBUFFER];

		va_list arglist;
		va_start(arglist,lpszFormat);

		_vstprintf(tcBuf,lpszFormat,arglist);

		JTERSE_WriteProgramLogString(-1,tcBuf);
	}



#endif	// __JTERSE_LOG_ENABLE__
}}


