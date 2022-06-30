#pragma once

#include "LibsOther/CLabel.h"

#include "default.h"
#include "resource.h"		// main symbols

#include "Libs\PSLanguage.h"
#include "Libs\PSRegistry.h"
#include "Libs\PSTaskView.h"

#include "PSTabCtrlPage.h"

#include "afxwin.h"

// ConfigurationDlg dialog

class AboutDlg : public CDialog
{
	DECLARE_DYNAMIC(AboutDlg)
public:
	AboutDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTDLG };

	Language::Lang*	lpLang;
	Registry::Reg*	lpPrefs;

	std::string			m_AppName;
	std::string			m_Copyright1;
	std::string			m_Copyright2;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CStatic m_labAppName;
	CStatic m_labCopyright1;
	CStatic m_labCopyright2;
	CLabel	labWww;
};
