#include "MainDlg.h"
#include <time.h>
#include "..\Wintdsrv\resource.h"
#include "AboutDlg.h"

MainDlg::MainDlg() :m_iMotherDay(0)
{

}

MainDlg::~MainDlg()
{

}

#define DEF_TIME   WM_USER + 500

void MainDlg::Init()
{
	int ii  = 4 % 7;
	bool bl = true;
	//DuiLib::CControlUI *pSettingBtn = dynamic_cast<DuiLib::CButtonUI*>(m_pm.FindControl(_T("setbtn")));
	//if (pSettingBtn) pSettingBtn->OnEvent += DuiLib::MakeDelegate(this, &MainDlg::OnSetting);
	/*for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			wchar_t buff[128] = { 0 };
			swprintf_s(buff, L"option_select_%d_%d", i, j);
			DuiLib::COptionExUI* pOption = dynamic_cast<DuiLib::COptionExUI*>(m_pm.FindControl(buff));
			if (pOption) pOption->OnNotify += DuiLib::MakeDelegate(this, &MainDlg::OnOptOnClick);
			wchar_t buffUserdata[256]{};
			swprintf_s(buffUserdata, L"%d_%d", i, j);
			pOption->SetUserData(buffUserdata);
		}
	}*/


	time_t t;
	time(&t);
	struct tm newtime;
	localtime_s(&newtime, &t);

	m_iNowYear = newtime.tm_year + 1900;
	m_iNowMonth = newtime.tm_mon + 1;
	m_iNowDay = newtime.tm_mday;

	//打印日历
	PrintMonthCalendar(newtime.tm_year + 1900, newtime.tm_mon + 1);

	//初始化控件
	m_pLabelCanclender = static_cast<DuiLib::CLabelUI*>(m_pm.FindControl(L"textcalenter"));
	m_pLabelLunarcalenter = static_cast<DuiLib::CLabelUI*>(m_pm.FindControl(L"textLunarcalenter"));
	m_pLabelTime = static_cast<DuiLib::CLabelEx*>(m_pm.FindControl(L"texttime"));

	wchar_t wcalenter[256] = { 0 };
	swprintf_s(wcalenter, L"阳历 : %d-%02d-%d", newtime.tm_year + 1900, newtime.tm_mon + 1, newtime.tm_mday);
	m_pLabelCanclender->SetText(wcalenter);

	//阴历
	FormatLunarMonthDay(m_iNowYear, m_iNowMonth, m_iNowDay);


	//时间
	wchar_t buffTime[256] = { 0 };
	swprintf_s(buffTime, L"%02d:%02d:%02d", newtime.tm_hour, newtime.tm_min, newtime.tm_sec);
	m_pLabelTime->SetText(buffTime);

	//选择年
	m_pComboyear = static_cast<DuiLib::CComboExUI*>(m_pm.FindControl(_T("comboyear")));
	if (m_pComboyear != NULL)
		m_pComboyear->OnNotify += DuiLib::MakeDelegate(this, &MainDlg::OnSelectYear);

	//选择月
	m_pComMonth = static_cast<DuiLib::CComboExUI*>(m_pm.FindControl(_T("combomonth")));
	if (m_pComMonth != NULL)
		m_pComMonth->OnNotify += DuiLib::MakeDelegate(this, &MainDlg::OnSelectMonth);

	m_pEditYear = static_cast<DuiLib::CEditUI*>(m_pm.FindControl(L"year_edit"));

	//初始化年月控件 1901-2050
	for (int i = 1900; i < 2051; i++)
	{
		DuiLib::CListLabelElementUI* pListElement = new DuiLib::CListLabelElementUI();
		wchar_t buff[256] = { 0 };
		swprintf_s(buff, L"%d", i);
		pListElement->SetText(buff);
		RECT rc;
		rc.left = 20;
		rc.top = 1;
		rc.right = 1;
		rc.bottom = 1;
		pListElement->SetTextPadding(rc);
		m_pComboyear->Add(pListElement);
	}

	for (int i = 1; i < 13; i++)
	{
		DuiLib::CListLabelElementUI* pListElement = new DuiLib::CListLabelElementUI();
		wchar_t buff[256] = { 0 };
		swprintf_s(buff, L"%d月", i);
		pListElement->SetText(buff);
		pListElement->SetAttribute(L"font", L"3");
		RECT rc;
		rc.left = 10;
		rc.top = 1;
		rc.right = 1;
		rc.bottom = 1;
		pListElement->SetTextPadding(rc);
		m_pComMonth->Add(pListElement);
	}
	m_iSelectDay = m_iNowDay;
	today();
	SetTimer(m_hWnd, DEF_TIME, 1000, NULL);

	

}

