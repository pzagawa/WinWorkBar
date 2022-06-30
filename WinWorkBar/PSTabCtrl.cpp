// PSTabCtrl.cpp : implementation file
//

#include "PSTabCtrl.h"
#include "LibsOther\utils.h"

// CPSTabCtrl

IMPLEMENT_DYNAMIC(CPSTabCtrl, CTabCtrl)
CPSTabCtrl::CPSTabCtrl()
{
}

CPSTabCtrl::~CPSTabCtrl()
{
	//i set parent of vecPages dialogs to this, so these dialogs destroys it's parent
}


BEGIN_MESSAGE_MAP(CPSTabCtrl, CTabCtrl)
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnTcnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnTcnSelchanging)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CPSTabCtrl message handlers

void CPSTabCtrl::AddPage(std::string Title, CPSTabCtrlPage* Page, int ImageIndex)
{
	TCITEM TI;
	ZeroMemory(&TI, sizeof(TI));
	TI.mask = TCIF_TEXT | ((ImageIndex == -1)?0:TCIF_IMAGE);
	TI.iImage = ImageIndex;
	TI.pszText = (char*)Title.c_str();

	InsertItem((int)vecPages.size(), &TI);
	Page->SetParent(this);

	vecPages.push_back(Page);
	vecWNDs.push_back(Page->GetFocus());
}

void CPSTabCtrl::SetTitle(int ItemID, std::string S)
{
	TCITEM TI;
	ZeroMemory(&TI, sizeof(TI));
	TI.mask = TCIF_TEXT;
	TI.pszText = (char*)S.c_str();
	SetItem(ItemID, &TI);

	int Idx = GetCurSel();
	if (Idx != -1)
	{
		CPSTabCtrlPage* lpPage = vecPages[Idx];
		lpPage->RedrawWindow();
	}
}

void CPSTabCtrl::ResizeDialog(int ItemID)
{
	if ((ItemID != -1) || (ItemID < (int)vecPages.size()))
	{
		CPSTabCtrlPage* lpDlg = vecPages[ItemID];
		if (lpDlg != NULL)
		{
			CRect rect;
			CRect clrect;
			GetItemRect(ItemID, &rect);
			GetClientRect(&clrect);

			int x, y, nWidth, nHeight;
			DWORD dwStyle = GetStyle();

			if (dwStyle & TCS_VERTICAL)
			{
				int nTabWidth = rect.Width () * GetRowCount ();
				x = (dwStyle & TCS_RIGHT) ? 4 : nTabWidth + 4;
				y = 4;
				nWidth = clrect.Width() - nTabWidth - 8;
				nHeight = clrect.Height() - 8;
			} else {
				int nTabHeight = rect.Height () * GetRowCount ();
				x = 4;
				y = (dwStyle & TCS_BOTTOM) ? 4 : nTabHeight + 4;
				nWidth = clrect.Width() - 8;
				nHeight = clrect.Height() - nTabHeight - 8;
			}
			//Add border
			x+=4;
			y+=4;
			nWidth-=8;
			nHeight-=8;
			lpDlg->SetWindowPos (NULL, x, y, nWidth, nHeight, SWP_NOZORDER);
		}
	}
}

void CPSTabCtrl::ShowPage(int ItemID)
{
	SetCurSel(ItemID);
	NMHDR	tNMHDR;
	LRESULT	tResult;
	OnTcnSelchanging(&tNMHDR, &tResult);
	OnTcnSelchange(&tNMHDR, &tResult);
}

void CPSTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CTabCtrl::OnSize(nType, cx, cy);	

	int Idx = GetCurSel();
	if (Idx != -1)
		ResizeDialog(Idx);
}

void CPSTabCtrl::OnTcnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	int Idx = GetCurSel();
	if (Idx == -1)
		return;

	CPSTabCtrlPage* lpPage = vecPages[Idx];
	vecWNDs[Idx] = lpPage->GetFocus();
	lpPage->ShowWindow(SW_HIDE);

	*pResult = 0;
}

void CPSTabCtrl::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	int Idx = GetCurSel();
	if (Idx == -1)
		return;

	CPSTabCtrlPage* lpPage = vecPages[Idx];
	if (vecWNDs[Idx])
		vecWNDs[Idx]->SetFocus();
	ResizeDialog(Idx);
	lpPage->ShowWindow(SW_SHOW);

	*pResult = 0;

	Utils::EnableViewMenuByTab();
}

void CPSTabCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CTabCtrl::OnKillFocus(pNewWnd);
	int Idx = GetCurSel();
	if (Idx == -1)
		return;
	CPSTabCtrlPage* lpPage = vecPages[Idx];
	vecWNDs[Idx] = lpPage->GetFocus();
}

void CPSTabCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CTabCtrl::OnSetFocus(pOldWnd);
	int Idx = GetCurSel();
	if (Idx == -1)
		return;
	if (vecWNDs[Idx])
		vecWNDs[Idx]->SetFocus();
}

