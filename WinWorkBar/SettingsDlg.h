#pragma once

#include "default.h"
#include "resource.h"		// main symbols

#include "LibsOther\BtnST.h"
#include "LibsOther\ThemeHelperST.h"
#include "LibsOther\XPStyleButtonST.h"

#include "Libs\PSLanguage.h"
#include "Libs\PSRegistry.h"

#include "afxwin.h"
#include "afxcmn.h"

// SettingsDlg dialog

class SettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(SettingsDlg)
public:
	SettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SettingsDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG };

	Language::Lang*	lpLang;
	Registry::Reg*	lpPrefs;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	std::string ShortLangName;

	CStatic grp1;
	CStatic grp2;
	CStatic grp3;
	CStatic grp4;
	CStatic grp5;
	CComboBox listLanguage;
	CEdit edHideWindowSeconds;
	CSpinButtonCtrl spinHideWindowSeconds;
	CStatic labHideWindowSeconds;
	CButton chkHotKey1;
	CButton chkHotKey2;
	CButton chkHotKey3;
	CHotKeyCtrl edHotKey1;
	CHotKeyCtrl edHotKey2;
	CHotKeyCtrl edHotKey3;
	CStatic labHotKey1;
	CStatic labHotKey2;
	CStatic labHotKey3;
	CStatic labRemindShow;
	CEdit edRemindShow;
	CSpinButtonCtrl spinRemindShow;
	CStatic labRemindShowValue;
	CStatic labRemindSound;
	CComboBox listRemindSounds;
	CXPStyleButtonST btnTestRemindSound;
	CButton chkPlayPcSpeakerAlso;
	CButton chkAutoRun;
	CButton btnOk;
	CButton btnCancel;
	CStatic grpClockStyle;
	CComboBox listClockStyle;
	CButton chkShowBarOnClick;
	CStatic grpPutAwayTask;
	CEdit edPutAwayTask;
	CStatic labPutAwayTask;
	CSpinButtonCtrl spinPutAwayTask;

	void LoadSoundFiles();

	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeListLang();
	afx_msg void OnBnClickedSettingHotkeyChk1();
	afx_msg void OnBnClickedSettingHotkeyChk2();
	afx_msg void OnBnClickedSettingHotkeyChk3();
	afx_msg void OnBnClickedBtnTestsound();
};
