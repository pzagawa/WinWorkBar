
#pragma once

#include "..\default.h"
#include "..\WinWorkBar.h"
#include "..\WinWorkBarDlg.h"
#include "..\ExportToHtml.h"

#include "..\LibsOther\BtnST.h"
#include "..\LibsOther\ThemeHelperST.h"
#include "..\LibsOther\XPStyleButtonST.h"

#include "..\Libs\PSTinyXML.h"
#include "..\Libs\PSToolTip.h"

#include "..\LibsOther\FParser\fparser.h"

namespace Utils
{
	//Tables definitions
	const	PSXML::FieldDef FieldList_categories[] = 
	{
		"Name",
		"Date",
		NULL,
	};

	//Tables definitions
	const	PSXML::FieldDef FieldList_notes[] = 
	{
		"Text",
		"Date",
		"ShowFrom",
		"ShowTo",
		"Modified",
		"Categories",
		NULL,
	};

	const	PSXML::FieldDef FieldList_tasks[] = 
	{
		"Text",
		"StartDate",
		"EndDate",
		"Categories",
		"Priority",
		"Progress",
		"Repeat",
		"Date",
		"Modified",
		NULL,
	};


	//Functions
	void	Delay(int MSec);
	void	ProcessMessages();

	void	SetWorkBarPosition();
	void	SetWorkBarWidth(int Width);
	int		GetWorkBarWidth();
	void	ShowWorkBar();
	void	HideWorkBar(bool ForceHide);

	RECT&	WinTaskBarRect();
	UINT	WinTaskBarEdge(CRect rc);
	UINT	GetWinTaskBarEdge();

	void	ProcessMessages();
	bool	WorkBarVisible();

	void	SetDefaultHotKeys();
	bool	RegisterHotKeys();
	void	UnRegisterHotKeys();
	void	InitHotKeys();
	void	GetConfiguredHotKeys();
	std::string	HotKeyNormalName(int HKT);

	void	Init();
	void	SetupControls();
	void	UpdateMenuItemsState();
	void	SetMenuItemsStateByListItemsSelect();
	void	SetControlsStrings();
	void	SetViewMenuStrings();

	void	SetWindowTransparency();

	void	ResizeControls();
	void	ResizeControlsIfNeeded(int W, int H);

	void	UpdateDateAndTimeLabels();

	void	SetOtherCalendars();

	void	DaySelected(const SYSTEMTIME& ST);

	void InitCommandLine();
	bool RunByCommandLine(bool RunInNewProcess);
	bool CreateLangFiles(std::string Param);

	void OnTrayMenuShow(HMENU TM);
	void OnTrayEvent(LPARAM lParam);

	HDWP ResizeTabButton(HDWP PosHwnd, CXPStyleButtonST& Btn, CRect& posR, int& btnLeft);
	void SetupXPButton(CXPStyleButtonST& Btn, int IconResID, int LangID);

	void SetupRichEditForNote(CRichEditCtrl& Ed);
	void RunRichEditLink(CRichEditCtrl& Ed, ENLINK *pEnLink);

	void	LoadAndResetTable_Categories(bool CleanTable);
	void	LoadAndResetTable_Notes(bool CleanTable);
	void	LoadAndResetTable_Tasks(bool CleanTable);

	void FormStringToListDisplay(std::string& S, int MaxChars = 200);

	void ShowRichEditContextMenu(CDialog& Dlg, MSGFILTER *pMsgFilter, CRichEditCtrl& RED, int EditResource);

	void SetupViewMenu();
	void SetViewMenuState(DataViewFilter& DVF);
	void GetViewMenuState_OnCommand(WPARAM wParam, LPARAM lParam);

	void ReloadDataInVisibleList();

	bool Accept_Categories(PSXML::PSTinyXML& TB, DataViewFilter& DVF);
	bool Accept_Date(int iFilterSelect, std::string& StartDate);
	bool Accept_Priority(PSXML::PSTinyXML& TB, DataViewFilter& DVF);
	bool Accept_Progress(PSXML::PSTinyXML& TB, DataViewFilter& DVF);

	std::string InfoListString(int Count, int Selected);
	std::string GetFilterDescription(DataViewFilter& DVF);

	bool Accept_NoteDateRange(const SYSTEMTIME& SelectedDate, PSXML::PSTinyXML&	TB);
	bool Accept_TaskDateRange(const SYSTEMTIME& SelectedDate, PSXML::PSTinyXML&	TB);
	bool Accept_TaskDateRangeForToday(const SYSTEMTIME& SelectedDate, PSXML::PSTinyXML&	TB);

	bool IsRepeatedDay(const SYSTEMTIME& dateStart, const SYSTEMTIME& dateToday, const DWORD& dwRepeats);

	void EnableViewMenuByTab();

	HDWP BeginAlignControls(int Count);
	void EndAlignControls(HDWP hPos);
	void NextAlignControl(HDWP& hPos, CWnd& C, CRect& R, int W, bool AlignLeft = true);
	void DrawControlsLine(CDC& dc, CWnd& Dlg, int Ypos);

	void ShowWindow_categories_forNote(dlgData_note& DN);
	void ShowWindow_categories_forTask(dlgData_task& DT);
	void ShowWindow_repeats_forTask(dlgData_task& DT);

	bool IsAnyWindowVisible();

	bool ExportData(std::string sFileName);

	std::string SoundFolder();
	void PlayInfoSound(std::string Name, bool PCSpeakerAlso);

	std::string EvalExpression(HWND hWnd, std::string Exp, bool& Error);
	void DoEvaluateExpression(CRichEditCtrl& red, CDialog& Dlg);
	void DoEvaluateExpression_msg(MSGFILTER& msgf, CRichEditCtrl& red, CDialog& Dlg);

	bool ImportData(PSXML::PSTinyXML& DestTB, std::string sExternalFileName = "", CProgressCtrl* cPrg = NULL);

}
