// EditCategories.cpp : implementation file
//

#include "EditCategories.h"
#include "WinWorkBarDlg.h"

// CEditCategories dialog

extern CWinWorkBarDlg*	lpMain;

IMPLEMENT_DYNAMIC(CEditCategories, CDialog)
CEditCategories::CEditCategories(CWnd* pParent /*=NULL*/)
	: CDialog(CEditCategories::IDD, pParent)
{
}

CEditCategories::~CEditCategories()
{
}

void CEditCategories::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_CATEGORY, edCategoryName);
	DDX_Control(pDX, IDC_LIST_CATEGORY, listCategories);
	DDX_Control(pDX, IDOK, btnOK);
	DDX_Control(pDX, IDCANCEL, btnCancel);
	DDX_Control(pDX, IDC_BTN_ADD, btnAdd);
	DDX_Control(pDX, ID_BTN_DEFAULT, btnDefault);
}


BEGIN_MESSAGE_MAP(CEditCategories, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CATEGORY, OnLvnItemchangedListCategory)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
	ON_BN_CLICKED(ID_BTN_DEFAULT, OnBnClickedBtnDefault)
END_MESSAGE_MAP()


// CEditCategories message handlers

BOOL CEditCategories::OnInitDialog()
{
	CDialog::OnInitDialog();

	lpLang = &lpMain->ML;

	//Set lang
	SetWindowText(lpLang->Text(25));
	btnAdd.SetWindowText(lpLang->Text(26));
	btnOK.SetWindowText(lpLang->Text(27));
	btnCancel.SetWindowText(lpLang->Text(28));
	btnDefault.SetWindowText(lpLang->Text(46));
	//Prepare list
	CRect R;
	listCategories.GetClientRect(&R);
	listCategories.InsertColumn(0, lpLang->Text(29), LVCFMT_LEFT, R.Width() - 40 - GetSystemMetrics(SM_CXVSCROLL));
	listCategories.InsertColumn(1, "ID", LVCFMT_RIGHT, 40);

	LoadListItems();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool	CEditCategories::LoadListItems()
{
	PSXML::PSTinyXML&	TB = lpMain->tbCategories;
	bUpdateMode = false;
	SetButtonText();
	edCategoryName.SetWindowText("");
	listCategories.DeleteAllItems();
	for (int i = 0; i <= 31; i++)
	{
		std::string ItemID = PSXML::IntToStr(i + 1);
		if (TB.Locate(ItemID))
		{
			if (TB.GetData())
			{
				listCategories.InsertItem(i, TB.FieldByName("Name").AsString().c_str());
				listCategories.SetItemText(i, 1, ItemID.c_str());
			}
		} else {
			listCategories.InsertItem(i, "");
			listCategories.SetItemText(i, 1, ItemID.c_str());
		}
	}
	return true;
}

bool	CEditCategories::SaveListItems()
{
	PSXML::PSTinyXML&	TB = lpMain->tbCategories;
	for (int i = 0; i < listCategories.GetItemCount(); i++)
	{
		CString ID = listCategories.GetItemText(i, 1);
		std::string ItemID = (LPCTSTR)ID;
		CString Name = listCategories.GetItemText(i, 0);
		std::string S = (LPCTSTR)Name;
		if (TB.Locate(ItemID))
		{
			if (TB.GetData())
			{
				TB.FieldByName("Name").Set(S);
				TB.FieldByName("Date").SetNowDate();
				TB.UpdateData();
			}
		} else {
			TB.SetID(ItemID);
			TB.FieldByName("Name").Set(S);
			TB.FieldByName("Date").SetNowDate();
			TB.AddRow();
		}
	}
	Utils::SetupViewMenu();
	return true;
}

void CEditCategories::OnLvnItemchangedListCategory(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->uChanged == LVIF_STATE)
	{
		if (pNMLV->iItem == -1)
		{
			bUpdateMode = false;
			edCategoryName.SetWindowText("");
		} else {
			CString S = listCategories.GetItemText(pNMLV->iItem, 0);
			edCategoryName.SetWindowText(S);
			bUpdateMode = !S.IsEmpty();
		}
		SetButtonText();
	}
	*pResult = 0;
}

void CEditCategories::SetButtonText()
{
	if (bUpdateMode)
	{
		btnAdd.SetWindowText(lpLang->Text(31));
	} else {
		btnAdd.SetWindowText(lpLang->Text(26));
	}
}

void CEditCategories::OnOK()
{
	//Get list strings and save to table !
	PSXML::PSTinyXML&	TB = lpMain->tbCategories;
	SaveListItems();
	if (TB.SaveTable())
	{
		if (lpMain->dlgCategories.IsWindowVisible() == TRUE)
			lpMain->dlgCategories.LoadListItems();
	} else {
		std::string S = lpLang->Text(32);
		S+= ":\n";
		ShowError(S + TB.TableFileName());
	}

	CDialog::OnOK();
}

void CEditCategories::OnBnClickedBtnAdd()
{
	CString NewName;
	edCategoryName.GetWindowText(NewName);
	NewName.MakeUpper();
	//Name existing test
	for (int i = 0; i < listCategories.GetItemCount(); i++)
	{
		CString Name = listCategories.GetItemText(i, 0);
		if ((!NewName.IsEmpty()) && (NewName == Name.MakeUpper()))
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}
	}
	//Save
	if (bUpdateMode)
	{
		int ItemID = listCategories.GetSelectionMark();
		if (ItemID != -1)
		{
			CString S;
			edCategoryName.GetWindowText(S);
			S.Trim();
			listCategories.SetItemText(ItemID, 0, S);
			listCategories.SetItemState(0, 0, LVIS_SELECTED);
			bUpdateMode = false;
			SetButtonText();
			return;
		}
	} else {
		//Save to first empty position
		for (int i = 0; i < listCategories.GetItemCount(); i++)
		{
			CString Name = listCategories.GetItemText(i, 0);
			if (Name.IsEmpty())
			{
				edCategoryName.GetWindowText(NewName);
				listCategories.SetItemText(i, 0, NewName);
				listCategories.EnsureVisible(i, FALSE);
				return;
			}
		}
	}
	MessageBeep(MB_ICONEXCLAMATION);
}

void CEditCategories::OnBnClickedBtnDefault()
{
	if (ShowUserMsg(this, umtWarning_YesNo, 116) == IDYES)
	{
		Utils::LoadAndResetTable_Categories(true);
		LoadListItems();
		Utils::SetupViewMenu();
	}
}
