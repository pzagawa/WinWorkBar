// DlgEdit_Task.cpp : implementation file
//

#include "DlgEdit_Task.h"
#include "WinWorkBarDlg.h"

// CDlgEdit_Task dialog

extern CWinWorkBarDlg*	lpMain;

IMPLEMENT_DYNAMIC(CDlgEdit_Task, CDialog)
CDlgEdit_Task::CDlgEdit_Task(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEdit_Task::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(A_IDI_MAIN);
  m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_ACCTABLE));
	
	pEarlyActiveWindow = NULL;

	DataTask.bNewTask = true;
	DataTask.dwCategories = 0;
	DataTask.dwRepeats = 0;
	DataTask.sID.clear();
	DataTask.sText.clear();
}

CDlgEdit_Task::~CDlgEdit_Task()
{
}

void CDlgEdit_Task::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REDIT, redText);
	DDX_Control(pDX, IDC_EDIT_TASK_BTN_CATEGORIES, btnCategories);
	DDX_Control(pDX, IDC_EDIT_TASK_LAB_CATEGORIES, labCategories);
	DDX_Control(pDX, IDOK, btnOK);
	DDX_Control(pDX, IDCANCEL, btnCancel);
	DDX_Control(pDX, IDC_DATE_START, edDateStart);
	DDX_Control(pDX, IDC_TIME_START, edTimeStart);
	DDX_Control(pDX, IDC_DATE_STOP, edDateStop);
	DDX_Control(pDX, IDC_LAB_START, labStart);
	DDX_Control(pDX, IDC_CHK_STOP, chkStop);
	DDX_Control(pDX, IDC_LAB_PRI, labPriority);
	DDX_Control(pDX, IDC_LIST_PRI, listPriority);
	DDX_Control(pDX, IDC_LAB_PROGRESS, labProgress);
	DDX_Control(pDX, IDC_SLD_PROGRESS, trackProgress);
	DDX_Control(pDX, IDC_LAB_PRG_INFO, labProgressInfo);
	DDX_Control(pDX, IDC_EDIT_TASK_BTN_REPEATS, btnRepeats);
	DDX_Control(pDX, IDC_EDIT_TASK_LAB_REPEATS, labRepeats);
	DDX_Control(pDX, IDC_BTN_DELETE_TASK, btnDelete);
}


BEGIN_MESSAGE_MAP(CDlgEdit_Task, CDialog)
	ON_NOTIFY(EN_LINK, IDC_REDIT, OnEnLinkRedit)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_EN_CHANGE(IDC_REDIT, OnEnChangeEditTaskText)
	ON_BN_CLICKED(IDC_EDIT_TASK_BTN_CATEGORIES, OnBnClickedEditTaskBtnCategories)
	ON_COMMAND(ID_ACC_SAVE, OnAccSave)
	ON_NOTIFY(EN_MSGFILTER, IDC_REDIT, OnEnMsgfilterEditTaskText)
	ON_BN_CLICKED(IDC_CHK_STOP, OnBnClickedChkStop)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_DELETE_TASK, OnBnClickedBtnDeleteTask)
	ON_BN_CLICKED(IDC_EDIT_TASK_BTN_REPEATS, OnBnClickedEditTaskBtnRepeats)
	ON_BN_CLICKED(IDC_LAB_PRG_INFO, OnBnClickedLabPrgInfo)
END_MESSAGE_MAP()


// CDlgEdit_Task message handlers

