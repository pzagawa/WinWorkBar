// PSCalendar.cpp : implementation file
//

#include "PSCalendar.h"


// PSCalendar

IMPLEMENT_DYNAMIC(PSCalendar, CStatic)

//Code
//-----------------------------------------

enum CellColorType
{
	ccoltDefault,
	ccoltOtherMonth,
	ccoltCurrentDay,
	ccoltDayOfWeek,
	ccoltNote,
	ccoltFreeDay,
	ccoltFreeDay_Default,
	ccoltFreeDay_OtherMonth,
	ccoltNote_Default,
	ccoltNote_CurrentDay,
	ccoltNote_FreeDay,
	ccoltNote_OtherMonth,
	ccoltNote_OtherMonthFreeDay,
};

struct	CellColors
{
	COLORREF	Background;
	COLORREF	Border;
	COLORREF	Font;
};

CellColors	CellColorsList[] =
{
	RGB(220, 250, 255),	RGB(64, 143, 231),	RGB(0, 64, 128),		//ccoltDefault
	RGB(212, 212,	212),	RGB(183, 183, 183),	RGB(147, 147, 147),	//ccoltOtherMonth
	RGB(255, 255, 192),	RGB(0, 0, 0),				RGB(0, 0, 0),				//ccoltCurrentDay
	RGB(0, 136, 217),		RGB(0, 64, 128),		RGB(255, 255, 255),	//ccoltDayOfWeek
	RGB(0, 210, 0),			RGB(0, 128, 0),			RGB(0, 210, 0),			//ccoltNote

	RGB(255, 128, 128),	RGB(200, 0, 0),			RGB(255, 255, 255),	//ccoltFreeDay - HEADER
	RGB(255, 245, 245),	RGB(255, 120, 120),	RGB(200, 32, 32),		//ccoltFreeDay_Default
	RGB(255, 190,	190),	RGB(255, 160, 160),	RGB(255, 100, 100),	//ccoltFreeDay_OtherMonth

	RGB(220, 250, 255),	RGB(30, 130, 255),	RGB(0, 64, 128),		//ccoltNote_Default
	RGB(255, 255, 192),	RGB(0, 0, 0),				RGB(0, 0, 0),				//ccoltNote_CurrentDay
	RGB(255, 245, 245),	RGB(200, 120, 120),	RGB(200, 32, 32),		//ccoltNote_FreeDay
	RGB(212, 212,	212),	RGB(183, 183, 183),	RGB(147, 147, 147),	//ccoltNote_OtherMonth
	RGB(255, 190,	190),	RGB(255, 160, 160),	RGB(255, 100, 100),	//ccoltNote_OtherMonthFreeDay
};

COLORREF	TitleBarColors[] =
{
	RGB(0, 120, 170),		//bkg
	RGB(0, 64, 80),			//border
	RGB(255, 255, 255),	//font
	RGB(0, 150, 200),		//button left, right bkg
	RGB(0, 170, 230),		//button center
	RGB(0, 0, 0),				//button border
	RGB(0, 80, 140),		//button symbol
	RGB(200, 200, 255),	//button mouseover
};

const	POINT NoteArrow[] = 
{
	0, 0,
	3, 0,
	3, 3,
	0, 0,
	2, 1,
	3, 1,
	3, 2,
	2, 1,
};

const	POINT btnSymbolSize = { 3, 6 };

const	int btnMargin	= 3;
const	int btnWidth	= 24;

const	POINT btnSymbolRight[] = 
{
	0, 0,
	3, 3,
	0, 6,
	0, 0,

	1, 1,
	2, 3,
	1, 5,
	1, 1,
};

const	POINT btnSymbolLeft[] = 
{
	3, 0,
	3, 6,
	0, 3,
	3, 0,

	2, 1,
	2, 5,
	1, 3,
	2, 1,
};

const UINT MouseOverTimerID = 0x1234AAAA;


