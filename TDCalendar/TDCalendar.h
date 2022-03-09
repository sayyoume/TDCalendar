#pragma  once

#include <string>

/*
 * �ڼ��ܼ��㹫ʽ: ����������ǽ���ĵڼ��죬�ٳ���7ȡ��+1
*/

class TDCalendar
{
public:
	long long DateDiff(int iStartYear , int iStartMonth , int iStartDay , int iEndYear, int iEndMonth, int iEndDay); //���㹫���������ڼ���������  1��1��1�� --- 65535��12��31��
	
	bool      IsLeapYear(int nYear);                        //�Ƿ�����
	int       WeekDay(int iYear, int iMonth, int iDay);     //����iYear,iMonth,iDay��Ӧ�����ڼ�            1��1��1�� --- 65535��12��31��
	int       MonthDays(int iYear, int iMonth);             //����iYear��iMonth�µ�����                    1��1�� --- 65535��12��
	int       GetYearDays(int iYear);						//����iYear���������
	long long LunarYearDays(int iLunarYear);	            //��������iLunarYear���������                 1901��1��---2050��12��
	long long GetLunarLeapMonth(int iLunarYear);	        //��������iLunarYear��������·ݣ���û�з���0  1901��1��---2050��12��
	
	//��������x������x�µ����������xΪ���£�
	//����Ϊ�ڶ���iLunarMonth�µ��������������Ϊ0  1901��1��---2050��12��
	long  LunarMonthDays(int iLunarYear, int iLunarMonth);
	int   AfterDayLunarDate(int &iYear, int &iMonth, int &iDay, long long iSpanDays);    //�����1901��1��1�չ�iSpanDays������������
	int   GetLunarHolDay(int iYear, int iMonth, int iDay);                             //���㹫��iYear��iMonth��iDay�ն�Ӧ�Ľ��� 0-24��0���ǽ���
	void  GetLunarDate(int iYear, int iMonth, int iDay,int &iLunarYear, int &iLunarMonth, int &iLunarDay); //���������õ����� 1901��1��1��---2050��12��31��

	void  FormatLunarYear(int iYear, std::wstring& sYear);     //�����ʽ������ɼ��귨��ʾ���ַ���
	void  FormatMonth(int iMonth, std::wstring& sMonth);       //���¸�ʽ������ɼ��귨��ʾ���ַ���
	void  FormatLunarDay(int iDay, std::wstring& sDay);        //�����ʽ���������ַ��� 1-30
};
