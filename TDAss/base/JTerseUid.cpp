
#include "../JLHelper.h"
#include "JTerseUid.h"
#include <tchar.h>
#include <winioctl.h>
#include <string>
#include "../safe/machine_id.h"

using namespace std;

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

namespace JTerse{ namespace core{ namespace ReportLog{

	BOOL IsWow64_ID()
	{
		LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"),"IsWow64Process");

		BOOL bIsWow64 = false;

		if (NULL != fnIsWow64Process)
		{
			if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
			{
				// Error handle
				return false;
			}
		}

		return  bIsWow64;
	}

	string ReplaceString(const string &srcStr, const string &oldStr, const string &newStr)
	{
		if (srcStr.size() <= 0 || oldStr.size() <= 0)
		{
			return srcStr;
		}
		string strReturn = srcStr;
		string::size_type offset = 0;
		string::size_type start = strReturn.find(oldStr);
		while (start != string::npos)
		{
			offset = start + newStr.size();
			strReturn.replace(start, oldStr.size(), newStr);
			start = strReturn.find(oldStr, offset);
		}

		return strReturn;
	}

	BOOL GetCPUID(LPTSTR lpstrCpuID,size_t leng)   
	{ 
		/*unsigned   long   s1 = 0,s2 = 0; 

		__asm{   
			mov eax,01h   
				xor edx,edx   
				cpuid   
				mov s1,edx   
				mov s2,eax   
		}     

		if (s1 == 0 || s2 == 0)
			return FALSE;
		_stprintf_s(lpstrCpuID,leng,_T("%u_%u"),s1,s2);*/
		return TRUE;
	}
	BOOL ReadKeyValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,LPTSTR lpValue,DWORD dwLength)
	{
		BOOL bRet=FALSE;

		HKEY hKEY=NULL;
		DWORD dwValueLength=dwLength;
		DWORD dwType=REG_SZ; 

		DWORD dwVersion = 0;
		DWORD dwBuilder = 0;

		//ELEX_GetOSVersion(&dwVersion,&dwBuilder); 

		if (IsWow64_ID())
		{

			if(RegOpenKeyEx(hKey,lpSubKey,0,KEY_READ|KEY_WOW64_64KEY,&hKEY)!=ERROR_SUCCESS)
				goto ENDPOS;
		}
		else
		{
			if(RegOpenKeyEx(hKey,lpSubKey,0,KEY_READ,&hKEY)!=ERROR_SUCCESS)
				goto ENDPOS;
		}


		if(RegQueryValueEx(hKEY,lpValueName,NULL,&dwType,(LPBYTE)lpValue,&dwValueLength)!=ERROR_SUCCESS)
			goto ENDPOS;

		bRet=TRUE;

ENDPOS:

		if(hKEY!=NULL)
			RegCloseKey(hKEY);

		return bRet;
	}
	void GetOSGUID(LPTSTR lpstrOSID,DWORD leng)
	{

		ReadKeyValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Cryptography"),_T("MachineGuid"),lpstrOSID,leng);
	}

#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.    
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.    
#define  IOCTL_GET_DRIVE_INFO   0x0007c088    
#define  IOCTL_GET_VERSION          0x00074080    

	typedef struct _GETVERSIONOUTPARAMS    
	{    
		BYTE bVersion;      // Binary driver version.    
		BYTE bRevision;     // Binary driver revision.    
		BYTE bReserved;     // Not used.    
		BYTE bIDEDeviceMap; // Bit map of IDE devices.    
		DWORD fCapabilities; // Bit mask of driver capabilities.    
		DWORD dwReserved[4]; // For future use.    
	} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;  


	string ConvertName(char const * name)
	{
		if (name == 0)
		{
			return string("unnamed");
		}
		string ret(name);

		for (size_t i = 0; i < ret.size(); ++i)
		{
			char ch = ret[i];
			if (((ch >= '0') && (ch <= '9'))
				|| ((ch >= 'A') && (ch <= 'Z'))
				|| ((ch >= 'a') && (ch <= 'z'))
				|| (ch == '-'))
			{
				continue;
			}
			ret[i] = 'X';
		}
		return ret;
	}


