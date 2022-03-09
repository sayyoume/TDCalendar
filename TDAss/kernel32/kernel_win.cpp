
#include "kernel_win.h"

BOOL WINAPI WriteFileDynamically(HANDLE hFile,LPCVOID lpBuffer,
                          DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped) 
{
    typedef BOOL (WINAPI* WriteFileFunction)(HANDLE hFile,LPCVOID lpBuffer,
        DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped);

    WriteFileFunction write_file;
    write_file = reinterpret_cast<WriteFileFunction>(
        GetProcAddress(GetModuleHandle(L"kernel32.dll"), "WriteFile"));

    return write_file(hFile,lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,lpOverlapped);
}



HANDLE WINAPI CreateFileDynamically(LPCWSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,
                         LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile)
{
    typedef HANDLE (WINAPI* CreateFileFunction)(LPCWSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);

    CreateFileFunction create_file;
    create_file = reinterpret_cast<CreateFileFunction>(
        GetProcAddress(GetModuleHandle(L"kernel32.dll"), "CreateFileW"));

    return create_file(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
}


BOOL WINAPI CloseHandleDynamiclly(HANDLE hObject)
{
    typedef BOOL (WINAPI* CloseHandleFunction)(HANDLE hObject);

    CloseHandleFunction close_handle;
    close_handle = reinterpret_cast<CloseHandleFunction>(
        GetProcAddress(GetModuleHandle(L"kernel32.dll"), "CloseHandle"));

    return close_handle(hObject);
}


BOOL WINAPI DeleteFileDynamiclly(LPCWSTR lpFileName)
{
    typedef BOOL (WINAPI* DeleteFileFunction)(LPCWSTR lpFileName);

    DeleteFileFunction delete_file;
    delete_file = reinterpret_cast<DeleteFileFunction>(
        GetProcAddress(GetModuleHandle(L"kernel32.dll"), "DeleteFileW"));

    return delete_file(lpFileName);
}


HMODULE WINAPI GetModuleHandleADynamiclly(LPCSTR lpModuleName)
{
    typedef HMODULE (WINAPI* GetModuleHandleFunction)(LPCSTR lpModuleName);

    GetModuleHandleFunction get_ModuleHandle;
    get_ModuleHandle = reinterpret_cast<GetModuleHandleFunction>(
        GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetModuleHandleA"));

    return get_ModuleHandle(lpModuleName);
}



HINSTANCE ShellExecuteDynamiclly(
      HWND hwnd,LPCWSTR lpOperation,LPCWSTR lpFile,
      LPCWSTR lpParameters,LPCWSTR lpDirectory,INT nShowCmd
    )
{
    typedef HINSTANCE (WINAPI* ShellExecuteFunction)(HWND hwnd,LPCWSTR lpOperation,LPCWSTR lpFile,
        LPCWSTR lpParameters,LPCWSTR lpDirectory,INT nShowCmd);

    ShellExecuteFunction shell_exe;
    shell_exe = reinterpret_cast<ShellExecuteFunction>(
        GetProcAddress(GetModuleHandle(L"shell32.dll"), "ShellExecuteW"));

    return shell_exe(hwnd,lpOperation,lpFile,lpParameters,lpDirectory,nShowCmd);
}



//USER32.dll
int WINAPI LoadStringDynamiclly(
    _In_opt_  HINSTANCE hInstance,
    _In_      UINT uID,
    _Out_     LPWSTR lpBuffer,
    _In_      int nBufferMax
    )
{
    typedef int (WINAPI* LoadStringFunction)(HINSTANCE hInstance,
         UINT uID,
        LPWSTR lpBuffer,
        int nBufferMax);

    LoadStringFunction load_string;
    load_string = reinterpret_cast<LoadStringFunction>(
        GetProcAddress(GetModuleHandle(L"user32.dll"), "LoadStringW"));

    return load_string(hInstance,uID,lpBuffer,nBufferMax);
}



HICON WINAPI LoadIconDynamiclly(
    _In_opt_  HINSTANCE hInstance,
    _In_      LPCWSTR lpIconName
    )
{
    typedef HICON (WINAPI* LoadIconFunction)(HINSTANCE hInstance,
        LPCWSTR lpIconName);

    LoadIconFunction load_icon;
    load_icon = reinterpret_cast<LoadIconFunction>(
        GetProcAddress(GetModuleHandle(L"user32.dll"), "LoadIconW"));

    return load_icon(hInstance,lpIconName);
}


int __cdecl wsprintfDynamiclly(
    _Out_  LPWSTR lpOut,
    _In_   LPCWSTR lpFmt,
    _In_    ...
    )
{
    typedef int (__cdecl* wsprintfFunction)(LPWSTR lpOut,
        LPCWSTR lpFmt, ...);

    wsprintfFunction _wsprintf;
    _wsprintf = reinterpret_cast<wsprintfFunction>(
        GetProcAddress(GetModuleHandle(L"user32.dll"), "wsprintfW"));

    //return _wsprintf(lpOut,lpFmt,...);

    wchar_t tcBuf[1024*10] = {0};

    va_list arglist;
    va_start(arglist,lpFmt);

    vswprintf_s(tcBuf,lpFmt,arglist);

    return _wsprintf(lpOut,tcBuf);
}



RPC_STATUS
    RPC_ENTRY
    RpcStringFreeDynamiclly (
    RPC_WSTR __RPC_FAR * String
    )
{
    typedef RPC_STATUS (RPC_ENTRY* RpcStringFreeFunction)(RPC_WSTR __RPC_FAR * String);

    RpcStringFreeFunction rpc_StringFree;
    rpc_StringFree = reinterpret_cast<RpcStringFreeFunction>(
        GetProcAddress(LoadLibraryW(L"rpcrt4.dll"), "RpcStringFreeW"));

    return rpc_StringFree(String);
}


RPC_STATUS
    RPC_ENTRY
    UuidToStringDynamiclly (
    const UUID __RPC_FAR * Uuid,
    RPC_WSTR __RPC_FAR * StringUuid
    )
{
    typedef RPC_STATUS (RPC_ENTRY* UuidToStringFunction)(const UUID __RPC_FAR * Uuid,
        RPC_WSTR __RPC_FAR * StringUuid);

    UuidToStringFunction Uuid_ToString;
    Uuid_ToString = reinterpret_cast<UuidToStringFunction>(
        GetProcAddress(LoadLibraryW(L"rpcrt4.dll"), "UuidToStringW"));

    return Uuid_ToString(Uuid,StringUuid);
}


RPC_STATUS RPC_ENTRY UuidCreateDynamiclly(
    UUID __RPC_FAR *Uuid
    )
{
    typedef RPC_STATUS (RPC_ENTRY* UuidCreateFunction)(UUID __RPC_FAR *Uuid);

    UuidCreateFunction Uuid_Create;
    Uuid_Create = reinterpret_cast<UuidCreateFunction>(
        GetProcAddress(LoadLibraryW(L"rpcrt4.dll"), "UuidCreate"));

    return Uuid_Create(Uuid);
}

struct hostent* FAR gethostbynameDynamiclly(
    const char *name
    )
{
    typedef struct hostent* (__stdcall* gethostbynameFunction)(const char *name);

    gethostbynameFunction get_hostbyname;
    get_hostbyname = reinterpret_cast<gethostbynameFunction>(
        GetProcAddress(LoadLibraryW(L"ws2_32.dll"), "gethostbyname"));

    return get_hostbyname(name);
}


char FAR * __stdcall FAR inet_ntoaDynamiclly (struct in_addr in)
{
    typedef char FAR * (__stdcall* inet_ntoaFunction)(struct in_addr in);

    inet_ntoaFunction _inet_ntoa;
    _inet_ntoa = reinterpret_cast<inet_ntoaFunction>(
        GetProcAddress(LoadLibraryW(L"ws2_32.dll"), "inet_ntoa"));

    return _inet_ntoa(in);
}


int __stdcall WSACleanupDynamiclly(void)
{
    typedef int(__stdcall* WSACleanupFunction)(void);

    WSACleanupFunction _WSACleanup;
    _WSACleanup = reinterpret_cast<WSACleanupFunction>(
        GetProcAddress(LoadLibraryW(L"ws2_32.dll"), "WSACleanup"));

    return _WSACleanup();
}


int __stdcall WSAStartupDynamiclly(WORD wVersionRequested,LPWSADATA lpWSAData)
{
    typedef int(__stdcall* WSAStartupFunction)(WORD wVersionRequested,LPWSADATA lpWSAData);

    WSAStartupFunction _WSAStartup;
    _WSAStartup = reinterpret_cast<WSAStartupFunction>(
        GetProcAddress(LoadLibraryW(L"ws2_32.dll"), "WSAStartup"));

    return _WSAStartup(wVersionRequested,lpWSAData);
}


//ADVAPI32.dll
BOOL WINAPI OpenProcessTokenDynamiclly(
    HANDLE ProcessHandle,
    DWORD DesiredAccess,
    PHANDLE TokenHandle)
{
    typedef BOOL (WINAPI* OpenProcessTokenFunction)(
        _In_   HANDLE ProcessHandle,
        _In_   DWORD DesiredAccess,
        _Out_  PHANDLE TokenHandle
        );

    OpenProcessTokenFunction openprocess_token;
    openprocess_token = reinterpret_cast<OpenProcessTokenFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "OpenProcessToken"));

    return openprocess_token(ProcessHandle,DesiredAccess,TokenHandle);
}