//Calendar cell
//-----------------------------------------
void PSCalendarCell::Paint(CDC& dc, PSCalendar& Calendar)
{
	if (Style.State == ccsNone)
		return;

	CellColors	CellColor = CellColorsList[ccoltDefault];

	//State for week days line
	if (Style.State == ccsDayOfWeek)
	{
		CellColor = CellColorsList[ccoltDayOfWeek];
		if (Style.Freeday)
			CellColor = CellColorsList[ccoltFreeDay];
	}

	//State for other month
	if (Style.State == ccsOtherMonth)
	{
		CellColor = CellColorsList[ccoltOtherMonth];
		if (Style.Freeday)
			CellColor = CellColorsList[ccoltFreeDay_OtherMonth];
	}

	//State for default day
	if (Style.State == ccsDefault)
	{
		CellColor = CellColorsList[ccoltDefault];
		if (Style.Freeday)
			CellColor = CellColorsList[ccoltFreeDay_Default];
	}

	//State for current day
	if (Style.State == ccsCurrentDay)
	{
		CellColor = CellColorsList[ccoltCurrentDay];
	}

	//Create colors
	COLORREF BkgColor = CellColor.Background;
	if (DayEqual(Calendar.m_TimeSelectedDay))
	{
		if (Style.State == ccsCurrentDay)
		{
			BkgColor-=0x330000;
		} else {
			if (Style.Freeday)
				BkgColor-=0x222200;
			else
				BkgColor-=0x002222;
		}
	}
	CBrush Bkg(BkgColor);
	CPen Line(PS_SOLID, 1, CellColor.Border);
	dc.SetTextColor(CellColor.Font);

	//Draw border and fill
	CRect R = Rect;
	R.DeflateRect(Margin, Margin);
	CBrush* DefaultBrush = dc.SelectObject(&Bkg);
	CPen* DefaultPen = dc.SelectObject(&Line);
	dc.Rectangle(&R);
	dc.SelectObject(DefaultBrush);
	dc.SelectObject(DefaultPen);

	//Draw selected cell border
	if (!Calendar.bSimpleStyle)
	{
		if (DayEqual(Calendar.m_TimeSelectedDay))
		{
			CRect R = Rect;
			CBrush* DefaultBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
			CPen* DefaultPen = dc.SelectObject(&Line);
			POINT P = {2, 2};
			dc.RoundRect(&R, P);
			dc.SelectObject(DefaultBrush);
			dc.SelectObject(DefaultPen);
		}
	}

	//Draw note arrow
	if (Style.Note)
	{
		CellColors NoteColor = CellColorsList[ccoltNote_Default];
		if (Style.State == ccsCurrentDay)
			NoteColor = CellColorsList[ccoltNote_CurrentDay];
		if (Style.Freeday)
			NoteColor = CellColorsList[ccoltNote_FreeDay];

		if (Style.State == ccsOtherMonth)
		{
			NoteColor = CellColorsList[ccoltNote_OtherMonth];
			if (Style.Freeday)
				NoteColor = CellColorsList[ccoltNote_OtherMonthFreeDay];
		}
		
		CPen NoteLine(PS_SOLID, 1, NoteColor.Border);

		POINT NoteArrowBuff[8];
		for (int i = 0; i < 8; i++)
		{
			NoteArrowBuff[i].x = Rect.left + Rect.Width() + NoteArrow[i].x - (Margin << 1) - 3 - 1;
			NoteArrowBuff[i].y = Rect.top + NoteArrow[i].y + (Margin << 1);
		}

		DefaultPen = dc.SelectObject(&NoteLine);
		dc.Polyline(NoteArrowBuff, 8);
		dc.SelectObject(DefaultPen);
	}

	//Change text style
	CFont* DefaultFont = NULL;
	if (MouseOver)
	{
		DefaultFont = dc.SelectObject(&Calendar.m_Font_Bold);
	} else {
		DefaultFont = dc.SelectObject(&Calendar.m_Font_Normal);
	}

	//Draw text
	if (Style.State == ccsDayOfWeek)
	{
		dc.DrawText(ValueBuff, -1, &Rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	if (Style.State == ccsDefault)
	{
		dc.DrawText(DayAsString(), -1, &Rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	if (Style.State == ccsOtherMonth)
	{
		dc.DrawText(DayAsString(), -1, &Rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	if (Style.State == ccsCurrentDay)
	{
		dc.DrawText(DayAsString(), -1, &Rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}

	dc.SelectObject(DefaultFont);
}

void PSCalendarCell::TestCursorPos(CWnd& Dlg)
{
	POINT P;
	GetCursorPos(&P);
	Dlg.ScreenToClient(&P);
	if (Rect.PtInRect(P))
	{
		if (!MouseOver)
		{
			MouseOver = true;
			Dlg.RedrawWindow(&Rect);
		}
	} else {
		if (MouseOver)
		{
			MouseOver = false;
			Dlg.RedrawWindow(&Rect);
		}
	}
}

void PSCalendarElement::TestCursorPos(CWnd& Dlg)
{
	POINT P;
	GetCursorPos(&P);
	Dlg.ScreenToClient(&P);
	if (R.PtInRect(P))
	{
		if (!MouseOver)
		{
			MouseOver = true;
			Dlg.RedrawWindow(&R);
		}
	} else {
		if (MouseOver)
		{
			MouseOver = false;
			Dlg.RedrawWindow(&R);
		}
	}
}


//Main
//-----------------------------------------
PSCalendar::PSCalendar()
{
	Title_RowHeight = 19;
	WeekDay_RowHeight = 19;
	Day_RowHeight = 17;
	FnCalCellClick = NULL;
	FnCalCellRightClick = NULL;
	FnNavButtonClick = NULL;

	for (int i = 0; i < 7; i++)
		FreeDays[i] = false;
	FreeDays[5] = true;
	FreeDays[6] = true;

	SetDefaultFont();

	DoCreateFont(m_Font_Normal);
	//SetFontName(m_Font_Normal, "Arial");
	SetFontSize(m_Font_Normal, 8);
	SetFontStyle(m_Font_Bold, true, false, false);
	SetFontStyle(m_Font_Underline, false, false, true);

	ZeroMemory(&m_TimeToday, sizeof(SYSTEMTIME));

	GetLocalTime();
	SelectDay(m_TimeToday);
	ShowAt();

	//Init weekday names
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME1, DaysOfWeek[0].ValueBuff, sizeof(DaysOfWeek[0].ValueBuff));
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME2, DaysOfWeek[1].ValueBuff, sizeof(DaysOfWeek[1].ValueBuff));
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME3, DaysOfWeek[2].ValueBuff, sizeof(DaysOfWeek[2].ValueBuff));
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME4, DaysOfWeek[3].ValueBuff, sizeof(DaysOfWeek[3].ValueBuff));
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME5, DaysOfWeek[4].ValueBuff, sizeof(DaysOfWeek[4].ValueBuff));
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME6, DaysOfWeek[5].ValueBuff, sizeof(DaysOfWeek[5].ValueBuff));
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME7, DaysOfWeek[6].ValueBuff, sizeof(DaysOfWeek[6].ValueBuff));

	//Init styles
	DaysOfWeek[5].Style.Freeday = true;
	DaysOfWeek[6].Style.Freeday = true;

	bSimpleStyle = false;
}

