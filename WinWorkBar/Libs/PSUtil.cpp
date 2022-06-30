//
// PSUtil
// Utilities
// Copyright by Piotr Zagawa. 2002 All rights reserved.
// Wszystkie prawa zastrze¿one
//

#include "PSUtil.h"

//------------------------------------------------------------------------------------------------

std::string Text::CreateNewGUID()
{
	GUID	MyGUID;
  CoCreateGuid(&MyGUID);
	//BYTE* lpBuff = reinterpret_cast<BYTE*>(&MyGUID);
	return Conv::BuffToHexString(reinterpret_cast<BYTE*>(&MyGUID), sizeof(MyGUID));
}

//Date const format to/from text (for save to xml)
std::string Text::DateToXML(const FILETIME& FT)
{
	if ((FT.dwLowDateTime == 0) && (FT.dwHighDateTime == 0))
		return "";
	SYSTEMTIME ST;
	char Out[32] = { '0' };
	if (FileTimeToSystemTime(&FT, &ST) == TRUE)
	{
		wsprintf(Out, "%0.2d-%0.2d-%0.4d %0.2d:%0.2d:%0.2d",
			ST.wDay, ST.wMonth, ST.wYear, ST.wHour, ST.wMinute, ST.wSecond);
	}
	return Out;
}

bool Text::DateFromXML(const std::string& S, FILETIME& FT)
{
	if (S.empty())
	{
		FT.dwLowDateTime = 0;
		FT.dwHighDateTime = 0;
		return true;
	} else {
		if ((int)S.length() == 19)
		{
			SYSTEMTIME ST;
			ZeroMemory(&ST, sizeof(ST));
//DD-MM-YYYY HH:MM:SS
//0123456789012345678
			ST.wDay			= Text::StrToInt(S.substr(0, 2));
			ST.wMonth		= Text::StrToInt(S.substr(3, 2));
			ST.wYear		= Text::StrToInt(S.substr(6, 4));
			ST.wHour		= Text::StrToInt(S.substr(11, 2));
			ST.wMinute	= Text::StrToInt(S.substr(14, 2));
			ST.wSecond	= Text::StrToInt(S.substr(17, 2));
			SystemTimeToFileTime(&ST, &FT);
			return true;
		} else {
			return false;
		}
	}
}

//Text
//------------------------------
std::string Text::DateToString(FILETIME FT, std::string FormatString, bool ShowTime)
{
	std::string	S = "";
	SYSTEMTIME	ST;
	FILETIME		LocalFileTime;
	FileTimeToLocalFileTime(&FT, &LocalFileTime);
	if (FileTimeToSystemTime(&LocalFileTime, &ST) != 0)
	{
		S = FormatString;
		S = Text::Replace(S, "<day>", Text::FormatInt(ST.wDay, "%0.2d"));
		S = Text::Replace(S, "<month>", Text::FormatInt(ST.wMonth, "%0.2d"));
		S = Text::Replace(S, "<year>", Text::FormatInt(ST.wYear, "%0.4d"));
		if (ShowTime)
		{
			S = Text::Replace(S, "<hour>", " " + Text::FormatInt(ST.wHour, "%0.2d"));
			S = Text::Replace(S, "<separator>", ":");
			S = Text::Replace(S, "<minutes>", Text::FormatInt(ST.wMinute, "%0.2d"));
		} else {
			S = Text::Replace(S, "<hour>", "");
			S = Text::Replace(S, "<separator>", "");
			S = Text::Replace(S, "<minutes>", "");
		}
	}
	return S;
}

std::string Text::CurrentLocalDateTime()
{
	SYSTEMTIME ST;
	FILETIME FT;
	GetSystemTime(&ST);
	SystemTimeToFileTime(&ST, &FT);
	return DateTimeToStringLocale(FT);
}

std::string Text::CurrentLocalDateTimeLong()
{
	SYSTEMTIME ST;
	FILETIME FT;
	GetSystemTime(&ST);
	SystemTimeToFileTime(&ST, &FT);
	return DateTimeToStringLocaleLong(FT);
}