BOOL WINAPI LookupPrivilegeValueWDynamiclly(
     LPCWSTR lpSystemName,
     LPCWSTR lpName,
     PLUID lpLuid
    )
{
    typedef BOOL (WINAPI* LookupPrivilegeValueFunction)(
        LPCWSTR lpSystemName,
        LPCWSTR lpName,
        PLUID lpLuid);

    LookupPrivilegeValueFunction lookupPrivilege_Value;
    lookupPrivilege_Value = reinterpret_cast<LookupPrivilegeValueFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "LookupPrivilegeValueW"));

    return lookupPrivilege_Value(lpSystemName,lpName,lpLuid);
}


BOOL WINAPI AdjustTokenPrivilegesDynamiclly(
    HANDLE TokenHandle,
    BOOL DisableAllPrivileges,
    PTOKEN_PRIVILEGES NewState,
    DWORD BufferLength,
    PTOKEN_PRIVILEGES PreviousState,
    PDWORD ReturnLength)
{
    typedef BOOL (WINAPI* AdjustTokenPrivilegesFunction)(
        HANDLE TokenHandle,
        BOOL DisableAllPrivileges,
        PTOKEN_PRIVILEGES NewState,
        DWORD BufferLength,
        PTOKEN_PRIVILEGES PreviousState,
        PDWORD ReturnLength);

    AdjustTokenPrivilegesFunction adjustToken_Privileges;
    adjustToken_Privileges = reinterpret_cast<AdjustTokenPrivilegesFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "AdjustTokenPrivileges"));

    return adjustToken_Privileges(TokenHandle,DisableAllPrivileges,NewState,
        BufferLength,PreviousState,ReturnLength);

}

BOOL WINAPI ConvertStringSidToSidWDynamiclly(
    LPCWSTR StringSid,
    PSID *Sid)
{
    typedef BOOL (WINAPI* ConvertStringSidToSidFunction)(
        LPCWSTR StringSid,
        PSID *Sid);
    
    ConvertStringSidToSidFunction ConvertStringSidToSid_;
    ConvertStringSidToSid_ = reinterpret_cast<ConvertStringSidToSidFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "ConvertStringSidToSidW"));

    return ConvertStringSidToSid_(StringSid,Sid);

}