PSCalendar::~PSCalendar()
{
}


BEGIN_MESSAGE_MAP(PSCalendar, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_CREATE()
ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// PSCalendar message handlers
int PSCalendar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetWindowText("");

	if (GetSafeHwnd())
		RedrawWindow();

	return 0;
}

void	PSCalendar::SetFreeDays(int Day, bool Free)
{
	FreeDays[Day]=Free;
	DaysOfWeek[Day].Style.Freeday = Free;
	for (int row = 0; row < 6; row++)
	{
		int Mul = row * 7;
		for (int i = 0;i < 7; i++)
			if (i == Day)
				Days[i + Mul].Style.Freeday = Free;
	}
	RedrawWindow();
}

void PSCalendar::GetLocalTime()
{
	FILETIME	FT;
	FILETIME	LocalFileTime;
	GetSystemTime(&m_TimeToday);
	SystemTimeToFileTime(&m_TimeToday, &FT);
	FileTimeToLocalFileTime(&FT, &LocalFileTime);
	FileTimeToSystemTime(&LocalFileTime, &m_TimeToday);
}

void PSCalendar::UpdateToday(bool Redraw)
{
	GetLocalTime();
	CalcCalendarData();
	SetNotes();
	if (Redraw)
		RedrawWindow();
}

void PSCalendar::ShowAt(int Month, int Year)
{
	if ((Month == 0) && (Year == 0))
	{
		m_StartMonth = m_TimeToday.wMonth;
		m_StartYear = m_TimeToday.wYear;
	} else {
		m_StartMonth = Month;
		m_StartYear = Year;
	}
	CalcCalendarData();

	SetNotes();

	if (GetSafeHwnd())
		RedrawWindow();
}