std::string Text::CurrentLocalTime(std::string Format)
{
	SYSTEMTIME ST;
	FILETIME FT;
	GetSystemTime(&ST);
	SystemTimeToFileTime(&ST, &FT);
	if (Format.empty())
		return TimeToStringLocale(FT, NULL);
	else
		return TimeToStringLocale(FT, Format.c_str());
}

std::string Text::CurrentLocalDate(std::string Format)
{
	SYSTEMTIME ST;
	FILETIME FT;
	GetSystemTime(&ST);
	SystemTimeToFileTime(&ST, &FT);
	if (Format.empty())
		return DateToStringLocale(FT, NULL);
	else
		return DateToStringLocale(FT, Format.c_str());
}

std::string Text::CurrentLocalDateLong(std::string Format)
{
	SYSTEMTIME ST;
	FILETIME FT;
	GetSystemTime(&ST);
	SystemTimeToFileTime(&ST, &FT);
	if (Format.empty())
		return DateToStringLocaleLong(FT, NULL);
	else
		return DateToStringLocaleLong(FT, Format.c_str());
}

std::string Text::DateTimeToStringLocale(FILETIME FT, bool ChangeToLocalTime)
{
	std::string	S = "";
	SYSTEMTIME	ST;
	FILETIME		LocalFileTime;

	if (ChangeToLocalTime)
		FileTimeToLocalFileTime(&FT, &LocalFileTime);
	else
		LocalFileTime = FT;

	FileTimeToSystemTime(&LocalFileTime, &ST);
	char Buff[128];
	if (GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &ST, NULL, Buff, sizeof(Buff)) == 0)
		return "";
	S = Buff;
	if (GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS | TIME_NOTIMEMARKER, &ST, NULL, Buff, sizeof(Buff)) == 0)
		return "";
	S += " ";
	S += Buff;
	return S;
}

std::string Text::DateTimeToStringLocaleLong(FILETIME FT, bool ChangeToLocalTime)
{
	std::string	S = "";
	SYSTEMTIME	ST;
	FILETIME		LocalFileTime;

	if (ChangeToLocalTime)
		FileTimeToLocalFileTime(&FT, &LocalFileTime);
	else
		LocalFileTime = FT;

	FileTimeToSystemTime(&LocalFileTime, &ST);
	char Buff[128];
	if (GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &ST, NULL, Buff, sizeof(Buff)) == 0)
		return "";
	S = Buff;
	if (GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS | TIME_NOTIMEMARKER, &ST, NULL, Buff, sizeof(Buff)) == 0)
		return "";
	S += " ";
	S += Buff;
	return S;
}

std::string Text::TimeToStringLocale(FILETIME FT, LPCTSTR Format, bool ChangeToLocalTime)
{
	std::string	S = "";
	SYSTEMTIME	ST;
	FILETIME		LocalFileTime;

	if (ChangeToLocalTime)
		FileTimeToLocalFileTime(&FT, &LocalFileTime);
	else
		LocalFileTime = FT;

	FileTimeToSystemTime(&LocalFileTime, &ST);
	char Buff[64];
	if (GetTimeFormat(LOCALE_USER_DEFAULT, 0, &ST, Format, Buff, sizeof(Buff)) == 0)
		return "";
	S = Buff;
	return S;
}

std::string Text::DateToStringLocale(FILETIME FT, LPCTSTR Format, bool ChangeToLocalTime)
{
	std::string	S = "";
	SYSTEMTIME	ST;
	FILETIME		LocalFileTime;

	if (ChangeToLocalTime)
		FileTimeToLocalFileTime(&FT, &LocalFileTime);
	else
		LocalFileTime = FT;

	FileTimeToSystemTime(&LocalFileTime, &ST);
	char Buff[64];
	if (GetDateFormat(LOCALE_USER_DEFAULT, 0, &ST, Format, Buff, sizeof(Buff)) == 0)
		return "";
	S = Buff;
	return S;
}

