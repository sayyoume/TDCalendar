
#pragma once

#include "../Ass.h"

#include <windows.h>
#include <stdio.h>

APDDLL_DECLSPEC BOOL WINAPI WriteFileDynamically(HANDLE hFile, LPCVOID lpBuffer,
                          DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped);

APDDLL_DECLSPEC HANDLE WINAPI CreateFileDynamically(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);

APDDLL_DECLSPEC BOOL WINAPI CloseHandleDynamiclly(HANDLE hObject);

APDDLL_DECLSPEC BOOL WINAPI DeleteFileDynamiclly(LPCWSTR lpFileName);


APDDLL_DECLSPEC HMODULE WINAPI GetModuleHandleADynamiclly(LPCSTR lpModuleName);




//shell32
APDDLL_DECLSPEC HINSTANCE ShellExecuteDynamiclly(
    _In_opt_  HWND hwnd,
    _In_opt_  LPCWSTR lpOperation,
    _In_      LPCWSTR lpFile,
    _In_opt_  LPCWSTR lpParameters,
    _In_opt_  LPCWSTR lpDirectory,
    _In_      INT nShowCmd
    );


//USER32.dll
APDDLL_DECLSPEC int WINAPI LoadStringDynamiclly(
    _In_opt_  HINSTANCE hInstance,
    _In_      UINT uID,
    _Out_     LPWSTR lpBuffer,
    _In_      int nBufferMax
    );

APDDLL_DECLSPEC HICON WINAPI LoadIconDynamiclly(
    _In_opt_  HINSTANCE hInstance,
    _In_      LPCWSTR lpIconName
    );

APDDLL_DECLSPEC int __cdecl wsprintfDynamiclly(
    _Out_  LPWSTR lpOut,
    _In_   LPCWSTR lpFmt,
    _In_    ...
    );


//RPCRT4.dll

APDDLL_DECLSPEC RPC_STATUS
    RPC_ENTRY
    RpcStringFreeDynamiclly (
     RPC_WSTR __RPC_FAR * String
    );


APDDLL_DECLSPEC RPC_STATUS
    RPC_ENTRY
    UuidToStringDynamiclly (
     const UUID __RPC_FAR * Uuid,
     RPC_WSTR __RPC_FAR * StringUuid
    );


APDDLL_DECLSPEC RPC_STATUS RPC_ENTRY UuidCreateDynamiclly(
    UUID __RPC_FAR *Uuid
    );




//WS2_32.dll
struct hostent* FAR gethostbynameDynamiclly(
    const char *name
    );


APDDLL_DECLSPEC char FAR * __stdcall FAR inet_ntoaDynamiclly(struct in_addr in);


APDDLL_DECLSPEC int __stdcall WSACleanupDynamiclly(void);


APDDLL_DECLSPEC int __stdcall WSAStartupDynamiclly(WORD wVersionRequested, LPWSADATA lpWSAData);



//ADVAPI32.dll
APDDLL_DECLSPEC BOOL WINAPI OpenProcessTokenDynamiclly(HANDLE ProcessHandle, DWORD DesiredAccess,
                                       PHANDLE TokenHandle);
APDDLL_DECLSPEC BOOL WINAPI LookupPrivilegeValueWDynamiclly(
    LPCTSTR lpSystemName,LPCTSTR lpName,PLUID lpLuid);

APDDLL_DECLSPEC BOOL WINAPI AdjustTokenPrivilegesDynamiclly(
      HANDLE TokenHandle,
      BOOL DisableAllPrivileges,
      PTOKEN_PRIVILEGES NewState,
      DWORD BufferLength,
      PTOKEN_PRIVILEGES PreviousState,
      PDWORD ReturnLength);

APDDLL_DECLSPEC BOOL WINAPI ConvertStringSidToSidWDynamiclly(
    LPCWSTR StringSid,
    PSID *Sid);



APDDLL_DECLSPEC BOOL WINAPI CreateProcessAsUserWDynamiclly(
    HANDLE hToken,
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation);


APDDLL_DECLSPEC BOOL WINAPI GetTokenInformationDynamiclly(
    HANDLE TokenHandle,
    TOKEN_INFORMATION_CLASS TokenInformationClass,
    LPVOID TokenInformation,
    DWORD TokenInformationLength,
    PDWORD ReturnLength
    );


