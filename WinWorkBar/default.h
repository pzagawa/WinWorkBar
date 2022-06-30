// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <string>

#include "Libs\PSTinyXML.h"

//Work bar global defaults
const	int DefaultWidth	= 260;
const	int MaxWidth			= 800;
const	int DefaultMargin	= 6;					//margines dla np. grupa -> formularz, grupa -> Tab
const	int DefaultTabBarHeight	= 30;		//wysokoœæ paska przycisków nad list¹ w zak³adkach NOTES i TASKS
const	int DefaultTabBtnWidth	= 32;		//szerokoœæ przycisku
const	int DefaultTabBarMargin	= 2;		//margines dla paska przycisków nad list¹
const	int ListDateColumnWidth	= 90;
const	int ListDateTimeColumnWidth	= 130;

const int ButtonHeight = 22;					//wymiary przycisków okien dialogowych
const int ButtonWidth = 80;
const int ButtonWidthSmall = 24;

const int ControlLineHeight = ButtonHeight + DefaultMargin + DefaultMargin - 1;

const	POINT MinDialogSize = { 490, 400 };


//Note data structure
struct dlgData_note
{
	bool				bNewNote;
	std::string sID;
	std::string	sText;
	DWORD				dwCategories;
	SYSTEMTIME	DateFrom;
	SYSTEMTIME	DateTo;
	dlgData_note()
	{
		bNewNote = true;
		dwCategories = 0;
		memset(&DateFrom, 0, sizeof(SYSTEMTIME));
		memset(&DateTo, 0, sizeof(SYSTEMTIME));
	}
	bool DateFromEmpty() { return DateFrom.wYear == 0; };
	bool DateToEmpty() { return DateTo.wYear == 0; };
};


//Task data structure
struct dlgData_task
{
	bool				bNewTask;
	std::string sID;
	std::string	sText;
	DWORD				dwCategories;
	DWORD				dwRepeats;
	SYSTEMTIME	DateStart;
	SYSTEMTIME	DateStop;
	int					iPriority;
	int					iProgress;
	dlgData_task()
	{
		bNewTask = true;
		dwCategories = 0;
		dwRepeats = 0;
		memset(&DateStart, 0, sizeof(SYSTEMTIME));
		memset(&DateStop, 0, sizeof(SYSTEMTIME));
		iPriority = 0;
		iProgress = 0;
	}
	bool DateStartEmpty() { return DateStart.wYear == 0; };
	bool DateStopEmpty() { return DateStop.wYear == 0; };
};


//Data view filter
const std::string dvfFormat = "%0.4x_%d%d%d%d%d";

struct DataViewFilter
{
	DWORD dwSelectedCategories;
	int iProgress;
	int iPriority;
	int iVisibility;
	int iAdded;
	int iModified;
	DataViewFilter()
	{
		Reset();
	}
	void Reset()
	{
		dwSelectedCategories = 0;
		iProgress = 0;
		iPriority = 0;
		iVisibility = 0;
		iAdded = 0;
		iModified = 0;
	}
	void Set(std::string S)
	{
		Reset();
		if (S.length() == 10)
		{
			std::string N = S.substr(0, 4);
			dwSelectedCategories = PSXML::HexToInt(N);
			iProgress = PSXML::StrToInt(S.substr(5, 1));
			iPriority = PSXML::StrToInt(S.substr(6, 1));
			iVisibility = PSXML::StrToInt(S.substr(7, 1));
			iAdded = PSXML::StrToInt(S.substr(8, 1));
			iModified = PSXML::StrToInt(S.substr(9, 1));
		}
	}
	std::string Get()
	{
		CString S;
		S.Format(dvfFormat.c_str(), dwSelectedCategories, iProgress, iPriority, iVisibility, iAdded, iModified);
		return (LPCTSTR)S;
	}
	bool All() { return ((dwSelectedCategories + iProgress + iPriority + iVisibility + iAdded + iModified) == 0); };
	bool Equal(DataViewFilter& dvf)
	{
		return ((dvf.dwSelectedCategories == dwSelectedCategories) &&
			(dvf.iProgress == iProgress) && (dvf.iPriority == iPriority) &&
			(dvf.iVisibility == iVisibility) && (dvf.iAdded == iAdded) && (dvf.iModified == iModified));
	}
};

