// SettingsDlg.cpp : implementation file
//

#include "Default.h"
#include "WinWorkBarDlg.h"
#include "SettingsDlg.h"

extern CWinWorkBarDlg*	lpMain;

// SettingsDlg dialog

IMPLEMENT_DYNAMIC(SettingsDlg, CDialog)
SettingsDlg::SettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SettingsDlg::IDD, pParent)
{
}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRP1, grp1);
	DDX_Control(pDX, IDC_GRP2, grp2);
	DDX_Control(pDX, IDC_GRP3, grp3);
	DDX_Control(pDX, IDC_GRP4, grp4);
	DDX_Control(pDX, IDC_GRP5, grp5);
	DDX_Control(pDX, IDC_LIST_LANG, listLanguage);
	DDX_Control(pDX, IDC_ED_HIDEWINDOWAFTER, edHideWindowSeconds);
	DDX_Control(pDX, IDC_SPIN_HIDEWINDOWAFTER, spinHideWindowSeconds);
	DDX_Control(pDX, IDC_LAB_HIDEWINDOWAFTER, labHideWindowSeconds);
	DDX_Control(pDX, IDC_SETTING_HOTKEY_CHK1, chkHotKey1);
	DDX_Control(pDX, IDC_SETTING_HOTKEY_CHK2, chkHotKey2);
	DDX_Control(pDX, IDC_SETTING_HOTKEY_CHK3, chkHotKey3);
	DDX_Control(pDX, IDC_PROFILE_HOTKEY1, edHotKey1);
	DDX_Control(pDX, IDC_PROFILE_HOTKEY2, edHotKey2);
	DDX_Control(pDX, IDC_PROFILE_HOTKEY3, edHotKey3);
	DDX_Control(pDX, IDC_PROFILE_MISC_LAB1, labHotKey1);
	DDX_Control(pDX, IDC_PROFILE_MISC_LAB2, labHotKey2);
	DDX_Control(pDX, IDC_PROFILE_MISC_LAB3, labHotKey3);
	DDX_Control(pDX, IDC_LAB_REMIND_SHOW, labRemindShow);
	DDX_Control(pDX, IDC_ED_REMIND_SHOW_MINUTESBEFORE, edRemindShow);
	DDX_Control(pDX, IDC_SPIN_MINUTESBEFORE, spinRemindShow);
	DDX_Control(pDX, IDC_LAB_REMINDSHOW, labRemindShowValue);
	DDX_Control(pDX, IDC_LAB_REMIND_SOUND, labRemindSound);
	DDX_Control(pDX, IDC_LIST_SOUNDS, listRemindSounds);
	DDX_Control(pDX, IDC_BTN_TESTSOUND, btnTestRemindSound);
	DDX_Control(pDX, IDC_CHK_PCSPEAKER, chkPlayPcSpeakerAlso);
	DDX_Control(pDX, IDC_CHK_AUTORUN, chkAutoRun);
	DDX_Control(pDX, IDOK, btnOk);
	DDX_Control(pDX, IDCANCEL, btnCancel);
	DDX_Control(pDX, IDC_GRP6, grpClockStyle);
	DDX_Control(pDX, IDC_LST_CLOCKSTYLE, listClockStyle);
	DDX_Control(pDX, IDC_CHK_SHOWBARONCLICK, chkShowBarOnClick);
	DDX_Control(pDX, IDC_GRP_TASKLATER, grpPutAwayTask);
	DDX_Control(pDX, IDC_ED_TASKPUTAWAY, edPutAwayTask);
	DDX_Control(pDX, IDC_LAB_TASKPUTAWAY, labPutAwayTask);
	DDX_Control(pDX, IDC_SPIN_PUTTASKAWAY, spinPutAwayTask);
}

