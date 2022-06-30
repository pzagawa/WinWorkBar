// Repeats.cpp : implementation file
//

#include "Repeats.h"
#include "WinWorkBarDlg.h"
#include <algorithm>

// CRepeats dialog

extern CWinWorkBarDlg*	lpMain;

IMPLEMENT_DYNAMIC(CRepeats, CDialog)
CRepeats::CRepeats(CWnd* pParent /*=NULL*/)
	: CDialog(CRepeats::IDD, pParent)
{
	dwRepeats = 0;
}

CRepeats::~CRepeats()
{
}

void CRepeats::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRP_TYPE, grpType);
	DDX_Control(pDX, IDC_GRP_WEEKDAY, grpWeekDay);
	DDX_Control(pDX, IDC_WEEKDAYLIST, listWeekDays);
	DDX_Control(pDX, IDC_RD_OFF, rdOff);
	DDX_Control(pDX, IDC_RD_DAYS, rdDays);
	DDX_Control(pDX, IDC_RD_MONTHS, rdMonths);
	DDX_Control(pDX, IDC_RD_YEARS, rdYears);
	DDX_Control(pDX, IDC_RD_WEEKDAYS, rdWeekDays);
	DDX_Control(pDX, IDOK, btnOK);
	DDX_Control(pDX, IDCANCEL, btnCancel);
}


BEGIN_MESSAGE_MAP(CRepeats, CDialog)
	ON_BN_CLICKED(IDC_RD_DAYS, OnBnClickedRdDays)
	ON_BN_CLICKED(IDC_RD_MONTHS, OnBnClickedRdMonths)
	ON_BN_CLICKED(IDC_RD_OFF, OnBnClickedRdOff)
	ON_BN_CLICKED(IDC_RD_WEEKDAYS, OnBnClickedRdWeekdays)
	ON_BN_CLICKED(IDC_RD_YEARS, OnBnClickedRdYears)
END_MESSAGE_MAP()


// CRepeats message handlers

BOOL CRepeats::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Set lang
	lpLang = &lpMain->ML;
	SetWindowText(lpLang->Text(120));

	grpType.SetWindowText(lpLang->Text(123));
	grpWeekDay.SetWindowText(lpLang->Text(124));

	rdOff.SetWindowText(lpLang->Text(125));
	rdDays.SetWindowText(lpLang->Text(126));
	rdMonths.SetWindowText(lpLang->Text(127));
	rdYears.SetWindowText(lpLang->Text(128));
	rdWeekDays.SetWindowText(lpLang->Text(129));

	btnOK.SetWindowText(lpLang->Text(49));
	btnCancel.SetWindowText(lpLang->Text(28));

	//Day of week list
	CRect R;
	listWeekDays.SetExtendedStyle(LVS_EX_CHECKBOXES);
	listWeekDays.GetWindowRect(&R);
	listWeekDays.InsertColumn(0, "", LVCFMT_LEFT, R.Width() - 4);
	for (int i = 0; i < 7; i++)
	{
		std::string DayName = Language::GetLanguageDayName(lpLang->CurrentLangID(), i + 1);
		listWeekDays.InsertItem(i, DayName.c_str());
	}

	InitControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRepeats::OnOK()
{
	SaveData();
	CDialog::OnOK();
}

void CRepeats::OnCancel()
{
	CDialog::OnCancel();
}

void CRepeats::InitControls()
{
	WORD Type = HIWORD(dwRepeats);
	WORD Data = LOWORD(dwRepeats);

	rdOff.SetCheck((Type == 0)?BST_CHECKED:BST_UNCHECKED);
	rdDays.SetCheck((Type == 1)?BST_CHECKED:BST_UNCHECKED);
	rdMonths.SetCheck((Type == 2)?BST_CHECKED:BST_UNCHECKED);
	rdYears.SetCheck((Type == 3)?BST_CHECKED:BST_UNCHECKED);
	rdWeekDays.SetCheck((Type == 4)?BST_CHECKED:BST_UNCHECKED);

	if (Type == 4)
	{
		listWeekDays.SetCheck(0, Misc::IsBitSet(Data, 0));	//Monday
		listWeekDays.SetCheck(1, Misc::IsBitSet(Data, 1));
		listWeekDays.SetCheck(2, Misc::IsBitSet(Data, 2));
		listWeekDays.SetCheck(3, Misc::IsBitSet(Data, 3));
		listWeekDays.SetCheck(4, Misc::IsBitSet(Data, 4));
		listWeekDays.SetCheck(5, Misc::IsBitSet(Data, 5));
		listWeekDays.SetCheck(6, Misc::IsBitSet(Data, 6));	//Sunday
	}
	EnableControls(Type);
}

void CRepeats::EnableControls(int Type)
{
	bool EnableWeekDays = (Type == 4);
	grpWeekDay.EnableWindow(EnableWeekDays);
	listWeekDays.EnableWindow(EnableWeekDays);
}

void CRepeats::OnBnClickedRdOff()
{
	EnableControls(0);
}

void CRepeats::OnBnClickedRdDays()
{
	EnableControls(1);
}

void CRepeats::OnBnClickedRdMonths()
{
	EnableControls(2);
}

void CRepeats::OnBnClickedRdYears()
{
	EnableControls(3);
}

void CRepeats::OnBnClickedRdWeekdays()
{
	EnableControls(4);
}

void CRepeats::SaveData()
{
	WORD Type = 0;
	WORD Data = 0;

	if (rdDays.GetCheck() == BST_CHECKED)
		Type = 1;
	if (rdMonths.GetCheck() == BST_CHECKED)
		Type = 2;
	if (rdYears.GetCheck() == BST_CHECKED)
		Type = 3;
	if (rdWeekDays.GetCheck() == BST_CHECKED)
		Type = 4;

	if (Type == 4)
	{
		(listWeekDays.GetCheck(0))?(Misc::SetBit(Data, 0)):0;	//Monday
		(listWeekDays.GetCheck(1))?(Misc::SetBit(Data, 1)):0;
		(listWeekDays.GetCheck(2))?(Misc::SetBit(Data, 2)):0;
		(listWeekDays.GetCheck(3))?(Misc::SetBit(Data, 3)):0;
		(listWeekDays.GetCheck(4))?(Misc::SetBit(Data, 4)):0;
		(listWeekDays.GetCheck(5))?(Misc::SetBit(Data, 5)):0;
		(listWeekDays.GetCheck(6))?(Misc::SetBit(Data, 6)):0;
	}
	dwRepeats = (Type << 16) | Data;
}

std::string CRepeats::RepeatsAsString(DWORD dwData)
{
	WORD Type = HIWORD(dwData);
	WORD Data = LOWORD(dwData);
	std::string S;
	switch (Type)
	{
		case 1: S = lpMain->ML.Text(126); break;
		case 2: S = lpMain->ML.Text(127); break;
		case 3: S = lpMain->ML.Text(128); break;
		case 4: S = lpMain->ML.Text(129); break;
	}
	if (Type == 4)
	{
		S += ": ";
		std::string D;
		for (int i = 0; i < 7; i++)
		{
			if (Misc::IsBitSet(Data, i))
				D += Language::GetLanguageDayName(lpMain->ML.CurrentLangID(), i + 1) + ", ";
		}
		size_t Pos = D.rfind(',');
		if (Pos != std::string::npos)
			D = D.erase(Pos);
		S += D;
	}
	return S;
}
