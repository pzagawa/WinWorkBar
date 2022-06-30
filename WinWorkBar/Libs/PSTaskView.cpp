// PSTaskView.cpp : implementation file
//

#include "PSTaskView.h"


// PSTaskView

const UINT MouseOverTimerID = 0x1234BBBB;

const	POINT RoundCorner = {6, 6};
const	int		xMargin			= 8;
const	int		yMargin			= 8;
const	int		yLineSpace	= 2;
const	int		xRectSpace	= 4;
const	int		xItemMargin	= 8;
const	int		wProgress		= 18;
const	int		hProgress		= 8;
const	int		wPrioprity	= 7;
const	int		hPrioprity	= 7;

enum	clList
{
	colorBorder,
	colorTitle,
	colorSummary,
	colorTitleLine,
	colorHour,
	colorProgressPen,
	colorProgressBkg,
	colorProgressPenDone,
	colorProgressBkgDone,
	colorItemName,
	colorPriorityHigh,
	colorPriorityLow,
	colorItemNameDone,
	colorPriorityNone,
	colorTitleEmpty,
	colorHourDone,
	colorPriorityDone,
	colorMarkPen,
	colorMarkBkg,
};

const	COLORREF	Colors[] =
{
	RGB(88, 119, 173),	//border
	RGB(71, 91, 143),		//title
	RGB(32, 128, 208),	//summary
	RGB(48, 192, 255),	//titleline
	RGB(220, 128, 80),	//hour
	RGB(220, 120, 0),		//progress pen
	RGB(255, 190, 100),	//progress bkg
	RGB(90, 180, 90),		//progress pen done
	RGB(148, 229, 148),	//progress bkg done
	RGB(0, 0, 0),				//item name
	RGB(255, 80, 80),		//priority high
	RGB(100, 100, 255),	//priority low
	RGB(160, 160, 160),	//item name done
	RGB(100, 208, 255),	//priority none
	RGB(144, 192, 232),	//title empty
	RGB(140, 210, 140),	//hour done
	RGB(190, 190, 190),	//priority done
	RGB(160, 160, 208),	//colorMarkPen
	RGB(210, 210, 240),	//colorMarkBkg
};

const POINT ArrowUp[] =
{
	0, 3,
	3, 0,
	6, 3,
	4, 3,
	4, 6,
	2, 6,
	2, 3,
	0, 3,
	3, 1,
	3, 5,
	2, 2,
	5, 2,
};

const POINT ArrowDown[] =
{
	0, 3,
	2, 3,
	2, 0,
	4, 0,
	4, 3,
	6, 3,
	3, 6,
	0, 3,
	2, 4,
	4, 4,
	3, 1,
	3, 6,
};


POINT BuffArrow[12];


//--------------------------------------------------------------------------

bool PSTaskViewItem::TestCursorOver(CWnd* Window, int Yposition)
{
	POINT P;
	GetCursorPos(&P);
	Window->ScreenToClient(&P);
	P.y -= Yposition;
	CRect TestRect = R;
	TestRect.DeflateRect(0, 1);
	if (TestRect.PtInRect(P))
	{
		if (!MouseOver)
		{
			TestRect = R;
			MouseOver = true;
			TestRect.top += Yposition;
			TestRect.bottom += Yposition;
			Window->RedrawWindow(&TestRect);
			return true;
		}
	} else {
		if (MouseOver)
		{
			TestRect = R;
			MouseOver = false;
			TestRect.top += Yposition;
			TestRect.bottom += Yposition;
			Window->RedrawWindow(&TestRect);
			return true;
		}
	}
	return false;
}

bool PSTaskViewItem::TestCursorPos(CWnd* Window, int Yposition)
{
	POINT P;
	GetCursorPos(&P);
	Window->ScreenToClient(&P);
	P.y -= Yposition;
	CRect TestRect = R;
	TestRect.DeflateRect(0, 1);
	if (TestRect.PtInRect(P))
	{
		return true;
	} else {
		return false;
	}
	return false;
}