void PSCalendar::SetDefaultFont()
{
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(m_FontLog),&m_FontLog);
}

void PSCalendar::DoCreateFont(CFont& F)
{
	F.DeleteObject();
	m_FontLog.lfQuality = ANTIALIASED_QUALITY;
	F.CreateFontIndirect(&m_FontLog);
}

void PSCalendar::SetFontName(CFont& F, std::string Name)
{
	lstrcpy(m_FontLog.lfFaceName, Name.c_str());
	DoCreateFont(F);
	RedrawWindow();
}

void PSCalendar::SetFontSize(CFont& F, int Size)
{
	CFont cf;
	LOGFONT lf;

	cf.CreatePointFont(Size * 10, m_FontLog.lfFaceName);
	cf.GetLogFont(&lf);

	m_FontLog.lfHeight = lf.lfHeight;
	m_FontLog.lfWidth = lf.lfWidth;

	DoCreateFont(F);

	if (GetSafeHwnd())
		RedrawWindow();
}

void PSCalendar::SetFontStyle(CFont& F, bool B, bool I, bool U)
{
	m_FontLog.lfWeight = (B)?FW_BOLD:FW_NORMAL;
	m_FontLog.lfItalic = (I)?TRUE:FALSE;
	m_FontLog.lfUnderline = (U)?TRUE:FALSE;
	DoCreateFont(F);

	if (GetSafeHwnd())
		RedrawWindow();
}

//Calc rectangles
//---------------------------------------------
void PSCalendar::CalcRectangles()
{
	CRect R;
	GetClientRect(&R);

	//Wyliczam równ¹ szerokoœæ dla ka¿dej komórki
	int CellWidth = R.Width() / 7;
	int SumWidth = CellWidth * 7;
	while (true)
	{
		SumWidth = CellWidth * 7;
		if (SumWidth > R.Width())
		{
			CellWidth--;
			if (CellWidth == 0)
				break;
		} else {
			break;
		}
	}

	//Centrowanie w poziomie
	int	TopMargin  = 0;
	int LeftMargin = (R.Width() - SumWidth) >> 1;

	//Inicjalizacja wartoœci dla RECT TitleBar
	TitleBar.R.top = TopMargin;
	TitleBar.R.bottom = TitleBar.R.top + Title_RowHeight;
	TitleBar.R.left = LeftMargin;
	TitleBar.R.right = TitleBar.R.left + SumWidth;

	//Inicjalizacja wartoœci przycisków TitleBar
	btnNext.R.top = TitleBar.R.top + btnMargin;
	btnNext.R.bottom = TitleBar.R.bottom - btnMargin;
	btnNext.R.right = TitleBar.R.right - btnMargin;
	btnNext.R.left = btnNext.R.right - btnWidth;

	btnToday.R = btnNext.R;
	btnToday.R.right = btnNext.R.left - btnMargin;
	btnToday.R.left = btnToday.R.right - btnWidth;

	btnPrev.R = btnToday.R;
	btnPrev.R.right = btnToday.R.left - btnMargin;
	btnPrev.R.left = btnPrev.R.right - btnWidth;

	TitleCaption.R = TitleBar.R;
	TitleCaption.R.right = btnPrev.R.left;
	TitleCaption.R.DeflateRect(1, 1);

	//Inicjalizacja wartoœci dla tablicy DaysOfWeek
	CRect CellR;
	CellR.top = TitleBar.R.bottom + 1;
	CellR.bottom = CellR.top + WeekDay_RowHeight;
	CellR.left = LeftMargin;
	CellR.right = CellR.left + CellWidth;
	for (int i = 0; i < 7; i++)
	{
		DaysOfWeek[i].Rect = CellR;
		DaysOfWeek[i].Style.State = ccsDayOfWeek;
		CellR.left += CellWidth;
		CellR.right += CellWidth;
	}

	//Inicjalizacja wartoœci dla tablicy Days
	for (int row = 0; row < 6; row++)
	{
		int Mul = row * 7;
		CellR.top = CellR.bottom + TopMargin;
		CellR.bottom = CellR.top + Day_RowHeight;
		CellR.left = LeftMargin;
		CellR.right = CellR.left + CellWidth;
		for (int i = 0;i < 7; i++)
		{
			Days[i + Mul].Rect = CellR;
			CellR.left += CellWidth;
			CellR.right += CellWidth;
			//Aktualizacja stylu dla wolnych dni
			Days[i + Mul].Style.Freeday = FreeDays[i];
		}
	}
	if (GetSafeHwnd())
		RedrawWindow();
}

