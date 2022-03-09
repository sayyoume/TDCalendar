#include "stdafx.h"
#include "UIOptionEx.h"

namespace DuiLib
{
	DuiLib::COptionExUI::COptionExUI()
	{

	}

	COptionExUI::~COptionExUI()
	{

	}

	LPCTSTR COptionExUI::GetClass() const
	{
		return _T("OptionExUI");
	}

	LPVOID COptionExUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, L"OptionEx") == 0) return static_cast<COptionExUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	LPCTSTR COptionExUI::GetGroup() const
	{
		return m_sGroupName;
	}

	void COptionExUI::SetGroup(LPCTSTR pStrGroupName)
	{
		if (pStrGroupName == NULL)
		{
			if (m_sGroupName.IsEmpty()) return;
			m_sGroupName.Empty();
		}
		else {
			if (m_sGroupName == pStrGroupName) return;
			if (!m_sGroupName.IsEmpty() && m_pManager)
			{
				m_pManager->RemoveOptionGroup(m_sGroupName, this);
			}
			m_sGroupName = pStrGroupName;
		}

		if (!m_sGroupName.IsEmpty())
		{
			if (m_pManager)
				m_pManager->AddOptionGroup(m_sGroupName, this);
		}
		else 
		{
			if (m_pManager)
				m_pManager->RemoveOptionGroup(m_sGroupName, this);
		}

		Selected(m_bSelected);
	}

	UINT COptionExUI::GetControlFlags() const
	{
		return (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}


	bool COptionExUI::Activate()
	{
		if (!CContainerUI::Activate())
			return false;

		if (m_pManager != NULL)
			m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);

		if (!m_sGroupName.IsEmpty())
		{
			Selected(true);
		}
		else
		{
			Selected(!m_bSelected);
		}
		return true;
	}

	void COptionExUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		CContainerUI::SetManager(pManager, pParent, bInit);
		if (bInit && !m_sGroupName.IsEmpty()) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
		}
	}

	bool COptionExUI::IsSelected() const
	{
		return m_bSelected;
	}

	void COptionExUI::Selected(bool bSelected)
	{
		if (m_bSelected == bSelected) return;
		m_bSelected = bSelected;
		if (m_bSelected) m_uButtonState |= UISTATE_SELECTED;
		else m_uButtonState &= ~UISTATE_SELECTED;

		if (m_pManager != NULL) {
			if (!m_sGroupName.IsEmpty()) {
				if (m_bSelected) {
					CStdPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
					for (int i = 0; i < aOptionGroup->GetSize(); i++) {
						COptionExUI* pControl = static_cast<COptionExUI*>(aOptionGroup->GetAt(i));
						if (pControl != this) {
							pControl->Selected(false);
						}
					}
					m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
				}
			}
			else {
				m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}

		Invalidate();
	}

	void COptionExUI::DoEvent(TEventUI& event)
	{
			if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pParent != NULL) m_pParent->DoEvent(event);
			else CContainerUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_SETFOCUS)
		{
			Invalidate();
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			Invalidate();
		}
		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
		{
			if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			wchar_t buff[MAX_PATH] = { 0 };
			swprintf_s(buff, L"TDCALEN %d-%d-%d-%d", m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
			OutputDebugString(buff);
			if ((m_uButtonState & UISTATE_CAPTURED) != 0)
			{
				
				if (::PtInRect(&m_rcItem, event.ptMouse))
				{
					m_uButtonState |= UISTATE_PUSHED;
					
				}
				else
				{
					m_uButtonState &= ~UISTATE_PUSHED;
				}
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_BUTTONUP)
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
				if (::PtInRect(&m_rcItem, event.ptMouse)) Activate();
				m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			if (IsEnabled()) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			if (IsEnabled()) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if (event.Type == UIEVENT_SETCURSOR) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		CContainerUI::DoEvent(event);
	}

	void COptionExUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("group")) == 0) SetGroup(pstrValue);
		else if (_tcsicmp(pstrName, _T("normalimage")) == 0) SetNormalImage(pstrValue);
		else if (_tcscmp(pstrName, _T("selected")) == 0) Selected(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("selectedimage")) == 0) SetSelectedImage(pstrValue);
		else if (_tcscmp(pstrName, _T("selectedhotimage")) == 0) SetSelectedHotImage(pstrValue);
		else if (_tcscmp(pstrName, _T("foreimage")) == 0) SetForeImage(pstrValue);
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}
	
	LPCTSTR COptionExUI::GetNormalImage()
	{
		return m_sNormalImage;
	}

	void COptionExUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR COptionExUI::GetSelectedImage()
	{
		return m_sSelectedImage;
	}

	void COptionExUI::SetSelectedImage(LPCTSTR pStrImage)
	{
		m_sSelectedImage = pStrImage;
		Invalidate();
	}


	LPCTSTR COptionExUI::GetSelectedHotImage()
	{
		return m_sSelectedHotImage;
	}

	void COptionExUI::SetSelectedHotImage(LPCTSTR pStrImage)
	{
		m_sSelectedHotImage = pStrImage;
		Invalidate();
	}

	void COptionExUI::PaintStatusImage(HDC hDC)
	{
		m_uButtonState &= ~UISTATE_PUSHED;

		if ((m_uButtonState & UISTATE_HOT) != 0 && IsSelected() && !m_sSelectedHotImage.IsEmpty())
		{
			if (!DrawImage(hDC, (LPCTSTR)m_sSelectedHotImage))
				m_sSelectedHotImage.Empty();
			else goto Label_ForeImage;
		}
		else if ((m_uButtonState & UISTATE_SELECTED) != 0) 
		{
			if (!m_sSelectedImage.IsEmpty()) {
				if (!DrawImage(hDC, (LPCTSTR)m_sSelectedImage)) m_sSelectedImage.Empty();
				else goto Label_ForeImage;
			}
			/*else if (m_dwSelectedBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
				return;
			}*/
		}

		if (!m_sNormalImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)m_sNormalImage)) m_sNormalImage.Empty();
			else goto Label_ForeImage;
		}

		CContainerUI::PaintStatusImage(hDC);

	Label_ForeImage:
		if (!m_sForeImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)m_sForeImage)) m_sForeImage.Empty();
		}
	}

	void COptionExUI::SetForeImage(LPCTSTR pStrImage)
	{
		m_sForeImage = pStrImage;
		Invalidate();
	}

}