void MainDlg::Notify(DuiLib::TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		/*for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 7; j++)
			{
				wchar_t buff[256] = { 0 };
				swprintf_s(buff, L"option_select_%d_%d", i, j);
				DuiLib::COptionExUI* pOption = dynamic_cast<DuiLib::COptionExUI*>(m_pm.FindControl(buff));
				if (msg.pSender == m_pm.FindControl(buff))
				{
					swprintf_s(buff, L"text_%d_%d", i, j);
					DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
					std::wstring sText = pLabel->GetText();
					m_iSelectDay = _wtoi(sText.c_str());

					wchar_t wcalenter[256] = { 0 };
					swprintf_s(wcalenter, L"阳历 : %d-%02d-%d", _wtoi(m_pComboyear->GetText()), _wtoi(m_pComMonth->GetText()), _wtoi(pLabel->GetText()));
					m_pLabelCanclender->SetText(wcalenter);

					FormatLunarMonthDay(_wtoi(m_pComboyear->GetText()), _wtoi(m_pComMonth->GetText()), _wtoi(pLabel->GetText()));

					break;
				}
			}
		}*/

		if (msg.pSender == m_pm.FindControl(L"btntoday"))
		{
			today();
		}
		if (msg.pSender == m_pm.FindControl(L"btn_left"))
		{
			std::wstring sYear = m_pComboyear->GetText();
			std::wstring sMonth = m_pComMonth->GetText();
			 int iYear = _wtoi(sYear.c_str());
			 int iMonth = _wtoi(sMonth.c_str());
			 int iLastYear = iYear;
			 int iLastMonth = iMonth-1;

			if (iMonth == 1)
			{
				iLastMonth = 12;
				iLastYear = iYear - 1;
			}
			else
			{
				iLastMonth = iMonth - 1;
				iLastYear = iYear;
			}

			
			m_pComboyear->SelectItem(iLastYear-1900);
			m_pComMonth->SelectItem(iLastMonth-1);
		}
		if (msg.pSender == m_pm.FindControl(L"btn_right"))
		{
			std::wstring sYear = m_pComboyear->GetText();
			std::wstring sMonth = m_pComMonth->GetText();
			int iYear = _wtoi(sYear.c_str());
			int iMonth = _wtoi(sMonth.c_str());

			int iNextYear = iYear;
			int iNextMonth = iMonth - 1;
			if (iMonth == 12)
			{
				iNextMonth = 1;
				iNextYear = iYear + 1;
			}
			else
			{
				iNextMonth = iMonth + 1;
				iNextYear = iYear;
			}
			m_pComboyear->SelectItem(iNextYear-1900);
			m_pComMonth->SelectItem(iNextMonth-1);
		}
		else if (msg.pSender == m_pm.FindControl(L"btn_calc"))
		{
			ShellExecute(NULL, L"open", L"calc.exe", NULL, NULL, SW_SHOW);
		}
		else if (msg.pSender == m_pm.FindControl(L"btn_jd"))
		{
			std::wstring sJd = L"http://union-click.jd.com/jdc?e=0&p=AyIPZRprFDJWWA1FBCVbV0IUEEULRFRBSkAOClBMW2VcAkF4THtPTzlGXnpHUEsEa3lFfBRNVxkyEQ9VH14TABsFZRhSEwAbAFQTXiUyEgZlUDUUMhoAUhJTEQMUBmUbXxQBEQJTGV0TBRMPZRxrV1JaTwtfB0EyIjdl&t=W1dCFBBFC0RUQUpADgpQTFs%3D";
			ShellExecute(NULL, L"open", sJd.c_str(), NULL, NULL, SW_SHOW);
		}
		else if (msg.pSender == m_pm.FindControl(L"btn_tb"))
		{
			std::wstring sTaobao = L"https://ai.taobao.com?pid=mm_14846141_9642939_75096765";
			ShellExecute(NULL, L"open", sTaobao.c_str(), NULL, NULL, SW_SHOW);
		}
		else if (msg.pSender == m_pm.FindControl(L"btn_tm"))
		{
			std::wstring sTmall = L"https://s.click.taobao.com/t?e=m%3D2%26s%3DDa3lIEfBkfYcQipKwQzePCperVdZeJvipRe%2F8jaAHci5VBFTL4hn2VOtODU6g7dar8hK%2FDw%2Bbm2ZIrK8HyK8yf0hpEbF4%2BoYTAVHNZ9SfYeZPoqK5IWdq70CYBEjBf0rLxRPKN2FDAck%2FCKKDVvEs8YMXU3NNCg%2F";
			ShellExecute(NULL, L"open", sTmall.c_str(), NULL, NULL, SW_SHOW);
		}
		else if (msg.pSender == m_pm.FindControl(L"btn_12306"))
		{
			std::wstring sTmall = L"https://www.12306.cn";
			ShellExecute(NULL, L"open", sTmall.c_str(), NULL, NULL, SW_SHOW);
		}

	}
	WindowBase::Notify(msg);
}


