// CDlgTasks.cpp : implementation file
//

#include "CDlgTasks.h"
#include "WinWorkBarDlg.h"

// CDlgTasks dialog

extern CWinWorkBarDlg*	lpMain;

IMPLEMENT_DYNAMIC(CDlgTasks, CDialog)

CDlgTasks::CDlgTasks(CWnd* pParent /*=NULL*/)
	: CPSTabCtrlPage(CDlgTasks::IDD, pParent)
{
}

CDlgTasks::~CDlgTasks()
{
}

void CDlgTasks::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TASKS, listTasks);
	DDX_Control(pDX, IDC_BTN_TASKS_NEW, btnNew);
	DDX_Control(pDX, IDC_BTN_TASKS_EDIT, btnEdit);
	DDX_Control(pDX, IDC_BTN_TASKS_DELETE, btnDelete);
	DDX_Control(pDX, IDC_INFO_TASK, labInfo_task);
}


BEGIN_MESSAGE_MAP(CDlgTasks, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TASKS, OnLvnItemchangedListTasks)
	ON_BN_CLICKED(IDC_BTN_TASKS_NEW, OnBnClickedBtnTasksNew)
	ON_BN_CLICKED(IDC_BTN_TASKS_EDIT, OnBnClickedBtnTasksEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TASKS, OnNMDblclkListTasks)
	ON_BN_CLICKED(IDC_BTN_TASKS_DELETE, OnBnClickedBtnTasksDelete)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_TASKS, OnLvnKeydownListTasks)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_TASKS, OnNMCustomdrawListTasks)
END_MESSAGE_MAP()


// CDlgTasks message handlers
BOOL CDlgTasks::OnInitDialog()
{
	CDialog::OnInitDialog();

	labInfo_task_ttip.Set(&labInfo_task);

	InitControls();

	LoadListItems();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTasks::InitControls()
{
	CWinWorkBarDlg& Dlg = *lpMain;

	//Setup controls
	Utils::SetupXPButton(btnNew, IDI_TBAR_BTN_NEW, 71);
	Utils::SetupXPButton(btnEdit, IDI_TBAR_BTN_EDIT, 72);
	Utils::SetupXPButton(btnDelete, IDI_TBAR_BTN_DELETE, 73);

	//List
	CRect ListRect;
	listTasks.GetClientRect(&ListRect);	
	listTasks.SetExtendedStyle(listTasks.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	int iColumnCount = listTasks.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < iColumnCount; i++)
   listTasks.DeleteColumn(0);

	listTasks.InsertColumn(0, Dlg.ML.Text(76), LVCFMT_LEFT, DefaultWidth - 64);						//contents
	listTasks.InsertColumn(1, Dlg.ML.Text(25), LVCFMT_LEFT, ListDateTimeColumnWidth);			//categories
	listTasks.InsertColumn(2, Dlg.ML.Text(117), LVCFMT_CENTER, ListDateTimeColumnWidth);	//date
	listTasks.InsertColumn(3, Dlg.ML.Text(94), LVCFMT_CENTER, ListDateColumnWidth);				//to
	listTasks.InsertColumn(4, Dlg.ML.Text(120), LVCFMT_LEFT, ListDateTimeColumnWidth);	//repeats
	listTasks.InsertColumn(5, Dlg.ML.Text(118), LVCFMT_CENTER, ListDateColumnWidth);	//priority
	listTasks.InsertColumn(6, Dlg.ML.Text(119), LVCFMT_CENTER, ListDateColumnWidth);	//progress
	listTasks.InsertColumn(7, Dlg.ML.Text(77), LVCFMT_CENTER, ListDateTimeColumnWidth);		//added
	listTasks.InsertColumn(8, Dlg.ML.Text(78), LVCFMT_CENTER, ListDateTimeColumnWidth);		//modified
	listTasks.InsertColumn(9, "", LVCFMT_LEFT, 0);		//UID hidden column

	EnableButtonsByState();
}

void CDlgTasks::OnOK()
{
	//CDialog::OnOK();
}

void CDlgTasks::OnCancel()
{
	//CPSTabCtrlPage::OnCancel();
}

BOOL CDlgTasks::OnEraseBkgnd(CDC* pDC)
{
	return DrawDefaultBackground(pDC, true, true);
	//return CDialog::OnEraseBkgnd(pDC);
}

void CDlgTasks::SetInfo(std::string S)
{
	labInfo_task_ttip.Update(Utils::GetFilterDescription(lpMain->Settings.ViewFilter).c_str());
	labInfo_task.SetWindowText((" " + S).c_str());
	labInfo_task.RedrawWindow();
}

void CDlgTasks::ResizeControls()
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
	if (!DeferWindowPos(PosHwnd, labInfo_task.GetSafeHwnd(), NULL, btnR.left, btnR.top, btnR.Width(), btnR.Height(), SWP_NOZORDER))
		return;

	//Pos labTaskView
	posR = ClientR;
	posR.DeflateRect(0, DefaultTabBarHeight, 0, 0);
	if (!(PosHwnd == DeferWindowPos(PosHwnd, listTasks.GetSafeHwnd(), NULL, posR.left, posR.top, posR.Width(), posR.Height(), SWP_NOZORDER)))
		return;
	EndDeferWindowPos(PosHwnd);
}