BEGIN_MESSAGE_MAP(SettingsDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_LIST_LANG, OnCbnSelchangeListLang)
	ON_BN_CLICKED(IDC_SETTING_HOTKEY_CHK1, OnBnClickedSettingHotkeyChk1)
	ON_BN_CLICKED(IDC_SETTING_HOTKEY_CHK2, OnBnClickedSettingHotkeyChk2)
	ON_BN_CLICKED(IDC_SETTING_HOTKEY_CHK3, OnBnClickedSettingHotkeyChk3)
	ON_BN_CLICKED(IDC_BTN_TESTSOUND, OnBnClickedBtnTestsound)
END_MESSAGE_MAP()


// SettingsDlg message handlers
BOOL SettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	lpLang = &lpMain->ML;
	lpPrefs = &lpMain->Prefs;

	SetWindowText(lpLang->Text(7));

	grp1.SetWindowText(lpLang->Text(150));
	grp2.SetWindowText(lpLang->Text(151));
	grp3.SetWindowText(lpLang->Text(153));
	grp4.SetWindowText(lpLang->Text(154));
	grp5.SetWindowText(lpLang->Text(155));
	grpClockStyle.SetWindowText(lpLang->Text(186));
	labHideWindowSeconds.SetWindowText(lpLang->Text(152));
	labHotKey1.SetWindowText(lpLang->Text(69));
	labHotKey2.SetWindowText(lpLang->Text(14));
	labHotKey3.SetWindowText(lpLang->Text(13));
	labRemindShow.SetWindowText(lpLang->Text(157));
	labRemindShowValue.SetWindowText(lpLang->Text(158));
	labRemindSound.SetWindowText(lpLang->Text(159));
	chkPlayPcSpeakerAlso.SetWindowText(lpLang->Text(160));
	chkAutoRun.SetWindowText(lpLang->Text(156));
	btnOk.SetWindowText(lpLang->Text(27));
	btnCancel.SetWindowText(lpLang->Text(28));
	chkShowBarOnClick.SetWindowText(lpLang->Text(189));
	grpPutAwayTask.SetWindowText(lpLang->Text(191));
	labPutAwayTask.SetWindowText(lpLang->Text(192));

	Utils::SetupXPButton(btnTestRemindSound, IDI_CFG_TESTSOUND, 161);

	//Styles list
	listClockStyle.ResetContent();
	listClockStyle.InsertString(0, lpMain->ML.Text(187));
	listClockStyle.InsertString(1, lpMain->ML.Text(188));
	listClockStyle.SetCurSel(lpMain->Settings.iClockStyle);

	//Load languages list
	lpLang->GetListOfAvailableLangFiles();
	listLanguage.ResetContent();
	std::string ShortName = "";
	int iSelectItem = -1;
	for (int IDX = 0; IDX < (int)Language::GetLangFilesList().size(); IDX++)
	{
		ShortName = Language::GetLangFilesList()[IDX];
		int LangID = Language::ShortNameToLangID(ShortName);
		int iInsertedItem = listLanguage.InsertString(IDX, Language::GetLanguageLocalizedName(LangID).c_str());
		if (lpLang->CurrentLangID() == LangID)
			iSelectItem = iInsertedItem;
	}
	if (iSelectItem != -1)
		listLanguage.SetCurSel(iSelectItem);

	//Hotkeys
	std::string HotKeyStr1 = lpMain->Settings.HotKey_ShowWorkBar.substr(2);
	std::string HotKeyStr2 = lpMain->Settings.HotKey_NewTask.substr(2);
	std::string HotKeyStr3 = lpMain->Settings.HotKey_NewNote.substr(2);

	std::string chkHotKeyState1 = lpMain->Settings.HotKey_ShowWorkBar.substr(0, 1);
	std::string chkHotKeyState2 = lpMain->Settings.HotKey_NewTask.substr(0, 1);
	std::string chkHotKeyState3 = lpMain->Settings.HotKey_NewNote.substr(0, 1);

	chkHotKey1.SetCheck((chkHotKeyState1 == "+")?BST_CHECKED:BST_UNCHECKED);
	chkHotKey2.SetCheck((chkHotKeyState2 == "+")?BST_CHECKED:BST_UNCHECKED);
	chkHotKey3.SetCheck((chkHotKeyState3 == "+")?BST_CHECKED:BST_UNCHECKED);

	chkShowBarOnClick.SetCheck((lpMain->Settings.bShowBarOnClick)?BST_CHECKED:BST_UNCHECKED);

	Misc::StringToHotKey(HotKeyStr1, edHotKey1);
	Misc::StringToHotKey(HotKeyStr2, edHotKey2);
	Misc::StringToHotKey(HotKeyStr3, edHotKey3);

	//reenable hotkeys
	OnBnClickedSettingHotkeyChk1();
	OnBnClickedSettingHotkeyChk2();
	OnBnClickedSettingHotkeyChk3();

	//Sound
	LoadSoundFiles();

	//Other
	chkPlayPcSpeakerAlso.SetCheck((lpMain->Settings.PlayPCSpeaker)?BST_CHECKED:BST_UNCHECKED);
	chkAutoRun.SetCheck((lpMain->Settings.AutoStart)?BST_CHECKED:BST_UNCHECKED);

	spinRemindShow.SetRange(0, 180);
	spinHideWindowSeconds.SetRange(0, 30);
	spinPutAwayTask.SetRange(0, 180);

	spinRemindShow.SetPos(lpMain->Settings.MinutesBeforeRemind);
	spinHideWindowSeconds.SetPos(lpMain->Settings.HideWindowAfterSec);
	spinPutAwayTask.SetPos(lpMain->Settings.iPutAwayMinutes);

	//Center
	CenterWindow(CWnd::GetDesktopWindow());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SettingsDlg::OnOK()
{
	//HotKeys
	std::string HotKeyStr1 = ((chkHotKey1.GetCheck() == BST_CHECKED)?"+":"-");
		HotKeyStr1 += " " + Misc::HotKeyToString(edHotKey1);
	std::string HotKeyStr2 = ((chkHotKey2.GetCheck() == BST_CHECKED)?"+":"-");
		HotKeyStr2 += " " + Misc::HotKeyToString(edHotKey2);
	std::string HotKeyStr3 = ((chkHotKey3.GetCheck() == BST_CHECKED)?"+":"-");
		HotKeyStr3 += " " + Misc::HotKeyToString(edHotKey3);

	lpMain->Settings.bShowBarOnClick = (chkShowBarOnClick.GetCheck() == BST_CHECKED);

	//Save new hotkeys
	lpMain->Settings.HotKey_ShowWorkBar = HotKeyStr1;
	lpMain->Settings.HotKey_NewTask = HotKeyStr2;
	lpMain->Settings.HotKey_NewNote = HotKeyStr3;

	std::string chkHotKeyState1 = lpMain->Settings.HotKey_ShowWorkBar.substr(0, 1);
	std::string chkHotKeyState2 = lpMain->Settings.HotKey_NewTask.substr(0, 1);
	std::string chkHotKeyState3 = lpMain->Settings.HotKey_NewNote.substr(0, 1);

	Utils::InitHotKeys();

	//Sound
	CString sSoundFileName;
	listRemindSounds.GetWindowText(sSoundFileName);
	lpMain->Settings.SoundFileName = sSoundFileName;

	//Other
	int iHideWindowSeconds = spinHideWindowSeconds.GetPos();
	int iRemindShow = spinRemindShow.GetPos();
	int iPutAwayTaskForMinutes = spinPutAwayTask.GetPos();

	lpMain->Settings.HideWindowAfterSec = (iHideWindowSeconds > 30)?30:iHideWindowSeconds;
	lpMain->HideDelay = lpMain->Settings.HideWindowAfterSec * 1000;

	lpMain->Settings.iPutAwayMinutes = (iPutAwayTaskForMinutes > 180)?180:iPutAwayTaskForMinutes;
	lpMain->Settings.MinutesBeforeRemind = (iRemindShow > 180)?180:iRemindShow;
	lpMain->Settings.PlayPCSpeaker = (chkPlayPcSpeakerAlso.GetCheck() == BST_CHECKED);
	lpMain->Settings.AutoStart = (chkAutoRun.GetCheck() == BST_CHECKED);
	lpMain->Settings.iClockStyle = listClockStyle.GetCurSel();

	//Save language
	if (!ShortLangName.empty())
	{
		lpMain->ML.SetLanguage(ShortLangName);
		lpMain->Settings.Language = ShortLangName;

		//Reload strings
		Utils::SetControlsStrings();
		lpMain->dlgToday.LoadListItems();

		lpMain->dlgTasks.InitControls();
		lpMain->dlgTasks.LoadListItems();

		lpMain->dlgNotes.InitControls();
		lpMain->dlgNotes.LoadListItems();
	}

	lpMain->Settings.Write();

	CDialog::OnOK();
}

