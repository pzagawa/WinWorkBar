#pragma once

#include "default.h"
#include "resource.h"		// main symbols

#include <string>

#include "LibsOther\ThemeHelperST.h"

// CPSTabCtrlPage dialog

class CPSTabCtrlPage : public CDialog
{
public:
	CPSTabCtrlPage(CWnd* pParent = NULL);   // standard constructor
	CPSTabCtrlPage(UINT nIDTemplate, CWnd* pParent = NULL);

	virtual ~CPSTabCtrlPage();

// Dialog Data
	enum { IDD = 0 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CThemeHelperST*	lpThemeHelper;

	BOOL DrawDefaultBackground(CDC* pDC, bool DefaultDrawing, bool Border);
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