void CDlgTasks::OnSize(UINT nType, int cx, int cy)
{
	CPSTabCtrlPage::OnSize(nType, cx, cy);
	ResizeControls();
}

void CDlgTasks::EnableButtonsByState()
{
	BOOL Selected = (listTasks.GetSelectedCount() > 0)?TRUE:FALSE;
	btnNew.EnableWindow(TRUE);
	btnEdit.EnableWindow(Selected);
	btnDelete.EnableWindow(Selected);
}

BOOL CDlgTasks::PreTranslateMessage(MSG* pMsg)
{
	labInfo_task_ttip.CheckMsgLoop(pMsg);
	return CPSTabCtrlPage::PreTranslateMessage(pMsg);
}

static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
   CString    strItem1 = pListCtrl->GetItemText((int)lParam1, 0);
   CString    strItem2 = pListCtrl->GetItemText((int)lParam2, 0);
   return strcmp(strItem1, strItem2);
}

bool AcceptRowByView_tasks(PSXML::PSTinyXML& TB, DataViewFilter& DVF)
{
	return (
		Utils::Accept_Categories(TB, DVF) &&
		Utils::Accept_Priority(TB, DVF) &&
		Utils::Accept_Progress(TB, DVF) &&
		Utils::Accept_Date(DVF.iAdded, TB.FieldByName("Date").AsString()) &&
		Utils::Accept_Date(DVF.iModified, TB.FieldByName("Modified").AsString())
	);
}