BOOL WINAPI CreateProcessAsUserWDynamiclly(
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
    LPPROCESS_INFORMATION lpProcessInformation)
{
    typedef BOOL (WINAPI* CreateProcessAsUserWFunction)(
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

    CreateProcessAsUserWFunction CreateProcessAsUserWFunction_;
    CreateProcessAsUserWFunction_ = reinterpret_cast<CreateProcessAsUserWFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "CreateProcessAsUserW"));

    return CreateProcessAsUserWFunction_(hToken,lpApplicationName,lpCommandLine,
        lpProcessAttributes,lpThreadAttributes,bInheritHandles,dwCreationFlags,
        lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation);
}



BOOL WINAPI GetTokenInformationDynamiclly(
    HANDLE TokenHandle,
    TOKEN_INFORMATION_CLASS TokenInformationClass,
    LPVOID TokenInformation,
    DWORD TokenInformationLength,
    PDWORD ReturnLength
    )
{
    typedef BOOL (WINAPI* GetTokenInformationFunction)(
        HANDLE TokenHandle,
        TOKEN_INFORMATION_CLASS TokenInformationClass,
        LPVOID TokenInformation,
        DWORD TokenInformationLength,
        PDWORD ReturnLength
        );

    GetTokenInformationFunction GetTokenInformationFunction_;
    GetTokenInformationFunction_ = reinterpret_cast<GetTokenInformationFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "GetTokenInformation"));



    return GetTokenInformationFunction_(TokenHandle,TokenInformationClass,TokenInformation,
        TokenInformationLength,ReturnLength);
}


BOOL WINAPI DuplicateTokenExDynamiclly(
    _In_      HANDLE hExistingToken,
    _In_      DWORD dwDesiredAccess,
    _In_opt_  LPSECURITY_ATTRIBUTES lpTokenAttributes,
    _In_      SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
    _In_      TOKEN_TYPE TokenType,
    _Out_     PHANDLE phNewToken
    )
{
    typedef BOOL (WINAPI* DuplicateTokenExFunction)(
        HANDLE hExistingToken,
        DWORD dwDesiredAccess,
        LPSECURITY_ATTRIBUTES lpTokenAttributes,
        SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
        TOKEN_TYPE TokenType,
        PHANDLE phNewToken
        );

    DuplicateTokenExFunction DuplicateTokenExFunction_;
    DuplicateTokenExFunction_ = reinterpret_cast<DuplicateTokenExFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "DuplicateTokenEx"));

    return DuplicateTokenExFunction_(hExistingToken,dwDesiredAccess,lpTokenAttributes,
        ImpersonationLevel,TokenType,phNewToken);


}

BOOL WINAPI SetTokenInformationDynamiclly(
    _In_  HANDLE TokenHandle,
    _In_  TOKEN_INFORMATION_CLASS TokenInformationClass,
    _In_  LPVOID TokenInformation,
    _In_  DWORD TokenInformationLength
    )
{
    typedef BOOL (WINAPI* SetTokenInformationFunction)(
          HANDLE TokenHandle,
          TOKEN_INFORMATION_CLASS TokenInformationClass,
          LPVOID TokenInformation,
          DWORD TokenInformationLength);

    SetTokenInformationFunction SetTokenInformationFunction_;
    SetTokenInformationFunction_ = reinterpret_cast<SetTokenInformationFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "SetTokenInformation"));

    return SetTokenInformationFunction_(TokenHandle,TokenInformationClass,TokenInformation,
        TokenInformationLength);
}


BOOL WINAPI SetSecurityDescriptorDaclDynamiclly(
    _Inout_   PSECURITY_DESCRIPTOR pSecurityDescriptor,
    _In_      BOOL bDaclPresent,
    _In_opt_  PACL pDacl,
    _In_      BOOL bDaclDefaulted
    )
{
    typedef BOOL (WINAPI* SetSecurityDescriptorDaclFunction)(
        PSECURITY_DESCRIPTOR pSecurityDescriptor,
        BOOL bDaclPresent,
        PACL pDacl,
        BOOL bDaclDefaulted
        );

    SetSecurityDescriptorDaclFunction SetSecurityDescriptorDaclFunction_;
    SetSecurityDescriptorDaclFunction_ = reinterpret_cast<SetSecurityDescriptorDaclFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "SetSecurityDescriptorDacl"));

    return SetSecurityDescriptorDaclFunction_(pSecurityDescriptor,bDaclPresent,
        pDacl,bDaclDefaulted);
}


BOOL WINAPI QueryServiceStatusExDynamiclly(
    _In_       SC_HANDLE hService,
    _In_       SC_STATUS_TYPE InfoLevel,
    _Out_opt_  LPBYTE lpBuffer,
    _In_       DWORD cbBufSize,
    _Out_      LPDWORD pcbBytesNeeded
    )
{
    typedef BOOL (WINAPI* QueryServiceStatusExFunction)(
               SC_HANDLE hService,
               SC_STATUS_TYPE InfoLevel,
          LPBYTE lpBuffer,
               DWORD cbBufSize,
              LPDWORD pcbBytesNeeded
        );

    QueryServiceStatusExFunction QueryServiceStatusExFunction_;
    QueryServiceStatusExFunction_ = reinterpret_cast<QueryServiceStatusExFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "QueryServiceStatusEx"));

    return QueryServiceStatusExFunction_(hService,InfoLevel,lpBuffer,
        cbBufSize,pcbBytesNeeded);
}


BOOL WINAPI QueryServiceStatusDynamiclly(
    _In_   SC_HANDLE hService,
    _Out_  LPSERVICE_STATUS lpServiceStatus
    )
{
    typedef BOOL (WINAPI* QueryServiceStatusFunction)(
          SC_HANDLE hService,
         LPSERVICE_STATUS lpServiceStatus
        );

    QueryServiceStatusFunction QueryServiceStatusFunction_;
    QueryServiceStatusFunction_ = reinterpret_cast<QueryServiceStatusFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "QueryServiceStatus"));

    return QueryServiceStatusFunction_(hService,lpServiceStatus);
}


