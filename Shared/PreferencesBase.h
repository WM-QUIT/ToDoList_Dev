#if !defined(AFX_PREFERENCESBASE_H__19D0237F_28DC_4B45_9762_DF9B4F7D8492__INCLUDED_)
#define AFX_PREFERENCESBASE_H__19D0237F_28DC_4B45_9762_DF9B4F7D8492__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "scrollingpropertypagehost.h"
#include "dialoghelper.h"
#include "winhelpbutton.h"

#include "..\Interfaces\IPreferences.h"

/////////////////////////////////////////////////////////////////////////////

const UINT WM_PPB_CTRLCHANGE = ::RegisterWindowMessage(_T("WM_PPB_CTRLCHANGE"));

/////////////////////////////////////////////////////////////////////////////

class IPreferences;
class CDeferWndMove;

/////////////////////////////////////////////////////////////////////////////
// CPreferencesPageBase dialog

class CPreferencesPageBase : public CPropertyPage, protected CDialogHelper
{
	DECLARE_DYNAMIC(CPreferencesPageBase);

	// Construction
public:
	CPreferencesPageBase(UINT nDlgTemplateID);
	CPreferencesPageBase(UINT nDlgTemplateID, UINT nHelpID);
	~CPreferencesPageBase();
	
	virtual void LoadPreferences(const IPreferences* /*pPrefs*/) {}
	virtual void SavePreferences(IPreferences* /*pPrefs*/) {}

	void SetBackgroundColor(COLORREF color);
	CWnd* GetDlgItem(UINT nID) const;
	UINT GetHelpID() const { return m_nHelpID; }

protected:
	HBRUSH m_brush;
	COLORREF m_crback;
	BOOL m_bFirstShow;
	UINT m_nHelpID;

protected:
	afx_msg void OnControlChange(UINT nID = -1);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlgBase dialog

class CPreferencesDlgBase : public CDialog, protected CDialogHelper
{
	// Construction
public:
	CPreferencesDlgBase(UINT nDlgTemplateID, 
						UINT nPPHostFrameCtrlID,
						UINT nHelpBtnIconID = 0,
						CWnd* pParent = NULL);   // standard constructor
	
	int DoModal(IPreferences* prefs, int nInitPage = -1);
	void SetPageBackgroundColor(COLORREF color);

	void LoadPreferences(const IPreferences* prefs);
	void SavePreferences(IPreferences* prefs);
	
protected:
	CScrollingPropertyPageHost m_ppHost;
	CWinHelpButton m_btnHelp;

	UINT m_nDlgTemplateID;
	UINT m_nPPHostFrameCtrlID;
	UINT m_nHelpBtnIconID;

	int m_nInitPage;
	CSize m_sizeOrgWindow;
	CSize m_sizeCurClient;

	// Temporary pointer only
	IPreferences* m_pDoModalPrefs;
	
protected:
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnHelpButton();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	DECLARE_MESSAGE_MAP()
		
protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	
	void OnApply(); // called from derived class
	
	BOOL EnsurePageCreated(int nPage) { return m_ppHost.EnsurePageCreated(nPage); }
	BOOL EnsurePageCreated(const CPreferencesPageBase* pPage) { return m_ppHost.EnsurePageCreated(pPage); }

	virtual BOOL SetActivePage(int nPage);
	virtual BOOL SetActivePage(CPreferencesPageBase* pPage);
	virtual BOOL AddPage(CPreferencesPageBase* pPage);
	virtual void ReposContents(CDeferWndMove& dwm, int nDX, int nDY);

	BOOL CreatePPHost(UINT nHostID);
	BOOL CreatePPHost(LPRECT pRect);
	void Resize(int cx = 0, int cy = 0);

	int GetActiveIndex() const { return m_ppHost.GetActiveIndex(); }
	CPreferencesPageBase* GetActivePage() { return (CPreferencesPageBase*)m_ppHost.GetActivePage(); }
	int FindPage(CPreferencesPageBase* pPage) { return m_ppHost.FindPage(pPage); }
	
	void ForwardMessage(UINT nMsg) { m_ppHost.ForwardMessage(nMsg); }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESBASE_H__19D0237F_28DC_4B45_9762_DF9B4F7D8492__INCLUDED_)
