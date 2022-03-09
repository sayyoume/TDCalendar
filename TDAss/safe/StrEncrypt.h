#pragma once
#include "../Ass.h"

namespace safe
{
	class APDDLL_DECLSPEC CStrEncrypt
	{
	public:
		static CStrEncrypt* shared();

		std::string StringEncrypt(const char* srcstr, int srclen = -1);   //�ַ�������
		std::string StringDecrypt(const char* srcstr);           //�ַ�������

		std::wstring WStringEncrypt(const std::wstring& srwstr, int srclen = -1);   //Unicode�ַ������� 
		std::wstring WStringDecrypt(const std::wstring& srwstr);                   //Unicode�ַ�������
	private:
		CStrEncrypt(void);
		~CStrEncrypt(void);
		void clearPtr();
		char UnIndex64(BYTE nIndex);
		BYTE Index64(char ch);
		void ToBase64(const char* instr, int len, char* outstr);
		void UnBase64(const char* instr, int len, char* outstr);
		char EncryptChar(char c) ;
		char randchar(); 
		char  HexToASCII(unsigned char  data_hex);
		bool HexGroupToString(char *OutStrBuffer, char *InHexBuffer, int HexLength);
		bool StringToHexGroup(char *OutHexBuffer, char *InStrBuffer, int strLength);

		unsigned long GetTime(void);
	};
}