BOOL WINAPI ControlServiceDynamiclly(
    _In_   SC_HANDLE hService,
    _In_   DWORD dwControl,
    _Out_  LPSERVICE_STATUS lpServiceStatus
    )
{
    typedef BOOL (WINAPI* ControlServiceFunction)(
           SC_HANDLE hService,
           DWORD dwControl,
          LPSERVICE_STATUS lpServiceStatus
        );

    ControlServiceFunction ControlServiceFunction_;
    ControlServiceFunction_ = reinterpret_cast<ControlServiceFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "ControlService"));

    return ControlServiceFunction_(hService,dwControl,lpServiceStatus);
}


BOOL WINAPI SetServiceStatusDynamiclly(
    _In_  SERVICE_STATUS_HANDLE hServiceStatus,
    _In_  LPSERVICE_STATUS lpServiceStatus
    )
{
    typedef BOOL (WINAPI* SetServiceStatusFunction)(
        SERVICE_STATUS_HANDLE hServiceStatus,
        LPSERVICE_STATUS lpServiceStatus
        );

    SetServiceStatusFunction SetServiceStatusFunction_;
    SetServiceStatusFunction_ = reinterpret_cast<SetServiceStatusFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "SetServiceStatus"));

    return SetServiceStatusFunction_(hServiceStatus,lpServiceStatus);
}


BOOL WINAPI StartServiceWDynamiclly(
    _In_      SC_HANDLE hService,
    _In_      DWORD dwNumServiceArgs,
    _In_opt_  LPCWSTR *lpServiceArgVectors
    )
{
    typedef BOOL (WINAPI* StartServiceWFunction)(
             SC_HANDLE hService,
             DWORD dwNumServiceArgs,
           LPCWSTR *lpServiceArgVectors
        );

    StartServiceWFunction StartServiceWFunction_;
    StartServiceWFunction_ = reinterpret_cast<StartServiceWFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "StartServiceW"));

    return StartServiceWFunction_(hService,dwNumServiceArgs,lpServiceArgVectors);
}


BOOL WINAPI ChangeServiceConfigWDynamiclly(
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
    )
{
    typedef BOOL (WINAPI* ChangeServiceConfigWFunction)(
              SC_HANDLE hService,
              DWORD dwServiceType,
              DWORD dwStartType,
              DWORD dwErrorControl,
          LPCWSTR lpBinaryPathName,
          LPCWSTR lpLoadOrderGroup,
          LPDWORD lpdwTagId,
           LPCWSTR lpDependencies,
           LPCWSTR lpServiceStartName,
           LPCWSTR lpPassword,
           LPCWSTR lpDisplayName
        );

    ChangeServiceConfigWFunction ChangeServiceConfigWFunction_;
    ChangeServiceConfigWFunction_ = reinterpret_cast<ChangeServiceConfigWFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "ChangeServiceConfigW"));

    return ChangeServiceConfigWFunction_( hService,
         dwServiceType,dwStartType,dwErrorControl,
         lpBinaryPathName,lpLoadOrderGroup,
          lpdwTagId,lpDependencies,lpServiceStartName,
         lpPassword,lpDisplayName);
}


BOOL WINAPI ChangeServiceConfig2WDynamiclly(
    _In_      SC_HANDLE hService,
    _In_      DWORD dwInfoLevel,
    _In_opt_  LPVOID lpInfo
    )
{
    typedef BOOL (WINAPI* ChangeServiceConfig2WFunction)(
              SC_HANDLE hService,
              DWORD dwInfoLevel,
          LPVOID lpInfo);

    ChangeServiceConfig2WFunction ChangeServiceConfig2WFunction_;
    ChangeServiceConfig2WFunction_ = reinterpret_cast<ChangeServiceConfig2WFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "ChangeServiceConfig2W"));

    return ChangeServiceConfig2WFunction_(hService,dwInfoLevel,lpInfo);
}


SERVICE_STATUS_HANDLE WINAPI RegisterServiceCtrlHandlerExWDynamiclly(
    _In_      LPCWSTR lpServiceName,
    _In_      LPHANDLER_FUNCTION_EX lpHandlerProc,
    _In_opt_  LPVOID lpContext
    )
{
    typedef SERVICE_STATUS_HANDLE (WINAPI* RegisterServiceCtrlHandlerExFunction)(
            LPCWSTR lpServiceName,
            LPHANDLER_FUNCTION_EX lpHandlerProc,
            LPVOID lpContext);
    
    RegisterServiceCtrlHandlerExFunction RegisterServiceCtrlHandlerExFunction_;
    RegisterServiceCtrlHandlerExFunction_ =  reinterpret_cast<RegisterServiceCtrlHandlerExFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegisterServiceCtrlHandlerExW"));

    return RegisterServiceCtrlHandlerExFunction_(lpServiceName,lpHandlerProc,lpContext);
}


SC_HANDLE WINAPI OpenServiceWDynamiclly(
    _In_  SC_HANDLE hSCManager,
    _In_  LPCWSTR lpServiceName,
    _In_  DWORD dwDesiredAccess
    )
{
    typedef SC_HANDLE (WINAPI* OpenServiceWFunction)(
          SC_HANDLE hSCManager,
          LPCWSTR lpServiceName,
          DWORD dwDesiredAccess
        );

    OpenServiceWFunction OpenServiceWFunction_;
    OpenServiceWFunction_ = reinterpret_cast<OpenServiceWFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "OpenServiceW"));

    return OpenServiceWFunction_(hSCManager,lpServiceName,dwDesiredAccess);
}


