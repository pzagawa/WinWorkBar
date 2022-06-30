//
// PSRegistry
// Registry access i/o class
// Copyright by Piotr Zagawa. 2002 All rights reserved.
// Wszystkie prawa zastrze¿one
//
// ver.1.0:	09-09-2002
// ver.1.1:	11-04-2003
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <string>
#include <vector>
#include <Windows.h>

//------------------------------------------------------------------------------------------------

namespace	Registry
{
	const int DataBuffSize = 2048;

	const	std::string	MySoft	= "SOFTWARE\\PeterSoft\\";

	class	Reg
	{
		std::string	_SubKey;			//ci¹g podklucza
		std::string	_SubKeySave;	//ci¹g podklucza niezmieniany
		std::string _AppName;
		HKEY				MainKey;			//zasób klucza g³ównego, np. HKEY_LOCAL_MACHINE
		HKEY				OutKey;				//zasób klucza utworzony po otwarciu klucza w rejestrze

		BYTE	Data[DataBuffSize];	//bufor na dane dla wartoœci pobieranych z podklucza
		DWORD	dwBuffSize;					//rozmiar bufora na dane

		bool	ReadValue_ToDataBuffer(std::string Name);			//wspólna procedura odczytu wartoœci do bufora
		bool	WriteValue_FromDataBuffer(std::string Name);	//... zapisu wartoœci z bufora do rejestru
	public:
		Reg(HKEY hKey = HKEY_CURRENT_USER, std::string SubKey = MySoft);
		~Reg();

		void	SetAppName(std::string AppName);
		void	SetKey(HKEY hKey = HKEY_CURRENT_USER, std::string SubKey = MySoft);	//ustawienie klucza operacyjnego

		bool	OpenKey();
		void	CloseKey();
		int		KeyExists();
		int		CreateKey();
		bool	ValueExists(std::string Name);

		bool	DeleteKey();
		bool	DeleteKey(std::string Key);
		bool	DeleteSubKeys(std::string Key);

		bool	GetListOfKeys(std::string Key, std::vector<std::string>& L);
		bool	GetListOfValues(std::string Key, std::vector<std::string>& L);

		//Step by step (set, open, create key) access
		std::string	ReadValue_AsString(std::string ValueName, std::string Default = "");
		int					ReadValue_AsInteger(std::string Name, int Default = -1);
		bool				ReadValue_AsBool(std::string Name, bool Default = false);

		bool	WriteValue(std::string Name, std::string Value);
		bool	WriteValue(std::string Name, char* Value);
		bool	WriteValue(std::string Name, int	Value);
		bool	WriteValue(std::string Name, bool	Value);

		//Direct style access - one step
		bool Write(std::string Key, std::string Name, std::string Value);
		bool Write(std::string Key, std::string Name, int Value);
		bool Write(std::string Key, std::string Name, bool Value);

		std::string ReadAsString(std::string Key, std::string Name, std::string FailValue = "");
		int					ReadAsInt(std::string Key, std::string Name, int FailValue = 0);
		bool				ReadAsBool(std::string Key, std::string Name, bool FailValue = false);
	};
}