APDDLL_DECLSPEC BOOL WINAPI DuplicateTokenExDynamiclly(
    _In_      HANDLE hExistingToken,
    _In_      DWORD dwDesiredAccess,
    _In_opt_  LPSECURITY_ATTRIBUTES lpTokenAttributes,
    _In_      SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
    _In_      TOKEN_TYPE TokenType,
    _Out_     PHANDLE phNewToken
    );



APDDLL_DECLSPEC BOOL WINAPI SetTokenInformationDynamiclly(
    _In_  HANDLE TokenHandle,
    _In_  TOKEN_INFORMATION_CLASS TokenInformationClass,
    _In_  LPVOID TokenInformation,
    _In_  DWORD TokenInformationLength
    );


APDDLL_DECLSPEC BOOL WINAPI SetSecurityDescriptorDaclDynamiclly(
    _Inout_   PSECURITY_DESCRIPTOR pSecurityDescriptor,
    _In_      BOOL bDaclPresent,
    _In_opt_  PACL pDacl,
    _In_      BOOL bDaclDefaulted
    );


APDDLL_DECLSPEC BOOL WINAPI QueryServiceStatusExDynamiclly(
    _In_       SC_HANDLE hService,
    _In_       SC_STATUS_TYPE InfoLevel,
    _Out_opt_  LPBYTE lpBuffer,
    _In_       DWORD cbBufSize,
    _Out_      LPDWORD pcbBytesNeeded
    );

APDDLL_DECLSPEC BOOL WINAPI QueryServiceStatusDynamiclly(
    _In_   SC_HANDLE hService,
    _Out_  LPSERVICE_STATUS lpServiceStatus
    );



APDDLL_DECLSPEC BOOL WINAPI ControlServiceDynamiclly(
    _In_   SC_HANDLE hService,
    _In_   DWORD dwControl,
    _Out_  LPSERVICE_STATUS lpServiceStatus
    );


APDDLL_DECLSPEC BOOL WINAPI SetServiceStatusDynamiclly(
    _In_  SERVICE_STATUS_HANDLE hServiceStatus,
    _In_  LPSERVICE_STATUS lpServiceStatus
    );


APDDLL_DECLSPEC BOOL WINAPI StartServiceWDynamiclly(
    _In_      SC_HANDLE hService,
    _In_      DWORD dwNumServiceArgs,
    _In_opt_  LPCWSTR *lpServiceArgVectors
    );


APDDLL_DECLSPEC BOOL WINAPI ChangeServiceConfigWDynamiclly(
    _In_       SC_HANDLE hService,
    _In_       DWORD dwServiceType,
    _In_       DWORD dwStartType,
    _In_       DWORD dwErrorControl,
    _In_opt_   LPCWSTR lpBinaryPathName,
    _In_opt_   LPCWSTR lpLoadOrderGroup,
    _Out_opt_  LPDWORD lpdwTagId,
    _In_opt_   LPCWSTR lpDependencies,
    _In_opt_   LPCWSTR lpServiceStartName,
    _In_opt_   LPCWSTR lpPassword,
    _In_opt_   LPCWSTR lpDisplayName
    );


APDDLL_DECLSPEC BOOL WINAPI ChangeServiceConfig2WDynamiclly(
    _In_      SC_HANDLE hService,
    _In_      DWORD dwInfoLevel,
    _In_opt_  LPVOID lpInfo
    );


APDDLL_DECLSPEC SERVICE_STATUS_HANDLE WINAPI RegisterServiceCtrlHandlerExWDynamiclly(
    _In_      LPCWSTR lpServiceName,
    _In_      LPHANDLER_FUNCTION_EX lpHandlerProc,
    _In_opt_  LPVOID lpContext
    );

APDDLL_DECLSPEC SC_HANDLE WINAPI OpenServiceWDynamiclly(
    _In_  SC_HANDLE hSCManager,
    _In_  LPCWSTR lpServiceName,
    _In_  DWORD dwDesiredAccess
    );


APDDLL_DECLSPEC SC_LOCK WINAPI LockServiceDatabaseDynamiclly(
    _In_  SC_HANDLE hSCManager
    );



APDDLL_DECLSPEC BOOL WINAPI UnlockServiceDatabaseDynamiclly(
    _In_  SC_LOCK ScLock
    );


APDDLL_DECLSPEC BOOL WINAPI StartServiceCtrlDispatcherWDynamiclly(
    _In_  const SERVICE_TABLE_ENTRYW *lpServiceTable
    );


