
#pragma once

namespace DuiLib
{
	class UILIB_API COptionExUI : public CContainerUI
	{
	public:
		COptionExUI();
		~COptionExUI();
		LPCTSTR GetClass() const;
		LPCTSTR GetGroup() const;
		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
		UINT GetControlFlags() const;
		bool Activate();
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetGroup(LPCTSTR pStrGroupName = NULL);
		void DoEvent(TEventUI& event);
		bool IsSelected() const;
		virtual void Selected(bool bSelected);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedImage();
		void SetSelectedImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedHotImage();
		void SetSelectedHotImage(LPCTSTR pStrImage);
		void SetForeImage(LPCTSTR pStrImage);

		void PaintStatusImage(HDC hDC);
	protected:
		CDuiString		m_sGroupName;
		bool			m_bSelected;
		UINT            m_uButtonState;

		CDuiString m_sNormalImage;
		CDuiString m_sSelectedImage;
		CDuiString m_sSelectedHotImage;
		CDuiString		m_sForeImage;
	};

}