bool CDlgTasks::LoadListItems()
{
	CWinWorkBarDlg& Dlg = *lpMain;
	PSXML::PSTinyXML&	TB = lpMain->tbTasks;
	SetInfo(Dlg.ML.Text(113));
	listTasks.SetRedraw(false);
	listTasks.DeleteAllItems();
	int count = 0;
	int idx = 0;
	TB.First();
	while (!TB.IsEof())
	{
		if (TB.GetData())
		{
			if (AcceptRowByView_tasks(TB, Dlg.Settings.ViewFilter))
			{
				std::string Text = TB.FieldByName("Text").AsString();
				Utils::FormStringToListDisplay(Text);
				int ItemIndex = listTasks.InsertItem(idx, Text.c_str());

				DWORD dwCategories = PSXML::HexToInt(TB.FieldByName("Categories").AsString());
				std::string Categories = lpMain->dlgCategories.CategoriesAsString(dwCategories);
				listTasks.SetItemText(ItemIndex, 1, Categories.c_str());

				SYSTEMTIME ST;
				std::string Date;
				if (PSXML::DateFromStr(TB.FieldByName("StartDate").AsString(), ST))
				{
					Date = Text::DateTimeToStringLocale(ST, NULL, false);
					listTasks.SetItemText(ItemIndex, 2, Date.c_str());
				}
				if (PSXML::DateFromStr(TB.FieldByName("EndDate").AsString(), ST))
				{
					Date = Text::DateToStringLocale(ST, NULL, false);
					listTasks.SetItemText(ItemIndex, 3, Date.c_str());
				}

				DWORD dwRepeats = PSXML::HexToInt(TB.FieldByName("Repeat").AsString());
				std::string Repeats = lpMain->dlgRepeats.RepeatsAsString(dwRepeats);
				listTasks.SetItemText(ItemIndex, 4, Repeats.c_str());
				
				//Priority
				std::string sPriority;
				switch (TB.FieldByName("Priority").AsInt())
				{
					case -1: sPriority = lpMain->ML.Text(108); break;
					case 0: sPriority = lpMain->ML.Text(107); break;
					case 1: sPriority = lpMain->ML.Text(106); break;
				}
				listTasks.SetItemText(ItemIndex, 5, sPriority.c_str());

				//Progress
				int iProgress = TB.FieldByName("Progress").AsInt();
				std::string sProgress = Text::IntToStr(iProgress) + " %";
				if (iProgress == 0)
					sProgress = lpMain->ML.Text(103);
				if (iProgress == 100)
					sProgress = lpMain->ML.Text(105);
				listTasks.SetItemText(ItemIndex, 6, sProgress.c_str());

				//Progress check box
				listTasks.SetCheck(ItemIndex, (iProgress == 100));

				//Dates
				if (PSXML::DateFromStr(TB.FieldByName("Date").AsString(), ST))
				{
					Date = Text::DateTimeToStringLocale(ST, NULL, false);
					listTasks.SetItemText(ItemIndex, 7, Date.c_str());
				}
				if (PSXML::DateFromStr(TB.FieldByName("Modified").AsString(), ST))
				{
					Date = Text::DateTimeToStringLocale(ST, NULL, false);
					listTasks.SetItemText(ItemIndex, 8, Date.c_str());
				}

				//set item data to checked state for painting grayed items
				listTasks.SetItemData(ItemIndex, (iProgress == 100)?1:0);

				listTasks.SetItemText(ItemIndex, 9, TB.GetID().c_str());
				idx++;
			}
		}
		TB.Next();
		count++;
	}
	listTasks.SortItems(MyCompareProc, (LPARAM)&listTasks);
	listTasks.SetRedraw(true);
	EnableButtonsByState();
	SetInfo(Utils::InfoListString(count, idx));
	return true;
}

void CDlgTasks::OnBnClickedBtnTasksNew()
{
	DoTaskNew();
}

void CDlgTasks::OnBnClickedBtnTasksEdit()
{
	if (Misc::ListCtrlItemsCount(listTasks, true) != 1)
		return;
	std::string ID;
	int ItemID = Misc::ListCtrlFirstSelectedItem(listTasks);
	if (ItemID != -1)
	{
		ID = listTasks.GetItemText(ItemID, 9);
		DoTaskEdit(ID);
	}
}

void CDlgTasks::DoTaskNew()
{
	CDlgEdit_Task& Dlg = lpMain->dlgEditTask;
	dlgData_task DN;
	DN.bNewTask = true;
	Dlg.ShowDialog(true, DN);
}

void CDlgTasks::DoTaskEdit(std::string ID)
{
	CDlgEdit_Task& Dlg = lpMain->dlgEditTask;
	PSXML::PSTinyXML&	TB = lpMain->tbTasks;
	if ((!ID.empty()) & (TB.Locate(ID)))
	{
		TB.GetData();
		dlgData_task DT;
		DT.bNewTask = false;
		DT.sID = ID;
		DT.sText = TB.FieldByName("Text").AsString();
		DT.dwCategories = PSXML::HexToInt(TB.FieldByName("Categories").AsString());
		if (!PSXML::DateFromStr(TB.FieldByName("StartDate").AsString(), DT.DateStart))
			DT.DateStart.wYear = 0;
		if (!PSXML::DateFromStr(TB.FieldByName("EndDate").AsString(), DT.DateStop))
			DT.DateStop.wYear = 0;

		DT.dwRepeats = PSXML::HexToInt(TB.FieldByName("Repeat").AsString());
		DT.iPriority = TB.FieldByName("Priority").AsInt();
		DT.iProgress = TB.FieldByName("Progress").AsInt();		
		Dlg.ShowDialog(true, DT);
	}
}