SC_LOCK WINAPI LockServiceDatabaseDynamiclly(
    _In_  SC_HANDLE hSCManager
    )
{
    typedef SC_LOCK (WINAPI* LockServiceDatabaseFunction)(
        SC_HANDLE hSCManager
        );

    LockServiceDatabaseFunction LockServiceDatabaseFunction_;
    LockServiceDatabaseFunction_ = reinterpret_cast<LockServiceDatabaseFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "LockServiceDatabase"));

    return LockServiceDatabaseFunction_(hSCManager);
}



BOOL WINAPI UnlockServiceDatabaseDynamiclly(
    _In_  SC_LOCK ScLock
    )
{
    typedef BOOL (WINAPI* UnlockServiceDatabaseFunction)(
        SC_LOCK ScLock
        );

    UnlockServiceDatabaseFunction UnlockServiceDatabaseFunction_;
    UnlockServiceDatabaseFunction_ = reinterpret_cast<UnlockServiceDatabaseFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "UnlockServiceDatabase"));

    return UnlockServiceDatabaseFunction_(ScLock);
}


BOOL WINAPI StartServiceCtrlDispatcherWDynamiclly(
    _In_  const SERVICE_TABLE_ENTRYW *lpServiceTable
    )
{
    typedef BOOL (WINAPI* StartServiceCtrlDispatcherWFunction)(
         const SERVICE_TABLE_ENTRYW *lpServiceTable
        );

    StartServiceCtrlDispatcherWFunction StartServiceCtrlDispatcherWFunction_;
    StartServiceCtrlDispatcherWFunction_ = reinterpret_cast<StartServiceCtrlDispatcherWFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "StartServiceCtrlDispatcherW"));

    return StartServiceCtrlDispatcherWFunction_(lpServiceTable);
}


LONG WINAPI RegOpenKeyExWDynamiclly(
    _In_        HKEY hKey,
    _In_opt_    LPCTSTR lpSubKey,
    _Reserved_  DWORD ulOptions,
    _In_        REGSAM samDesired,
    _Out_       PHKEY phkResult
    )
{
    typedef LONG (WINAPI* RegOpenKeyExWFunction)(
              HKEY hKey,
           LPCTSTR lpSubKey,
          DWORD ulOptions,
          REGSAM samDesired,
          PHKEY phkResult);

    RegOpenKeyExWFunction RegOpenKeyExWFunction_;
    RegOpenKeyExWFunction_ = reinterpret_cast<RegOpenKeyExWFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegOpenKeyExW"));

    return RegOpenKeyExWFunction_(hKey,lpSubKey,ulOptions,samDesired,phkResult);
}


LONG WINAPI RegCreateKeyExWDynamiclly(
    _In_        HKEY hKey,
    _In_        LPCWSTR lpSubKey,
    _Reserved_  DWORD Reserved,
    _In_opt_    LPWSTR lpClass,
    _In_        DWORD dwOptions,
    _In_        REGSAM samDesired,
    _In_opt_    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _Out_       PHKEY phkResult,
    _Out_opt_   LPDWORD lpdwDisposition
    )
{
    typedef LONG (WINAPI* RegCreateKeyExFunction)(
                HKEY hKey,
                LPCWSTR lpSubKey,
          DWORD Reserved,
            LPWSTR lpClass,
                DWORD dwOptions,
                REGSAM samDesired,
            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
               PHKEY phkResult,
           LPDWORD lpdwDisposition
        );

    RegCreateKeyExFunction RegCreateKeyExFunction_;

    RegCreateKeyExFunction_ = reinterpret_cast<RegCreateKeyExFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegCreateKeyExW"));

    return RegCreateKeyExFunction_(hKey,lpSubKey,Reserved,lpClass,dwOptions,
        samDesired,lpSecurityAttributes,phkResult,lpdwDisposition);

}


LONG WINAPI RegQueryValueExWDynamiclly(
    _In_         HKEY hKey,
    _In_opt_     LPCWSTR lpValueName,
    _Reserved_   LPDWORD lpReserved,
    _Out_opt_    LPDWORD lpType,
    _Out_opt_    LPBYTE lpData,
    _Inout_opt_  LPDWORD lpcbData
    )
{
    typedef LONG (WINAPI* RegQueryValueExWFunction)(
                 HKEY hKey,
             LPCWSTR lpValueName,
           LPDWORD lpReserved,
            LPDWORD lpType,
            LPBYTE lpData,
          LPDWORD lpcbData
        );

    RegQueryValueExWFunction RegQueryValueExWFunction_;
    RegQueryValueExWFunction_ = reinterpret_cast<RegQueryValueExWFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegQueryValueExW"));

    return RegQueryValueExWFunction_(hKey,lpValueName,lpReserved,
         lpType,
         lpData,
         lpcbData);
}

LONG WINAPI RegSetValueExWDynamiclly(
    _In_        HKEY hKey,
    _In_opt_    LPCWSTR lpValueName,
    _Reserved_  DWORD Reserved,
    _In_        DWORD dwType,
    _In_        const BYTE *lpData,
    _In_        DWORD cbData
    )
{
    typedef LONG (WINAPI* RegSetValueExWFunction)(
                HKEY hKey,
            LPCWSTR lpValueName,
          DWORD Reserved,
                DWORD dwType,
                const BYTE *lpData,
                DWORD cbData
        );

    RegSetValueExWFunction RegSetValueExWFunction_;
    RegSetValueExWFunction_ = reinterpret_cast<RegSetValueExWFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegSetValueExW"));

    return RegSetValueExWFunction_(hKey,lpValueName,Reserved,
        dwType,lpData,cbData);
}



