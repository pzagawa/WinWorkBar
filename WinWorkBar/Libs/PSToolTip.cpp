#include "pstooltip.h"

PSToolTip::PSToolTip(void)
{
	m_pWnd = NULL;
	m_ToolTip.m_hWnd = NULL;
}

PSToolTip::~PSToolTip(void)
{
}

void PSToolTip::Set(CWnd* pWnd, bool EntireControlRectangle)
{
	m_pWnd = pWnd;
	if (m_ToolTip.m_hWnd)
	{
		m_ToolTip.DestroyWindow();
		m_ToolTip.m_hWnd = NULL;
	}
	m_ToolTip.Create(m_pWnd, 0);
	m_ToolTip.Activate(false);
	m_ToolTip.SetMaxTipWidth(400);
	if (EntireControlRectangle)
		AddRect("");

	//TTDT_AUTOPOP   Retrieve the length of time the tool tip window remains visible if the pointer is stationary within a tool's bounding rectangle. 
	//TTDT_INITIAL   Retrieve the length of time the pointer must remain stationary within a tool's bounding rectangle before the tool tip window appears. 
	//TTDT_RESHOW   Retrieve the length of time it takes for subsequent tool tip windows to appear as the pointer moves from one tool to another. 
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 4000);
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 100);
	m_ToolTip.SetDelayTime(TTDT_RESHOW, 50);
}

void PSToolTip::AddRect(LPCTSTR lpText, LPRECT Rect, int ID)
{
	if (lpText)
	{
		CRect rectBtn;
		m_pWnd->GetClientRect(rectBtn);
		if (Rect)
			rectBtn = *Rect;
		m_ToolTip.AddTool(m_pWnd, lpText, rectBtn, ID);
	}
}

void PSToolTip::RemoveRect(int ID)
{
	if (m_ToolTip.m_hWnd)
		m_ToolTip.DelTool(m_pWnd, ID);
}

void PSToolTip::Update(LPCTSTR lpText, LPRECT Rect, int ID)
{
	if ((m_ToolTip.m_hWnd) && (lpText))
	{
		RemoveRect(ID);
		AddRect(lpText, Rect, ID);
		Text(lpText, ID);
	}
}

void PSToolTip::Text(LPCTSTR lpText, int ID)
{
	if ((m_ToolTip.m_hWnd) && (lpText))
	{
		m_ToolTip.UpdateTipText(lpText, m_pWnd, ID);
		m_ToolTip.Activate(true);
	}
}

void PSToolTip::CheckMsgLoop(MSG* pMsg)
{
	if(pMsg->message== WM_MOUSEMOVE)
		m_ToolTip.RelayEvent(pMsg);
}
