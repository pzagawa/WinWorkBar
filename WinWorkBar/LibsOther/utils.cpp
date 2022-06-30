
#include "utils.h"
#include <mmsystem.h>

extern CWinWorkBarDlg*	lpMain;

namespace Utils
{
	//winbar pos data 
	CRect DesktopRect;
	CRect MainRect;
	int		MainX					= 0;
	int		MainY					= 0;
	int		MainW					= DefaultWidth;
	int		MainH					= 0;
	int		DeskW					= 0;
	int		DeskH					= 0;
	int		TaskBarW			= 0;
	int		TaskBarH			= 0;
	int		Step					= 4;
	UINT	uTaskBarEdge	= 0;

	//Sizes
	int		grpDateAndTime_Height	= 107;
	int		grpCalendar_Height		= 169;

	int		labMargin							= 8;	//margines dla np. label -> grupa
	int		labMarginTop					= labMargin + labMargin + (labMargin >> 1);
	int		labLineMargin					= 0;

	//other
	bool Visible		= false;

	//HotKeys table
	const	int		HotKeysCount = sizeof(DefaultHotKeys) / sizeof(HotKeyItem);
	HotKeyItem	HotKeys[HotKeysCount];

	//Menu item cmd id for categories
	const int MenuItemStartCmdID	= 10000;
	const int MenuItemStopCmdID		= 10031;


	//Code
	//-------------------------------------------------------------------------

	void	SetWorkBarWidth(int Width)
	{
		if (Width < DefaultWidth)
			Width = DefaultWidth;
		if (Width > MaxWidth)
			Width = MaxWidth;
		MainW = Width;
	}

	int		GetWorkBarWidth()
	{
		return MainW;
	}

	//Fn's
	void SetWorkBarPosition()
	{
		CWnd* lpDesktop = lpMain->GetDesktopWindow();
		lpDesktop->GetClientRect(&DesktopRect);
		lpMain->GetWindowRect(&MainRect);
		CRect TaskBarRect = WinTaskBarRect();

		DeskW = DesktopRect.Width();
		DeskH = DesktopRect.Height();
		TaskBarW = TaskBarRect.Width();
		TaskBarH = TaskBarRect.Height();

		uTaskBarEdge = Utils::WinTaskBarEdge(TaskBarRect);
		switch(uTaskBarEdge)
		{
			case ABE_LEFT:
				MainX = DeskW - lpMain->LeftMargin;
				MainY = 0;
				MainH = DeskH;
				break;
			case ABE_RIGHT:
				MainX = - MainW + lpMain->LeftMargin;
				MainY = 0;
				MainH = DeskH;
				break;
			case ABE_TOP:
				MainX = DeskW - lpMain->LeftMargin;
				MainY = TaskBarH;
				MainH = DeskH - TaskBarH;
				break;
			case ABE_BOTTOM:
				MainX = DeskW - lpMain->LeftMargin;
				MainY = 0;
				MainH = DeskH - TaskBarH;
				break;
		}
		lpMain->ShowWindow(SW_HIDE);
		lpMain->SetWindowPos(NULL, MainX, MainY, MainW, MainH, 0);

		//Show all !
		lpMain->ShowWindow(SW_SHOW);
	}

	RECT&	WinTaskBarRect()
	{
		static CRect R;
		static APPBARDATA abd;
		CWnd *pwnd = lpMain->FindWindow("Shell_TrayWnd", NULL);
		if (pwnd)
		{
			abd.cbSize = sizeof(APPBARDATA);
			abd.hWnd = pwnd->m_hWnd;         
			SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
			R = abd.rc;
		}
		return R;
	}

	UINT WinTaskBarEdge(CRect rc)
	{
		UINT uEdge = -1;      
		if (rc.top == rc.left && rc.bottom > rc.right)
		{
				uEdge = ABE_LEFT;
		}
		else if (rc.top == rc.left && rc.bottom < rc.right)
		{
				uEdge = ABE_TOP;
		}
		else if (rc.top > rc.left )
		{
				uEdge = ABE_BOTTOM;
		}
		else
		{
				uEdge = ABE_RIGHT;
		}
		return uEdge;
	} 

	UINT	GetWinTaskBarEdge()
	{
		return uTaskBarEdge;
	}

	void ShowWorkBar()
	{
		if (Visible)
			return;

		int X = MainX;
		int Y = MainY;
		int W = MainW;
		int H = MainH;

		switch(uTaskBarEdge)
		{
			case ABE_RIGHT:
				X = 0;
				Visible = true;
				break;
			case ABE_LEFT:
			case ABE_TOP:
			case ABE_BOTTOM:				
				X = (DeskW - W);
				Visible = true;
				break;
		}

		if (lpMain->Settings.WindowTopmost)
		{
			lpMain->SetWindowPos(NULL, X, Y, W, H, 0);
		} else {
			lpMain->SetWindowPos(&CWnd::wndTopMost, X, Y, W, H, SWP_NOACTIVATE);
			lpMain->BringWindowToTop();
			lpMain->SetWindowPos(&CWnd::wndNoTopMost, X, Y, W, H, SWP_NOACTIVATE);
		}
		ProcessMessages();
	}

	void HideWorkBar(bool ForceHide)
	{
		if (!ForceHide)
		{
			if (lpMain->Settings.PinWindow)
				return;
		}

		if (!Visible)
			return;

		int X = MainX;
		int Y = MainY;
		int W = MainW;
		int H = MainH;

		switch(uTaskBarEdge)
		{
			case ABE_RIGHT:
				X = - MainW + lpMain->LeftMargin;
				Visible = false;
				break;
			case ABE_LEFT:
			case ABE_TOP:
			case ABE_BOTTOM:
				X = (DeskW - lpMain->LeftMargin);
				Visible = false;
				break;
		}

		if (lpMain->Settings.WindowTopmost)
		{
			lpMain->SetWindowPos(NULL, X, Y, W, H, SWP_NOACTIVATE | SWP_NOZORDER);
		} else {
			lpMain->SetWindowPos(&CWnd::wndBottom, X, Y, W, H, SWP_NOACTIVATE);
		}
		ProcessMessages();

		//Bring opened windows to top !!!
		if (lpMain->dlgSettings.GetSafeHwnd() && lpMain->dlgSettings.IsWindowVisible())
			lpMain->dlgSettings.BringWindowToTop();

		if (lpMain->dlgToolInfo.GetSafeHwnd())
			lpMain->dlgToolInfo.BringInfoWindowToFront();
		Delay(1);
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
			ProcessMessages();
	}

	bool WorkBarVisible()
	{
		return Visible;
	}

	void	SetDefaultHotKeys()
	{
		CopyMemory(&HotKeys, &DefaultHotKeys, sizeof(HotKeys));
	}

	bool	RegisterHotKeys()
	{
		bool Result = true;
		for (int idx = 0; idx < HotKeysCount; idx++)
		{
			UINT Modifiers = Misc::TranslateMFCHotKeyModifierToWin32(HotKeys[idx].Modifier);
			if (HotKeys[idx].on)
				if (RegisterHotKey(lpMain->m_hWnd, HotKeys[idx].ID, Modifiers, HotKeys[idx].VKey) == 0)
					Result = false;
		}
		return Result;
	}

	void	UnRegisterHotKeys()
	{
		for (int idx = 0; idx < HotKeysCount; idx++)
			UnregisterHotKey(lpMain->m_hWnd, HotKeys[idx].ID);
	}

	void	InitHotKeys()
	{
		CWinWorkBarDlg& Dlg = *lpMain;

		//Read prefs here...

		//Register it
		UnRegisterHotKeys();
		GetConfiguredHotKeys();
		RegisterHotKeys();

		SetControlsStrings();
	}

	void	GetConfiguredHotKeys()
	{
		CWinWorkBarDlg& Dlg = *lpMain;

		std::string HotKeyStr1 = Dlg.Settings.HotKey_ShowWorkBar.substr(2);
		std::string HotKeyStr2 = Dlg.Settings.HotKey_NewTask.substr(2);
		std::string HotKeyStr3 = Dlg.Settings.HotKey_NewNote.substr(2);

		std::string chkHotKeyState1 = Dlg.Settings.HotKey_ShowWorkBar.substr(0, 1);
		std::string chkHotKeyState2 = Dlg.Settings.HotKey_NewTask.substr(0, 1);
		std::string chkHotKeyState3 = Dlg.Settings.HotKey_NewNote.substr(0, 1);

		HotKeys[hktShowWorkBar].on = (chkHotKeyState1 == "+");
		HotKeys[hktNewTask].on = (chkHotKeyState2 == "+");
		HotKeys[hktNewNote].on = (chkHotKeyState3 == "+");

		WORD	wVirtualKeyCode	= 0;
		WORD	wModifiers = 0;

		Misc::StringToHotKey(HotKeyStr1, wVirtualKeyCode, wModifiers);
			HotKeys[hktShowWorkBar].Modifier = wModifiers;
			HotKeys[hktShowWorkBar].VKey = wVirtualKeyCode;

		Misc::StringToHotKey(HotKeyStr2, wVirtualKeyCode, wModifiers);
			HotKeys[hktNewTask].Modifier = wModifiers;
			HotKeys[hktNewTask].VKey = wVirtualKeyCode;

		Misc::StringToHotKey(HotKeyStr3, wVirtualKeyCode, wModifiers);
			HotKeys[hktNewNote].Modifier = wModifiers;
			HotKeys[hktNewNote].VKey = wVirtualKeyCode;
	}

	void	Init()
	{
		lpMain->Prefs.SetAppName(ProgramName);
		lpMain->PrefsMachine.SetAppName(ProgramName);
		lpMain->Settings.Reg = &lpMain->Prefs;
		lpMain->Settings.Read();
		lpMain->ML.SetLanguage(lpMain->Settings.Language);

		SetDefaultHotKeys();
		lpMain->ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
		lpMain->SetWindowText(lpMain->GetAppTitle().c_str());
		lpMain->SetWindowTopmost(lpMain->Settings.WindowTopmost);

		//Load and init data tables before use
		LoadAndResetTable_Categories(false);
		LoadAndResetTable_Notes(false);
		LoadAndResetTable_Tasks(false);

		SetWindowTransparency();
		SetupControls();
	}

