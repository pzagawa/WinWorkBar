// PSTabCtrlPage.cpp : implementation file
//

#include "PSTabCtrlPage.h"
#include <uxtheme.h>
#include <Tmschema.h>

// CPSTabCtrlPage dialog

CPSTabCtrlPage::CPSTabCtrlPage(CWnd* pParent)
	: CDialog(CPSTabCtrlPage::IDD, pParent)
{
	lpThemeHelper = NULL;
}

CPSTabCtrlPage::CPSTabCtrlPage(UINT nIDTemplate, CWnd* pParent)
	: CDialog(CPSTabCtrlPage::IDD, pParent)
{
}

CPSTabCtrlPage::~CPSTabCtrlPage()
{
}

void CPSTabCtrlPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPSTabCtrlPage, CDialog)
//	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CPSTabCtrlPage message handlers

BOOL CPSTabCtrlPage::DrawDefaultBackground(CDC* pDC, bool DefaultDrawing, bool Border)
{
	CRect R;
	GetClientRect(&R);
	if (DefaultDrawing)
	{
		//..else default drawing
		CBrush Bkg(GetSysColor(COLOR_BTNFACE));
		CBrush* lpDefBrush = pDC->SelectObject(&Bkg);
		CPen Line(PS_SOLID, 1, (Border)?0xAB7E74:GetSysColor(COLOR_BTNFACE));
		CPen* lpDefPen = pDC->SelectObject(&Line);
		pDC->Rectangle(&R);
		pDC->SelectObject(lpDefBrush);
		pDC->SelectObject(lpDefPen);
		return TRUE;
	} else {
		//if visual styles enabled, then xp drawing
		if ((lpThemeHelper) && (lpThemeHelper->IsAppThemed() == TRUE))
		{
			HTHEME hTheme = lpThemeHelper->OpenThemeData(GetSafeHwnd(), L"TAB");
			if (hTheme)
			{
				lpThemeHelper->DrawThemeBackground(hTheme, GetSafeHwnd(), pDC->GetSafeHdc(), TABP_BODY, 0, &R, NULL);
				lpThemeHelper->CloseThemeData(hTheme);
				return TRUE;
			}
		}
	}
	return TRUE;
}

void CPSTabCtrlPage::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	//CDialog::OnOK();
}

void CPSTabCtrlPage::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	//CDialog::OnCancel();
}