LRESULT MainDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg) {
	case WM_CREATE:						lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:					lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:					lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	//case WM_LBUTTONDOWN:				
	case WM_TIMER:						lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
	default:
		bHandled = FALSE;
	}
	if (bHandled) return lRes;
	return WindowBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT MainDlg::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~(WS_CAPTION | WS_MAXIMIZEBOX);
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_pm.Init(m_hWnd);

	WindowBase::LoadSkin(_T("main\\skin.xml"), NULL);
	Init();
	return 0;
}

LRESULT MainDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostQuitMessage(0L);
	bHandled = FALSE;
	return 0;
}

bool MainDlg::OnSelectMonth(void* wparam)
{
	DuiLib::TNotifyUI* pMsg = (DuiLib::TNotifyUI*)wparam;
	if (pMsg->sType != DUI_MSGTYPE_ITEMSELECT)
		return false;

	m_pEditYear->SetVisible(false);
	//InitOption();
	DuiLib::CComboExUI *pCombo = dynamic_cast<DuiLib::CComboExUI*>(pMsg->pSender);
	if (pCombo == NULL) return false;

	int nCursel = pMsg->wParam;

	DuiLib::CControlUI *pItem = dynamic_cast<DuiLib::CControlUI*>(pCombo->GetItemAt(nCursel));
	if (pItem == NULL) return false;
	

	ClearControl();
	PrintMonthCalendar(_wtoi(m_pComboyear->GetText()), _wtoi(pItem->GetText()));
	m_iSelectMonth = _wtoi(pItem->GetText());

	wchar_t wcalenter[256] = { 0 };
	swprintf_s(wcalenter, L"阳历 : %d-%02d-%d", _wtoi(m_pComboyear->GetText()), _wtoi(pItem->GetText()), m_iSelectDay);
	m_pLabelCanclender->SetText(wcalenter);

	FormatLunarMonthDay(_wtoi(m_pComboyear->GetText()), _wtoi(pItem->GetText()), m_iSelectDay);

	for (int i = 0; i < 6; i++) //列
	{
		for (int j = 0; j < 7; j++) //行
		{
			wchar_t buff[128] = { 0 };
			swprintf_s(buff, L"text_%d_%d", i, j);
			DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
			if (_wtoi(pLabel->GetText()) == m_iSelectDay)
			{
				swprintf_s(buff, L"option_select_%d_%d", i, j);
				DuiLib::COptionExUI* pOption = dynamic_cast<DuiLib::COptionExUI*>(m_pm.FindControl(buff));
				pOption->Selected(true);
				break;
			}
		}
	}

	if (_wtoi(m_pComboyear->GetText()) == m_iNowYear &&  _wtoi(pItem->GetText()) == m_iNowMonth)
		today();

	return true;
}

