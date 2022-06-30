#pragma once

#include "default.h"
#include "resource.h"		// main symbols

#include "Libs\PSLanguage.h"
#include "Libs\PSRegistry.h"
#include "Libs\PSTinyXML.h"

#include "afxwin.h"

// CEditCategories dialog

class CEditCategories : public CDialog
{
	DECLARE_DYNAMIC(CEditCategories)

	bool	bUpdateMode;
	void	SetButtonText();
public:
	CEditCategories(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditCategories();

// Dialog Data
	enum { IDD = IDD_EDIT_CATEGORIES };

	Language::Lang*	lpLang;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void OnOK();
public:
	CEdit		edCategoryName;
	CButton btnAdd;
	CButton btnOK;
	CButton btnCancel;
	CButton btnDefault;
	CListCtrl listCategories;

	virtual BOOL OnInitDialog();

	bool	LoadListItems();
	bool	SaveListItems();

	afx_msg void OnLvnItemchangedListCategory(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDefault();
};