	void SetupControls()
	{
		CWinWorkBarDlg& Dlg = *lpMain;

		//Setup icons
		Dlg.SysIcons.AddResourceIcon("Task", "", IDI_ICO_TASK);
		Dlg.SysIcons.AddResourceIcon("Note", "", IDI_ICO_NOTE);
		Dlg.SysIcons.AddResourceIcon("Today", "", IDI_ICO_TODAY);
		Dlg.SysIcons.AddResourceIcon("Find", "", IDI_ICO_FIND);

		//Set tab icons
		Dlg.m_Tab.SetImageList(&Dlg.SysIcons.GetIconList());

		//Set tab pages
		Dlg.dlgToday.Create(IDD_TAB_TODAY, lpMain);
		Dlg.dlgTasks.Create(IDD_TAB_TASKS, lpMain);
		Dlg.dlgNotes.Create(IDD_TAB_NOTES, lpMain);

		Dlg.dlgToday.lpThemeHelper = &lpMain->m_ThemeHelper;
		Dlg.dlgTasks.lpThemeHelper = &lpMain->m_ThemeHelper;
		Dlg.dlgNotes.lpThemeHelper = &lpMain->m_ThemeHelper;

		Dlg.m_Tab.AddPage(Dlg.ML.Text(21), &Dlg.dlgToday, 3);
		Dlg.m_Tab.AddPage(Dlg.ML.Text(16), &Dlg.dlgTasks, 1);
		Dlg.m_Tab.AddPage(Dlg.ML.Text(17), &Dlg.dlgNotes, 2);

		//Set main menu
		Dlg.MainMenu.LoadMenu(IDR_MAINMENU);
		Dlg.SetMenu(&Dlg.MainMenu);
		//Set MenuXP
		CMenuXP::SetXPLookNFeel(lpMain);
		CMenuXP::UpdateMenuBar(lpMain);
		Dlg.imgListMainMenu.Create(16, 16, ILC_COLOR32|ILC_MASK, 0, 4);
		Dlg.bmpMainMenuXP.LoadBitmap(IDB_MENUXP);
		Dlg.imgListMainMenu.Add(&Dlg.bmpMainMenuXP, RGB(255,0,255));
		//Set mainmenu items images
		CMenuXP::SetMenuItemImage(ID_MM_NEWTASK, Dlg.imgListMainMenu, 0);
		CMenuXP::SetMenuItemImage(ID_MM_NEWNOTE, Dlg.imgListMainMenu, 1);
		CMenuXP::SetMenuItemImage(ID_MM_ABOUT, Dlg.imgListMainMenu, 2);
		CMenuXP::SetMenuItemImage(ID_MM_EXIT, Dlg.imgListMainMenu, 3);
		CMenuXP::SetMenuItemImage(ID_MM_SETTINGS, Dlg.imgListMainMenu, 4);
		CMenuXP::SetMenuItemImage(ID_MM_HIDE, Dlg.imgListMainMenu, 5);
		CMenuXP::SetMenuItemImage(ID_MM_CATEGORIES, Dlg.imgListMainMenu, 6);

		SetupViewMenu();
		SetControlsStrings();
	}

	void	UpdateMenuItemsState()
	{
		//Set menu item "dont hide" state
		UINT State = (lpMain->Settings.PinWindow)?MF_CHECKED:MF_UNCHECKED;
		lpMain->MainMenu.CheckMenuItem(ID_MM_DO_NOT_HIDE, MF_BYCOMMAND | State);
		//"on top" state
		State = (lpMain->Settings.WindowTopmost)?MF_CHECKED:MF_UNCHECKED;
		lpMain->MainMenu.CheckMenuItem(ID_MM_ONTOP, MF_BYCOMMAND | State);
	}

	void SetMenuItemsStateByListItemsSelect()
	{
		CWinWorkBarDlg& Dlg = *lpMain;

		int SelectedItems = 0;

		//Tasks tab
		if (Dlg.dlgTasks.GetSafeHwnd())
			if (Dlg.dlgTasks.IsWindowVisible())
				SelectedItems = Misc::ListCtrlItemsCount(Dlg.dlgTasks.listTasks, true);
		//Notes tab
		if (Dlg.dlgNotes.GetSafeHwnd())
			if (Dlg.dlgNotes.IsWindowVisible())
				SelectedItems = Misc::ListCtrlItemsCount(Dlg.dlgNotes.listNotes, true);

		if (Dlg.MainMenu.GetSafeHmenu())
		{
			Dlg.MainMenu.EnableMenuItem(ID_MM_EDIT,	MF_BYCOMMAND | ((SelectedItems == 1)?(MF_ENABLED):(MF_GRAYED | MF_DISABLED)));
			Dlg.MainMenu.EnableMenuItem(ID_MM_DELETE,	MF_BYCOMMAND | ((SelectedItems >= 1)?(MF_ENABLED):(MF_GRAYED | MF_DISABLED)));
			Dlg.MainMenu.EnableMenuItem(ID_MM_VIEWINBROWSER,	MF_BYCOMMAND | ((SelectedItems >= 1)?(MF_ENABLED):(MF_GRAYED | MF_DISABLED)));
			Dlg.MainMenu.EnableMenuItem(ID_MM_SAVEASHTML,	MF_BYCOMMAND | ((SelectedItems >= 1)?(MF_ENABLED):(MF_GRAYED | MF_DISABLED)));
		}
	}

	void	SetControlsStrings()
	{
		//Set main menu caption
		lpMain->MainMenu.ModifyMenu(0, MF_BYPOSITION | MF_STRING, 0, lpMain->ML.Text(4));
		lpMain->MainMenu.ModifyMenu(1, MF_BYPOSITION | MF_STRING, 1, lpMain->ML.Text(18));
		lpMain->MainMenu.ModifyMenu(2, MF_BYPOSITION | MF_STRING, 2, lpMain->ML.Text(20));

		std::string strHotKey_Show = lpMain->ML.Text(6);
			strHotKey_Show += "\t";
			strHotKey_Show += HotKeyNormalName(hktShowWorkBar);

		std::string strHotKey_NewTask = lpMain->ML.Text(14);
			strHotKey_NewTask += "\t";
			strHotKey_NewTask += HotKeyNormalName(hktNewTask);

		std::string strHotKey_NewNote = lpMain->ML.Text(13);
			strHotKey_NewNote += "\t";
			strHotKey_NewNote += HotKeyNormalName(hktNewNote);

		//Set all menu items captions
		lpMain->MainMenu.ModifyMenu(ID_MM_HIDE,			MF_BYCOMMAND | MF_STRING, ID_MM_HIDE, strHotKey_Show.c_str());
		lpMain->MainMenu.ModifyMenu(ID_MM_SETTINGS, MF_BYCOMMAND | MF_STRING, ID_MM_SETTINGS, lpMain->ML.Text(7));
		lpMain->MainMenu.ModifyMenu(ID_MM_ABOUT,		MF_BYCOMMAND | MF_STRING, ID_MM_ABOUT, lpMain->ML.Text(8));
		lpMain->MainMenu.ModifyMenu(ID_MM_EXIT,			MF_BYCOMMAND | MF_STRING, ID_MM_EXIT, lpMain->ML.Text(9));
		lpMain->MainMenu.ModifyMenu(ID_MM_DO_NOT_HIDE,	MF_BYCOMMAND | MF_STRING, ID_MM_DO_NOT_HIDE, lpMain->ML.Text(10));
		lpMain->MainMenu.ModifyMenu(ID_MM_HELP,	MF_BYCOMMAND | MF_STRING, ID_MM_HELP, lpMain->ML.Text(67));
		lpMain->MainMenu.ModifyMenu(ID_MM_REGISTER,	MF_BYCOMMAND | MF_STRING, ID_MM_REGISTER, lpMain->ML.Text(52));
		lpMain->MainMenu.ModifyMenu(ID_MM_HOMEPAGE,	MF_BYCOMMAND | MF_STRING, ID_MM_HOMEPAGE, lpMain->ML.Text(68));
		lpMain->MainMenu.ModifyMenu(ID_MM_ONTOP,	MF_BYCOMMAND | MF_STRING, ID_MM_ONTOP, lpMain->ML.Text(84));

		lpMain->MainMenu.ModifyMenu(ID_MM_NEWTASK,	MF_BYCOMMAND | MF_STRING, ID_MM_NEWTASK, strHotKey_NewTask.c_str());
		lpMain->MainMenu.ModifyMenu(ID_MM_NEWNOTE,	MF_BYCOMMAND | MF_STRING, ID_MM_NEWNOTE, strHotKey_NewNote.c_str());
		lpMain->MainMenu.ModifyMenu(ID_MM_EDIT,	MF_BYCOMMAND | MF_STRING, ID_MM_EDIT, lpMain->ML.Text(70));
		lpMain->MainMenu.ModifyMenu(ID_MM_CATEGORIES,	MF_BYCOMMAND | MF_STRING, ID_MM_CATEGORIES, lpMain->ML.Text(25));
		lpMain->MainMenu.ModifyMenu(ID_MM_FIND,	MF_BYCOMMAND | MF_STRING, ID_MM_FIND, lpMain->ML.Text(15));
		lpMain->MainMenu.ModifyMenu(ID_MM_DELETE,	MF_BYCOMMAND | MF_STRING, ID_MM_DELETE, lpMain->ML.Text(19));
		lpMain->MainMenu.ModifyMenu(ID_MM_VIEWINBROWSER,	MF_BYCOMMAND | MF_STRING, ID_MM_VIEWINBROWSER, lpMain->ML.Text(143));
		lpMain->MainMenu.ModifyMenu(ID_MM_SAVEASHTML,	MF_BYCOMMAND | MF_STRING, ID_MM_SAVEASHTML, lpMain->ML.Text(144));
		lpMain->MainMenu.ModifyMenu(ID_MM_IMPORT,	MF_BYCOMMAND | MF_STRING, ID_MM_IMPORT, lpMain->ML.Text(178));
		lpMain->MainMenu.ModifyMenu(ID_MM_EXPORT,	MF_BYCOMMAND | MF_STRING, ID_MM_EXPORT, lpMain->ML.Text(179));

		SetViewMenuStrings();

		UpdateMenuItemsState();

		//Set zodiac locale
		lpMain->labClock.SetZodiacLocale(lpMain->ML.Text(185));

		//Set group static captions
		lpMain->grpDateAndTime.SetWindowText(lpMain->ML.Text(11));
		lpMain->grpCalendar.SetWindowText(lpMain->ML.Text(12));

		//Set tab titles
		lpMain->m_Tab.SetTitle(0, lpMain->ML.Text(21));
		lpMain->m_Tab.SetTitle(1, lpMain->ML.Text(16));
		lpMain->m_Tab.SetTitle(2, lpMain->ML.Text(17));

		SetMenuItemsStateByListItemsSelect();
	}

	void SetMenuItemString(CMenu* M, UINT MenuItemID, bool ByPos, const char* Text, bool RadioItem = false)
	{
		static MENUITEMINFO MI;
		MI.cbSize = sizeof(MENUITEMINFO);
		MI.fMask = MIIM_TYPE | MIIM_CHECKMARKS;
		MI.fType = MFT_STRING | ((RadioItem)?MFT_RADIOCHECK:0);
		MI.dwTypeData = (char*)Text;
		MI.cch = (int)strlen(Text);
		MI.hbmpChecked = NULL;
		M->SetMenuItemInfo(MenuItemID, &MI, ByPos);
	}