LONG WINAPI RegCloseKeyDynamiclly(
    _In_  HKEY hKey
    )
{
    typedef LONG (WINAPI* RegCloseKeyFunction)(
        HKEY hKey
        );

    RegCloseKeyFunction RegCloseKeyFunction_;
    RegCloseKeyFunction_ = reinterpret_cast<RegCloseKeyFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegCloseKey"));

    return RegCloseKeyFunction_(hKey);
}



LONG WINAPI RegDeleteValueWDynamiclly(
    _In_      HKEY hKey,
    _In_opt_  LPCWSTR lpValueName
    )
{
    typedef LONG (WINAPI* RegDeleteValueFunction)(
              HKEY hKey,
          LPCWSTR lpValueName
        );

    RegDeleteValueFunction RegDeleteValueFunction_;
    RegDeleteValueFunction_ = reinterpret_cast<RegDeleteValueFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegDeleteValueW"));

    return RegDeleteValueFunction_(hKey,lpValueName);
}


SC_HANDLE WINAPI OpenSCManagerWDynamiclly(
    _In_opt_  LPCWSTR lpMachineName,
    _In_opt_  LPCWSTR lpDatabaseName,
    _In_      DWORD dwDesiredAccess
    )
{
    typedef SC_HANDLE (WINAPI* OpenSCManagerWFunction)(
         LPCWSTR lpMachineName,
         LPCWSTR lpDatabaseName,
         DWORD dwDesiredAccess
        );

    OpenSCManagerWFunction OpenSCManagerWFunction_;

    OpenSCManagerWFunction_ = reinterpret_cast<OpenSCManagerWFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "OpenSCManagerW"));

    return OpenSCManagerWFunction_(lpMachineName,lpDatabaseName,dwDesiredAccess);
}


BOOL WINAPI DeleteServiceDynamiclly(
    _In_  SC_HANDLE hService
    )
{
    typedef BOOL (WINAPI* DeleteServiceFunction)(
        SC_HANDLE hService
        );

    DeleteServiceFunction DeleteServiceFunction_;

    DeleteServiceFunction_ = reinterpret_cast<DeleteServiceFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "DeleteService"));

    return DeleteServiceFunction_(hService);
}


BOOL WINAPI CloseServiceHandleDynamiclly(
    _In_  SC_HANDLE hSCObject
    )
{
    typedef BOOL (WINAPI* CloseServiceHandleFunction)(
        SC_HANDLE hSCObject
        );

    CloseServiceHandleFunction CloseServiceHandleFunction_;

    CloseServiceHandleFunction_ = reinterpret_cast<CloseServiceHandleFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "CloseServiceHandle"));

    return CloseServiceHandleFunction_(hSCObject);
}



SC_HANDLE WINAPI CreateServiceWDynamiclly(
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
    )
{
    typedef SC_HANDLE (WINAPI* CreateServiceFunction)(
        SC_HANDLE hSCManager,
        LPCWSTR lpServiceName,
         LPCWSTR lpDisplayName,
           DWORD dwDesiredAccess,
               DWORD dwServiceType,
               DWORD dwStartType,
               DWORD dwErrorControl,
           LPCWSTR lpBinaryPathName,
           LPCWSTR lpLoadOrderGroup,
          LPDWORD lpdwTagId,
           LPCWSTR lpDependencies,
           LPCWSTR lpServiceStartName,
          LPCWSTR lpPassword
        );

    CreateServiceFunction CreateServiceFunction_;

    CreateServiceFunction_ = reinterpret_cast<CreateServiceFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "CreateServiceW"));

    return CreateServiceFunction_(hSCManager,
         lpServiceName,
         lpDisplayName,
         dwDesiredAccess,
         dwServiceType,
         dwStartType,
         dwErrorControl,
         lpBinaryPathName,
         lpLoadOrderGroup,
         lpdwTagId,
         lpDependencies,
         lpServiceStartName,
         lpPassword);
}

BOOL WINAPI InitializeSecurityDescriptorDynamiclly(
    _Out_  PSECURITY_DESCRIPTOR pSecurityDescriptor,
    _In_   DWORD dwRevision
    )
{
    typedef BOOL (WINAPI* InitializeSecurityDescriptorFunction)(
        PSECURITY_DESCRIPTOR pSecurityDescriptor,
        DWORD dwRevision
        );

    InitializeSecurityDescriptorFunction InitializeSecurityDescriptorFunction_;

    InitializeSecurityDescriptorFunction_ = reinterpret_cast<InitializeSecurityDescriptorFunction>(
        GetProcAddress(LoadLibraryW(L"advapi32.dll"), "InitializeSecurityDescriptor"));

    return InitializeSecurityDescriptorFunction_(pSecurityDescriptor,dwRevision);
}

//[SHELL32.dll] 
BOOL SHGetSpecialFolderPathDynamiclly(
    HWND hwndOwner,LPWSTR lpszPath,int csidl,BOOL fCreate)
{
    typedef BOOL (WINAPI* SHGetSpecialFolderPathFunction)(
        HWND hwndOwner,
        _Out_  LPTSTR lpszPath,
        _In_   int csidl,
        _In_   BOOL fCreate
        );


    SHGetSpecialFolderPathFunction get_specialFolderpath;

    get_specialFolderpath = reinterpret_cast<SHGetSpecialFolderPathFunction>(
        GetProcAddress(LoadLibraryW(L"shell32.dll"), "SHGetSpecialFolderPathW"));

    return get_specialFolderpath(hwndOwner,lpszPath,csidl,fCreate);

}


LPWSTR* CommandLineToArgvDynamiclly(
    LPCWSTR lpCmdLine,
    int *pNumArgs
    )
{
    typedef LPWSTR* (WINAPI* CommandLineToArgvFunction)(
        _In_   LPCWSTR lpCmdLine,
        _Out_  int *pNumArgs

        );


    CommandLineToArgvFunction commandLine_ToArgv;

    commandLine_ToArgv = reinterpret_cast<CommandLineToArgvFunction>(
        GetProcAddress(LoadLibraryW(L"shell32.dll"), "CommandLineToArgvW"));

    return commandLine_ToArgv(lpCmdLine,pNumArgs);
}