std::string Text::DateToStringLocaleLong(FILETIME FT, LPCTSTR Format, bool ChangeToLocalTime)
{
	std::string	S = "";
	SYSTEMTIME	ST;
	FILETIME		LocalFileTime;

	if (ChangeToLocalTime)
		FileTimeToLocalFileTime(&FT, &LocalFileTime);
	else
		LocalFileTime = FT;

	FileTimeToSystemTime(&LocalFileTime, &ST);
	char Buff[64];
	if (GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &ST, Format, Buff, sizeof(Buff)) == 0)
		return "";
	S = Buff;
	return S;
}

std::string Text::TimeToStringLocale(SYSTEMTIME SysTime, LPCTSTR Format, bool ChangeToLocalTime)
{
	std::string	S = "";
	SYSTEMTIME	ST;
	FILETIME		LocalFileTime;
	FILETIME		FT;

	SystemTimeToFileTime(&SysTime, &FT);

	if (ChangeToLocalTime)
		FileTimeToLocalFileTime(&FT, &LocalFileTime);
	else
		LocalFileTime = FT;

	FileTimeToSystemTime(&LocalFileTime, &ST);
	char Buff[64];
	if (GetTimeFormat(LOCALE_USER_DEFAULT, 0, &ST, Format, Buff, sizeof(Buff)) == 0)
		return "";
	S = Buff;
	return S;
}

std::string Text::DateToStringLocale(SYSTEMTIME SysTime, LPCTSTR Format, bool ChangeToLocalTime)
{
	std::string	S = "";
	SYSTEMTIME	ST;
	FILETIME		LocalFileTime;
	FILETIME		FT;

	SystemTimeToFileTime(&SysTime, &FT);

	if (ChangeToLocalTime)
		FileTimeToLocalFileTime(&FT, &LocalFileTime);
	else
		LocalFileTime = FT;

	FileTimeToSystemTime(&LocalFileTime, &ST);
	char Buff[64];
	if (GetDateFormat(LOCALE_USER_DEFAULT, 0, &ST, Format, Buff, sizeof(Buff)) == 0)
		return "";
	S = Buff;
	return S;
}

std::string Text::DateTimeToStringLocale(SYSTEMTIME SysTime, LPCTSTR Format, bool ChangeToLocalTime)
{
	std::string S = DateToStringLocale(SysTime, Format, ChangeToLocalTime);
	S += " ";
	S += TimeToStringLocale(SysTime, Format, ChangeToLocalTime);
	return S;
}

std::string Text::ReplaceString(const char* Source, const char* Find, const char* With)
{
	std::string S				= Source;
	std::string Dest		= "";
	std::basic_string<char>::size_type	IDX		= 0;
	std::basic_string<char>::size_type	Size	= S.size();
	while (IDX < Size)
	{
		if (S.find(Find, IDX) == IDX)
		{
			Dest += With;
			IDX+=strlen(Find);
		} else {
			Dest += S.at(IDX);
			IDX++;
		}
	}
	return Dest;
}

std::string Text::Replace(const std::string& Source, const std::string& Find, const std::string& With)
{
	return ReplaceString((char*)Source.c_str(), (char*)Find.c_str(), (char*)With.c_str());
}

std::string Text::IntToStr(__int64 Value)
{
	static char Buff[32];
	_i64toa(Value, Buff, 10);
	return Buff;
}

std::string Text::IntegerToGroups(__int64 Value)
{
	std::string S = IntToStr(Value);
	int Size = (int)S.size();
	int IDX = 1;
	while (IDX <= Size)
	{
		if (IDX % 3 == 0)
			S.insert(Size - IDX, " ");
		IDX++;
	}
	return S;
}

