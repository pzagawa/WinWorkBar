// WinWorkBar.h : main header file for the PROJECT_NAME application
//

#pragma once

#include "resource.h"		// main symbols
#include "default.h"

#include "CMessageOnlyWindow.h"
#include "Libs\PSOneInstance.h"

// CWinWorkBarApp:
// See WinWorkBar.cpp for the implementation of this class
//

class CWinWorkBarApp : public CWinApp
{
public:
	CWinWorkBarApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CWinWorkBarApp theApp;