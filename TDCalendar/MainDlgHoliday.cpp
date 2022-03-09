#include "MainDlg.h"
#include <time.h>

void MainDlg::SetHolidayBkimage(int iYear, int iMonth, int iDay, DuiLib::COptionExUI* pOption)
{
	bool bHoliday = false;
	bool bWork = false;
	if (iYear == 2016)
	{
		if (iMonth == 12)
		{
			if (iDay == 31)//��Ϣ
			{
				bHoliday = true;
			}
		}
	}
	else if (iYear == 2017)
	{
		if (iMonth == 1) {
			if (iDay == 1 || iDay == 2 || iDay == 27 || iDay == 28 || iDay == 29 || iDay == 30 || iDay == 31)
			{
				bHoliday = true;
			}
			if (iDay == 22)//�ϰ�
			{
				bWork = true;
			}
		}
		else if (iMonth == 2) {
			if (iDay == 1 || iDay == 2) {
				bHoliday = true;
			}
			if (iDay == 4)//�ϰ�
			{
				bWork = true;
			}
		}
		else if (iMonth == 4) {
			if (iDay == 1)//�ϰ�
			{
				bWork = true;
			}
			if (iDay == 2 || iDay == 3 || iDay == 4 || iDay == 29 || iDay == 30)
			{
				bHoliday = true;
			}
		}
		else if (iMonth == 5) {
			if (iDay == 1 || iDay == 28 || iDay == 29 || iDay == 30)//��Ϣ
			{
				bHoliday = true;
			}
			if (iDay == 27)//�ϰ�
			{
				bWork = true;
			}
		}

		else if (iMonth == 9) {
			if (iDay == 30)//�ϰ�
			{
				bWork = true;
			}
		}
		else if (iMonth == 10) {
			if (iDay == 1 || iDay == 2 || iDay == 3 || iDay == 4 || iDay == 5 || iDay == 6 || iDay == 7 || iDay == 8)
			{
				//��Ϣ
				bHoliday = true;
			}
		}
		else if (iYear == 2018)
		{
			if (iMonth == 1) {
				if (iDay == 1)
				{
					bHoliday = true;
				}
			}
			else if (iMonth == 2) {
				if (iDay >= 15 && iDay <= 21) {
					bHoliday = true;
				}
				if (iDay == 24)//�ϰ�
				{
					bWork = true;
				}
			}
			else if (iMonth == 4) {
				if (iDay == 1)//�ϰ�
				{
					bWork = true;
				}
				if (iDay == 2 || iDay == 3 || iDay == 4 || iDay == 29 || iDay == 30)
				{
					bHoliday = true;
				}
			}
			else if (iMonth == 5) {
				if (iDay == 1 || iDay == 28 || iDay == 29 || iDay == 30)//��Ϣ
				{
					bHoliday = true;
				}
				if (iDay == 27)//�ϰ�
				{
					bWork = true;
				}
			}

			else if (iMonth == 9) {
				if (iDay == 30)//�ϰ�
				{
					bWork = true;
				}
			}
			else if (iMonth == 10) {
				if (iDay == 1 || iDay == 2 || iDay == 3 || iDay == 4 || iDay == 5 || iDay == 6 || iDay == 7 || iDay == 8)
				{
					//��Ϣ
					bHoliday = true;
				}
			}

		}
		DuiLib::CDuiString sImage = L"file='main\\cadrbk.png' source='0,0,48,50'";
		DuiLib::CDuiString sImageSelect = L"file='main\\select.png' source='0,0,48,50'";
		if (bHoliday)
		{
			sImage = L"file='main\\jia.png' source='0,0,48,50'";
			sImageSelect = L"file='main\\jiaselect.png' source='0,0,48,50'";
		}
		if (bWork)
		{
			sImage = L"file='main\\work.png' source='0,0,48,50'";
			sImageSelect = L"file='main\\workselecp.png' source='0,0,48,50'";
		}

		pOption->SetNormalImage(sImage);
		pOption->SetSelectedImage(sImageSelect);
	}
}

void MainDlg::SetHoliday(DuiLib::CLabelUI* pControl, int iYear, int iMonth, int iDay, int iLunarYear, int iLunarMonth, int iLunarDay)
{

	if (iMonth == 2 && iDay == 14)
	{
		pControl->SetText(L"���˽�");
		pControl->SetTextColor(0xffff0000);
	}
	if (iMonth == 4 && iDay == 1)
	{
		pControl->SetText(L"���˽�");
		pControl->SetTextColor(0xffff0000);
	}
	if (iMonth == 5 && iDay == 1)
	{
		pControl->SetText(L"�Ͷ���");
		pControl->SetTextColor(0xffff0000);
	}
	if (iMonth == 5)
	{
		if (m_tdcalendar.WeekDay(iYear, iMonth, iDay) == 0)
		{
			m_iMotherDay++;
			if (m_iMotherDay == 2)
			{
				pControl->SetText(L"ĸ�׽�");
				pControl->SetTextColor(0xffff0000);
			}
		}
	}
	if (iMonth == 6 && iDay == 1)
	{
		pControl->SetText(L"��ͯ��");
		pControl->SetTextColor(0xffff0000);
	}
	if (iMonth == 8 && iDay == 1)
	{
		pControl->SetText(L"������");
		pControl->SetTextColor(0xffff0000);
	}
	if (iMonth == 9 && iDay == 10)
	{
		pControl->SetText(L"��ʦ��");
		pControl->SetTextColor(0xffff0000);
	}
	if (iMonth == 10 && iDay == 1)
	{
		pControl->SetText(L"�����");
		pControl->SetTextColor(0xffff0000);
	}
	if (iMonth == 12 && iDay == 25)
	{
		pControl->SetText(L"ʥ����");
		pControl->SetTextColor(0xffff0000);
	}
	if (iLunarMonth == 1 && iLunarDay == 1)
	{
		pControl->SetText(L"����");
		pControl->SetTextColor(0xffff0000);
	}
	if (iLunarMonth == 7 && iLunarDay == 7)
	{
		pControl->SetText(L"��Ϧ��");
		pControl->SetTextColor(0xffff0000);
	}
}