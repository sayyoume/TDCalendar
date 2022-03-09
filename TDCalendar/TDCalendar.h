#pragma  once

#include <string>

/*
 * 第几周计算公式: 先算出今天是今年的第几天，再除以7取整+1
*/

class TDCalendar
{
public:
	long long DateDiff(int iStartYear , int iStartMonth , int iStartDay , int iEndYear, int iEndMonth, int iEndDay); //计算公历两个日期间相差的天数  1年1月1日 --- 65535年12月31日
	
	bool      IsLeapYear(int nYear);                        //是否闰年
	int       WeekDay(int iYear, int iMonth, int iDay);     //计算iYear,iMonth,iDay对应是星期几            1年1月1日 --- 65535年12月31日
	int       MonthDays(int iYear, int iMonth);             //返回iYear年iMonth月的天数                    1年1月 --- 65535年12月
	int       GetYearDays(int iYear);						//返回iYear年的总天数
	long long LunarYearDays(int iLunarYear);	            //返回阴历iLunarYear年的总天数                 1901年1月---2050年12月
	long long GetLunarLeapMonth(int iLunarYear);	        //返回阴历iLunarYear年的闰月月份，如没有返回0  1901年1月---2050年12月
	
	//返回阴历x年阴历x月的天数，如果x为闰月，
	//高字为第二个iLunarMonth月的天数，否则高字为0  1901年1月---2050年12月
	long  LunarMonthDays(int iLunarYear, int iLunarMonth);
	int   AfterDayLunarDate(int &iYear, int &iMonth, int &iDay, long long iSpanDays);    //计算从1901年1月1日过iSpanDays天后的阴历日期
	int   GetLunarHolDay(int iYear, int iMonth, int iDay);                             //计算公历iYear年iMonth月iDay日对应的节气 0-24，0表不是节气
	void  GetLunarDate(int iYear, int iMonth, int iDay,int &iLunarYear, int &iLunarMonth, int &iLunarDay); //根据阳历得到阴历 1901年1月1日---2050年12月31日

	void  FormatLunarYear(int iYear, std::wstring& sYear);     //把年格式化成天干记年法表示的字符串
	void  FormatMonth(int iMonth, std::wstring& sMonth);       //把月格式化成天干记年法表示的字符串
	void  FormatLunarDay(int iDay, std::wstring& sDay);        //把天格式化成中文字符串 1-30
};