//--------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(PSTaskView, CStatic)
PSTaskView::PSTaskView()
{
	YPos = 0;
	b24HoursTime = true;
	OnTaskViewClickItem = NULL;
	iViewHeight = 0;
	ZeroMemory(&stCurrentTime, sizeof(SYSTEMTIME));

	CreateFonts();
}

PSTaskView::~PSTaskView()
{
}


BEGIN_MESSAGE_MAP(PSTaskView, CStatic)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// PSTaskView message handlers
int PSTaskView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	Clear();

	return 0;
}

void PSTaskView::CreateFonts()
{
	CFont SysFont;
	LOGFONT LF;
	SysFont.CreateStockObject(DEFAULT_GUI_FONT);
	SysFont.GetLogFont(&LF);

	LF.lfQuality = ANTIALIASED_QUALITY;

	//Normal list
	FontList[ftDefault].CreateFontIndirect(&LF);

	LF.lfWeight = FW_BOLD;
	FontList[ftBold].CreateFontIndirect(&LF);

	LF.lfWeight = FW_NORMAL;
	LF.lfUnderline = TRUE;
	FontList[ftUnderline].CreateFontIndirect(&LF);

	LF.lfWeight = FW_NORMAL;
	LF.lfUnderline = FALSE;
	LF.lfStrikeOut = TRUE;
	FontList[ftStrikeOut].CreateFontIndirect(&LF);

	//Hot list
	LF.lfStrikeOut = FALSE;
	LF.lfUnderline = TRUE;
	FontListHot[ftDefault].CreateFontIndirect(&LF);

	LF.lfWeight = FW_BOLD;
	FontListHot[ftBold].CreateFontIndirect(&LF);

	LF.lfWeight = FW_NORMAL;
	LF.lfUnderline = TRUE;
	FontListHot[ftUnderline].CreateFontIndirect(&LF);

	LF.lfWeight = FW_NORMAL;
	LF.lfStrikeOut = TRUE;
	FontListHot[ftStrikeOut].CreateFontIndirect(&LF);
}

void PSTaskView::DrawBackground(CDC& dc)
{
	//Draw default bkg
	COLORREF Background = GetSysColor(COLOR_3DFACE);
	CBrush DefBkg(Background);
	CPen DefPen(PS_SOLID, 1, Background);
	CPen* lpDefPen = dc.SelectObject(&DefPen);
	CBrush* lpDefBrush = dc.SelectObject(&DefBkg);
	dc.Rectangle(&Rect);
	dc.SelectObject(lpDefBrush);
	dc.SelectObject(lpDefPen);

	//Draw view
	CBrush Bkg(RGB(255, 255, 255));
	CPen Pen(PS_SOLID, 1, Colors[colorBorder]);
	lpDefPen = dc.SelectObject(&Pen);
	lpDefBrush = dc.SelectObject(&Bkg);
	dc.RoundRect(&Rect, RoundCorner);
	dc.SelectObject(lpDefBrush);
	dc.SelectObject(lpDefPen);
}

void PSTaskView::DrawBorder(CDC& dc)
{
	CPen Pen(PS_SOLID, 1, Colors[colorBorder]);
	CPen* lpDefPen = dc.SelectObject(&Pen);
	CBrush* lpDefBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	CPoint P(6, 6);
	dc.RoundRect(&Rect, P);
	dc.SelectObject(lpDefBrush);
	dc.SelectObject(lpDefPen);
}

void PSTaskView::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	CPaintDC DefDC(this); // device context for painting

	CMemDC dc(&DefDC, &Rect);

	//Set clipping
	if (CtrlRegion.m_hObject)
		dc.SelectClipRgn(&CtrlRegion);

	COLORREF DefBKColor = dc.SetBkColor(RGB(255, 255, 255));
	int DefBkMode = dc.SetBkMode(TRANSPARENT);
	DrawBackground(dc);

	for (int i = 0; i < (int)Items.size(); i++)
	{
		_ListItem& Item = Items[i];
		DrawTitle(dc, Item, YPos);
		for (int j = 0; j < (int)Item.SubItems.size(); j++)
		{
			DrawItem(dc, Item.SubItems[j], Item.Type, YPos);
		}
	}

	DrawBorder(dc);
	ClearScrollBackground(dc);
	dc.SetBkMode(DefBkMode);
	dc.SetBkColor(DefBKColor);

	UpdateScrollBar();
}