BOOL CDlgEdit_Task::OnInitDialog()
{
	CDialog::OnInitDialog();
	Utils::SetupRichEditForNote(redText);

	//Setup controls
	SetIcon(m_hIcon, TRUE);
	btnCategories.SetThemeHelper(&lpMain->m_ThemeHelper);
	btnDelete.SetThemeHelper(&lpMain->m_ThemeHelper);
	btnOK.SetThemeHelper(&lpMain->m_ThemeHelper);
	btnCancel.SetThemeHelper(&lpMain->m_ThemeHelper);
	btnCategories.SetAlign(CButtonST::ST_ALIGN_VERT);
	btnCategories.SetIcon(IDI_ICO_CATEGORIES);

	btnRepeats.SetThemeHelper(&lpMain->m_ThemeHelper);
	btnRepeats.SetAlign(CButtonST::ST_ALIGN_VERT);
	btnRepeats.SetIcon(IDI_ICO_REPEATS);

	labCategories_tip.Set(&labCategories);
	labRepeats_tip.Set(&labRepeats);

	edTimeStart.SetFormat((lpMain->Settings.Time24Hour)?"HH:mm":"hh:mm tt");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEdit_Task::UpdateProgressLabel()
{
	std::string S = Text::IntToStr(trackProgress.GetPos() * 10) + " %";
	if (trackProgress.GetPos() == 0)
		S = lpMain->ML.Text(103);
	if (trackProgress.GetPos() == 10)
		S = lpMain->ML.Text(105);
	labProgressInfo.SetWindowText(S.c_str());
	labProgressInfo.SetCheck((trackProgress.GetPos() == 10)?BST_CHECKED:BST_UNCHECKED);
}

void CDlgEdit_Task::ShowDialog(bool Show, dlgData_task& DT, SYSTEMTIME* NewTime)
{
	//Close util windows if opened
	if (lpMain->dlgCategories.GetSafeHwnd() && lpMain->dlgCategories.IsWindowVisible())
		lpMain->dlgCategories.EndDialog(IDCANCEL);
	if (lpMain->dlgRepeats.GetSafeHwnd() && lpMain->dlgRepeats.IsWindowVisible())
		lpMain->dlgRepeats.EndDialog(IDCANCEL);

	lpMain->dlgToday.labTaskView.SelectedTaskID = (Show)?DT.sID:"";
	lpMain->dlgToday.labTaskView.RedrawWindow();

	if (Show)
	{
		//Check save
		if (IsWindowVisible())
		{
			if (DT.sID == DataTask.sID)
				return;
			if (SaveData(true) == IDCANCEL)
				return;
		}

		//Init controls before show
		DataTask = DT;
		std::string DlgTitle = ProgramName + " - " + lpMain->ML.Text((DataTask.bNewTask)?14:80);
		SetWindowText(DlgTitle.c_str());

		btnCategories.SetTooltipText(lpMain->ML.Text(25));
		labStart.SetWindowText(lpMain->ML.Text(117));
		chkStop.SetWindowText(lpMain->ML.Text(93));
		btnOK.SetWindowText(lpMain->ML.Text(27));
		btnOK.SetTooltipText("Ctrl + S");
		btnCancel.SetWindowText(lpMain->ML.Text(28));
		btnCancel.SetTooltipText("Esc");
		btnDelete.SetWindowText(lpMain->ML.Text(73));

		//Set btnDelete state
		btnDelete.EnableWindow(!DataTask.bNewTask);

		labPriority.SetWindowText(lpMain->ML.Text(118));

		listPriority.ResetContent();
		listPriority.InsertString(0, lpMain->ML.Text(108));
		listPriority.InsertString(1, lpMain->ML.Text(107));
		listPriority.InsertString(2, lpMain->ML.Text(106));

		labProgress.SetWindowText(lpMain->ML.Text(119));
		trackProgress.SetRange(0, 10);
		btnRepeats.SetTooltipText(lpMain->ML.Text(120));

		//Set RichEdit text
		redText.SetSel(0, -1);
		redText.Clear();
		if (DataTask.bNewTask)
		{
			DataTask.sText.clear();
		} else {
			redText.SetWindowText(DataTask.sText.c_str());
		}
		
		//Set categories label by Categories DWORD
		std::string sCategories = lpMain->dlgCategories.CategoriesAsString(DataTask.dwCategories);
		labCategories.SetWindowText(sCategories.c_str());
		labCategories_tip.Update(sCategories.c_str());

		//Set date range
		chkStop.SetCheck((DataTask.DateStopEmpty())?BST_UNCHECKED:BST_CHECKED);
		OnBnClickedChkStop();

		SYSTEMTIME ST;
		if (NewTime)
		{
			ST = *NewTime;
		} else {
			GetLocalTime(&ST);
			ST.wDay = lpMain->labCalendar.GetSelectedDay().wDay;
			ST.wDayOfWeek = lpMain->labCalendar.GetSelectedDay().wDayOfWeek;
			ST.wMonth = lpMain->labCalendar.GetSelectedDay().wMonth;
			ST.wYear = lpMain->labCalendar.GetSelectedDay().wYear;
		}

		edDateStart.SetTime(&ST);
		edTimeStart.SetTime(&ST);
		edDateStop.SetTime(&ST);
		if (!DataTask.DateStartEmpty())
		{
			edDateStart.SetTime(&DataTask.DateStart);
			edTimeStart.SetTime(&DataTask.DateStart);
		}
		if (!DataTask.DateStopEmpty())
			edDateStop.SetTime(&DataTask.DateStop);

		listPriority.SetCurSel(DataTask.iPriority + 1);
		trackProgress.SetPos(DataTask.iProgress / 10);
		UpdateProgressLabel();

		std::string sRepeats = lpMain->dlgRepeats.RepeatsAsString(DataTask.dwRepeats);
		labRepeats.SetWindowText(sRepeats.c_str());
		labRepeats_tip.Update(sRepeats.c_str());

		//Show window
		pEarlyActiveWindow = GetActiveWindow();
		ResizeControls();
		Misc::LoadWindowSizeAndPos(*this, lpMain->Prefs, "DlgEdit_task", SW_SHOW, MinDialogSize.x, MinDialogSize.y);
		SetForegroundWindow();
		redText.SetFocus();
		bDataChanged = false;
	} else {
		Misc::SaveWindowSizeAndPos(*this, lpMain->Prefs, "DlgEdit_task");
		ShowWindow(SW_HIDE);
		if (pEarlyActiveWindow)
			pEarlyActiveWindow->SetActiveWindow();
	}
}

BOOL CDlgEdit_Task::DestroyWindow()
{
	return CDialog::DestroyWindow();
}

void CDlgEdit_Task::OnEnLinkRedit(NMHDR *pNMHDR, LRESULT *pResult)
{
	ENLINK *pEnLink = reinterpret_cast<ENLINK *>(pNMHDR);
	Utils::RunRichEditLink(redText, pEnLink);
	*pResult = 0;
}

void CDlgEdit_Task::OnOK()
{
	if (SaveData(false) == IDCANCEL)
		return;
	ShowDialog(false, DataTask);
}

void CDlgEdit_Task::OnCancel()
{
	if (SaveData(true) == IDCANCEL)
		return;
	ShowDialog(false, DataTask);
}

void CDlgEdit_Task::ResizeControls()
{
	CRect	posR;
	CRect posTopBar;
	CRect posBottomBar;
	CRect	ClientR;
	GetClientRect(&ClientR);
	CSize Line(0, ControlLineHeight);

	HDWP hPos = Utils::BeginAlignControls(10);

	posR.SetRect(DefaultMargin, DefaultMargin, 0, DefaultMargin + ButtonHeight);
	posTopBar = posR;
	Utils::NextAlignControl(hPos, btnCategories, posR, ButtonWidthSmall);
	posR.DeflateRect(0, 2);
	Utils::NextAlignControl(hPos, labCategories, posR, ClientR.Width() - DefaultMargin - DefaultMargin - DefaultMargin - ButtonWidthSmall);

	posTopBar += Line;
	posR.SetRect(DefaultMargin + 2, posTopBar.top, posR.left, posTopBar.bottom);
	Utils::NextAlignControl(hPos, labStart, posR, 36);
	Utils::NextAlignControl(hPos, edDateStart, posR, 84);
	Utils::NextAlignControl(hPos, edTimeStart, posR, 72);
	posR.left += DefaultMargin;
	Utils::NextAlignControl(hPos, chkStop, posR, 40);
	Utils::NextAlignControl(hPos, edDateStop, posR, 84);
	posR.left += DefaultMargin;
	Utils::NextAlignControl(hPos, btnRepeats, posR, ButtonWidthSmall);
	posR.DeflateRect(0, 2);
	Utils::NextAlignControl(hPos, labRepeats, posR, ClientR.Width() - posR.right - DefaultMargin - DefaultMargin);

	posTopBar += Line;
	posR.SetRect(DefaultMargin + 2, posTopBar.top, posR.left, posTopBar.bottom);
	Utils::NextAlignControl(hPos, labPriority, posR, 54);
	Utils::NextAlignControl(hPos, listPriority, posR, 100);
	posR.left += DefaultMargin;
	Utils::NextAlignControl(hPos, labProgress, posR, 50);
	Utils::NextAlignControl(hPos, trackProgress, posR, 112);
	Utils::NextAlignControl(hPos, labProgressInfo, posR, 82);

	posR.SetRect(ClientR.right - ButtonWidth - DefaultMargin, ClientR.bottom - ButtonHeight - DefaultMargin, ClientR.right - DefaultMargin, ClientR.bottom - DefaultMargin);
	posBottomBar = posR;

	Utils::NextAlignControl(hPos, btnCancel, posR, ButtonWidth, false);
	Utils::NextAlignControl(hPos, btnOK, posR, ButtonWidth, false);
	posR.left = DefaultMargin;
	Utils::NextAlignControl(hPos, btnDelete, posR, ButtonWidth);

	posR.SetRect(ClientR.left + DefaultMargin, posTopBar.bottom + DefaultMargin, ClientR.right - DefaultMargin, posBottomBar.top - DefaultMargin);
	Utils::NextAlignControl(hPos, redText, posR, ClientR.Width() - DefaultMargin - DefaultMargin);

	Utils::EndAlignControls(hPos);
	RedrawWindow();
}

void CDlgEdit_Task::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	ResizeControls();
}

