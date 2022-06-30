//
// PSToolTip
// Copyright by Piotr Zagawa. 2003 All rights reserved.
// Wszystkie prawa zastrze¿one
//
// ver.1.1:	09-01-2004
//

#pragma once

#include "afxcmn.h"
#include "afxwin.h"

class PSToolTip
{
	CToolTipCtrl	m_ToolTip;
	CWnd*					m_pWnd;

	void AddRect(LPCTSTR lpText, LPRECT Rect = NULL, int ID = 1);
	void RemoveRect(int ID = 1);
public:
	PSToolTip(void);
	~PSToolTip(void);

	void CheckMsgLoop(MSG* pMsg);

	/*
		ENABLE NOTIFY for pWnd CONTROL !!!
	*/
	void Set(CWnd* pWnd, bool EntireControlRectangle = true);

	void Update(LPCTSTR lpText, LPRECT Rect = NULL, int ID = 1);
	void Text(LPCTSTR lpText, int ID = 1);
};