void CDlgTasks::OnNMDblclkListTasks(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedBtnTasksEdit();
	*pResult = 0;
}

void CDlgTasks::OnBnClickedBtnTasksDelete()
{
	PSXML::PSTinyXML&	TB = lpMain->tbTasks;
	if (Misc::ListCtrlItemsCount(listTasks, true) == 0)
		return;

	if (ShowUserMsg(this, umtWarning_YesNo, 130) == IDYES)
	{
		for (int I = 0; I < listTasks.GetItemCount(); I++)
		{
			if (listTasks.GetItemState(I, LVIS_SELECTED) == LVIS_SELECTED)
			{
				std::string UID = listTasks.GetItemText(I, 9);
				TB.Remove(UID);
				lpMain->dlgToday.TaskReminder.ClearTaskAsNotViewed(UID);
			}
		}
		TB.SaveTable();
		LoadListItems();
		lpMain->dlgToday.LoadListItems();
	}
}

void CDlgTasks::OnLvnKeydownListTasks(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDow->wVKey == VK_RETURN)
		OnBnClickedBtnTasksEdit();
	*pResult = 0;
}

void CDlgTasks::OnNMCustomdrawListTasks(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	//default processing
	*pResult = CDRF_DODEFAULT;
	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
  {
		*pResult = CDRF_NOTIFYITEMDRAW;
	} else {
		if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		{
			LPARAM dwItemData = pLVCD->nmcd.lItemlParam;
			bool bChecked = (dwItemData == 1);
			COLORREF crText = RGB(0, 0, 0);
			if (bChecked)
			{
				crText = RGB(160, 160, 160);
			}
      pLVCD->clrText = crText;
			*pResult = CDRF_DODEFAULT;
		}
	}
}

void CDlgTasks::SetTaskItemProgress(std::string sUID, bool bFinished)
{
	if (!sUID.empty())
	{
		PSXML::PSTinyXML&	TBTask = lpMain->tbTasks;
		if (TBTask.Locate(sUID))
		{
			if (TBTask.GetData())
			{
				TBTask.FieldByName("Modified").SetNowDate();
				TBTask.FieldByName("Progress").Set((bFinished)?100:0);
				TBTask.UpdateData();
				TBTask.SaveTable();
				lpMain->dlgToday.TaskReminder.ClearTaskAsNotViewed(sUID);
				lpMain->dlgTasks.LoadListItems();
				lpMain->dlgToday.LoadListItems();
			}
		}
	}
}

void CDlgTasks::OnLvnItemchangedListTasks(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uChanged == LVIF_STATE)
	{
		//set controls state
		EnableButtonsByState();
		Utils::SetMenuItemsStateByListItemsSelect();

		//set checked and task item progress
		int iItem = pNMLV->iItem;
		UINT NewCheckState = INDEXTOSTATEIMAGEMASK(LVIS_STATEIMAGEMASK & pNMLV->uNewState);
		UINT OldCheckState = INDEXTOSTATEIMAGEMASK(LVIS_STATEIMAGEMASK & pNMLV->uOldState);
		if (NewCheckState != OldCheckState)
		{
			bool bChecked = (listTasks.GetCheck(iItem) == TRUE);
			std::string sUID = listTasks.GetItemText(iItem, 9);
			SetTaskItemProgress(sUID, bChecked);
		}
	}
	Misc::ProcessMessages();
	*pResult = 0;
}