void CDlgEdit_Task::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = MinDialogSize;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgEdit_Task::OnEnChangeEditTaskText()
{
	bDataChanged = true;
}

void CDlgEdit_Task::OnBnClickedEditTaskBtnCategories()
{
	Utils::ShowWindow_categories_forTask(DataTask);
}

int CDlgEdit_Task::SaveData(bool AskIfChanged)
{
	PSXML::PSTinyXML&	TB = lpMain->tbTasks;
	CString S;
	redText.GetWindowText(S);
	DataTask.sText = S;

	//Get and correct date range
	edDateStart.GetTime(&DataTask.DateStart);

	SYSTEMTIME T;
	edTimeStart.GetTime(&T);
	DataTask.DateStart.wMilliseconds = 0;
	DataTask.DateStart.wSecond = 0;
	DataTask.DateStart.wMinute = T.wMinute;
	DataTask.DateStart.wHour = T.wHour;

	edDateStop.GetTime(&DataTask.DateStop);

	Date::RemoveTime(DataTask.DateStop);
	Date::CopyTime(DataTask.DateStart, DataTask.DateStop);

	CTime TimeStart(DataTask.DateStart);
	CTime TimeStop(DataTask.DateStop);
	if (TimeStop < TimeStart)
		DataTask.DateStop = DataTask.DateStart;

	//Get priority
	DataTask.iPriority = listPriority.GetCurSel() - 1;

	//Get progress
	DataTask.iProgress = trackProgress.GetPos() * 10;

	//Check if text changed ?
	int MsgOut = IDCANCEL;
	if (AskIfChanged)
	{
		if (bDataChanged)
		{
			MsgOut = ShowUserMsg(this, umtWarning_YesNoCancel, 81);
			if ((MsgOut == IDNO) || (MsgOut == IDCANCEL))
			{
				if (MsgOut == IDCANCEL)
					redText.SetFocus();
				return MsgOut;
			}
		} else {
			return IDNO;
		}
	}

	//Check if content empty ?
	if (DataTask.sText.empty())
	{
		ShowUserMsg(this, umtWarning, 82);
		return IDCANCEL;
	}

	//Validate data
	if ((chkStop.GetCheck() == BST_CHECKED) && (DataTask.dwRepeats == 0))
		DataTask.dwRepeats = 0x00010000;

	//Save data...
	bDataChanged = false;
	if (DataTask.bNewTask)
	{
		TB.SetUID();
		TB.FieldByName("Text").Set(DataTask.sText);
		TB.FieldByName("Date").SetNowDate();
		TB.FieldByName("Modified").Clear();
		TB.FieldByName("Categories").Set(PSXML::IntToHex(DataTask.dwCategories));
		TB.FieldByName("Repeat").Set(PSXML::IntToHex(DataTask.dwRepeats));
		TB.FieldByName("StartDate").Set(PSXML::DateToStr(DataTask.DateStart));
		TB.FieldByName("EndDate").Set((chkStop.GetCheck() == BST_CHECKED)?PSXML::DateToStr(DataTask.DateStop):"");
		TB.FieldByName("Priority").Set(DataTask.iPriority);
		TB.FieldByName("Progress").Set(DataTask.iProgress);
		TB.AddRow();
	} else {
		if ((!DataTask.sID.empty()) && (TB.Locate(DataTask.sID)))
		{
			TB.GetData();
			TB.FieldByName("Text").Set(DataTask.sText);
			TB.FieldByName("Modified").SetNowDate();
			TB.FieldByName("Categories").Set(PSXML::IntToHex(DataTask.dwCategories));
			TB.FieldByName("Repeat").Set(PSXML::IntToHex(DataTask.dwRepeats));
			TB.FieldByName("StartDate").Set(PSXML::DateToStr(DataTask.DateStart));
			TB.FieldByName("EndDate").Set((chkStop.GetCheck() == BST_CHECKED)?PSXML::DateToStr(DataTask.DateStop):"");
			TB.FieldByName("Priority").Set(DataTask.iPriority);
			TB.FieldByName("Progress").Set(DataTask.iProgress);
			TB.UpdateData();
			lpMain->dlgToday.TaskReminder.ClearTaskAsNotViewed(DataTask.sID);
		}
	}
	TB.SaveTable();
	lpMain->dlgTasks.LoadListItems();
	lpMain->dlgToday.LoadListItems();
	return IDYES;
}

