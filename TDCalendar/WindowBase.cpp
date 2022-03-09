#include "WindowBase.h"



	WindowBase::WindowBase()
	:m_pCloseBtn(NULL)
	,m_pMinBtn(NULL)
	{
	}

	WindowBase::~WindowBase()
	{
	}

	void WindowBase::InitControl()
	{
		m_pCloseBtn = static_cast<DuiLib::CButtonUI*>(m_pm.FindControl(L"btnclose"));
		m_pMinBtn = static_cast<DuiLib::CButtonUI*>(m_pm.FindControl(L"btnmin"));
	}

	void WindowBase::LoadSkin(LPCTSTR lpszXmlFile, CDialogBuilderCallbackEx* cb)
	{
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI* pRoot = builder.Create(lpszXmlFile, (UINT)0, cb, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);
		InitControl();
	}

	LRESULT WindowBase::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return TRUE;
	}

	LRESULT WindowBase::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (::IsIconic(*this)) bHandled = FALSE;
		return(wParam == 0) ? TRUE : FALSE;
	}

	LRESULT WindowBase::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return TRUE;
	}

	LRESULT WindowBase::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return TRUE;
	}

	LRESULT WindowBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		RECT rcCaption = m_pm.GetCaptionRect();
		if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom)
		{
			DuiLib::CControlUI* pControl = static_cast<DuiLib::CControlUI*>(m_pm.FindControl(pt));
			if (pControl == NULL)
			{
				return HTCAPTION;
			}

			if (!pControl->IsMouseEnabled())
			{
				return HTCAPTION;
			}
		}
		return HTCLIENT;
	}

	LRESULT WindowBase::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		DuiLib::CDuiRect rcWork = oMonitor.rcWork;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMaxPosition.x = rcWork.left;
		lpMMI->ptMaxPosition.y = rcWork.top;
		lpMMI->ptMaxSize.x = rcWork.right;
		lpMMI->ptMaxSize.y = rcWork.bottom;

		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == SC_CLOSE) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if (::IsZoomed(*this) != bZoomed) {
			if (!bZoomed) {
				DuiLib::CControlUI* pControl = static_cast<DuiLib::CControlUI*>(m_pm.FindControl(_T("btnmax")));
				if (pControl) pControl->SetVisible(false);
				pControl = static_cast<DuiLib::CControlUI*>(m_pm.FindControl(_T("btnrestore")));
				if (pControl) pControl->SetVisible(true);
			}
			else {
				DuiLib::CControlUI* pControl = static_cast<DuiLib::CControlUI*>(m_pm.FindControl(_T("btnmax")));
				if (pControl) pControl->SetVisible(true);
				pControl = static_cast<DuiLib::CControlUI*>(m_pm.FindControl(_T("btnrestore")));
				if (pControl) pControl->SetVisible(false);
			}
		}
		return lRes;
	}

	LRESULT WindowBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg) {
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
		}
		if (bHandled) return lRes;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	void WindowBase::Notify(DuiLib::TNotifyUI& msg)
	{
		if (msg.sType == _T("click"))
		{
			DuiLib::CDuiString strName = msg.pSender->GetName();
			if (msg.pSender == m_pm.FindControl(L"btnclose"))
			{
				::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
				return;
			}
			else if (msg.pSender == m_pm.FindControl(L"btnmin"))
			{
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return;
			}
		}

	}

	LRESULT WindowBase::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}
