//
// PSLanguage
// Copyright by Piotr Zagawa. 2002 All rights reserved.
// Wszystkie prawa zastrze¿one
//
// ver.1.0:	06-10-2002
//

#include "PSLanguage.h"
#include "PSUtil.h"

//------------------------------------------------------------------------------------------------

namespace	Language
{ 
	std::string LangPath = "Lang\\";	//default path to find language files

	//Global list of system supported LangID's
	ListLangID	SystemLangIDList;
	//Global list of available lang files
	ListLangID	AvailableShortLangNamesList;

	const ListLangID& GetSystemLangIDList()
	{ 
		return SystemLangIDList; 
	};

	const ListLangID& GetLangFilesList()
	{ 
		return AvailableShortLangNamesList; 
	};

	//Class ----------------------------------------------------
	Lang::Lang(const TextItem* Items, bool UseLangFiles, std::string LangFilesDirectory)
	{
		_LangFilesDirectory = LangFilesDirectory;
		if (_LangFilesDirectory.empty())
		{
			TCHAR Buff[MAX_PATH * 2];
			if (GetModuleFileName(NULL, Buff, sizeof(Buff)/sizeof(TCHAR)))
			{
				_LangFilesDirectory = File::ExtractPath(Buff);
				_LangFilesDirectory = File::IncludeBackSlash(_LangFilesDirectory);
			}
		}

		_TextItems = NULL;
		_FileTextItems = NULL;
		_RecDir.fnEvent = EnumLangFiles;
		SetItems(Items);

		SystemLangIDList.clear();
		AvailableShortLangNamesList.clear();
		EnumSystemLocales(EnumLocalesProc, LCID_SUPPORTED);

		//Force set to internal english
		_UseLangFiles = false;
		SetLanguage(EN_LangID);
		_UseLangFiles = UseLangFiles;
	}

	Lang::~Lang()
	{
		FreeMemoryForTextFile();
	}

	void	Lang::FreeMemoryForTextFile()
	{
		if (_FileTextItems)
		{
			delete [] _FileTextItems;
			_FileTextItems = NULL;
		}
	}

	void	Lang::ReserveMemoryForTextFile(int ItemsCount)
	{
		_ItemsCount = ItemsCount;
		FreeMemoryForTextFile();
		_FileTextItems = new FileTextItem[_ItemsCount];
		ZeroMemory(_FileTextItems, sizeof(FileTextItem) * _ItemsCount);
	}

	void  Lang::SetItems(const TextItem* Items)
	{
		_TextItems  = (TextItem*)Items;
		const TextItem* _lpItems = _TextItems;
		_ItemsCount = 0;	//Items counter
		while (_lpItems->LangList[0])
		{
			_ItemsCount++;
			_lpItems++;
		}
		_TextItemsCount = _ItemsCount;
	}

	bool	Lang::SetLanguage(LANGID LocaleID)
	{
		bool Result = false;
		bool Loaded = false;
		FreeMemoryForTextFile();
		if (_UseLangFiles)
		{
			if (LangFileExists(LocaleID))
			{
				if (LoadFromFile(LocaleID))
				{
					_CurrentLanguage = LocaleID;
					Loaded = true;
					Result = true;
				}
			}
		}
		if (!Loaded)
		{
			std::string S = GetLanguageShortName(LocaleID);
			if (Text::StringsEqual(S, "plk"))
			{
				_CurrentLanguage = PL_LangID;
			} else {
				_CurrentLanguage = EN_LangID;
			}
			Result = true;
		}
		return Result;
	}

	bool	Lang::SetLanguage(std::string LangShortName)
	{
		LANGID LID = SystemLangID();
		if (!LangShortName.empty())
			LID = ShortNameToLangID(LangShortName);

		if (LID)
		{
			SetLanguage(LID);
			return true;
		}
		return false;
	}

	const char* Lang::Text(int ID)
	{
		if (ID < 0)
			return "";
		if (ID > (ItemsCount() - 1))
			return "";

		if (_FileTextItems)
		{
			return _FileTextItems[ID].LangItem;
		} else {
			if (_CurrentLanguage == PL_LangID)
				return (char*)_TextItems[ID].LangList[0];
			if (_CurrentLanguage == EN_LangID)
				return (char*)_TextItems[ID].LangList[1];
			return "";
		}
	};

	LANGID	Lang::SystemLangID()
	{
		return GetSystemDefaultLangID();
	}

	int	Lang::ItemsCount()
	{
		return _ItemsCount;
	}

	std::string Lang::LangFileName(LANGID LocaleID)
	{
		return _LangFilesDirectory + LangPath + GetLanguageShortName(LocaleID) + ".txt";
	}