BOOL CDlgEdit_Task::PreTranslateMessage(MSG* pMsg)
{
	TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg);
	labCategories_tip.CheckMsgLoop(pMsg);
	labRepeats_tip.CheckMsgLoop(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

//Accelerator key click: Ctrl+S
void CDlgEdit_Task::OnAccSave()
{
	OnOK();
}

//Filter for CRichEdit events
void CDlgEdit_Task::OnEnMsgfilterEditTaskText(NMHDR *pNMHDR, LRESULT *pResult)
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	Utils::ShowRichEditContextMenu(*this, pMsgFilter, redText, IDR_EDITMENU);
	Utils::DoEvaluateExpression_msg(*pMsgFilter, redText, *this);
	*pResult = 0;
}

void CDlgEdit_Task::OnBnClickedChkStop()
{
	edDateStop.EnableWindow(chkStop.GetCheck() == BST_CHECKED);
	UpdateRepeatsByDateRange();
}

void CDlgEdit_Task::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateProgressLabel();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgEdit_Task::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Utils::DrawControlsLine(dc, *this, ControlLineHeight);
	Utils::DrawControlsLine(dc, *this, ControlLineHeight + ControlLineHeight);
}

void CDlgEdit_Task::OnBnClickedBtnDeleteTask()
{
	PSXML::PSTinyXML&	TB = lpMain->tbTasks;
	if (!DataTask.bNewTask)
	{
		if (ShowUserMsg(this, umtWarning_YesNo, 122) == IDYES)
		{
			TB.Remove(DataTask.sID);
			TB.SaveTable();
			lpMain->dlgToday.TaskReminder.ClearTaskAsNotViewed(DataTask.sID);
			lpMain->dlgTasks.LoadListItems();
			lpMain->dlgToday.LoadListItems();
			OnCancel();
		}
	}
}

void CDlgEdit_Task::OnBnClickedEditTaskBtnRepeats()
{
	Utils::ShowWindow_repeats_forTask(DataTask);
}

void CDlgEdit_Task::UpdateRepeatsByDateRange()
{
	if (DataTask.dwRepeats == 0)
	{
		if (chkStop.GetCheck() == BST_CHECKED)
		{
			DataTask.dwRepeats = 0x00010000; //set every day repeat
		} else {
			DataTask.dwRepeats = 0;	//clear repeat
		}
		std::string sRepeats = lpMain->dlgRepeats.RepeatsAsString(DataTask.dwRepeats);
		labRepeats.SetWindowText(sRepeats.c_str());
		labRepeats_tip.Update(sRepeats.c_str());

		lpMain->dlgRepeats.dwRepeats = DataTask.dwRepeats;
		lpMain->dlgRepeats.InitControls();
	}
}

void CDlgEdit_Task::OnBnClickedLabPrgInfo()
{
	trackProgress.SetPos((labProgressInfo.GetCheck() == BST_CHECKED)?10:0);
	UpdateProgressLabel();
}