std::string Text::FormatByteSize(__int64 Value)
{
	std::string S = "B";
	double NewValue = (double)Value;
	if (NewValue >= 1024)
	{
		S = "KB";
		NewValue = (NewValue / 1024.0F);
	}
	if (NewValue >= 1024)
	{
		S = "MB";
		NewValue = (NewValue / 1024.0F);
	}
	if (NewValue >= 1024)
	{
		S = "GB";
		NewValue = (NewValue / 1024.0F);
	}
	Value = (__int64)(NewValue * 10.0F);
	if (S == "B")
		S = IntToStr((__int64)NewValue) + " " + S;
	else
		S = FloatToStr((float)NewValue, 2) + " " + S;
	return S;
}

std::string Text::FormatTime(int Seconds)
{
	int M = (Seconds / 60) % 60;
	int H = (Seconds / 3600) % 99;
	Seconds = Seconds % 60;
	static char Buff[32];
	sprintf(Buff, "%0.2d:%0.2d.%0.2d", H, M, Seconds);
	return Buff;
}

std::string Text::FloatToStr(float Value, int Scale)
{
	int Dec, Sign;
	char* Buff = fcvt(Value, Scale, &Dec, &Sign);
	std::string S = Buff;
	if ((Dec >= 0) && (Dec < (int)S.length()))
		if (S.length() > 0)
			S.insert(Dec, ".");
	if (S.length() > 0)
		if (S.at(0) == '.')
			S = "0" + S;
	return S;
}

int	Text::StrToInt(std::string S)
{
	return atoi(S.c_str());
}

std::string Text::FormatInt(int Value, std::string Format)
{
	char Buff[256];
	sprintf(Buff, Format.c_str(), Value);
	return (std::string)Buff;
}

std::string Text::FormatWithTags(std::string Format, std::list<std::string>& Tokens, std::list<std::string>& Values)
{
	std::string S = Format;
	std::list<std::string>::iterator ValuesIDX = Values.begin();
	for (std::list<std::string>::iterator IDX = Tokens.begin(); IDX != Tokens.end(); IDX++)
	{
		if (ValuesIDX == Values.end())
			break;
		S = ReplaceString(S.c_str(), (*IDX).c_str(), (*ValuesIDX++).c_str());
	}
	return S;
}

std::string Text::UpperCase(std::string S)
{
	char* lpText = ::new char[S.length() + 1];
	if (lpText)
	{
		strcpy(lpText, S.c_str());
		_strupr(lpText);
		S = lpText;
		::delete [] lpText;
	}
	return S;
}

std::string Text::LowerCase(std::string S)
{
	char* lpText = ::new char[S.length() + 1];
	if (lpText)
	{
		strcpy(lpText, S.c_str());
		_strlwr(lpText);
		S = lpText;
		::delete [] lpText;
	}
	return S;
}

std::string Text::UpperCaseFirstLetter(std::string S)
{
	if (S.length() > 0)
		S[0] = toupper(S[0]);
	return S;
}

DWORD Text::HexStringToInt(std::string HexValue)
{
	static const std::string HexTable = "0123456789ABCDEF";
	DWORD N = 0;
	HexValue = Text::UpperCase(HexValue);
	if (((int)HexValue.find("0X")) == 0)
		HexValue = HexValue.substr(2);
	int CharIndex = 0;
	while (CharIndex < (int)(HexValue.length()))
	{
		int CharValue = (int)HexTable.find(HexValue[CharIndex]);
		if (CharValue == -1)
			return 0;
		N = N + (CharValue << (4 * (HexValue.length() - CharIndex - 1)));
		CharIndex++;
	}
	return N;
}

