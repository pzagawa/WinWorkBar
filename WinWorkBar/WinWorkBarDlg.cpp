// WinWorkBarDlg.cpp : implementation file
//

#include "WinWorkBar.h"
#include "WinWorkBarDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWinWorkBarDlg dialog
CWinWorkBarDlg*	lpMain = NULL;

CWinWorkBarDlg::CWinWorkBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinWorkBarDlg::IDD, pParent), ML(GetAppTextItems(), true)
{
	m_hIcon = AfxGetApp()->LoadIcon(A_IDI_MAIN);
	lpMain = this;
	HideDelay = StartupTimerHideDelay;
	LeftMargin = 4;
	Initialized = false;
}

void CWinWorkBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	DDX_Control(pDX, IDC_GRP_DATEANDTIME, grpDateAndTime);
	DDX_Control(pDX, IDC_GRP_CALENDAR, grpCalendar);
	DDX_Control(pDX, IDC_LAB_CAL, labCalendar);
	DDX_Control(pDX, IDC_LAB_CLOCK, labClock);
	DDX_Control(pDX, IDC_LAB_CAL1, labCalendarL1);
	DDX_Control(pDX, IDC_LAB_CAL2, labCalendarR1);
}


BEGIN_MESSAGE_MAP(CWinWorkBarDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_MM_EXIT, OnMmExit)
	ON_COMMAND(ID_MM_HIDE, OnMmHide)
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCHITTEST()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_COMMAND(ID_MM_DO_NOT_HIDE, OnMmDoNotHide)
	ON_COMMAND(ID_MM_NEWTASK, OnMmNewtask)
	ON_COMMAND(ID_MM_NEWNOTE, OnMmNewnote)
	ON_COMMAND(ID_MM_FIND, OnMmFind)
	ON_COMMAND(ID_MM_DELETE, OnMmDelete)
	ON_WM_WINDOWPOSCHANGED()
	ON_COMMAND(ID_MM_CATEGORIES, OnMmCategories)
	ON_COMMAND(ID_MM_ABOUT, OnMmAbout)
	ON_COMMAND(ID_MM_HELP, OnMmHelp)
	ON_COMMAND(ID_MM_HOMEPAGE, OnMmHomepage)
	ON_COMMAND(ID_MM_REGISTER, OnMmRegister)
	ON_COMMAND(ID_TRAYMENU_NEWTASK, OnTraymenuNewtask)
	ON_COMMAND(ID_TRAYMENU_NEWNOTE, OnTraymenuNewnote)
	ON_COMMAND(ID_TRAYMENU_SHOWHIDE, OnTraymenuShowhide)
	ON_COMMAND(ID_TRAYMENU_SETTINGS, OnTraymenuSettings)
	ON_COMMAND(ID_TRAYMENU_ABOUT, OnTraymenuAbout)
	ON_COMMAND(ID_TRAYMENU_CLOSE, OnTraymenuClose)
	ON_COMMAND(ID_MM_SETTINGS, OnMmSettings)
	ON_COMMAND(ID_MM_EDIT, OnMmEdit)
	ON_COMMAND(ID_MM_ONTOP, OnMmOntop)

	ON_MENUXP_MESSAGES()

	ON_WM_DRAWITEM()
	ON_COMMAND(ID_MM_VIEWINBROWSER, OnMmViewinbrowser)
	ON_COMMAND(ID_MM_SAVEASHTML, OnMmSaveashtml)
	ON_COMMAND(ID_MM_IMPORT, OnMmImport)
	ON_COMMAND(ID_MM_EXPORT, OnMmExport)
//	ON_WM_NCLBUTTONUP()
//	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

IMPLEMENT_MENUXP(CWinWorkBarDlg, CDialog);


std::string	CWinWorkBarDlg::GetAppTitle()
{
	std::string S = ProgramName;
	S += " ";
	S += Misc::GetAppVersion(true);
	return S;
}

// CWinWorkBarDlg message handlers

