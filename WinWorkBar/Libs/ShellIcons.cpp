
#include "ShellIcons.h"

#ifndef _AFXDLL

#include <vector>
#include <string>

namespace std
{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}

namespace {

/**
*	Yet another tokenizer
*
*	\param	x			Output iterator
*	\param	in			The string to tokenize
*	\param	delimiters	A string containing token separators
*
*	\par Original take from
*	"http://gcc.gnu.org/onlinedocs/libstdc++/21_strings/stringtok_std_h.txt"
*
*	The Standard C (and C++) function strtok() leaves a lot to be desired in terms of user-friendliness.
*	It's unintuitive, it destroys the character string on which it operates, and it requires you to handle
*	all the memory problems. But it does let the client code decide what to use to break the string into
*	pieces; it allows you to choose the "whitespace," so to speak. 
*
*	A C++ implementation lets us keep the good things and fix those annoyances. The implementation here is
*	more intuitive (you only call it once, not in a loop with varying argument), it does not affect the
*	original string at all, and all the memory allocation is handled for you. 
*
*	It's called tokenize, and it's a template function. It's given in this file in a less-portable for
*	than it could be, to keep this example simple (for example, see the comments on what kind of string it
*	will accept). The author uses a more general (but less readable) form of it for parsing command
*	strings and the like. If you compiled and ran this code using it: 
*	\code
   std::list<std::string>  ls;
   utl::tokenize (std::back_inserter(ls), " this  \t is\t\n  a test  ");
   for (std::list<std::string>::const_iterator it = ls.begin(); it != ls.end(); ++it)
   {
       std::cerr << ':' << (*it) << ":\n";
   }
	\endcode
*	You would see this as output: 
*	<pre>
   :this:
   :is:
   :a:
   :test:
</pre>
*	with all the whitespace removed. The original \a s is still available for use, \a ls will clean up
*	after itself, and \a ls.size() will return how many tokens there were. 
*/
	template <typename outit>
	void tokenize (outit x, const std::tstring &in, const TCHAR * delimiters = _T(" \t\n"))
	{
		const std::tstring::size_type len = in.length();
		std::tstring::size_type i = 0;

		while ( i < len )
		{
			// eat leading whitespace
			i = in.find_first_not_of (delimiters, i);
			if (i == std::tstring::npos)
				return;   // nothing left but white space

			// find the end of the token
			std::tstring::size_type j = in.find_first_of (delimiters, i);

			// push token
			if (j == std::tstring::npos) {
				*x = in.substr(i);
				++x;
				return;
			} else {
				*x = in.substr(i, j-i);
				++x;
			}

			// set up for next loop
			i = j + 1;
		}
	}
} // anonymous namespace

#endif // _AFXDLL

HICON ExtractShellIcon (int nIndex, bool bLargeIcons /*= false*/)
{
	HICON hIcon = NULL;

	// Shell icons can be customized by the registry:
	// HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Icons
	// "<ShellIconIndex>" = "<Filename>,<IconIndex>"
	// E.g.
	// "3" = "c:\MyFolderIcon.ico,1"
	HKEY hkeyShellIcons;
	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons"), 0, KEY_READ, &hkeyShellIcons) == ERROR_SUCCESS)
	{
		TCHAR szBuffer[ MAX_PATH * sizeof TCHAR];
		DWORD dwSize = MAX_PATH * sizeof TCHAR;

		TCHAR szIndex[6] = {0};
		_stprintf (szIndex, _T("%d"), nIndex);
		if (RegQueryValueEx (hkeyShellIcons, szIndex, NULL, NULL, (LPBYTE)szBuffer, &dwSize) == ERROR_SUCCESS)
		{
#ifdef _AFXDLL
			CString strFileName, strIndex;
			VERIFY (AfxExtractSubString (strFileName, szBuffer, 0, _T(',')));
			VERIFY (AfxExtractSubString (strIndex, szBuffer, 1, _T(',')));
			ExtractIconEx (strFileName, atoi(strIndex), bLargeIcons ? &hIcon : NULL, bLargeIcons ? NULL : &hIcon, 1);
#else
			std::vector<std::tstring> ls;
			tokenize(std::back_inserter(ls), szBuffer, _T(","));
			if ((int)ls.size() > 1)
				ExtractIconEx (ls[0].c_str(), atoi(ls[1].c_str()), bLargeIcons ? &hIcon : NULL, bLargeIcons ? NULL : &hIcon, 1);
#endif
		}

		RegCloseKey( hkeyShellIcons );
	}

	// Not customized? Then get the original icon from shell23.dll
	if (!hIcon)
		ExtractIconEx (_T("SHELL32.DLL"), nIndex, bLargeIcons ? &hIcon : NULL, bLargeIcons ? NULL : &hIcon, 1);

	return hIcon;
}
