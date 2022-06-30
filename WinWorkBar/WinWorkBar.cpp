// WinWorkBar.cpp : Defines the class behaviors for the application.
//

#include "WinWorkBar.h"
#include "WinWorkBarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinWorkBarApp

BEGIN_MESSAGE_MAP(CWinWorkBarApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWinWorkBarApp construction

CWinWorkBarApp::CWinWorkBarApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	EnableHtmlHelp();
}


// The one and only CWinWorkBarApp object

CWinWorkBarApp theApp;

// CWinWorkBarApp initialization

BOOL CWinWorkBarApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	AfxInitRichEdit2();

	CWinWorkBarDlg dlg;
	m_pMainWnd = &dlg;

	//One instance test !
	Util::PSOneInstance ThereCanBeOnlyOne(dlg.GetSafeHwnd(), "{86787C05-3CA9-4e24-8D7A-311BC264D314}");
	if (ThereCanBeOnlyOne.AlreadyRunning())
		return FALSE;

	CMessageOnlyWindow	MessageWindow;
	MessageWindow.Create(IDD_MESSAGE_ONLY_WINDOW, NULL);

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	MessageWindow.DestroyWindow();

	return FALSE;
}

BOOL CWinWorkBarApp::PreTranslateMessage(MSG* pMsg)
{
	// Magical mystical MFC F1 Help msg!
	if(pMsg->message == 0x4d) 
	{
		AfxGetApp()->HtmlHelp(NULL, HH_DISPLAY_TOPIC) ;
		return(TRUE); // Eat it
	}

	return CWinApp::PreTranslateMessage(pMsg);
}
