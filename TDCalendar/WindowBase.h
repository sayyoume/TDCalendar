#pragma once

#include "UIlib.h"

class  CDialogBuilderCallbackEx : public  DuiLib::IDialogBuilderCallback
{
public:
	DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);
};

class  WindowBase : public DuiLib::CWindowWnd, public DuiLib::INotifyUI
{
public:
	WindowBase();
	~WindowBase();
public:
	LPCTSTR GetWindowClassName() const { return _T("UG_BASE_WINDOW"); }
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnPrepare() {}
	void Notify(DuiLib::TNotifyUI& msg);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	void LoadSkin(LPCTSTR lpszXmlFile, CDialogBuilderCallbackEx* cb = NULL);
	void InitControl();

public:
	DuiLib::CPaintManagerUI m_pm;
	DuiLib::CButtonUI* m_pCloseBtn;
	DuiLib::CButtonUI* m_pMinBtn;
};