BOOL CWinWorkBarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	dlgToolInfo.Create(IDD_TOOLINFOWINDOW_DIALOG);

	Utils::InitCommandLine();
	Utils::RunByCommandLine(false);

	//Left window border
	//LeftMargin = GetSystemMetrics(SM_CXDLGFRAME);
	LeftMargin = 2;

	// Set the icon for this dialog.  The framework does this automatically when the application's main window is not a dialog
	HICON hSmIcon = (HICON)LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE(A_IDI_MAIN), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
  SetIcon(m_hIcon, TRUE);		// Set big icon
	SetIcon(hSmIcon, FALSE);	// Set small icon

	//Tray icon
	TrayIcon.Init(GetSafeHwnd());
	TrayIcon.SetIcon(hSmIcon, GetAppTitle());
	TrayIcon.SetMenuResource(IDR_TRAYMENU);
	TrayIcon.fnOnMenuShow = Utils::OnTrayMenuShow;
	TrayIcon.fnOnEvent = Utils::OnTrayEvent;
	
	//Init
	Utils::Init();
	Utils::UpdateDateAndTimeLabels();

	//Set calendar
	labCalendar.FnCalCellClick = FnOnCalCellClick;
	labCalendar.FnCalCellRightClick = FnOnCalCellRightClick;
	labCalendar.FnNavButtonClick = FnOnNavButtonClick;
	labCalendar.InitializeAll();
	labCalendarL1.InitializeAll();
	labCalendarL1.bSimpleStyle = true;
	labCalendarR1.InitializeAll();
	labCalendarR1.bSimpleStyle = true;

	//Set panel width
	Utils::SetWorkBarWidth(Settings.PanelWidth);
	Utils::SetWorkBarPosition();
	Utils::ResizeControls();
	Utils::ShowWorkBar();
	Utils::InitHotKeys();

	Initialized = true;

	Utils::ResizeControls();

	m_nTimer_Hide = SetTimer((UINT)WM_TIMER_HIDE, (UINT)TimerCheckDelay, NULL);
	m_nTimer_GetTime = SetTimer((UINT)WM_TIMER_GETTIME, (UINT)100, NULL);

	m_Tab.ShowPage(0);
	Utils::ProcessMessages();

	//Create edit dialogs
	dlgEditNote.Create(CDlgEdit_Note::IDD, GetDesktopWindow());
	dlgEditTask.Create(CDlgEdit_Task::IDD, GetDesktopWindow());
	dlgSearch.Create(CDlgSearch::IDD, GetDesktopWindow());
	Utils::ProcessMessages();

	labCalendar.UpdateToday();
	Utils::ProcessMessages();

	//Reminder timer 5 seconds
	m_nTimer_RemindCheck = SetTimer((UINT)WM_TIMER_REMIND_CHECK, (UINT)5000, NULL);
	Utils::ProcessMessages();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWinWorkBarDlg::StartHideTimer()
{
	m_nTimer_Hide = SetTimer((UINT)WM_TIMER_HIDE, (UINT)TimerCheckDelay, NULL);
}

void CWinWorkBarDlg::OnDestroy()
{
	CDialog::OnDestroy();

	KillTimer(m_nTimer_RemindCheck);
	KillTimer(m_nTimer_GetTime);
	KillTimer(m_nTimer_Hide);
	KillTimer(m_nTimer_HideDelay);

	Utils::UnRegisterHotKeys();

	dlgEditTask.CloseWindow();
	dlgEditNote.CloseWindow();
	dlgSearch.CloseWindow();
	dlgToolInfo.CloseWindow();

	dlgEditTask.DestroyWindow();
	dlgEditNote.DestroyWindow();
	dlgSearch.DestroyWindow();
	dlgToolInfo.DestroyWindow();
}

void CWinWorkBarDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType == ODT_MENU)
	{
		if (CMenuXP::OnDrawItem(lpDrawItemStruct, m_hWnd))
			return;
	}
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CWinWorkBarDlg::SetWindowTopmost(bool On)
{
	if (On)
	{
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	} else {
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

void CWinWorkBarDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWinWorkBarDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CWinWorkBarDlg::CursorInWindow()
{
	GetCursorPos(&m_CurrentMousePoint);
	CRect R;
	GetWindowRect(&R);
	return (R.PtInRect(m_CurrentMousePoint) == TRUE);
}

void CWinWorkBarDlg::OnTimer(UINT nIDEvent)
{
	//testing for hide
	if (nIDEvent == WM_TIMER_HIDE)
	{
		if (CursorInWindow() || Utils::IsAnyWindowVisible())
		{
			//do nothing...
		} else {
			KillTimer(m_nTimer_Hide);
			m_nTimer_HideDelay = SetTimer((UINT)WM_TIMER_HIDEDELAY, (UINT)HideDelay, NULL);
			//Change hide delay to default
			static boolean FirstRun = true;
			if (FirstRun)
			{
				FirstRun = false;
				HideDelay = Settings.HideWindowAfterSec * 1000;
			}
		}
	}

	//hide pause
	if (nIDEvent == WM_TIMER_HIDEDELAY)
	{
		if (Utils::WorkBarVisible())
		{
			KillTimer(m_nTimer_HideDelay);
			if (CursorInWindow())
			{
				StartHideTimer();
			} else {
				Utils::HideWorkBar(false);
			}
		}
	}

	if (nIDEvent == WM_TIMER_GETTIME)
	{
		Utils::UpdateDateAndTimeLabels();
	}

	if (nIDEvent == WM_TIMER_REMIND_CHECK)
	{
		dlgToday.TaskReminder.iMinutesBeforeRemind = Settings.MinutesBeforeRemind;
		std::string sTaskUID = dlgToday.TaskReminder.GetTaskUIDToView();
		if ((!sTaskUID.empty()) && (dlgToolInfo.sUID.empty()))
				dlgToolInfo.ShowInfoWindow(sTaskUID);
	}

	CDialog::OnTimer(nIDEvent);
}

LRESULT CWinWorkBarDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	if (wParam == hktShowWorkBar)
	{
		SwitchShowHideWorkBar();
	}
	if (wParam == hktNewTask)
	{
		OnMmNewtask();
	}
	if (wParam == hktNewNote)
	{
		OnMmNewnote();
	}
	return 0;
}

void CWinWorkBarDlg::SwitchShowHideWorkBar()
{
	if (Utils::WorkBarVisible())
	{
		Utils::HideWorkBar(true);
	} else {
		Utils::ShowWorkBar();
		StartHideTimer();
	}
}

void CWinWorkBarDlg::ShowBarNow()
{
	if (!Utils::WorkBarVisible())
		Utils::ShowWorkBar();
	KillTimer(m_nTimer_HideDelay);
	StartHideTimer();
}

void CWinWorkBarDlg::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (!Settings.bShowBarOnClick)
		ShowBarNow();
	CDialog::OnNcMouseMove(nHitTest, point);
}

void CWinWorkBarDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (Settings.bShowBarOnClick)
		ShowBarNow();
	CDialog::OnLButtonUp(nFlags, point);
}

void CWinWorkBarDlg::OnOK()
{

}

void CWinWorkBarDlg::OnCancel()
{

}

void CWinWorkBarDlg::OnClose()
{
	Utils::HideWorkBar(true);
	CDialog::OnClose();
}

void CWinWorkBarDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//Utils::HideWorkBar();
	CDialog::OnLButtonDown(nFlags, point);
}

void CWinWorkBarDlg::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDialog::OnSettingChange(uFlags, lpszSection);
	if	(
				(uFlags == SPI_SETWORKAREA) ||	//change taskbar pos
				(uFlags == 0)										//change resolution
			)	
	{
		Utils::HideWorkBar(true);
		Utils::SetWorkBarPosition();
	}
}

LRESULT CWinWorkBarDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DISPLAYCHANGE)
	{
		Utils::HideWorkBar(true);
		Utils::SetWorkBarPosition();
	}

	TrayIcon.DoWindowProc(message, wParam, lParam);

	return CDialog::WindowProc(message, wParam, lParam);
}

