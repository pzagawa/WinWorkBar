// DlgSearch.cpp : implementation file
//

#include "DlgSearch.h"
#include "WinWorkBarDlg.h"

// CDlgSearch dialog

extern CWinWorkBarDlg*	lpMain;

IMPLEMENT_DYNAMIC(CDlgSearch, CDialog)
CDlgSearch::CDlgSearch(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSearch::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(A_IDI_MAIN);
  m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_ACCTABLE));
}

CDlgSearch::~CDlgSearch()
{
}

void CDlgSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAB1, labWithWords);
	DDX_Control(pDX, IDC_LAB2, labWithoutWords);
	DDX_Control(pDX, IDC_ED_WITHWORDS, edWithWords);
	DDX_Control(pDX, IDC_ED_WITHOUTWORDS, edWithoutWords);
	DDX_Control(pDX, IDC_FINDLIST, listFinds);
	DDX_Control(pDX, IDOK, btnOk);
	DDX_Control(pDX, IDCANCEL, btnCancel);
	DDX_Control(pDX, IDC_LAB_STATUS, labStatus);
}


BEGIN_MESSAGE_MAP(CDlgSearch, CDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	ON_NOTIFY(NM_DBLCLK, IDC_FINDLIST, OnNMDblclkFindlist)
END_MESSAGE_MAP()


// CDlgSearch message handlers

BOOL CDlgSearch::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Setup controls
	SetIcon(m_hIcon, TRUE);

	//List
	CRect ListRect;
	GetClientRect(&ListRect);
	listFinds.SetExtendedStyle(listFinds.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	listFinds.InsertColumn(0, lpMain->ML.Text(76), LVCFMT_LEFT, ListRect.Width() - 40);			//contents
	listFinds.InsertColumn(1, lpMain->ML.Text(123), LVCFMT_CENTER, 76);	//type (note/task)
	listFinds.InsertColumn(2, lpMain->ML.Text(77), LVCFMT_CENTER, ListDateTimeColumnWidth);	//added
	listFinds.InsertColumn(3, lpMain->ML.Text(78), LVCFMT_CENTER, ListDateTimeColumnWidth);	//modified
	listFinds.InsertColumn(4, "", LVCFMT_LEFT, 0);		//UID hidden column

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSearch::OnOK()
{
	LoadListItems();
}

void CDlgSearch::OnCancel()
{
	ShowDialog(false);
}

void CDlgSearch::ShowDialog(bool Show)
{
	if (Show)
	{
		std::string DlgTitle = ProgramName + " - " + lpMain->ML.Text(15);
		SetWindowText(DlgTitle.c_str());

		labWithWords.SetWindowText(lpMain->ML.Text(134));
		labWithoutWords.SetWindowText(lpMain->ML.Text(135));
		btnOk.SetWindowText(lpMain->ML.Text(136));
		btnCancel.SetWindowText(lpMain->ML.Text(28));
		labStatus.SetWindowText(lpMain->ML.Text(139));

		//Show window
		ResizeControls();
		Misc::LoadWindowSizeAndPos(*this, lpMain->Prefs, "DlgSearch", SW_SHOW, MinDialogSize.x, MinDialogSize.y);
		SetForegroundWindow();
	} else {
		Misc::SaveWindowSizeAndPos(*this, lpMain->Prefs, "DlgSearch");
		ShowWindow(SW_HIDE);
	}
}

void CDlgSearch::ResizeControls()
{
	CRect	posR;
	CRect posTopBar;
	CRect posBottomBar;
	CRect	ClientR;
	GetClientRect(&ClientR);
	CSize Line(0, ControlLineHeight - 5);

	HDWP hPos = Utils::BeginAlignControls(4);

	posR.SetRect(DefaultMargin, DefaultMargin, 0, DefaultMargin + ButtonHeight);
	posTopBar = posR;

	int iLabWidth = 80;
	int iEditWidth = ClientR.Width() - (DefaultMargin * 4) - iLabWidth - ButtonWidth;

	Utils::NextAlignControl(hPos, labWithWords, posR, iLabWidth);
	posR.DeflateRect(0, 2);
	Utils::NextAlignControl(hPos, edWithWords, posR, iEditWidth);
	posR.InflateRect(0, 2);
	Utils::NextAlignControl(hPos, btnOk, posR, ButtonWidth);
	
	posTopBar += Line;
	posR.SetRect(DefaultMargin, posTopBar.top, posR.left, posTopBar.bottom);

	Utils::NextAlignControl(hPos, labWithoutWords, posR, iLabWidth);
	posR.DeflateRect(0, 2);
	Utils::NextAlignControl(hPos, edWithoutWords, posR, iEditWidth);
	posR.InflateRect(0, 2);
	Utils::NextAlignControl(hPos, btnCancel, posR, ButtonWidth);

	Line.SetSize(0, ControlLineHeight);
	posTopBar += Line;
	posR.SetRect(DefaultMargin, posTopBar.top, posR.left, posTopBar.bottom);
	Utils::NextAlignControl(hPos, labStatus, posR, ClientR.Width() - DefaultMargin - DefaultMargin);

	posR.SetRect(DefaultMargin, posTopBar.bottom + DefaultMargin, ClientR.Width() - DefaultMargin, ClientR.Height() - DefaultMargin);
	Utils::NextAlignControl(hPos, listFinds, posR, ClientR.Width() - DefaultMargin - DefaultMargin);

	Utils::EndAlignControls(hPos);
	RedrawWindow();
}

void CDlgSearch::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	ResizeControls();
}

void CDlgSearch::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = MinDialogSize;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgSearch::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Utils::DrawControlsLine(dc, *this, ControlLineHeight + ControlLineHeight - 5);
}

