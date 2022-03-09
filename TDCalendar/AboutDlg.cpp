
#include "AboutDlg.h"
#include <string>

#define UNINSTALLSTRING L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{FCB860B2-EA0A-45D7-BD1C-9D790AD36F12}"

CAboutDlg::CAboutDlg()
{
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::OnPrepare()
{
}

void CAboutDlg::Init()
{
	m_pCloseBtn = dynamic_cast<DuiLib::CButtonUI*>(m_pm.FindControl(_T("btn_closebtn")));
	SetVersion();
}

void CAboutDlg::Notify(DuiLib::TNotifyUI& msg)
{
	if (msg.sType == _T("windowinit")) OnPrepare();
	else if (msg.sType == _T("click"))
	{
		if (msg.pSender == m_pCloseBtn)
		{
			::PostMessage(m_hWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);return;
		}else if (msg.pSender == m_pm.FindControl(L"about_ok"))
		{
			::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		}else if (msg.pSender == m_pm.FindControl(L"about_licenseprotocol"))
		{
			ShellExecute(NULL, L"open", _T("http://www.h4321.com"), NULL, NULL, SW_SHOWDEFAULT);
		}
	}
}

LRESULT CAboutDlg::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~(WS_CAPTION | WS_MAXIMIZEBOX);
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );

	m_pm.Init(m_hWnd);
	DuiLib::CDialogBuilder builder;

	DuiLib::CControlUI* pRoot = builder.Create(_T("main\\about.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();

	return 0;
}

LRESULT CAboutDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CAboutDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	bHandled = FALSE;

	return 0;
}

LRESULT CAboutDlg::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (::IsIconic(*this)) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CAboutDlg::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CAboutDlg::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CAboutDlg::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_pm.GetCaptionRect();
	if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
		DuiLib::CControlUI* pControl = static_cast<DuiLib::CControlUI*>(m_pm.FindControl(pt));
		if (pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0)
			return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CAboutDlg::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CAboutDlg::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CAboutDlg::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
	if (wParam == SC_CLOSE) {
		Close();
		bHandled = TRUE;
		return 0;
	}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CAboutDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg) {
	case WM_CREATE:					lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:					lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:				lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:				lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:				lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:				lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:				lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:					lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_GETMINMAXINFO:			lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
	case WM_SYSCOMMAND:				lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
	default:
		bHandled = FALSE;
	}
	if (bHandled) return lRes;
	if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CAboutDlg::SetVersion()
{
	
	std::wstring sText = L"版本：2.0.0.1";

	DuiLib::CLabelUI* pLabe = static_cast<DuiLib::CLabelUI*>(m_pm.FindControl(L"about_textVersion"));
	pLabe->SetText(sText.c_str());

}

LRESULT CAboutDlg::OnLanguageChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetVersion();

	return 0;
}
