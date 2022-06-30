// PSCuteClock.cpp : implementation file
//

#include "PSCuteClock.h"

// PSCuteClock

IMPLEMENT_DYNAMIC(PSCuteClock, CStatic)

struct ZodiacItem
{
	int		iImageIdx;
	char*	Name;
	char*	sDateB;
	char*	sDateE;
};

const	ZodiacItem ZodiacSigns[] =
{
	11,	"Capricorn",		"01.01",	"19.01",
	0,	"Aquarius",			"20.01",	"18.02",
	1,	"Pisces",				"19.02",	"20.03",
	2,	"Aries",				"21.03",	"19.04",
	3,	"Taurus",				"20.04",	"20.05",
	4,	"Gemini",				"21.05",	"21.06",
	5,	"Cancer",				"22.06",	"22.07",
	6,	"Leo",					"23.07",	"22.08",
	7,	"Virgo",				"23.08",	"22.09",
	8,	"Libra",				"23.09",	"22.10",
	9,	"Scorpio",			"23.10",	"21.11",
	10,	"Sagittarius",	"22.11",	"21.12",
	11,	"Capricorn",		"22.12",	"31.12",
	-1,	NULL,	NULL, NULL, NULL,
};

const int iDigitW = 12;
const int iDigitH = 19;
const int iDigitSmallW = 10;
const int iDigitSmallH = 15;

const	int iClockW = 90;
const	int iClockH = 19;
const	int iClockMargin = 6;

const int iZodiacW	= 33;
const int iZodiacH	= 27;

const int	iAmW = 18;
const int iAmH = 10;

//Clock size
const	int	iDefW = 228;
const int iDefH = 78;

const	CSize iDefSize(iDefW, iDefH);


PSCuteClock::PSCuteClock()
{
	iStyle = 0;

	bmLCD.LoadBitmap(IDB_CUTECLOCK_LCD);
	bmLCDsmall.LoadBitmap(IDB_CUTECLOCK_LCDSMALL);
	bmZodiac.LoadBitmap(IDB_CUTECLOCK_ZODIAC);
	bmAM.LoadBitmap(IDB_CUTECLOCK_AM);

	//create fonts
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(fFontLog), &fFontLog);

	fFontLog.lfQuality = NONANTIALIASED_QUALITY;
	fSysFont.CreateFontIndirect(&fFontLog);

	fFontLog.lfQuality = NONANTIALIASED_QUALITY;
	fFontLog.lfWeight = FW_BOLD;
	fSysFontBold.CreateFontIndirect(&fFontLog);

	//CDC* tmpDC = GetDC();
	CDC* tmpDC = NULL;
	CWnd* pDesktop = GetDesktopWindow();
	if (pDesktop)
		tmpDC = GetDesktopWindow()->GetDC();

	iFontSpace = GetTextCharacterExtra(tmpDC->m_hDC);
	strcpy(fFontLog.lfFaceName, "Arial");
	fFontLog.lfQuality = ANTIALIASED_QUALITY;
	fFontLog.lfHeight = -MulDiv(14, GetDeviceCaps(tmpDC->m_hDC, LOGPIXELSY), 72);
	fSysFontBoldSize1.CreateFontIndirect(&fFontLog);

	fFontLog.lfHeight = -MulDiv(12, GetDeviceCaps(tmpDC->m_hDC, LOGPIXELSY), 72);
	fSysFontBoldSize2.CreateFontIndirect(&fFontLog);

	//ReleaseDC(tmpDC);

	Refresh();

	for (int i = 0; i < 7; i++)
		FreeDays[i] = false;
	FreeDays[5] = true;
	FreeDays[6] = true;

	b24Clock = true;
	bDisplayPoints = true;
	SetZodiacLocale("");
}

PSCuteClock::~PSCuteClock()
{
}