BOOL PathRemoveFileSpecDynamiclly(LPWSTR pszPath)
{
    typedef BOOL (WINAPI* PathRemoveFileSpecFunction)(LPWSTR pszPath );

    PathRemoveFileSpecFunction pathRemove_FileSpec;

    pathRemove_FileSpec = reinterpret_cast<PathRemoveFileSpecFunction>(
        GetProcAddress(LoadLibraryW(L"shlwapi.dll"), "PathRemoveFileSpecW"));

    if (pathRemove_FileSpec == NULL)
    {
        return FALSE;
    }

    return pathRemove_FileSpec(pszPath);
}


PWSTR PathSkipRootDynamiclly(
    PWSTR pszPath
    )
{
    typedef PWSTR (WINAPI* PathSkipRootFunction)(PWSTR pszPath );

    PathSkipRootFunction pathSkip_Root;

    pathSkip_Root = reinterpret_cast<PathSkipRootFunction>(
        GetProcAddress(LoadLibraryW(L"shlwapi.dll"), "PathSkipRootW"));

    if (pathSkip_Root == NULL)
    {
        return NULL;
    }

    return pathSkip_Root(pszPath);
}

BOOL PathFileExistsDynamiclly(LPCWSTR pszPath)
{
    typedef BOOL (WINAPI* PathFileExistsFunction)(LPCWSTR pszPath );

    PathFileExistsFunction pathFile_Exists;

    pathFile_Exists = reinterpret_cast<PathFileExistsFunction>(
        GetProcAddress(LoadLibraryW(L"shlwapi.dll"), "PathFileExistsW"));

    if (pathFile_Exists == NULL)
    {
        return FALSE;
    }

    return pathFile_Exists(pszPath);
}



LSTATUS SHDeleteKeyDynamiclly(HKEY hkey,LPCWSTR pszSubKey)
{
    typedef LSTATUS (WINAPI* SHDeleteKeyFunction)(HKEY hkey,LPCWSTR pszSubKey);

    SHDeleteKeyFunction sh_DeleteKey;

    sh_DeleteKey = reinterpret_cast<SHDeleteKeyFunction>(
        GetProcAddress(LoadLibraryW(L"shlwapi.dll"), "SHDeleteKeyW"));

    if (sh_DeleteKey == NULL)
    {
        return -1;
    }

    return sh_DeleteKey(hkey,pszSubKey);
}



//[WININET.dll]
BOOL HttpQueryInfoWDynamiclly(
    HINTERNET hRequest,
    DWORD dwInfoLevel,
    LPVOID lpvBuffer,
    LPDWORD lpdwBufferLength,
    LPDWORD lpdwIndex
    )
{
    typedef BOOL (WINAPI* HttpQueryInfoFunction)(HINTERNET hRequest,
        DWORD dwInfoLevel,
        LPVOID lpvBuffer,
        LPDWORD lpdwBufferLength,
        LPDWORD lpdwIndex);

    HttpQueryInfoFunction http_queryInfo;

    http_queryInfo = reinterpret_cast<HttpQueryInfoFunction>(
        GetProcAddress(LoadLibraryW(L"wininet.dll"), "HttpQueryInfoW"));

    if (http_queryInfo == NULL)
    {
        return FALSE;
    }

    return http_queryInfo(hRequest,dwInfoLevel,lpvBuffer,lpdwBufferLength,lpdwIndex);
}



HINTERNET InternetOpenWDynamiclly(
    LPCWSTR lpszAgent,
    DWORD dwAccessType,
    LPCWSTR lpszProxyName,
    LPCWSTR lpszProxyBypass,
    DWORD dwFlags
    )
{
    typedef HINTERNET (WINAPI* InternetOpenFunction)(LPCWSTR lpszAgent,
        DWORD dwAccessType,
        LPCWSTR lpszProxyName,
        LPCWSTR lpszProxyBypass,
        DWORD dwFlags);

    InternetOpenFunction http_queryInfo;

    http_queryInfo = reinterpret_cast<InternetOpenFunction>(
        GetProcAddress(LoadLibraryW(L"wininet.dll"), "InternetOpenW"));

    if (http_queryInfo == NULL)
    {
        return NULL;
    }


    return http_queryInfo(lpszAgent,dwAccessType,lpszProxyName,lpszProxyBypass,dwFlags);
}



HINTERNET InternetConnectWDynamiclly(
      HINTERNET hInternet,
      LPCWSTR lpszServerName,
      INTERNET_PORT nServerPort,
      LPCWSTR lpszUsername,
      LPCWSTR lpszPassword,
      DWORD dwService,
      DWORD dwFlags,
      DWORD_PTR dwContext
    )
{
    typedef HINTERNET (WINAPI* InternetConnectFunction)(HINTERNET hInternet,
        LPCWSTR lpszServerName,
        INTERNET_PORT nServerPort,
        LPCWSTR lpszUsername,
        LPCWSTR lpszPassword,
        DWORD dwService,
        DWORD dwFlags,
        DWORD_PTR dwContext);

    InternetConnectFunction http_queryInfo;

    http_queryInfo = reinterpret_cast<InternetConnectFunction>(
        GetProcAddress(LoadLibraryW(L"wininet.dll"), "InternetConnectW"));

    if (http_queryInfo == NULL)
    {
        return NULL;
    }
    return http_queryInfo(hInternet,lpszServerName,nServerPort,lpszUsername,lpszPassword,
        dwService,dwFlags,dwContext);

}


