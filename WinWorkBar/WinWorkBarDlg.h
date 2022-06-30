// WinWorkBarDlg.h : header file
//

#pragma once

#include <string>
#include "default.h"

#include "LibsOther\language.h"
#include "LibsOther\utils.h"
#include "LibsOther\BtnST.h"
#include "LibsOther\XPStyleButtonST.h"
#include "LibsOther\LayeredWindowHelperST.h"
#include "LibsOther\MenuXP\MenuXP.h"

#include "PSTabCtrl.h"

#include "CDlgToday.h"
#include "CDlgTasks.h"
#include "CDlgNotes.h"

#include "DlgEdit_Note.h"
#include "DlgEdit_Task.h"
#include "DlgSearch.h"

#include "EditCategories.h"
#include "Categories.h"
#include "Repeats.h"
#include "AboutDlg.h"
#include "ToolInfoWindowDlg.h"
#include "SettingsDlg.h"
#include "DlgImportExport.h"

#include "afxcmn.h"
#include "afxwin.h"

#include "Libs\PSUtil.h"
#include "Libs\PSMFCUtils.h"
#include "Libs\PSLanguage.h"
#include "Libs\PSRegistry.h"
#include "Libs\PSLabel.h"
#include "Libs\PSCalendar.h"
#include "Libs\PSTaskView.h"
#include "Libs\PSAuthor.h"
#include "Libs\PSToolTip.h"
#include "Libs\PSTrayIconEx.h"
#include "Libs\PSTinyXML.h"
#include "PSCuteClock.h"


//Misc data
//------------------------------------------------------

const		UINT WM_TIMER_HIDE					= 1;
const		UINT WM_TIMER_HIDEDELAY		= 2;
const		UINT WM_TIMER_GETTIME			= 3;
const		UINT WM_TIMER_REMIND_CHECK	= 4;

const		int TimerCheckDelay = 40;		//ms
const		int StartupTimerHideDelay = 5000;	//ms

const	std::string ProgramName = "WinWorkBar";

//Hot keys
enum	HotKeyType
{
	hktShowWorkBar	= 0,
	hktNewTask			= 1,
	hktNewNote			= 2,
};

struct	HotKeyItem
{
	bool	on;
	int		ID;
	UINT	Modifier;
	UINT	VKey;
	char*	S;
};

const	HotKeyItem DefaultHotKeys[] =
{
	true,	hktShowWorkBar,	HOTKEYF_CONTROL | HOTKEYF_SHIFT,	VK_F10,	"+ 7900 0300",
	true,	hktNewTask,			HOTKEYF_CONTROL | HOTKEYF_SHIFT,	VK_F6,	"+ 7500 0300",
	true,	hktNewNote,			HOTKEYF_CONTROL | HOTKEYF_SHIFT,	VK_F7,	"+ 7600 0300",
};


//Events
void FnOnCalCellClick(PSCalendarCell& CalCell);
void FnOnCalCellRightClick(PSCalendarCell& CalCell);
void FnOnNavButtonClick();


//App settings
struct AppSettings
{
	bool				bFirstRun;
	//int				OpaquePercent;
	std::string	Language;
	std::string DataFolder;
	bool				PinWindow;
	int					PanelWidth;
	bool				Time24Hour;
	bool				WindowTopmost;
	int					MinutesBeforeRemind;
	int					iClockStyle;
	bool				bShowBarOnClick;
	int					iPutAwayMinutes;

	std::string	SoundFileName;
	bool				PlayPCSpeaker;
	int					HideWindowAfterSec;
	bool				AutoStart;

	std::string	HotKey_ShowWorkBar;
	std::string	HotKey_NewTask;
	std::string	HotKey_NewNote;

	DataViewFilter		ViewFilter;