void PSTaskView::ClearScrollBackground(CDC& dc)
{
	COLORREF Background = GetSysColor(COLOR_3DFACE);
	CBrush DefBkg(Background);
	CPen DefPen(PS_SOLID, 1, Background);
	CPen* lpDefPen = dc.SelectObject(&DefPen);
	CBrush* lpDefBrush = dc.SelectObject(&DefBkg);
	dc.Rectangle(&ScrollRect);
	dc.SelectObject(lpDefBrush);
	dc.SelectObject(lpDefPen);
}

void PSTaskView::DrawTitle(CDC& dc, _ListItem& LI, int Y)
{
	CRect R = LI.Title.R;
	R.top += Y;
	R.bottom += Y;

	CFont* lpDefFont = dc.SelectObject((LI.Title.MouseOver)?&FontListHot[ftBold]:&FontList[ftBold]);

	//Get summary width
	CString Summary;
	if (LI.Type == tpTask)
	{
		Summary.Format("%d/%d", LI.Title.Done, LI.Title.Count);
	} else {
		Summary.Format("%d", LI.Title.Count);
	}
	CSize SummarySize = dc.GetTextExtent(Summary);
	R.right = R.right - SummarySize.cx - xRectSpace;

	//Draw title
	dc.SetTextColor(Colors[(LI.Title.Count)?colorTitle:colorTitleEmpty]);
	dc.DrawText(LI.Title.Name.c_str(), &R, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);

	//Draw summary
	R.left = R.right;
	R.right = LI.Title.R.right;
		
	dc.SetTextColor((LI.Title.Count)?Colors[colorSummary]:Colors[colorTitleEmpty]);
	dc.DrawText(Summary, &R, DT_RIGHT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
	dc.SelectObject(lpDefFont);

	//Draw title line
	CPen Line(PS_SOLID, 1, Colors[colorTitleLine]);
	CPen* lpDefPen = dc.SelectObject(&Line);
	dc.MoveTo(LI.Title.R.left, LI.Title.R.bottom + Y);
	dc.LineTo(LI.Title.R.right + 1, LI.Title.R.bottom + Y);
	dc.SelectObject(lpDefPen);
}

COLORREF DarkerColor(COLORREF crColor, double dFactor)
{
	if (dFactor > 0.0 && dFactor <= 1.0)
	{
		BYTE red,green,blue,lightred,lightgreen,lightblue;
		red = GetRValue(crColor);
		green = GetGValue(crColor);
		blue = GetBValue(crColor);
		lightred = (BYTE)(red-(dFactor * red));
		lightgreen = (BYTE)(green-(dFactor * green));
		lightblue = (BYTE)(blue-(dFactor * blue));
		crColor = RGB(lightred,lightgreen,lightblue);
	}
	return crColor;
}

void PSTaskView::DrawItem(CDC& dc, _SubItem& I, _Type Type, int Y)
{
	CRect R = I.R;
	R.top += Y;
	R.bottom += Y;

	if (Type == tpTask)
	{
		//Draw marker
		if (IsCurrentTime(I))
		{
			CRect rectMarker = R;
			rectMarker.top = rectMarker.top + ((rectMarker.Height() - 4) >> 1);
			rectMarker.bottom = rectMarker.top + 4;
			rectMarker.left += 2;
			rectMarker.right = rectMarker.left + 4;
			CPen PenMark(PS_SOLID, 1, Colors[colorMarkPen]);
			CBrush BkgMark(Colors[colorMarkBkg]);
			CPen* lpDefPenMark = dc.SelectObject(&PenMark);
			CBrush* lpDefBkgMark = dc.SelectObject(&BkgMark);
			dc.Rectangle(&rectMarker);
			dc.SelectObject(lpDefPenMark);
			dc.SelectObject(lpDefBkgMark);
		}

		//Draw hour
		CString Hour;
		int H = I.timeHour;
		if (b24HoursTime)
		{
			Hour.Format("%d:%0.2d", H, I.timeMinute);
		} else {
			if (H > 12)
			{
				H -= 12;
				Hour.Format("%d:%0.2d pm", H, I.timeMinute);
			} else {
				Hour.Format("%d:%0.2d am", H, I.timeMinute);
			}
		}
		CRect rectH = R;
		CFont* lpDefFont = dc.SelectObject(&FontList[ftBold]);
		CSize TextSize = dc.GetTextExtent((b24HoursTime)?"00:00":"00:00 xx");
		rectH.left += xItemMargin;
		rectH.right = rectH.left + TextSize.cx + xRectSpace;
		if (I.Progress == 100)
		{
			dc.SetTextColor(Colors[colorHourDone]);
		} else {
			dc.SetTextColor(Colors[colorHour]);
		}
		dc.DrawText(Hour, &rectH, DT_RIGHT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);
		dc.SelectObject(lpDefFont);

		//Draw progress/done
		CRect rectPrg = R;
		rectPrg.left = rectH.right + xRectSpace + (xRectSpace >> 1);
		rectPrg.right = rectPrg.left + wProgress + xRectSpace;
		int prgMargin = (rectPrg.Height() >> 1) - (hProgress >> 1);
		rectPrg.top += prgMargin;
		rectPrg.bottom -= prgMargin;

		CPen PenDone(PS_SOLID, 1, Colors[colorProgressPenDone]);
		CBrush BkgDone(Colors[colorProgressBkgDone]);
		CPen Pen(PS_SOLID, 1, Colors[colorProgressPen]);
		CBrush Bkg(Colors[colorProgressBkg]);
		CPen FillPen(PS_SOLID, 1, Colors[colorProgressBkg]);

		CPen* lpDefPen = NULL;
		CBrush* lpDefBkg = NULL;

		//Is it done ?
		if (I.Progress == 100)
		{
			lpDefPen = dc.SelectObject(&PenDone);
			lpDefBkg = dc.SelectObject(&BkgDone);
			dc.Rectangle(&rectPrg);
			dc.SelectObject(lpDefPen);
			dc.SelectObject(lpDefBkg);
		} else {
			//Draw border
			CBrush WhiteBkg(RGB(255, 255, 255));
			lpDefPen = dc.SelectObject(&Pen);
			lpDefBkg = dc.SelectObject(&WhiteBkg);
			dc.Rectangle(&rectPrg);
			dc.SelectObject(lpDefPen);
			dc.SelectObject(lpDefBkg);
			//Draw fill
			CRect rectFill = rectPrg;
			rectFill.DeflateRect(1, 1);
			lpDefPen = dc.SelectObject(&FillPen);
			lpDefBkg = dc.SelectObject(&Bkg);
			int W = int ((float)I.Progress * ((float)rectFill.Width() / 100.0));
			rectFill.right = rectFill.left + W;
			dc.Rectangle(&rectFill);
			dc.SelectObject(lpDefPen);
			dc.SelectObject(lpDefBkg);
		}

		//Draw priority
		CRect rectPri = R;
		rectPri.left = rectPrg.right + xRectSpace;
		rectPri.right = rectPri.left + wPrioprity + xRectSpace;
		if (I.Priority == 1)
		{
			for (int i = 0; i < 12; i++)
			{
				BuffArrow[i].x = rectPri.left + ArrowUp[i].x;
				BuffArrow[i].y = rectPri.top + ArrowUp[i].y;
				BuffArrow[i].x += (rectPri.Width() >> 1) - (wPrioprity >> 1);
				BuffArrow[i].y += (rectPri.Height() >> 1) - (hPrioprity >> 1);
			}
			CPen PriPen(PS_SOLID, 1, Colors[(I.Progress == 100)?colorPriorityDone:colorPriorityHigh]);
			lpDefPen = dc.SelectObject(&PriPen);
			dc.Polyline(BuffArrow, 12);
			dc.SelectObject(lpDefPen);
		}
		if (I.Priority == -1)
		{
			for (int i = 0; i < 12; i++)
			{
				BuffArrow[i].x = rectPri.left + ArrowDown[i].x;
				BuffArrow[i].y = rectPri.top + ArrowDown[i].y;
				BuffArrow[i].x += (rectPri.Width() >> 1) - (wPrioprity >> 1);
				BuffArrow[i].y += (rectPri.Height() >> 1) - (hPrioprity >> 1);
			}
			CPen PriPen(PS_SOLID, 1, Colors[(I.Progress == 100)?colorPriorityDone:colorPriorityLow]);
			lpDefPen = dc.SelectObject(&PriPen);
			dc.Polyline(BuffArrow, 12);
			dc.SelectObject(lpDefPen);
		}
		if (I.Priority == 0)
		{
			CPen PriPen(PS_SOLID, 1, Colors[(I.Progress == 100)?colorPriorityDone:colorPriorityNone]);
			lpDefPen = dc.SelectObject(&PriPen);
			int PMarginx = (rectPri.Width() >> 1) - (wPrioprity >> 1);
			int PMarginy = (rectPri.Height() >> 1) - 1;
			dc.MoveTo(rectPri.left + PMarginx, rectPri.top + PMarginy);
			dc.LineTo(rectPri.right - PMarginx, rectPri.top + PMarginy);
			dc.SelectObject(lpDefPen);
		}

		//Draw item name
		CRect rectName = R;
		rectName.left = rectPri.right + xRectSpace;
		rectName.right = R.right;
		if (I.Progress == 100)
		{
			lpDefFont = dc.SelectObject((I.MouseOver)?&FontListHot[ftStrikeOut]:&FontList[ftStrikeOut]);
			dc.SetTextColor(Colors[colorItemNameDone]);
		} else {
			lpDefFont = dc.SelectObject((I.MouseOver)?&FontListHot[ftDefault]:&FontList[ftDefault]);
			dc.SetTextColor(Colors[colorItemName]);
		}

		if (SelectedTaskID == I.ID)
		{
			CBrush NameBkgBrush(RGB(224, 228, 236));
			dc.FillRect(&rectName, &NameBkgBrush);
		}

		dc.DrawText(I.Name.c_str(), &rectName, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
		dc.SelectObject(lpDefFont);
	}	//end if (Type == tpTask)

	if (Type == tpNote)
	{
		CRect rectNoteName = R;
		CFont* lpDefFont = dc.SelectObject((I.MouseOver)?&FontListHot[ftDefault]:&FontList[ftDefault]);
		rectNoteName.left += xItemMargin;
		dc.SetTextColor(Colors[colorItemName]);

		if (SelectedNoteID == I.ID)
		{
			CBrush NameBkgBrush(RGB(224, 228, 236));
			dc.FillRect(&rectNoteName, &NameBkgBrush);
		}

		dc.DrawText(I.Name.c_str(), &rectNoteName, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
		dc.SelectObject(lpDefFont);
	}
}

void PSTaskView::Clear()
{
	SetWindowText("");
	Items.clear();
	RePosition();
	if (GetSafeHwnd())
		RedrawWindow();
}

void PSTaskView::Add(_ListItem& Item)
{
	Items.push_back(Item);
}

void PSTaskView::RePosition()
{
	CDC* lpDC = GetDC();
	CSize TextSize = lpDC->GetTextExtent("ABC");
	ReleaseDC(lpDC);
	if (!TextSize.cy)
		TextSize.SetSize(16, 16);

	GetClientRect(&Rect);
	ClipRect = Rect;
	ScrollRect = Rect;
	iScrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
	Rect.right -= iScrollBarWidth;
	ScrollRect.left = Rect.right;
	
	iViewHeight = 0;

	int ListItemTop = 0;
	for (int i = 0; i < (int)Items.size(); i++)
	{
		int SubItemsDone = 0;
		int SubItemsCount = 0;
		int ListItemBottom = 0;

		_ListItem& Item = Items[i];

		//RePos Title
		Item.Title.R.top = yMargin;
		Item.Title.R.bottom = Item.Title.R.top + TextSize.cy;
		Item.Title.R.left = xMargin;
		Item.Title.R.right = Rect.Width() - xMargin;

		Item.Title.R.top += ListItemTop;
		Item.Title.R.bottom += ListItemTop;

		//Repos Items
		int ListSubItemTop = Item.Title.R.bottom;
		for (int j = 0; j < (int)Item.SubItems.size(); j++)
		{
			_SubItem& SubItem = Item.SubItems[j];

			SubItem.R = Item.Title.R;
			SubItem.R.top = ListSubItemTop + yLineSpace;
			SubItem.R.bottom = SubItem.R.top + TextSize.cy;

			SubItemsDone += (SubItem.Progress == 100)?1:0;
			SubItemsCount++;
			ListItemBottom = SubItem.R.bottom;
			ListSubItemTop += SubItem.R.Height();
		}

		if ((int)Item.SubItems.size() == 0)
			ListItemBottom = yMargin + Item.Title.R.Height();

		Item.R = Item.Title.R;
		Item.R.bottom = ListItemBottom;
		
		ListItemTop += Item.R.Height() + yMargin;

		Item.Title.Done = SubItemsDone;
		Item.Title.Count = SubItemsCount;
	}
	iViewHeight += ListItemTop;
}

void PSTaskView::UpdateSize()
{
	RePosition();
	//Set region for clipping
	CtrlRegion.DeleteObject();
	CtrlRegion.CreateRectRgn(ClipRect.left, ClipRect.top, ClipRect.right, ClipRect.bottom);
	//Scroll bar size & pos
	scrollBarV.SetWindowPos(&CWnd::wndNoTopMost, ScrollRect.left, ScrollRect.top, iScrollBarWidth, ScrollRect.Height(), SWP_NOZORDER);
	UpdateScrollBar();
	RedrawWindow();
}

void PSTaskView::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	if (GetSafeHwnd())
	{
		UpdateSize();
	}
}

void PSTaskView::SetYScroll(int Y)
{
	YPos = Y;
	RedrawWindow();
}

void PSTaskView::InitializeAll()
{
	static bool Initialized = false;
	if (!Initialized)
	{
		Initialized = true;
		scrollBarV.Create(SBS_RIGHTALIGN | SBS_VERT | WS_CHILD, (CRect)&Rect, this, 1);
		UpdateSize();
		scrollBarV.ShowScrollBar();
	}
}

void PSTaskView::UpdateScrollBar()
{
	if (iViewHeight > (Rect.Height() - yLineSpace))
	{
		scrollBarV.SetScrollRange(0, (iViewHeight - Rect.Height() + yLineSpace));
		scrollBarV.EnableScrollBar();
		if (scrollBarV.GetScrollPos() == (iViewHeight - Rect.Height() + yLineSpace))
			YPos = -(iViewHeight - Rect.Height() + yLineSpace);
	} else {
		scrollBarV.EnableScrollBar(ESB_DISABLE_BOTH);
		YPos = 0;
	}
}

void PSTaskView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos); 
	maxpos = pScrollBar->GetScrollLimit();
	// Get the current position of scroll box.
	int curpos = pScrollBar->GetScrollPos();

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
		case SB_TOP:      // Scroll to far left.
			curpos = minpos;
			break;
		case SB_BOTTOM:      // Scroll to far right.
			curpos = maxpos;
			break;
		case SB_ENDSCROLL:   // End scroll.
			break;
		case SB_LINEUP:      // Scroll left.
			if (curpos > minpos)
				curpos--;
			break;
		case SB_LINEDOWN:   // Scroll right.
			if (curpos < maxpos)
				curpos++;
			break;
		case SB_PAGEUP:    // Scroll one page left.
			{
				// Get the page size. 
				SCROLLINFO   info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);
				if (curpos > minpos)
					curpos = max(minpos, curpos - (int) info.nPage);
			}
			break;
		case SB_PAGEDOWN:      // Scroll one page right.
			{
				// Get the page size. 
				SCROLLINFO   info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);
				if (curpos < maxpos)
					curpos = min(maxpos, curpos + (int) info.nPage);
			}
			break;
		case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
			curpos = nPos;      // of the scroll box at the end of the drag operation.
			break;
		case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
			curpos = nPos;     // position that the scroll box has been dragged to.
			break;
	}
	pScrollBar->SetScrollPos(curpos);
	SetYScroll(-curpos);

	CStatic::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL PSTaskView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int Max = iViewHeight - Rect.Height() + yLineSpace;
	int Y = YPos;
	Y = Y + (zDelta >> 2);

	if (Y > 0)
		Y = 0;
	if (Y < (-Max))
		Y = -Max;

	scrollBarV.SetScrollPos(-Y);
	SetYScroll(Y);

	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

