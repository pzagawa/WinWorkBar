//
// PSMFCUtils - utilities for MFC
// Copyright by Piotr Zagawa. 2003 All rights reserved.
// Wszystkie prawa zastrze¿one
//
// ver.1.0:	12-04-2003
//

#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "Shlobj.h"
#include <Shlwapi.h>
#include <string>
#include <vector>

#include "PSRegistry.h"

//Macro for use with dll version comparing
#define PACKVERSION(major,minor) MAKELONG(minor,major)

//------------------------------------------------------------------------------------------------

namespace Text
{
	std::string GetText(CString& S);
}

namespace Date
{
	void RemoveTime(SYSTEMTIME& ST);
	void CopyTime(const SYSTEMTIME& srcST, SYSTEMTIME& dstST);
	bool DateEqual(const SYSTEMTIME& ST1, const SYSTEMTIME& ST2);

	FILETIME& Max(FILETIME& D1, FILETIME& D2);
	int Cmp(FILETIME& D1, FILETIME& D2);
	std::string	DateAsTextInfo(SYSTEMTIME Today, SYSTEMTIME SysTime, std::string Names[], bool ChangeToLocalTime = true);
}

namespace Number
{
	int Cmp(__int64& N1, __int64& N2);
	int Cmp(int& N1, int& N2);
	int Ratio(__int64 Max, __int64 Min);
	std::string IntToHex(int I);
	int HexToInt(std::string& S);
}

namespace	Misc
{
	std::string GetMyDataFolder(std::string ProgramName, std::string Company = "Vetch Utilities");

	COLORREF DarkerColor(COLORREF crColor, double dFactor);

	DWORD GetDllVersion(LPCTSTR lpszDllName);

	std::string GetHotKeyNormalName(WORD& wCode, WORD& wModifiers);
	std::string HotKeyToString(CHotKeyCtrl& C);
	bool	StringToHotKey(const std::string& S, WORD& wVirtualKeyCode, WORD& wModifiers);
	bool	StringToHotKey(const std::string& S, CHotKeyCtrl& C);
	UINT	TranslateMFCHotKeyModifierToWin32(WORD M);

	void FlashUserWindow(HWND hwnd, bool bCont = false);
	void FlashUserWindowStop(HWND hwnd);

	void ExecuteFile(std::string FileName, std::string CmdLineParams = "");

	std::string ApplicationFileName();
	std::string ApplicationFolder();

	int	ListCtrlItemsCount(CListCtrl& List, bool SelectedOnly);
	int	ListCtrlFirstSelectedItem(CListCtrl& List);
	void SelectAllItems(CListCtrl& List);
	void DeSelectAllItems(CListCtrl& List);

	void	SetRichEditFormat(CRichEditCtrl& RE, CHARRANGE& Range, COLORREF TextColor, bool Bold);

	void ProcessMessages();
	void Delay(int MSec);

	void LoadComboEditMRU(Registry::Reg& Reg, std::string Key, CComboBox& Combo);
	void SaveComboEditMRU(Registry::Reg& Reg, std::string Key, CComboBox& Combo, std::string CurrentString);

	//Name of logged user
	std::string UserName();

	//CSIDL_DRIVES 
	//CSIDL_DESKTOP
	std::string BrowseForFolder(HWND hwnd, char* szTitle, int nCSIDL = CSIDL_DRIVES);

	std::string GetFolder(std::string Token);

	std::string	GetEnvironementString(std::string Name);

	HICON	GetIconByFileType(CString Extension, bool bLargeIcons, std::string& FileType);

	void SaveWindowSizeAndPos(CDialog& Dlg, Registry::Reg& Prefs, std::string Name);
	void LoadWindowSizeAndPos(CDialog& Dlg, Registry::Reg& Prefs, std::string Name, UINT SHOWCMD, int DefWidth, int DefHeight);

	void SaveWindowStateAndSize(CDialog& Dlg, std::string Name, Registry::Reg& Prefs);
	void LoadWindowStateAndSize(CDialog& Dlg, std::string Name, Registry::Reg& Prefs, int DefWidth, int DefHeight, int MinWidth = -1,int MinHeight = -1);

	//Gets edit control text
	std::string GetEditText(CWnd& Dlg);
	//Gets control RECT, for it's client coords, for use with SetWindowPos
	RECT DlgClientRect(CWnd& ParentCtrl, CWnd& Control);

	//File icon and other data extraction !
	struct InfoExtractData
	{
		std::string Name;	//key field - only one record can exist in array
		std::string Desc;
		int					IconIndex;
	};

	class PSExtractSysIcon
	{
		CImageList										_ImageListSmall;
		bool													_HiColor;
		InfoExtractData								_EmptyInfoData;
		InfoExtractData								_InfoData;
		std::vector<InfoExtractData>	_InfoDataList;
	public:
		PSExtractSysIcon();
		~PSExtractSysIcon();
		CImageList& GetIconList() { return _ImageListSmall; };
		void AddIcon(std::string FileExtension);
		void AddIcon(std::string UniqueName, std::string Desc, int ShellIconID);
		void AddIcon(std::string UniqueName, std::string Desc, HICON Icon);
		void AddResourceIcon(std::string UniqueName, std::string Desc, int IconResourceID);

		int GetItemIndex(std::string Name);	// finds item by Name key. Returns -1, if not found
		InfoExtractData& GetData(std::string Name);
	};

	int ExecuteProgram(std::string FileName, std::string CmdLine, bool WaitForFinish, WORD wShowWindow = SW_SHOW);

	void SnapDialog_OnWindowPosChanging(CDialog& Dlg, WINDOWPOS* lpwndpos, int m_nXOffset = 10, int m_nYOffset = 10);

	bool SetTransparency(HWND hwnd, COLORREF KeyColor, BYTE Level, bool TranAlpha, bool TranKey);

}