LRESULT CWinWorkBarDlg::OnNcHitTest(CPoint point)
{
	//Lock sizing by selected borders. DISABLE DIALOG MOVING !
	CRect WindowR;
	CRect ClientR;

	this->GetWindowRect(&WindowR);
	this->GetClientRect(&ClientR);

	//Get rect of title bar close button
	CRect CloseButton = WindowR;

	int BorderX = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXBORDER);
	int BorderY = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYBORDER);
	int SmallButtonMargin = (GetSystemMetrics(SM_CYSMCAPTION) >> 1) - (GetSystemMetrics(SM_CYSMSIZE) >> 1);
 
	CloseButton.top += BorderY + SmallButtonMargin;
	CloseButton.bottom = CloseButton.top + GetSystemMetrics(SM_CYSMSIZE);
	CloseButton.right -= BorderX - SmallButtonMargin;
	CloseButton.left = CloseButton.right - GetSystemMetrics(SM_CXSMSIZE);// + BorderX;

	//Set height of testing rectangle to border
	WindowR.bottom = WindowR.bottom - GetSystemMetrics(SM_CXSIZEFRAME);
	WindowR.top = WindowR.bottom - ClientR.Height();

	//Set width to left or right border
	switch(Utils::GetWinTaskBarEdge())
	{
		case ABE_LEFT:
		case ABE_TOP:
		case ABE_BOTTOM:
			WindowR.right = WindowR.left + GetSystemMetrics(SM_CXSIZEFRAME);	//sizing left
			break;
		case ABE_RIGHT:
			WindowR.left = WindowR.right - GetSystemMetrics(SM_CXSIZEFRAME);	//sizing right
			break;
	}

	//Add menu height to client area
	ClientR.top -= GetSystemMetrics(SM_CYMENU);

	//Prevent lock client area
	this->ClientToScreen(&ClientR);
	if (ClientR.PtInRect(point))
		return CDialog::OnNcHitTest(point);

	//Prevent lock caption close button area	
	if (CloseButton.PtInRect(point))
		return CDialog::OnNcHitTest(point);

	if (Settings.bShowBarOnClick)
		if (!Utils::WorkBarVisible())
			return HTCLIENT;

	//Test cursor pos
	if (!WindowR.PtInRect(point))
	{
		BringWindowToTop();
		return FALSE;
	}
	return CDialog::OnNcHitTest(point);
}

//Resizing
//-------------------------------------------------
void CWinWorkBarDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (Initialized)
	{
		//Set new panel width
		CRect R;
		this->GetWindowRect(&R);
		Utils::SetWorkBarWidth(R.Width());
		Utils::ResizeControls();
	}
}

void CWinWorkBarDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	//Lock width to max & min
	CRect R = *pRect;
	switch(Utils::GetWinTaskBarEdge())
	{
		case ABE_LEFT:
		case ABE_TOP:
		case ABE_BOTTOM:
			if (R.Width() > MaxWidth)
				pRect->left = pRect->right - MaxWidth;
			if (R.Width() < DefaultWidth)
				pRect->left = pRect->right - DefaultWidth;
			break;
		case ABE_RIGHT:
			if (R.Width() > MaxWidth)
				pRect->right = pRect->left + MaxWidth;
			if (R.Width() < DefaultWidth)
				pRect->right = pRect->left + DefaultWidth;
			break;
	}
	Utils::ResizeControlsIfNeeded(pRect->right - pRect->right, pRect->bottom - pRect->top);
}

void CWinWorkBarDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanged(lpwndpos);
	Utils::ResizeControlsIfNeeded(lpwndpos->cx, lpwndpos->cy);
}


//TRAYICON MENU EVENTS !
//-------------------------------------------------

void CWinWorkBarDlg::OnTraymenuNewtask()
{
	OnMmNewtask();
}

void CWinWorkBarDlg::OnTraymenuNewnote()
{
	OnMmNewnote();
}

void CWinWorkBarDlg::OnTraymenuShowhide()
{
	SwitchShowHideWorkBar();
}

void CWinWorkBarDlg::OnTraymenuSettings()
{
	OnMmSettings();
}

void CWinWorkBarDlg::OnTraymenuAbout()
{
	OnMmAbout();
}

void CWinWorkBarDlg::OnTraymenuClose()
{
	OnMmExit();
}


//CALENDAR EVENTS !
//-------------------------------------------------
void FnOnCalCellClick(PSCalendarCell& CalCell)
{
	Utils::DaySelected(CalCell.GetSysTime());
}

void FnOnCalCellRightClick(PSCalendarCell& CalCell)
{
	dlgData_task DT;
	DT.bNewTask = true;
	lpMain->dlgEditTask.ShowDialog(true, DT, &CalCell.GetSysTime());
}

void FnOnNavButtonClick()
{
	Utils::SetOtherCalendars();
}


