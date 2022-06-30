// TestSetupAndTerminateDlg.cpp : implementation file
//

#include "CMessageOnlyWindow.h"
#include "WinWorkBarDlg.h"

extern CWinWorkBarDlg*	lpMain;

// CMessageOnlyWindow dialog

CMessageOnlyWindow::CMessageOnlyWindow(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageOnlyWindow::IDD, pParent)
{
}

void CMessageOnlyWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMessageOnlyWindow, CDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CMessageOnlyWindow message handlers

BOOL CMessageOnlyWindow::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

const UINT WM_WINWORKBAR_TERMINATE = WM_USER + 4444;

LRESULT CMessageOnlyWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_WINWORKBAR_TERMINATE)
	{
		if (lpMain)
			lpMain->OnMmExit();
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