	void getGUID(wchar_t * wcsRand, unsigned int len)
	{ 

		//IDINFO dinfo;
		//if (GetPhysicalDriveInfoInNT(0,&dinfo) && strlen(dinfo.sSerialNumber) > 0)
		//{
		//	string sn = dinfo.sSerialNumber;
		//	sn = ReplaceString(sn," ","_");
		//	string model = dinfo.sModelNumber;
		//	model = ReplaceString(model," ","_");
		//	//printf_s("%s_%s\r\n",model.c_str(),sn.c_str());

		//	sn = ConvertName(sn.c_str());
		//	model = ConvertName(model.c_str());

		//	int nNewSize = ::MultiByteToWideChar(CP_UTF8, 0, sn.c_str(), -1, NULL, 0);
		//	wchar_t* pwszSerialBuf = new wchar_t[ nNewSize + 1 ];
		//	memset( pwszSerialBuf, 0, nNewSize + 1 );
		//	::MultiByteToWideChar(CP_UTF8, 0, sn.c_str(), nNewSize, pwszSerialBuf, nNewSize);

		//	nNewSize = ::MultiByteToWideChar(CP_UTF8, 0, model.c_str(), -1, NULL, 0);
		//	wchar_t* pwszModelBuf = new wchar_t[ nNewSize + 1 ];
		//	memset( pwszModelBuf, 0, nNewSize + 1 );
		//	::MultiByteToWideChar(CP_UTF8, 0, model.c_str(), nNewSize, pwszModelBuf, nNewSize);

		//	ZeroMemory( wcsRand, len * sizeof(wchar_t) );
		//	swprintf_s(wcsRand,len,L"%s_%s",pwszModelBuf,pwszSerialBuf);

		//	delete []pwszModelBuf;
		//	delete []pwszSerialBuf;

		//}
		//else
		//{	

			std::string sid;
			rlz_lib::GetMachineId(&sid);
			int nNewSize = ::MultiByteToWideChar(CP_UTF8, 0, sid.c_str(), -1, NULL, 0);
			wchar_t* pwszBuf = new wchar_t[nNewSize + 1];
			memset(pwszBuf, 0, nNewSize + 1);
			::MultiByteToWideChar(CP_UTF8, 0, sid.c_str(), nNewSize, pwszBuf, nNewSize);

			ZeroMemory(wcsRand, len * sizeof(wchar_t));
			swprintf_s(wcsRand, len, L"%s", pwszBuf);

			delete[]pwszBuf;
		//}
	}

	// ���豸
	// filename: �豸�ġ��ļ�����(�豸·��)
	HANDLE OpenDevice(LPCTSTR filename)
	{
		HANDLE hDevice;

		// ���豸
		hDevice = ::CreateFile(filename,            // �ļ���
			GENERIC_READ | GENERIC_WRITE,          // ��д��ʽ
			FILE_SHARE_READ | FILE_SHARE_WRITE,    // ����ʽ
			NULL,                    // Ĭ�ϵİ�ȫ������
			OPEN_EXISTING,           // ������ʽ
			0,                       // ���������ļ�����
			NULL);                   // �������ģ���ļ�

		return hDevice;
	}

