// ToolInfoWindowDlg.cpp : implementation file
//

#include "ToolInfoWindowDlg.h"
#include "Libs\PSUtil.h"

#include "WinWorkBarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CWinWorkBarDlg*	lpMain;

const	int TipIconSizeW	= 34;
const	int TipIconSizeH	= 48;
const	int	ControlMargin	= 6;
const	int	ControlHeight	= 13;
const	int	btnWidth			= 80;
const	int	DialogRound		= 9;
const	int TipIconSpace	= ControlMargin + TipIconSizeW + ControlMargin;


// CToolInfoWindowDlg dialog

CToolInfoWindowDlg::CToolInfoWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolInfoWindowDlg::IDD, pParent)
{
}

void CToolInfoWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_TI_SHOW, btnShowTask);
	DDX_Control(pDX, IDC_BTN_TI_FINISH, btnFinishTask);
	DDX_Control(pDX, IDC_BTN_TI_PUTAWAY, btnPutAway);
}

BEGIN_MESSAGE_MAP(CToolInfoWindowDlg, CDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
	ON_STN_CLICKED(IDC_BTN_TI_FINISH, OnStnClickedBtnTiFinish)
	ON_STN_CLICKED(IDC_BTN_TI_SHOW, OnStnClickedBtnTiShow)
	ON_WM_LBUTTONDOWN()
	ON_STN_CLICKED(IDC_BTN_TI_PUTAWAY, OnStnClickedBtnTiPutaway)
END_MESSAGE_MAP()


// CToolInfoWindowDlg message handlers

BOOL CToolInfoWindowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	TipInfoIcon = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE((IDB_TIPINFOICON)));

	//Set controls
	this->GetClientRect(&DlgRect);

	l1R.left = TipIconSpace;
	l1R.right = DlgRect.Width() - ControlMargin;
	l1R.top = ControlMargin;
	l1R.bottom = ControlMargin + ControlHeight;

	l2R.left = TipIconSpace;
	l2R.right = DlgRect.Width() - ControlMargin;
	l2R.top = l1R.bottom + ControlMargin;
	l2R.bottom = l2R.top + ControlHeight;

	prgRect.left = TipIconSpace;
	prgRect.right = DlgRect.Width() - ControlMargin;
	prgRect.top = l2R.bottom + ControlMargin;
	prgRect.bottom = prgRect.top + ControlHeight + 2;
	
	prgRect.top += 2;
	prgRect.bottom += 2;

	int iLeft = prgRect.right - btnWidth;
	btnShowTask.SetWindowPos(&CWnd::wndTop, iLeft, prgRect.top, btnWidth, prgRect.Height(), 0);
	iLeft -= btnWidth + ControlMargin;
	btnFinishTask.SetWindowPos(&CWnd::wndTop, iLeft, prgRect.top, btnWidth, prgRect.Height(), 0);
	iLeft -= btnWidth + ControlMargin;
	btnPutAway.SetWindowPos(&CWnd::wndTop, iLeft, prgRect.top, btnWidth, prgRect.Height(), 0);

	//Set region
	DlgRgn.CreateRoundRectRgn(0, 0, DlgRect.Width() + 1, DlgRect.Height() + 1, DialogRound, DialogRound);
	DlgFrameRgn.CreateRoundRectRgn(0, 0, DlgRect.Width() + 1, DlgRect.Height() + 1, DialogRound, DialogRound);
	SetWindowRgn((HRGN)DlgRgn, TRUE);

	//Prepare objects for paint
	ToolTipBkg.CreateSolidBrush((COLORREF)GetSysColor(COLOR_INFOBK));
	FrameBrush.CreateSolidBrush(RGB(0, 0, 0));
	SysFont.CreateStockObject(DEFAULT_GUI_FONT);

	LOGFONT	mLogFont;
	SysFont.GetLogFont(&mLogFont);
	mLogFont.lfWeight = FW_BOLD;
	BoldFont.CreateFontIndirect(&mLogFont);

	btnShowTask.SetBkColor(RGB(255, 236, 160));
	btnShowTask.SetTextColor(RGB(40, 40, 40));
	btnShowTask.SetLinkCursor(LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_PS_HAND)));
	btnShowTask.SetLink(TRUE, FALSE);
	btnShowTask.SetSunken(TRUE);

	btnFinishTask.SetBkColor(RGB(160, 237, 208));
	btnFinishTask.SetTextColor(RGB(40, 40, 40));
	btnFinishTask.SetLinkCursor(LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_PS_HAND)));
	btnFinishTask.SetLink(TRUE, FALSE);
	btnFinishTask.SetSunken(TRUE);

	btnPutAway.SetBkColor(RGB(202, 218, 248));
	btnPutAway.SetTextColor(RGB(40, 40, 40));
	btnPutAway.SetLinkCursor(LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_PS_HAND)));
	btnPutAway.SetLink(TRUE, FALSE);
	btnPutAway.SetSunken(TRUE);

	Clear();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void CToolInfoWindowDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDialog::OnPaint();

	//Draw window
	dc.SetBkMode(TRANSPARENT);
	dc.FillRect(&DlgRect, &ToolTipBkg);
	dc.FrameRgn(&DlgFrameRgn, &FrameBrush, 1, 1);

	//Draw text
	CFont* lpOldFont = dc.SelectObject(&BoldFont);
	dc.DrawText(lab1, &l1R, DT_END_ELLIPSIS);
	dc.SelectObject(lpOldFont);

	lpOldFont = dc.SelectObject(&SysFont);
	dc.DrawText(lab2, &l2R, DT_END_ELLIPSIS);
	dc.SelectObject(lpOldFont);

	//Draw tip icon
	Misc::DrawTransparentBitmap(dc.GetSafeHdc(), TipInfoIcon, ControlMargin, (DlgRect.Height() >> 1) - (TipIconSizeH >> 1), RGB(255, 0, 255));
}

