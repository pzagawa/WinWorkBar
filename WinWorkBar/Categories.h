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

// CCategories dialog

class CCategories : public CDialog
{
	DECLARE_DYNAMIC(CCategories)

	DWORD	GetCategories();
	void	SetCategories(DWORD C);
public:
	CCategories(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCategories();

// Dialog Data
	enum { IDD = IDD_CATEGORIES };

	Language::Lang*	lpLang;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void OnOK();
public:
	DWORD	dwCategories;

	CListCtrl listCategories;
	CStatic grp1;
	CEdit labCategories;
	CButton btnOK;
	CButton btnEditCategories;
	CButton btnSelectAll;
	CButton btnDeselectAll;
	CButton btnCancel;

	bool	LoadListItems();

	std::string CategoriesAsString();
	std::string	CategoriesAsString(DWORD C);

	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedBtnEdit();
	afx_msg void OnBnClickedBtnDeselectall();
	afx_msg void OnLvnItemchangedListCategory(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSelectall();
};
