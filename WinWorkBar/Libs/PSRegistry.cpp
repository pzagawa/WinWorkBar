//
// Registry preferences
// 09-09-2002 © Piotr Zagawa "Vetch"
//

#include "PSRegistry.h"

//------------------------------------------------------------------------------------------------

Registry::Reg::Reg(HKEY hKey, std::string SubKey)
{
	MainKey = hKey;
	_SubKey = SubKey;
	_SubKeySave = SubKey;
	OutKey = NULL;
	ZeroMemory(&Data, DataBuffSize);
}

Registry::Reg::~Reg()
{
	if (OutKey)
	{
		RegFlushKey(OutKey);
		RegCloseKey(OutKey);
	}
}

void	Registry::Reg::SetAppName(std::string AppName)
{
	_AppName = AppName + "\\";
	_SubKeySave += _AppName;
}

void	Registry::Reg::SetKey(HKEY hKey, std::string SubKey)
{
	CloseKey();
	MainKey = hKey;
	_SubKey = SubKey;
	_SubKeySave = SubKey;
}

//Zwraca true, jeœli uda³o siê otworzyæ podany klucz
bool	Registry::Reg::OpenKey()
{
	if (MainKey)
	{
		if (RegOpenKeyEx(MainKey, (char*)_SubKey.c_str(), 0, KEY_ALL_ACCESS, &OutKey) == ERROR_SUCCESS)
		{	
			return true;
		} else {
			CloseKey();
			return false;
		}
	} else {
		return false;
	}
}

void	Registry::Reg::CloseKey()
{
	if (OutKey)
	{
		RegCloseKey(OutKey);
	}
	OutKey = NULL;
}

