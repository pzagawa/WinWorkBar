
#include "../LibsOther/Language.h"

extern CWinWorkBarDlg*	lpMain;

//-----------------------------------------------------------------------------------------

//STATIC LANGUAGE INTERNAL STRINGS TABLE
const Language::TextItem AppTextItems[] =
{
	//0
	"Autor t�umaczenia: Piotr Zagawa",	//PL TEXT ITEM
	"Translation author: Piotr Zagawa",	//EN TEXT ITEM
	"Data: 05-06-2003",
	"Date: 05-06-2003",
	"email: vetch@vetch.magot.pl",
	"email: vetch@vetch.magot.pl",
	"--------------------------------",
	"--------------------------------",

	//ID: 4
	"Menu",
	"Menu",
	"Nie chowaj",
	"Not hide",

	//MainMenu ID: 6
	"Ukryj",				//id: 6
	"Hide",
	"Ustawienia",		//id: 7
	"Settings",
	"O programie",	//id: 8
	"About",
	"Zako�cz",			//id: 9
	"Exit",

	"Nie chowaj",	//id: 10
	"Don't hide",

	"Data i czas",	//id: 11
	"Date and time",
	"Kalendarz",		//id: 12
	"Calendar",

	//MainMenu ID: 13
	"Nowa notatka",
	"New note",
	"Nowe zadanie",
	"New task",
	"Szukaj",		//id 15
	"Search",

	//Tab ID: 16
	"Zadania",
	"Tasks",
	"Notatki",
	"Notes",
	
	//File menu ID: 18
	"Plik",
	"File",
	"Usu�",	//19
	"Delete",
	"Widok",	//20
	"View",

	//Tab ID: 21
	"Dzisiaj",
	"Today",
	"Jutro",
	"Tomorrow",
	"Wczoraj",
	"Yesterday",
	"Za tydzie�",
	"For week",

	//Categories ID: 25
	"Kategorie",
	"Categories",
	"Dodaj",	//26
	"Add",
	"Zapisz",	//27
	"Save",
	"Anuluj",	//28
	"Cancel",
	"Nazwa",	//29
	"Name",
	"Nie mo�na za�adowa� pliku danych",	//30
	"Cannot load data file",
	"Popraw",	//31
	"Update",
	"Nie mo�na zapisa� pliku danych",	//32
	"Cannot save data file",

	//Cat. items ID: 33
	"Praca",			//id: 33
	"Work",
	"Osobiste",		//id: 34
	"Personal",
	"Dni wolne",	//id: 35
	"Days off",
	"Urodziny",		//id:	36
	"Birthdays",
	"Pomys�y",		//id: 37
	"Ideas",
	"�wi�ta",			//id: 38
	"Holidays",
	"Projekty",		//id: 39
	"Projects",
	"Telefony",		//id: 40
	"Phones",
	"Pieni�dze",	//id: 41
	"Money",
	"Op�aty",			//id: 42
	"Payments",
	"Zakupy",			//id: 43
	"Shopping",
	"Spotkania",	//id: 44
	"Meetings",
	"Cytaty",			//id: 45
	"Citations",

	//Button default
	"Domy�lne",				//id: 46
	"Default",

	//Categories select
	"Wybierz kategorie",	//id: 47
	"Select categories",
	"Wyczy��",	//id: 48
	"Clear",
	"OK",				//id: 49
	"OK",
	"Wybrane",	//id: 50
	"Selected",
	"Wszystko",	//id: 51
	"All",

	//About dialog ID: 52
	"Rejestruj",
	"Register",

	//License status ID: 53
	"Ta kopia programu jest zarejestrowana dla",		//id: 53
	"This program copy is registered for",
	"Ta kopia programu NIE jest zarejestrowana",		//id: 54
	"This program copy is NOT registered",
	"Liczba dni testowania programu",								//id: 55
	"Amount of program testing days",
	"Okres testowania programu (<days> dni) min��",	//id: 56
	"Program testing period (<days> days) passed",
	"Aby zarejestrowa� ten program, kliknij przycisk 'Rejestruj'",	//id: 57
	"To register this program, click 'Register' button",
	"(wersja <version>)",		//id: 58
	"(<version> version)",
	"Pobierz now� wersj�",	//id: 59
	"Download new version",

	//Register dialog ID: 60
	"Otw�rz stron� rejestracji programu",	//id: 60
	"Open program registration page",
	"Wprowad� otrzyman� nazw� u�ytkownika i kod rejestracyjny",	//id: 61
	"Enter received user name and registration code",
	"U�ytkownik",		//id: 62
	"User",
	"Kod rejestracyjny",	//id: 63
	"Registration code",
	"Zrobione",		//id: 64
	"Done",
	"Zamknij",		//id: 65
	"Close",

	//Registration link id: 66
	"Rejestruj program",
	"Register program",

	//Main menu
	"Pomoc",					//id: 67
	"Help",
	"Strona domowa",	//id: 68
	"Home page",

	//TrayIcon menu
	"Poka�/Ukryj",		//id: 69
	"Show/Hide",

	//Main menu - file
	"Edycja",					//id: 70
	"Edit",

	//TabBar buttons
	"Nowy",			//id: 71
	"New",
	"Edycja",		//id: 72
	"Edit",
	"Usu�",			//id: 73
	"Delete",
	"Szukaj",		//id: 74
	"Search",
	"Widok",		//id: 75
	"View",

	//LIST - Notes
	"Tre��",				//id: 76
	"Contents",
	"Dodane",				//id: 77
	"Added",
	"Zmienione",		//id: 78
	"Changed",

	//Title bar desc
	"Edycja notatki",	//id: 79
	"Note edit",
	"Edycja zadania",	//id: 80
	"Task edit",

	//Ask message
	"Czy chcesz zapisa� zmieniony tekst ?",	//id: 81
	"Do you want to save changed text ?",

	"Wpisz tre��",		//id: 82
	"Enter content",

	"Czy chcesz usun�� zaznaczone notatki ?",	//id: 83
	"Do you want delete selected notes ?",

	"Zawsze na wierzchu",	//id: 84
	"Always on top",

	//RichEdit context menu
	"Wstaw skr�t www",	//id: 85
	"Insert www link",
	"Wstaw skr�t email",
	"Insert email link",
	"Wstaw skr�t do pliku",
	"Insert file link",
	"Wstaw dat�",
	"Insert date",
	"Wstaw d�ug� dat�",
	"Insert long date",
	"Wstaw czas",
	"Insert time",
	"Wstaw dat� i czas",
	"Insert date and time",

	//Date range
	"Poka� od",		//id: 92
	"Show from",
	"do",					//id: 93
	"to",
	"Do",					//id: 94
	"To",

	//View menu items
	"Wszystko",				//id: 95
	"All",
	"wszystko",				//id: 96
	"all",
	"Wg kategorii",		//id: 97
	"By categories",
	"Wg post�pu",			//id: 98
	"By progress",
	"Wg priorytetu",	//id: 99
	"By priority",
	"Wg daty widoczno�ci",	//id: 100
	"By visibility date",
	"Wg daty dodania",			//id: 101
	"By adding date",
	"Wg daty zmiany",				//id: 102
	"By change date",

	"nie zrobione",					//id: 103
	"not done",
	"zrobione cz�ciowo",		//id: 104
	"partially done",
	"zako�czone",						//id: 105
	"finished",

	"wa�ne",								//id: 106
	"important",
	"domy�lne",							//id: 107
	"default",
	"nie wa�ne",						//id: 108
	"not important",

	"ostatnie 30 dni",			//id: 109
	"last 30 days",
	"ostatnie 7 dni",				//id: 110
	"last 7 days",
	"wczoraj",							//id: 111
	"yesterday",
	"dzisiaj",							//id: 112
	"today",

	//lab info
	"Od�wie�anie...",	//id: 113
	"Refreshing...",	
	"Wszystkie",			//id: 114
	"All",
	"Wybrane",				//id: 115
	"Selected",

	//Categories defalut load 
	"Czy chcesz ustawi� domy�lne warto�ci ?",		//id: 116
	"Do you want set default values ?",

	//Edit task
	"Data",				//id: 117
	"Date",
	"Priorytet",	//id: 118
	"Priority",
	"Post�p",			//id: 119
	"Progress",
	"Powt�rzenia",	//id: 120
	"Repeats",

	"Czy chcesz usun�� notatk� ?",	//id: 121
	"Do you want delete note ?",
	"Czy chcesz usun�� zadanie ?",	//id: 122
	"Do you want delete task ?",

	//Repeats dialog
	"Rodzaj",				//id: 123
	"Type",
	"Dni tygodnia",	//id: 124
	"Week days",

	"wy��czone",		//id: 125
	"off",
	"co dzie�",			//id: 126
	"every day",
	"co miesi�c",		//id: 127
	"every month",
	"co rok",				//id: 128
	"every year",
	"dni tygodnia",	//id: 129
	"week days",

	"Czy chcesz usun�� zaznaczone zadania ?",	//id: 130
	"Do you want delete selected tasks ?",

	//Tool info window
	"Zako�cz",	//id: 131
	"Finish",
	"Poka�",		//id: 132
	"Show",
	"zadanie o",	//id: 133
	"task at",

	//Search
	"Ze s�owami",		//id: 134
	"With words",
	"Bez s��w",			//id: 135
	"Without words",
	"Szukaj !",			//id: 136
	"Search !",
	"zadanie",			//id: 137
	"task",
	"notatka",			//id: 138
	"note",

	"Wprowad� jedno lub kilka s��w i wci�nij przycisk 'Szukaj !'",	//id: 139
	"Enter one or few words and press 'Search !' button",
	"Nie znaleziono zada�/notatek wg podanych s��w",		//id: 140
	"No tasks/notes found by given words",

	"Znalezionych pozycji: %d. Zada�: %d. Notatek: %d",	//id: 141
	"Items finded: %d. Tasks: %d. Notes: %d",
	"Szukanie...",	//id: 142
	"Searching...",

	"Poka� w przegl�darce",	//id: 143
	"Show in browser",
	"Zapisz jako HTML",		//id: 144
	"Save as HTML",

	"Lista notatek",//id: 145
	"Notes list",
	"Lista zada�",	//id: 146
	"Tasks lists",
	"Notatka",			//id: 147
	"Note",
	"Zadanie",			//id: 148
	"Task",
	"Pliki html",		//id: 149
	"Html files",

	//Settings
	"J�zyk / Language",	//id: 150
	"Language / J�zyk",
	"Ukryj okno po",		//id: 151
	"Hide window after",
	"sekundach",				//id: 152
	"seconds",
	"Steruj programem przez klawisze HotKey",	//id: 153
	"Control program with HotKeys",
	"Uruchom program",			//id: 154
	"Execute program",
	"Przypomnienie",				//id: 155
	"Reminder",
	"przy starcie systemu",	//id: 156
	"at system start",
	"Poka�",	//id: 157
	"Show",
	"minut wcze�niej",	//id: 158
	"minutes earlier",
	"D�wi�k",	//id: 159
	"Sound",
	"przez wewn�trzny g�o�niczek",	//id: 160
	"by internal speaker",
	"Test",	//id: 161
	"Test",
	"Wy��czony",	//id: 162
	"Off",

	//Expression evaluator
	"B��d sk�adni",				//SYNTAX_ERROR	id: 163
	"Syntax error",
	"Niedopasowane nawiasy",	//MISM_PARENTH	id: 164
	"Mismatched parenthesis",
	"Brak ')'",				//MISSING_PARENTH	id: 165
	"Missing ')'",
	"Puste wyra�enie",		//EMPTY_PARENTH	id: 166
	"Empty parentheses",
	"B��d sk�adni: oczekiwany operator",	//EXPECT_OPERATOR	id: 167
	"Syntax error: operator expected",
	"Brak pami�ci",				//OUT_OF_MEMORY	id: 168
	"Not enough memory",
	"Nieoczekiwany b��d",	//UNEXPECTED_ERROR	id: 169
	"An unexpected error ocurred",
	"B��d sk�adni w parametrze 'Vars' dla funkcji 'FunctionParser::Parse()'",	//INVALID_VARS	id: 170
	"Syntax error in parameter 'Vars' given to 'FunctionParser::Parse()'",
	"Nieprawid�owa liczba parametr�w dla funkcji",	//ILL_PARAMS_AMOUNT id: 171
	"Illegal number of parameters to function",
	"B��d sk�adni: przedwczesny koniec ci�gu",	//PREMATURE_EOS	id: 172
	"Syntax error: premature end of string",
	"B��d sk�adni: oczekiwany nawias '(' za funkcj�",	//EXPECT_PARENTH_FUNC id: 173
	"Syntax error: expecting '(' after function",
	
	"B��d przy znaku",	//id: 174
	"Error at char",
	"B��d oblicze�",		//id: 175
	"Calculation error",
	"Oblicz",						//id: 176
	"Calculate",

	"Witaj w programie WinWorkBar !",	//id: 177
	"Welcome in WinWorkBar program !",

	"Importuj dane",	//id: 178
	"Import data",
	"Eksportuj dane",	//id: 179
	"Export data",

	"Pliki xml",		//id: 180
	"Xml files",

	"Wybierz dane dla operacji",	//id: 181
	"Select data for operation",

	"Otw�rz",	//id: 182
	"Open",

	"Plik nie jest dokumentem programu WinWorkbar",	//id: 183
	"File is not a WinWorkBar program document",

	"Nie mo�na za�adowa� pliku szablonu html",	//id: 184
	"Cannot load html template file",

	//Clock - zodiac list
	"Wodnik,Ryby,Baran,Byk,Bli�ni�ta,Rak,Lew,Panna,Waga,Skorpion,Strzelec,Kozioro�ec",	//id: 185
	"Aquarius,Pisces,Aries,Taurus,Gemini,Cancer,Leo,Virgo,Libra,Scorpio,Sagittarius,Capricorn",

	"Styl zegara",	//id: 186
	"Clock style",

	"LCD i znaki zodiaku",		//id: 187
	"LCD and zodiac signs",
	"Zegar tekstowy",	//id :188
	"Text clock",

	"Poka� okno na klikni�cie",	//id: 189
	"Show window on click",

	"P�niej",	//id: 190
	"Later",

	"Od�� zadanie na",	//id: 191
	"Put away task for",
	"minut",		//id: 192
	"minutes",

	//ending item
	NULL, 
	NULL,
};