void PSTaskView::TestItemClick(CPoint point)
{
	PSTaskViewItem TVItem;
	for (int i = 0; i < (int)Items.size(); i++)
	{
		_ListItem& Item = Items[i];
		if (Item.Title.TestCursorPos(this, YPos))
		{
			TVItem = Item.Title;
			TVItem.Clicked = true;
			TVItem.TypeIsTask = (Item.Type == tpTask);
			break;
		}
		for (int j = 0; j < (int)Item.SubItems.size(); j++)
		{
			_SubItem& SubItem = Item.SubItems[j];
			if (SubItem.TestCursorPos(this, YPos))
			{
				TVItem = SubItem;
				TVItem.Clicked = true;
				TVItem.TypeIsTask = (Item.Type == tpTask);
				break;
			}
		}
	}
	if (TVItem.Clicked)
	{
		if (OnTaskViewClickItem)
			OnTaskViewClickItem(*this, point, TVItem);
	}
}

void PSTaskView::OnLButtonUp(UINT nFlags, CPoint point)
{
	TestItemClick(point);
	CStatic::OnLButtonUp(nFlags, point);
}

void PSTaskView::OnLButtonDown(UINT nFlags, CPoint point)
{

	CStatic::OnLButtonDown(nFlags, point);
}

void PSTaskView::OnLButtonDblClk(UINT nFlags, CPoint point)
{

	CStatic::OnLButtonDblClk(nFlags, point);
}

