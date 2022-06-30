//
// PSLanguage
// Copyright by Piotr Zagawa. 2002 All rights reserved.
// Wszystkie prawa zastrze¿one
//
// ver.1.0:	06-10-2002
//

#include <afx.h>
#include "PSMFCUtils.h"
#include "PSFile.h"
#include "PSRegistry.h"
#include "PSUtil.h"
#include "ShellIcons.h"

//------------------------------------------------------------------------------------------------

namespace Text
{
	std::string Text::GetText(CString& S)
	{	
		std::string NewStr = S.GetBuffer();
		S.ReleaseBuffer();
		return NewStr;
	}
}

namespace Date
{
	void RemoveTime(SYSTEMTIME& ST)
	{
		ST.wHour = 0;
		ST.wMilliseconds = 0;
		ST.wMinute = 0;
		ST.wSecond = 0;
	}

	void CopyTime(const SYSTEMTIME& srcST, SYSTEMTIME& dstST)
	{
		dstST.wHour = srcST.wHour;
		dstST.wMilliseconds = srcST.wMilliseconds;
		dstST.wMinute = srcST.wMinute;
		dstST.wSecond = srcST.wSecond;
	}

	bool DateEqual(const SYSTEMTIME& ST1, const SYSTEMTIME& ST2)
	{
		return ((ST1.wYear == ST2.wYear) && (ST1.wMonth == ST2.wMonth) && (ST1.wDay == ST2.wDay));
	}

	FILETIME& Max(FILETIME& D1, FILETIME& D2)
	{
		int I = (int)CompareFileTime(&D1, &D2);	//-1: D1 < D2, 0: equal, 1: D1 > D2
		if (I == 1)
			return D1;
		return D2;
	}

	int Cmp(FILETIME& D1, FILETIME& D2)
	{
		return CompareFileTime(&D1, &D2);
	}

	//Names[0] = Today
	//Names[1] = Tommorrow
	//Names[2] = Yesterday
	//Names[3] = For week
	std::string	DateAsTextInfo(SYSTEMTIME Today, SYSTEMTIME SysTime, std::string Names[], bool ChangeToLocalTime)
	{
		std::string S = Text::DateToStringLocale(SysTime, NULL, ChangeToLocalTime);
		CTime ST(SysTime.wYear, SysTime.wMonth, SysTime.wDay, 0, 0, 0, 0);
		CTime TD(Today.wYear, Today.wMonth, Today.wDay, 0, 0, 0, 0);
		CTimeSpan Range = ST - TD;
		//today
		if (Range.GetDays() == 0)
			return Names[0];
		//tomorrow
		if (Range.GetDays() == 1)
			return Names[1];
		//yesterday
		if (Range.GetDays() == -1)
			return Names[2];
		//for week
		if (Range.GetDays() == 7)
			return Names[3];
		return S;
	}

}

namespace Number
{
	int Cmp(int& N1, int& N2)
	{
		if (N1 == N2)
			return 0;
		if (N1 > N2)
			return 1;
		else
			return -1;
	}

	int Cmp(__int64& N1, __int64& N2)
	{
		if (N1 == N2)
			return 0;
		if (N1 > N2)
			return 1;
		else
			return -1;
	}

	int Ratio(__int64 Max, __int64 Min)
	{
		if (Max)
			return (int)(((double)Min * 100.0F) / (double)Max);
		else
			return 0;
	}

	std::string IntToHex(int I)
	{
		char _Buff[16] = {0};
		itoa(I, _Buff, 16);
		return _Buff;
	}

	int HexToInt(std::string& S)
	{
		static const char List[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
		if (S.empty())
			return 0;
		int		Sum = 0;
		char C[2] = { 0, 0 };
		DWORD	Mask = 0x1;
		int Len = (int)S.length();
		for (int i = 0; i < Len; i++)
		{
			char C = toupper(S[Len - 1 - i]);
			int D = 0;
			for (int j = 0; j < 16; j++)
			{
				if (C == List[j])
				{
					D = j;
					break;
				}
			}
			Sum += (D * (Mask));
			Mask = Mask << 4;
		}
		return Sum;
	}

}

namespace	Misc
{

	std::string GetMyDataFolder(std::string ProgramName, std::string Company)
	{
		DWORD DllVersion = GetDllVersion(TEXT("Shell32.dll"));

		std::string S;
		TCHAR szPath[MAX_PATH];

		if (DllVersion >= PACKVERSION(5,00))
		{
			if (SUCCEEDED(SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, FALSE)))
			{
				S = szPath;
			}
		}