	bool	Lang::LangFileExists(LANGID LocaleID)
	{
		return File::FileExists(LangFileName(LocaleID));
	}

	void	Lang::SaveToFile(DWORD dwCreateType)
	{
		std::string FileName = _LangFilesDirectory + LangPath;
		char	LineBuff[2048];
		::CreateDirectory(FileName.c_str(), NULL);
		File::TextFile	MyFile;
		MyFile.Open((char*)LangFileName(_CurrentLanguage).c_str(), File::faWrite, File::fsRead, dwCreateType);
		if (MyFile.NoError())
		{
			for (int I = 0; I < _ItemsCount; I++)
			{
				std::string S = Text(I);
				S = Text::Replace(S, "\n", "\\n");
				sprintf(LineBuff, "%0.4d:%s", I, S.c_str());
				MyFile.WriteLine(LineBuff);
			}
		}
		MyFile.Close();
	}

	bool	Lang::LoadFromFile(LANGID LocaleID)
	{
		TCHAR Buff[256];
		bool Result = false;
		FreeMemoryForTextFile();

		File::TextFile	MyFile;
		MyFile.Open((char*)LangFileName(LocaleID).c_str(), File::faRead, File::fsRead, OPEN_EXISTING);
		if (MyFile.NoError())
		{
			Result = true;
			_ItemsCount = MyFile.LinesCount();
			if (_ItemsCount == _TextItemsCount)
			{
				ReserveMemoryForTextFile(_ItemsCount);
				TCHAR LineNumber[8];
				for (int I = 0; I < _ItemsCount; I++)
				{
					MyFile.ReadLine();
					lstrcpyn(LineNumber, MyFile.StringBuffer(), 5);
					
					std::string S = MyFile.StringBuffer() + 5;
					S = Text::Replace(S, "\\n", "\n");
					lstrcpy(_FileTextItems[I].LangItem, S.c_str());

					if (atoi(LineNumber) != I)
					{
						wsprintf(Buff, "String ID:%s, in language file is not equal it's position (%d) !", LineNumber, I);
						MessageBox(NULL, Buff, "Warning !", MB_OK | MB_ICONWARNING);
						Result = false;
						break;
					}
				}
			} else {
				wsprintf(Buff, "Invalid amount (is %d, must be %d) of items in language file '%s' !", _ItemsCount, _TextItemsCount, LangFileName(LocaleID).c_str());
				MessageBox(NULL, Buff, "Warning !", MB_OK | MB_ICONWARNING);
				Result = false;
			}
		}
		MyFile.Close();
		if (!Result)
		{
			_ItemsCount = _TextItemsCount;
			FreeMemoryForTextFile();
		}
		return Result;
	}

	std::string Lang::LanguageInfo()
	{
		static const char* Desc[] = {
			"Your system language is '%s'. ",
			"File of your language is '%s.txt'. ",

			"I can't find file '%s.txt' of your language in '%s' subdirectory, so it cannot be "
			"loaded automatically. If you like this software, then you can help me, and translate "
			"any existing language text file to your language. Then, name it '%s.txt' and send to my "
			"email address. Thank you. ",
		};
		char	LineBuff[2048];
		std::string S = "";
		//Adding description
		LANGID LocaleID = SystemLangID();
		sprintf(LineBuff, Desc[0], GetLanguageLocalizedName(LocaleID).c_str());
		S+=LineBuff;
		if (LangFileExists(SystemLangID()))
		{
			sprintf(LineBuff, Desc[1], GetLanguageShortName(LocaleID).c_str());
			S+=LineBuff;
		} else {
			sprintf(LineBuff, Desc[2], GetLanguageShortName(LocaleID).c_str(),
				LangPath.c_str(), GetLanguageShortName(LocaleID).c_str());
			S+=LineBuff;
		}
		return S;
	}

	std::string Lang::LanguageInfoForConfig(std::string Desc)
	{
		std::string S = Desc;
		LANGID LocaleID = SystemLangID();
		S = Text::Replace(S, "<lang>", GetLanguageLocalizedName(LocaleID));
		S = Text::Replace(S, "<file>", GetLanguageShortName(LocaleID));
		S = Text::Replace(S, "<current>", GetLanguageLocalizedName(CurrentLangID()));
		S = Text::ConditionString(S, LangFileExists(SystemLangID()));
		return S;
	}

	void	Lang::ShowLanguageInfo()
	{
		MessageBox(NULL, LanguageInfo().c_str(), "Information", MB_OK | MB_ICONINFORMATION);
	}