static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
   CString    strItem1 = pListCtrl->GetItemText((int)lParam1, 0);
   CString    strItem2 = pListCtrl->GetItemText((int)lParam2, 0);
   return strcmp(strItem1, strItem2);
}

bool CDlgSearch::AcceptRow_byWords(std::string& S)
{
	CString sWithWords;
	CString sWithoutWords;
	edWithWords.GetWindowText(sWithWords);
	edWithoutWords.GetWindowText(sWithoutWords);

	std::vector<std::string> vWithWords;
	Text::ParseStringToVector((LPCSTR)sWithWords.MakeLower(), vWithWords, " ");
	std::vector<std::string> vWithoutWords;
	Text::ParseStringToVector((LPCSTR)sWithoutWords.MakeLower(), vWithoutWords, " ");

	std::string sLowStr = strlwr((char*)S.c_str());

	bool bWithWords_finded = false;
	for (int i = 0; i < (int)vWithWords.size(); i++)
		if (sLowStr.find(vWithWords[i]) != std::string::npos)
		{
			bWithWords_finded = true;
			break;
		}

	bool bWithoutWords_finded = false;
	for (int i = 0; i < (int)vWithoutWords.size(); i++)
		if (sLowStr.find(vWithoutWords[i]) != std::string::npos)
		{
			bWithoutWords_finded = true;
			break;
		}

	return (bWithWords_finded && (!bWithoutWords_finded));
}

void CDlgSearch::LoadListItemsByTable(PSXML::PSTinyXML& TB, int& SummaryCount, int Type)
{
	int count = 0;
	TB.First();
	while (!TB.IsEof())
	{
		if (TB.GetData())
		{
			std::string Text = TB.FieldByName("Text").AsString();
			if (AcceptRow_byWords(Text))
			{
				Utils::FormStringToListDisplay(Text, 1024);
				int ItemIndex = listFinds.InsertItem(count, Text.c_str());

				listFinds.SetItemText(ItemIndex, 1, lpMain->ML.Text((Type == 1)?137:138));
				listFinds.SetItemData(ItemIndex, Type);

				SYSTEMTIME ST;
				std::string Date;
				if (PSXML::DateFromStr(TB.FieldByName("Date").AsString(), ST))
				{
					Date = Text::DateTimeToStringLocale(ST, NULL, false);
					listFinds.SetItemText(ItemIndex, 2, Date.c_str());
				}
				if (PSXML::DateFromStr(TB.FieldByName("Modified").AsString(), ST))
				{
					Date = Text::DateTimeToStringLocale(ST, NULL, false);
					listFinds.SetItemText(ItemIndex, 3, Date.c_str());
				}

				listFinds.SetItemText(ItemIndex, 4, TB.GetID().c_str());
				count++;
			}
		}
		TB.Next();
	}
	SummaryCount+=count;
}

bool	CDlgSearch::LoadListItems()
{
	labStatus.SetWindowText(lpMain->ML.Text(142));
	labStatus.RedrawWindow();

	CWinWorkBarDlg& Dlg = *lpMain;
	listFinds.SetRedraw(false);
	listFinds.DeleteAllItems();

	int TasksCount = 0;
	int NotesCount = 0;
	LoadListItemsByTable(lpMain->tbTasks, TasksCount, 1);
	LoadListItemsByTable(lpMain->tbNotes, NotesCount, 2);

	listFinds.SortItems(MyCompareProc, (LPARAM)&listFinds);
	listFinds.SetRedraw(true);

	if ((TasksCount + NotesCount) > 0)
	{
		CString S;
		S.Format(lpMain->ML.Text(141), TasksCount + NotesCount, TasksCount, NotesCount);
		labStatus.SetWindowText(S);
	} else {
		labStatus.SetWindowText(lpMain->ML.Text(140));
	}
	labStatus.RedrawWindow();

	return true;
}

void CDlgSearch::ShowItem()
{
	if (Misc::ListCtrlItemsCount(listFinds, true) != 1)
		return;
	std::string ID;
	int ItemID = Misc::ListCtrlFirstSelectedItem(listFinds);
	if (ItemID != -1)
	{
		ID = listFinds.GetItemText(ItemID, 4);
		if (listFinds.GetItemData(ItemID) == 1)
			lpMain->dlgTasks.DoTaskEdit(ID);
		if (listFinds.GetItemData(ItemID) == 2)
			lpMain->dlgNotes.DoNoteEdit(ID);
	}
}

void CDlgSearch::OnNMDblclkFindlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShowItem();
	*pResult = 0;
}