bool MainDlg::OnSelectYear(void* wparam)
{
	
	DuiLib::TNotifyUI* pMsg = (DuiLib::TNotifyUI*)wparam;
	if (pMsg->sType != DUI_MSGTYPE_ITEMSELECT)
		return false;
	
	m_pEditYear->SetVisible(false);
	InitOption();
	DuiLib::CComboExUI *pCombo = dynamic_cast<DuiLib::CComboExUI*>(pMsg->pSender);
	if (pCombo == NULL) return false;

	
	int nCursel = pMsg->wParam;

	DuiLib::CControlUI *pItem = dynamic_cast<DuiLib::CControlUI*>(pCombo->GetItemAt(nCursel));
	if (pItem == NULL) return false;



	std::wstring  sMonth = m_pComMonth->GetText();
	int nPos = sMonth.rfind(L"月");
	sMonth = sMonth.substr(0, nPos);

	int iYear = _wtoi(pItem->GetText());
	int iMonth = _wtoi(sMonth.c_str());
	m_iSelectYear = iYear;
	//打印日历
	ClearControl();
	PrintMonthCalendar(iYear,iMonth);

	wchar_t wcalenter[256] = { 0 };
	swprintf_s(wcalenter, L"阳历: %d-%02d-%d", iYear, iMonth, m_iSelectDay);
	m_pLabelCanclender->SetText(wcalenter);

	FormatLunarMonthDay(iYear, iMonth, m_iSelectDay);


	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			wchar_t buff[128] = { 0 };
			swprintf_s(buff, L"text_%d_%d", i, j);
			DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
			if (_wtoi(pLabel->GetText()) == m_iSelectDay)
			{
				swprintf_s(buff, L"option_select_%d_%d", i, j);
				DuiLib::COptionExUI* pOption = dynamic_cast<DuiLib::COptionExUI*>(m_pm.FindControl(buff));
				pOption->Selected(true);
				break;
			}
		}
	}

	if (iYear == m_iNowYear && iMonth == m_iNowMonth)
		today();
	
	return true;
}