	void	Lang::SaveHelpFile(bool ShowLangList, DWORD dwCreateType)
	{
		std::string FileName = _LangFilesDirectory + LangPath;
		char	LineBuff[2048];
		::CreateDirectory(FileName.c_str(), NULL);
		File::TextFile	MyFile;
		FileName += "_info.txt";
		MyFile.Open((char*)FileName.c_str(), File::faWrite, File::fsRead, dwCreateType);
		if (MyFile.NoError())
		{
			LineBuff[0] = 0;
			MyFile.WriteLine(LanguageInfo());
			if (ShowLangList)
			{
				for (int I = 0; I < (int)SystemLangIDList.size(); I++)
				{
					LANGID LocaleID = (LANGID)Text::HexStringToInt(SystemLangIDList[I]);
					sprintf(LineBuff, "%s.txt %s",
						GetLanguageShortName(LocaleID).c_str(),
						GetLanguageLocalizedName(LocaleID).c_str());
					MyFile.WriteLine(LineBuff);
				}
			}
		}
		MyFile.Close();
	}

	bool	Lang::GetListOfAvailableLangFiles()
	{
		bool Result = false;
		if (_UseLangFiles)
		{
			AvailableShortLangNamesList.clear();
			Result = _RecDir.Get(_LangFilesDirectory + LangPath);
			std::sort(AvailableShortLangNamesList.begin(), AvailableShortLangNamesList.end());
		} else {
			AvailableShortLangNamesList.push_back("eng");
			AvailableShortLangNamesList.push_back("plk");
		}
		return Result;
	}

	std::string	GetLocaleInfoString(int LocaleID, LCTYPE Type)
	{
		std::string S = "";
		int BuffSize = GetLocaleInfo(LocaleID, Type, NULL, 0);
		if (BuffSize > 0)
		{
			TCHAR* lpBuffer = new TCHAR[BuffSize];
			if (GetLocaleInfo(LocaleID, Type, lpBuffer, BuffSize) > 0)
				S = lpBuffer;
			delete [] lpBuffer;
		}
		return S;
	}

	std::string	GetLanguageDayName(int LocaleID, int Day)
	{
		LCTYPE DN = LOCALE_SDAYNAME1;
		switch (Day)
		{
			case 1:	DN = LOCALE_SDAYNAME1; break;
			case 2:	DN = LOCALE_SDAYNAME2; break;
			case 3:	DN = LOCALE_SDAYNAME3; break;
			case 4:	DN = LOCALE_SDAYNAME4; break;
			case 5:	DN = LOCALE_SDAYNAME5; break;
			case 6:	DN = LOCALE_SDAYNAME6; break;
			case 7:	DN = LOCALE_SDAYNAME7; break;
		};
		return GetLocaleInfoString(LocaleID, DN);
	}

	std::string	GetLanguageEnglishName(int LocaleID)
	{
		return GetLocaleInfoString(LocaleID, LOCALE_SENGLANGUAGE);
	}

	std::string	GetLanguageLocalizedName(int LocaleID)
	{
		return GetLocaleInfoString(LocaleID, LOCALE_SLANGUAGE);
	}

	std::string	GetLanguageShortName(int LocaleID)
	{
		return Text::LowerCase(GetLocaleInfoString(LocaleID, LOCALE_SABBREVLANGNAME));
	}

	LANGID	ShortNameToLangID(std::string LangShortName)
	{
		std::string LangID = "";
		LangShortName = Text::LowerCase(LangShortName);
		for (int IDX = 0; IDX < (int)SystemLangIDList.size(); IDX++)
		{
			LangID = SystemLangIDList[IDX];
			LangID = GetLanguageShortName(Text::HexStringToInt(LangID));
			if (Text::StringsEqual(LangShortName, LangID))
			{
				return (LANGID)Text::HexStringToInt(SystemLangIDList[IDX]);
			}
		}
		return 0;
	}

	//------------------------------------------------------------------------
	BOOL	CALLBACK EnumLocalesProc(LPTSTR lpLocaleString)
	{
		SystemLangIDList.push_back(lpLocaleString);
		return true;
	}

	void	EnumLangFiles(LPVOID UserData, const std::string Dir, const WIN32_FIND_DATA& FileData, bool& Cancel, bool& Exclude)
	{	
		std::string S = FileData.cFileName;
		if (Text::LowerCase(File::FileExtension(S)) == "txt")
		{
			S = File::RemoveFileExtension(S);
			if (ShortNameToLangID(S))
			{
				AvailableShortLangNamesList.push_back(S);
			}
		}
	}

	void SetLangSubFolder(std::string sName)
	{
		LangPath = sName;
		LangPath += "\\";
	}

}
