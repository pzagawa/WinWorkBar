//
// CuteClock 1.0
// 28-04-2004 © Piotr Zagawa
//

#pragma once

#include "resource.h"		// main symbols
#include "Libs\MemDC.h"
#include <afxwin.h>
#include <string>
#include <vector>

// PSCuteClock
class PSCuteClock : public CStatic
{
	CBitmap bmLCD;
	CBitmap bmLCDsmall;
	CBitmap bmZodiac;
	CBitmap bmAM;

	CRect		rtClock;
	CFont		fSysFont;
	CFont		fSysFontBold;
	CFont		fSysFontBoldSize1;
	CFont		fSysFontBoldSize2;

	int			iFontSpace;
	LOGFONT	fFontLog;
	int			iZodiacIndex;
	bool		bDisplayPoints;
	SYSTEMTIME	ST;
	std::vector<std::string>	sZodiacNamesList;

	void DrawClock(CDC& dcDest, CDC& dcSrc, CDC& dcSrcSmall, int iX, int iY);
	void DrawZodiacSign(CDC& dcDest, CDC& dcSrc, int Index, int iPosY);
	int GetCurrentZodiac();
	std::string GetLocalizedDate(LPCTSTR Format);
	std::string GetLocalizedTime(LPCTSTR Format, LPSYSTEMTIME stData);
	std::string GetZodiacLocalizedName();
	CString ZodiacDateInfo();
	bool	IsWeekend();

	void DrawOutline(CDC& dc, std::string S, CRect R, UINT uFormat);

	void DrawStyle_Old(CDC& dc);
	void DrawStyle_New(CDC& dc);
	void DrawStyle_AnalogClock(CDC& dc);

	DECLARE_DYNAMIC(PSCuteClock)
public:
	PSCuteClock();
	virtual ~PSCuteClock();

protected:
	DECLARE_MESSAGE_MAP()
public:
	//input data
	int			iStyle;
	bool		b24Clock;
	bool		FreeDays[7];

	void Refresh();
	void SetZodiacLocale(std::string S);

	CSize& GetDefaultSize();

	afx_msg void OnPaint();
};