DWORD InternetAttemptConnectDynamiclly(
    _In_  DWORD dwReserved
    )
{
    typedef DWORD(__stdcall* InternetAttemptConnectFunction)(DWORD dwReserved);

    InternetAttemptConnectFunction internet_AttemptConnect;
    internet_AttemptConnect = reinterpret_cast<InternetAttemptConnectFunction>(
        GetProcAddress(LoadLibraryW(L"wininet.dll"), "InternetAttemptConnect"));

    return internet_AttemptConnect(dwReserved);
}


BOOL InternetReadFileDynamiclly(
    _In_   HINTERNET hFile,
    _Out_  LPVOID lpBuffer,
    _In_   DWORD dwNumberOfBytesToRead,
    _Out_  LPDWORD lpdwNumberOfBytesRead
    )
{
   typedef BOOL (WINAPI* InternetReadFileFuncation)(
           HINTERNET hFile,
          LPVOID lpBuffer,
           DWORD dwNumberOfBytesToRead,
          LPDWORD lpdwNumberOfBytesRead);


   InternetReadFileFuncation InternetReadFileFuncation_;
   InternetReadFileFuncation_ = reinterpret_cast<InternetReadFileFuncation>(
       GetProcAddress(LoadLibraryW(L"wininet.dll"), "InternetReadFile"));

   return InternetReadFileFuncation_(hFile,lpBuffer,dwNumberOfBytesToRead,
       lpdwNumberOfBytesRead);

}


BOOL HttpSendRequestWDynamiclly(
    _In_  HINTERNET hRequest,
    _In_  LPCWSTR lpszHeaders,
    _In_  DWORD dwHeadersLength,
    _In_  LPVOID lpOptional,
    _In_  DWORD dwOptionalLength
    )
{
    typedef BOOL (WINAPI* HttpSendRequestWFuncation)(
        HINTERNET hRequest,
        LPCWSTR lpszHeaders,
         DWORD dwHeadersLength,
         LPVOID lpOptional,
         DWORD dwOptionalLength);

    HttpSendRequestWFuncation HttpSendRequestWFuncation_;
    HttpSendRequestWFuncation_ = reinterpret_cast<HttpSendRequestWFuncation>(
        GetProcAddress(LoadLibraryW(L"wininet.dll"), "HttpSendRequestW"));

    return HttpSendRequestWFuncation_(hRequest,lpszHeaders,dwHeadersLength,
        lpOptional,dwOptionalLength);


}



BOOL InternetCloseHandleDynamiclly(
    _In_  HINTERNET hInternet
    )
{
    typedef BOOL (WINAPI* InternetCloseHandleFuncation)(
        HINTERNET hInternet);

    InternetCloseHandleFuncation InternetCloseHandleFuncation_;
    InternetCloseHandleFuncation_ = reinterpret_cast<InternetCloseHandleFuncation>(
        GetProcAddress(LoadLibraryW(L"wininet.dll"), "InternetCloseHandle"));

    return InternetCloseHandleFuncation_(hInternet);
}


HINTERNET HttpOpenRequestWDynamiclly(
    _In_  HINTERNET hConnect,
    _In_  LPCWSTR lpszVerb,
    _In_  LPCWSTR lpszObjectName,
    _In_  LPCWSTR lpszVersion,
    _In_  LPCWSTR lpszReferer,
    _In_  LPCWSTR *lplpszAcceptTypes,
    _In_  DWORD dwFlags,
    _In_  DWORD_PTR dwContext
    )
{
    typedef HINTERNET (WINAPI* HttpOpenRequestFuncation)(
          HINTERNET hConnect,
          LPCWSTR lpszVerb,
          LPCWSTR lpszObjectName,
          LPCWSTR lpszVersion,
          LPCWSTR lpszReferer,
          LPCWSTR *lplpszAcceptTypes,
          DWORD dwFlags,
          DWORD_PTR dwContext
        );

    HttpOpenRequestFuncation HttpOpenRequestFuncation_;
    HttpOpenRequestFuncation_ = reinterpret_cast<HttpOpenRequestFuncation>(
        GetProcAddress(LoadLibraryW(L"wininet.dll"), "HttpOpenRequestW"));

    return HttpOpenRequestFuncation_(hConnect,lpszVerb,lpszObjectName,lpszVersion,
        lpszReferer,lplpszAcceptTypes,dwFlags,dwContext);
}


BOOL InternetSetOptionDynamiclly(
    _In_  HINTERNET hInternet,
    _In_  DWORD dwOption,
    _In_  LPVOID lpBuffer,
    _In_  DWORD dwBufferLength
    )
{
    typedef BOOL (WINAPI* InternetSetOptionFunction)(
         HINTERNET hInternet,
         DWORD dwOption,
         LPVOID lpBuffer,
         DWORD dwBufferLength);

    InternetSetOptionFunction InternetSetOptionFunction_;
    InternetSetOptionFunction_ = reinterpret_cast<InternetSetOptionFunction>(
        GetProcAddress(LoadLibraryW(L"wininet.dll"), "InternetSetOptionW"));

    return InternetSetOptionFunction_(hInternet,dwOption,lpBuffer,dwBufferLength);
}


BOOL InternetCrackUrlWDynamiclly(
    _In_     LPCWSTR lpszUrl,
    _In_     DWORD dwUrlLength,
    _In_     DWORD dwFlags,
    _Inout_  LPURL_MYCOMPONENTSW lpUrlComponents
    )
{
    typedef BOOL (WINAPI* InternetCrackUrlWFunction)(
        LPCWSTR lpszUrl,
        DWORD dwUrlLength,
        DWORD dwFlags,
        LPURL_MYCOMPONENTSW lpUrlComponents);

    InternetCrackUrlWFunction InternetCrackUrlWFunction_;
    InternetCrackUrlWFunction_ = reinterpret_cast<InternetCrackUrlWFunction>(
        GetProcAddress(LoadLibraryW(L"wininet.dll"), "InternetCrackUrlW"));


    return InternetCrackUrlWFunction_(lpszUrl,dwUrlLength,dwFlags,lpUrlComponents);
}