//PAINT !!!
//---------------------------------------------
void PSCalendar::OnPaint()
{
	CPaintDC DefDC(this); //Do not call CStatic::OnPaint() for painting messages

	CRect Rect;
	GetClientRect(&Rect);

	CMemDC dc(&DefDC, &Rect);

	dc.SetBkColor(GetSysColor(COLOR_3DFACE));

	//Fill bkg
	dc.SetBkMode(TRANSPARENT);
	CBrush Bkg(GetSysColor(COLOR_3DFACE));
	dc.FillRect(&Rect, &Bkg);

	PaintTitleBar(dc);

	//Paint week days
	CFont* DefaultFont = dc.SelectObject(&m_Font_Bold);
	for (int i = 0;i < 7; i++)
		DaysOfWeek[i].Paint(dc, *this);
	dc.SelectObject(DefaultFont);

	//Paint month days
	DefaultFont = dc.SelectObject(&m_Font_Normal);
	for (int i = 0; i < 42; i++)
		Days[i].Paint(dc, *this);
	dc.SelectObject(DefaultFont);
}

void PSCalendar::PaintTitleBar(CDC& dc)
{
	static int XMargin = 4;
	CBrush TitleBkg(TitleBarColors[0]);
	CPen TitleLine(PS_SOLID, 1, TitleBarColors[1]);
	
	CRect R = TitleBar.R;
	CBrush* DefaultBrush = dc.SelectObject(&TitleBkg);
	CPen* DefaultPen = dc.SelectObject(&TitleLine);
	dc.Rectangle(&R);
	dc.SelectObject(DefaultBrush);
	dc.SelectObject(DefaultPen);

	dc.SetTextColor(TitleBarColors[2]);
	CFont* DefaultFont = dc.SelectObject(&m_Font_Bold);

	//Draw title text
	std::string S;
	static char Buff[10];
	Buff[0] = 0;
	itoa(m_StartMonth, Buff, 10);
	S += Buff;
	S += " - ";
	itoa(m_StartYear, Buff, 10);
	S += Buff;

	R.DeflateRect(XMargin, 0);

	dc.DrawText(S.c_str(), -1, &TitleCaption.R, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	dc.SelectObject(DefaultFont);

	//Draw buttons
	if (!bSimpleStyle)
	{
		CBrush btnBkgMouseOver(TitleBarColors[7]);
		CBrush btnBkg(TitleBarColors[3]);
		CBrush btnCenterBkg(TitleBarColors[4]);
		POINT Round = {6, 6};

		//btnPrev
		if (btnPrev.MouseOver)
		{
			DefaultBrush = dc.SelectObject(&btnBkgMouseOver);
		} else {
			DefaultBrush = dc.SelectObject(&btnBkg);
		}
		DefaultPen = dc.SelectObject(&TitleLine);
		dc.RoundRect(&btnPrev.R, Round);
		dc.SelectObject(DefaultBrush);
		dc.SelectObject(DefaultPen);

		//btnNext
		if (btnNext.MouseOver)
		{
			DefaultBrush = dc.SelectObject(&btnBkgMouseOver);
		} else {
			DefaultBrush = dc.SelectObject(&btnBkg);
		}
		DefaultPen = dc.SelectObject(&TitleLine);
		dc.RoundRect(&btnNext.R, Round);
		dc.SelectObject(DefaultBrush);
		dc.SelectObject(DefaultPen);

		//btnToday
		if (btnToday.MouseOver)
		{
			DefaultBrush = dc.SelectObject(&btnBkgMouseOver);
		} else {
			DefaultBrush = dc.SelectObject(&btnCenterBkg);
		}
		DefaultPen = dc.SelectObject(&TitleLine);
		dc.RoundRect(&btnToday.R, Round);
		dc.SelectObject(DefaultBrush);
		dc.SelectObject(DefaultPen);

		//Symbols data init
		int LeftMargin = (btnWidth >> 1) - (btnSymbolSize.x >> 1);
		int TopMargin = (btnPrev.R.Height() >> 1) - (btnSymbolSize.y >> 1);
		CPen btnLine(PS_SOLID, 1, TitleBarColors[6]);
		CBrush btnSymbolBkg(TitleBarColors[6]);
		DefaultPen = dc.SelectObject(&btnLine);
		DefaultBrush = dc.SelectObject(&btnSymbolBkg);

		//btnArrowLeft
		POINT btnL[8];
		for (int i = 0; i < 8; i++)
		{
			btnL[i].x = btnPrev.R.left + btnSymbolLeft[i].x + LeftMargin - 1;
			btnL[i].y = btnPrev.R.top + btnSymbolLeft[i].y + TopMargin;
		}
		dc.Polyline(btnL, 8);
		//btnArrowRight
		POINT btnR[8];
		for (int i = 0; i < 8; i++)
		{
			btnR[i].x = btnNext.R.left + btnSymbolRight[i].x + LeftMargin;
			btnR[i].y = btnNext.R.top + btnSymbolRight[i].y + TopMargin;
		}
		dc.Polyline(btnR, 8);
		//btnToday
		LeftMargin = (btnWidth >> 1) - (btnSymbolSize.x);
		TopMargin = (btnPrev.R.Height() >> 1) - (btnSymbolSize.y >> 1);
		RECT btnCenterSymbol = { btnToday.R.left + LeftMargin, btnToday.R.top + TopMargin, btnToday.R.right - LeftMargin, btnToday.R.bottom - TopMargin };
		dc.Rectangle(&btnCenterSymbol);
		dc.SelectObject(DefaultBrush);
		dc.SelectObject(DefaultPen);
	}
}

bool	PSCalendar::IsWeekend()
{
	//change days numbers
	int DayOfWeek = m_TimeToday.wDayOfWeek;
	if (DayOfWeek == 0)
	{
		DayOfWeek = 6;
	} else {
		DayOfWeek--;
	}
	return FreeDays[DayOfWeek];
}

//Needs: day, month, year
//returns: numebr of week day: monday 0... sunday 7
int PSCalendar::NumberDayOfWeek(SYSTEMTIME ST)
{
	FILETIME FT;
	SystemTimeToFileTime(&ST, &FT);
	FileTimeToSystemTime(&FT, &ST); //(0 - sunday, 1 - monday, etc...)
	//change days numbers
	if (ST.wDayOfWeek == 0)
	{
		ST.wDayOfWeek = 6;
	} else {
		ST.wDayOfWeek--;
	}
	return ST.wDayOfWeek;
}

int PSCalendar::NumberDaysOfMonth(SYSTEMTIME ST)
{
	int days[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if (ST.wMonth == 2)
	{
		if (ST.wYear % 4)       // Not leap year
			return 28;
		if (ST.wYear % 100)     // It is leap year
			return 29;
		if (ST.wYear % 400)     // Not leap year
			return 28;
		return 29;          // It is leap year
	} else {
		return days[ST.wMonth];
	}
}

void PSCalendar::CalcCalendarData()
{
	SYSTEMTIME PrevDate;
	SYSTEMTIME CurrDate;
	SYSTEMTIME NextDate;
	ZeroMemory(&CurrDate, sizeof(SYSTEMTIME));

	CurrDate.wDay = 1;
	CurrDate.wMonth = m_StartMonth;
	CurrDate.wYear = m_StartYear;

	//Pobieranie liczy dni poprzedniego miesi¹ca
	PrevDate = CurrDate;
	PrevDate.wMonth--;
	if (PrevDate.wMonth < 1)
	{
		PrevDate.wMonth = 12;
		PrevDate.wYear--;
	}
	int PrevMonthDays = NumberDaysOfMonth(PrevDate);

	//Pobieranie liczy dni bie¿¹cego miesi¹ca
	int CurrentMonthDays = NumberDaysOfMonth(CurrDate);

	//Pobieranie liczy dni nastêpnego miesi¹ca
	NextDate = CurrDate;
	NextDate.wMonth++;
	if (NextDate.wMonth > 12)
	{
		NextDate.wMonth = 1;
		NextDate.wYear++;
	}
	int NextMonthDays = NumberDaysOfMonth(NextDate);

	//Pobieranie numeru dnia 1szego bie¿¹cego miesi¹ca
	int FirstDayOfMonth = NumberDayOfWeek(CurrDate);

	//Walidacja
	if (FirstDayOfMonth > 41)
		FirstDayOfMonth = 41;
	if (CurrentMonthDays > 41)
		CurrentMonthDays = 41;
	if (CurrentMonthDays < 0)
		CurrentMonthDays = 0;

	//Wprowadzanie danych dla komórki
	int PrevCounter = PrevMonthDays - (FirstDayOfMonth - 1);
	int CurrCounter = 1;
	int NextCounter = 1;

	for (int i = 0; i < 42; i++)
	{
		if (i < FirstDayOfMonth)
		{
			Days[i].Style.State = ccsOtherMonth;
			Days[i].Day = PrevCounter++;
			Days[i].Month = PrevDate.wMonth;
			Days[i].Year = PrevDate.wYear;
		}
		if (i >= FirstDayOfMonth)
		{
			if (i < (FirstDayOfMonth + CurrentMonthDays))
			{
				Days[i].Style.State = ccsDefault;
				Days[i].Day = CurrCounter++;
				Days[i].Month = CurrDate.wMonth;
				Days[i].Year = CurrDate.wYear;
				if (Days[i].DayEqual(m_TimeToday))
					Days[i].Style.State = ccsCurrentDay;
			} else {
				Days[i].Style.State = ccsOtherMonth;
				Days[i].Day = NextCounter++;
				Days[i].Month = NextDate.wMonth;
				Days[i].Year = NextDate.wYear;
			}
		}
	}
}

void PSCalendar::ShowAtPrevMonth()
{
	int M = m_StartMonth;
	int Y = m_StartYear;
	M--;
	if (M < 1)
	{
		M = 12;
		Y--;
	}
	ShowAt(M, Y);
}

void PSCalendar::ShowAtNextMonth()
{
	int M = m_StartMonth;
	int Y = m_StartYear;
	M++;
	if (M > 12)
	{
		M = 1;
		Y++;
	}
	ShowAt(M, Y);
}

void PSCalendar::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	Resize();
}

void PSCalendar::OnMouseMove(UINT nFlags, CPoint point)
{
	btnToday.TestCursorPos(*this);
	btnPrev.TestCursorPos(*this);
	btnNext.TestCursorPos(*this);

	for (int i = 0; i < 42; i++)
		Days[i].TestCursorPos(*this);

	KillTimer(MouseOverTimerID);
	SetTimer(MouseOverTimerID, 10, NULL);

	CStatic::OnMouseMove(nFlags, point);
}

void PSCalendar::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == MouseOverTimerID)
	{
		btnToday.TestCursorPos(*this);
		btnPrev.TestCursorPos(*this);
		btnNext.TestCursorPos(*this);

		for (int i = 0; i < 42; i++)
			Days[i].TestCursorPos(*this);
	}
	CStatic::OnTimer(nIDEvent);
}