void MainDlg::PrintMonthCalendar(int iYear, int iMonth)
{
	m_iMotherDay = 0; //每次打印月的时候将m_iMotherDay置0，5月份第二个周日是母亲节
	int iLunarYear;
	int iLunarMonth;
	int iLunarDay;

	m_tdcalendar.AfterDayLunarDate(iLunarYear, iLunarMonth, iLunarDay, m_tdcalendar.DateDiff(1901, 1, 1, iYear, iMonth, 1));
	int day = 1;

	//得到当前月上个月的天数

	if (iMonth == 1)
	{
		m_iLasyMonth = 12;
		m_iLasyYear = iYear - 1;
	}
	else
	{
		m_iLasyMonth = iMonth - 1;
		m_iLasyYear = iYear;
	}

	if (iMonth == 12)
	{
		m_NextMonth = 1;
		m_NextYear = iYear + 1;
	}
	else
	{
		m_NextMonth = iMonth + 1;
		m_NextYear = iYear;
	}

	//上个月的天数
	int nLastMonthDays = m_tdcalendar.MonthDays(m_iLasyYear, m_iLasyMonth);
	//下个月的天数
	int nNextMonthDays = m_tdcalendar.MonthDays(m_NextYear, m_NextMonth);

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			//上个月日期
			int iTemp = m_tdcalendar.WeekDay(iYear, iMonth, 1);
			if (j < iTemp && i == 0)
			{
				wchar_t buff[128] = { 0 };
				swprintf_s(buff, L"option_select_%d_%d", i, j);
				DuiLib::COptionExUI* pOption = dynamic_cast<DuiLib::COptionExUI*>(m_pm.FindControl(buff));
				if (pOption) pOption->OnNotify += DuiLib::MakeDelegate(this, &MainDlg::OnShowlastMonth);
				pOption->SetTag(1);


				wchar_t bufftest[MAX_PATH]{};
				swprintf_s(bufftest, L"dddddd--------PrintMonthCalendar   i---%d,j----%d", i, j);
				OutputDebugString(bufftest);
				wchar_t buffUserdata[256]{};
				swprintf_s(buffUserdata, L"%d_%d", i, j);
				pOption->SetUserData(buffUserdata);
				
				//设置阳历
				wchar_t buffText[128] = { 0 };
				swprintf_s(buffText, L"text_%d_%d", i, j);
				DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buffText));

				swprintf_s(buffText, L"%d", nLastMonthDays - (iTemp-j)+1);
				pLabel->SetText(buffText);
				pLabel->SetFont(2);
				pLabel->SetTextColor(0xffb9b9b9);


				//设置阴历
				std::wstring sLastLunarDay = L"";
				int iLastLunarYear, iLastLunarMonth, iLastLunarDay;

				//根据阳历得到阴历
				m_tdcalendar.GetLunarDate(m_iLasyYear, m_iLasyMonth, nLastMonthDays - (iTemp - j) + 1, iLastLunarYear, iLastLunarMonth, iLastLunarDay);
				//格式化为中文字符串
				m_tdcalendar.FormatLunarDay(iLastLunarDay, sLastLunarDay);

				swprintf_s(buff, L"textyinli_%d_%d", i, j);
				DuiLib::CLabelUI* pLunaLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
				pLunaLabel->SetText(sLastLunarDay.c_str());
				pLunaLabel->SetTextColor(0xffbfbfbf);
				pLunaLabel->SetFont(0);
				
				SetHolidayBkimage(m_iLasyYear, m_iLasyMonth, nLastMonthDays - (iTemp - j) + 1, pOption);
				continue;
			}
				
			//下个月日期
			if (day > m_tdcalendar.MonthDays(iYear, iMonth) && iMonth !=0)
			{
				int iAllDays = m_tdcalendar.MonthDays(iYear, iMonth);
				int iTemp = m_tdcalendar.WeekDay(iYear, iMonth, iAllDays);

				int iNowDat = day -iAllDays  ;

				wchar_t buff[128] = { 0 };
				swprintf_s(buff, L"option_select_%d_%d", i, j);
				DuiLib::COptionExUI* pOption = dynamic_cast<DuiLib::COptionExUI*>(m_pm.FindControl(buff));
				pOption->SetTag(3);
				
				wchar_t buffUserdata[256]{};
				swprintf_s(buffUserdata, L"%d_%d", i, j);
				pOption->SetUserData(buffUserdata);
				
				//设置阳历
				wchar_t buffText[128] = { 0 };
				swprintf_s(buffText, L"text_%d_%d", i, j);
				DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buffText));

				swprintf_s(buffText, L"%d", iNowDat);
				pLabel->SetText(buffText);
				pLabel->SetFont(2);
				pLabel->SetTextColor(0xffb9b9b9);

				//设置阴历
				std::wstring sLastLunarDay = L"";
				int iLastLunarYear, iLastLunarMonth, iLastLunarDay;

				//根据阳历得到阴历
				m_tdcalendar.GetLunarDate(m_NextYear, m_NextMonth, iNowDat, iLastLunarYear, iLastLunarMonth, iLastLunarDay);
				//格式化为中文字符串
				m_tdcalendar.FormatLunarDay(iLastLunarDay, sLastLunarDay);

				swprintf_s(buff, L"textyinli_%d_%d", i, j);
				DuiLib::CLabelUI* pLunaLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));


				pLunaLabel->SetText(sLastLunarDay.c_str());
				pLunaLabel->SetTextColor(0xffbfbfbf);
				pLunaLabel->SetFont(0);
				
				SetHolidayBkimage(m_NextYear, m_NextMonth, iNowDat, pOption);
				day++;
				continue;
				//break;
			}
				
			//当前月
			wchar_t buffOpt[128] = { 0 };
			swprintf_s(buffOpt, L"option_select_%d_%d", i, j);
			DuiLib::COptionExUI* pOption = dynamic_cast<DuiLib::COptionExUI*>(m_pm.FindControl(buffOpt));
			if (pOption) pOption->OnNotify += DuiLib::MakeDelegate(this, &MainDlg::OnShowNowMonth);
			wchar_t buffUserdata[256]{};
			swprintf_s(buffUserdata, L"%d_%d", i, j);
			pOption->SetUserData(buffUserdata);
			pOption->SetTag(2);

			SetHolidayBkimage(iYear, iMonth, day, pOption);
		
			
			//阳历
			wchar_t buff[128] = { 0 };
			swprintf_s(buff, L"text_%d_%d", i, j);
			DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));

			swprintf_s(buff, L"%d", day);
			pLabel->SetText(buff);
			pLabel->SetFont(2);
			pLabel->SetTextColor(0xff000000);

			//设置颜色 周六日为红色
			if (m_tdcalendar.WeekDay(iYear, iMonth, day) == 0 || m_tdcalendar.WeekDay(iYear, iMonth, day) == 6)
				pLabel->SetTextColor(0xffe02d2d);


			//阴历
			std::wstring sLunarDay = L"";

			//根据阳历得到阴历
			m_tdcalendar.GetLunarDate(iYear, iMonth, day, iLunarYear, iLunarMonth, iLunarDay);
			//格式化为中文字符串
			m_tdcalendar.FormatLunarDay(iLunarDay, sLunarDay);

			swprintf_s(buff, L"textyinli_%d_%d", i, j);
			DuiLib::CLabelUI* pLunaLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
			pLunaLabel->SetText(sLunarDay.c_str());
			pLunaLabel->SetTextColor(0xffa7a6aa);
			SetHoliday(pLunaLabel, iYear, iMonth, day, iLunarYear, iLunarMonth, iLunarDay);
			pLunaLabel->SetFont(0);
			day++;
		}
	}
}