APDDLL_DECLSPEC LONG WINAPI RegOpenKeyExWDynamiclly(
    _In_        HKEY hKey,
    _In_opt_    LPCTSTR lpSubKey,
    _Reserved_  DWORD ulOptions,
    _In_        REGSAM samDesired,
    _Out_       PHKEY phkResult
    );


APDDLL_DECLSPEC LONG WINAPI RegCreateKeyExWDynamiclly(
    _In_        HKEY hKey,
    _In_        LPCWSTR lpSubKey,
    _Reserved_  DWORD Reserved,
    _In_opt_    LPWSTR lpClass,
    _In_        DWORD dwOptions,
    _In_        REGSAM samDesired,
    _In_opt_    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _Out_       PHKEY phkResult,
    _Out_opt_   LPDWORD lpdwDisposition
    );

APDDLL_DECLSPEC LONG WINAPI RegQueryValueExWDynamiclly(
    _In_         HKEY hKey,
    _In_opt_     LPCWSTR lpValueName,
    _Reserved_   LPDWORD lpReserved,
    _Out_opt_    LPDWORD lpType,
    _Out_opt_    LPBYTE lpData,
    _Inout_opt_  LPDWORD lpcbData
    );

APDDLL_DECLSPEC LONG WINAPI RegSetValueExWDynamiclly(
    _In_        HKEY hKey,
    _In_opt_    LPCWSTR lpValueName,
    _Reserved_  DWORD Reserved,
    _In_        DWORD dwType,
    _In_        const BYTE *lpData,
    _In_        DWORD cbData
    );


APDDLL_DECLSPEC LONG WINAPI RegCloseKeyDynamiclly(
    _In_  HKEY hKey
    );


APDDLL_DECLSPEC LONG WINAPI RegDeleteValueWDynamiclly(
    _In_      HKEY hKey,
    _In_opt_  LPCWSTR lpValueName
    );


APDDLL_DECLSPEC SC_HANDLE WINAPI OpenSCManagerWDynamiclly(
    _In_opt_  LPCWSTR lpMachineName,
    _In_opt_  LPCWSTR lpDatabaseName,
    _In_      DWORD dwDesiredAccess
    );

APDDLL_DECLSPEC BOOL WINAPI DeleteServiceDynamiclly(
    _In_  SC_HANDLE hService
    );

APDDLL_DECLSPEC BOOL WINAPI CloseServiceHandleDynamiclly(
    _In_  SC_HANDLE hSCObject
    );

APDDLL_DECLSPEC SC_HANDLE WINAPI CreateServiceWDynamiclly(
    _In_       SC_HANDLE hSCManager,
    _In_       LPCWSTR lpServiceName,
    _In_opt_   LPCWSTR lpDisplayName,
    _In_       DWORD dwDesiredAccess,
    _In_       DWORD dwServiceType,
    _In_       DWORD dwStartType,
    _In_       DWORD dwErrorControl,
    _In_opt_   LPCWSTR lpBinaryPathName,
    _In_opt_   LPCWSTR lpLoadOrderGroup,
    _Out_opt_  LPDWORD lpdwTagId,
    _In_opt_   LPCWSTR lpDependencies,
    _In_opt_   LPCWSTR lpServiceStartName,
    _In_opt_   LPCWSTR lpPassword
    );

APDDLL_DECLSPEC BOOL WINAPI InitializeSecurityDescriptorDynamiclly(
    _Out_  PSECURITY_DESCRIPTOR pSecurityDescriptor,
    _In_   DWORD dwRevision
    );




//[SHELL32.dll] 
APDDLL_DECLSPEC BOOL SHGetSpecialFolderPathDynamiclly(
    HWND hwndOwner,LPWSTR lpszPath,int csidl,BOOL fCreate);


APDDLL_DECLSPEC LPWSTR* CommandLineToArgvDynamiclly(
    LPCWSTR lpCmdLine,
    int *pNumArgs
    );


//[SHLWAPI.dll]
APDDLL_DECLSPEC BOOL PathRemoveFileSpecDynamiclly(
     LPWSTR pszPath
    );


APDDLL_DECLSPEC PWSTR PathSkipRootDynamiclly(
    PWSTR pszPath
    );


APDDLL_DECLSPEC BOOL PathFileExistsDynamiclly(LPCWSTR pszPath);

APDDLL_DECLSPEC LSTATUS SHDeleteKeyDynamiclly(HKEY hkey, LPCWSTR pszSubKey);


//[WININET.dll]
typedef LPVOID HINTERNET;
BOOL HttpQueryInfoWDynamiclly(
    HINTERNET hRequest,
    DWORD dwInfoLevel,
    LPVOID lpvBuffer,
    LPDWORD lpdwBufferLength,
    LPDWORD lpdwIndex
    );