void PSCalendar::OnLButtonUp(UINT nFlags, CPoint point)
{
	//Test for nav buttons
	if (btnToday.R.PtInRect(point))
	{
		UpdateToday(false);
		SelectDayToday();
		
		FireCellClickEvent();

		if (FnNavButtonClick)
			FnNavButtonClick();
		return;
	}

	//prev/next buttons
	if (btnPrev.R.PtInRect(point))
	{
		ShowAtPrevMonth();
		if (FnNavButtonClick)
			FnNavButtonClick();
	}
	if (btnNext.R.PtInRect(point))
	{
		ShowAtNextMonth();
		if (FnNavButtonClick)
			FnNavButtonClick();
	}

	//Test for day cells
	for (int i = 0; i < 42; i++)
	{
		if (Days[i].Rect.PtInRect(point))
		{
			SelectDay(Days[i].GetSysTime());
			FireCellClickEvent();
			return;
		}
	}
	CStatic::OnLButtonUp(nFlags, point);
}

void PSCalendar::FireCellClickEvent()
{
	for (int i = 0; i < 42; i++)
	{
		if (Days[i].DayEqual(m_TimeSelectedDay))
		{
			if (FnCalCellClick)
				FnCalCellClick(Days[i]);
			return;
		}
	}
}

void PSCalendar::SelectDay(SYSTEMTIME& ST)
{
	ZeroMemory(&m_TimeSelectedDay, sizeof(SYSTEMTIME));

	m_TimeSelectedDay.wDay = ST.wDay;
	m_TimeSelectedDay.wMonth = ST.wMonth;
	m_TimeSelectedDay.wYear = ST.wYear;

	//get number of week day
	FILETIME FT;
	SystemTimeToFileTime(&m_TimeSelectedDay, &FT);
	FileTimeToSystemTime(&FT, &m_TimeSelectedDay); //(0 - sunday, 1 - monday, etc...)

	m_TimeSelectedDay.wHour = 0;
	m_TimeSelectedDay.wMinute = 0;
	m_TimeSelectedDay.wSecond = 0;
	m_TimeSelectedDay.wMilliseconds = 0;

	ShowAt(ST.wMonth, ST.wYear);
	FireCellClickEvent();

	if (FnNavButtonClick)
		FnNavButtonClick();
}