void MainDlg::ClearControl()
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			wchar_t buff[128] = { 0 };
			swprintf_s(buff, L"text_%d_%d", i, j);
			DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
			pLabel->SetText(L"");

			swprintf_s(buff, L"textyinli_%d_%d", i, j);
			pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
			pLabel->SetText(L"");

			//清除背景
			swprintf_s(buff, L"option_select_%d_%d", i, j);
			DuiLib::COptionExUI* pOption = dynamic_cast<DuiLib::COptionExUI*>(m_pm.FindControl(buff));
			DuiLib::CDuiString sImage = L"file='main\\cadrbk.png' source='0,0,48,50'";
			pOption->SetNormalImage(sImage);
		}
	}
}

void MainDlg::today()
{

	time_t t;
	time(&t);
	struct tm newtime;
	localtime_s(&newtime, &t);
	
	m_pComboyear->SelectItem(newtime.tm_year);
	m_pComMonth->SelectItem(newtime.tm_mon);

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			wchar_t buff[128] = { 0 };
			swprintf_s(buff, L"text_%d_%d", i, j);
			DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
			std::wstring sText = pLabel->GetText();

			swprintf_s(buff, L"%d", m_iNowDay);

			if (_tcsicmp(buff, sText.c_str()) == 0)
			{
				swprintf_s(buff, L"option_select_%d_%d", i, j);
				DuiLib::COptionExUI* pOption = dynamic_cast<DuiLib::COptionExUI*>(m_pm.FindControl(buff));
				if (pOption != NULL && pOption->GetTag()==2)
				{
					DuiLib::CDuiString sImage = L"file='main\\today.png' source='0,0,48,50'";
					pOption->SetNormalImage(sImage);
					pOption->Selected(true);
				}

				wchar_t wcalenter[256] = { 0 };
				swprintf_s(wcalenter, L"阳历 : %d-%02d-%d", newtime.tm_year + 1900, newtime.tm_mon + 1, m_iNowDay);
				m_pLabelCanclender->SetText(wcalenter);

				FormatLunarMonthDay(m_iNowYear, m_iNowMonth, m_iNowDay);
				break;
			}
		}
	}

}

void MainDlg::SetSelect(int iYear, int iMonth, int iDay)
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			wchar_t buff[128] = { 0 };
			swprintf_s(buff, L"text_%d_%d", i, j);
			DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
			std::wstring sText = pLabel->GetText();

			wchar_t bufftoday[125]{};
			
			swprintf_s(bufftoday, L"%d", iDay);

			if (_tcsicmp(bufftoday, sText.c_str()) == 0)
			{
				wchar_t wcalenter[256] = { 0 };
				swprintf_s(wcalenter, L"阳历 : %d-%02d-%d", iYear, iMonth, iDay);
				m_pLabelCanclender->SetText(wcalenter);

				FormatLunarMonthDay(iYear, iMonth, iDay);
				return;
			}
		}
	}
}

LRESULT MainDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	if (wParam == DEF_TIME)
	{
		time_t t;
		time(&t);
		struct tm newtime;
		localtime_s(&newtime, &t);
		wchar_t buff[256] = { 0 };
		swprintf_s(buff, L"%02d:%02d:%02d", newtime.tm_hour, newtime.tm_min, newtime.tm_sec);

		m_pLabelTime->SetText(buff);
	}

	return 0;
}