//Returns first (true) or second (false) string of condition by Value
std::string Text::ConditionString(std::string S, bool Value, std::string Format)
{
	if (Format.length() == 3)
	{
		int LeftMark = (int)S.find(Format[0]);
		int RightMark = (int)S.find(Format[2]);
		if ((LeftMark != -1) && (RightMark != -1))
		{
			if (LeftMark < RightMark)
			{
				int Delimiter = (int)S.find(Format[1], LeftMark);
				if (Delimiter != -1)
				{
					std::string TrueStr = S.substr(LeftMark + 1, Delimiter - LeftMark - 1);
					std::string FalseStr = S.substr(Delimiter + 1, RightMark - Delimiter - 1);
					S = S.replace(LeftMark, RightMark - LeftMark + 1, (Value)?TrueStr:FalseStr);
				}
			}
		}
	}
	return S;
}

bool	Text::StringsEqual(std::string S1, std::string S2)
{
	return (_stricmp(S1.c_str(), S2.c_str()) == 0);
}

bool	Text::StringFirstEqualToSecondBegin(std::string S1, std::string S2)
{
	S2 = S2.substr(0, S1.length());
	return (_stricmp(S1.c_str(), S2.c_str()) == 0);
}

std::string Text::TrimLeft(const char* Source, char TrimChar)
{
	char*	lpSource = (char*)Source;
	while ((*lpSource) && (*lpSource == TrimChar))
		lpSource++;
	return lpSource;
}

std::string Text::TrimRight(const char* Source, char TrimChar)
{
	std::string S = "";
	char*	lpSource = (char*)Source + lstrlen(Source);
	while (true)
	{
		if (lpSource <= Source)
			break;
		if (!*lpSource)
		{
			if (*(lpSource - 1) != TrimChar)
				break;
			lpSource--;
			continue;
		}
		if (*(lpSource - 1) != TrimChar)
			break;
		lpSource--;
	}
	S = Source;
	return S.substr(0, (int)(lpSource - Source));
}

std::string	Text::Trim(const char* Source)
{
	return TrimRight((char*)TrimLeft(Source).c_str());
}

int Text::ParseStringToList(std::string S, std::list<std::string>& List, std::string Separator)
{
	List.clear();
	int idx = 0;

	S = Trim(S.c_str());
	if (!S.empty())
		if (S.find(Separator) != (S.size() - 1))
			S = S + Separator;

	while (true)
	{
		int pos = (int)S.find(Separator);
		if (pos == -1)
			break;
		std::string Item = Trim(S.substr(0, pos).c_str());
		if (!Item.empty())
		{
			List.push_back(Item);
			idx++;
		}
		S = S.substr(pos + (int)Separator.size());
	}
	return idx;
}

int Text::ParseStringToVector(std::string S, std::vector<std::string>& Vector, std::string Separator)
{
	Vector.clear();
	int idx = 0;

	S = Trim(S.c_str());
	if (!S.empty())
		if (S.find(Separator) != (S.size() - 1))
			S = S + Separator;

	while (true)
	{
		int pos = (int)S.find(Separator);
		if (pos == -1)
			break;
		std::string Item = Trim(S.substr(0, pos).c_str());
		if (!Item.empty())
		{
			Vector.push_back(Item);
			idx++;
		}
		S = S.substr(pos + (int)Separator.size());
	}
	return idx;
}

std::string Text::ParseVectorToString(std::vector<std::string>& Vector, std::string Separator)
{
	std::string S;
	for (int i = 0; i < (int)Vector.size(); i++)
		S+=Vector[i] + Separator;
	size_t Pos = S.rfind(',');
	if (Pos != std::string::npos)
		S = S.erase(Pos);
	return S;
}

std::string Text::GetValueOfParam(std::string S, std::string ParamName, std::string Separator)
{
	std::vector<std::string> Params;
	Text::ParseStringToVector(S, Params, Separator);
	for (int i = 0; i < (int)Params.size(); i++)
	{
		std::string S = Params[i];
		std::string paramName = S.substr(0, S.find("="));
		std::string paramValue = S.substr(S.find("=") + 1);
		if (paramName == ParamName)
			return paramValue;
	}
	return "";
}


