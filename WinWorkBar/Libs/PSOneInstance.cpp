#include "PSOneInstance.h"

// --------------------------------------------------------------------------------

namespace Util
{
	PSOneInstance::PSOneInstance(HWND MainApplicationWindow, std::string AppID)
	{
		_MapFile = NULL;
		_lpView = NULL;
		_AlreadyRunning = false;
		_AppWindow = MainApplicationWindow;
		Check(AppID);
	}

	PSOneInstance::~PSOneInstance(void)
	{
		if (_lpView)
		{
			UnmapViewOfFile(_lpView);
			_lpView = NULL;
		}
		if (_MapFile)
		{
			CloseHandle(_MapFile);
			_MapFile = NULL;
		}
	}

	void PSOneInstance::Check(const std::string AppSign)
	{
		_AlreadyRunning = false;
		_MapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, AppSign.c_str());
		if (_MapFile)
		{
			_AlreadyRunning = (GetLastError() == ERROR_ALREADY_EXISTS);
			_lpView = MapViewOfFile(_MapFile, FILE_MAP_WRITE, 0, 0, 0);
			if (_lpView)
			{
				if (_AlreadyRunning)
				{
					CopyMemory(&_AppWindow, _lpView, sizeof(_AppWindow));
				} else {
					CopyMemory(_lpView, &_AppWindow, sizeof(_AppWindow));
				}
				UnmapViewOfFile(_lpView);
				//if (DestroyOnExit)
				//{
				//	CloseHandle(_MapFile);
				//	_MapFile = NULL;
				//}
			} else {
				_AlreadyRunning = true;
			}
		} else {
			_AlreadyRunning = true;
		}
	}

	bool	PSOneInstance::BringRunningAppToFront()
	{
		if (_AppWindow)
		{
			if (IsIconic(_AppWindow))
				SendMessage(_AppWindow, WM_SYSCOMMAND, SC_RESTORE, 0);
			HWND TopWindow = GetLastActivePopup(_AppWindow);
			if (TopWindow && (TopWindow != _AppWindow) && IsWindowVisible(TopWindow) && IsWindowEnabled(TopWindow))
				SetForegroundWindow(TopWindow);
			return true;
		}
		return false;
	}
}