BEGIN_MESSAGE_MAP(PSCuteClock, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// PSCuteClock message handlers

void DrawTestRect(CDC& dc, RECT& R)
{
	CBrush* pDefBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(&R);
	dc.SelectObject(pDefBrush);
}

void PSCuteClock::OnPaint()
{
	CPaintDC dc(this);

	if (iStyle == 0)
		DrawStyle_New(dc);
	if (iStyle == 1)
		DrawStyle_Old(dc);
	if (iStyle == 2)
		DrawStyle_AnalogClock(dc);
}

void PSCuteClock::DrawZodiacSign(CDC& dcDest, CDC& dcSrc, int Index, int iPosY)
{
	dcDest.BitBlt(iClockMargin, iPosY, iZodiacW - 1, iZodiacH - 1, &dcSrc, ((iZodiacW) * Index) + 1, 1, SRCCOPY);
}

int PSCuteClock::GetCurrentZodiac()
{
	SYSTEMTIME stNew = ST;
	stNew.wDayOfWeek = 0;
	stNew.wHour = 0;
	stNew.wMinute = 0;
	stNew.wSecond = 0;
	stNew.wMilliseconds = 0;
	
	CTime CurrTime(stNew, 0);
	int i = 0;
	while (ZodiacSigns[i].iImageIdx != -1)
	{
		std::string sDateB = ZodiacSigns[i].sDateB;
		std::string sDateE = ZodiacSigns[i].sDateE;

		int iDayBegin = atoi(sDateB.substr(0, 2).c_str());
		int iMonthBegin = atoi(sDateB.substr(3, 2).c_str());
		int iDayEnd = atoi(sDateE.substr(0, 2).c_str());
		int iMonthEnd = atoi(sDateE.substr(3, 2).c_str());

		CTime TimeBegin(ST.wYear, iMonthBegin, iDayBegin, 0, 0, 0, 0);
		CTime TimeEnd(ST.wYear, iMonthEnd, iDayEnd, 0, 0, 0, 0);

		if ((CurrTime >= TimeBegin) && (CurrTime <= TimeEnd))
			return i;
		i++;
	}
	return -1;
}

void PSCuteClock::DrawClock(CDC& dcDest, CDC& dcSrc, CDC& dcSrcSmall, int iX, int iY)
{
	int iYSmall = iDigitH - iDigitSmallH + iY;
	int iPosX = iX;

	rtClock.top = iY;
	rtClock.bottom = rtClock.top + iDigitH;
	rtClock.left = iPosX;

	std::string S = GetLocalizedTime((b24Clock)?"HH':'mm':'ss":"hh':'mm':'ss", &ST);

	int iHourA = 0;
	int iHourB = 0;
	int iMinuteA = 0;
	int iMinuteB = 0;
	int iSecondA = 0;
	int iSecondB = 0;

	if ((int)S.length() == 8)
	{
		char Buff[2] = {0,0};
		Buff[0] = S[0];	iHourA = atoi(Buff);
		Buff[0] = S[1];	iHourB = atoi(Buff);
		Buff[0] = S[3];	iMinuteA = atoi(Buff);
		Buff[0] = S[4];	iMinuteB = atoi(Buff);
		Buff[0] = S[6];	iSecondA = atoi(Buff);
		Buff[0] = S[7];	iSecondB = atoi(Buff);
	}

	//Drawing only if not zero
	if (iHourA)
	{
		dcDest.BitBlt(iPosX, iY, iDigitW, iDigitH, &dcSrc, ((iDigitW + 1) * iHourA) + 1, 1, SRCCOPY);
	}

	iPosX += iDigitW + 2;

	dcDest.BitBlt(iPosX, iY, iDigitW, iDigitH, &dcSrc, ((iDigitW + 1) * iHourB) + 1, 1, SRCCOPY);
		iPosX += iDigitW + 2;

	if (bDisplayPoints)
	{
		dcDest.BitBlt(iPosX, iY, iDigitW - 8, iDigitH, &dcSrc, ((iDigitW + 1) * 10) + 1 + 4, 1, SRCCOPY);
	}

	iPosX += iDigitW - 8 + 2;

	dcDest.BitBlt(iPosX, iY, iDigitW, iDigitH, &dcSrc, ((iDigitW + 1) * iMinuteA) + 1, 1, SRCCOPY);
		iPosX += iDigitW + 2;
	dcDest.BitBlt(iPosX, iY, iDigitW, iDigitH, &dcSrc, ((iDigitW + 1) * iMinuteB) + 1, 1, SRCCOPY);
		iPosX += iDigitW + 2;

	dcDest.BitBlt(iPosX, iY, iDigitW - 8, iDigitH, &dcSrc, ((iDigitW + 1) * 11) + 1 + 4, 1, SRCCOPY);
		iPosX += iDigitW - 8 + 2;

	dcDest.BitBlt(iPosX, iYSmall, iDigitSmallW, iDigitSmallH, &dcSrcSmall, ((iDigitSmallW + 1) * iSecondA) + 1, 1, SRCCOPY);
		iPosX += iDigitSmallW + 2;
	dcDest.BitBlt(iPosX, iYSmall, iDigitSmallW, iDigitSmallH, &dcSrcSmall, ((iDigitSmallW + 1) * iSecondB) + 1, 1, SRCCOPY);
		iPosX += iDigitSmallW + 2;

	rtClock.right = iPosX - 2;
}

void PSCuteClock::Refresh()
{
	GetLocalTime(&ST);
	bDisplayPoints = ((ST.wMilliseconds >= 0) && (ST.wMilliseconds <= 500));
	static int iCurrentDay = 0;
	if (ST.wDay != iCurrentDay)
	{
		iZodiacIndex = GetCurrentZodiac();
	}
	if (GetSafeHwnd())
		RedrawWindow();
}

std::string PSCuteClock::GetLocalizedDate(LPCTSTR Format)
{
	char Buff[128];
	if (GetDateFormat(LOCALE_USER_DEFAULT, 0, &ST, Format, Buff, sizeof(Buff)) == 0)
		Buff[0] = 0;
	return Buff;
}

std::string PSCuteClock::GetLocalizedTime(LPCTSTR Format, LPSYSTEMTIME stData)
{
	char Buff[64];
	if (GetTimeFormat(LOCALE_USER_DEFAULT, 0, stData, Format, Buff, sizeof(Buff)) == 0)
		Buff[0] = 0;
	return Buff;
}

CSize& PSCuteClock::GetDefaultSize()
{
	return (CSize&)iDefSize;
}

//"Wodnik,Ryby,Baran,Byk,BliŸniêta,Rak,Lew,Panna,Waga,Skorpion,Strzelec,Kozioro¿ec";
//"Aquarius,Pisces,Aries,Taurus,Gemini,Cancer,Leo,Virgo,Libra,Scorpio,Sagittarius,Capricorn";
void PSCuteClock::SetZodiacLocale(std::string S)
{
	if (S.empty())
		S = "Aquarius,Pisces,Aries,Taurus,Gemini,Cancer,Leo,Virgo,Libra,Scorpio,Sagittarius,Capricorn";

	sZodiacNamesList.clear();
	int iStart = 0;
	while (true)
	{
		int iPos = (int)S.find(",", iStart);
		sZodiacNamesList.push_back(S.substr(iStart, iPos - iStart));
		iStart = iPos + 1;
		if (iPos == std::string.npos)
			break;
	}
}

std::string PSCuteClock::GetZodiacLocalizedName()
{
	int iItemIndex = ZodiacSigns[iZodiacIndex].iImageIdx;
	if ((int)sZodiacNamesList.size() > iItemIndex)
		return sZodiacNamesList[iItemIndex];
	return "";
}

CString PSCuteClock::ZodiacDateInfo()
{
	CString sZodiacInfo;

	char* sDB = ZodiacSigns[iZodiacIndex].sDateB;
	char* sDE = ZodiacSigns[iZodiacIndex].sDateE;

	if (iZodiacIndex == 0)
		sDB = ZodiacSigns[12].sDateB;
	if (iZodiacIndex == 12)
		sDE = ZodiacSigns[0].sDateE;

	sZodiacInfo.Format("%s - %s", sDB, sDE);
	return sZodiacInfo;
}

bool	PSCuteClock::IsWeekend()
{
	//change days numbers
	int DayOfWeek = ST.wDayOfWeek;
	if (DayOfWeek == 0)
	{
		DayOfWeek = 6;
	} else {
		DayOfWeek--;
	}
	return FreeDays[DayOfWeek];
}

void PSCuteClock::DrawStyle_Old(CDC& dc)
{
	CRect R;
	GetClientRect(&R);
	CMemDC MemDC(&dc, &R);

	CPen*		pDefPen		= NULL;
	CBrush*	pDefBrush	= NULL;
	CFont*	pDefFont	= NULL;

	//Main background
	CBrush MainBkgBrush(GetSysColor(COLOR_3DFACE));
	CPen MainBkgPen(PS_SOLID, 1, GetSysColor(COLOR_3DFACE));
	pDefPen = MemDC.SelectObject(&MainBkgPen);
	pDefBrush = MemDC.SelectObject(&MainBkgBrush);
	MemDC.Rectangle(&R);
	MemDC.SelectObject(pDefPen);
	MemDC.SelectObject(pDefBrush);

	//Frame
	POINT P = {6,6};
	CBrush FrameBrush(RGB(255, 255, 255));
	CPen FramePen(PS_SOLID, 1, RGB(132, 146, 180));
	pDefPen = MemDC.SelectObject(&FramePen);
	pDefBrush = MemDC.SelectObject(&FrameBrush);
	MemDC.RoundRect(&R, P);
	MemDC.SelectObject(pDefPen);
	MemDC.SelectObject(pDefBrush);

	//Get text sizes
	pDefFont = MemDC.SelectObject(&fSysFontBoldSize1);
	std::string sTime = GetLocalizedTime((b24Clock)?"H':'mm'.'ss":"h':'mm'.'ss tt", &ST);
	CSize sTimeSize = MemDC.GetOutputTextExtent(sTime.c_str(), 1);
	MemDC.SelectObject(pDefFont);

	pDefFont = MemDC.SelectObject(&fSysFontBoldSize2);
	std::string sWeekDay = GetLocalizedDate("dddd");
	CSize sWeekDaySize = MemDC.GetOutputTextExtent(sWeekDay.c_str(), 1);
	MemDC.SelectObject(pDefFont);

	pDefFont = MemDC.SelectObject(&fSysFontBoldSize2);
	std::string sFullDate = GetLocalizedDate("d MMMM yyyy");
	CSize sFullDateSize = MemDC.GetOutputTextExtent(sFullDate.c_str(), 1);
	MemDC.SelectObject(pDefFont);

	//Set rectangles.
	int iLinesMargin = (R.Height() - (sTimeSize.cy + sWeekDaySize.cy + sFullDateSize.cy)) / 4;
	int iLinePosY = iLinesMargin;

	CRect rtTime(iClockMargin, iLinePosY, R.Width() - iClockMargin, iLinePosY + sTimeSize.cy);
	iLinePosY += iLinesMargin + sTimeSize.cy;
	CRect rtWeekDay(iClockMargin, iLinePosY, R.Width() - iClockMargin, iLinePosY + sWeekDaySize.cy);
	iLinePosY += iLinesMargin + sWeekDaySize.cy;
	CRect rtFullDate(iClockMargin, iLinePosY, R.Width() - iClockMargin, iLinePosY + sFullDateSize.cy);

	//DrawTestRect(MemDC, rtTime);
	//DrawTestRect(MemDC, rtWeekDay);
	//DrawTestRect(MemDC, rtFullDate);

	MemDC.SetBkMode(TRANSPARENT);

	//Draw time
	MemDC.SetTextCharacterExtra(3);

	pDefFont = MemDC.SelectObject(&fSysFontBoldSize1);
	MemDC.SetTextColor(RGB(255, 128, 0));
	DrawOutline(MemDC, sTime, rtTime, DT_CENTER);
	MemDC.SetTextColor(RGB(255, 208, 160));
	MemDC.DrawText(sTime.c_str(), -1, rtTime, DT_CENTER);
	MemDC.SelectObject(pDefFont);

	MemDC.SetTextCharacterExtra(iFontSpace);

	//Draw week day
	pDefFont = MemDC.SelectObject(&fSysFontBoldSize2);
	MemDC.SetTextColor((IsWeekend())?RGB(255, 64, 64):RGB(88, 160, 220));
	MemDC.DrawText(sWeekDay.c_str(), -1, rtWeekDay, DT_CENTER);
	MemDC.SelectObject(pDefFont);

	//Draw full date
	pDefFont = MemDC.SelectObject(&fSysFontBoldSize2);
	MemDC.SetTextColor(RGB(64, 160, 64));
	MemDC.DrawText(sFullDate.c_str(), -1, rtFullDate, DT_CENTER);
	MemDC.SelectObject(pDefFont);
}

void PSCuteClock::DrawOutline(CDC& dc, std::string S, CRect R, UINT uFormat)
{
	CRect tmp = R;

	int iWidth = 1;

	R.OffsetRect(-iWidth, 0);
	dc.DrawText(S.c_str(), -1, R, uFormat);
	R.OffsetRect((iWidth << 1), 0);
	dc.DrawText(S.c_str(), -1, R, uFormat);
	R = tmp;

	R.OffsetRect(0, -iWidth);
	dc.DrawText(S.c_str(), -1, R, uFormat);
	R.OffsetRect(0, (iWidth << 1));
	dc.DrawText(S.c_str(), -1, R, uFormat);
	R = tmp;

	R.OffsetRect(-iWidth, -iWidth);
	dc.DrawText(S.c_str(), -1, R, uFormat);
	R.OffsetRect((iWidth << 1), (iWidth << 1));
	dc.DrawText(S.c_str(), -1, R, uFormat);
	R = tmp;

	R.OffsetRect(iWidth, -iWidth);
	dc.DrawText(S.c_str(), -1, R, uFormat);
	R.OffsetRect(-(iWidth << 1), (iWidth << 1));
	dc.DrawText(S.c_str(), -1, R, uFormat);
	R = tmp;
}

void PSCuteClock::DrawStyle_New(CDC& dc)
{
	CRect R;
	GetClientRect(&R);
	CMemDC MemDC(&dc, &R);

	CPen*		pDefPen		= NULL;
	CBrush*	pDefBrush	= NULL;
	CFont*	pDefFont	= NULL;

	CSize TextSize = MemDC.GetOutputTextExtent("A", 1);

	//Main background
	CBrush MainBkgBrush(GetSysColor(COLOR_3DFACE));
	CPen MainBkgPen(PS_SOLID, 1, GetSysColor(COLOR_3DFACE));
	pDefPen = MemDC.SelectObject(&MainBkgPen);
	pDefBrush = MemDC.SelectObject(&MainBkgBrush);
	MemDC.Rectangle(&R);
	MemDC.SelectObject(pDefPen);
	MemDC.SelectObject(pDefBrush);

	//Frame
	CBrush FrameBrush(RGB(255, 255, 255));
	CPen FramePen(PS_SOLID, 1, RGB(132, 146, 180));
	pDefPen = MemDC.SelectObject(&FramePen);
	pDefBrush = MemDC.SelectObject(&FrameBrush);
	POINT P = {6,6};
	MemDC.RoundRect(&R, P);
	MemDC.SelectObject(pDefPen);
	MemDC.SelectObject(pDefBrush);

	//Draw LCD clock
	int iAmSpaceWidth = (b24Clock)?0:(iClockMargin + iAmW);

	CDC bmDC;
	CDC bmDCsmall;
	bmDC.CreateCompatibleDC(&dc);
	bmDCsmall.CreateCompatibleDC(&dc);
	CBitmap* pDefBmpLCD = bmDC.SelectObject(&bmLCD);
	CBitmap* pDefBmpLCDsmall = bmDCsmall.SelectObject(&bmLCDsmall);
	DrawClock(MemDC, bmDC, bmDCsmall, R.Width() - iClockW - iClockMargin - iAmSpaceWidth, iClockMargin);
	bmDCsmall.SelectObject(pDefBmpLCDsmall);
	bmDC.SelectObject(pDefBmpLCD);
	//DrawTestRect(MemDC, rtClock);

	//Draw am/pm image
	if (!b24Clock)
	{
		CDC bmAMDC;
		bmAMDC.CreateCompatibleDC(&dc);
		CBitmap* pDefBmpAM = bmAMDC.SelectObject(&bmAM);
		int iAmYPos = iClockMargin + iDigitH - iAmH + 1;
		int iAmIndex = 0;
		if (ST.wHour > 12)
			iAmIndex = 1;
		MemDC.BitBlt(R.Width() - iAmW - iClockMargin, iAmYPos, iAmW - 1, iAmH - 1, &bmAMDC, ((iAmW) * iAmIndex) + 1, 1, SRCCOPY);
		bmAMDC.SelectObject(pDefBmpAM);
	}

	int iLinePosY = iClockMargin + iClockH + iClockMargin;

	//Draw h line
	CPen LinePen(PS_SOLID, 1, RGB(255, 220, 184));
	pDefPen = MemDC.SelectObject(&LinePen);
	MemDC.MoveTo(iClockMargin, iLinePosY);
	MemDC.LineTo(R.Width() - iClockMargin, iLinePosY);
	MemDC.SelectObject(pDefPen);

	//Draw zodiac other text
	pDefFont = MemDC.SelectObject(&fSysFont);
	CRect rtZodiacText(iClockMargin + iZodiacW + iClockMargin, iLinePosY + iClockMargin, 150, iLinePosY + TextSize.cy + iClockMargin);
	MemDC.SetTextColor(RGB(128, 128, 192));
	MemDC.DrawText(ZodiacDateInfo(), -1, rtZodiacText, DT_END_ELLIPSIS | DT_LEFT);
	MemDC.SelectObject(pDefFont);
	//DrawTestRect(MemDC, rtZodiacText);

	//Draw zodiac localized text
	pDefFont = MemDC.SelectObject(&fSysFont);
	CRect rtZodiacTextLoc(iClockMargin + iZodiacW + iClockMargin, R.Height() - iClockMargin - TextSize.cy, 150, R.Height() - iClockMargin);
	MemDC.SetTextColor(RGB(192, 128, 192));
	MemDC.DrawText(GetZodiacLocalizedName().c_str(), -1, rtZodiacTextLoc, DT_END_ELLIPSIS | DT_LEFT);
	MemDC.SelectObject(pDefFont);
	//DrawTestRect(MemDC, rtZodiacTextLoc);

	//Draw zodiac image
	CDC bmZodiacDC;
	bmZodiacDC.CreateCompatibleDC(&dc);
	CBitmap* pDefBmpZodiac = bmZodiacDC.SelectObject(&bmZodiac);
	int iZodiacY = ((R.Height() - iLinePosY) >> 1) + ((iClockMargin + iZodiacH + iClockMargin) >> 1);
	DrawZodiacSign(MemDC, bmZodiacDC, ZodiacSigns[iZodiacIndex].iImageIdx, iZodiacY);
	bmDC.SelectObject(pDefBmpZodiac);

	//Draw week day
	pDefFont = MemDC.SelectObject(&fSysFontBold);
	CRect rtWeekDayText(iClockMargin, iLinePosY + iClockMargin, R.Width() - iClockMargin, iLinePosY + TextSize.cy + iClockMargin);
	MemDC.SetTextColor((IsWeekend())?RGB(255, 64, 64):RGB(88, 160, 220));
	MemDC.DrawText(GetLocalizedDate("dddd").c_str(), -1, rtWeekDayText, DT_END_ELLIPSIS | DT_RIGHT);
	MemDC.SelectObject(pDefFont);
	//DrawTestRect(MemDC, rtWeekDayText);

	//Draw full date
	pDefFont = MemDC.SelectObject(&fSysFontBold);
	CRect rtFullDateText(iClockMargin, R.Height() - iClockMargin - TextSize.cy, R.Width() - iClockMargin, R.Height() - iClockMargin);
	MemDC.SetTextColor(RGB(64, 160, 64));
	MemDC.DrawText(GetLocalizedDate("d MMMM yyyy").c_str(), -1, rtFullDateText, DT_END_ELLIPSIS | DT_RIGHT);
	MemDC.SelectObject(pDefFont);
	//DrawTestRect(MemDC, rtFullDateText);
}

void PSCuteClock::DrawStyle_AnalogClock(CDC& dc)
{
	CRect R;
	GetClientRect(&R);
	CMemDC MemDC(&dc, &R);

	CPen*		pDefPen		= NULL;
	CBrush*	pDefBrush	= NULL;
	CFont*	pDefFont	= NULL;

	//Main background
	CBrush MainBkgBrush(GetSysColor(COLOR_3DFACE));
	CPen MainBkgPen(PS_SOLID, 1, GetSysColor(COLOR_3DFACE));
	pDefPen = MemDC.SelectObject(&MainBkgPen);
	pDefBrush = MemDC.SelectObject(&MainBkgBrush);
	MemDC.Rectangle(&R);
	MemDC.SelectObject(pDefPen);
	MemDC.SelectObject(pDefBrush);

	//Frame
	POINT P = {6,6};
	CBrush FrameBrush(RGB(255, 255, 255));
	CPen FramePen(PS_SOLID, 1, RGB(132, 146, 180));
	pDefPen = MemDC.SelectObject(&FramePen);
	pDefBrush = MemDC.SelectObject(&FrameBrush);
	MemDC.RoundRect(&R, P);
	MemDC.SelectObject(pDefPen);
	MemDC.SelectObject(pDefBrush);

	//Clock
	CRect rtClock = R;
	rtClock.top += iClockMargin;
	rtClock.bottom -= iClockMargin;
	rtClock.left = iClockMargin;
	rtClock.right = rtClock.left + rtClock.Height();

	CBrush ClockBrush(RGB(255, 224, 192));
	CPen ClockPen(PS_SOLID, 1, RGB(255, 160, 64));
	pDefPen = MemDC.SelectObject(&ClockPen);
	pDefBrush = MemDC.SelectObject(&ClockBrush);
	MemDC.Ellipse(&rtClock);
	MemDC.SelectObject(pDefPen);
	MemDC.SelectObject(pDefBrush);
	//DrawTestRect(MemDC, rtClock);

	/*
	//Points
	CPen PointPen(PS_SOLID, 1, RGB(0, 0, 0));
	pDefPen = MemDC.SelectObject(&PointPen);

	int iCenterX = rtClock.CenterPoint().x;
	int iCenterY = rtClock.CenterPoint().y;
	int iRadius = (rtClock.Width() >> 1);

	for (int i = 0; i < 360; i++)
	{
		if (!(i % 30))
		{
			MemDC.MoveTo(rtClock.CenterPoint());
			MemDC.AngleArc(iCenterX, iCenterY, iRadius, (float)i, 0);
		}
	}
	MemDC.SelectObject(pDefPen);

	//Bkg
	CPen BkgPen(PS_NULL, 1, RGB(0, 0, 0));
	pDefPen = MemDC.SelectObject(&BkgPen);
	pDefBrush = MemDC.SelectObject(&ClockBrush);
	CRect rtBkg = rtClock;
	rtBkg.DeflateRect(1, 1);
	MemDC.Ellipse(&rtBkg);
	MemDC.SelectObject(pDefPen);
	MemDC.SelectObject(pDefBrush);
	*/


}