	// ����������IDENTIFY DEVICE���������豸��Ϣ
	// hDevice: �豸���
	// pIdInfo:  �豸��Ϣ�ṹָ��
	BOOL IdentifyDevice(HANDLE hDevice, PIDINFO pIdInfo)
	{
		PSENDCMDINPARAMS pSCIP;      // �������ݽṹָ��
		PSENDCMDOUTPARAMS pSCOP;     // ������ݽṹָ��
		DWORD dwOutBytes;            // IOCTL������ݳ���
		BOOL bResult;                // IOCTL����ֵ

		// ��������/������ݽṹ�ռ�
		pSCIP = (PSENDCMDINPARAMS)::GlobalAlloc(LMEM_ZEROINIT, sizeof(SENDCMDINPARAMS) - 1);
		pSCOP = (PSENDCMDOUTPARAMS)::GlobalAlloc(LMEM_ZEROINIT, sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1);

		// ָ��ATA/ATAPI����ļĴ���ֵ
		//    pSCIP->irDriveRegs.bFeaturesReg = 0;
		//    pSCIP->irDriveRegs.bSectorCountReg = 0;
		//    pSCIP->irDriveRegs.bSectorNumberReg = 0;
		//    pSCIP->irDriveRegs.bCylLowReg = 0;
		//    pSCIP->irDriveRegs.bCylHighReg = 0;
		//    pSCIP->irDriveRegs.bDriveHeadReg = 0;
		pSCIP->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;

		// ָ������/������ݻ�������С
		pSCIP->cBufferSize = 0;
		pSCOP->cBufferSize = sizeof(IDINFO);

		// IDENTIFY DEVICE
		bResult = ::DeviceIoControl(hDevice,        // �豸���
			DFP_RECEIVE_DRIVE_DATA,                 // ָ��IOCTL
			pSCIP, sizeof(SENDCMDINPARAMS) - 1,     // �������ݻ�����
			pSCOP, sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1,    // ������ݻ�����
			&dwOutBytes,                // ������ݳ���
			(LPOVERLAPPED)NULL);        // ��ͬ��I/O

		// �����豸�����ṹ
		::memcpy(pIdInfo, pSCOP->bBuffer, sizeof(IDINFO));

		// �ͷ�����/������ݿռ�
		::GlobalFree(pSCOP);
		::GlobalFree(pSCIP);

		return bResult;
	}

	// ��SCSI MINI-PORT��������IDENTIFY DEVICE���������豸��Ϣ
	// hDevice: �豸���
	// pIdInfo:  �豸��Ϣ�ṹָ��
	BOOL IdentifyDeviceAsScsi(HANDLE hDevice, int nDrive, PIDINFO pIdInfo)
	{
		PSENDCMDINPARAMS pSCIP;     // �������ݽṹָ��
		PSENDCMDOUTPARAMS pSCOP;    // ������ݽṹָ��
		PSRB_IO_CONTROL pSRBIO;     // SCSI����������ݽṹָ��
		DWORD dwOutBytes;           // IOCTL������ݳ���
		BOOL bResult;               // IOCTL����ֵ

		// ��������/������ݽṹ�ռ�
		pSRBIO = (PSRB_IO_CONTROL)::GlobalAlloc(LMEM_ZEROINIT,
			sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1);
		pSCIP = (PSENDCMDINPARAMS)((char *)pSRBIO + sizeof(SRB_IO_CONTROL));
		pSCOP = (PSENDCMDOUTPARAMS)((char *)pSRBIO + sizeof(SRB_IO_CONTROL));

		// �������/�������
		pSRBIO->HeaderLength = sizeof(SRB_IO_CONTROL);
		pSRBIO->Timeout = 10000;
		pSRBIO->Length = sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1;
		pSRBIO->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
		::strncpy_s((char *)pSRBIO->Signature,8, "SCSIDISK", 8);

		// ָ��ATA/ATAPI����ļĴ���ֵ
		//    pSCIP->irDriveRegs.bFeaturesReg = 0;
		//    pSCIP->irDriveRegs.bSectorCountReg = 0;
		//    pSCIP->irDriveRegs.bSectorNumberReg = 0;
		//    pSCIP->irDriveRegs.bCylLowReg = 0;
		//    pSCIP->irDriveRegs.bCylHighReg = 0;
		//    pSCIP->irDriveRegs.bDriveHeadReg = 0;
		pSCIP->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
		pSCIP->bDriveNumber = nDrive;

		// IDENTIFY DEVICE
		bResult = ::DeviceIoControl(hDevice,    // �豸���
			IOCTL_SCSI_MINIPORT,                // ָ��IOCTL
			pSRBIO, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDINPARAMS) - 1,    // �������ݻ�����
			pSRBIO, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1,    // ������ݻ�����
			&dwOutBytes,            // ������ݳ���
			(LPOVERLAPPED)NULL);    // ��ͬ��I/O

