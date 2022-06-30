// DlgEdit_Note.cpp : implementation file
//

#include "DlgEdit_Note.h"
#include "WinWorkBarDlg.h"

// CDlgEdit_Note dialog

extern CWinWorkBarDlg*	lpMain;

IMPLEMENT_DYNAMIC(CDlgEdit_Note, CDialog)
CDlgEdit_Note::CDlgEdit_Note(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEdit_Note::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(A_IDI_MAIN);
  m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_ACCTABLE));
	
	pEarlyActiveWindow = NULL;

	DataNote.bNewNote = true;
	DataNote.dwCategories = 0;
	DataNote.sID.clear();
	DataNote.sText.clear();
}

CDlgEdit_Note::~CDlgEdit_Note()
{
}

void CDlgEdit_Note::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REDIT, redText);
	DDX_Control(pDX, IDC_EDIT_NOTE_BTN_CATEGORIES, btnCategories);
	DDX_Control(pDX, IDC_EDIT_NOTE_LAB_CATEGORIES, labCategories);
	DDX_Control(pDX, IDOK, btnOK);
	DDX_Control(pDX, IDCANCEL, btnCancel);
	DDX_Control(pDX, IDC_DATEFROM, edDateFrom);
	DDX_Control(pDX, IDC_DATETO, edDateTo);
	DDX_Control(pDX, IDC_CHKFROM, chkFrom);
	DDX_Control(pDX, IDC_CHKTO, chkTo);
	DDX_Control(pDX, IDC_BTN_DELETE, btnDelete);
}


BEGIN_MESSAGE_MAP(CDlgEdit_Note, CDialog)
	ON_NOTIFY(EN_LINK, IDC_REDIT, OnEnLinkRedit)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_EN_CHANGE(IDC_REDIT, OnEnChangeEditNoteText)
	ON_BN_CLICKED(IDC_EDIT_NOTE_BTN_CATEGORIES, OnBnClickedEditNoteBtnCategories)
	ON_COMMAND(ID_ACC_SAVE, OnAccSave)
	ON_NOTIFY(EN_MSGFILTER, IDC_REDIT, OnEnMsgfilterEditNoteText)
	ON_BN_CLICKED(IDC_CHKFROM, OnBnClickedChkfrom)
	ON_BN_CLICKED(IDC_CHKTO, OnBnClickedChkto)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBnClickedBtnDelete)
END_MESSAGE_MAP()


// CDlgEdit_Note message handlers

