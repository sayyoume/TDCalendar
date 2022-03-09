#pragma once

namespace DuiLib {
	class UILIB_API CLabelEx : public DuiLib::CLabelUI
	{
	public:
		CLabelEx();
		~CLabelEx();

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void SetText(LPCTSTR pstrText);

		SIZE EstimateSize(SIZE szAvailable);
		void PaintText(HDC hDC);

	private:
		void DrawText(HDC hDC, RECT &rc, DWORD dwStyle);
		bool m_bAntiAlias;
	};
}