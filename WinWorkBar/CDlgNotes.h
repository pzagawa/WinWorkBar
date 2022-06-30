#pragma once

#include "default.h"
#include "resource.h"		// main symbols

#include "LibsOther\BtnST.h"
#include "LibsOther\ThemeHelperST.h"
#include "LibsOther\XPStyleButtonST.h"

#include "Libs\PSMFCUtils.h"
#include "Libs\PSUtil.h"
#include "Libs\PSTinyXML.h"
#include "Libs\PSLanguage.h"
#include "Libs\PSRegistry.h"
#include "Libs\PSToolTip.h"

#include "PSTabCtrlPage.h"
#include "afxcmn.h"
#include "afxwin.h"

// CDlgNotes dialog

class CDlgNotes : public CPSTabCtrlPage
{
	DECLARE_DYNAMIC(CDlgNotes)
public:
	CDlgNotes(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNotes();

// Dialog Data
	enum { IDD = IDD_TAB_NOTES };

	Language::Lang*	lpLang;
	Registry::Reg*	lpPrefs;

	void ResizeControls();
	void EnableButtonsByState();
protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
public:
	CStatic		labInfo_note;
	PSToolTip	labInfo_note_ttip;

	CListCtrl listNotes;
	CXPStyleButtonST	btnNew;
	CXPStyleButtonST	btnEdit;
	CXPStyleButtonST	btnDelete;

	bool	LoadListItems();
	void InitControls();
	void SetInfo(std::string S);

	void DoNoteNew();
	void DoNoteEdit(std::string ID);

	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnItemchangedListNotes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnNotesEdit();
	afx_msg void OnBnClickedBtnNotesNew();
	afx_msg void OnNMDblclkListNotes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnNotesDelete();
	afx_msg void OnLvnKeydownListNotes(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
