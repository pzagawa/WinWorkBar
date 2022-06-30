#pragma once

#include "default.h"
#include "resource.h"		// main symbols

#include "Libs\PSLanguage.h"
#include "Libs\PSRegistry.h"
#include "Libs\PSTaskView.h"

#include "PSTabCtrlPage.h"
#include "afxwin.h"

#include "taskreminder.h"

void fnOnTaskViewClickItem(PSTaskView& TaskView, CPoint& P, PSTaskViewItem& Item);

// CDlgToday dialog

class CDlgToday : public CPSTabCtrlPage
{
	DECLARE_DYNAMIC(CDlgToday)
public:
	CDlgToday(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgToday();

// Dialog Data
	enum { IDD = IDD_TAB_TODAY };

	Language::Lang*	lpLang;
	Registry::Reg*	lpPrefs;
protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
public:
	PSTaskView	labTaskView;

	PSTaskReminder	TaskReminder;

	bool	LoadListItems();

	void ResizeControls();

	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
