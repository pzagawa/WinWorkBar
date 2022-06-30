// 
// PSOneInstance - blocks next instances of program
// ver 1.0: 29-03-2003 / 12:55
// 

#pragma    once

#include <Windows.h>
#include <string>

// --------------------------------------------------------------------------------

namespace Util
{
	class PSOneInstance
	{
		HANDLE	_MapFile;
		LPVOID	_lpView;
		HWND		_AppWindow;
		bool		_AlreadyRunning;
		void		Check(const std::string AppSign);
	public:
		PSOneInstance(HWND MainApplicationWindow, std::string AppID);
		~PSOneInstance(void);
		bool	BringRunningAppToFront();
		const bool AlreadyRunning() { return _AlreadyRunning; };
	};
}
