//
// PSLanguage
// Copyright by Piotr Zagawa. 2002 All rights reserved.
// Wszystkie prawa zastrze¿one
//
// ver.1.0:	06-10-2002
//

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include "PSFile.h"

//------------------------------------------------------------------------------------------------

//Multilanguage support
namespace	Language
{ 
	//Language identifiers
	const	LANGID	PL_LangID = 0x0415;	//Polish
	const	LANGID	EN_LangID = 0x0809;	//English (United Kingdom)

	const	std::string ConfigDescription = "Your system language is <lang>. Language file 'Lang\\<file>.txt' <exists:not exists>.";

	//String item structure
  struct  TextItem
  {
    const char* LangList[2];
  };

	//String item structure in language file
  struct  FileTextItem
  {
		char	LangItem[1024];
  };

  //LIST for test ONLY !!!
  const TextItem  TestItems[] =
  {
    "Tekst 0 PL",
    "Text 0 EN",
    "Tekst 1 PL",
    "Text 1 EN",
		NULL, NULL,	//ending item
  };

	typedef std::vector<std::string> ListLangID;


  //----------------------------
  //Lang CLASS
  //----------------------------
  class Lang
  {
		bool					_UseLangFiles;				//false - use only const lang table
		int						_ItemsCount;					//count of items
		int						_TextItemsCount;			//save of text items count
    TextItem*			_TextItems;						//pointer to const table
    FileTextItem*	_FileTextItems;				//file buffer
		LANGID				_CurrentLanguage;			//current language LANGID
		std::string		_LangFilesDirectory;	//home directory for Lang\files directory. Default: application path.

		File::RecursiveDir	_RecDir;				//for get list of available language files in Lang directory

		void	FreeMemoryForTextFile();
		void	ReserveMemoryForTextFile(int ItemsCount);
    void	SetItems(const TextItem* Items);
		int		ItemsCount();
		bool	LoadFromFile(LANGID LocaleID);
		bool	LangFileExists(LANGID LocaleID);
		std::string LangFileName(LANGID LocaleID);
  public:
		Lang(const TextItem* Items, bool UseLangFiles, std::string LangFilesDirectory = "");
    ~Lang();

		bool	SetLanguage(std::string LangShortName);	//eg. PLK, ENG, ENU, etc... if empty, then system lang
		bool	SetLanguage(LANGID LocaleID);

    const char* Text(int ID);

		LANGID	CurrentLangID() { return _CurrentLanguage; };
		LANGID	SystemLangID();

		std::string LanguageInfo();
		std::string LanguageInfoForConfig(std::string Desc = ConfigDescription);

		void	ShowLanguageInfo();

		void	SaveToFile(DWORD dwCreateType = CREATE_ALWAYS);
		void	SaveHelpFile(bool ShowLangList = false, DWORD dwCreateType = CREATE_ALWAYS);

		bool	GetListOfAvailableLangFiles();
  };

	const ListLangID& GetSystemLangIDList();
	const ListLangID& GetLangFilesList();

	std::string	GetLanguageEnglishName(int LocaleID);
	std::string	GetLanguageLocalizedName(int LocaleID);
	std::string	GetLanguageShortName(int LocaleID);	//eg: eng, enu, plk, ita...

	std::string	GetLanguageDayName(int LocaleID, int Day);

	std::string	GetLocaleInfoString(int LocaleID, LCTYPE Type);

	LANGID	ShortNameToLangID(std::string LangShortName);

	//Callback and events
	BOOL	CALLBACK EnumLocalesProc(LPTSTR lpLocaleString);
	void	EnumLangFiles(LPVOID UserData, const std::string Dir, const WIN32_FIND_DATA& FileData, bool& Cancel, bool& Exclude);

	void SetLangSubFolder(std::string sName = "Lang");

};