	void SetViewMenuStrings()
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		CMenu* pViewMenu = Dlg.MainMenu.GetSubMenu(2);	//get view submenu
		if (pViewMenu)
		{
			SetMenuItemString(pViewMenu, ID_VIEW_ALL, FALSE, Dlg.ML.Text(95));
			SetMenuItemString(pViewMenu, ID_BYCATEGORIES_ALL, FALSE, Dlg.ML.Text(96));
			SetMenuItemString(pViewMenu, ID_BYPROGRESS_ALL, FALSE, Dlg.ML.Text(96));
			SetMenuItemString(pViewMenu, ID_BYPRIORITY_ALL, FALSE, Dlg.ML.Text(96));
			SetMenuItemString(pViewMenu, ID_BYVISIBILITY_ALL, FALSE, Dlg.ML.Text(96));
			SetMenuItemString(pViewMenu, ID_BYADDINGDATE_ALL, FALSE, Dlg.ML.Text(96));
			SetMenuItemString(pViewMenu, ID_BYCHANGINGDATE_ALL, FALSE, Dlg.ML.Text(96));

			SetMenuItemString(pViewMenu, 2, TRUE, Dlg.ML.Text(97));	//by categories
			SetMenuItemString(pViewMenu, 3, TRUE, Dlg.ML.Text(98));	//by progress
			SetMenuItemString(pViewMenu, 4, TRUE, Dlg.ML.Text(99));	//by priority
			SetMenuItemString(pViewMenu, 5, TRUE, Dlg.ML.Text(100));	//by visibility
			SetMenuItemString(pViewMenu, 6, TRUE, Dlg.ML.Text(101));	//by adding
			SetMenuItemString(pViewMenu, 7, TRUE, Dlg.ML.Text(102));	//by modifing

			//Setup progress submenu
			SetMenuItemString(pViewMenu, ID_BYPROGRESS_NOTDONE, FALSE, Dlg.ML.Text(103), true);
			SetMenuItemString(pViewMenu, ID_BYPROGRESS_PARTIALLYDONE, FALSE, Dlg.ML.Text(104), true);
			SetMenuItemString(pViewMenu, ID_BYPROGRESS_FINISHED, FALSE, Dlg.ML.Text(105), true);
			//Setup priority submenu
			SetMenuItemString(pViewMenu, ID_BYPRIORITY_IMPORTANT, FALSE, Dlg.ML.Text(106), true);
			SetMenuItemString(pViewMenu, ID_BYPRIORITY_DEFAULT, FALSE, Dlg.ML.Text(107), true);
			SetMenuItemString(pViewMenu, ID_BYPRIORITY_NOTIMPORTANT, FALSE, Dlg.ML.Text(108), true);
			//Setup visibility submenu
			SetMenuItemString(pViewMenu, ID_BYVISIBILITY_THISMONTH, FALSE, Dlg.ML.Text(109), true);
			SetMenuItemString(pViewMenu, ID_BYVISIBILITY_THISWEEK, FALSE, Dlg.ML.Text(110), true);
			SetMenuItemString(pViewMenu, ID_BYVISIBILITY_YESTERDAY, FALSE, Dlg.ML.Text(111), true);
			SetMenuItemString(pViewMenu, ID_BYVISIBILITY_TODAY, FALSE, Dlg.ML.Text(112), true);
			//Setup addingdate submenu
			SetMenuItemString(pViewMenu, ID_BYADDINGDATE_THISMONTH, FALSE, Dlg.ML.Text(109), true);
			SetMenuItemString(pViewMenu, ID_BYADDINGDATE_THISWEEK, FALSE, Dlg.ML.Text(110), true);
			SetMenuItemString(pViewMenu, ID_BYADDINGDATE_YESTERDAY, FALSE, Dlg.ML.Text(111), true);
			SetMenuItemString(pViewMenu, ID_BYADDINGDATE_TODAY, FALSE, Dlg.ML.Text(112), true);
			//Setup changingdate submenu
			SetMenuItemString(pViewMenu, ID_BYCHANGINGDATE_THISMONTH, FALSE, Dlg.ML.Text(109), true);
			SetMenuItemString(pViewMenu, ID_BYCHANGINGDATE_THISWEEK, FALSE, Dlg.ML.Text(110), true);
			SetMenuItemString(pViewMenu, ID_BYCHANGINGDATE_YESTERDAY, FALSE, Dlg.ML.Text(111), true);
			SetMenuItemString(pViewMenu, ID_BYCHANGINGDATE_TODAY, FALSE, Dlg.ML.Text(112), true);
		}
	}

	std::string	HotKeyNormalName(int HKT)
	{
		if (HotKeys[HKT].on)
		{
			WORD	VKey = HotKeys[HKT].VKey;
			WORD	Mod = HotKeys[HKT].Modifier;
			return Misc::GetHotKeyNormalName(VKey, Mod);
		} else {
			return "";
		}
	}

	void	SetWindowTransparency()
	{
		/*
		if (lpMain->Settings.OpaquePercent == 100)
		{
			lpMain->LayeredDlg.RemoveLayeredStyle(lpMain->GetSafeHwnd());
		} else {
			lpMain->LayeredDlg.AddLayeredStyle(lpMain->GetSafeHwnd());
			lpMain->LayeredDlg.SetTransparentPercentage(lpMain->GetSafeHwnd(), lpMain->Settings.OpaquePercent);
		}
		*/
	}

	void	ResizeControlsIfNeeded(int W, int H)
	{
		//Refresh only on size change !
		static int SizeW = 0;
		static int SizeH = 0;
		if ((SizeW != W) || (SizeH != H))
			ResizeControls();
		SizeW = W;
		SizeH = H;
	}

	void	ResizeControls()
	{
		CWinWorkBarDlg& Dlg = *lpMain;
	
		CRect	ClientR;
		CRect	posR;
		CRect	grpDateAndTimeR;
		CRect	grpCalendarR;
		CRect TabR;

		Dlg.GetClientRect(&ClientR);

		HDWP PosHwnd = BeginDeferWindowPos(7);
		if (!PosHwnd)
			return;

		//Pos grpDateAndTime
		posR = ClientR;
		posR.left += DefaultMargin;
		posR.right -= DefaultMargin;
		posR.top += DefaultMargin;
		posR.bottom = posR.top + grpDateAndTime_Height;
		grpDateAndTimeR = posR;
		if (!(PosHwnd == DeferWindowPos(PosHwnd, Dlg.grpDateAndTime.GetSafeHwnd(), NULL, posR.left, posR.top, posR.Width(), posR.Height(), SWP_NOZORDER)))
			return;

		//Pos grpCalendar
		posR.top += posR.bottom;
		posR.bottom = posR.top + grpCalendar_Height;
		grpCalendarR = posR;
		if (!(PosHwnd == DeferWindowPos(PosHwnd, Dlg.grpCalendar.GetSafeHwnd(), NULL, posR.left, posR.top, posR.Width(), posR.Height(), SWP_NOZORDER)))
			return;
		
		//Pos Tab
		posR.top = posR.bottom + DefaultMargin + (DefaultMargin >> 1);
		posR.bottom = ClientR.bottom - DefaultMargin;
		TabR = posR;
		if (!(PosHwnd == DeferWindowPos(PosHwnd, Dlg.m_Tab.GetSafeHwnd(), NULL, posR.left, posR.top, posR.Width(), posR.Height(), SWP_NOZORDER)))
			return;

		//labClock
		CRect labR = grpDateAndTimeR;
		labR.top += labMarginTop;
		labR.bottom = labR.top + 78;
		labR.left += labMargin;
		labR.right -= labMargin;
		if (!(PosHwnd == DeferWindowPos(PosHwnd, Dlg.labClock.GetSafeHwnd(), NULL, labR.left, labR.top, labR.Width(), labR.Height(), SWP_NOZORDER)))
			return;

		//Set calendard(s)

		//label Calendar
		CRect RCal = grpCalendarR;
		RCal.top += labMarginTop;
		RCal.bottom -= labMargin;
		RCal.left += labMargin;
		RCal.right -= labMargin;

		CRect RCal1 = RCal;
		CRect RCal2 = RCal;

		const int DefaultCalendarWidth = 190;
		int iShowCalendars = 1;
		if (RCal.Width() > ((DefaultCalendarWidth + DefaultMargin) * 2))
			iShowCalendars = 2;
		if (RCal.Width() > ((DefaultCalendarWidth + DefaultMargin) * 3))
			iShowCalendars = 3;

		UINT Calendar1_visibleflag = (iShowCalendars >= 2)?SWP_SHOWWINDOW:SWP_HIDEWINDOW;
		UINT Calendar2_visibleflag = (iShowCalendars == 3)?SWP_SHOWWINDOW:SWP_HIDEWINDOW;

		if (iShowCalendars == 2)
		{
			RCal1.right = (RCal.Width() / 2) + (DefaultMargin * 2);
			RCal.left = RCal1.right + DefaultMargin;
		}
		if (iShowCalendars == 3)
		{
			RCal1.right = (RCal.Width() / 3) + (DefaultMargin * 3);
			RCal.left = RCal1.right + DefaultMargin;
			RCal.right = RCal.left + RCal1.Width();
			RCal2.left = RCal.right + DefaultMargin;
		}

		//label Calendar
		if (!(PosHwnd == DeferWindowPos(PosHwnd, Dlg.labCalendar.GetSafeHwnd(), NULL, RCal.left, RCal.top, RCal.Width(), RCal.Height(), SWP_NOZORDER)))		
			return;

		//label Calendar1
		if (!(PosHwnd == DeferWindowPos(PosHwnd, Dlg.labCalendarL1.GetSafeHwnd(), NULL, RCal1.left, RCal1.top, RCal1.Width(), RCal1.Height(), SWP_NOZORDER | Calendar1_visibleflag)))
			return;

		//label Calendar2
		if (!(PosHwnd == DeferWindowPos(PosHwnd, Dlg.labCalendarR1.GetSafeHwnd(), NULL, RCal2.left, RCal2.top, RCal2.Width(), RCal2.Height(), SWP_NOZORDER | Calendar2_visibleflag)))
			return;

		EndDeferWindowPos(PosHwnd);
	}

	void SetOtherCalendars()
	{
		CWinWorkBarDlg& Dlg = *lpMain;

		//update second and third calendars
		int iMonth = Dlg.labCalendar.StartMonth();
		int iYear = Dlg.labCalendar.StartYear();

		int iPrevYear = iYear;
		int iNextYear = iYear;

		int iPrevMonth = iMonth - 1;
		if (iPrevMonth < 1)
		{
			iPrevMonth = 12;
			iPrevYear--;
		}

		int iNextMonth = iMonth + 1;
		if (iNextMonth > 12)
		{
			iNextMonth = 1;
			iNextYear++;
		}
		
		SYSTEMTIME ST = Dlg.labCalendar.GetToday();

		Dlg.labCalendarL1.SelectDay(ST);
		Dlg.labCalendarL1.SetToday(ST);
		Dlg.labCalendarR1.SelectDay(ST);
		Dlg.labCalendarR1.SetToday(ST);

		Dlg.labCalendarL1.ShowAt(iPrevMonth, iPrevYear);
		Dlg.labCalendarR1.ShowAt(iNextMonth, iNextYear);
	}

	void	UpdateDateAndTimeLabels()
	{
		static bool bLock = false;
		if (bLock)
			return;
		bLock = true;

		CWinWorkBarDlg& Dlg = *lpMain;

		//Clock
		Dlg.labClock.iStyle = Dlg.Settings.iClockStyle;
		Dlg.labClock.b24Clock = Dlg.Settings.Time24Hour;
		for (int i = 0; i < 7; i++)
			Dlg.labClock.FreeDays[i] = Dlg.labCalendar.FreeDays[i];
		Dlg.labClock.Refresh();

		//Updating cal date !
		SYSTEMTIME ST;
		GetLocalTime(&ST);
		Dlg.dlgToday.labTaskView.SetCurrentTime(ST);

		static SYSTEMTIME LastTime = { 0, 0, 0, 0, 0, 0, 0, 0 };
		if ((LastTime.wYear != ST.wYear) || (LastTime.wMonth != ST.wMonth) || (LastTime.wDay != ST.wDay))
		{
			LastTime = ST;

			//UPDATES INFO !!!
			Dlg.labCalendar.UpdateToday();
			Dlg.labCalendar.SelectDayToday();

			SetOtherCalendars();

			Dlg.dlgToday.TaskReminder.ResetForNewDay();

			//Redraw all
			Dlg.grpCalendar.RedrawWindow();
		}

		Misc::ProcessMessages();
		bLock = false;
	}

	void	DaySelected(const SYSTEMTIME& ST)
	{
		CWinWorkBarDlg& Dlg = *lpMain;

		//Show first tab date title
		std::string DateNames[5];
		DateNames[0] = Dlg.ML.Text(21);
		DateNames[1] = Dlg.ML.Text(22);
		DateNames[2] = Dlg.ML.Text(23);
		DateNames[3] = Dlg.ML.Text(24);

		std::string FirstTabTitle = Date::DateAsTextInfo(Dlg.labCalendar.GetToday(), Dlg.labCalendar.GetSelectedDay(), DateNames);

		Dlg.dlgToday.LoadListItems();

		Dlg.m_Tab.SetTitle(0, FirstTabTitle);
	}

	void InitCommandLine()
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		//Convert command line to vector
		Dlg.m_CmdParams.clear();
		for (int i = 1; i < __argc; i++)
			Dlg.m_CmdParams.push_back(__argv[i]);
	}

	bool RunByCommandLine(bool RunInNewProcess)
	{
		bool Result = false;
		CWinWorkBarDlg& Dlg = *lpMain;
		if ((int)Dlg.m_CmdParams.size() > 0)
		{
			//First command line param: FILENAME (or createlangfiles)
			std::string Param1 = Dlg.m_CmdParams[0];

			if (Param1.size() > 0)
			{
				if (CreateLangFiles(Param1))
					return true;
			}
		}
		return Result;
	}

	bool CreateLangFiles(std::string Param)
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		if (Param == ":langfiles")
		{
			Dlg.ML.SetLanguage("eng");
			Dlg.ML.SaveToFile(CREATE_NEW);
			Dlg.ML.SetLanguage("plk");
			Dlg.ML.SaveToFile(CREATE_NEW);
			PostQuitMessage(0);
			return true;
		}
		return false;
	}

	void	MenuItem_SetCheckMarks(HMENU hMenu, int ID, int BmpIdOn, int BmpIdOff)
	{
		HBITMAP	MB = (BmpIdOn)?LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(BmpIdOn)):NULL;
		HBITMAP	MB_off	= (BmpIdOff)?LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(BmpIdOff)):NULL;
		SetMenuItemBitmaps(hMenu, ID, MF_BYCOMMAND, MB_off, MB);
	}

	void OnTrayMenuShow(HMENU TM)
	{
		std::string strHotKey_Show = lpMain->ML.Text(69);
			strHotKey_Show += "\t";
			strHotKey_Show += HotKeyNormalName(hktShowWorkBar);

		std::string strHotKey_NewTask = lpMain->ML.Text(14);
			strHotKey_NewTask += "\t";
			strHotKey_NewTask += HotKeyNormalName(hktNewTask);

		std::string strHotKey_NewNote = lpMain->ML.Text(13);
			strHotKey_NewNote += "\t";
			strHotKey_NewNote += HotKeyNormalName(hktNewNote);

		//Set all menu items captions
		ModifyMenu(TM, ID_TRAYMENU_NEWTASK, MF_BYCOMMAND | MF_STRING, ID_TRAYMENU_NEWTASK, strHotKey_NewTask.c_str());
		ModifyMenu(TM, ID_TRAYMENU_NEWNOTE, MF_BYCOMMAND | MF_STRING, ID_TRAYMENU_NEWNOTE, strHotKey_NewNote.c_str());
		ModifyMenu(TM, ID_TRAYMENU_SHOWHIDE, MF_BYCOMMAND | MF_STRING, ID_TRAYMENU_SHOWHIDE, strHotKey_Show.c_str());

		ModifyMenu(TM, ID_TRAYMENU_SETTINGS, MF_BYCOMMAND | MF_STRING, ID_TRAYMENU_SETTINGS, lpMain->ML.Text(7));
		ModifyMenu(TM, ID_TRAYMENU_ABOUT, MF_BYCOMMAND | MF_STRING, ID_TRAYMENU_ABOUT, lpMain->ML.Text(8));
		ModifyMenu(TM, ID_TRAYMENU_CLOSE, MF_BYCOMMAND | MF_STRING, ID_TRAYMENU_CLOSE, lpMain->ML.Text(9));

		//Set menu items images
		CMenuXP::SetMenuItemImage(ID_TRAYMENU_NEWTASK, lpMain->imgListMainMenu, 0);
		CMenuXP::SetMenuItemImage(ID_TRAYMENU_NEWNOTE, lpMain->imgListMainMenu, 1);
		CMenuXP::SetMenuItemImage(ID_TRAYMENU_ABOUT, lpMain->imgListMainMenu, 2);
		CMenuXP::SetMenuItemImage(ID_TRAYMENU_CLOSE, lpMain->imgListMainMenu, 3);
		CMenuXP::SetMenuItemImage(ID_TRAYMENU_SETTINGS, lpMain->imgListMainMenu, 4);
		CMenuXP::SetMenuItemImage(ID_TRAYMENU_SHOWHIDE, lpMain->imgListMainMenu, 5);

		::SetMenuDefaultItem(TM, ID_TRAYMENU_SHOWHIDE, FALSE);
	}

	void OnTrayEvent(LPARAM lParam)
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
		{
			if (WorkBarVisible())
			{
				HideWorkBar(true);
			} else {
				ShowWorkBar();
				if (!lpMain->Settings.WindowTopmost)
				{
					lpMain->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
					lpMain->BringWindowToTop();
					lpMain->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
				}
				Dlg.StartHideTimer();
			}
		}
	}

	HDWP ResizeTabButton(HDWP PosHwnd, CXPStyleButtonST& Btn, CRect& posR, int& btnLeft)
	{
		CRect btnR = posR;
		btnR.left = btnLeft + DefaultTabBarMargin;
		btnR.right = btnLeft + DefaultTabBtnWidth;
		btnLeft += DefaultTabBtnWidth;
		return DeferWindowPos(PosHwnd, Btn.GetSafeHwnd(), NULL, btnR.left, btnR.top, btnR.Width(), btnR.Height(), SWP_NOZORDER);
	}

	void SetupXPButton(CXPStyleButtonST& Btn, int IconResID, int LangID)
	{
		Btn.SetThemeHelper(&lpMain->m_ThemeHelper);
		Btn.SetAlign(CButtonST::ST_ALIGN_VERT);
		Btn.SetIcon(IconResID);
		Btn.SetTooltipText(lpMain->ML.Text(LangID));
		Btn.DrawAsToolbar(true);
	}

	void SetupRichEditForNote(CRichEditCtrl& Ed)
	{
		Ed.SetOptions(ECOOP_SET, ECO_AUTOWORDSELECTION | ECO_AUTOVSCROLL | ECO_AUTOHSCROLL | ECO_WANTRETURN);
		//W³¹czam obs³ugê URLi
		Ed.SetAutoURLDetect();
		long RichEdMask = Ed.GetEventMask();
		Ed.SetEventMask(RichEdMask | ENM_CHANGE | ENM_LINK | ENM_MOUSEEVENTS | ENM_KEYEVENTS);
		//Wy³¹czam opcjê IMF_AUTOFONT w EM_SETLANGOPTIONS, która powodowa³a prze³¹czanie fontów przy literce 'ñ' !
		LRESULT LangOptions = Ed.SendMessage(EM_GETLANGOPTIONS, 0, 0);
		Ed.SendMessage(EM_SETLANGOPTIONS, 0, LangOptions & (~IMF_AUTOFONT));
		//Wy³¹czam obs³ugê obiektów OLE - dla obs³ugi kopiowania ze schowka
		Ed.SetTextMode(TM_PLAINTEXT);
		//Maksymalnie 1MB znaków w oknie
		Ed.LimitText(1024 * 1024);
	}

	void RunRichEditLink(CRichEditCtrl& Ed, ENLINK *pEnLink)
	{
		if (pEnLink->msg == WM_LBUTTONUP)
		{
			CString LinkStr;
			Ed.GetTextRange(pEnLink->chrg.cpMin, pEnLink->chrg.cpMax, LinkStr);
			ShellExecute(NULL, "open", LinkStr, NULL, NULL, SW_SHOWNORMAL);
		}
	}

	//TinyXML TABLE Default loads
	void	LoadAndResetTable_Categories(bool CleanTable)
	{
		PSXML::PSTinyXML& TB = lpMain->tbCategories;
		TB.SetProgramData(ProgramName, Misc::GetAppVersion(false), lpMain->Settings.DataFolder);
		if (!TB.LoadTable("Categories", FieldList_categories))
			TB.SaveTable();
		if (TB.LoadTable("Categories", FieldList_categories))
		{
			if (CleanTable)
				TB.RemoveAll();
			if (TB.HasNoRows())
			{
				int LangItemStartID = 33;
				int LangItemEndID = 45;
				for (int i = LangItemStartID; i < (LangItemStartID + 32); i++)
				{
					int ID = i - LangItemStartID + 1;
					std::string S = "";
					if (i <= LangItemEndID)
						S = lpMain->ML.Text(i);
					TB.SetID(ID);
					TB.FieldByName("Name").Set(S);
					TB.FieldByName("Date").SetNowDate();
					TB.AddRow();
				}
				TB.SaveTable();
			}
		} else {
			std::string S = lpMain->ML.Text(30);
			S+= ":\n";
			ShowError(S + TB.TableFileName());
		}
	}

	void	LoadAndResetTable_Notes(bool CleanTable)
	{
		PSXML::PSTinyXML& TB = lpMain->tbNotes;
		TB.SetProgramData(ProgramName, Misc::GetAppVersion(false), lpMain->Settings.DataFolder);
		if (!TB.LoadTable("Notes", FieldList_notes))
			TB.SaveTable();
		if (TB.LoadTable("Notes", FieldList_notes))
		{
			if (CleanTable)
				TB.RemoveAll();
			if (TB.HasNoRows())
			{
				if (ImportData(TB))
					TB.SaveTable();
			}
		} else {
			std::string S = lpMain->ML.Text(30);
			S+= ":\n";
			ShowError(S + TB.TableFileName());
		}
	}

	void	LoadAndResetTable_Tasks(bool CleanTable)
	{
		PSXML::PSTinyXML& TB = lpMain->tbTasks;
		TB.SetProgramData(ProgramName, Misc::GetAppVersion(false), lpMain->Settings.DataFolder);
		if (!TB.LoadTable("Tasks", FieldList_tasks))
			TB.SaveTable();
		if (TB.LoadTable("Tasks", FieldList_tasks))
		{
			if (CleanTable)
				TB.RemoveAll();
			if (TB.HasNoRows())
			{
				if (lpMain->Settings.bFirstRun)
				{
					//Add starter task
					TB.ClearFields();
					TB.SetUID();
					TB.FieldByName("Text").Set(lpMain->ML.Text(177));
					TB.FieldByName("Date").SetNowDate();
					TB.FieldByName("Modified").Clear();
					TB.FieldByName("Categories").Set(0);
					TB.FieldByName("Repeat").Set(0);
					TB.FieldByName("StartDate").SetNowDate();
					TB.FieldByName("Priority").Set(0);
					TB.FieldByName("Progress").Set(0);
					TB.AddRow();
					TB.SaveTable();
					lpMain->Settings.bFirstRun = false;
				}

				if (ImportData(TB))
					TB.SaveTable();
			}
		} else {
			std::string S = lpMain->ML.Text(30);
			S+= ":\n";
			ShowError(S + TB.TableFileName());
		}
	}

	void FormStringToListDisplay(std::string& S, int MaxChars)
	{
		if ((int)S.length() > MaxChars)
			S.erase(MaxChars);
		S = Text::Replace(S, "\r\n", " ");
		S = Text::Replace(S, "\t", "");
	}

	void ShowRichEditContextMenu(CDialog& Dlg, MSGFILTER *pMsgFilter, CRichEditCtrl& RED, int EditResource)
	{
		if ((pMsgFilter->msg == WM_RBUTTONUP) || ((pMsgFilter->msg == WM_KEYUP) && (pMsgFilter->wParam == VK_APPS)))
		{
			Language::Lang& ML = lpMain->ML;
			CHARRANGE CR;
			RED.GetSel(CR);
			POINT PT;
			::GetCursorPos(&PT);
			if (pMsgFilter->msg == WM_KEYUP)
			{
				CPoint P;
				//Windows NT/2000/XP
				//this is shitty richedit version test
				if (GetVersion() < 0x80000000)
				{
					P = RED.PosFromChar(CR.cpMin);
				} else {
					P.SetPoint(0, 0);
				}
				PT = P;
				RED.ClientToScreen(&PT);
			}
			HMENU hmenu;
			if ((hmenu = LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(EditResource))))
			{
				HMENU hmenuTrackPopup = GetSubMenu(hmenu, 0);

				//Localize menu items
				std::string strCalcItem = lpMain->ML.Text(176);
				strCalcItem += "\t";
				strCalcItem += CHotKeyCtrl::GetKeyName(VK_CONTROL, FALSE);
				strCalcItem += " +  ";
				strCalcItem += CHotKeyCtrl::GetKeyName(VK_RETURN, FALSE);

				ModifyMenu(hmenuTrackPopup, ID_EDM_CALCULATE, MF_BYCOMMAND | MF_STRING, ID_EDM_CALCULATE, strCalcItem.c_str());
				ModifyMenu(hmenuTrackPopup, ID_EDM_INSERT_WWWLINK, MF_BYCOMMAND | MF_STRING, ID_EDM_INSERT_WWWLINK, ML.Text(85));
				ModifyMenu(hmenuTrackPopup, ID_EDM_INSERT_EMAILLINK, MF_BYCOMMAND | MF_STRING, ID_EDM_INSERT_EMAILLINK, ML.Text(86));
				ModifyMenu(hmenuTrackPopup, ID_EDM_INSERT_FILELINK, MF_BYCOMMAND | MF_STRING, ID_EDM_INSERT_FILELINK, ML.Text(87));
				ModifyMenu(hmenuTrackPopup, ID_EDM_INSERT_DATE, MF_BYCOMMAND | MF_STRING, ID_EDM_INSERT_DATE, ML.Text(88));
				ModifyMenu(hmenuTrackPopup, ID_EDM_INSERT_LONGDATE, MF_BYCOMMAND | MF_STRING, ID_EDM_INSERT_LONGDATE, ML.Text(89));
				ModifyMenu(hmenuTrackPopup, ID_EDM_INSERT_TIME, MF_BYCOMMAND | MF_STRING, ID_EDM_INSERT_TIME, ML.Text(90));
				ModifyMenu(hmenuTrackPopup, ID_EDM_INSERT_DATETIME, MF_BYCOMMAND | MF_STRING, ID_EDM_INSERT_DATETIME, ML.Text(91));
				//Display menu
				int Out = TrackPopupMenu(hmenuTrackPopup, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, PT.x, PT.y, 0, Dlg.GetSafeHwnd(), NULL);
				DestroyMenu(hmenu);
				std::string S;
				switch (Out)
				{
					case ID_EDM_CALCULATE:					DoEvaluateExpression(RED, Dlg);	break;
					case ID_EDM_INSERT_WWWLINK:			S = "http://"; break;
					case ID_EDM_INSERT_EMAILLINK:		S = "mailto:"; break;
					case ID_EDM_INSERT_FILELINK:		S = "file:";	 break;
					case ID_EDM_INSERT_DATE:				S = Text::CurrentLocalDate("");			break;
					case ID_EDM_INSERT_LONGDATE:		S = Text::CurrentLocalDateLong("");	break;
					case ID_EDM_INSERT_TIME:				S = Text::CurrentLocalTime("");			break;
					case ID_EDM_INSERT_DATETIME:		S = Text::CurrentLocalDateTime();		break;
				}
				RED.ReplaceSel(S.c_str(), true);
			}
		}
	}

	void SetMenuItemRadioCheckType(CMenu* pMenu, UINT nIDItem)
	{
		if (pMenu)
		{
			static MENUITEMINFO MII;
			static TCHAR S[128];
			MII.cbSize = sizeof(MENUITEMINFO);
			MII.fMask = MIIM_CHECKMARKS | MIIM_TYPE;
			MII.fType = MFT_RADIOCHECK;
			MII.hbmpChecked = NULL;
			S[0] = 0;
			pMenu->GetMenuString(nIDItem, S, 128, MF_BYCOMMAND);
			MII.dwTypeData = S;
			MII.cch = (int)strlen(S);
			pMenu->SetMenuItemInfo(nIDItem, &MII);
		}
	}

	void SetupViewMenu()
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		PSXML::PSTinyXML& TB = Dlg.tbCategories;
		//Reload categories menu
		CMenu* pViewMenu = Dlg.MainMenu.GetSubMenu(2);	//get view submenu
		if (pViewMenu)
		{
			CMenu* pCatSubMenu = pViewMenu->GetSubMenu(2);
			if (pCatSubMenu)
			{
				//Delete all menu items with command
				for (int i = MenuItemStartCmdID; i < MenuItemStopCmdID; i++)
					pCatSubMenu->DeleteMenu(i, MF_BYCOMMAND);
				//Add categories
				MENUITEMINFO MIIC;
				MIIC.cbSize = sizeof(MENUITEMINFO);
				MIIC.fMask = MIIM_TYPE | MIIM_ID;
				MIIC.fType = MFT_STRING;
				TB.First();
				while (!TB.IsEof())
				{
					TB.GetData();
					std::string CatName = TB.FieldByName("Name").AsString();
					int CatID = TB.GetIDAsInt();
					if (!CatName.empty())
					{
						MIIC.wID = MenuItemStartCmdID + CatID;
						MIIC.dwTypeData = (char*)CatName.c_str();
						MIIC.cch = (int)CatName.size();
						pCatSubMenu->InsertMenuItem(-1, &MIIC, FALSE);
					}
					TB.Next();
				}
			}
			//Set checkmark type for items "all"
			MENUITEMINFO MII;
			ZeroMemory(&MII, sizeof(MENUITEMINFO));
			MII.cbSize = sizeof(MENUITEMINFO);
			MII.fMask = MIIM_CHECKMARKS;
			MII.hbmpChecked = NULL;
			pViewMenu->SetMenuItemInfo(ID_VIEW_ALL, &MII);
			pViewMenu->SetMenuItemInfo(ID_BYCATEGORIES_ALL, &MII);
			pViewMenu->SetMenuItemInfo(ID_BYPROGRESS_ALL, &MII);
			pViewMenu->SetMenuItemInfo(ID_BYPRIORITY_ALL, &MII);
			pViewMenu->SetMenuItemInfo(ID_BYVISIBILITY_ALL, &MII);
			pViewMenu->SetMenuItemInfo(ID_BYADDINGDATE_ALL, &MII);
			pViewMenu->SetMenuItemInfo(ID_BYCHANGINGDATE_ALL, &MII);
			//Set checkmark type for category items
			if (pCatSubMenu)
			{
				for (int i = MenuItemStartCmdID; i < MenuItemStopCmdID; i++)
					pCatSubMenu->SetMenuItemInfo(i, &MII, FALSE);
			}
			//Set radio items - progress
			SetMenuItemRadioCheckType(pViewMenu, ID_BYPROGRESS_NOTDONE);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYPROGRESS_PARTIALLYDONE);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYPROGRESS_FINISHED);
			//Set radio items - priority
			SetMenuItemRadioCheckType(pViewMenu, ID_BYPRIORITY_IMPORTANT);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYPRIORITY_DEFAULT);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYPRIORITY_NOTIMPORTANT);
			//Set radio items - visibility
			SetMenuItemRadioCheckType(pViewMenu, ID_BYVISIBILITY_THISMONTH);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYVISIBILITY_THISWEEK);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYVISIBILITY_YESTERDAY);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYVISIBILITY_TODAY);
			//Set radio items - addingdate
			SetMenuItemRadioCheckType(pViewMenu, ID_BYADDINGDATE_THISMONTH);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYADDINGDATE_THISWEEK);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYADDINGDATE_YESTERDAY);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYADDINGDATE_TODAY);
			//Set radio items - changingdate
			SetMenuItemRadioCheckType(pViewMenu, ID_BYCHANGINGDATE_THISMONTH);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYCHANGINGDATE_THISWEEK);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYCHANGINGDATE_YESTERDAY);
			SetMenuItemRadioCheckType(pViewMenu, ID_BYCHANGINGDATE_TODAY);
		}
		SetViewMenuState(Dlg.Settings.ViewFilter);
	}

	void SetViewMenuState(DataViewFilter& DVF)
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		CMenu* pViewMenu = Dlg.MainMenu.GetSubMenu(2);	//get view submenu
		if (pViewMenu)
		{
			//Setup main menu item - ALL
			pViewMenu->CheckMenuItem(ID_VIEW_ALL, MF_BYCOMMAND | (DVF.All())?MF_CHECKED:MF_UNCHECKED);
			//Setup categories submenu
			CMenu* pCatSubMenu = pViewMenu->GetSubMenu(2);	//get categories submenu
			if (pCatSubMenu)
			{
				pCatSubMenu->CheckMenuItem(ID_BYCATEGORIES_ALL, MF_BYCOMMAND | (DVF.dwSelectedCategories)?MF_UNCHECKED:MF_CHECKED);
				for (int i = 0; i < (int)pCatSubMenu->GetMenuItemCount(); i++)
				{
					UINT ItemID = pCatSubMenu->GetMenuItemID(i);
					if (ItemID == ID_BYCATEGORIES_ALL)
						continue;
					//Get menu item CatID
					int CatID = ItemID - MenuItemStartCmdID;
					//Check if CatID is in DWORD selected
					bool CatSelected = (DVF.dwSelectedCategories & (DWORD)((DWORD)1 << (CatID - 1))) > 0;
					pCatSubMenu->CheckMenuItem(i, MF_BYPOSITION | ((CatSelected)?MF_CHECKED:MF_UNCHECKED));
				}
			}
			//Setup progress submenu
			pViewMenu->CheckMenuItem(ID_BYPROGRESS_ALL, MF_BYCOMMAND | (DVF.iProgress)?MF_UNCHECKED:MF_CHECKED);
			pViewMenu->CheckMenuItem(ID_BYPROGRESS_NOTDONE, MF_BYCOMMAND | (DVF.iProgress == 1)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYPROGRESS_PARTIALLYDONE, MF_BYCOMMAND | (DVF.iProgress == 2)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYPROGRESS_FINISHED, MF_BYCOMMAND | (DVF.iProgress == 3)?MF_CHECKED:MF_UNCHECKED);
			//Setup priority submenu
			pViewMenu->CheckMenuItem(ID_BYPRIORITY_ALL, MF_BYCOMMAND | (DVF.iPriority)?MF_UNCHECKED:MF_CHECKED);
			pViewMenu->CheckMenuItem(ID_BYPRIORITY_IMPORTANT, MF_BYCOMMAND | (DVF.iPriority == 1)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYPRIORITY_DEFAULT, MF_BYCOMMAND | (DVF.iPriority == 2)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYPRIORITY_NOTIMPORTANT, MF_BYCOMMAND | (DVF.iPriority == 3)?MF_CHECKED:MF_UNCHECKED);
			//Setup visibility submenu
			pViewMenu->CheckMenuItem(ID_BYVISIBILITY_ALL, MF_BYCOMMAND | (DVF.iVisibility)?MF_UNCHECKED:MF_CHECKED);
			pViewMenu->CheckMenuItem(ID_BYVISIBILITY_THISMONTH, MF_BYCOMMAND | (DVF.iVisibility == 1)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYVISIBILITY_THISWEEK, MF_BYCOMMAND | (DVF.iVisibility == 2)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYVISIBILITY_YESTERDAY, MF_BYCOMMAND | (DVF.iVisibility == 3)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYVISIBILITY_TODAY, MF_BYCOMMAND | (DVF.iVisibility == 4)?MF_CHECKED:MF_UNCHECKED);
			//Setup addingdate submenu
			pViewMenu->CheckMenuItem(ID_BYADDINGDATE_ALL, MF_BYCOMMAND | (DVF.iAdded)?MF_UNCHECKED:MF_CHECKED);
			pViewMenu->CheckMenuItem(ID_BYADDINGDATE_THISMONTH, MF_BYCOMMAND | (DVF.iAdded == 1)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYADDINGDATE_THISWEEK, MF_BYCOMMAND | (DVF.iAdded == 2)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYADDINGDATE_YESTERDAY, MF_BYCOMMAND | (DVF.iAdded == 3)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYADDINGDATE_TODAY, MF_BYCOMMAND | (DVF.iAdded == 4)?MF_CHECKED:MF_UNCHECKED);
			//Setup changingdate submenu
			pViewMenu->CheckMenuItem(ID_BYCHANGINGDATE_ALL, MF_BYCOMMAND | (DVF.iModified)?MF_UNCHECKED:MF_CHECKED);
			pViewMenu->CheckMenuItem(ID_BYCHANGINGDATE_THISMONTH, MF_BYCOMMAND | (DVF.iModified == 1)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYCHANGINGDATE_THISWEEK, MF_BYCOMMAND | (DVF.iModified == 2)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYCHANGINGDATE_YESTERDAY, MF_BYCOMMAND | (DVF.iModified == 3)?MF_CHECKED:MF_UNCHECKED);
			pViewMenu->CheckMenuItem(ID_BYCHANGINGDATE_TODAY, MF_BYCOMMAND | (DVF.iModified == 4)?MF_CHECKED:MF_UNCHECKED);
		}
	}

	void GetViewMenuItemState(CMenu* M, UINT mItemID, int& Variable, int ValueifChecked)
	{
		if ((M->GetMenuState(mItemID, MF_BYCOMMAND) & MF_CHECKED) == MF_CHECKED)
			Variable = ValueifChecked;
	}

	void GetViewMenuState_OnCommand(WPARAM wParam, LPARAM lParam)
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		//Is command from menu ?
		if (HIWORD(wParam) == 0)
		{
			DataViewFilter& DVF = Dlg.Settings.ViewFilter;
			DataViewFilter DVFcopy = DVF;
			WORD IDItem = LOWORD(wParam);
			//Get categories submenu value
			if ((IDItem >= MenuItemStartCmdID) && (IDItem <= MenuItemStopCmdID))
			{
				CMenu* pViewMenu = Dlg.MainMenu.GetSubMenu(2);	//get view submenu
				if (pViewMenu)
				{
					CMenu* pCatSubMenu = pViewMenu->GetSubMenu(2);	//get categories submenu
					if (pCatSubMenu)
					{
						int CatID = IDItem - MenuItemStartCmdID;
						bool ItemSelected = (pCatSubMenu->GetMenuState(IDItem, MF_BYCOMMAND) == MF_CHECKED);
						DWORD Bit = ((DWORD)1 << ((int)CatID - 1));
						if (ItemSelected)
						{
							DVF.dwSelectedCategories&=(~Bit);
						} else {
							DVF.dwSelectedCategories|=Bit;
						}
					}
				}
			}
			if (IDItem == ID_BYCATEGORIES_ALL)
				DVF.dwSelectedCategories = 0;
			if (IDItem == ID_VIEW_ALL)
				DVF.Reset();
			//Get others item id
			switch (IDItem)
			{
				//progress submenu value
				case	ID_BYPROGRESS_ALL: DVF.iProgress = 0; break;
				case	ID_BYPROGRESS_NOTDONE: DVF.iProgress = 1; break;
				case	ID_BYPROGRESS_PARTIALLYDONE: DVF.iProgress = 2; break;
				case	ID_BYPROGRESS_FINISHED: DVF.iProgress = 3; break;
				//priority submenu value
				case	ID_BYPRIORITY_ALL: DVF.iPriority = 0; break;
				case	ID_BYPRIORITY_IMPORTANT: DVF.iPriority = 1; break;
				case	ID_BYPRIORITY_DEFAULT: DVF.iPriority = 2; break;
				case	ID_BYPRIORITY_NOTIMPORTANT: DVF.iPriority = 3; break;
				//visibility submenu value
				case	ID_BYVISIBILITY_ALL: DVF.iVisibility = 0; break;
				case	ID_BYVISIBILITY_THISMONTH: DVF.iVisibility = 1; break;
				case	ID_BYVISIBILITY_THISWEEK: DVF.iVisibility = 2; break;
				case	ID_BYVISIBILITY_YESTERDAY: DVF.iVisibility = 3; break;
				case	ID_BYVISIBILITY_TODAY: DVF.iVisibility = 4; break;
				//addingdate submenu value
				case	ID_BYADDINGDATE_ALL: DVF.iAdded = 0; break;
				case	ID_BYADDINGDATE_THISMONTH: DVF.iAdded = 1; break;
				case	ID_BYADDINGDATE_THISWEEK: DVF.iAdded = 2; break;
				case	ID_BYADDINGDATE_YESTERDAY: DVF.iAdded = 3; break;
				case	ID_BYADDINGDATE_TODAY: DVF.iAdded = 4; break;
				//changingdate submenu value
				case	ID_BYCHANGINGDATE_ALL: DVF.iModified = 0; break;
				case	ID_BYCHANGINGDATE_THISMONTH: DVF.iModified = 1; break;
				case	ID_BYCHANGINGDATE_THISWEEK: DVF.iModified = 2; break;
				case	ID_BYCHANGINGDATE_YESTERDAY: DVF.iModified = 3; break;
				case	ID_BYCHANGINGDATE_TODAY: DVF.iModified = 4; break;
			}
			if (!DVFcopy.Equal(DVF))
			{
				SetViewMenuState(DVF);
				Dlg.Settings.Write();
				ReloadDataInVisibleList();
			}
		}
	}

	void ReloadDataInVisibleList()
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		Dlg.dlgNotes.LoadListItems();
		Dlg.dlgTasks.LoadListItems();
	}

	bool Accept_Categories(PSXML::PSTinyXML& TB, DataViewFilter& DVF)
	{
		DWORD dwCategories = PSXML::HexToInt(TB.FieldByName("Categories").AsString());
		return bool((DVF.dwSelectedCategories & dwCategories) == DVF.dwSelectedCategories);
	}

	bool Accept_Priority(PSXML::PSTinyXML& TB, DataViewFilter& DVF)
	{
		int iPriority = TB.FieldByName("Priority").AsInt();
		switch (DVF.iPriority)
		{
			case 0: return true;	//all
			case 1: return (iPriority == 1);	//important
			case 2: return (iPriority == 0);	//default
			case 3: return (iPriority == -1);	//non-important
		}
		return true;
	}

	bool Accept_Progress(PSXML::PSTinyXML& TB, DataViewFilter& DVF)
	{
		int iProgress = TB.FieldByName("Progress").AsInt();
		switch (DVF.iProgress)
		{
			case 0: return true;	//all
			case 1: return (iProgress == 0);	//not done
			case 2: return ((iProgress > 0) && (iProgress < 100));	//partially done
			case 3: return (iProgress == 100);	//finished
		}
		return true;
	}

	bool Accept_Date(int iFilterSelect, std::string& StartDate)
	{
		if (iFilterSelect == 0)
			return true;
		static SYSTEMTIME stFrom;
		if (PSXML::DateFromStr(StartDate, stFrom))
		{
			static SYSTEMTIME stTODAY;
			GetLocalTime(&stTODAY);
			CTime ctTODAY(stTODAY, 0);
			CTime ctFrom(stFrom, 0);
			if (ctFrom <= ctTODAY)
			{
				CTimeSpan elpTime = ctTODAY - ctFrom;
				LONGLONG Days = elpTime.GetDays();
				if (iFilterSelect == 1)	//30 last days
					return (Days <= 30);
				if (iFilterSelect == 2)	//7 last days
					return (Days <= 7);
				if (iFilterSelect == 3)	//1 last days
					return (Days == 1);
				if (iFilterSelect == 4)	//today
					return (Days == 0);
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	std::string InfoListString(int Count, int Selected)
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		std::string S;
		if (Count == Selected)
		{
			S = Dlg.ML.Text(114);
			S += " (" + Text::IntToStr(Count) + ")";
		} else {
			S = Dlg.ML.Text(115);
			S += " (" + Text::IntToStr(Selected) + "/" + Text::IntToStr(Count) + ")";
		}
		return S;
	}

	std::string GetFilterDescription(DataViewFilter& DVF)
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		Language::Lang& L = Dlg.ML;

		std::string S = L.Text(20);
		if (DVF.All())
		{
			S += "\n";
			S += " " + (std::string)L.Text(95);
		} else {
			if (DVF.dwSelectedCategories)
			{
				S += "\n " + (std::string)L.Text(97) + "\n";
				std::string sCatList = Dlg.dlgCategories.CategoriesAsString(DVF.dwSelectedCategories);
				sCatList = Text::Replace(sCatList, ", ", "\n  - ");
				S += "  - " + sCatList;	
			}
			if (DVF.iProgress)
			{
				S += "\n " + (std::string)L.Text(98) + "\n";
				switch (DVF.iProgress)
				{
					case 1: S += "  - " + (std::string)Dlg.ML.Text(103); break;
					case 2: S += "  - " + (std::string)Dlg.ML.Text(104); break;
					case 3: S += "  - " + (std::string)Dlg.ML.Text(105); break;
				}
			}
			if (DVF.iPriority)
			{
				S += "\n " + (std::string)L.Text(99) + "\n";
				switch (DVF.iPriority)
				{
					case 1: S += "  - " + (std::string)Dlg.ML.Text(106); break;
					case 2: S += "  - " + (std::string)Dlg.ML.Text(107); break;
					case 3: S += "  - " + (std::string)Dlg.ML.Text(108); break;
				}
			}
			if (DVF.iVisibility)
			{
				S += "\n " + (std::string)L.Text(100) + "\n";
				switch (DVF.iVisibility)
				{
					case 1: S += "  - " + (std::string)Dlg.ML.Text(109); break;
					case 2: S += "  - " + (std::string)Dlg.ML.Text(110); break;
					case 3: S += "  - " + (std::string)Dlg.ML.Text(111); break;
					case 4: S += "  - " + (std::string)Dlg.ML.Text(112); break;
				}
			}
			if (DVF.iAdded)
			{
				S += "\n " + (std::string)L.Text(101) + "\n";
				switch (DVF.iAdded)
				{
					case 1: S += "  - " + (std::string)Dlg.ML.Text(109); break;
					case 2: S += "  - " + (std::string)Dlg.ML.Text(110); break;
					case 3: S += "  - " + (std::string)Dlg.ML.Text(111); break;
					case 4: S += "  - " + (std::string)Dlg.ML.Text(112); break;
				}
			}
			if (DVF.iModified)
			{
				S += "\n " + (std::string)L.Text(102) + "\n";
				switch (DVF.iModified)
				{
					case 1: S += "  - " + (std::string)Dlg.ML.Text(109); break;
					case 2: S += "  - " + (std::string)Dlg.ML.Text(110); break;
					case 3: S += "  - " + (std::string)Dlg.ML.Text(111); break;
					case 4: S += "  - " + (std::string)Dlg.ML.Text(112); break;
				}
			}
		}
		return S;
	}

	bool Accept_NoteDateRange(const SYSTEMTIME& SelectedDate, PSXML::PSTinyXML&	TB)
	{
		static SYSTEMTIME stFrom;
		static SYSTEMTIME stTo;
		if (PSXML::DateFromStr(TB.FieldByName("ShowFrom").AsString(), stFrom))
		{
			CTime ctSelected(SelectedDate, 0);
			CTime ctFrom(stFrom, 0);
			if (PSXML::DateFromStr(TB.FieldByName("ShowTo").AsString(), stTo))
			{
				CTime ctTo(stTo, 0);
				return ((ctSelected >= ctFrom) && (ctSelected <= ctTo));
			} else {
				return (ctSelected >= ctFrom);
			}
		} else {
			return true;
		}
	}

	bool Accept_TaskDateRange(const SYSTEMTIME& SelectedDate, PSXML::PSTinyXML&	TB)
	{
		static SYSTEMTIME stDate;
		static SYSTEMTIME stTo;
		if (PSXML::DateFromStr(TB.FieldByName("StartDate").AsString(), stDate))
		{
			Date::RemoveTime(stDate);

			if (Date::DateEqual(SelectedDate, stDate))
				return true;

			CTime ctSelected(SelectedDate, 0);
			CTime ctDate(stDate, 0);

			if (ctSelected >= ctDate)
			{
				bool bInRange = false;
				if (PSXML::DateFromStr(TB.FieldByName("EndDate").AsString(), stTo))
				{
					Date::RemoveTime(stTo);
					CTime ctTo(stTo, 0);
					bInRange = (ctSelected <= ctTo);
				} else {
					bInRange = true;
				}

				bool bDone = (TB.FieldByName("Progress").AsInt() == 100);
				DWORD dwRepeats = PSXML::HexToInt(TB.FieldByName("Repeat").AsString());

				//Is repeat on ?
				if (HIWORD(dwRepeats))
				{
					if (IsRepeatedDay(stDate, SelectedDate, dwRepeats))
					{
						if (!bDone)
							return true;
						return bInRange;
					} else {
						return false;
					}
				} else {
					if (bDone)
					{
						return false;
					} else {
						return bInRange;
					}
				}

			} else {
				return false;
			}
		} else {
			return true;
		}
	}

	bool Accept_TaskDateRangeForToday(const SYSTEMTIME& SelectedDate, PSXML::PSTinyXML&	TB)
	{
		static SYSTEMTIME stDate;
		static SYSTEMTIME stTo;
		if (PSXML::DateFromStr(TB.FieldByName("StartDate").AsString(), stDate))
		{
			Date::RemoveTime(stDate);

			if (Date::DateEqual(SelectedDate, stDate))
				return true;

			if (PSXML::DateFromStr(TB.FieldByName("EndDate").AsString(), stTo))
			{
				Date::RemoveTime(stTo);

				if (Date::DateEqual(SelectedDate, stDate))
					return true;
			}
		}
		return false;
	}

	bool IsRepeatedDay(const SYSTEMTIME& dateStart, const SYSTEMTIME& dateToday, const DWORD& dwRepeats)
	{
		WORD Type = HIWORD(dwRepeats);
		WORD Data = LOWORD(dwRepeats);

		//repeat every day
		if (Type == 1)
			return true;
		//repeat every month
		if (Type == 2)
			return (dateStart.wDay == dateToday.wDay);
		//repeat every year
		if (Type == 3)
			return ((dateStart.wMonth == dateToday.wMonth) && (dateStart.wDay == dateToday.wDay));
		//repeat in selected week days
		if (Type == 4)
		{
			int iWeekDay = dateToday.wDayOfWeek - 1;
			if (iWeekDay == -1)
				iWeekDay = 6;
			return Misc::IsBitSet(dwRepeats, iWeekDay);
		}
		return false;
	}
	
	void EnableViewMenuByTab()
	{
		lpMain->MainMenu.EnableMenuItem(2, MF_BYPOSITION | 
			((lpMain->dlgToday.IsWindowVisible())?(MF_GRAYED | MF_DISABLED):MF_ENABLED));

		DrawMenuBar(lpMain->GetSafeHwnd());

		SetMenuItemsStateByListItemsSelect();
	}

	HDWP BeginAlignControls(int Count)
	{
		return BeginDeferWindowPos(Count);
	}

	void EndAlignControls(HDWP hPos)
	{
		if (hPos)
			EndDeferWindowPos(hPos);
	}

	void NextAlignControl(HDWP& hPos, CWnd& C, CRect& R, int W, bool AlignLeft)
	{
		if (hPos)
		{
			if (AlignLeft)
			{
				R.right = R.left + W;
			} else {
				R.left = R.right - W;
			}
			hPos = DeferWindowPos(hPos, C.GetSafeHwnd(), NULL, R.left, R.top, R.Width(), R.Height(), SWP_NOZORDER);
			if (AlignLeft)
			{
				R.left = R.right + DefaultMargin;
			} else {
				R.right = R.left - DefaultMargin;
			}
		}
	}

	void DrawControlsLine(CDC& dc, CWnd& Dlg, int Ypos)
	{
		DWORD cDlgFace = GetSysColor(COLOR_3DFACE);
		COLORREF cNewColor = RGB(GetRValue(cDlgFace) - 28, GetGValue(cDlgFace) - 28, GetBValue(cDlgFace) - 28);
		CPen MyPen(PS_SOLID, 1, cNewColor);
		CPen* pOld = dc.SelectObject(&MyPen);
		CRect R;
		Dlg.GetClientRect(&R);
		dc.MoveTo(DefaultMargin, Ypos);
		dc.LineTo(R.right - DefaultMargin, Ypos);
		dc.SelectObject(pOld);
	}

	void ShowWindow_categories_forNote(dlgData_note& DN)
	{
		if (lpMain->dlgCategories.IsWindowVisible() == TRUE)
		{
			lpMain->dlgCategories.BringWindowToTop();
		} else {
			lpMain->dlgCategories.dwCategories = DN.dwCategories;
			lpMain->dlgCategories.DoModal();
			DN.dwCategories = lpMain->dlgCategories.dwCategories;

			if (lpMain->dlgEditNote.IsWindowVisible() == TRUE)
			{
				lpMain->dlgEditNote.BringWindowToTop();
				std::string sCategories = lpMain->dlgCategories.CategoriesAsString(DN.dwCategories);
				lpMain->dlgEditNote.labCategories.SetWindowText(sCategories.c_str());
				lpMain->dlgEditNote.labCategories_tip.Update(sCategories.c_str());
			}
		}
	}

	void ShowWindow_categories_forTask(dlgData_task& DT)
	{
		if (lpMain->dlgCategories.IsWindowVisible() == TRUE)
		{
			lpMain->dlgCategories.BringWindowToTop();
		} else {
			lpMain->dlgCategories.dwCategories = DT.dwCategories; 
			lpMain->dlgCategories.DoModal();
			DT.dwCategories = lpMain->dlgCategories.dwCategories;

			if (lpMain->dlgEditTask.IsWindowVisible() == TRUE)
			{
				lpMain->dlgEditTask.BringWindowToTop();
				std::string sCategories = lpMain->dlgCategories.CategoriesAsString(DT.dwCategories);
				lpMain->dlgEditTask.labCategories.SetWindowText(sCategories.c_str());
				lpMain->dlgEditTask.labCategories_tip.Update(sCategories.c_str());
			}
		}
	}

	void ShowWindow_repeats_forTask(dlgData_task& DT)
	{
		INT_PTR nResponse = IDCANCEL;
		if (lpMain->dlgRepeats.IsWindowVisible() == TRUE)
		{
			lpMain->dlgRepeats.BringWindowToTop();
		} else {
			lpMain->dlgRepeats.dwRepeats = DT.dwRepeats; 
			nResponse = lpMain->dlgRepeats.DoModal();
			DT.dwRepeats = lpMain->dlgRepeats.dwRepeats;

			if (lpMain->dlgEditTask.IsWindowVisible() == TRUE)
			{
				lpMain->dlgEditTask.BringWindowToTop();
				std::string sRepeats = lpMain->dlgRepeats.RepeatsAsString(DT.dwRepeats);
				lpMain->dlgEditTask.labRepeats.SetWindowText(sRepeats.c_str());
				lpMain->dlgEditTask.labRepeats_tip.Update(sRepeats.c_str());

				if ((nResponse == IDOK) || (DT.dwRepeats == 0))
				{
					lpMain->dlgEditTask.chkStop.SetCheck(BST_UNCHECKED);
					lpMain->dlgEditTask.edDateStop.EnableWindow(lpMain->dlgEditTask.chkStop.GetCheck() == BST_CHECKED);
				}
			}
		}
	}

	bool IsAnyWindowVisible()
	{
		CWinWorkBarDlg& M = *lpMain;
		return (
			(M.dlgEditNote.GetSafeHwnd() && M.dlgEditNote.IsWindowVisible()) ||
			(M.dlgEditTask.GetSafeHwnd() && M.dlgEditTask.IsWindowVisible()) ||
			(M.dlgCategories.GetSafeHwnd() && M.dlgCategories.IsWindowVisible()) ||
			(M.dlgEditCategories.GetSafeHwnd() && M.dlgEditCategories.IsWindowVisible()) ||
			(M.dlgSearch.GetSafeHwnd() && M.dlgSearch.IsWindowVisible()) ||
			(M.dlgSettings.GetSafeHwnd() && M.dlgSettings.IsWindowVisible())
			);
	}

	bool ExportData_notes(ExportToHtml& EX)
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		EX.Set(lpMain->GetAppTitle() + " : " + Dlg.ML.Text(145), Dlg.GetAppTitle(), Text::CurrentLocalDateTime());
		ExportToHtml::dataNote DN;
		SYSTEMTIME ST;
		PSXML::PSTinyXML&	TB = lpMain->tbNotes;
		for (int I = 0; I < Dlg.dlgNotes.listNotes.GetItemCount(); I++)
		{
			if (Dlg.dlgNotes.listNotes.GetItemState(I, LVIS_SELECTED) == LVIS_SELECTED)
			{
				std::string UID = Dlg.dlgNotes.listNotes.GetItemText(I, 6);
				if (TB.Locate(UID) && TB.GetData())
				{
					DN.NoteName = Dlg.ML.Text(147);
					DN.NoteText = Text::Replace(TB.FieldByName("Text").AsString(), "\r\n", "<br>");
					DWORD dwCategories = PSXML::HexToInt(TB.FieldByName("Categories").AsString());
					std::string Categories = Dlg.dlgCategories.CategoriesAsString(dwCategories);
					DN.NoteCategories = std::string(Dlg.ML.Text(25)) + ": " + Categories;

					DN.NoteDate = "";
					if (PSXML::DateFromStr(TB.FieldByName("Date").AsString(), ST))
						DN.NoteDate = std::string(Dlg.ML.Text(77)) + ": " + Text::DateTimeToStringLocale(ST, NULL, false);
					if (PSXML::DateFromStr(TB.FieldByName("Modified").AsString(), ST))
						DN.NoteDate += ". " + std::string(Dlg.ML.Text(78)) + ": " + Text::DateTimeToStringLocale(ST, NULL, false);

					DN.NoteDateRange = "";
					if (PSXML::DateFromStr(TB.FieldByName("ShowFrom").AsString(), ST))
						DN.NoteDateRange = std::string(Dlg.ML.Text(92)) + ": " + Text::DateToStringLocale(ST, NULL, false);
					if (PSXML::DateFromStr(TB.FieldByName("ShowTo").AsString(), ST))
						DN.NoteDateRange += ", " + std::string(Dlg.ML.Text(93)) + ": " + Text::DateToStringLocale(ST, NULL, false);

					EX.AddNote(DN);
				}
			}
		}
		return true;
	}

	bool ExportData_tasks(ExportToHtml& EX)
	{
		CWinWorkBarDlg& Dlg = *lpMain;
		EX.Set(lpMain->GetAppTitle() + " : " + Dlg.ML.Text(146), lpMain->GetAppTitle(), Text::CurrentLocalDateTime());
		ExportToHtml::dataTask DT;
		SYSTEMTIME ST;
		PSXML::PSTinyXML&	TB = lpMain->tbTasks;
		for (int I = 0; I < Dlg.dlgTasks.listTasks.GetItemCount(); I++)
		{
			if (Dlg.dlgTasks.listTasks.GetItemState(I, LVIS_SELECTED) == LVIS_SELECTED)
			{
				std::string UID = Dlg.dlgTasks.listTasks.GetItemText(I, 9);
				if (TB.Locate(UID) && TB.GetData())
				{
					DT.TaskName = Dlg.ML.Text(148);
					DT.TaskText = Text::Replace(TB.FieldByName("Text").AsString(), "\r\n", "<br>");

					DWORD dwCategories = PSXML::HexToInt(TB.FieldByName("Categories").AsString());
					std::string Categories = Dlg.dlgCategories.CategoriesAsString(dwCategories);
					DT.TaskCategories = std::string(Dlg.ML.Text(25)) + ": " + Categories;

					DT.TaskDate = "";
					if (PSXML::DateFromStr(TB.FieldByName("Date").AsString(), ST))
						DT.TaskDate = std::string(Dlg.ML.Text(77)) + ": " + Text::DateTimeToStringLocale(ST, NULL, false);
					if (PSXML::DateFromStr(TB.FieldByName("Modified").AsString(), ST))
						DT.TaskDate += ". " + std::string(Dlg.ML.Text(78)) + ": " + Text::DateTimeToStringLocale(ST, NULL, false);

					DT.TaskDateRange = "";
					if (PSXML::DateFromStr(TB.FieldByName("StartDate").AsString(), ST))
						DT.TaskDateRange = std::string(Dlg.ML.Text(117)) + ": " + Text::DateTimeToStringLocale(ST, NULL, false);
					if (PSXML::DateFromStr(TB.FieldByName("EndDate").AsString(), ST))
						DT.TaskDateRange += ", " + std::string(Dlg.ML.Text(93)) + ": " + Text::DateToStringLocale(ST, NULL, false);

					DWORD dwRepeats = PSXML::HexToInt(TB.FieldByName("Repeat").AsString());
					DT.TaskRepeat = std::string(Dlg.ML.Text(120)) + ": " + Dlg.dlgRepeats.RepeatsAsString(dwRepeats);

					//Priority
					DT.TaskPriority = std::string(Dlg.ML.Text(118)) + ": ";
					switch (TB.FieldByName("Priority").AsInt())
					{
						case -1: DT.TaskPriority += Dlg.ML.Text(108); break;
						case 0: DT.TaskPriority += Dlg.ML.Text(107); break;
						case 1: DT.TaskPriority += Dlg.ML.Text(106); break;
					}

					//Progress
					int iProgress = TB.FieldByName("Progress").AsInt();
					DT.TaskProgress = std::string(Dlg.ML.Text(119)) + ": " + Text::IntToStr(iProgress) + " %";
					if (iProgress == 0)
						DT.TaskProgress = std::string(Dlg.ML.Text(119)) + ": " + lpMain->ML.Text(103);
					if (iProgress == 100)
						DT.TaskProgress = std::string(Dlg.ML.Text(119)) + ": " + lpMain->ML.Text(105);

					EX.AddTask(DT);
				}
			}
		}
		return true;
	}

	bool ExportData(std::string sFileName)
	{
		ExportToHtml EX;
		if (EX.Load("Default.htm"))
		{
			if (lpMain->dlgNotes.IsWindowVisible())
				ExportData_notes(EX);
			if (lpMain->dlgTasks.IsWindowVisible())
				ExportData_tasks(EX);

			EX.Save(sFileName);
		} else {
			lpMain->MessageBox(lpMain->ML.Text(184), NULL, MB_OK | MB_ICONERROR);
		}
		return true;
	}

	std::string SoundFolder()
	{
		return File::IncludeBackSlash(Misc::ApplicationFolder()) + "Sounds\\";
	}

	void PlayInfoSound(std::string Name, bool PCSpeakerAlso)
	{
		std::string Ext = File::FileExtension(Name);
		if (!Ext.empty())
		{
			if (Ext == "system")
			{
				if (Name == "Asterisk.system")
					MessageBeep(MB_ICONASTERISK);
				if (Name == "Exclamation.system")
					MessageBeep(MB_ICONEXCLAMATION);
				if (Name == "Hand.system")
					MessageBeep(MB_ICONHAND);
				if (Name == "Default.system")
					MessageBeep(MB_OK);
			}
			if (Ext == "wav")
			{
				std::string FileName = Utils::SoundFolder() + Name;
				if (File::FileExists(FileName))
				{
					PlaySound(FileName.c_str(), NULL, SND_ASYNC | SND_FILENAME);
				}
			}
		}
		if (PCSpeakerAlso)
		{
			Beep(3000, 50);
			Misc::Delay(50);
			Beep(4000, 50);
			Misc::Delay(50);
			Beep(5000, 50);
		}
	}

	std::string EvalExpression(HWND hWnd, std::string Exp, bool& Error)
	{
		Error = false;
		FunctionParser FP;
		std::string Vars;
		int iError = FP.Parse(Exp, Vars, true);
		if (iError == -1)
		{
			double Value = FP.Eval(NULL);
			char	Buffer[64];
			int     digits = 10;
			_gcvt(Value, digits, Buffer);
			//remove last point
			std::string S = Buffer;
			if (S.size() > 0)
			{
				if (S[S.size() - 1] == '.')
					S = S.substr(0, S.size() - 1);
			}
			return S;
		} else {
			Error = true;
			if (FP.GetParseErrorType() != FunctionParser::FP_NO_ERROR)
			{
				int iMsg = 163;	//default error
				switch (FP.GetParseErrorType())
				{
					case	FunctionParser::SYNTAX_ERROR:					163;	break;
					case	FunctionParser::MISM_PARENTH:					164;	break;
					case	FunctionParser::MISSING_PARENTH:			165;	break;
					case	FunctionParser::EMPTY_PARENTH:				166;	break;
					case	FunctionParser::EXPECT_OPERATOR:			167;	break;
					case	FunctionParser::OUT_OF_MEMORY:				168;	break;
					case	FunctionParser::UNEXPECTED_ERROR:			169;	break;
					case	FunctionParser::INVALID_VARS:					170;	break;
					case	FunctionParser::ILL_PARAMS_AMOUNT:		171;	break;
					case	FunctionParser::PREMATURE_EOS:				172;	break;
					case	FunctionParser::EXPECT_PARENTH_FUNC:	173;	break;
				}

				std::string S = lpMain->ML.Text(iMsg);

				S += "\n";
				S += lpMain->ML.Text(174);
				S += ": " + Text::IntToStr(iError);

				MessageBox(hWnd, S.c_str(), lpMain->ML.Text(175), MB_ICONERROR | MB_OK);
			}
		}
		return "";
	}

	void DoEvaluateExpression(CRichEditCtrl& red, CDialog& Dlg)
	{
		int iLineIdx = red.LineFromChar(-1);
		int iLineLen = red.LineLength(-1);

		int iBufferSize = iLineLen + 1;
		TCHAR* lText = new TCHAR[iBufferSize];
		if (lText)
		{
			ZeroMemory(lText, iBufferSize);
			red.GetLine(iLineIdx, lText, iBufferSize);

			lText[iBufferSize - 1] = 0;
			CString TS = lText;
			TS.Replace("\n","");
			TS.TrimRight();

			int iPos = TS.Find("=");
			if (iPos != -1)
			{
				TS.Delete(iPos, TS.GetLength() - iPos + 1);
				TS.TrimRight();
			}

			bool bError = false;
			std::string sResult = Utils::EvalExpression(Dlg.GetSafeHwnd(), (LPCTSTR)TS, bError);
			if (!bError)
			{
				std::string sLine = TS;
				sLine += " = " + sResult;

				int iBegin = red.LineIndex(iLineIdx);
				if (iBegin != -1)
				{
					int iEnd = iBegin + iLineLen;
					red.SetSel(iBegin, iEnd);
					red.ReplaceSel(sLine.c_str());
				}
			}
		}
	}

	void DoEvaluateExpression_msg(MSGFILTER& msgf, CRichEditCtrl& red, CDialog& Dlg)
	{
		bool bCtrlDown = ((GetKeyState(VK_CONTROL) & 0x80) == 0x80);
		if ((bCtrlDown) && (msgf.wParam == VK_RETURN))
		{
			if (msgf.msg == WM_KEYDOWN)
			{
				int iLineIdx = red.LineFromChar(-1);
				int iLineLen = red.LineLength(-1);

				int iBufferSize = iLineLen + 1;
				TCHAR* lText = new TCHAR[iBufferSize];
				if (lText)
				{
					ZeroMemory(lText, iBufferSize);
					red.GetLine(iLineIdx, lText, iBufferSize);

					lText[iBufferSize - 1] = 0;
					CString TS = lText;
					TS.Replace("\n","");
					TS.TrimRight();

					int iPos = TS.Find("=");
					if (iPos != -1)
					{
						TS.Delete(iPos, TS.GetLength() - iPos + 1);
						TS.TrimRight();
					}
		
					bool bError = false;
					std::string sResult = Utils::EvalExpression(Dlg.GetSafeHwnd(), (LPCTSTR)TS, bError);
					if (!bError)
					{
						std::string sLine = TS;
						sLine += " = " + sResult;

						int iBegin = red.LineIndex(iLineIdx);
						if (iBegin != -1)
						{
							int iEnd = iBegin + iLineLen;
							red.SetSel(iBegin, iEnd);
							red.ReplaceSel(sLine.c_str());
						}
					}
				}
			}
		}
	}

	bool ImportData(PSXML::PSTinyXML& DestTB, std::string sExternalFileName, CProgressCtrl* cPrg)
	{
		CWaitCursor WC;
		std::string sFolder = File::IncludeBackSlash(Misc::ApplicationFolder());
		sFolder += "Import\\";
		sFolder += Language::GetLanguageShortName(lpMain->ML.CurrentLangID());
		sFolder += "_";

		if (cPrg)
			cPrg->SetPos(0);

		PSXML::PSTinyXML ImportTB;
		ImportTB.SetDirectFileName(sExternalFileName);
		ImportTB.SetProgramData(ProgramName, Misc::GetAppVersion(false), sFolder);
		if (ImportTB.LoadTable(DestTB))
		{
			int i = 1;
			if (cPrg)
				cPrg->SetRange(0, ImportTB.RowCount());

			ImportTB.First();
			while (!ImportTB.IsEof())
			{
				if (ImportTB.GetData())
				{
					if (&DestTB == &lpMain->tbCategories)
					{
						if (DestTB.Locate(ImportTB.GetID()))
						{
							DestTB.FieldByName("Name").Set(ImportTB.FieldByName("Name").AsString());
							DestTB.UpdateData();
						}
					} else {
						if (!DestTB.RowExists(ImportTB.GetID()))
						{
							DestTB.ClearFields();
							DestTB.SetUID(ImportTB.GetID());
							DestTB.CopyRowValues(ImportTB);
							DestTB.AddRow();
						}
					}
					if (cPrg)
						cPrg->SetPos(i++);
				}
				ImportTB.Next();
			}
			return true;
		}
		return false;
	}

}