//Conversion
//-----------------------------------------------------
std::string	Conv::BuffToHexString(const BYTE* Buff, int BuffLen, const char* HexTable)
{
	std::string S = "";
	for (int IDX = 0; IDX < BuffLen; IDX++)
	{
		S += ByteToHex(Buff[IDX], HexTable);
	}
	return	S;
}

//Buff must be half of S size !
void	Conv::HexStringToBuff(std::string S, BYTE* Buff, const char* HexTable)
{
	if ((S.length() % 2) != 0)
		throw ConvError(3);

	std::string Hex = "";
	int IDX = 0;
	int BuffIDX = 0;
	while (IDX < (int)S.length())
	{
		Hex = S.substr(IDX, 2);
		Buff[BuffIDX++] = HexToByte(Hex, HexTable);
		IDX+=2;
	}
}

std::string	Conv::ByteToHex(BYTE Value, const char* HexTable)
{
	int High	= (Value & 0xF0) >> 4;
	int Low		= (Value & 0x0F);
	std::string S = "XX";
	S[0] = HexTable[High];
	S[1] = HexTable[Low];
	return S;
};

//Gets two chars from string and turns it into byte
BYTE	Conv::HexToByte(std::string Value, const char* HexTable)
{
	byte	B = 0;
	std::string S = "";
	if (Value.length() < 2)
	{
		throw ConvError(1);
	} else {
		byte	High	= HexCharToByte(Value[0], HexTable);
		byte	Low		= HexCharToByte(Value[1], HexTable);
		B = (High << 4) | (Low & 0x0F);
	}
	return B;
};

BYTE	Conv::HexCharToByte(char Value, const char* HexTable)
{
	char* lpDigits = (char*)HexTable;
	while (*lpDigits)
	{
		if (*lpDigits == Value)
			return (byte)(lpDigits - HexTable);
		lpDigits++;
	} 
	throw ConvError(2);
}

//------------------------------
//Number
//------------------------------
Number::SmartInt::SmartInt()
{
	_Target		= 0;
	_Current	= 0;
	_Step			= 1;
}

void	Number::SmartInt::Set(int Value, bool Clear)
{
	_Target	= Value;
	if (Clear)
		_Current = Value;
}

int	Number::SmartInt::Get()
{ 
	return _Current; 
};

void	Number::SmartInt::Step(int Value)
{ 
	_Step = Value; 
};

void	Number::SmartInt::StepIt()
{
	if (_Current != _Target)
	{

		if (_Target > 0)
		{
			int STEP = 1;
			int iRange = abs(_Current - _Target);

			if (iRange > 32)
				STEP = 2;
			if (iRange > 64)
				STEP = 4;
			if (iRange > 128)
				STEP = 8;
			if (iRange > 256)
				STEP = 16;
			if (iRange > 512)
				STEP = 32;

			_Step = STEP;
		}


		if (_Current > _Target)
			_Current-=_Step;
		if (_Current < _Target)
			_Current+=_Step;
	}
}

//------------------------------
//Returns current module version
//short = true -> short info (x.x)
//------------------------------
std::string Misc::GetAppVersion(bool Short, int* NumericVersion)
{
	std::string	S = "";
	char	FileName[MAX_PATH * 2];
	GetModuleFileName(NULL, (LPTSTR)&FileName, sizeof(FileName));

	DWORD lpdwHandle = 0;
	DWORD VerSize	= GetFileVersionInfoSize((LPTSTR)&FileName, &lpdwHandle);
	if (VerSize)
	{
		BYTE*	lpBuffer = ::new BYTE[VerSize];
		if (GetFileVersionInfo((LPTSTR)&FileName, NULL, VerSize, lpBuffer))
		{
			char	SubBlock[] = "\\";			
			LPVOID	lpValueBuffer = NULL;
			UINT		uiValueBufferLen	= 0;
			if (VerQueryValue(lpBuffer, (LPTSTR)&SubBlock, &lpValueBuffer, &uiValueBufferLen))
			{
				VS_FIXEDFILEINFO*	FI = (VS_FIXEDFILEINFO*)lpValueBuffer;
				int	I1 = (FI->dwProductVersionMS >> 16) & 0x0000FFFF;
				int I2 = (FI->dwProductVersionMS & 0x0000FFFF);
				int I3 = (FI->dwProductVersionLS >> 16) & 0x0000FFFF;
				int I4 = (FI->dwProductVersionLS & 0x0000FFFF);

				if (NumericVersion)
					*NumericVersion = (I1 * 1000) + (I2 * 100) + (I3 * 10) + (I4);

				char	VersionString[64];
				if (Short)
					sprintf(VersionString, "%d.%d", I1, I2);
				else
					sprintf(VersionString, "%d.%d.%d.%d", I1, I2, I3, I4);
				S = VersionString;
			}
		}
		::delete [] lpBuffer;
	}
	return S;
}

