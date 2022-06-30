#pragma once

#include "default.h"
#include "resource.h"		// main symbols

#include "LibsOther\BtnST.h"
#include "LibsOther\ThemeHelperST.h"
#include "LibsOther\XPStyleButtonST.h"

#include "Libs\PSLanguage.h"
#include "Libs\PSRegistry.h"
#include "Libs\PSToolTip.h"

#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"

// CDlgSearch dialog

class CDlgSearch : public CDialog
{
	DECLARE_DYNAMIC(CDlgSearch)

	HICON		m_hIcon;
	HACCEL	m_hAccel;

	void ResizeControls();
	bool	LoadListItems();
	bool AcceptRow_byWords(std::string& S);
	void LoadListItemsByTable(PSXML::PSTinyXML& TB, int& SummaryCount, int Type);
public:
	CDlgSearch(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSearch();

// Dialog Data
	enum { IDD = IDD_SEARCH };

protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic labWithWords;
	CStatic labWithoutWords;
	CEdit edWithWords;
	CEdit edWithoutWords;
	CListCtrl listFinds;
	CButton btnOk;
	CButton btnCancel;
	CStatic labStatus;

	void ShowDialog(bool Show);
	void ShowItem();

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnPaint();
	afx_msg void OnNMDblclkFindlist(NMHDR *pNMHDR, LRESULT *pResult);
};