BOOL CDlgEdit_Note::OnInitDialog()
{
	CDialog::OnInitDialog();
	Utils::SetupRichEditForNote(redText);

	//Setup controls
	SetIcon(m_hIcon, TRUE);
	btnDelete.SetThemeHelper(&lpMain->m_ThemeHelper);
	btnCategories.SetThemeHelper(&lpMain->m_ThemeHelper);
	btnOK.SetThemeHelper(&lpMain->m_ThemeHelper);
	btnCancel.SetThemeHelper(&lpMain->m_ThemeHelper);
	btnCategories.SetAlign(CButtonST::ST_ALIGN_VERT);
	btnCategories.SetIcon(IDI_ICO_CATEGORIES);

	labCategories_tip.Set(&labCategories);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEdit_Note::ShowDialog(bool Show, dlgData_note& DN)
{
	//Close util windows if opened
	lpMain->dlgCategories.EndDialog(IDCANCEL);

	lpMain->dlgToday.labTaskView.SelectedNoteID = (Show)?DN.sID:"";
	lpMain->dlgToday.labTaskView.RedrawWindow();

	if (Show)
	{
		//Check save
		if (IsWindowVisible())
		{
			if (DN.sID == DataNote.sID)
				return;
			if (SaveData(true) == IDCANCEL)
				return;
		}

		//Init controls before show
		DataNote = DN;
		std::string DlgTitle = ProgramName + " - " + lpMain->ML.Text((DataNote.bNewNote)?13:79);
		SetWindowText(DlgTitle.c_str());

		btnCategories.SetTooltipText(lpMain->ML.Text(25));
		chkFrom.SetWindowText(lpMain->ML.Text(92));
		chkTo.SetWindowText(lpMain->ML.Text(93));
		btnOK.SetWindowText(lpMain->ML.Text(27));
		btnOK.SetTooltipText("Ctrl + S");
		btnCancel.SetWindowText(lpMain->ML.Text(28));
		btnCancel.SetTooltipText("Esc");
		btnDelete.SetWindowText(lpMain->ML.Text(73));

		//Set btnDelete state
		btnDelete.EnableWindow(!DataNote.bNewNote);

		//Set RichEdit text
		redText.SetSel(0, -1);
		redText.Clear();
		if (DataNote.bNewNote)
		{
			DataNote.sText.clear();
		} else {
			redText.SetWindowText(DataNote.sText.c_str());
		}
		
		//Set categories label by Categories DWORD
		std::string sCategories = lpMain->dlgCategories.CategoriesAsString(DataNote.dwCategories);
		labCategories.SetWindowText(sCategories.c_str());
		labCategories_tip.Update(sCategories.c_str());

		//Set date range
		chkFrom.SetCheck((DataNote.DateFromEmpty())?BST_UNCHECKED:BST_CHECKED);
		chkTo.SetCheck((DataNote.DateToEmpty())?BST_UNCHECKED:BST_CHECKED);
		OnBnClickedChkfrom();
		OnBnClickedChkto();

		SYSTEMTIME ST;
		GetLocalTime(&ST);
		ST.wDay = lpMain->labCalendar.GetSelectedDay().wDay;
		ST.wDayOfWeek = lpMain->labCalendar.GetSelectedDay().wDayOfWeek;
		ST.wMonth = lpMain->labCalendar.GetSelectedDay().wMonth;
		ST.wYear = lpMain->labCalendar.GetSelectedDay().wYear;

		edDateFrom.SetTime(&ST);
		edDateTo.SetTime(&ST);
		if (!DataNote.DateFromEmpty())
			edDateFrom.SetTime(&DataNote.DateFrom);
		if (!DataNote.DateToEmpty())
			edDateTo.SetTime(&DataNote.DateTo);

		//Show window
		pEarlyActiveWindow = GetActiveWindow();
		ResizeControls();
		Misc::LoadWindowSizeAndPos(*this, lpMain->Prefs, "DlgEdit_note", SW_SHOW, MinDialogSize.x, MinDialogSize.y);
		SetForegroundWindow();
		redText.SetFocus();
		bDataChanged = false;
	} else {
		Misc::SaveWindowSizeAndPos(*this, lpMain->Prefs, "DlgEdit_note");
		ShowWindow(SW_HIDE);
		if (pEarlyActiveWindow)
			pEarlyActiveWindow->SetActiveWindow();
	}
}

BOOL CDlgEdit_Note::DestroyWindow()
{
	return CDialog::DestroyWindow();
}

void CDlgEdit_Note::OnEnLinkRedit(NMHDR *pNMHDR, LRESULT *pResult)
{
	ENLINK *pEnLink = reinterpret_cast<ENLINK *>(pNMHDR);
	Utils::RunRichEditLink(redText, pEnLink);
	*pResult = 0;
}

void CDlgEdit_Note::OnOK()
{
	if (SaveData(false) == IDCANCEL)
		return;
	ShowDialog(false, DataNote);
}

void CDlgEdit_Note::OnCancel()
{
	if (SaveData(true) == IDCANCEL)
		return;
	ShowDialog(false, DataNote);
}

void CDlgEdit_Note::ResizeControls()
{
	CRect	posR;
	CRect posTopBar;
	CRect posBottomBar;
	CRect	ClientR;
	GetClientRect(&ClientR);
	CSize Line(0, ControlLineHeight);

	HDWP hPos = Utils::BeginAlignControls(5);

	posR.SetRect(DefaultMargin, DefaultMargin, 0, DefaultMargin + ButtonHeight);
	posTopBar = posR;

	Utils::NextAlignControl(hPos, btnCategories, posR, ButtonWidthSmall);
	posR.DeflateRect(0, 2);
	Utils::NextAlignControl(hPos, labCategories, posR, ClientR.Width() - DefaultMargin - DefaultMargin - DefaultMargin - ButtonWidthSmall);

	posTopBar += Line;
	posR.SetRect(DefaultMargin + 2, posTopBar.top, posR.left, posTopBar.bottom);

	Utils::NextAlignControl(hPos, chkFrom, posR, 74);
	Utils::NextAlignControl(hPos, edDateFrom, posR, 84);
	posR.left += DefaultMargin;
	Utils::NextAlignControl(hPos, chkTo, posR, 40);
	Utils::NextAlignControl(hPos, edDateTo, posR, 84);

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

void CDlgEdit_Note::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	ResizeControls();
}

void CDlgEdit_Note::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = MinDialogSize;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgEdit_Note::OnEnChangeEditNoteText()
{
	bDataChanged = true;
}

void CDlgEdit_Note::OnBnClickedEditNoteBtnCategories()
{
	Utils::ShowWindow_categories_forNote(DataNote);
}

int CDlgEdit_Note::SaveData(bool AskIfChanged)
{
	PSXML::PSTinyXML&	TB = lpMain->tbNotes;
	CString S;
	redText.GetWindowText(S);
	DataNote.sText = S;

	//Get and correct date range
	edDateFrom.GetTime(&DataNote.DateFrom);
	edDateTo.GetTime(&DataNote.DateTo);
	Date::RemoveTime(DataNote.DateFrom);
	Date::RemoveTime(DataNote.DateTo);
	CTime TimeFrom;
	CTime TimeTo;
	edDateFrom.GetTime(TimeFrom);
	edDateTo.GetTime(TimeTo);
	if (TimeTo < TimeFrom)
		DataNote.DateTo = DataNote.DateFrom;

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
	if (DataNote.sText.empty())
	{
		ShowUserMsg(this, umtWarning, 82);
		return IDCANCEL;
	}

	//Save data...
	bDataChanged = false;
	if (DataNote.bNewNote)
	{
		TB.SetUID();
		TB.FieldByName("Text").Set(DataNote.sText);
		TB.FieldByName("Date").SetNowDate();
		TB.FieldByName("Modified").Clear();
		TB.FieldByName("Categories").Set(PSXML::IntToHex(DataNote.dwCategories));
		TB.FieldByName("ShowFrom").Set((chkFrom.GetCheck() == BST_CHECKED)?PSXML::DateToStr(DataNote.DateFrom):"");
		TB.FieldByName("ShowTo").Set((chkTo.GetCheck() == BST_CHECKED)?PSXML::DateToStr(DataNote.DateTo):"");
		TB.AddRow();
	} else {
		if ((!DataNote.sID.empty()) && (TB.Locate(DataNote.sID)))
		{
			TB.GetData();
			TB.FieldByName("Text").Set(DataNote.sText);
			TB.FieldByName("Modified").SetNowDate();
			TB.FieldByName("Categories").Set(PSXML::IntToHex(DataNote.dwCategories));
			TB.FieldByName("ShowFrom").Set((chkFrom.GetCheck() == BST_CHECKED)?PSXML::DateToStr(DataNote.DateFrom):"");
			TB.FieldByName("ShowTo").Set((chkTo.GetCheck() == BST_CHECKED)?PSXML::DateToStr(DataNote.DateTo):"");
			TB.UpdateData();
		}
	}
	TB.SaveTable();
	lpMain->dlgNotes.LoadListItems();
	lpMain->dlgToday.LoadListItems();
	return IDYES;
}

BOOL CDlgEdit_Note::PreTranslateMessage(MSG* pMsg)
{
	TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg);
	labCategories_tip.CheckMsgLoop(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

//Accelerator key click: Ctrl+S
void CDlgEdit_Note::OnAccSave()
{
	OnOK();
}

//Filter for CRichEdit events
void CDlgEdit_Note::OnEnMsgfilterEditNoteText(NMHDR *pNMHDR, LRESULT *pResult)
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	Utils::ShowRichEditContextMenu(*this, pMsgFilter, redText, IDR_EDITMENU);
	Utils::DoEvaluateExpression_msg(*pMsgFilter, redText, *this);
	*pResult = 0;
}

void CDlgEdit_Note::OnBnClickedChkfrom()
{
	edDateFrom.EnableWindow(chkFrom.GetCheck() == BST_CHECKED);
	chkTo.EnableWindow(chkFrom.GetCheck() == BST_CHECKED);
	if (chkFrom.GetCheck() == BST_UNCHECKED)
		chkTo.SetCheck(BST_UNCHECKED);
	OnBnClickedChkto();
}

void CDlgEdit_Note::OnBnClickedChkto()
{
	edDateTo.EnableWindow(chkTo.GetCheck() == BST_CHECKED);
}

void CDlgEdit_Note::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Utils::DrawControlsLine(dc, *this, ControlLineHeight);
}

void CDlgEdit_Note::OnBnClickedBtnDelete()
{
	PSXML::PSTinyXML&	TB = lpMain->tbNotes;
	if (!DataNote.bNewNote)
	{
		if (ShowUserMsg(this, umtWarning_YesNo, 121) == IDYES)
		{
			TB.Remove(DataNote.sID);
			TB.SaveTable();
			lpMain->dlgNotes.LoadListItems();
			lpMain->dlgToday.LoadListItems();
			OnCancel();
		}
	}
}
