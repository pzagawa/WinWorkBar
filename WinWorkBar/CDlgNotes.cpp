// CDlgNotes.cpp : implementation file
//

#include "CDlgNotes.h"
#include "WinWorkBarDlg.h"

// CDlgNotes dialog

extern CWinWorkBarDlg*	lpMain;

IMPLEMENT_DYNAMIC(CDlgNotes, CDialog)

CDlgNotes::CDlgNotes(CWnd* pParent /*=NULL*/)
	: CPSTabCtrlPage(CDlgNotes::IDD, pParent)
{
}

CDlgNotes::~CDlgNotes()
{
}

void CDlgNotes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NOTES, listNotes);
	DDX_Control(pDX, IDC_BTN_NOTES_NEW, btnNew);
	DDX_Control(pDX, IDC_BTN_NOTES_EDIT, btnEdit);
	DDX_Control(pDX, IDC_BTN_NOTES_DELETE, btnDelete);
	DDX_Control(pDX, IDC_INFO_NOTE, labInfo_note);
}


BEGIN_MESSAGE_MAP(CDlgNotes, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_NOTES, OnLvnItemchangedListNotes)
	ON_BN_CLICKED(IDC_BTN_NOTES_EDIT, OnBnClickedBtnNotesEdit)
	ON_BN_CLICKED(IDC_BTN_NOTES_NEW, OnBnClickedBtnNotesNew)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_NOTES, OnNMDblclkListNotes)
	ON_BN_CLICKED(IDC_BTN_NOTES_DELETE, OnBnClickedBtnNotesDelete)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_NOTES, OnLvnKeydownListNotes)
END_MESSAGE_MAP()


// CDlgNotes message handlers
BOOL CDlgNotes::OnInitDialog()
{
	CDialog::OnInitDialog();

	labInfo_note_ttip.Set(&labInfo_note);

	InitControls();

	LoadListItems();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNotes::InitControls()
{
	CWinWorkBarDlg& Dlg = *lpMain;

	//Setup controls
	Utils::SetupXPButton(btnNew, IDI_TBAR_BTN_NEW, 71);
	Utils::SetupXPButton(btnEdit, IDI_TBAR_BTN_EDIT, 72);
	Utils::SetupXPButton(btnDelete, IDI_TBAR_BTN_DELETE, 73);

	//List
	CRect ListRect;
	listNotes.GetClientRect(&ListRect);
	listNotes.SetExtendedStyle(listNotes.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int iColumnCount = listNotes.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < iColumnCount; i++)
   listNotes.DeleteColumn(0);

	listNotes.InsertColumn(0, Dlg.ML.Text(76), LVCFMT_LEFT, DefaultWidth - 64);					//contents
	listNotes.InsertColumn(1, Dlg.ML.Text(25), LVCFMT_LEFT, ListDateTimeColumnWidth);		//categories
	listNotes.InsertColumn(2, Dlg.ML.Text(92), LVCFMT_CENTER, ListDateColumnWidth);			//show from
	listNotes.InsertColumn(3, Dlg.ML.Text(94), LVCFMT_CENTER, ListDateColumnWidth);			//show to
	listNotes.InsertColumn(4, Dlg.ML.Text(77), LVCFMT_CENTER, ListDateTimeColumnWidth);	//added
	listNotes.InsertColumn(5, Dlg.ML.Text(78), LVCFMT_CENTER, ListDateTimeColumnWidth);	//modified
	listNotes.InsertColumn(6, "", LVCFMT_LEFT, 0);		//UID hidden column

	EnableButtonsByState();
}

void CDlgNotes::OnOK()
{
	//CDialog::OnOK();
}

void CDlgNotes::OnCancel()
{
	//CPSTabCtrlPage::OnCancel();
}

BOOL CDlgNotes::OnEraseBkgnd(CDC* pDC)
{
	return DrawDefaultBackground(pDC, true, true);
	//return CDialog::OnEraseBkgnd(pDC);
}

void CDlgNotes::SetInfo(std::string S)
{
	labInfo_note_ttip.Update(Utils::GetFilterDescription(lpMain->Settings.ViewFilter).c_str());
	labInfo_note.SetWindowText((" " + S).c_str());
	labInfo_note.RedrawWindow();
}

void CDlgNotes::ResizeControls()
{
	CRect	ClientR;
	CRect	posR;
	GetClientRect(&ClientR);
	HDWP PosHwnd = BeginDeferWindowPos(3);
	if (!PosHwnd)
		return;
	//Pos buttons
	int btnLeft		= 1;
	posR = ClientR;
	posR.DeflateRect(0, DefaultTabBarMargin, 0, 0);
	posR.bottom = posR.top + DefaultTabBarHeight - (DefaultTabBarMargin << 1);
	if (!Utils::ResizeTabButton(PosHwnd, btnNew, posR, btnLeft))
		return;
	if (!Utils::ResizeTabButton(PosHwnd, btnEdit, posR, btnLeft))
		return;
	if (!Utils::ResizeTabButton(PosHwnd, btnDelete, posR, btnLeft))
		return;

	//Resize info label
	CRect btnR = posR;
	btnR.left = btnLeft + (DefaultTabBarMargin << 1);
	btnR.right = posR.right - (DefaultTabBarMargin << 1);
	btnR.top += 3;
	btnR.bottom -= 3;
	if (!DeferWindowPos(PosHwnd, labInfo_note.GetSafeHwnd(), NULL, btnR.left, btnR.top, btnR.Width(), btnR.Height(), SWP_NOZORDER))
		return;

	//Pos labTaskView
	posR = ClientR;
	posR.DeflateRect(0, DefaultTabBarHeight, 0, 0);
	if (!(PosHwnd == DeferWindowPos(PosHwnd, listNotes.GetSafeHwnd(), NULL, posR.left, posR.top, posR.Width(), posR.Height(), SWP_NOZORDER)))
		return;
	EndDeferWindowPos(PosHwnd);
}

void CDlgNotes::OnSize(UINT nType, int cx, int cy)
{
	CPSTabCtrlPage::OnSize(nType, cx, cy);
	ResizeControls();
}

static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
   CString    strItem1 = pListCtrl->GetItemText((int)lParam1, 0);
   CString    strItem2 = pListCtrl->GetItemText((int)lParam2, 0);
   return strcmp(strItem1, strItem2);
}

