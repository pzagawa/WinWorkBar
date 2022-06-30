#pragma once

#include <string>

#include "afxcmn.h"
#include "afxwin.h"

#include "MemDC.h"

// PSLabel

class PSLabel : public CStatic
{
	DECLARE_DYNAMIC(PSLabel)
public:
	PSLabel();
	virtual ~PSLabel();

	enum	Align {alLeft, alRight, alCenter};
protected:
	DECLARE_MESSAGE_MAP()
private:
	std::string	m_S;
	Align				m_A;
	COLORREF		m_FontColor;
	COLORREF		m_BkgColor;
	bool				m_BkgColorEnabled;
	int					m_Outline;
	COLORREF		m_OutlineColor;
	CFont				m_Font;
	LOGFONT			m_FontLog;
	int					m_FontSpace;

	void DoCreateFont();
public:
	afx_msg void OnPaint();

	void SetDefaultFont();

	void SetText(const std::string& S);
	const std::string& PSLabel::GetText();

	void SetAlign(Align A);
	void SetFontName(std::string Name);
	void SetFontStyle(bool B, bool I, bool U);
	void SetFontSize(int Size);
	void SetFontSpace(int Size);
	void SetFontColor(COLORREF Color);
	void SetBkgColor(bool Enable, COLORREF Color);
	void SetOutline(int Size, COLORREF Color);
	void DrawOutline(CDC& dc, CRect& Rect, std::string& S, UINT Format);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


