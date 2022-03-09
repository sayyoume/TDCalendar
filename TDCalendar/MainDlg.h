#pragma  once
#include "WindowBase.h"
#include "TDCalendar.h"
#include <functional>

class MainDlg : public WindowBase
{
public:
	MainDlg();
	~MainDlg();
public:
	void Init();
	void Notify(DuiLib::TNotifyUI& msg);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LPCTSTR GetWindowClassName() const { return _T("TDClockMainFrame"); }
private:
	void PrintMonthCalendar(int iYear,int iMonth); //打印日历
	bool OnSelectMonth(void* wparam);
	bool OnSelectYear(void* wparam);

	void ClearControl();
	void today();  //返回今天日期
	void SetSelect(int iYear,int iMonth,int iDay); //选择日期
	void FormatLunarMonthDay(int iYear,int iMonth,int iDay);

	void InitOption();

	bool OnOptOnClick(void* param);

	bool OnShowlastMonth(void* param);
	bool OnShowNowMonth(void* param);
	bool OnShowNextMonth(void* param);

	void SetHolidayBkimage(int iYear, int iMonth, int iDay, DuiLib::COptionExUI* pOption);
	bool OnSetting(void* param);

	void SetHoliday(DuiLib::CLabelUI* pControl, int iYear, int iMonth, int iDay, int iLunarYear, int iLunarMonth, int iLunarDay);
private:
	DuiLib::CComboExUI * m_pComboyear;
	DuiLib::CComboExUI * m_pComMonth;
	DuiLib::CLabelUI * m_pLabelCanclender;
	DuiLib::CLabelUI * m_pLabelLunarcalenter;
	DuiLib::CLabelEx * m_pLabelTime;
	DuiLib::CEditUI*   m_pEditYear;
	TDCalendar m_tdcalendar;
	int m_iSelectYear;
	int m_iSelectMonth;
	int m_iSelectDay; 
	int m_iNowYear  ;   //当前年
	int m_iNowMonth ;   //当前月
	int m_iNowDay;

	int m_iLasyYear;  //上1年
	int m_iLasyMonth; //上个月

	int m_NextYear;
	int m_NextMonth;

	int m_iMotherDay;
	
};