bool AcceptRowByView_notes(PSXML::PSTinyXML& TB, DataViewFilter& DVF)
{
	return (
		Utils::Accept_Categories(TB, DVF) &&
		Utils::Accept_Date(DVF.iVisibility, TB.FieldByName("ShowFrom").AsString()) &&
		Utils::Accept_Date(DVF.iAdded, TB.FieldByName("Date").AsString()) &&
		Utils::Accept_Date(DVF.iModified, TB.FieldByName("Modified").AsString())
	);
}

bool	CDlgNotes::LoadListItems()
{
	CWinWorkBarDlg& Dlg = *lpMain;
	PSXML::PSTinyXML&	TB = lpMain->tbNotes;
	SetInfo(Dlg.ML.Text(113));

	listNotes.SetRedraw(false);
	listNotes.DeleteAllItems();

	int count = 0;
	int idx = 0;
	TB.First();
	while (!TB.IsEof())
	{
		if (TB.GetData())
		{
			if (AcceptRowByView_notes(TB, Dlg.Settings.ViewFilter))
			{
				std::string Text = TB.FieldByName("Text").AsString();
				Utils::FormStringToListDisplay(Text);
				int ItemIndex = listNotes.InsertItem(idx, Text.c_str());

				DWORD dwCategories = PSXML::HexToInt(TB.FieldByName("Categories").AsString());
				std::string Categories = lpMain->dlgCategories.CategoriesAsString(dwCategories);
				listNotes.SetItemText(ItemIndex, 1, Categories.c_str());

				SYSTEMTIME ST;
				std::string Date;
				if (PSXML::DateFromStr(TB.FieldByName("ShowFrom").AsString(), ST))
				{
					Date = Text::DateToStringLocale(ST, NULL, false);
					listNotes.SetItemText(ItemIndex, 2, Date.c_str());
				}
				if (PSXML::DateFromStr(TB.FieldByName("ShowTo").AsString(), ST))
				{
					Date = Text::DateToStringLocale(ST, NULL, false);
					listNotes.SetItemText(ItemIndex, 3, Date.c_str());
				}
				if (PSXML::DateFromStr(TB.FieldByName("Date").AsString(), ST))
				{
					Date = Text::DateTimeToStringLocale(ST, NULL, false);
					listNotes.SetItemText(ItemIndex, 4, Date.c_str());
				}
				if (PSXML::DateFromStr(TB.FieldByName("Modified").AsString(), ST))
				{
					Date = Text::DateTimeToStringLocale(ST, NULL, false);
					listNotes.SetItemText(ItemIndex, 5, Date.c_str());
				}

				listNotes.SetItemText(ItemIndex, 6, TB.GetID().c_str());
				idx++;
			}
		}
		TB.Next();
		count++;
	}
	listNotes.SortItems(MyCompareProc, (LPARAM)&listNotes);
	listNotes.SetRedraw(true);
	EnableButtonsByState();
	SetInfo(Utils::InfoListString(count, idx));
	return true;
}