//MENU EVENTS !
//-------------------------------------------------
void CWinWorkBarDlg::OnMmHide()
{
	Utils::HideWorkBar(true);
}

void CWinWorkBarDlg::OnMmDoNotHide()
{
	Settings.PinWindow = !Settings.PinWindow;
	Utils::UpdateMenuItemsState();
	Settings.Write();
}

void CWinWorkBarDlg::OnMmOntop()
{
	Settings.WindowTopmost = !Settings.WindowTopmost;
	Utils::UpdateMenuItemsState();
	Settings.Write();
	SetWindowTopmost(Settings.WindowTopmost);
	dlgToolInfo.BringInfoWindowToFront();
}

void CWinWorkBarDlg::OnMmAbout()
{
	AboutDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
}

void CWinWorkBarDlg::OnMmHelp()
{
	AfxGetApp()->HtmlHelp(NULL, HH_DISPLAY_TOPIC) ;
}

void CWinWorkBarDlg::OnMmHomepage()
{
	ShellExecute(NULL, "open", Author::www.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void CWinWorkBarDlg::OnMmRegister()
{
}

void CWinWorkBarDlg::OnMmExit()
{
	Settings.PanelWidth = Utils::GetWorkBarWidth();

	Settings.Write();
	Utils::HideWorkBar(true);

	CDialog::OnOK();
}

void CWinWorkBarDlg::OnMmNewtask()
{
	dlgData_task DT;
	DT.bNewTask = true;
	dlgEditTask.ShowDialog(true, DT);
}

void CWinWorkBarDlg::OnMmNewnote()
{
	dlgData_note DN;
	DN.bNewNote = true;
	dlgEditNote.ShowDialog(true, DN);
}

void CWinWorkBarDlg::OnMmEdit()
{
	if (dlgNotes.IsWindowVisible())
		dlgNotes.OnBnClickedBtnNotesEdit();
	if (dlgTasks.IsWindowVisible())
		dlgTasks.OnBnClickedBtnTasksEdit();
}

void CWinWorkBarDlg::OnMmDelete()
{
	if (dlgNotes.IsWindowVisible())
		dlgNotes.OnBnClickedBtnNotesDelete();
	if (dlgTasks.IsWindowVisible())
		dlgTasks.OnBnClickedBtnTasksDelete();
}

void CWinWorkBarDlg::OnMmCategories()
{
	dlgEditCategories.DoModal();
}

void CWinWorkBarDlg::OnMmSettings()
{
	if (!dlgSettings.IsWindowVisible())
		dlgSettings.DoModal();
}

void CWinWorkBarDlg::OnMmFind()
{
	dlgSearch.ShowDialog(true);
}

BOOL CWinWorkBarDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	Utils::GetViewMenuState_OnCommand(wParam, lParam);
	return CDialog::OnCommand(wParam, lParam);
}

void CWinWorkBarDlg::OnMmViewinbrowser()
{
	std::string sFileName = File::IncludeBackSlash(Misc::GetFolder("temp")) + "WinWorkBar_ExportedData.htm";
	Utils::ExportData(sFileName);
	if (File::FileExists(sFileName))
		Misc::ExecuteFile(sFileName);
}

void CWinWorkBarDlg::OnMmSaveashtml()
{
	const char szFilterTemplate[] = "%s (*.htm)|*.htm||";
	char szFileFilter[128];
	wsprintf(szFileFilter, szFilterTemplate, ML.Text(149));

	CFileDialog	FileDialog(false, "htm", NULL, OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, szFileFilter);
	std::string InitFolder = Misc::GetFolder("desktop");
	FileDialog.GetOFN().lpstrInitialDir = InitFolder.c_str();
	TCHAR sIniFileName[128] = {"WinWorkBar data"};
	FileDialog.GetOFN().lpstrFile = sIniFileName;
	if (FileDialog.DoModal() == IDOK)
	{
		std::string sFileName = FileDialog.GetPathName();
		Utils::ExportData(sFileName);
	}
}

void CWinWorkBarDlg::OnMmImport()
{
	CDlgImportExport Dlg;
	Dlg.bImport = true;
	Dlg.DoModal();
}

void CWinWorkBarDlg::OnMmExport()
{
	CDlgImportExport Dlg;
	Dlg.bImport = false;
	Dlg.DoModal();
}