HINTERNET InternetOpenWDynamiclly(
      LPCWSTR lpszAgent,
      DWORD dwAccessType,
      LPCWSTR lpszProxyName,
      LPCWSTR lpszProxyBypass,
      DWORD dwFlags
    );


typedef WORD INTERNET_PORT;
HINTERNET InternetConnectWDynamiclly(
    _In_  HINTERNET hInternet,
    _In_  LPCWSTR lpszServerName,
    _In_  INTERNET_PORT nServerPort,
    _In_  LPCWSTR lpszUsername,
    _In_  LPCWSTR lpszPassword,
    _In_  DWORD dwService,
    _In_  DWORD dwFlags,
    _In_  DWORD_PTR dwContext
    );


DWORD InternetAttemptConnectDynamiclly(
    _In_  DWORD dwReserved
    );


BOOL InternetReadFileDynamiclly(
    _In_   HINTERNET hFile,
    _Out_  LPVOID lpBuffer,
    _In_   DWORD dwNumberOfBytesToRead,
    _Out_  LPDWORD lpdwNumberOfBytesRead
    );


BOOL HttpSendRequestWDynamiclly(
    _In_  HINTERNET hRequest,
    _In_  LPCWSTR lpszHeaders,
    _In_  DWORD dwHeadersLength,
    _In_  LPVOID lpOptional,
    _In_  DWORD dwOptionalLength
    );


BOOL InternetCloseHandleDynamiclly(
    _In_  HINTERNET hInternet
    );


HINTERNET HttpOpenRequestWDynamiclly(
    _In_  HINTERNET hConnect,
    _In_  LPCWSTR lpszVerb,
    _In_  LPCWSTR lpszObjectName,
    _In_  LPCWSTR lpszVersion,
    _In_  LPCWSTR lpszReferer,
    _In_  LPCWSTR *lplpszAcceptTypes,
    _In_  DWORD dwFlags,
    _In_  DWORD_PTR dwContext
    );


BOOL InternetSetOptionDynamiclly(
    _In_  HINTERNET hInternet,
    _In_  DWORD dwOption,
    _In_  LPVOID lpBuffer,
    _In_  DWORD dwBufferLength
    );


typedef enum  { 
    MYINTERNET_SCHEME_PARTIAL     = -2,
    MYINTERNET_SCHEME_UNKNOWN     = -1,
    MYINTERNET_SCHEME_DEFAULT     = 0,
    MYINTERNET_SCHEME_FTP,
    MYINTERNET_SCHEME_GOPHER,
    MYINTERNET_SCHEME_HTTP,
    MYINTERNET_SCHEME_HTTPS,
    MYINTERNET_SCHEME_FILE,
    MYINTERNET_SCHEME_NEWS,
    MYINTERNET_SCHEME_MAILTO,
    MYINTERNET_SCHEME_SOCKS,
    MYINTERNET_SCHEME_JAVASCRIPT,
    MYINTERNET_SCHEME_VBSCRIPT,
    MYINTERNET_SCHEME_RES,
    MYINTERNET_SCHEME_FIRST       = MYINTERNET_SCHEME_FTP,
    MYINTERNET_SCHEME_LAST        = MYINTERNET_SCHEME_VBSCRIPT
} MYINTERNET_SCHEME, * LPMYINTERNET_SCHEME;



typedef struct {
    DWORD           dwStructSize;
    LPWSTR          lpszScheme;
    DWORD           dwSchemeLength;
    MYINTERNET_SCHEME nScheme;
    LPWSTR          lpszHostName;
    DWORD           dwHostNameLength;
    INTERNET_PORT   nPort;
    LPWSTR          lpszUserName;
    DWORD           dwUserNameLength;
    LPWSTR          lpszPassword;
    DWORD           dwPasswordLength;
    LPWSTR          lpszUrlPath;
    DWORD           dwUrlPathLength;
    LPWSTR          lpszExtraInfo;
    DWORD           dwExtraInfoLength;
} URL_MYCOMPONENTSW, *LPURL_MYCOMPONENTSW;

//#include <WinInet.h>

BOOL InternetCrackUrlWDynamiclly(
    _In_     LPCWSTR lpszUrl,
    _In_     DWORD dwUrlLength,
    _In_     DWORD dwFlags,
    _Inout_  LPURL_MYCOMPONENTSW lpUrlComponents
    );