int	Misc::DisplayContextMenu(HINSTANCE hInstance, UINT Flags, HWND hwnd, POINT pt, INT MenuResource, OnMenuCreate Fn_OnMenuCreate)
{ 
	HMENU hmenu;            // top-level menu 
	HMENU hmenuTrackPopup;  // shortcut menu 

	//Load the menu resource. 
	if ((hmenu = LoadMenu(hInstance, MAKEINTRESOURCE(MenuResource))) == NULL)
		return -1;

	//Get handle to next menu item
	hmenuTrackPopup = GetSubMenu(hmenu, 0); 

	SetForegroundWindow(hwnd);

	if (Fn_OnMenuCreate)
		Fn_OnMenuCreate(hmenuTrackPopup);

	int Out = TrackPopupMenu(hmenuTrackPopup, Flags | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
	PostMessage(hwnd, WM_NULL, 0, 0); 

	//Destroy the menu.
	DestroyMenu(hmenu);
	return Out;
}

DWORD	Misc::FindMemory(const BYTE* Buffer, DWORD BufferSize, const BYTE* Sequence, DWORD SequenceSize)
{
	if ((!BufferSize) || (!SequenceSize))
		return -1;
	if (BufferSize < SequenceSize)
		return -1;

	BYTE* lpSource = const_cast<BYTE*>(Buffer);
	DWORD	CurrentPos = -1;
	while (true)
	{
		CurrentPos = DWORD(lpSource - Buffer);
		if (CurrentPos > (BufferSize - SequenceSize))
			return -1;
		if (memcmp(lpSource, Sequence, SequenceSize) == 0)
			return CurrentPos;
		lpSource++;
	}
	return -1;
}

int	Misc::PercentOf(__int64 BaseValue, __int64 Progress)
{
	return static_cast<int>((BaseValue)?(Progress * 100) / BaseValue:0);
}

bool	Misc::SysIsBetterThanWin95()
{
	OSVERSIONINFO VI;
	VI.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VI);
	bool	Result = false;
	switch	(VI.dwPlatformId)
	{
		case	VER_PLATFORM_WIN32_WINDOWS:
			{
				Result = ((VI.dwMajorVersion >= 4) && (VI.dwMinorVersion >= 10));	//better or equal Win98
				break;
			}
		case	VER_PLATFORM_WIN32_NT:
			{
				Result = ((VI.dwMajorVersion >= 4) && (VI.dwMinorVersion >= 0));	//better or equal Win4.0
				break;
			}
	}
	return Result;
}

void	Misc::DrawBitmap(HDC hDC, HBITMAP hBitmap, int xStart, int yStart)
{
	HDC hdcMem;			// device context for memory
  HGDIOBJ hbmOld; // old bitmap area we're over-writing
  BITMAP bm;			// bitmap we're using

	hdcMem = CreateCompatibleDC(hDC);
  hbmOld = ::SelectObject(hdcMem, hBitmap);
  ::GetObject(hBitmap, sizeof(bm), &bm);
  BitBlt(hDC, xStart, yStart, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0,SRCCOPY);
	SelectObject(hdcMem, hbmOld);
  DeleteDC(hdcMem);
}

