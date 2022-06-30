#pragma once

#include "default.h"
#include "resource.h"		// main symbols

#include "LibsOther\BtnST.h"
#include "LibsOther\ThemeHelperST.h"
#include "LibsOther\XPStyleButtonST.h"

#include "Libs\PSLanguage.h"
#include "Libs\PSRegistry.h"
#include "Libs\PSToolTip.h"

#include "PSTabCtrlPage.h"
#include "afxcmn.h"
#include "afxwin.h"

// CDlgTasks dialog

class CDlgTasks : public CPSTabCtrlPage
{
	DECLARE_DYNAMIC(CDlgTasks)
public:
	CDlgTasks(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTasks();

// Dialog Data
	enum { IDD = IDD_TAB_TASKS };

	Language::Lang*	lpLang;
	Registry::Reg*	lpPrefs;

	void ResizeControls();
	void EnableButtonsByState();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CStatic		labInfo_task;
	PSToolTip	labInfo_task_ttip;

	CListCtrl listTasks;
	CXPStyleButtonST	btnNew;
	CXPStyleButtonST	btnEdit;
	CXPStyleButtonST	btnDelete;

	void SetTaskItemProgress(std::string sUID, bool bFinished);

	void SetInfo(std::string S);
	bool LoadListItems();
	void InitControls();

	void DoTaskNew();
	void DoTaskEdit(std::string ID);

	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLvnItemchangedListTasks(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnTasksNew();
	afx_msg void OnBnClickedBtnTasksEdit();
	afx_msg void OnNMDblclkListTasks(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnTasksDelete();
	afx_msg void OnLvnKeydownListTasks(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawListTasks(NMHDR *pNMHDR, LRESULT *pResult);
};