		if (S.empty())
		{
			if (SUCCEEDED(SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE)))
			{
				S = szPath;
			} else {
				S = ApplicationFolder();
			}
		}

		S = File::IncludeBackSlash(S);
		S += Company;
		S = File::IncludeBackSlash(S);
		S += ProgramName;
		return File::IncludeBackSlash(S);
	}

	COLORREF DarkerColor(COLORREF crColor, double dFactor)
	{
		if (dFactor > 0.0 && dFactor <= 1.0)
		{
			BYTE red,green,blue,lightred,lightgreen,lightblue;
			red = GetRValue(crColor);
			green = GetGValue(crColor);
			blue = GetBValue(crColor);
			lightred = (BYTE)(red-(dFactor * red));
			lightgreen = (BYTE)(green-(dFactor * green));
			lightblue = (BYTE)(blue-(dFactor * blue));
			crColor = RGB(lightred,lightgreen,lightblue);
		}
		return crColor;
	}

	/*
	example of use:
	DWORD DllVersion = GetDllVersion(TEXT("Shell32.dll"));
	if(DllVersion >= PACKVERSION(5,00))
	*/
	DWORD GetDllVersion(LPCTSTR lpszDllName)
	{
		HINSTANCE hinstDll;
		DWORD dwVersion = 0;
		hinstDll = LoadLibrary(lpszDllName);
		if(hinstDll)
		{
			DLLGETVERSIONPROC pDllGetVersion;
			pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");
			if(pDllGetVersion)
			{
				DLLVERSIONINFO dvi;
				HRESULT hr;
				ZeroMemory(&dvi, sizeof(dvi));
				dvi.cbSize = sizeof(dvi);
				hr = (*pDllGetVersion)(&dvi);
				if(SUCCEEDED(hr))
				{
					dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
				}
			}
			FreeLibrary(hinstDll);
		}
		return dwVersion;
	}

	std::string GetHotKeyNormalName(WORD& wCode, WORD& wModifiers)
	{
		static const TCHAR szPlus[] = _T(" + ");
		CString strKeyName;
		if (wCode != 0 || wModifiers != 0)
		{
			if (wModifiers & HOTKEYF_CONTROL)
			{
				strKeyName += CHotKeyCtrl::GetKeyName(VK_CONTROL, FALSE);
				strKeyName += szPlus;
			}

			if (wModifiers & HOTKEYF_SHIFT)
			{
				strKeyName += CHotKeyCtrl::GetKeyName(VK_SHIFT, FALSE);
				strKeyName += szPlus;
			}

			if (wModifiers & HOTKEYF_ALT)
			{
				strKeyName += CHotKeyCtrl::GetKeyName(VK_MENU, FALSE);
				strKeyName += szPlus;
			}

			strKeyName += CHotKeyCtrl::GetKeyName(wCode, wModifiers & HOTKEYF_EXT);
		}
		std::string S = strKeyName.GetBuffer();
		strKeyName.ReleaseBuffer();
		return S;
	}

	std::string HotKeyToString(CHotKeyCtrl& C)
	{
		WORD	wVirtualKeyCode	= 0;
		WORD	wModifiers = 0;
		C.GetHotKey(wVirtualKeyCode, wModifiers);
		return Conv::BuffToHexString((BYTE*)&wVirtualKeyCode, sizeof(WORD))
			+ " " +	Conv::BuffToHexString((BYTE*)&wModifiers, sizeof(WORD));
	}

	bool	StringToHotKey(const std::string& S, WORD& wVirtualKeyCode, WORD& wModifiers)
	{
		if (S.length() == 9)
		{
			std::string sVirtualKeyCode = S.substr(0, 4);
			std::string sModifiers			= S.substr(5, 4);
			if (sVirtualKeyCode.length() == 4)
				Conv::HexStringToBuff(sVirtualKeyCode, (BYTE*)&wVirtualKeyCode);
			if (sModifiers.length() == 4)
				Conv::HexStringToBuff(sModifiers, (BYTE*)&wModifiers);
			return true;
		}
		return false;
	}

	bool	StringToHotKey(const std::string& S, CHotKeyCtrl& C)
	{
		WORD	wVirtualKeyCode	= 0;
		WORD	wModifiers = 0;
		if (StringToHotKey(S, wVirtualKeyCode, wModifiers))
		{
			C.SetHotKey(wVirtualKeyCode, wModifiers);
			return true;
		}
		return false;
	}

	//Translate MFC style HotKeyModifier to Win32 style
	UINT	TranslateMFCHotKeyModifierToWin32(WORD M)
	{
		UINT Modifier = 0;
		if ((M & HOTKEYF_SHIFT) == HOTKEYF_SHIFT)
			Modifier |= MOD_SHIFT;
		if ((M & HOTKEYF_CONTROL) == HOTKEYF_CONTROL)
			Modifier |= MOD_CONTROL;
		if ((M & HOTKEYF_ALT) == HOTKEYF_ALT)
			Modifier |= MOD_ALT;
		if ((M & HOTKEYF_EXT) == HOTKEYF_EXT)
			Modifier |= MOD_WIN;

		return Modifier;
	}

	void FlashUserWindow(HWND hwnd, bool bCont)
	{
		FLASHWINFO FI;
		FI.cbSize = sizeof(FI);
		FI.dwTimeout = 300;
		FI.uCount = 3;
		FI.hwnd = hwnd;
		FI.dwFlags = FLASHW_TRAY;
		if (bCont)
			FI.dwFlags |= FLASHW_TIMER;
		FlashWindowEx(&FI);
	}

	void FlashUserWindowStop(HWND hwnd)
	{
		FLASHWINFO FI;
		FI.cbSize = sizeof(FI);
		FI.dwTimeout = 300;
		FI.uCount = 0;
		FI.hwnd = hwnd;
		FI.dwFlags = FLASHW_STOP;
		FlashWindowEx(&FI);
	}

	void ExecuteFile(std::string FileName, std::string CmdLineParams)
	{
		std::string TargetDir = File::ExtractPath(FileName);
		HINSTANCE Out = ShellExecute(NULL, "open", FileName.c_str(), CmdLineParams.c_str(), TargetDir.c_str(), SW_SHOWNORMAL);
	}

	std::string ApplicationFileName()
	{
		std::string S = "";
		int BuffSize = MAX_PATH * 2;
		char* Buff = new char[BuffSize];
		if (Buff)
		{
			if (GetModuleFileName(NULL, Buff, BuffSize) != 0)
				S = Buff;
			delete [] Buff;
		}
		return S;
	}

	std::string ApplicationFolder()
	{
		std::string S = ApplicationFileName();
		return File::ExtractPath(S);
	}

	int	ListCtrlItemsCount(CListCtrl& List, bool SelectedOnly)
	{
		int Count = 0;
		int Items = List.GetItemCount();
		for (int I = 0; I < Items; I++)
		{
			if (SelectedOnly)
			{
				if (List.GetItemState(I, LVIS_SELECTED) == LVIS_SELECTED)
					Count++;
			} else {
				Count++;
			}
		}
		return Count;
	}

	int	ListCtrlFirstSelectedItem(CListCtrl& List)
	{
		int Items = List.GetItemCount();
		for (int I = 0; I < Items; I++)
		{
			if (List.GetItemState(I, LVIS_SELECTED) == LVIS_SELECTED)
				return I;
		}
		return -1;
	}

	void SelectAllItems(CListCtrl& List)
	{
		for (int idx = 0; idx < List.GetItemCount(); idx++)
			List.SetItemState(idx, LVIS_SELECTED, LVIS_SELECTED);
	}

	void DeSelectAllItems(CListCtrl& List)
	{
		for (int idx = 0; idx < List.GetItemCount(); idx++)
			List.SetItemState(idx, 0, LVIS_SELECTED);
	}

	void	SetRichEditFormat(CRichEditCtrl& RE, CHARRANGE& Range, COLORREF TextColor, bool Bold)
	{
		static CHARFORMAT CF;
		CF.cbSize = sizeof(CF);
		RE.SetSel(Range);
		CF.dwMask = ((Bold)?CFM_BOLD:0) | CFM_COLOR;
		CF.dwEffects = (Bold)?CFE_BOLD:0;
		CF.crTextColor = TextColor;
		RE.SetSelectionCharFormat(CF);
	}

	void ProcessMessages()
	{
		CWinApp* pApp = AfxGetApp();
		MSG msg;
		while (PeekMessage ( &msg, NULL, 0, 0, PM_NOREMOVE ))
			pApp->PumpMessage();
	}

	void Delay(int MSec)
	{
		long int Start = GetTickCount();
		while (((long int)GetTickCount() - Start) < MSec)
		{	
			ProcessMessages();
			Sleep(10);
		}
	}

	void LoadComboEditMRU(Registry::Reg& Reg, std::string Key, CComboBox& Combo)
	{
		for (int idx = 0; idx < 7; idx++)
		{
			std::string T = Reg.ReadAsString(Key, "item_" + Text::IntToStr(idx), "");
			if (!T.empty())
				Combo.AddString(T.c_str());
		}
	}

	void SaveComboEditMRU(Registry::Reg& Reg, std::string Key, CComboBox& Combo, std::string CurrentString)
	{
		Combo.InsertString(0, CurrentString.c_str());
		if (Combo.GetCount() > 7)
			Combo.DeleteString(Combo.GetCount() - 1);

		int Items = Combo.GetCount();	
		for (int idx = 0; idx < Items; idx++)
		{
			CString S = "";
			Combo.GetLBText(idx, S);
			std::string T = Text::GetText(S);
			bool ItemExists = false;
			for (int idxa = 0; idxa < 7; idxa++)
				if (Text::StringsEqual(Reg.ReadAsString(Key, "item_" + Text::IntToStr(idxa), "*"), T))
				{
					ItemExists = true;
					break;
				}
			if (!ItemExists)
				Reg.Write(Key, "item_" + Text::IntToStr(idx), T);
		}
	}

	std::string UserName()
	{
		TCHAR szUserName[1024];
		DWORD	dwUserNameLen = sizeof(szUserName);
		std::string UserName = "";
		if (GetUserName(szUserName, &dwUserNameLen) != 0)
			UserName = szUserName;
		return UserName;
	}

	std::string BrowseForFolder(HWND hwnd, char* szTitle, int nCSIDL)
	{
		char					Buff[MAX_PATH] = {0};
		BROWSEINFO		BI;
		LPITEMIDLIST	pidlRoot = NULL;
		LPITEMIDLIST	pidlSelected = NULL;
		LPMALLOC			pMalloc = NULL;

		SHGetMalloc(&pMalloc);
		if(nCSIDL)
		{
			SHGetSpecialFolderLocation(hwnd, nCSIDL, &pidlRoot);
		} else {
			pidlRoot = NULL;
		}

		BI.hwndOwner = hwnd;
		BI.pidlRoot = pidlRoot;
		BI.pszDisplayName = NULL;
		BI.lpszTitle = szTitle;
		BI.ulFlags = BIF_NEWDIALOGSTYLE | BIF_USENEWUI;
		BI.lpfn = NULL;
		BI.lParam = 0;

		if (pidlSelected = SHBrowseForFolder(&BI))
		{
			SHGetPathFromIDList(pidlSelected, Buff);
			pMalloc->Free(pidlSelected);
		}
		if (pidlRoot)
			pMalloc->Free(pidlRoot);
		pMalloc->Release();
		return Buff;
	}

	std::string GetFolder(std::string Token)
	{
		std::string S = "";
		int BuffSize = MAX_PATH * 4;
		char* Buff = new char[BuffSize];

		Registry::Reg MyReg(HKEY_LOCAL_MACHINE);
		const char* CurrVersion		= "Software\\Microsoft\\Windows\\CurrentVersion\\";
		const char* ShellFolders	= "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders\\";

		if (Text::StringsEqual(Token, "SendTo"))
		{
			MyReg.SetKey(HKEY_CURRENT_USER, ShellFolders);
			S = MyReg.ReadAsString("", "Favorites");
		}
		if (Text::StringsEqual(Token, "favorites"))
		{
			MyReg.SetKey(HKEY_CURRENT_USER, ShellFolders);
			S = MyReg.ReadAsString("", "Favorites");
		}
		if (Text::StringsEqual(Token, "mydocs"))
		{
			MyReg.SetKey(HKEY_CURRENT_USER, ShellFolders);
			S = MyReg.ReadAsString("", "Personal");
		}
		if (Text::StringsEqual(Token, "startmenu"))
		{
			MyReg.SetKey(HKEY_CURRENT_USER, ShellFolders);
			S = MyReg.ReadAsString("", "Start Menu");
		}
		if (Text::StringsEqual(Token, "cookies"))
		{
			MyReg.SetKey(HKEY_CURRENT_USER, ShellFolders);
			S = MyReg.ReadAsString("", "Cookies");
		}
		if (Text::StringsEqual(Token, "fonts"))
		{
			MyReg.SetKey(HKEY_CURRENT_USER, ShellFolders);
			S = MyReg.ReadAsString("", "Fonts");
		}
		if (Text::StringsEqual(Token, "startmenuprograms"))
		{
			MyReg.SetKey(HKEY_CURRENT_USER, ShellFolders);
			S = MyReg.ReadAsString("", "Programs");
		}
		if (Text::StringsEqual(Token, "programfiles"))
		{
			MyReg.SetKey(HKEY_LOCAL_MACHINE, CurrVersion);
			S = MyReg.ReadAsString("", "ProgramFilesDir");
		}
		if (Text::StringsEqual(Token, "commonfiles"))
		{
			MyReg.SetKey(HKEY_LOCAL_MACHINE, CurrVersion);
			S = MyReg.ReadAsString("", "CommonFilesDir");
		}
		if (Text::StringsEqual(Token, "autostart"))
		{
			MyReg.SetKey(HKEY_CURRENT_USER, ShellFolders);
			S = MyReg.ReadAsString("", "Startup");
		}
		if (Text::StringsEqual(Token, "desktop"))
		{
			MyReg.SetKey(HKEY_CURRENT_USER, ShellFolders);
			S = MyReg.ReadAsString("", "Desktop");
		}
		if (Text::StringsEqual(Token, "system"))
		  if (GetSystemDirectory(Buff, BuffSize) != 0)
				S = Buff;
		if (Text::StringsEqual(Token, "windows"))
			if (GetWindowsDirectory(Buff, BuffSize) != 0)
				S = Buff;
		if (Text::StringsEqual(Token, "application"))
			if (GetModuleFileName(NULL, Buff, BuffSize) != 0)
				S = File::ExtractPath(Buff);
		if (Text::StringsEqual(Token, "temp"))
			if (GetTempPath(BuffSize, Buff) != 0)
				S = Buff;
		if (!S.empty())
			S = File::IncludeBackSlash(S);

		delete [] Buff;
		return S;
	}

	//Name can be: %USERPROFILE%, etc... (look @ SET in CMD mode)
	std::string	GetEnvironementString(std::string Name)
	{
		TCHAR	Buff[MAX_PATH] = {0};
		if (ExpandEnvironmentStrings(Name.c_str(), Buff, MAX_PATH) == 0)
		{
			return "";
		} else {
			return Buff;
		}
	}

	HICON	GetIconByFileType(CString Extension, bool bLargeIcons, std::string& FileType)
	{
		// create dummy filename "foo.ext" 
		CString sDummyFileName = _T("foo.") + Extension;
		// Call SHGetFileInfo to get the icon. Call with dummy filename and
		// SHGFI_USEFILEATTRIBUTES flag, so file doesn't actually have to exist.
		SHFILEINFO shfi;
		memset(&shfi,0,sizeof(shfi));
		SHGetFileInfo(sDummyFileName, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi), 
			SHGFI_ICON|SHGFI_TYPENAME|((bLargeIcons)?0:SHGFI_SMALLICON)|SHGFI_USEFILEATTRIBUTES);
		FileType = shfi.szTypeName;
		return shfi.hIcon;
	}

	void SaveWindowSizeAndPos(CDialog& Dlg, Registry::Reg& Prefs, std::string Name)
	{
		RECT Rect;
		Dlg.GetWindowRect(&Rect);
		Prefs.Write("Windows\\" + Name, "posX", Rect.left);
		Prefs.Write("Windows\\" + Name, "posY", Rect.top);
		Prefs.Write("Windows\\" + Name, "sizeW", Rect.right - Rect.left);
		Prefs.Write("Windows\\" + Name, "sizeH", Rect.bottom - Rect.top);
	}

	void LoadWindowSizeAndPos(CDialog& Dlg, Registry::Reg& Prefs, std::string Name, UINT SHOWCMD, int DefWidth, int DefHeight)
	{
		WINDOWPLACEMENT lpWin;
		ZeroMemory(&lpWin, sizeof(lpWin));
		lpWin.length = sizeof(lpWin);

		//Window center at start
		RECT DesktopRect;
		Dlg.GetDesktopWindow()->GetWindowRect(&DesktopRect);

		int DesktopW = DesktopRect.right - DesktopRect.left;
		int DesktopH = DesktopRect.bottom - DesktopRect.top;
		
		int W = Prefs.ReadAsInt("Windows\\" + Name, "sizeW", DefWidth);
		int H = Prefs.ReadAsInt("Windows\\" + Name, "sizeH", DefHeight);

		int StartX = (DesktopW >> 1) - (W >> 1);
		int StartY = (DesktopH >> 1) - (H >> 1);

		int X = Prefs.ReadAsInt("Windows\\" + Name, "posX", StartX);
		int Y = Prefs.ReadAsInt("Windows\\" + Name, "posY", StartY);

		if (X < 0)
			X = 0;
		if (Y < 0)
			Y = 0;
		if ((X + W) > DesktopW)
			X = DesktopW - W;
		if ((Y + H) > DesktopH)
			Y = DesktopH - H;

		lpWin.rcNormalPosition.left = X;
		lpWin.rcNormalPosition.top = Y;
		lpWin.rcNormalPosition.right = lpWin.rcNormalPosition.left + W;
		lpWin.rcNormalPosition.bottom = lpWin.rcNormalPosition.top + H;

		lpWin.showCmd = SHOWCMD;
		Dlg.SetWindowPlacement(&lpWin);
	}

	void SaveWindowStateAndSize(CDialog& Dlg, std::string Name, Registry::Reg& Prefs)
	{
		std::string State = "normal";
		if (!Dlg.IsWindowVisible())
			return;
		if (Dlg.IsIconic())
			return;
		if (Dlg.IsZoomed())
			State = "max";
		Prefs.Write("Windows\\" + Name, "_state", State);
		if (State == "normal")
		{
			RECT Rect;
			Dlg.GetWindowRect(&Rect);
			Prefs.Write("Windows\\" + Name, "posX", Rect.left);
			Prefs.Write("Windows\\" + Name, "posY", Rect.top);
			Prefs.Write("Windows\\" + Name, "sizeW", Rect.right - Rect.left);
			Prefs.Write("Windows\\" + Name, "sizeH", Rect.bottom - Rect.top);
		}
	}

	void LoadWindowStateAndSize(CDialog& Dlg, std::string Name, Registry::Reg& Prefs, int DefWidth, int DefHeight, int MinWidth ,int MinHeight)
	{
		WINDOWPLACEMENT lpWin;
		ZeroMemory(&lpWin, sizeof(lpWin));
		std::string State = Prefs.ReadAsString("Windows\\" + Name, "_state", "normal");
		lpWin.length = sizeof(lpWin);

		//Window center at start
		RECT DesktopRect;
		Dlg.GetDesktopWindow()->GetWindowRect(&DesktopRect);

		int DesktopW = DesktopRect.right - DesktopRect.left;
		int DesktopH = DesktopRect.bottom - DesktopRect.top;
		
		int W = Prefs.ReadAsInt("Windows\\" + Name, "sizeW", DefWidth);
		int H = Prefs.ReadAsInt("Windows\\" + Name, "sizeH", DefHeight);

		if (MinWidth > W)
			W = DefWidth;
		if (MinHeight > H)
			H = DefHeight;
		
		int StartX = (DesktopW >> 1) - (W >> 1);
		int StartY = (DesktopH >> 1) - (H >> 1);

		int X = Prefs.ReadAsInt("Windows\\" + Name, "posX", StartX);
		int Y = Prefs.ReadAsInt("Windows\\" + Name, "posY", StartY);

		if (X < 0)
			X = 0;
		if (Y < 0)
			Y = 0;
		if ((X + W) > DesktopW)
			X = DesktopW - W;
		if ((Y + H) > DesktopH)
			Y = DesktopH - H;

		lpWin.rcNormalPosition.left = X;
		lpWin.rcNormalPosition.top = Y;
		lpWin.rcNormalPosition.right = lpWin.rcNormalPosition.left + W;
		lpWin.rcNormalPosition.bottom = lpWin.rcNormalPosition.top + H;

		if (State == "max")
		{
			lpWin.showCmd = SW_MAXIMIZE;
		} else {
			lpWin.showCmd = SW_SHOWNA;
		}
		Dlg.SetWindowPlacement(&lpWin);
	}

	std::string GetEditText(CWnd& Dlg)
	{
		CString CS;
		Dlg.GetWindowText(CS);
		return (std::string)CS;
	}

	RECT DlgClientRect(CWnd& ParentCtrl, CWnd& Control)
	{
		RECT R;
		RECT DlgClientRect;
		ParentCtrl.GetClientRect(&DlgClientRect);
		ParentCtrl.ClientToScreen(&DlgClientRect);
		Control.GetWindowRect(&R);
		R.left = R.left - DlgClientRect.left;
		R.right = R.right - DlgClientRect.left;
		R.top = R.top - DlgClientRect.top;
		R.bottom = R.bottom - DlgClientRect.top;
		return R;
	}


	//PSExtractSysIcon CLASS
	//------------------------------------------------------------------------------------------
	PSExtractSysIcon::PSExtractSysIcon()
	{
		// Get color depth
		HDC hDC = ::GetDC(NULL);
		bool _HiColor = ::GetDeviceCaps (hDC, NUMCOLORS) == -1;
		::ReleaseDC (NULL, hDC);
		// Create image list
		_ImageListSmall.Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics (SM_CYSMICON),
			_HiColor?(ILC_COLOR32|ILC_MASK) : (ILC_COLOR4|ILC_MASK), 100, 20);

		_EmptyInfoData.Name = "";
		_EmptyInfoData.Desc = "";
		_EmptyInfoData.IconIndex = -1;

		AddIcon("SI_DEF_DOCUMENT", "", SI_DEF_DOCUMENT);
	}

	PSExtractSysIcon::~PSExtractSysIcon()
	{
	};

	int PSExtractSysIcon::GetItemIndex(std::string Name)
	{
		Name = Text::LowerCase(Name);
		if (Name.empty())
			return -1;
		for (int IDX = 0; IDX < (int)_InfoDataList.size(); IDX++)
			if (_InfoDataList[IDX].Name == Name)
				return IDX;
		return -1;
	}

	InfoExtractData& PSExtractSysIcon::GetData(std::string Name)
	{
		if (Name == "")	//display default doc icon if no extension exists
			Name = "SI_DEF_DOCUMENT";

		int IDX = GetItemIndex(Name);
		if (IDX == -1)
			return _EmptyInfoData;
		return _InfoDataList[IDX];
	}

	void PSExtractSysIcon::AddIcon(std::string FileExtension)
	{
		FileExtension = Text::LowerCase(FileExtension);
		if (GetItemIndex(FileExtension) == -1)
		{
			HICON hIcon = GetIconByFileType(FileExtension.c_str(), false, _InfoData.Desc);
			if (hIcon)
			{
				int IDX = _ImageListSmall.Add(hIcon);
				if (IDX != -1)
				{
					_InfoData.Name = FileExtension;
					_InfoData.IconIndex = IDX;
					_InfoDataList.push_back(_InfoData);
				}
				DestroyIcon(hIcon);	
			}
		}
	}

	void PSExtractSysIcon::AddIcon(std::string UniqueName, std::string Desc, int ShellIconID)
	{
		UniqueName = Text::LowerCase(UniqueName);
		if (GetItemIndex(UniqueName) == -1)
		{
			HICON hIcon = ExtractShellIcon(ShellIconID, false);
			if (hIcon)
			{
				int IDX = _ImageListSmall.Add(hIcon);
				if (IDX != -1)
				{
					_InfoData.Name = UniqueName;
					_InfoData.Desc = Desc;
					_InfoData.IconIndex = IDX;
					_InfoDataList.push_back(_InfoData);
				}
				DestroyIcon(hIcon);	
			}
		}
	}

	void PSExtractSysIcon::AddIcon(std::string UniqueName, std::string Desc, HICON Icon)
	{
		UniqueName = Text::LowerCase(UniqueName);
		if (GetItemIndex(UniqueName) == -1)
		{
			if (Icon)
			{
				int IDX = _ImageListSmall.Add(Icon);
				if (IDX != -1)
				{
					_InfoData.Name = UniqueName;
					_InfoData.Desc = Desc;
					_InfoData.IconIndex = IDX;
					_InfoDataList.push_back(_InfoData);
				}
				DestroyIcon(Icon);	
			}
		}
	}

	void PSExtractSysIcon::AddResourceIcon(std::string UniqueName, std::string Desc, int IconResourceID)
	{
		HICON Icon = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IconResourceID), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
		if (Icon)
			AddIcon(UniqueName, Desc, Icon);
	}

	int ExecuteProgram(std::string FileName, std::string CmdLine, bool WaitForFinish, WORD wShowWindow)
	{
		DWORD	dwRetCode = 0;
		STARTUPINFO	SI;
		PROCESS_INFORMATION	PI;

		ZeroMemory(&SI, sizeof(SI));
		ZeroMemory(&PI, sizeof(PI));

		SI.cb = sizeof(SI);
		SI.dwFlags = STARTF_USESHOWWINDOW;
		SI.wShowWindow = wShowWindow;

		CmdLine = "\"" + FileName + "\" " + CmdLine;

		BOOL Out = CreateProcess(NULL, (char*)CmdLine.c_str(), NULL, NULL, FALSE,
			NORMAL_PRIORITY_CLASS, NULL, NULL, &SI, &PI);

		if (Out == TRUE)
		{
			if (WaitForFinish)
				WaitForSingleObject(PI.hProcess, INFINITE);
			GetExitCodeProcess(PI.hProcess, &dwRetCode);
			return dwRetCode;
		} else {
			return -1;
		}
		return 0;
	}

	void SnapDialog_OnWindowPosChanging(CDialog& Dlg, WINDOWPOS* lpwndpos, int m_nXOffset, int m_nYOffset)
	{
		RECT	wndRect, trayRect;
		int		leftTaskbar = 0, rightTaskbar = 0, topTaskbar = 0, bottomTaskbar = 0;

		Dlg.GetWindowRect(&wndRect);

		// Screen resolution
		int screenWidth =	GetSystemMetrics(SM_CXSCREEN); 
		int screenHeight =	GetSystemMetrics(SM_CYSCREEN);

		// Find the taskbar
		CWnd* pWnd = Dlg.FindWindow(_T("Shell_TrayWnd"), _T(""));
		pWnd->GetWindowRect(&trayRect);

		int wndWidth = wndRect.right - wndRect.left;
		int wndHeight = wndRect.bottom - wndRect.top;

		if(trayRect.top <= 0 && trayRect.left <= 0 && trayRect.right >= screenWidth) {
			// top taskbar
			topTaskbar = trayRect.bottom - trayRect.top;
		}
		else if(trayRect.top > 0 && trayRect.left <= 0) {
			// bottom taskbar
			bottomTaskbar = trayRect.bottom - trayRect.top;
		}
		else if(trayRect.top <= 0 && trayRect.left > 0) {
			// right taskbar
			rightTaskbar = trayRect.right - trayRect.left;
		}
		else {
			// left taskbar
			leftTaskbar = trayRect.right - trayRect.left;
		}

		// Snap to screen border
		// Left border
		if(lpwndpos->x >= -m_nXOffset + leftTaskbar && lpwndpos->x <= leftTaskbar + m_nXOffset) {
			lpwndpos->x = leftTaskbar;
		}

		// Top border
		if(lpwndpos->y >= -m_nYOffset && lpwndpos->y <= topTaskbar + m_nYOffset) {
			lpwndpos->y = topTaskbar;
		}

		// Right border
		if(lpwndpos->x + wndWidth <= screenWidth - rightTaskbar + m_nXOffset && lpwndpos->x + wndWidth >= screenWidth - rightTaskbar - m_nXOffset) {
			lpwndpos->x = screenWidth - rightTaskbar - wndWidth;
		}

		// Bottom border
		if( lpwndpos->y + wndHeight <= screenHeight - bottomTaskbar + m_nYOffset && lpwndpos->y + wndHeight >= screenHeight - bottomTaskbar - m_nYOffset) {
			lpwndpos->y = screenHeight - bottomTaskbar - wndHeight;
		}
	}

	bool SetTransparency(HWND hwnd, COLORREF KeyColor, BYTE Level, bool TranAlpha, bool TranKey)
	{
		const int _WS_EX_LAYERED = 0x00080000;
		const int _LMA_COLORKEY  = 0x00000001;
		const int _LMA_ALPHA     = 0x00000002;
		//SetLayeredWindowAttributes
		typedef	BOOL (*func)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

		bool Result = false;
		HMODULE LibHandle = LoadLibrary("user32");
		if (LibHandle)
		{
			func LayerFunc = (func)GetProcAddress(LibHandle, "SetLayeredWindowAttributes");
			if (LayerFunc)
			{
				DWORD	LMA_Flags = 0;
				if (TranAlpha)
					LMA_Flags = LMA_Flags + _LMA_ALPHA;
        if (TranKey)
					LMA_Flags = LMA_Flags + _LMA_COLORKEY;

				ULONG	ExtStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
				SetWindowLong(hwnd, GWL_EXSTYLE, ExtStyle | WS_EX_LAYERED);
  			LayerFunc(hwnd, KeyColor, Level, LMA_Flags);
				Result = true;
			}
			FreeLibrary(LibHandle);
		}
		return Result;
	}
}
