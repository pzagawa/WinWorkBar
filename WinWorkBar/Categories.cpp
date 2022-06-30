// Categories.cpp : implementation file
//

#include "Categories.h"
#include "WinWorkBarDlg.h"
#include <algorithm>
#include "Libs\PSMFCUtils.h"
#include "Libs\PSUtil.h"

#undef	StrToInt 

// CCategories dialog

extern CWinWorkBarDlg*	lpMain;

IMPLEMENT_DYNAMIC(CCategories, CDialog)
CCategories::CCategories(CWnd* pParent /*=NULL*/)
	: CDialog(CCategories::IDD, pParent)
{
	dwCategories = 0;
}

CCategories::~CCategories()
{
}

void CCategories::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CATEGORY, listCategories);
	DDX_Control(pDX, IDC_GRP1, grp1);
	DDX_Control(pDX, IDC_ED_SELECTED, labCategories);
	DDX_Control(pDX, IDOK, btnOK);
	DDX_Control(pDX, ID_BTN_EDIT, btnEditCategories);
	DDX_Control(pDX, ID_BTN_DESELECTALL, btnDeselectAll);
	DDX_Control(pDX, IDCANCEL, btnCancel);
	DDX_Control(pDX, ID_BTN_SELECTALL, btnSelectAll);
}


BEGIN_MESSAGE_MAP(CCategories, CDialog)
	ON_BN_CLICKED(ID_BTN_EDIT, OnBnClickedBtnEdit)
	ON_BN_CLICKED(ID_BTN_DESELECTALL, OnBnClickedBtnDeselectall)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CATEGORY, OnLvnItemchangedListCategory)
	ON_BN_CLICKED(ID_BTN_SELECTALL, OnBnClickedBtnSelectall)
END_MESSAGE_MAP()


// CCategories message handlers

BOOL CCategories::OnInitDialog()
{
	CDialog::OnInitDialog();

	lpLang = &lpMain->ML;

	//Set lang
	SetWindowText(lpLang->Text(47));
	btnOK.SetWindowText(lpLang->Text(49));
	btnCancel.SetWindowText(lpLang->Text(28));
	btnEditCategories.SetWindowText(lpLang->Text(25));
	btnDeselectAll.SetWindowText(lpLang->Text(48));
	btnSelectAll.SetWindowText(lpLang->Text(51));
	grp1.SetWindowText(lpLang->Text(50));
	
	//Prepare list
	CRect R;
	listCategories.GetClientRect(&R);
	listCategories.InsertColumn(0, lpLang->Text(29), LVCFMT_LEFT, R.Width() - GetSystemMetrics(SM_CXVSCROLL));
	listCategories.InsertColumn(1, "ID", LVCFMT_RIGHT, 0);
	listCategories.SetExtendedStyle(LVS_EX_CHECKBOXES);

	LoadListItems();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
   CString    strItem1 = pListCtrl->GetItemText((int)lParam1, 0);
   CString    strItem2 = pListCtrl->GetItemText((int)lParam2, 0);
   return strcmp(strItem1, strItem2);
}

bool	CCategories::LoadListItems()
{
	PSXML::PSTinyXML&	TB = lpMain->tbCategories;
	listCategories.DeleteAllItems();
	int ItemIndex = 0;
	for (int i = 0; i <= 31; i++)
	{
		std::string ItemID = PSXML::IntToStr(i + 1);
		if (TB.Locate(ItemID))
		{				
			if (TB.GetData())
			{
				std::string S = TB.FieldByName("Name").AsString();
				if (!S.empty())
				{
					listCategories.InsertItem(ItemIndex, S.c_str());
					listCategories.SetItemData(ItemIndex, ItemIndex);
					listCategories.SetItemText(ItemIndex, 1, TB.GetID().c_str());
					ItemIndex++;
				}
			}
		}
	}
	listCategories.SortItems(MyCompareProc, (LPARAM)&listCategories);
	SetCategories(dwCategories);
	return true;
}

std::string CCategories::CategoriesAsString()
{
	std::string S;
	for (int i = 0; i < listCategories.GetItemCount(); i++)
	{
		if (listCategories.GetCheck(i))
		{
			S+=listCategories.GetItemText(i, 0);
			S+=", ";
		}
	}	
	size_t Pos = S.rfind(',');
	if (Pos != std::string::npos)
		S = S.erase(Pos);
	return S;
}

std::string	CCategories::CategoriesAsString(DWORD C)
{
	PSXML::PSTinyXML&	TB = lpMain->tbCategories;
	std::string S;
	std::vector<std::string> List;
	if (TB.RowCount() > 0)
	{
		TB.First();
		while (!TB.IsEof())
		{
			TB.GetData();
			int ItemID = TB.GetIDAsInt();
			bool IsBitSet = (C & ((DWORD)1 << (ItemID - 1))?true:false);
			if (IsBitSet)
			{
				std::string Name = TB.FieldByName("Name").AsString();
				if (!Name.empty())
					List.push_back(Name);
			}
			TB.Next();
		}
	}

	std::sort(List.begin(), List.end());
	for (int i = 0; i < (int)List.size(); i++)
		S += List[i] + ", ";

	size_t Pos = S.rfind(',');
	if (Pos != std::string::npos)
		S = S.erase(Pos);
	return S;
}

DWORD	CCategories::GetCategories()
{
	DWORD C = 0;
	for (int i = 0; i < listCategories.GetItemCount(); i++)
	{
		if (listCategories.GetCheck(i))
		{
			int ID = PSXML::StrToInt((std::string)listCategories.GetItemText(i, 1));
			C |= (DWORD)1 << (ID - 1);
		}
	}	
	return C;
}

void	CCategories::SetCategories(DWORD C)
{
	for (int i = 0; i < listCategories.GetItemCount(); i++)
	{
		int ID = PSXML::StrToInt((std::string)listCategories.GetItemText(i, 1));
		listCategories.SetCheck(i, (C & ((DWORD)1 << (ID - 1))?TRUE:FALSE));
	}	
}

void CCategories::OnOK()
{
	dwCategories = GetCategories();
	CDialog::OnOK();
}

void CCategories::OnCancel()
{

	CDialog::OnCancel();
}

void CCategories::OnBnClickedBtnEdit()
{
	lpMain->dlgEditCategories.DoModal();
}

void CCategories::OnBnClickedBtnDeselectall()
{
	for (int i = 0; i < listCategories.GetItemCount(); i++)
		listCategories.SetCheck(i, FALSE);
}

void CCategories::OnBnClickedBtnSelectall()
{
	for (int i = 0; i < listCategories.GetItemCount(); i++)
		listCategories.SetCheck(i, TRUE);
}

void CCategories::OnLvnItemchangedListCategory(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uChanged == LVIF_STATE)
	{
		labCategories.SetWindowText(CategoriesAsString().c_str());
	}
	*pResult = 0;
}

