// ToolInfoWindowDlg.h : header file
//

#pragma once

#include "Default.h"
#include "Resource.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "LibsOther/CLabel.h"

#include <string>

class CToolInfoWindowDlg;

void	SetWindowPosition(CToolInfoWindowDlg& Dlg, int W, int H);
RECT&	WinTaskBarRect(CToolInfoWindowDlg& Dlg);
UINT	WinTaskBarEdge(CRect rc);

// CToolInfoWindowDlg dialog
class CToolInfoWindowDlg : public CDialog
{
// Construction
public:
	CToolInfoWindowDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TOOLINFOWINDOW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	HICON m_hIcon;

	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
public:
	std::string		sUID;

	CRgn					DlgRgn;
	CRgn					DlgFrameRgn;
	CRect					DlgRect;
	CRect					l1R;
	CRect					l2R;
	CRect					prgRect;

	CBrush				ToolTipBkg;
	CBrush				FrameBrush;
	CFont					SysFont;
	CFont					BoldFont;

	CString				lab1;
	CString				lab2;

	HBITMAP				TipInfoIcon;

	CLabel	btnPutAway;
	CLabel	btnShowTask;
	CLabel	btnFinishTask;

	void	Clear();
	void	Set(std::string L1, std::string L2);
	void	SetLabel1(std::string S);
	void	SetLabel2(std::string S);

	void	ShowInfoWindow(std::string sTaskUID);
	void	BringInfoWindowToFront();
	void	HideInfoWindow();

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnStnClickedBtnTiFinish();
	afx_msg void OnStnClickedBtnTiShow();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnStnClickedBtnTiPutaway();
};