//Sprawdza czy istnieje podany klucz
int	Registry::Reg::KeyExists()
{
	HKEY TmpKey;
	if (MainKey)
	{
		if (RegOpenKeyEx(MainKey, (char*)_SubKey.c_str(), 0, KEY_QUERY_VALUE, &TmpKey) == ERROR_SUCCESS)
		{	
			RegCloseKey(TmpKey);
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

//Tworzy podany klucz
int	Registry::Reg::CreateKey()
{
	DWORD	dwDisposition;
	HKEY TmpKey;
	if (MainKey)
	{
		if (RegCreateKeyEx(MainKey, (char*)_SubKey.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &TmpKey, &dwDisposition) == ERROR_SUCCESS)
		{
			RegCloseKey(TmpKey);
			switch (dwDisposition)
			{
				case	REG_CREATED_NEW_KEY:			return 2;	//utworzono nowy klucz
				case	REG_OPENED_EXISTING_KEY:	return 3;	//klucz ju¿ istnieje
				default:	
					return 1;
			};
		} else {
			return 0;	
		}
	} else {
		return 0;
	}
}

//Usuwa podany klucz
bool	Registry::Reg::DeleteKey()
{
	if (MainKey)
	{
		CloseKey();
		if (RegDeleteKey(MainKey, (char*)_SubKey.c_str()) == ERROR_SUCCESS)
		{
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool	Registry::Reg::DeleteKey(std::string Key)
{
	if (MainKey)
	{
		CloseKey();
		_SubKey = _SubKeySave + Key;
		if (RegDeleteKey(MainKey, (char*)_SubKey.c_str()) == ERROR_SUCCESS)
		{
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool Registry::Reg::DeleteSubKeys(std::string Key)
{
	std::vector<std::string> V;
	GetListOfKeys(Key, V);
	if (V.empty())
	{
		DeleteKey(Key);
	} else {
		for (int idx = 0; idx < (int)V.size(); idx++)
		{
			std::string SubKey = Key + "\\" + V[idx];
			DeleteSubKeys(SubKey);
		}
		DeleteKey(Key);
	}
	return true;
}

bool Registry::Reg::GetListOfKeys(std::string Key, std::vector<std::string>& L)
{
	bool Result = false;
	CloseKey();
	_SubKey = _SubKeySave + Key;
	if (OpenKey())
	{
		L.clear();
		FILETIME ftLastWriteTime;
		DWORD	SizeOfBuff = 300;
		TCHAR Buff[300] = {'0'};
		int idx = 0;
		while (RegEnumKeyEx(OutKey, idx++, Buff, &SizeOfBuff, NULL, NULL, NULL, &ftLastWriteTime) != ERROR_NO_MORE_ITEMS)
		{
			SizeOfBuff = 300;
			L.push_back(Buff);
		}
		Result = true;
		CloseKey();
	}
	return Result;
}

bool Registry::Reg::GetListOfValues(std::string Key, std::vector<std::string>& L)
{
	bool Result = false;
	CloseKey();
	_SubKey = _SubKeySave + Key;
	if (OpenKey())
	{
		L.clear();
		DWORD	SizeOfBuff = 300;
		TCHAR Buff[300] = {'0'};
		int idx = 0;
		while (RegEnumValue(OutKey, idx++, Buff, &SizeOfBuff, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS)
		{
			SizeOfBuff = 300;
			L.push_back(Buff);
		}
		Result = true;
		CloseKey();
	}
	return Result;
}

//Pobiera wartoœæ z rejestru do bufora
bool	Registry::Reg::ReadValue_ToDataBuffer(std::string Name)
{
	dwBuffSize = DataBuffSize;
	ZeroMemory(&Data, DataBuffSize);
	if (MainKey)
	{
		if (RegQueryValueEx(OutKey, (char*)Name.c_str(), NULL, NULL, (BYTE*)&Data, &dwBuffSize) == ERROR_SUCCESS)
			return true;
	}
	return false;
}

bool	Registry::Reg::ValueExists(std::string Name)
{
	return ReadValue_ToDataBuffer(Name);
}

//Zwraca wartoœæ o podanej nazwie
std::string	Registry::Reg::ReadValue_AsString(std::string Name, std::string Default)
{
	std::string S = Default;
	if (MainKey)
	{
		if (ReadValue_ToDataBuffer(Name))
			S = (char*)Data;
		if (S == "")
			S = Default;
	}
	return S;
}

bool	Registry::Reg::ReadValue_AsBool(std::string Name, bool Default)
{
	bool	B = Default;
	std::string	S;
	if (MainKey)
	{
		if (ReadValue_ToDataBuffer(Name))
		{
			S = (char*)Data;
			B = (S == "1")?true:false;
		}
	}
	return B;
}

int	Registry::Reg::ReadValue_AsInteger(std::string Name, int Default)
{
	int	I = Default;
	std::string	S;
	if (MainKey)
	{
		if (ReadValue_ToDataBuffer(Name))
		{
			S = (char*)Data;
			try
			{
				I = atoi((char*)S.c_str());
			} catch (...) {
				I = Default;
			}
		}
	}
	return I;
}

//Zapisuje zawartoœæ bufora pod podan¹ nazw¹
bool	Registry::Reg::WriteValue_FromDataBuffer(std::string Name)
{
	if (MainKey)
	{
		if (RegSetValueEx(OutKey, (char*)Name.c_str(), NULL, REG_SZ, (BYTE*)&Data, (DWORD)strlen((char*)&Data)) == ERROR_SUCCESS)
		{
			return true;
		}
	}
	return false;
}

//Zapisuje wartoœæ pod podan¹ nazw¹
bool	Registry::Reg::WriteValue(std::string Name, char* Value)
{
	if (MainKey)
	{
		strcpy((char*)&Data, Value);
		return WriteValue_FromDataBuffer(Name);
	}
	return false;
}

bool	Registry::Reg::WriteValue(std::string Name, std::string Value)
{
	return WriteValue(Name, (char*)Value.c_str());
}

bool	Registry::Reg::WriteValue(std::string Name, int	Value)
{
	if (MainKey)
	{
		strcpy((char*)&Data, itoa(Value, (char*)&Data, 10));
		return WriteValue_FromDataBuffer(Name);
	}
	return false;
}

bool	Registry::Reg::WriteValue(std::string Name, bool	Value)
{
	if (MainKey)
	{
		strcpy((char*)&Data, ((Value)?"1":"0"));
		return WriteValue_FromDataBuffer(Name);
	}
	return false;
}

//Direct style access
bool Registry::Reg::Write(std::string Key, std::string Name, std::string Value)
{
	bool Result = false;
	CloseKey();
	_SubKey = _SubKeySave + Key;
	if (!KeyExists())
		CreateKey();
	if (OpenKey())
	{
		Result = WriteValue(Name, Value);
		CloseKey();
	}
	return Result;
}

bool Registry::Reg::Write(std::string Key, std::string Name, int Value)
{
	bool Result = false;
	CloseKey();
	_SubKey = _SubKeySave + Key;
	if (!KeyExists())
		CreateKey();
	if (OpenKey())
	{
		Result = WriteValue(Name, Value);
		CloseKey();
	}
	return Result;
}

bool Registry::Reg::Write(std::string Key, std::string Name, bool Value)
{
	bool Result = false;
	CloseKey();
	_SubKey = _SubKeySave + Key;
	if (!KeyExists())
		CreateKey();
	if (OpenKey())
	{
		Result = WriteValue(Name, Value);
		CloseKey();
	}
	return Result;
}

std::string Registry::Reg::ReadAsString(std::string Key, std::string Name, std::string FailValue)
{
	std::string Result = FailValue;
	CloseKey();
	_SubKey = _SubKeySave + Key;
	if (KeyExists() && OpenKey())
	{
		Result = ReadValue_AsString(Name, FailValue);
		CloseKey();
	}
	return Result;
}

int Registry::Reg::ReadAsInt(std::string Key, std::string Name, int FailValue)
{
	int Result = FailValue;
	CloseKey();
	_SubKey = _SubKeySave + Key;
	if (KeyExists() && OpenKey())
	{
		Result = ReadValue_AsInteger(Name, FailValue);
		CloseKey();
	}
	return Result;
}

bool	Registry::Reg::ReadAsBool(std::string Key, std::string Name, bool FailValue)
{
	bool Result = FailValue;
	CloseKey();
	_SubKey = _SubKeySave + Key;
	if (KeyExists() && OpenKey())
	{
		Result = ReadValue_AsBool(Name, FailValue);
		CloseKey();
	}
	return Result;
}
