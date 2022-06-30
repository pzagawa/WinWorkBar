//
// PSUtil
// Utilities
// Copyright by Piotr Zagawa. 2002 All rights reserved.
// Wszystkie prawa zastrze¿one
//
// ver.1.2:	16-04-2003
// ver.1.1:	03-04-2003
// ver.1.0:	01-10-2002
//

#pragma once

#include	<string>
#include	<list>
#include	<vector>
#include  <time.h>
#include	<Windows.h>

//------------------------------------------------------------------------------------------------

//New line const
const char  _NL[] = { 13, 10, 0 }; //"\n"

//------------------------------------------------------------------------------------------------

namespace Text
{
	const std::string DefaultDateFormatString = "<day>-<month>-<year><hour><separator><minutes>";

	std::string CreateNewGUID();

	std::string DateToXML(const FILETIME& FT);
	bool DateFromXML(const std::string& S, FILETIME& FT);

	std::string DateToString(FILETIME FT, std::string FormatString = DefaultDateFormatString, bool ShowTime = true);

	std::string DateTimeToStringLocale(FILETIME FT, bool ChangeToLocalTime = true);
	std::string DateTimeToStringLocaleLong(FILETIME FT, bool ChangeToLocalTime = true);

	std::string TimeToStringLocale(FILETIME FT, LPCTSTR Format = NULL, bool ChangeToLocalTime = true);
	std::string DateToStringLocale(FILETIME FT, LPCTSTR Format = NULL, bool ChangeToLocalTime = true);
	std::string TimeToStringLocale(SYSTEMTIME SysTime, LPCTSTR Format = NULL, bool ChangeToLocalTime = true);
	std::string DateToStringLocale(SYSTEMTIME SysTime, LPCTSTR Format = NULL, bool ChangeToLocalTime = true);
	std::string DateToStringLocaleLong(FILETIME FT, LPCTSTR Format, bool ChangeToLocalTime = true);
	std::string DateTimeToStringLocale(SYSTEMTIME SysTime, LPCTSTR Format = NULL, bool ChangeToLocalTime = true);

	std::string CurrentLocalDateTime();
	std::string CurrentLocalDateTimeLong();
	std::string CurrentLocalTime(std::string Format);
	std::string CurrentLocalDate(std::string Format);
	std::string CurrentLocalDateLong(std::string Format);

  std::string ReplaceString(const char* Source, const char* Find, const char* With);
	std::string Replace(const std::string& Source, const std::string& Find, const std::string& With);
	std::string FormatWithTags(std::string Format, std::list<std::string>& Tokens, std::list<std::string>& Values);
	std::string IntToStr(__int64 Value);
	std::string FloatToStr(float Value, int Scale = 1);
	std::string FormatInt(int Value, std::string Format);
	std::string UpperCase(std::string S);
	std::string LowerCase(std::string S);
	std::string UpperCaseFirstLetter(std::string S);

	int	StrToInt(std::string S);
	DWORD HexStringToInt(std::string HexValue);
	//Returns first (true) or second (false) string of condition by Value
	std::string ConditionString(std::string S, bool Value, std::string Format = "<:>");
	std::string IntegerToGroups(__int64 Value);
	std::string FormatByteSize(__int64 Value);
	std::string FormatTime(int Seconds);
	bool	StringsEqual(std::string S1, std::string S2);
	bool	StringFirstEqualToSecondBegin(std::string S1, std::string S2);

	std::string TrimLeft(const char* Source, char TrimChar = ' ');
	std::string TrimRight(const char* Source, char TrimChar = ' ');
	std::string	Trim(const char* Source);

	int ParseStringToList(std::string S, std::list<std::string>& List, std::string Separator = ",");
	int ParseStringToVector(std::string S, std::vector<std::string>& Vector, std::string Separator = ",");

	std::string ParseVectorToString(std::vector<std::string>& Vector, std::string Separator = ", ");

	std::string GetValueOfParam(std::string S, std::string ParamName, std::string Separator = ";");
};

namespace	Exception
{
	class	Main
	{
	protected:
		int		_ErrorNumber;
		char	_Buff[1024];
	public:
		Main(int ErrorNumber = 0) : _ErrorNumber(ErrorNumber) { itoa(_ErrorNumber, _Buff, 10); };
		Main(char* DescString) { strcpy(_Buff, DescString); };
		int		ErrorAsInt() { return _ErrorNumber; };
		char*	ErrorAsString() { return _Buff; };
	};
};

namespace Conv
{
	//Default Hex to string/string to hex conversion table
	const	char	HexDigitsTable[]	= "0123456789ABCDEF";

	class	ConvError : public Exception::Main
	{
	public:
		ConvError(int ErrorNumber = 0) : Exception::Main(ErrorNumber) {};
		ConvError(char* DescString) { strcpy(_Buff, DescString); };
	};

	//Throws ConvError:
	// 1 - HexToByte error
	// 2 - HexCharToByte error
	// 3 - HexStringToBuff error

	std::string	BuffToHexString(const BYTE* Buff, int BuffLen, const char* HexTable = HexDigitsTable);
	void	HexStringToBuff(std::string S, BYTE* Buff, const char* HexTable = HexDigitsTable);

	std::string	ByteToHex(BYTE Value, const char* HexTable = HexDigitsTable);
	BYTE	HexToByte(std::string Value, const char* HexTable = HexDigitsTable);
	BYTE	HexCharToByte(char Value, const char* HexTable = HexDigitsTable);
};

namespace	Number
{
	class	SmartInt
	{
		int	_Target;
		int	_Current;
		int	_Step;
	public:
		SmartInt();
		void	Set(int Value, bool Clear = false);
		int	Get();
		void	Step(int Value);
		void	StepIt();
	};
};

namespace Misc
{
	typedef	void (*OnMenuCreate)(HMENU hmenu);

	int	DisplayContextMenu(HINSTANCE hInstance, UINT Flags, HWND hwnd, POINT pt, INT MenuResource, OnMenuCreate Fn_OnMenuCreate);
	DWORD	FindMemory(const BYTE* Buffer, DWORD BufferSize, const BYTE* Sequence, DWORD SequenceSize);
	std::string	GetAppVersion(bool Short, int* NumericVersion = NULL);
	int	PercentOf(__int64 BaseValue, __int64 Progress);
	bool	SysIsBetterThanWin95();
	void	DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, int xStart, int yStart, COLORREF cTransparentColor);
	void	DrawBitmap(HDC hdc, HBITMAP hBitmap, int xStart, int yStart);

	bool IsBitSet(const DWORD& wData, const int& BitNr);
	void SetBit(DWORD& wData, const int& BitNr);
	void SetBit(WORD& wData, const int& BitNr);

	int GetNumberDaysOfMonth(SYSTEMTIME ST);

}