void MainDlg::FormatLunarMonthDay(int iYear,int iMonth, int iDay)
{
	int iLunarYear;
	int iLunarMonth;
	int iLunarDay;

	std::wstring sLunarMonth;
	std::wstring sLunarDay;

	m_tdcalendar.GetLunarDate(iYear, iMonth, iDay, iLunarYear, iLunarMonth, iLunarDay);
	m_tdcalendar.FormatMonth(iLunarMonth, sLunarMonth);
	m_tdcalendar.FormatLunarDay(iLunarDay, sLunarDay);
	sLunarMonth.append(sLunarDay);
	sLunarMonth = L"阴历 : " + sLunarMonth;
	m_pLabelLunarcalenter->SetText(sLunarMonth.c_str());
}

void MainDlg::InitOption()
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			wchar_t buff[128] = { 0 };
			swprintf_s(buff, L"option_select_%d_%d", i, j);
			DuiLib::COptionExUI* pOption = dynamic_cast<DuiLib::COptionExUI*>(m_pm.FindControl(buff));
			pOption->SetEnabled(true);
		}
	}
}

bool MainDlg::OnShowlastMonth(void* param)
{
	DuiLib::TNotifyUI* pNotifyUI = static_cast<DuiLib::TNotifyUI*>(param);
	if (pNotifyUI == NULL || pNotifyUI->sType != DUI_MSGTYPE_CLICK)
		return false;
	
	if (pNotifyUI->sType == DUI_MSGTYPE_CLICK)
	{
		UINT_PTR tag =  pNotifyUI->pSender->GetTag();
		if (tag != 1)
			return true;

		std::wstring sUserData = pNotifyUI->pSender->GetUserData();
		int nPos =sUserData.find(L"_");
		std::wstring sI =sUserData.substr(0, nPos);
		std::wstring sJ = sUserData.substr(nPos + 1, sUserData.length());
		int i = _wtoi(sI.c_str());
		int j = _wtoi(sJ.c_str());

		//int j = _wtoi((sUserData.substr(nPos + 1, sUserData.length())).c_str);
		wchar_t buff[MAX_PATH]{};
		swprintf_s(buff, L"text_%d_%d", i, j);
		DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
		std::wstring sText = pLabel->GetText();

		m_iSelectDay = _wtoi(sText.c_str());

		//m_pComboyear->SelectItem(m_iLasyYear - 1900);
		//m_pComMonth->SelectItem(m_iLasyMonth-1);

		SetSelect(m_iSelectYear, m_iSelectMonth, m_iSelectDay);

// 		wchar_t wcalenter[256] = { 0 };
// 		swprintf_s(wcalenter, L"阳历 : %d-%02d-%d", _wtoi(m_pComboyear->GetText()), _wtoi(m_pComMonth->GetText()), _wtoi(pLabel->GetText()));
// 		m_pLabelCanclender->SetText(wcalenter);
// 
// 		FormatLunarMonthDay(_wtoi(m_pComboyear->GetText()), _wtoi(m_pComMonth->GetText()), _wtoi(pLabel->GetText()));

	}
	OutputDebugString(L"dddddd-------------LBUDDONDOWN");

	return false;
	DuiLib::TEventUI* pEvent = (DuiLib::TEventUI*)param;
	if (pEvent == NULL) return true;
	DuiLib::COptionExUI* pItem = dynamic_cast<DuiLib::COptionExUI*>(pEvent->pSender);
	DuiLib::CVerticalLayoutUI* pVer = (DuiLib::CVerticalLayoutUI*)pItem->FindSubControl(L"subcontrol_ver");
	pVer->SetBkColor(0xff000000);
	if (pEvent->Type == DuiLib::UIEVENT_BUTTONDOWN)
	{
		OutputDebugString(L"dddddd---------------UIEVENT_BUTTONDOWN\r\n");
		pVer->SetVisible(true);
	}
// 	else if (pEvent->Type == DuiLib::UIEVENT_MOUSELEAVE)
// 	{
// 		OutputDebugString(L"dddddd---------------mouselevar\r\n");
// 		pVer->SetVisible(false);
// 	}

	return true;
}