void PSCalendar::SelectDayToday()
{
	SelectDay(m_TimeToday);
}

SYSTEMTIME& PSCalendar::GetSelectedDay()
{
	return m_TimeSelectedDay;
}

bool PSCalendar::SelectedDayIsToday()
{
	return 
		(m_TimeSelectedDay.wDay == m_TimeToday.wDay) && 
		(m_TimeSelectedDay.wMonth == m_TimeToday.wMonth) && 
		(m_TimeSelectedDay.wYear == m_TimeToday.wYear);
}

void PSCalendar::InitializeAll()
{
	FireCellClickEvent();

	if (FnNavButtonClick)
		FnNavButtonClick();
}

void PSCalendar::SetNoteForDay(SYSTEMTIME& stDay, bool Note)
{
	for (int i = 0; i < 42; i++)
	{
		if (Days[i].DayEqual(stDay))
			Days[i].Style.Note = Note;
	}
	RedrawWindow();
}

void PSCalendar::NoteRangeListClear()
{
	listDateRange.clear();
}

void PSCalendar::NoteRangeListAdd(SYSTEMTIME& stFrom, SYSTEMTIME& stTo)
{
	DateRange DR;
	DR.stFrom = stFrom;
	DR.stTo = stTo;
	listDateRange.push_back(DR);
}