void CDlgNotes::EnableButtonsByState()
{
	int	SelectedItems = Misc::ListCtrlItemsCount(listNotes, true);
	btnNew.EnableWindow(TRUE);
	btnEdit.EnableWindow(SelectedItems == 1);
	btnDelete.EnableWindow(SelectedItems > 0);
}

void CDlgNotes::OnLvnItemchangedListNotes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uChanged == LVIF_STATE)
	{
		EnableButtonsByState();
		Utils::SetMenuItemsStateByListItemsSelect();
	}
	Misc::ProcessMessages();
	*pResult = 0;
}

void CDlgNotes::OnBnClickedBtnNotesNew()
{
	DoNoteNew();
}

void CDlgNotes::OnBnClickedBtnNotesEdit()
{
	if (Misc::ListCtrlItemsCount(listNotes, true) != 1)
		return;
	std::string ID;
	int ItemID = Misc::ListCtrlFirstSelectedItem(listNotes);
	if (ItemID != -1)
	{
		ID = listNotes.GetItemText(ItemID, 6);
		DoNoteEdit(ID);
	}
}

void CDlgNotes::DoNoteNew()
{
	CDlgEdit_Note& Dlg = lpMain->dlgEditNote;
	dlgData_note DN;
	DN.bNewNote = true;
	Dlg.ShowDialog(true, DN);
}

void CDlgNotes::DoNoteEdit(std::string ID)
{
	CDlgEdit_Note& Dlg = lpMain->dlgEditNote;
	PSXML::PSTinyXML&	TB = lpMain->tbNotes;
	if ((!ID.empty()) & (TB.Locate(ID)))
	{
		TB.GetData();
		dlgData_note DN;
		DN.bNewNote = false;
		DN.sID = ID;
		DN.sText = TB.FieldByName("Text").AsString();
		DN.dwCategories = PSXML::HexToInt(TB.FieldByName("Categories").AsString());
		if (!PSXML::DateFromStr(TB.FieldByName("ShowFrom").AsString(), DN.DateFrom))
			DN.DateFrom.wYear = 0;
		if (!PSXML::DateFromStr(TB.FieldByName("ShowTo").AsString(), DN.DateTo))
			DN.DateTo.wYear = 0;
		Dlg.ShowDialog(true, DN);
	}
}

void CDlgNotes::OnNMDblclkListNotes(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedBtnNotesEdit();
	*pResult = 0;
}

void CDlgNotes::OnBnClickedBtnNotesDelete()
{
	PSXML::PSTinyXML&	TB = lpMain->tbNotes;
	if (Misc::ListCtrlItemsCount(listNotes, true) == 0)
		return;

	if (ShowUserMsg(this, umtWarning_YesNo, 83) == IDYES)
	{
		for (int I = 0; I < listNotes.GetItemCount(); I++)
		{
			if (listNotes.GetItemState(I, LVIS_SELECTED) == LVIS_SELECTED)
			{
				std::string UID = listNotes.GetItemText(I, 6);
				TB.Remove(UID);
			}
		}
		TB.SaveTable();
		LoadListItems();
		lpMain->dlgToday.LoadListItems();
	}
}

void CDlgNotes::OnLvnKeydownListNotes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDow->wVKey == VK_RETURN)
		OnBnClickedBtnNotesEdit();
	*pResult = 0;
}

BOOL CDlgNotes::PreTranslateMessage(MSG* pMsg)
{
	labInfo_note_ttip.CheckMsgLoop(pMsg);
	return CPSTabCtrlPage::PreTranslateMessage(pMsg);
}