void PSTaskView::OnMouseMove(UINT nFlags, CPoint point)
{
	KillTimer(MouseOverTimerID);
	TestCursorOverItems();
	SetTimer(MouseOverTimerID, 20, NULL);
	CStatic::OnMouseMove(nFlags, point);
}

void PSTaskView::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == MouseOverTimerID)
	{
		TestCursorOverItems();
	}
	CStatic::OnTimer(nIDEvent);
}

void PSTaskView::TestCursorOverItems()
{
	for (int i = 0; i < (int)Items.size(); i++)
	{
		_ListItem& Item = Items[i];
		if (Item.Title.TestCursorOver(this, YPos))
			return;
		for (int j = 0; j < (int)Item.SubItems.size(); j++)
		{
			_SubItem& SubItem = Item.SubItems[j];
			if (SubItem.TestCursorOver(this, YPos))
				return;
		}
	}
}

void PSTaskView::SetCurrentTime(SYSTEMTIME& ST)
{
	if ((stCurrentTime.wHour != ST.wHour) || (stCurrentTime.wMinute != ST.wMinute))
	{
		stCurrentTime = ST;
		RedrawWindow();
	}
}

bool PSTaskView::IsCurrentTime(_SubItem& SI)
{
	if (stCurrentTime.wHour > SI.timeHour)
		return true;
	if (stCurrentTime.wHour == SI.timeHour)
	{
		if (stCurrentTime.wMinute >= SI.timeMinute)
			return true;
	}
	return false;
}
