#pragma once

#include "default.h"
#include "resource.h"		// main symbols

#include "Libs\PSLanguage.h"
#include "Libs\PSRegistry.h"
#include "Libs\PSMFCUtils.h"
#include "Libs\PSUtil.h"
#include "Libs\PSTinyXML.h"

#include "afxwin.h"
#include "afxcmn.h"

// CRepeats dialog

class CRepeats : public CDialog
{
	DECLARE_DYNAMIC(CRepeats)

	void InitControls();
	void EnableControls(int Type);
	void SaveData();
public:
	CRepeats(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRepeats();

// Dialog Data
	enum { IDD = IDD_REPEATS };

	Language::Lang*	lpLang;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void OnOK();
	virtual void OnCancel();
public:
	DWORD	dwRepeats;

	CButton rdOff;
	CButton rdDays;
	CButton rdMonths;
	CButton rdYears;
	CButton rdWeekDays;

	CStatic grpType;
	CStatic grpWeekDay;
	CListCtrl listWeekDays;
	CButton btnOK;
	CButton btnCancel;

	std::string RepeatsAsString(DWORD dwData);

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRdDays();
	afx_msg void OnBnClickedRdMonths();
	afx_msg void OnBnClickedRdOff();
	afx_msg void OnBnClickedRdWeekdays();
	afx_msg void OnBnClickedRdYears();
};
