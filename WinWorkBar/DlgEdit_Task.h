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

#include "Repeats.h"

// CDlgEdit_Task dialog

class CDlgEdit_Task : public CDialog
{
	DECLARE_DYNAMIC(CDlgEdit_Task)

	HICON		m_hIcon;
	HACCEL	m_hAccel;
	CWnd*		pEarlyActiveWindow;
	bool		bDataChanged;
	dlgData_task	DataTask;

	PSToolTip	labCategories_tip;
	PSToolTip	labRepeats_tip;

	void ResizeControls();
	int SaveData(bool AskIfChanged);

	void UpdateRepeatsByDateRange();
public:
	CDlgEdit_Task(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEdit_Task();

// Dialog Data
	enum { IDD = IDD_EDITDLG_TASK };

protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//Controls
	CRichEditCtrl redText;
	CEdit			labCategories;
	CXPStyleButtonST btnDelete;
	CXPStyleButtonST	btnOK;
	CXPStyleButtonST	btnCancel;
	CXPStyleButtonST	btnCategories;	
	CDateTimeCtrl edDateStart;
	CDateTimeCtrl edTimeStart;
	CDateTimeCtrl edDateStop;
	CStatic labStart;
	CButton chkStop;

	CStatic labPriority;
	CComboBox listPriority;
	CStatic labProgress;
	CSliderCtrl trackProgress;
	CButton labProgressInfo;
	CXPStyleButtonST btnRepeats;
	CEdit labRepeats;

	void UpdateProgressLabel();

	void ShowDialog(bool Show, dlgData_task& DT, SYSTEMTIME* NewTime = NULL);
	const	dlgData_task& GetDataTask() { return DataTask; };

	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnLinkRedit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnEnChangeEditTaskText();
	afx_msg void OnBnClickedEditTaskBtnCategories();
	afx_msg void OnAccSave();
	afx_msg void OnEnMsgfilterEditTaskText(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChkStop();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnDeleteTask();
	afx_msg void OnBnClickedEditTaskBtnRepeats();
	afx_msg void OnBnClickedLabPrgInfo();
};