	Registry::Reg* Reg;
	void	Read()
	{
		bFirstRun = Reg->ReadAsBool("", "FirstRun", true);
		//OpaquePercent = Reg->ReadAsInt("", "OpaquePercent", 100);
		PinWindow = Reg->ReadAsBool("", "PinWindow", false);
		PanelWidth = Reg->ReadAsInt("", "PanelWidth", DefaultWidth);
		Language = Reg->ReadAsString("", "Language", "");
		Time24Hour = (Language::GetLocaleInfoString(LOCALE_USER_DEFAULT, LOCALE_ITIME) == "1");
		WindowTopmost = Reg->ReadAsBool("", "WindowTopmost", true);

		HotKey_ShowWorkBar =	Reg->ReadAsString("", "HotKey_ShowWorkBar", DefaultHotKeys[hktShowWorkBar].S);
		HotKey_NewTask =	Reg->ReadAsString("", "HotKey_NewTask", DefaultHotKeys[hktNewTask].S);
		HotKey_NewNote =	Reg->ReadAsString("", "HotKey_NewNote", DefaultHotKeys[hktNewNote].S);

		ViewFilter.Set(Reg->ReadAsString("", "DataViewFilter", ""));
		MinutesBeforeRemind = Reg->ReadAsInt("", "MinutesBeforeRemind", 15);
		DataFolder = Reg->ReadAsString("", "DataFolder", Misc::GetMyDataFolder(ProgramName));

		SoundFileName = Reg->ReadAsString("", "SoundFileName", "TooTooTi.wav");
    PlayPCSpeaker = Reg->ReadAsBool("", "PlayPCSpeaker", false);
		HideWindowAfterSec = Reg->ReadAsInt("", "HideWindowAfterSec", 2);
		iClockStyle = Reg->ReadAsInt("", "ClockStyle", 0);
		bShowBarOnClick = Reg->ReadAsBool("", "ShowBarOnClick", false);
		iPutAwayMinutes = Reg->ReadAsInt("", "PutAwayMinutes", 5);

		//AutoStart
		Registry::Reg R(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\");
		AutoStart = (!R.ReadAsString("Run", ProgramName, "").empty());
		R.SetKey();
	}
	void	Write()
	{
		Reg->Write("", "FirstRun", bFirstRun);
		//Reg->Write("", "OpaquePercent", OpaquePercent);
		Reg->Write("", "PinWindow", PinWindow);
		Reg->Write("", "PanelWidth", PanelWidth);
		Reg->Write("", "Language", Language);
		Reg->Write("", "WindowTopmost", WindowTopmost);

		Reg->Write("", "HotKey_ShowWorkBar", HotKey_ShowWorkBar);
		Reg->Write("", "HotKey_NewTask", HotKey_NewTask);
		Reg->Write("", "HotKey_NewNote", HotKey_NewNote);

		Reg->Write("", "DataViewFilter", ViewFilter.Get());
		Reg->Write("", "MinutesBeforeRemind", MinutesBeforeRemind);
		Reg->Write("", "DataFolder", DataFolder);

		Reg->Write("", "SoundFileName", SoundFileName);
		Reg->Write("", "PlayPCSpeaker", PlayPCSpeaker);
		Reg->Write("", "HideWindowAfterSec", HideWindowAfterSec);
		Reg->Write("", "ClockStyle", iClockStyle);
		Reg->Write("", "ShowBarOnClick", bShowBarOnClick);
		Reg->Write("", "PutAwayMinutes", iPutAwayMinutes);

		//AutoStart
		Registry::Reg R(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\");
		if (AutoStart)
		{
			std::string S = "\"" + Misc::ApplicationFileName() + "\"";
			R.Write("Run", ProgramName, S);
		} else {
			R.Write("Run", ProgramName, (std::string)"");
		}
	}
};


// CWinWorkBarDlg dialog
//------------------------------------------------------------------------------
class CWinWorkBarDlg : public CDialog
{
// Construction
	UINT_PTR	m_nTimer_Hide;
	UINT_PTR	m_nTimer_HideDelay;
	UINT_PTR	m_nTimer_GetTime;
	UINT_PTR	m_nTimer_RemindCheck;

	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
public:
	CWinWorkBarDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_WINWORKBAR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	bool		KeyboradStateOk;
	CPoint	m_CurrentMousePoint;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_MENUXP()
public:
	HICON				m_hIcon;
	CImageList	imgListMainMenu;
	CBitmap			bmpMainMenuXP;
	CMenu				MainMenu;

	Language::Lang		ML;						//multilanguage support
	Registry::Reg			Prefs;				//registry preferences support for CURRENT USER
	Registry::Reg			PrefsMachine;	//registry preferences support for LOCAL MACHINE
	TrayIconEx::TI		TrayIcon;			//tray icon support

	//Command line params
	std::vector<std::string>	m_CmdParams;

	AppSettings				Settings;
	int								LeftMargin;
	int								HideDelay;

	CPSTabCtrl							m_Tab;
	CThemeHelperST					m_ThemeHelper;
	CLayeredWindowHelperST	LayeredDlg;

	bool										Initialized;	//set to true when OnInit done

	CStatic			grpDateAndTime;
	CStatic			grpCalendar;

	PSCuteClock	labClock;

	PSCalendar	labCalendar;
	PSCalendar	labCalendarL1;
	PSCalendar	labCalendarR1;

	Misc::PSExtractSysIcon	SysIcons;

	//Dialogs
	CDlgToday				dlgToday;
	CDlgTasks				dlgTasks;
	CDlgNotes				dlgNotes;

	CDlgEdit_Note		dlgEditNote;
	CDlgEdit_Task		dlgEditTask;
	CDlgSearch			dlgSearch;

	CEditCategories	dlgEditCategories;
	CCategories			dlgCategories;
	CRepeats				dlgRepeats;
	SettingsDlg			dlgSettings;

	PSXML::PSTinyXML	tbCategories;
	PSXML::PSTinyXML	tbNotes;
	PSXML::PSTinyXML	tbTasks;

	CToolInfoWindowDlg	dlgToolInfo;

	void	SwitchShowHideWorkBar();
	void	SetWindowTopmost(bool On);
	bool	CursorInWindow();
	void	StartHideTimer();
	void	ShowBarNow();
	std::string	GetAppTitle();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnMmExit();
	afx_msg void OnMmHide();
	afx_msg void OnMmDoNotHide();
	afx_msg void OnMmNewtask();
	afx_msg void OnMmNewnote();
	afx_msg void OnMmFind();
	afx_msg void OnMmDelete();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnMmCategories();
	afx_msg void OnMmAbout();
	afx_msg void OnMmHelp();
	afx_msg void OnMmHomepage();
	afx_msg void OnMmRegister();
	afx_msg void OnTraymenuNewtask();
	afx_msg void OnTraymenuNewnote();
	afx_msg void OnTraymenuShowhide();
	afx_msg void OnTraymenuSettings();
	afx_msg void OnTraymenuAbout();
	afx_msg void OnTraymenuClose();
	afx_msg void OnMmSettings();
	afx_msg void OnMmEdit();
	afx_msg void OnMmOntop();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMmViewinbrowser();
	afx_msg void OnMmSaveashtml();
	afx_msg void OnMmImport();
	afx_msg void OnMmExport();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
