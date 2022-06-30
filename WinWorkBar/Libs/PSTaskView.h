#pragma once

#include "MemDC.h"
#include <string>
#include <vector>

// PSTaskView

class PSTaskView;

class PSTaskViewItem
{
public:
	CRect	R;
	std::string ID;
	std::string Name;
	bool Clicked;
	bool TypeIsTask;
	bool IsTitle;
	bool IsSubItem;
	bool MouseOver;
	bool TestCursorOver(CWnd* Window, int Yposition);
	bool TestCursorPos(CWnd* Window, int Yposition);
	PSTaskViewItem()
	{
		Clicked = false;
		TypeIsTask = true;
		IsTitle = false;
		IsSubItem = false;
		MouseOver = false;
	}
};

typedef	void (*eventOnTaskViewClickItem)(PSTaskView& TaskView, CPoint& P, PSTaskViewItem& Item);


class PSTaskView : public CStatic
{
	DECLARE_DYNAMIC(PSTaskView)

public:
	enum	_Type
	{
		tpTask,
		tpNote,
	};

	class _Title : public PSTaskViewItem
	{
	public:
		int					Done;
		int					Count;
		_Title()
		{
			Done = 0;
			Count = 0;
			IsTitle = true;
		}
	};

	class _SubItem : public PSTaskViewItem
	{
	public:
		int					timeHour;
		int					timeMinute;
		int					Progress;		//100 % = done
		int					Priority;		//-1 - low, 0 - medium, 1 - high
		UINT				UserData;
		_SubItem()
		{
			timeHour = 0;
			timeMinute = 0;
			Progress = 0;
			Priority = 0;
			UserData = 0;
			IsSubItem = true;
		}
	};

	struct _ListItem
	{
		_Type									Type;
		_Title								Title;
		std::vector<_SubItem>	SubItems;
		CRect									R;
		_ListItem()
		{
			Type = tpTask;
		}
	};

private:
	enum	_Fonts
	{
		ftDefault,
		ftBold,
		ftUnderline,
		ftStrikeOut,
	};

	int					iViewHeight;
	int					iScrollBarWidth;
	CScrollBar	scrollBarV;

	int			YPos;
	CRgn		CtrlRegion;
	CRect		Rect;
	CRect		ClipRect;
	CRect		ScrollRect;
	CFont		FontList[4];
	CFont		FontListHot[4];
	std::vector<_ListItem>	Items;

	SYSTEMTIME	stCurrentTime;
	
	bool IsCurrentTime(_SubItem& SI);
	void DrawBackground(CDC& dc);
	void DrawBorder(CDC& dc);
	void ClearScrollBackground(CDC& dc);
	void DrawTitle(CDC& dc, _ListItem& LI, int Y);
	void DrawItem(CDC& dc, _SubItem& I, _Type Type, int Y);
	void CreateFonts();
	void SetYScroll(int Y);
	void UpdateScrollBar();
	void TestCursorOverItems();
	void TestItemClick(CPoint point);
public:
	std::string SelectedTaskID;
	std::string SelectedNoteID;

	bool	b24HoursTime;
	eventOnTaskViewClickItem	OnTaskViewClickItem;

	PSTaskView();
	virtual ~PSTaskView();

	void InitializeAll();			//CALL THIS IN OnInitDialog() !!!

	void Clear();
	void Add(_ListItem& Item);
	void UpdateSize();
	void RePosition();
	void SetCurrentTime(SYSTEMTIME& ST);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


