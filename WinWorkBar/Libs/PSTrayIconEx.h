//
// PSTrayInfo
// Tray icon tools
// Copyright by Piotr Zagawa. 2002 All rights reserved.
// Wszystkie prawa zastrze¿one
//
// ver.1.0:	23-07-2003
//

#pragma once


//*************************************************************************
//This is IMPORTANT !!!
//WITHOUT VALID VER SETTINGS, NOTIFYICONDATA structure has invalid size
//AND APPLICATION CRASHES !!!
//*************************************************************************

#ifdef PROGRAM_WINWORKBAR
	#define WINVER 0x0400
	#define _WIN32_IE 0x0400
#endif

#ifdef PROGRAM_WINPSF
	#define WINVER 0x0500
	#define _WIN32_IE 0x0500
#endif

#include "afxwin.h"
#include "afxcmn.h"

#include <windows.h>
#include <stdio.h>
#include <shellapi.h>
#include <string>


//---------------------------------------------------------------------------

namespace TrayIconEx
{
  const DWORD WM_NOTIFYICON = WM_USER + 0x1000;

	typedef	void (*OnEvent)(LPARAM lParam);
	typedef	void (*OnMenuShow)(HMENU TM);

	class TI
	{
		HWND	_hwnd;
		UINT	_uID;
		UINT	_Message;
		UINT	_MID;
		bool	_IconAdded;

		NOTIFYICONDATA	tnid;

		bool	Initialised();
		bool  Add(HICON hicon, LPSTR lpszTip);
		bool  Modify(HICON hicon);
		bool  ModifyToolTip(LPSTR lpszTip);

		LRESULT	OnMenuNotification(WPARAM wParam, LPARAM lParam);
	public:
		TI();
		~TI();

		bool	Init(HWND hwnd, UINT uID = 1, UINT Message = WM_NOTIFYICON);
		bool	SetIcon(HICON hicon, std::string Text);
		bool	SetToolTip(std::string Text);
		bool	SetMenuResource(UINT MID);
		bool	Delete();

		bool	DoWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

		OnEvent			fnOnEvent;
		OnMenuShow	fnOnMenuShow;
	};
};