void SettingsDlg::LoadSoundFiles()
{
	std::string Dir = Utils::SoundFolder();
	listRemindSounds.ResetContent();
	int idx = 0;

	//Load system sounds
	listRemindSounds.InsertString(idx++, lpMain->ML.Text(162));
	listRemindSounds.InsertString(idx++, "Default.system");
	listRemindSounds.InsertString(idx++, "Asterisk.system");
	listRemindSounds.InsertString(idx++, "Exclamation.system");
	listRemindSounds.InsertString(idx++, "Hand.system");

	//Load wav sounds
	if (File::DirectoryExists(Dir))
	{
		HANDLE					hSearch;
		WIN32_FIND_DATA	_FileData;
		hSearch = FindFirstFile((Dir + "*.*").c_str(), &_FileData); 
		if (hSearch != INVALID_HANDLE_VALUE) 
		{ 
			while (true)
			{
				if (FindNextFile(hSearch, &_FileData))
				{
					if ((_FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
					{
					} else {
						if (File::FileExtension(_FileData.cFileName) == "wav")
							listRemindSounds.InsertString(idx++, _FileData.cFileName);
					}
				} else {
					break;
				}
			}
			FindClose(hSearch);
		}
	}

	//Select saved
	int iSelectItem = listRemindSounds.FindString(-1, lpMain->Settings.SoundFileName.c_str());
	if (iSelectItem != CB_ERR)
		listRemindSounds.SetCurSel(iSelectItem);
}

void SettingsDlg::OnCbnSelchangeListLang()
{
	int SelectedIndex = listLanguage.GetCurSel();
	if (SelectedIndex <= (int)Language::GetLangFilesList().size())
		ShortLangName = Language::GetLangFilesList()[SelectedIndex];
}

void SettingsDlg::OnBnClickedSettingHotkeyChk1()
{
	BOOL Chk = (chkHotKey1.GetCheck() == BST_CHECKED);
	edHotKey1.EnableWindow(Chk);
	labHotKey1.EnableWindow(Chk);
}

void SettingsDlg::OnBnClickedSettingHotkeyChk2()
{
	BOOL Chk = (chkHotKey2.GetCheck() == BST_CHECKED);
	edHotKey2.EnableWindow(Chk);
	labHotKey2.EnableWindow(Chk);
}

void SettingsDlg::OnBnClickedSettingHotkeyChk3()
{
	BOOL Chk = (chkHotKey3.GetCheck() == BST_CHECKED);
	edHotKey3.EnableWindow(Chk);
	labHotKey3.EnableWindow(Chk);
}

void SettingsDlg::OnBnClickedBtnTestsound()
{
	CString S;
	listRemindSounds.GetWindowText(S);
	Utils::PlayInfoSound((LPCTSTR)S, (chkPlayPcSpeakerAlso.GetCheck() == BST_CHECKED));
}
