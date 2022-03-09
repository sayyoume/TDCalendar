#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <string>


#include "StrEncrypt.h"
#include "../JTerseUtilities.h"


#define BND_U      0x01    /* upper */
#define BND_L      0x02    /* lower */
#define BND_D      0x04    /* digit */
#define BND_C      0x08    /* cntrl */
#define BND_P      0x10    /* punct */
#define BND_S      0x20    /* white space (space/lf/tab) */
#define BND_X      0x40    /* hex digit */
#define BND_SP     0x80    /* hard space (0x20) */

unsigned char _ctypebnd[] = {0x00,                 /* EOF */
	BND_C,BND_C,BND_C,BND_C,BND_C,BND_C,BND_C,BND_C,                        /* 0-7 */
	BND_C,BND_C|BND_S,BND_C|BND_S,BND_C|BND_S,BND_C|BND_S,BND_C|BND_S,BND_C,BND_C,         /* 8-15 */
	BND_C,BND_C,BND_C,BND_C,BND_C,BND_C,BND_C,BND_C,                        /* 16-23 */
	BND_C,BND_C,BND_C,BND_C,BND_C,BND_C,BND_C,BND_C,                        /* 24-31 */
	BND_S|BND_SP,BND_P,BND_P,BND_P,BND_P,BND_P,BND_P,BND_P,                    /* 32-39 */
	BND_P,BND_P,BND_P,BND_P,BND_P,BND_P,BND_P,BND_P,                        /* 40-47 */
	BND_D,BND_D,BND_D,BND_D,BND_D,BND_D,BND_D,BND_D,                        /* 48-55 */
	BND_D,BND_D,BND_P,BND_P,BND_P,BND_P,BND_P,BND_P,                        /* 56-63 */
	BND_P,BND_U|BND_X,BND_U|BND_X,BND_U|BND_X,BND_U|BND_X,BND_U|BND_X,BND_U|BND_X,BND_U,      /* 64-71 */
	BND_U,BND_U,BND_U,BND_U,BND_U,BND_U,BND_U,BND_U,                        /* 72-79 */
	BND_U,BND_U,BND_U,BND_U,BND_U,BND_U,BND_U,BND_U,                        /* 80-87 */
	BND_U,BND_U,BND_U,BND_P,BND_P,BND_P,BND_P,BND_P,                        /* 88-95 */
	BND_P,BND_L|BND_X,BND_L|BND_X,BND_L|BND_X,BND_L|BND_X,BND_L|BND_X,BND_L|BND_X,BND_L,      /* 96-103 */
	BND_L,BND_L,BND_L,BND_L,BND_L,BND_L,BND_L,BND_L,                        /* 104-111 */
	BND_L,BND_L,BND_L,BND_L,BND_L,BND_L,BND_L,BND_L,                        /* 112-119 */
	BND_L,BND_L,BND_L,BND_P,BND_P,BND_P,BND_P,BND_C,                        /* 120-127 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /* 128-143 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /* 144-159 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /* 160-175 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /* 176-191 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /* 192-207 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /* 208-223 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /* 224-239 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};               /* 240-255 */

extern unsigned char _ctypebnd[];

#define bnd_isdigit(c) ((_ctypebnd+1)[c]&(BND_D))
#define bnd_islower(c) ((_ctypebnd+1)[c]&(BND_L))
#define bnd_isupper(c) ((_ctypebnd+1)[c]&(BND_U))

// #define bnd_isdigit(c) ((c) >= '0' && (c) <= '9')
// #define bnd_isupper(x) ((x)>='A' && (x) <= 'Z')
// #define bnd_islower(x) ((x)>= 'a' && (x) <= 'z')


namespace safe
{

CStrEncrypt* CStrEncrypt::shared()
{
	static CStrEncrypt obj;
	return &obj;
}

CStrEncrypt::CStrEncrypt(void)
{
}


CStrEncrypt::~CStrEncrypt(void)
{
	
}

// ��һ��������(0��63)ת��Ϊ�ַ�(��ĸ�����֡�'_'��'.')  
char CStrEncrypt::UnIndex64(BYTE nIndex) 
{ 
	char ch;

	nIndex %= 64;                    // ȡ��[0,64)��Χ��

	if (nIndex < 26)                // [0,26)���ش�д��ĸ 
	{ 
		ch = (char)('A' + nIndex); 
	} 
	else if (nIndex < 52)            // 26+[0,26)����Сд��ĸ 
	{ 
		ch = (char)('a' + nIndex - 26); 
	} 
	else if (nIndex < 62)            // 52+[0,10)�������� 
	{ 
		ch = (char)('0' + nIndex - 52); 
	} 
	else if (nIndex == 62)            // 62����'_' 
	{ 
		ch = '_'; 
	} 
	else if (nIndex == 63)            // 63����'.' 
	{ 
		ch = '.'; 
	} 
	else 
	{ 
		ch = 'A'; 
	}

	return ch; 
}

// Index64: ��һ���ַ�(��ĸ�����֡�'_'��'.')ת��Ϊ������(0��63)

BYTE CStrEncrypt::Index64(char ch) 
{ 
	BYTE nIndex;

	if (bnd_isupper(ch)) 
	{ 
		nIndex = ch - 'A'; 
	} 
	else if (bnd_islower(ch)) 
	{ 
		nIndex = 26 + ch - 'a'; 
	} 
	else if (bnd_isdigit(ch)) 
	{ 
		nIndex = 52 + ch - '0'; 
	} 
	else if (ch == '_') 
	{ 
		nIndex = 62; 
	} 
	else if (ch == '.') 
	{ 
		nIndex = 63; 
	} 
	else 
	{ 
		nIndex = 0; 
	}

	return nIndex; 
}

// ToBase64: ��һ�������ַ���ת��Ϊ���ַ�(��ĸ�����֡�'_'��'.')��ɵ��ַ���

// instr �����ַ���������Ϊlen��Ϊ3�ı��� 
// len   instr���ַ�������Ϊ3�ı��� 
// outstr ������ַ�(��ĸ�����֡�'_'��'.')��ɵ��ַ���������Ϊ4�ı��� 
void CStrEncrypt::ToBase64(const char* instr, int len, char* outstr) 
{ 
	//ASSERT(instr && len > 0 && len % 3 == 0);

	int i, j; 
	BYTE ch1, ch2, ch3;

	i = 0; 
	j = 0; 
	while (i + 2 < len) 
	{ 
		ch1 = (BYTE)instr[i]; 
		ch2 = (BYTE)instr[i + 1]; 
		ch3 = (BYTE)instr[i + 2];

		outstr[j] = UnIndex64(ch1 >> 2); 
		outstr[j + 1] = UnIndex64(((ch1 & 0x3) << 4) | (ch2 >> 4)); 
		outstr[j + 2] = UnIndex64(((ch2 & 0x0f) << 2) | (ch3 >> 6)); 
		outstr[j + 3] = UnIndex64(ch3 & 0x3f);

		i += 3; 
		j += 4; 
	} 
	outstr[j] = '\0'; 
}

// UnBase64: ��һ�����ַ�(��ĸ�����֡�'_'��'.')��ɵ��ַ���ת��Ϊ�����ַ���

// instr ���ַ�(��ĸ�����֡�'_'��'.')��ɵ��ַ���������Ϊlen��Ϊ4�ı��� 
// len   instr���ַ�������Ϊ4�ı��� 
// outstr �����ַ���������Ϊ3�ı��� 
void CStrEncrypt::UnBase64(const char* instr, int len, char* outstr) 
{ 
	//ASSERT(instr && len % 4 == 0);

	int i, j; 
	BYTE ch1, ch2, ch3, ch4;

	i = 0; 
	j = 0; 
	while (i + 3 < len) 
	{ 
		ch1 = Index64(instr[i]); 
		ch2 = Index64(instr[i + 1]); 
		ch3 = Index64(instr[i + 2]); 
		ch4 = Index64(instr[i + 3]);

		outstr[j] =  (ch1 << 2) | ((ch2 >> 4) & 0x3); 
		outstr[j + 1] = (ch2 << 4) | ((ch3 >> 2) & 0xf); 
		outstr[j + 2] = (ch3 << 6) | ch4;

		i += 4; 
		j += 3; 
	} 
	outstr[j] = '\0'; 
}

// EncryptChar: ��һ���ַ�������λ�任�����λ���䣬����7λ�Ե���1-7,2-6,3-5��

char CStrEncrypt::EncryptChar(char c) 
{ 
	BYTE x = 0;

	x += (c & 0x80); 
	x += (c & 0x40) >> 6; 
	x += (c & 0x20) >> 4; 
	x += (c & 0x10) >> 2; 
	x += (c & 0x08);// << 3; 
	x += (c & 0x04) << 2; 
	x += (c & 0x02) << 4; 
	x += (c & 0x01) << 6;

	return x; 
}

char CStrEncrypt::randchar() 
{ 
	char c = (char)rand(); 
	return 0 == c ? '\xAA' : c; 
}

// StringEncrypt: �������ִ�������Ĵ��ĳ���Ϊ(3 + len(srcstr))/3*4

std::string CStrEncrypt::StringEncrypt(const char* srcstr, int srclen) 
{ 
	if (srclen < 0) 
	{ 
		srclen = (NULL == srcstr) ? 0 : strlen(srcstr); 
	}

	int inlen = (1 + srclen + 2) + 1; 
	int outlen = (3 + srclen) / 3 * 4 + 1;

	char *buf = new char[inlen + outlen]; 
	char *inbuf = buf; 
	char *outbuf = buf + inlen;

	memset(buf, 0, sizeof(char) * (inlen + outlen));

	// ����Դ����(inbuf+1)��ÿ���ַ�������λ�任 
	for (int i = 0; i < srclen; i++) 
	{ 
		inbuf[i + 1] = EncryptChar(srcstr[i]); 
	}

	// ���ó��ȱ���ַ� inbuf[0] 
	// ��λ�任���ܲ���0�ַ����������λ����inbuf�ĳ���, 00:3n, 01:3n+1,10:3n+2 
	// 
	srand( GetTime() ); 

	inbuf[0] = randchar() & (~0x03);    // �����λΪ0ʱ��leadlen % 3 == 0

	int actlen = srclen + 1; 
	if (actlen % 3 == 1)        // ԭ��3n+1������������ַ���֤inbuf����Ϊ3n 
	{ 
		inbuf[0] |= 0x01; 
		inbuf[actlen] = randchar(); 
		inbuf[actlen + 1] = randchar(); 
		actlen += 2; 
	} 
	else if (actlen % 3 == 2)    // ԭ��3n+2����1������ַ���֤inbuf����Ϊ3n 
	{ 
		inbuf[0] |= 0x02; 
		inbuf[actlen] = randchar(); 
		actlen++; 
	}

	// ��inbufת����outbuf��outbuf����ĸ�����֡�'_'��'.'��ɣ�����Ϊ4�ı��� 
	ToBase64(inbuf, actlen, outbuf);

	std::string strResult(outbuf);

	delete[] buf;

	return strResult; 
}



std::string CStrEncrypt::StringDecrypt(const char* srcstr) 
{ 
	int srclen = (NULL == srcstr) ? 0 : strlen(srcstr) / 4 * 4; 
	int len = srclen * 3 / 4;

	if (0 == len) 
	{ 
		return ""; 
	}

	char *chBuf = new char[len + 1];

	UnBase64(srcstr, srclen, chBuf);

	if (1 == (chBuf[0] & 0x03)) 
		len -= 2; 
	else if (2 == (chBuf[0] & 0x03)) 
		len--; 
	chBuf[len] = 0;

	for (int i = 1; i < len; i++) 
	{ 
		chBuf[i] = EncryptChar(chBuf[i]); 
	}

	std::string strResult(chBuf + 1, len - 1);

	delete[] chBuf;

	return strResult; 
}
char CStrEncrypt::HexToASCII(unsigned char  data_hex)
{ 
	char  ASCII_Data;
	ASCII_Data=data_hex & 0x0F;
	if(ASCII_Data<10) 
		ASCII_Data=ASCII_Data+0x30; //��0--9��
	else  
		ASCII_Data=ASCII_Data+0x37;       //��A--F��
	return ASCII_Data;
}

bool CStrEncrypt::HexGroupToString(char *OutStrBuffer, char *InHexBuffer, int HexLength)
{
	int i, k=0;

	if (OutStrBuffer == NULL || InHexBuffer == NULL)
	{ 
		return false;
	}

	if (HexLength < 2)
	{
		return false;
	}

	for(i=0;i<HexLength;i++)
	{
		OutStrBuffer[k++]=HexToASCII((InHexBuffer[i]>>4)&0x0F);

		OutStrBuffer[k++]=HexToASCII(InHexBuffer[i]&0x0F);
	}

	OutStrBuffer[k]='\0';

	return true;
}
unsigned long CStrEncrypt::GetTime(void)
{
	FILETIME filetime = {0};

	GetSystemTimeAsFileTime(&filetime);

	return  filetime.dwLowDateTime ? filetime.dwLowDateTime: filetime.dwHighDateTime;
}
bool CStrEncrypt::StringToHexGroup(char *OutHexBuffer, char *InStrBuffer, int strLength)
{
	int i, k=0;

	unsigned char HByte,LByte;

	if(strLength % 2 !=0 )
	{
		return false;
	}

	if (OutHexBuffer == NULL || InStrBuffer == NULL)
	{
		return false;
	}

	for(i=0; i<strLength; i=i+2)
	{
		if(InStrBuffer[i]>='0' && InStrBuffer[i]<='9')
		{
			HByte=InStrBuffer[i]-'0';
		}
		else if(InStrBuffer[i]>='A' && InStrBuffer[i]<='F')
		{
			HByte=InStrBuffer[i]-'A' +10;
		}
		else
		{
			HByte=InStrBuffer[i];
			return false;
		}

		HByte=HByte <<4;

		HByte = HByte & 0xF0;

		if(InStrBuffer[i+1]>='0' && InStrBuffer[i+1]<='9')
		{
			LByte=InStrBuffer[i+1]-'0';
		}
		else if(InStrBuffer[i+1]>='A' && InStrBuffer[i+1]<='F')
		{
			LByte=InStrBuffer[i+1]-'A' +10;
		}
		else
		{
			LByte=InStrBuffer[i];
			return false;
		}
		OutHexBuffer[k++]=HByte |LByte;
	}
	return true;
}

std::wstring CStrEncrypt::WStringEncrypt(const std::wstring& srwstr, int srclen)   //Unicode�ַ������� 
{
    std::string strtemp = JTerse::core::ws2s(srwstr);

	return JTerse::core::s2ws(StringEncrypt(strtemp.c_str(), srclen));
}

std::wstring CStrEncrypt::WStringDecrypt(const std::wstring& srwstr)                   //Unicode�ַ�������
{
    std::string strtemp = JTerse::core::ws2s(srwstr);

	return JTerse::core::s2ws(StringDecrypt(strtemp.c_str()));
}

void * _memset(void *ptr,int c,size_t count)
{  
	void * start = ptr;  
	while(count--)  
	{  
		*(char *)ptr = (char)c;  
		ptr = (char *)ptr + 1;  
	}  
	return start;  
}

}