//-----------------------------------------------------------------------------------------

int ShowUserMsg(CWnd* Win, UserMsgType Type, int TextID, int CaptionID)
{
	UINT UMTypes[] =
	{
		MB_ICONERROR				| MB_OK,
    MB_ICONINFORMATION	| MB_OK,
    MB_ICONWARNING			| MB_OK,
    MB_ICONWARNING			| MB_YESNOCANCEL,
    MB_ICONWARNING			| MB_YESNO
	};
	char* lpCaption = (char*)((CaptionID)?lpMain->ML.Text(CaptionID):NULL);
	return Win->MessageBox(lpMain->ML.Text(TextID), lpCaption, UMTypes[Type] | MB_TOPMOST);
	return 0;
}

void ShowError(std::string S)
{
	lpMain->GetDesktopWindow()->MessageBox(S.c_str(), NULL, MB_ICONERROR | MB_OK | MB_TOPMOST);
}

void ShowError(CWnd* Win, std::string S)
{
	Win->MessageBox(S.c_str(), NULL, MB_ICONERROR | MB_OK);
}

void ShowError(CWnd* Win, int I)
{
	std::string S = Text::IntToStr(I);
	Win->MessageBox(S.c_str(), NULL, MB_ICONERROR | MB_OK);
}

const Language::TextItem* GetAppTextItems()
{
	return AppTextItems;
}