void	CToolInfoWindowDlg::Clear()
{
	lab1 = "";
	lab2 = "";
	this->RedrawWindow(l1R);
	this->RedrawWindow(l2R);
}

void	CToolInfoWindowDlg::Set(std::string L1, std::string L2)
{
	lab1 = L1.c_str();
	lab2 = L2.c_str();
	this->RedrawWindow(l1R);
	this->RedrawWindow(l2R);
}

void	CToolInfoWindowDlg::SetLabel1(std::string S)
{
	lab1 = S.c_str();
	this->RedrawWindow(l1R);
}

void	CToolInfoWindowDlg::SetLabel2(std::string S)
{
	lab2 = S.c_str();
	this->RedrawWindow(l2R);
}

void CToolInfoWindowDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);
}

void CToolInfoWindowDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonUp(nFlags, point);
	HideInfoWindow();
}

void CToolInfoWindowDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void	CToolInfoWindowDlg::ShowInfoWindow(std::string sTaskUID)
{
	sUID = sTaskUID;

	btnFinishTask.SetWindowText(lpMain->ML.Text(131));
	btnShowTask.SetWindowText(lpMain->ML.Text(132));
	btnPutAway.SetWindowText(lpMain->ML.Text(190));

	std::string labToolInfo1 = lpMain->GetAppTitle() + " : ";
	labToolInfo1 += lpMain->ML.Text(133);
	labToolInfo1 += " ";

	//Get task text
	PSXML::PSTinyXML&	TBTask = lpMain->tbTasks;
	if (TBTask.Locate(sTaskUID))
	{
		if (TBTask.GetData())
		{
			if (TBTask.FieldByName("Progress").AsInt() == 100)
				return;

			//Get task time
			int iHour = 0;
			int iMinute = 0;
			SYSTEMTIME stStartDate;
			std::string sDate = TBTask.FieldByName("StartDate").AsString();
			if (!PSXML::DateFromStr(sDate, stStartDate))
			{
				stStartDate.wHour = 0;
				stStartDate.wMinute = 0;
			}
			iHour = stStartDate.wHour;
			iMinute = stStartDate.wMinute;

			//Prepare localized time
			CString sHour;
			int H = iHour;
			if (lpMain->Settings.Time24Hour)
			{
				sHour.Format("%d:%0.2d", H, iMinute);
			} else {
				if (H > 12)
				{
					H -= 12;
					sHour.Format("%d:%0.2d pm", H, iMinute);
				} else {
					sHour.Format("%d:%0.2d am", H, iMinute);
				}
			}
			labToolInfo1 += sHour;

			//Get task text
			std::string labToolInfo2 = TBTask.FieldByName("Text").AsString();
			Utils::FormStringToListDisplay(labToolInfo2);

			Set(labToolInfo1, labToolInfo2);
			SetWindowPosition(*this, DlgRect.Width(), DlgRect.Height());
			BringInfoWindowToFront();

			Utils::PlayInfoSound(lpMain->Settings.SoundFileName, lpMain->Settings.PlayPCSpeaker);
		}
	}
}

void CToolInfoWindowDlg::BringInfoWindowToFront()
{
	bool bShow = true;
	PSXML::PSTinyXML&	TBTask = lpMain->tbTasks;
	if (TBTask.Locate(sUID))
	{
		if (TBTask.GetData())
		{
			if (lab1.IsEmpty() && lab2.IsEmpty())
				bShow = false;
			if (TBTask.FieldByName("Progress").AsInt() == 100)
				bShow = false;
			if (bShow)
			{
				SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			} else {
				HideInfoWindow();
			}
		}
	}
}

void CToolInfoWindowDlg::HideInfoWindow()
{
	ShowWindow(SW_HIDE);
	if (!sUID.empty())
		lpMain->dlgToday.TaskReminder.SetTaskAsViewed(sUID, false);
	sUID.clear();
	Clear();
}


