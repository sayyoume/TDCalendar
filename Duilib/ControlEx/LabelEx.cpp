#include "stdafx.h"
#include "LabelEx.h"
namespace DuiLib {

	CLabelEx::CLabelEx()
	{
		m_bAntiAlias = false;
	}

	CLabelEx::~CLabelEx()
	{
	}

	void CLabelEx::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("antialias")) == 0) {
			m_bAntiAlias = _tcscmp(pstrValue, _T("true")) == 0;
		}
		else
			CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CLabelEx::PaintText(HDC hDC)
	{
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		DrawText(hDC, rc, m_uTextStyle);
	}

	void CLabelEx::SetText(LPCTSTR pstrText)
	{
		DuiLib::CDuiString strText = pstrText;
		strText.Replace(_T("\\n"), _T("\n"));
		CLabelUI::SetText(strText);
	}

	void CLabelEx::DrawText(HDC hDC, RECT &rc, DWORD dwStyle)
	{
		Gdiplus::Graphics graphics(hDC);
		if (m_bAntiAlias)
			graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

		Gdiplus::Font font(hDC, m_pManager->GetFont(GetFont()));
		Gdiplus::RectF rectF((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)(rc.right - rc.left), (Gdiplus::REAL)(rc.bottom - rc.top));
		Gdiplus::Color cr(GetTextColor());

		Gdiplus::StringFormat stringFormat = Gdiplus::StringFormat::GenericTypographic();

		if ((dwStyle & DT_END_ELLIPSIS) != 0) {
			stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
		}
		else if ((dwStyle & DT_WORD_ELLIPSIS) != 0) {
			stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisWord);
		}
		else if ((dwStyle & DT_PATH_ELLIPSIS) != 0) {
			stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisPath);
		}

		int formatFlags = 0;
		if ((dwStyle & DT_NOCLIP) != 0) {
			formatFlags |= Gdiplus::StringFormatFlagsNoClip;
		}
		if ((dwStyle & DT_SINGLELINE) != 0) {
			formatFlags |= Gdiplus::StringFormatFlagsNoWrap;
		}

		stringFormat.SetFormatFlags(formatFlags);

		if ((dwStyle & DT_LEFT) != 0) {
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		}
		else if ((dwStyle & DT_CENTER) != 0) {
			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		}
		else if ((dwStyle & DT_RIGHT) != 0) {
			stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
		}
		else {
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		}
		stringFormat.GenericTypographic();
		if ((dwStyle & DT_TOP) != 0) {
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		}
		else if ((dwStyle & DT_VCENTER) != 0) {
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		}
		else if ((dwStyle & DT_BOTTOM) != 0) {
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		}
		else {
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		}

		if ((dwStyle & DT_CALCRECT) != 0)
		{
			Gdiplus::RectF bounds;
			graphics.MeasureString(m_sText, m_sText.GetLength(), &font, rectF, &stringFormat, &bounds);

			// MeasureString存在计算误差，这里加一像素
			rc.bottom = rc.top + (long)bounds.Height + 1;
			rc.right = rc.left + (long)bounds.Width + 1;
		}
		else
		{
			graphics.DrawString(m_sText, m_sText.GetLength(), &font, rectF, &stringFormat, &Gdiplus::SolidBrush(cr));
		}
	}

	SIZE CLabelEx::EstimateSize(SIZE szAvailable)
	{
		if (m_AutoSize)
		{
			RECT rc = { 0 };
			DrawText(m_pManager->GetPaintDC(), rc, m_uTextStyle | DT_CALCRECT);
			m_cxyFixed.cx = rc.right - rc.left + m_rcTextPadding.left + m_rcTextPadding.right;
			m_cxyFixed.cy = rc.bottom - rc.top + m_rcTextPadding.top + m_rcTextPadding.bottom;
			if (m_cxyFixed.cx < m_cxyMin.cx) m_cxyFixed.cx = m_cxyMin.cx;
			if (m_cxyFixed.cy < m_cxyMin.cy) m_cxyFixed.cy = m_cxyMin.cy;
			return m_cxyFixed;
		}
		if (m_cxyFixed.cy == 0) return DuiLib::CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 4);
		return CControlUI::EstimateSize(szAvailable);
	}

}