void PSCalendar::SetNotes()
{
	for (int i = 0; i < 42; i++)
		Days[i].Style.Note = false;
	for (int i = 0; i < 42; i++)
	{
		for (std::list<DateRange>::iterator a = listDateRange.begin(); a != listDateRange.end(); a++)
		{
			DateRange& DR = *a;
			if (Days[i].DayInRange(DR.stFrom, DR.stTo))
				Days[i].Style.Note = true;
		}
	}
	if (GetSafeHwnd())
		RedrawWindow();
}

void PSCalendar::OnRButtonUp(UINT nFlags, CPoint point)
{
	//Test for day cells
	for (int i = 0; i < 42; i++)
	{
		if (Days[i].Rect.PtInRect(point))
		{
			SYSTEMTIME stDay = GetDayForRightButton(Days[i].GetSysTime());
			FireCellRightButtonClickEvent(stDay);
			return;
		}
	}
	CStatic::OnRButtonUp(nFlags, point);
}

SYSTEMTIME PSCalendar::GetDayForRightButton(SYSTEMTIME& ST)
{
	SYSTEMTIME stDay;
	ZeroMemory(&stDay, sizeof(SYSTEMTIME));

	stDay.wDay = ST.wDay;
	stDay.wMonth = ST.wMonth;
	stDay.wYear = ST.wYear;

	//get number of week day
	FILETIME FT;
	SystemTimeToFileTime(&stDay, &FT);
	FileTimeToSystemTime(&FT, &stDay); //(0 - sunday, 1 - monday, etc...)

	stDay.wHour = 0;
	stDay.wMinute = 0;
	stDay.wSecond = 0;
	stDay.wMilliseconds = 0;

	return stDay;
}

void PSCalendar::FireCellRightButtonClickEvent(SYSTEMTIME& ST)
{
	for (int i = 0; i < 42; i++)
	{
		if (Days[i].DayEqual(ST))
		{
			if (FnCalCellRightClick)
				FnCalCellRightClick(Days[i]);
			return;
		}
	}
}