//--------------------------------------------------------------------------
// UTILS
//--------------------------------------------------------------------------
void SetWindowPosition(CToolInfoWindowDlg& Dlg, int W, int H)
{
	const	int	WindowMargin	= 8;
	CRect DesktopRect;

	CWnd* lpDesktop = Dlg.GetDesktopWindow();
	lpDesktop->GetClientRect(&DesktopRect);
	CRect TaskBarRect = WinTaskBarRect(Dlg);

	int	DeskW = DesktopRect.Width();
	int	DeskH = DesktopRect.Height();
	int	TaskBarW = TaskBarRect.Width();
	int	TaskBarH = TaskBarRect.Height();

	int X = 0;
	int Y = 0;
	UINT	uTaskBarEdge = WinTaskBarEdge(TaskBarRect);
	switch(uTaskBarEdge)
	{
		case ABE_LEFT:
			X = DeskW - W - WindowMargin;
			Y = DeskH - H - WindowMargin;
			break;
		case ABE_RIGHT:
			X = DeskW - W - TaskBarW - WindowMargin;
			Y = DeskH - H - WindowMargin;
			break;
		case ABE_TOP:
			X = DeskW - W - WindowMargin;
			Y = DeskH - H - WindowMargin;
			break;
		case ABE_BOTTOM:
			X = DeskW - W - WindowMargin;
			Y = DeskH - H - TaskBarH - WindowMargin;
			break;
	}
	Dlg.SetWindowPos(&CWnd::wndTop, X, Y, W, H, SWP_NOACTIVATE);
}

RECT&	WinTaskBarRect(CToolInfoWindowDlg& Dlg)
{
	static CRect R;
	static APPBARDATA abd;
	CWnd *pwnd = Dlg.FindWindow("Shell_TrayWnd", NULL);
	if (pwnd)
	{
		abd.cbSize = sizeof(APPBARDATA);
		abd.hWnd = pwnd->m_hWnd;         
		SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
		R = abd.rc;
	}
	return R;
}

UINT WinTaskBarEdge(CRect rc)
{
	UINT uEdge = -1;      
	if (rc.top == rc.left && rc.bottom > rc.right)
	{
			uEdge = ABE_LEFT;
	}
	else if (rc.top == rc.left && rc.bottom < rc.right)
	{
			uEdge = ABE_TOP;
	}
	else if (rc.top > rc.left )
	{
			uEdge = ABE_BOTTOM;
	}
	else
	{
			uEdge = ABE_RIGHT;
	}
	return uEdge;
} 

void CToolInfoWindowDlg::OnStnClickedBtnTiFinish()
{
	if (!sUID.empty())
	{
		PSXML::PSTinyXML&	TBTask = lpMain->tbTasks;
		if (TBTask.Locate(sUID))
		{
			if (TBTask.GetData())
			{
				TBTask.FieldByName("Progress").Set(100);
				TBTask.UpdateData();
				TBTask.SaveTable();
				lpMain->dlgTasks.LoadListItems();
				lpMain->dlgToday.LoadListItems();
				HideInfoWindow();
			}
		}
	}
}

void CToolInfoWindowDlg::OnStnClickedBtnTiShow()
{
	if (!sUID.empty())
	{
		lpMain->dlgTasks.DoTaskEdit(sUID);
		HideInfoWindow();
	}
}

bool AddMinutesToDate(SYSTEMTIME& ST, int iMinutes)
{
	CTime TimeStart(ST);
	CTimeSpan TimeRange(0, 0, iMinutes, 0);
	TimeStart = TimeStart + TimeRange;
	return TimeStart.GetAsSystemTime(ST);
}

void CToolInfoWindowDlg::OnStnClickedBtnTiPutaway()
{
	if (!sUID.empty())
	{
		PSXML::PSTinyXML&	TBTask = lpMain->tbTasks;
		if (TBTask.Locate(sUID))
		{
			if (TBTask.GetData())
			{
				bool bIsOk = true;
				SYSTEMTIME ST;
				std::string Date;
				if (PSXML::DateFromStr(TBTask.FieldByName("StartDate").AsString(), ST))
				{
					if (!AddMinutesToDate(ST, lpMain->Settings.iPutAwayMinutes))
						bIsOk = false;
					TBTask.FieldByName("StartDate").Set(PSXML::DateToStr(ST));
				}
				if (PSXML::DateFromStr(TBTask.FieldByName("EndDate").AsString(), ST))
				{
					if (!AddMinutesToDate(ST, lpMain->Settings.iPutAwayMinutes))
						bIsOk = false;
					TBTask.FieldByName("EndDate").Set(PSXML::DateToStr(ST));
				}

				if (bIsOk)
				{
					TBTask.FieldByName("Modified").SetNowDate();
					TBTask.UpdateData();
					TBTask.SaveTable();
					lpMain->dlgToday.TaskReminder.ClearTaskAsNotViewed(sUID);
					lpMain->dlgTasks.LoadListItems();
					lpMain->dlgToday.LoadListItems();
					HideInfoWindow();
				}
			}
		}
	}
}

