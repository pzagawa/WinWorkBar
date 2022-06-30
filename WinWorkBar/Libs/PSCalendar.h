#pragma once

#include <string>

#include "MemDC.h"

#include "afxcmn.h"
#include "afxwin.h"

#include <list>

// PSCalendar

//Cell
//-----------------------------------------
enum PSCalendarCellState
{
	ccsNone,
	ccsDefault,				//dzieñ bie¿¹cego miesi¹ca
	ccsOtherMonth,		//dzieñ innego miesi¹ca
	ccsCurrentDay,		//bie¿¹cy dzieñ
	ccsDayOfWeek			//nag³ówek - dzieñ tygodnia
};

class PSCalendarCellStyle
{
public:
	PSCalendarCellState	State;
	bool Note;
	bool Freeday;
	PSCalendarCellStyle()
	{
		State = ccsDefault;
		Note = false;
		Freeday = false;
	}
};

class PSCalendar;

class PSCalendarCell
{
public:
	bool	MouseOver;
	int		Margin;
	int		Space;
	CRect	Rect;
	PSCalendarCellStyle	Style;

	int		Month;
	int		Year;
	int		Day;

	std::string	ValueString;
	char	ValueBuff[10];

	PSCalendarCell()
	{
		MouseOver = false;
		Day = 0;
		Month = 0;
		Year = 0;
		ValueString = "";
		ValueBuff[0] = 0;
		Margin = 1;
		Space = 2;
		Style.State = ccsNone;
	}
	void Paint(CDC& dc, PSCalendar& Calendar);
	char* DayAsString()
	{
		itoa(Day, ValueBuff, 10);
		return ValueBuff;
	}
	std::string DateAsString()
	{
		char B[20];
		itoa(Day, B, 10);
		std::string S = B;
		S += "-";
		itoa(Month, B, 10);
		S += B;
		S += "-";
		itoa(Year, B, 10);
		S += B;
		return S;
	}
	bool DayEqual(SYSTEMTIME& ST)
	{
		return ((ST.wDay == Day) && (ST.wMonth == Month) && (ST.wYear == Year));
	}

	bool DayInRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd)
	{
		CTime ctFrom(stBegin);
		CTime ctTo(stEnd);
		CTime ctNow(Year, Month, Day, 0, 0, 0);
		return ((ctNow >= ctFrom) && (ctNow <= ctTo));
	}

	SYSTEMTIME& GetSysTime()
	{
		static SYSTEMTIME ST;
		ZeroMemory(&ST, sizeof(ST));
		ST.wDay = Day; ST.wMonth = Month; ST.wYear = Year;
		return ST;
	}

	void SetSysTime(SYSTEMTIME& ST)
	{
		ST.wYear = Year;
		ST.wMonth = Month;
		ST.wDay = Day;
	}
	void TestCursorPos(CWnd& Dlg);
};


struct	PSCalendarElement
{
	CRect	R;
	bool	MouseOver;
	PSCalendarElement()
	{
		MouseOver = false;
	}
	void TestCursorPos(CWnd& Dlg);
};


//Main
//-----------------------------------------
typedef	void (*eventOnCalCellClick)(PSCalendarCell& CalCell);
typedef	void (*eventOnCalCellRightClick)(PSCalendarCell& CalCell);
typedef	void (*eventOnNavButtonClick)();


class PSCalendar : public CStatic
{
	DECLARE_DYNAMIC(PSCalendar)

public:
	PSCalendar();
	virtual ~PSCalendar();

protected:
	DECLARE_MESSAGE_MAP()

private:
	CFont				m_Font_Normal;
	CFont				m_Font_Bold;
	CFont				m_Font_Underline;
	LOGFONT			m_FontLog;

	SYSTEMTIME	m_TimeToday;
	SYSTEMTIME	m_TimeSelectedDay;

	int					m_StartMonth;
	int					m_StartYear;

	PSCalendarElement	TitleBar;
	PSCalendarElement	TitleCaption;
	PSCalendarElement	btnPrev;
	PSCalendarElement	btnNext;
	PSCalendarElement	btnToday;

	PSCalendarCell	DaysOfWeek[7];
	PSCalendarCell	Days[42];

	struct	DateRange
	{
		SYSTEMTIME	stFrom;
		SYSTEMTIME	stTo;
	};

	std::list<DateRange>	listDateRange;

	void DoCreateFont(CFont& F);
	void GetLocalTime();
	void CalcRectangles();
	void CalcCalendarData();
	void PaintTitleBar(CDC& dc);
	void TestCursorPos();

	void FireCellClickEvent();
	void FireCellRightButtonClickEvent(SYSTEMTIME& ST);
	SYSTEMTIME GetDayForRightButton(SYSTEMTIME& ST);
public:
	bool FreeDays[7];

	bool bSimpleStyle;

	int	Title_RowHeight;
	int	WeekDay_RowHeight;
	int	Day_RowHeight;

	void	SetFreeDays(int Day, bool Free);

	void InitializeAll();			//CALL THIS IN OnInitDialog() !!!

	eventOnCalCellClick				FnCalCellClick;
	eventOnCalCellRightClick	FnCalCellRightClick;
	eventOnNavButtonClick			FnNavButtonClick;

	afx_msg void OnPaint();

	void SetDefaultFont();
	void SetFontName(CFont& F, std::string Name);
	void SetFontSize(CFont& F, int Size);
	void SetFontStyle(CFont& F, bool B, bool I, bool U);

	int	NumberDayOfWeek(SYSTEMTIME ST);
	int NumberDaysOfMonth(SYSTEMTIME ST);

	void UpdateToday(bool Redraw = true);
	void ShowAt(int Month = 0, int Year = 0);
	void ShowAtPrevMonth();
	void ShowAtNextMonth();

	int		StartMonth() { return m_StartMonth; };
	int		StartYear() { return m_StartYear; };

	void SetToday(SYSTEMTIME stToday) { m_TimeToday = stToday; };
	SYSTEMTIME	GetToday() { return m_TimeToday; };

	bool	IsWeekend();

	//Call this from parent, or subclass on init !!!
	void Resize()
	{ 
		CalcRectangles();
	};

	void SelectDay(SYSTEMTIME& ST);
	void SelectDayToday();
	bool SelectedDayIsToday();
	SYSTEMTIME& GetSelectedDay();

	friend	PSCalendarCell;

	void SetNoteForDay(SYSTEMTIME& stDay, bool Note);

	void SetNotes();
	void NoteRangeListClear();
	void NoteRangeListAdd(SYSTEMTIME& stFrom, SYSTEMTIME& stTo);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
