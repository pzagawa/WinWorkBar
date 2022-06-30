// PSLabel.cpp : implementation file
//

#include "PSLabel.h"


// PSLabel

IMPLEMENT_DYNAMIC(PSLabel, CStatic)
PSLabel::PSLabel()
{
	m_S = "";
	m_BkgColor = RGB(255, 255, 255);
	m_BkgColorEnabled = false;
	m_FontColor = RGB(0, 0, 0);
	m_A = alCenter;
	m_Outline = 0;
	SetDefaultFont();
	DoCreateFont();
}

PSLabel::~PSLabel()
{
}

BEGIN_MESSAGE_MAP(PSLabel, CStatic)
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// PSLabel message handlers

int PSLabel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetWindowText("");
	m_FontSpace = GetTextCharacterExtra((HDC)GetDC());

	return 0;
}

void PSLabel::SetDefaultFont()
{
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(m_FontLog),&m_FontLog);
}

void PSLabel::DoCreateFont()
{
	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&m_FontLog);
}

void PSLabel::SetText(const std::string& S)
{
	if(IsWindow(this->GetSafeHwnd())) 
	{
		m_S = S;
		RedrawWindow();
	}
}

const std::string& PSLabel::GetText()
{
	return m_S;
}

void PSLabel::SetAlign(Align A)
{
	m_A = A;
}

void PSLabel::SetFontName(std::string Name)
{
	lstrcpy(m_FontLog.lfFaceName, Name.c_str());
	DoCreateFont();
}

void PSLabel::SetFontSize(int Size)
{
	CFont cf;
	LOGFONT lf;

	cf.CreatePointFont(Size * 10, m_FontLog.lfFaceName);
	cf.GetLogFont(&lf);

	m_FontLog.lfHeight = lf.lfHeight;
	m_FontLog.lfWidth = lf.lfWidth;

	DoCreateFont();
}

void PSLabel::SetFontColor(COLORREF Color)
{
	m_FontColor = Color;
}

void PSLabel::SetBkgColor(bool Enable, COLORREF Color)
{
	m_BkgColorEnabled = Enable;
	m_BkgColor = Color;
}

void PSLabel::SetFontStyle(bool B, bool I, bool U)
{
	m_FontLog.lfWeight = (B)?FW_BOLD:FW_NORMAL;
	m_FontLog.lfItalic = (I)?TRUE:FALSE;
	m_FontLog.lfUnderline = (U)?TRUE:FALSE;
	DoCreateFont();
}

void PSLabel::SetOutline(int Size, COLORREF Color)
{
	m_Outline = Size;
	m_OutlineColor = Color;
}

void PSLabel::SetFontSpace(int Size)
{
	m_FontSpace = Size;
}

void PSLabel::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	CPaintDC DefDC(this); // device context for painting

	CRect R;
	GetClientRect(&R);
	CMemDC dc(&DefDC, &R);

	dc.SelectObject(&m_Font);
	dc.SetTextColor(m_FontColor);
	dc.SetBkColor(GetSysColor(COLOR_3DFACE));
	dc.SetTextCharacterExtra(m_FontSpace);

	UINT FormatAlign = alLeft;
	switch (m_A)
	{
	case	alLeft:
		FormatAlign = DT_LEFT;
		break;
	case	alRight:
		FormatAlign = DT_RIGHT;
		break;
	case	alCenter:
		FormatAlign = DT_CENTER;
		break;
	}

	UINT Format = DT_VCENTER | DT_SINGLELINE | FormatAlign;
	CRect Rect;
	GetClientRect(&Rect);
	if (m_BkgColorEnabled)
	{
		CBrush Bkg(m_BkgColor);
		dc.SetBkColor(m_BkgColor);
		dc.FillRect(&Rect, &Bkg);
	}

	dc.SetBkMode(TRANSPARENT);
	CBrush Bkg(GetSysColor(COLOR_3DFACE));
	dc.FillRect(&Rect, &Bkg);

	if (m_Outline == 0)
	{	
		dc.DrawText(m_S.c_str(), -1, &Rect, Format);
	} else {
		//drawing outline
		dc.SetTextColor(m_OutlineColor);
		DrawOutline(dc, Rect, m_S, Format);
		
		//drawing inner font
		dc.SetTextColor(m_FontColor);
		dc.DrawText(m_S.c_str(), -1, &Rect, Format);
	}
}

void PSLabel::DrawOutline(CDC& dc, CRect& Rect, std::string& S, UINT Format)
{
	//left
	CRect OR = Rect;
	OR.OffsetRect(-m_Outline, 0);
	dc.DrawText(m_S.c_str(), -1, &OR, Format);
	//right
	OR = Rect;
	OR.OffsetRect(m_Outline, 0);
	dc.DrawText(m_S.c_str(), -1, &OR, Format);
	//bottom
	OR = Rect;
	OR.OffsetRect(0, m_Outline);
	dc.DrawText(m_S.c_str(), -1, &OR, Format);
	//top
	OR = Rect;
	OR.OffsetRect(0, -m_Outline);
	dc.DrawText(m_S.c_str(), -1, &OR, Format);
	//top - left
	OR = Rect;
	OR.OffsetRect(-m_Outline, -m_Outline);
	dc.DrawText(m_S.c_str(), -1, &OR, Format);
	//top - right
	OR = Rect;
	OR.OffsetRect(m_Outline, -m_Outline);
	dc.DrawText(m_S.c_str(), -1, &OR, Format);
	//bottom - left
	OR = Rect;
	OR.OffsetRect(-m_Outline, m_Outline);
	dc.DrawText(m_S.c_str(), -1, &OR, Format);
	//bottom - right
	OR = Rect;
	OR.OffsetRect(m_Outline, m_Outline);
	dc.DrawText(m_S.c_str(), -1, &OR, Format);
}

