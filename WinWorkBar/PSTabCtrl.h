#pragma once

#include "default.h"
#include "resource.h"		// main symbols

#include "PSTabCtrlPage.h"
#include <string>
#include <vector>

// CPSTabCtrl

class CPSTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CPSTabCtrl)

	std::vector<CPSTabCtrlPage*>	vecPages;
	std::vector<CWnd*>						vecWNDs;

	void ResizeDialog(int ItemID);
public:
	CPSTabCtrl();
	virtual ~CPSTabCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void AddPage(std::string Title, CPSTabCtrlPage* Page, int ImageIndex = -1);
	void SetTitle(int ItemID, std::string S);
	void ShowPage(int ItemID);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