		// �����豸�����ṹ
		::memcpy(pIdInfo, pSCOP->bBuffer, sizeof(IDINFO));

		// �ͷ�����/������ݿռ�
		::GlobalFree(pSRBIO);

		return bResult;
	}

	// �����е��ַ������ߵ�
	// ԭ����ATA/ATAPI�е�WORD����Windows���õ��ֽ�˳���෴
	// �����������Ѿ����յ�������ȫ��������������������������
	void AdjustString(char* str, int len)
	{
		char ch;
		int i;

		// �����ߵ�
		for (i = 0; i < len; i += 2)
		{
			ch = str[i];
			str[i] = str[i + 1];
			str[i + 1] = ch;
		}

		// �����Ҷ���ģ�����Ϊ����� (ȥ����ߵĿո�)
		i = 0;
		while ((i < len) && (str[i] == ' ')) i++;

		::memmove(str, &str[i], len - i);

		// ȥ���ұߵĿո�
		i = len - 1;
		while ((i >= 0) && (str[i] == ' '))
		{
			str[i] = '\0';
			i--;
		}
	}

	// ��ȡIDEӲ�̵��豸��Ϣ���������㹻Ȩ��
	// nDrive: ��������(0=��һ��Ӳ�̣�1=0=�ڶ���Ӳ�̣�......)
	// pIdInfo: �豸��Ϣ�ṹָ��
	BOOL GetPhysicalDriveInfoInNT(int nDrive, PIDINFO pIdInfo)
	{
		HANDLE hDevice;         // �豸���
		BOOL bResult;           // ���ؽ��
		wchar_t szFileName[20];    // �ļ���

		::wsprintf(szFileName,L"\\\\.\\PhysicalDrive%d", nDrive);

		hDevice = OpenDevice(szFileName);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		// IDENTIFY DEVICE
		bResult = IdentifyDevice(hDevice, pIdInfo);

		if (bResult)
		{
			// �����ַ���
			AdjustString(pIdInfo->sSerialNumber, 20);
			AdjustString(pIdInfo->sModelNumber, 40);
			AdjustString(pIdInfo->sFirmwareRev, 8);
		}

		::CloseHandle (hDevice);

		return bResult;
	}

	// ��SCSI������ȡIDEӲ�̵��豸��Ϣ������Ȩ����Լ
	// nDrive: ��������(0=Primary Master, 1=Promary Slave, 2=Secondary master, 3=Secondary slave)
	// pIdInfo: �豸��Ϣ�ṹָ��
	BOOL GetIdeDriveAsScsiInfoInNT(int nDrive, PIDINFO pIdInfo)
	{
		HANDLE hDevice;         // �豸���
		BOOL bResult;           // ���ؽ��
		wchar_t szFileName[20];    // �ļ���

		::wsprintf(szFileName,L"\\\\.\\Scsi%d:", nDrive/2);

		hDevice = OpenDevice(szFileName);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		// IDENTIFY DEVICE
		bResult = IdentifyDeviceAsScsi(hDevice, nDrive%2, pIdInfo);

		// ����ǲ��ǿմ�
		if (pIdInfo->sModelNumber[0] == '\0')
		{
			bResult = FALSE;
		}

		if (bResult)
		{
			// �����ַ���
			AdjustString(pIdInfo->sSerialNumber, 20);
			AdjustString(pIdInfo->sModelNumber, 40);
			AdjustString(pIdInfo->sFirmwareRev, 8);
		}

		return bResult;
	}
}}}