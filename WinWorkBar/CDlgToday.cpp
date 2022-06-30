// CDlgToday.cpp : implementation file
//

#include "CDlgToday.h"
#include "WinWorkBarDlg.h"

// CDlgToday dialog

extern CWinWorkBarDlg*	lpMain;

IMPLEMENT_DYNAMIC(CDlgToday, CDialog)

CDlgToday::CDlgToday(CWnd* pParent /*=NULL*/)
	: CPSTabCtrlPage(CDlgToday::IDD, pParent)
{
}

CDlgToday::~CDlgToday()
{
}

void CDlgToday::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAB_TASKVIEW, labTaskView);
}


BEGIN_MESSAGE_MAP(CDlgToday, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDlgToday message handlers
BOOL CDlgToday::OnInitDialog()
{
	CDialog::OnInitDialog();

	labTaskView.OnTaskViewClickItem = fnOnTaskViewClickItem;
	labTaskView.InitializeAll();

	LoadListItems();
  
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgToday::OnOK()
{
	CDialog::OnOK();
}

BOOL CDlgToday::OnEraseBkgnd(CDC* pDC)
{
	return DrawDefaultBackground(pDC, true, false);
	//return CDialog::OnEraseBkgnd(pDC);
}

void CDlgToday::OnSize(UINT nType, int cx, int cy)
{
	CPSTabCtrlPage::OnSize(nType, cx, cy);
	ResizeControls();
}

void CDlgToday::OnSizing(UINT fwSide, LPRECT pRect)
{
	CPSTabCtrlPage::OnSizing(fwSide, pRect);
	//ResizeControls();
}

void CDlgToday::ResizeControls()
{
	CRect	ClientR;
	CRect	posR;
	GetClientRect(&ClientR);

	HDWP PosHwnd = BeginDeferWindowPos(1);
	if (!PosHwnd)
		return;

	//Pos labTaskView
	posR = ClientR;
	if (!(PosHwnd == DeferWindowPos(PosHwnd, labTaskView.GetSafeHwnd(), NULL, posR.left, posR.top, posR.Width(), posR.Height(), SWP_NOZORDER)))
		return;

	EndDeferWindowPos(PosHwnd);
}

bool SubItemsCompare_notes(PSTaskView::_SubItem S1, PSTaskView::_SubItem S2)
{
   return (stricmp((char*)S2.Name.c_str(), (char*)S1.Name.c_str()) > 0);
}

bool SubItemsCompare_tasks(PSTaskView::_SubItem S1, PSTaskView::_SubItem S2)
{
	if (S1.timeHour < S2.timeHour)
		return true;
	if (S1.timeHour == S2.timeHour)
		return (S1.timeMinute < S2.timeMinute);
  return false;
}

bool	CDlgToday::LoadListItems()
{
	CWinWorkBarDlg& Dlg = *lpMain;
	PSXML::PSTinyXML&	TBNote = lpMain->tbNotes;
	PSXML::PSTinyXML&	TBTask = lpMain->tbTasks;

	PSTaskView::_SubItem	SubItem;
	PSTaskView::_ListItem ItemTask;
	PSTaskView::_ListItem ItemNote;

	labTaskView.Clear();
	labTaskView.b24HoursTime = lpMain->Settings.Time24Hour;

	//Add notes
	Dlg.labCalendar.NoteRangeListClear();

	ItemNote.SubItems.clear();
	ItemNote.Type = PSTaskView::tpNote;
	ItemNote.Title.Name = Dlg.ML.Text(17);

	TBNote.First();
	while (!TBNote.IsEof())
	{
		if (TBNote.GetData())
		{
			//Add data to calendar
			static SYSTEMTIME stFrom;
			static SYSTEMTIME stTo;
			if (PSXML::DateFromStr(TBNote.FieldByName("ShowFrom").AsString(), stFrom))
			{
				if (PSXML::DateFromStr(TBNote.FieldByName("ShowTo").AsString(), stTo))
					Dlg.labCalendar.NoteRangeListAdd(stFrom, stTo);
			}

			//Add to report list
			ZeroMemory(&SubItem, sizeof(SubItem));
			SubItem.IsSubItem = true;
			if (Utils::Accept_NoteDateRange(Dlg.labCalendar.GetSelectedDay(), TBNote))
			{
				std::string Text = TBNote.FieldByName("Text").AsString();
				Utils::FormStringToListDisplay(Text);
				SubItem.ID = TBNote.GetID();
				SubItem.Name = Text;
				ItemNote.SubItems.push_back(SubItem);
			}
		}
		TBNote.Next();
	}
	std::sort(ItemNote.SubItems.begin(), ItemNote.SubItems.end(), SubItemsCompare_notes);

	//Add tasks
	TaskReminder.Clear();

	std::string sDate;
	SYSTEMTIME stStartDate;
	SYSTEMTIME stTodayDate;

	GetLocalTime(&stTodayDate);
	Date::RemoveTime(stTodayDate);

	ItemTask.SubItems.clear();
	ItemTask.Type = PSTaskView::tpTask;
	ItemTask.Title.Name = Dlg.ML.Text(16);

	TBTask.First();
	while (!TBTask.IsEof())
	{
		if (TBTask.GetData())
		{
			//Add to report list
			ZeroMemory(&SubItem, sizeof(SubItem));
			SubItem.IsSubItem = true;
			SubItem.ID = TBTask.GetID();
			SubItem.Progress = TBTask.FieldByName("Progress").AsInt();
			SubItem.Priority = TBTask.FieldByName("Priority").AsInt();

			sDate = TBTask.FieldByName("StartDate").AsString();
			if (!PSXML::DateFromStr(sDate, stStartDate))
			{
				stStartDate.wHour = 0;
				stStartDate.wMinute = 0;
			}
			SubItem.timeHour = stStartDate.wHour;
			SubItem.timeMinute = stStartDate.wMinute;

			if (Utils::Accept_TaskDateRange(Dlg.labCalendar.GetSelectedDay(), TBTask))
			{
				std::string Text = TBTask.FieldByName("Text").AsString();
				Utils::FormStringToListDisplay(Text);
				SubItem.Name = Text;
				ItemTask.SubItems.push_back(SubItem);
			}

			//Add current day task to reminder
			bool bTaskDateAccepted = Utils::Accept_TaskDateRange(stTodayDate, TBTask);
			if (bTaskDateAccepted  && (SubItem.Progress != 100))
				TaskReminder.Add(SubItem.ID, SubItem.timeHour, SubItem.timeMinute);

		}
		TBTask.Next();
	}
	std::sort(ItemTask.SubItems.begin(), ItemTask.SubItems.end(), SubItemsCompare_tasks);

	//Add all
	labTaskView.Add(ItemTask);
	labTaskView.Add(ItemNote);
	labTaskView.UpdateSize();

	//Update notes flags on calendar
	Dlg.labCalendar.SetNotes();

	TaskReminder.EnableCheck();
	return true;
}

void CDlgToday::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPSTabCtrlPage::OnShowWindow(bShow, nStatus);
	if (bShow == TRUE)
		labTaskView.UpdateSize();
}

void fnOnTaskViewClickItem(PSTaskView& TaskView, CPoint& P, PSTaskViewItem& Item)
{
	if (Item.TypeIsTask)
	{
		if (Item.IsTitle)
			lpMain->dlgTasks.DoTaskNew();
		if (Item.IsSubItem)
			lpMain->dlgTasks.DoTaskEdit(Item.ID);
	} else {
		if (Item.IsTitle)
			lpMain->dlgNotes.DoNoteNew();
		if (Item.IsSubItem)
			lpMain->dlgNotes.DoNoteEdit(Item.ID);
	}
}
