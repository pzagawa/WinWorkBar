// TestSetupAndTerminateDlg.h : header file
//

#pragma once

#include "default.h"
#include "resource.h"		// main symbols

// CMessageOnlyWindow dialog
class CMessageOnlyWindow : public CDialog
{
// Construction
public:
	CMessageOnlyWindow(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MESSAGE_ONLY_WINDOW };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
