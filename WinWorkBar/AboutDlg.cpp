// AboutDlg.cpp : implementation file
//

#include "Default.h"
#include "WinWorkBarDlg.h"

#include "AboutDlg.h"
#include "Libs\PSAuthor.h"

extern CWinWorkBarDlg*	lpMain;

// AboutDlg dialog

IMPLEMENT_DYNAMIC(AboutDlg, CDialog)
AboutDlg::AboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AboutDlg::IDD, pParent)
{
	lpLang = &lpMain->ML;
	lpPrefs = &lpMain->Prefs;

	m_AppName = lpMain->GetAppTitle();
	m_Copyright1 = Author::Line1;
	m_Copyright2 = Author::Line2;
}

AboutDlg::~AboutDlg()
{
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAB_APPNAME, m_labAppName);
	DDX_Control(pDX, IDC_LAB_COPYRIGHT1, m_labCopyright1);
	DDX_Control(pDX, IDC_LAB_COPYRIGHT2, m_labCopyright2);
	DDX_Control(pDX, IDC_LAB_WWW, labWww);
}

BEGIN_MESSAGE_MAP(AboutDlg, CDialog)
END_MESSAGE_MAP()


// AboutDlg message handlers
BOOL AboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	SetWindowText(lpLang->Text(8));
	
	CString AppString = m_AppName.c_str();
	AppString += " (";
	AppString += Misc::GetAppVersion(false).c_str();
	AppString += ")";

	m_labAppName.SetWindowText(AppString);

	m_labCopyright1.SetWindowText(m_Copyright1.c_str());
	m_labCopyright2.SetWindowText(m_Copyright2.c_str());

	//set www link
	CString S = Author::www.c_str();
	labWww.SetText(S);
	labWww.SetTransparent(TRUE);
	labWww.SetTextColor(RGB(0,0,255));
	labWww.SetLinkCursor(LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_PS_HAND)));
	labWww.SetHyperLink(Author::www.c_str());
	labWww.SetLink(TRUE, FALSE);

	CenterWindow(CWnd::GetDesktopWindow());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void AboutDlg::OnOK()
{
	CDialog::OnOK();
}