bool MainDlg::OnShowNowMonth(void* param)
{
	DuiLib::TNotifyUI* pNotifyUI = static_cast<DuiLib::TNotifyUI*>(param);
	if (pNotifyUI == NULL || pNotifyUI->sType != DUI_MSGTYPE_CLICK)
		return false;

	if (pNotifyUI->sType == DUI_MSGTYPE_CLICK)
	{
		UINT_PTR tag = pNotifyUI->pSender->GetTag();
		if (tag != 2)
			return true;

		std::wstring sUserData = pNotifyUI->pSender->GetUserData();
		int nPos = sUserData.find(L"_");
		std::wstring sI = sUserData.substr(0, nPos);
		std::wstring sJ = sUserData.substr(nPos + 1, sUserData.length());
		int i = _wtoi(sI.c_str());
		int j = _wtoi(sJ.c_str());

		wchar_t buff[MAX_PATH]{};
		swprintf_s(buff, L"text_%d_%d", i, j);
		DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
		std::wstring sText = pLabel->GetText();
		m_iSelectDay = _wtoi(sText.c_str());

		wchar_t wcalenter[256] = { 0 };
		swprintf_s(wcalenter, L"阳历 : %d-%02d-%d", _wtoi(m_pComboyear->GetText()), _wtoi(m_pComMonth->GetText()), _wtoi(pLabel->GetText()));
		m_pLabelCanclender->SetText(wcalenter);

		FormatLunarMonthDay(_wtoi(m_pComboyear->GetText()), _wtoi(m_pComMonth->GetText()), _wtoi(pLabel->GetText()));

	}
}

bool MainDlg::OnShowNextMonth(void* param)
{
	DuiLib::TNotifyUI* pNotifyUI = static_cast<DuiLib::TNotifyUI*>(param);
	if (pNotifyUI == NULL || pNotifyUI->sType != DUI_MSGTYPE_CLICK)
		return false;

	if (pNotifyUI->sType == DUI_MSGTYPE_CLICK)
	{
		UINT_PTR tag = pNotifyUI->pSender->GetTag();
		if (tag != 3)
			return true;

		std::wstring sUserData = pNotifyUI->pSender->GetUserData();
		int nPos = sUserData.find(L"_");
		std::wstring sI = sUserData.substr(0, nPos);
		std::wstring sJ = sUserData.substr(nPos + 1, sUserData.length());
		int i = _wtoi(sI.c_str());
		int j = _wtoi(sJ.c_str());

		wchar_t buff[MAX_PATH]{};
		swprintf_s(buff, L"text_%d_%d", i, j);
		DuiLib::CLabelUI* pLabel = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(buff));
		std::wstring sText = pLabel->GetText();
		m_iSelectDay = _wtoi(sText.c_str());

		wchar_t wcalenter[256] = { 0 };
		swprintf_s(wcalenter, L"阳历 : %d-%02d-%d", _wtoi(m_pComboyear->GetText()), _wtoi(m_pComMonth->GetText()), _wtoi(pLabel->GetText()));
		m_pLabelCanclender->SetText(wcalenter);

		FormatLunarMonthDay(_wtoi(m_pComboyear->GetText()), _wtoi(m_pComMonth->GetText()), _wtoi(pLabel->GetText()));
	}
}



bool MainDlg::OnSetting(void* param)
{
	DuiLib::TEventUI* pEvent = (DuiLib::TEventUI*)param;
	if (pEvent == NULL) return true;
	//	if (pEvent->Type == DuiLib::UIEVENT_SETCURSOR) return false;
	if (pEvent->Type != DuiLib::UIEVENT_BUTTONDOWN) return true;

	CAboutDlg* dlg = new CAboutDlg();
	dlg->Create(NULL, _T("TDCalendar"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, 0, 0, 0, 322, 392);
	dlg->ShowWindow();
	dlg->CenterWindow();

	return true;
}



bool MainDlg::OnOptOnClick(void* param)
{
	DuiLib::TNotifyUI* pNotifyUI = static_cast<DuiLib::TNotifyUI*>(param);
	if (pNotifyUI == NULL || pNotifyUI->sType != DUI_MSGTYPE_CLICK)
		return false;

	return true;
}