void	Misc::DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, int xStart, int yStart, COLORREF cTransparentColor)
{
  BITMAP     bm;
  COLORREF   cColor;
  HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
  HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
  HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
  POINT      ptSize;   hdcTemp = CreateCompatibleDC(hdc);
  SelectObject(hdcTemp, hBitmap);   // Select the bitmap   
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
  ptSize.x = bm.bmWidth;            // Get width of bitmap
  ptSize.y = bm.bmHeight;           // Get height of bitmap
  DPtoLP(hdcTemp, &ptSize, 1);      // Convert from device                                     // to logical points   // Create some DCs to hold temporary data.
  hdcBack   = CreateCompatibleDC(hdc);
  hdcObject = CreateCompatibleDC(hdc);
  hdcMem    = CreateCompatibleDC(hdc);
  hdcSave   = CreateCompatibleDC(hdc);   // Create a bitmap for each DC. DCs are required for a number of
  // GDI functions.   // Monochrome DC
  bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);   // Monochrome DC
  bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);   
	bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
  bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);   // Each DC must select a bitmap object to store pixel data.
  bmBackOld   = (HBITMAP)SelectObject(hdcBack, bmAndBack);
  bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
  bmMemOld    = (HBITMAP)SelectObject(hdcMem, bmAndMem);
  bmSaveOld   = (HBITMAP)SelectObject(hdcSave, bmSave);   // Set proper mapping mode.
  SetMapMode(hdcTemp, GetMapMode(hdc));   // Save the bitmap sent here, because it will be overwritten.
  BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);   // Set the background color of the source DC to the color.
  // contained in the parts of the bitmap that should be transparent
  cColor = SetBkColor(hdcTemp, cTransparentColor);   // Create the object mask for the bitmap by performing a BitBlt
  // from the source bitmap to a monochrome bitmap.
  BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);   // Set the background color of the source DC back to the original
  // color.
  SetBkColor(hdcTemp, cColor);   // Create the inverse of the object mask.
  BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);   // Copy the background of the main DC to the destination.
  BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart, SRCCOPY);   // Mask out the places where the bitmap will be placed.
  BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);   // Mask out the transparent colored pixels on the bitmap.
  BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);   // XOR the bitmap with the background on the destination DC.
  BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);   // Copy the destination to the screen.
  BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);   // Place the original bitmap back into the bitmap sent here.
  BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);   // Delete the memory bitmaps.
  DeleteObject(SelectObject(hdcBack, bmBackOld));
  DeleteObject(SelectObject(hdcObject, bmObjectOld));
  DeleteObject(SelectObject(hdcMem, bmMemOld));
  DeleteObject(SelectObject(hdcSave, bmSaveOld));   // Delete the memory DCs.
  DeleteDC(hdcMem);
  DeleteDC(hdcBack);
  DeleteDC(hdcObject);
  DeleteDC(hdcSave);
  DeleteDC(hdcTemp);
} 

bool Misc::IsBitSet(const DWORD& wData, const int& BitNr)
{
	DWORD dwMask = (DWORD)1 << BitNr;
	return ((wData & dwMask) == dwMask);
}

void Misc::SetBit(DWORD& wData, const int& BitNr)
{
	DWORD dwMask = (DWORD)1 << BitNr;
	wData |= dwMask;
}

void Misc::SetBit(WORD& wData, const int& BitNr)
{
	WORD dwMask = (WORD)1 << BitNr;
	wData |= dwMask;
}

int Misc::GetNumberDaysOfMonth(SYSTEMTIME ST)
{
	int days[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if (ST.wMonth == 2)
	{
		if (ST.wYear % 4)       // Not leap year
			return 28;
		if (ST.wYear % 100)     // It is leap year
			return 29;
		if (ST.wYear % 400)     // Not leap year
			return 28;
		return 29;          // It is leap year
	} else {
		return days[ST.wMonth];
	}
}

