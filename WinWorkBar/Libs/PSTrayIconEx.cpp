
//---------------------------------------------------------------------------

#include "PSTrayIconEx.h"
#include "PSMFCUtils.h"

//---------------------------------------------------------------------------

const UINT m_TaskbarCreatedMsg = ::RegisterWindowMessage(_T("TaskbarCreated"));


namespace TrayIconEx
{
	TI::TI()
	{
		tnid.cbSize = sizeof(NOTIFYICONDATA);

		_IconAdded = false;
		_hwnd = NULL;
		_uID = -1;
		_Message = 0;

		fnOnEvent = NULL;
		fnOnMenuShow = NULL;
	}

	TI::~TI()
	{
		Delete();
	}

	bool	TI::Init(HWND hwnd, UINT uID, UINT Message)
	{
		_hwnd = hwnd;
		_uID = uID;
		_Message = Message;
		_MID = -1;
		return true;
	}

	bool	TI::SetIcon(HICON hicon, std::string Text)
	{
		if (_IconAdded)
		{
			Modify(hicon);
			ModifyToolTip((char*)Text.c_str());
		} else {
			Add(hicon, (char*)Text.c_str());
		}
		return true;
	}

	bool	TI::SetToolTip(std::string Text)
	{
		if (_IconAdded)
			ModifyToolTip((char*)Text.c_str());
		return true;
	}

	bool	TI::Initialised()
	{
		return ((_hwnd) && (_uID > 0) && (_Message > WM_USER));
	}

	bool	TI::Add(HICON hicon, LPSTR lpszTip)
  { 
		if (!Initialised())
			return false;

    tnid.hWnd = _hwnd; 
    tnid.uID = _uID; 
    tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
    tnid.uCallbackMessage = _Message; 
    tnid.hIcon = hicon;
    if (lpszTip) 
      lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip)); 
    else 
      tnid.szTip[0] = '\0'; 

		_IconAdded = true;
    return (Shell_NotifyIcon(NIM_ADD, &tnid) == TRUE);
  } 

  bool	TI::Delete() 
  { 
		if (!Initialised())
			return false;

    tnid.hWnd = _hwnd; 
    tnid.uID = _uID;

		_IconAdded = false;
        
    return (Shell_NotifyIcon(NIM_DELETE, &tnid) == TRUE);
  }

  bool	TI::Modify(HICON hicon)
  {
		if (!Initialised())
			return false;

    tnid.hWnd   = _hwnd; 
    tnid.uID    = _uID;   
    tnid.uFlags = NIF_ICON;
    tnid.hIcon = hicon;

    return (Shell_NotifyIcon(NIM_MODIFY, &tnid) == TRUE);
  }

  bool	TI::ModifyToolTip(LPSTR lpszTip)
  {
		if (!Initialised())
			return false;

    tnid.hWnd = _hwnd;
    tnid.uID	= _uID;
    tnid.uFlags = NIF_TIP;
    if (lpszTip) 
      lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip)); 
    else 
      tnid.szTip[0] = '\0'; 

    return (Shell_NotifyIcon(NIM_MODIFY, &tnid) == TRUE);
  }

	bool	TI::SetMenuResource(UINT MID)
	{
		_MID = MID;
		return true;
	}

	bool	TI::DoWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		//Is this message for TrayIcon ?
		if ((message == _Message) && (wParam == _uID))
		{
			if (fnOnEvent)
				fnOnEvent(lParam);

			OnMenuNotification(wParam, lParam);

			return true;
		}

		//Taskbar recreated, reload icons !
    if (message == m_TaskbarCreatedMsg)
		{
			std::string S = tnid.szTip;
			HICON I = tnid.hIcon;
			Add(I, (char*)S.c_str());
			return true;
		}

		return false;
	}

	LRESULT TI::OnMenuNotification(WPARAM wParam, LPARAM lParam) 
	{
		if (_MID == -1)
			return FALSE;

    if (LOWORD(lParam) == WM_RBUTTONUP)
    {    
			HMENU hMenu = ::LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(_MID));
      if (!hMenu)
				return FALSE;

			HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
			if (!hSubMenu)
				return FALSE;

			if (fnOnMenuShow)
				fnOnMenuShow(hSubMenu);

      // Display and track the popup menu
      POINT pos;
      GetCursorPos(&pos);

      ::SetForegroundWindow(_hwnd);
      ::TrackPopupMenu(hSubMenu, 0, pos.x, pos.y, 0, _hwnd, NULL);

      // BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
      ::PostMessage(_hwnd, WM_NULL, 0, 0);

      DestroyMenu(hMenu);
    }
    return TRUE;
	}


};
