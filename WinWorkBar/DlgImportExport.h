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

// CDlgImportExport dialog

class CDlgImportExport : public CDialog
{
	DECLARE_DYNAMIC(CDlgImportExport)

public:
	CDlgImportExport(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgImportExport();

// Dialog Data
	enum { IDD = IDD_IMPORTEXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	bool	bImport;

	virtual BOOL OnInitDialog();

	void ExportDataXml(std::string sFileName);
	void ImportDataXml(std::string sFileName);

	CComboBox listData;
	CStatic labTitle;
	CProgressCtrl cProgress;
	CButton btnOk;
	CButton btnCancel;
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnCbnSelchangeListIeData();
};
