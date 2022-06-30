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

// CDlgEdit_Note dialog

class CDlgEdit_Note : public CDialog
{
	DECLARE_DYNAMIC(CDlgEdit_Note)

	HICON		m_hIcon;
	HACCEL	m_hAccel;
	CWnd*		pEarlyActiveWindow;
	bool		bDataChanged;
	dlgData_note	DataNote;

	PSToolTip	labCategories_tip;

	void ResizeControls();
	int SaveData(bool AskIfChanged);
public:
	CDlgEdit_Note(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEdit_Note();

// Dialog Data
	enum { IDD = IDD_EDITDLG_NOTE };

protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//Controls
	CRichEditCtrl redText;
	CEdit			labCategories;
	CXPStyleButtonST	btnDelete;
	CXPStyleButtonST	btnOK;
	CXPStyleButtonST	btnCancel;
	CXPStyleButtonST	btnCategories;
	CDateTimeCtrl edDateFrom;
	CDateTimeCtrl edDateTo;
	CButton chkFrom;
	CButton chkTo;

	void ShowDialog(bool Show, dlgData_note& DN);
	const	dlgData_note& GetDataNote() { return DataNote; };

	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnLinkRedit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnEnChangeEditNoteText();
	afx_msg void OnBnClickedEditNoteBtnCategories();
	afx_msg void OnAccSave();
	afx_msg void OnEnMsgfilterEditNoteText(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChkfrom();
	afx_msg void OnBnClickedChkto();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnDelete();
};
