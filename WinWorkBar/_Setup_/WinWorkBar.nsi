; WinWorkBar install script
; 08-10-2004

!define VER_MAJOR 1
!define VER_MINOR 3

AllowRootDirInstall	false
InstallColors				/windows
InstProgressFlags		smooth
XPStyle							on
LicenseBkColor			FFFFFF

Icon								"..\Graph\WinWorkBar.ico"
UninstallIcon				"..\Graph\Uninstall.ico"
CheckBitmap					"check_bitmap.bmp"

;Load languages - first is default
LoadLanguageFile "Langs\English.nlf"
	Name English
LoadLanguageFile "Langs\Polish.nlf"
	Name Polish

;INIT
Function .onInit
	;Language selection dialog
	Push ""
	Push ${LANG_ENGLISH}
	Push "English language"
	Push ${LANG_POLISH}
	Push "Jêzyk polski"
	Push A ; A means auto count languages for the auto count to work the first empty push (Push "") must remain
	
	LangDLL::LangDialog "Installer Language" "Select language for installer and help file"	
	
	Pop $LANGUAGE
	StrCmp $LANGUAGE "cancel" 0 +2
		Abort
FunctionEnd

LicenseText	/LANG=${LANG_ENGLISH}	"To continue, you must accept license content" "I Agree"
LicenseText	/LANG=${LANG_POLISH}	"Aby kontynuowaæ, musisz zaakceptowaæ treœæ licencji" "Akceptujê"

LicenseData	/LANG=${LANG_ENGLISH}	"..\Bin\license_en.txt"
LicenseData	/LANG=${LANG_POLISH}	"..\Bin\license_pl.txt"

BrandingText	/LANG=${LANG_ENGLISH}	" "
BrandingText	/LANG=${LANG_POLISH}	" "

OutFile		"..\_Install_\WinWorkBar_v${VER_MAJOR}${VER_MINOR}.exe"
Name			"WinWorkBar ${VER_MAJOR}.${VER_MINOR}"

Caption		/LANG=${LANG_ENGLISH}	"WinWorkBar version ${VER_MAJOR}.${VER_MINOR}"
Caption		/LANG=${LANG_POLISH}	"WinWorkBar wersja ${VER_MAJOR}.${VER_MINOR}"

SubCaption	/LANG=${LANG_ENGLISH}	0 ". License"
SubCaption	/LANG=${LANG_POLISH}	0 ". Licencja"

SubCaption	/LANG=${LANG_ENGLISH}	1 ". Options"
SubCaption	/LANG=${LANG_POLISH}	1 ". Opcje"

SubCaption	/LANG=${LANG_ENGLISH}	2 ". Install folder"
SubCaption	/LANG=${LANG_POLISH}	2 ". Katalog instalacyjny"

SubCaption	/LANG=${LANG_ENGLISH}	3 ". Installing..."
SubCaption	/LANG=${LANG_POLISH}	3 ". Instalowanie..."

SubCaption	/LANG=${LANG_ENGLISH}	4 ". Done"
SubCaption	/LANG=${LANG_POLISH}	4 ". Zrobione"

FileErrorText	/LANG=${LANG_ENGLISH}	"File save error$\r$\nFile can be locked or executed$\r$\nIf it is a program, finish it and repeat operation$\r$\n$0"
FileErrorText	/LANG=${LANG_POLISH}	"B³¹d zapisu pliku$\r$\nPlik mo¿e byæ zablokowany lub uruchomiony$\r$\nJeœli jest to program, zakoñcz go i ponów operacjê$\r$\n$0"

MiscButtonText		/LANG=${LANG_ENGLISH}	"Back"		"Next"	"Cancel"	"Close"
MiscButtonText		/LANG=${LANG_POLISH}	"Wstecz"	"Dalej"	"Porzuæ"	"Zamknij"

InstallButtonText	/LANG=${LANG_ENGLISH}	"Install"
InstallButtonText	/LANG=${LANG_POLISH}	"Instaluj"

DirText		/LANG=${LANG_ENGLISH}	"WinWorkBar will be installed in selected folder" "Select folder" "Browse"
DirText		/LANG=${LANG_POLISH}	"WinWorkBar zostanie zainstalowany w wybranym katalogu" "Wybierz katalog" "Przegl¹daj"

SpaceTexts	/LANG=${LANG_ENGLISH}	"Required " "Available "
SpaceTexts	/LANG=${LANG_POLISH}	"Wymagane " "Dostêpne "

DetailsButtonText	/LANG=${LANG_ENGLISH}	"Details"
DetailsButtonText	/LANG=${LANG_POLISH}	"Szczegó³y"

CompletedText		/LANG=${LANG_ENGLISH}	"Done"
CompletedText		/LANG=${LANG_POLISH}	"Zrobione"

ComponentText		/LANG=${LANG_ENGLISH}	"This program will install WinWorkBar on your computer" " " "Select components to install"
ComponentText		/LANG=${LANG_POLISH}	"Ten program zainstaluje WinWorkBar na Twoim komputerze" " " "Wybierz sk³adniki do zainstalowania"

InstallDir $PROGRAMFILES\WinWorkBar

;Nazwy sekcji
LangString SecMainName ${LANG_ENGLISH}	"WinWorkBar ${VER_MAJOR}.${VER_MINOR}"
LangString SecMainName ${LANG_POLISH}		"WinWorkBar ${VER_MAJOR}.${VER_MINOR}"

LangString Sec1Name ${LANG_ENGLISH}			"WinWorkBar group in Start Menu"
LangString Sec1Name ${LANG_POLISH}			"Grupa WinWorkBar w Menu Start"

LangString Sec2Name ${LANG_ENGLISH}			"Shortcut in Start Menu"
LangString Sec2Name ${LANG_POLISH}			"Skrót w Menu Start"

LangString Sec3Name ${LANG_ENGLISH}			"Shortcut on desktop"
LangString Sec3Name ${LANG_POLISH}			"Skrót na pulpicie"

LangString Sec4Name ${LANG_ENGLISH}			"Shortcut in quicklaunch bar"
LangString Sec4Name ${LANG_POLISH}			"Skrót w pasku szybkiego uruchamiania"

;SEKCJA PROGRAMU
Section !$(SecMainName) secmain
	SectionIn	RO

	;Send WM_WinWorkBar_TERMINATE = WM_USER + 4444 to hidden WinWorkBar message window !!!
	FindWindow		$9	"#32770"	"WinWorkBar HIDDEN MESSAGE WINDOW"
	IntCmp				$9	0			WindowNotFinded
		SendMessage	$9	5468	0	0
		Sleep	2000
	WindowNotFinded:
	
	;PLIKI
  SetOutPath $INSTDIR
	  File "..\Bin\WinWorkBar.exe"
	  File "..\Bin\File_Id.diz"
	 
		StrCmp $LANGUAGE ${LANG_ENGLISH} 0 +2
		  File "..\Help\EN\WinWorkBar.chm"
		StrCmp $LANGUAGE ${LANG_POLISH} 0 +2
		  File "..\Help\PL\WinWorkBar.chm"
	  
  SetOutPath $INSTDIR\ExportStyles
	  File /r "..\Bin\ExportStyles\*.*"

  SetOutPath $INSTDIR\Import
	  File /r "..\Bin\Import\*.*"

  SetOutPath $INSTDIR\Lang
	  File /r "..\Bin\Lang\*.*"

  SetOutPath $INSTDIR\Sounds
	  File /r "..\Bin\Sounds\*.*"

	;REJESTR
  WriteUninstaller	"$INSTDIR\Uninstall.exe"

  WriteRegStr HKLM	"Software\Microsoft\Windows\CurrentVersion\Uninstall\WinWorkBar" \
										"DisplayName" 			"WinWorkBar"

  WriteRegStr HKLM	"Software\Microsoft\Windows\CurrentVersion\Uninstall\WinWorkBar" \
										"UninstallString"	"$INSTDIR\Uninstall.exe"

	;Dane startowe
  WriteRegStr HKCU	"Software\PeterSoft\WinWorkBar"	"ProfileOptions_HideToTray"		"1"
  WriteRegStr HKCU	"Software\PeterSoft\WinWorkBar"	"ProfileOptions_StartInTray"	"1"

	;Autostart
  WriteRegStr HKCU	"Software\Microsoft\Windows\CurrentVersion\Run" \
										"WinWorkBar" "$\"$INSTDIR\WinWorkBar.exe$\""
						
	;Inne	
  WriteRegStr HKLM	"Software\PeterSoft\WinWorkBar"	"ExePath"	"$INSTDIR"
  WriteRegStr HKLM	"Software\PeterSoft\WinWorkBar"	"Version"	"${VER_MAJOR}.${VER_MINOR}"
SectionEnd

;SEKCJE SKRÓTÓW
Section !$(Sec1Name) sec1
	SectionIn	RO
	CreateDirectory		"$SMPROGRAMS\WinWorkBar\"
  CreateShortCut		"$SMPROGRAMS\WinWorkBar\WinWorkBar.lnk"	"$INSTDIR\WinWorkBar.exe"
  
	;HELP FILE LINK	  
	StrCmp $LANGUAGE ${LANG_ENGLISH} 0 +2
	  CreateShortCut 		"$SMPROGRAMS\WinWorkBar\WinWorkBar Help.lnk"	"$INSTDIR\WinWorkBar.chm"
	StrCmp $LANGUAGE ${LANG_POLISH} 0 +2
	  CreateShortCut 		"$SMPROGRAMS\WinWorkBar\WinWorkBar Pomoc.lnk"	"$INSTDIR\WinWorkBar.chm"

	;UNINSTALL LINK	  
	StrCmp $LANGUAGE ${LANG_ENGLISH} 0 +2
	  CreateShortCut	"$SMPROGRAMS\WinWorkBar\Uninstall.lnk"	"$INSTDIR\uninstall.exe"
	StrCmp $LANGUAGE ${LANG_POLISH} 0 +2
	  CreateShortCut	"$SMPROGRAMS\WinWorkBar\Odinstaluj.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section $(Sec2Name) sec2
  CreateShortCut	"$STARTMENU\WinWorkBar.lnk"					"$INSTDIR\WinWorkBar.exe"
SectionEnd

Section $(Sec3Name) sec3
  CreateShortCut	"$DESKTOP\WinWorkBar.lnk"						"$INSTDIR\WinWorkBar.exe"
SectionEnd

Section $(Sec4Name) sec4
  CreateShortCut	"$QUICKLAUNCH\WinWorkBar.lnk"				"$INSTDIR\WinWorkBar.exe"
SectionEnd


;INSTALL SUCCESS
Function .onInstSuccess
	;Run WinWorkBar in tray !
	Exec	"$INSTDIR\WinWorkBar.exe"
FunctionEnd

;UNINSTALL
UninstallText 			/LANG=${LANG_ENGLISH}	"Uninstalling WinWorkBar program"		"Folder"
UninstallText 			/LANG=${LANG_POLISH}	"Deinstalacja programu WinWorkBar"	"Katalog"

UninstallCaption		/LANG=${LANG_ENGLISH}	"WinWorkBar"
UninstallCaption		/LANG=${LANG_POLISH}	"WinWorkBar"

UninstallButtonText	/LANG=${LANG_ENGLISH}	"Uninstall"
UninstallButtonText	/LANG=${LANG_POLISH}	"Odinstaluj"

UninstallSubCaption	/LANG=${LANG_ENGLISH}	0	". Confirm"
UninstallSubCaption	/LANG=${LANG_POLISH}	0	". PotwierdŸ"

UninstallSubCaption	/LANG=${LANG_ENGLISH}	1	". Uninstalling files"
UninstallSubCaption	/LANG=${LANG_POLISH}	1	". Deinstalacja plików"

UninstallSubCaption	/LANG=${LANG_ENGLISH}	2	". Done"
UninstallSubCaption	/LANG=${LANG_POLISH}	2	". Zrobione"

Section Uninstall
	;Send WM_WINWORKBAR_TERMINATE = WM_USER + 4444 to hidden WinWorkBar message window !!!
	FindWindow		$9	"#32770"	"WinWorkBar HIDDEN MESSAGE WINDOW"
	IntCmp				$9	0			WindowNotFinded
		SendMessage	$9	5468	0	0
		Sleep	2000
	WindowNotFinded:

  ;Links
	Delete	"$SMPROGRAMS\WinWorkBar\*.*"
  RMDir		"$SMPROGRAMS\WinWorkBar"

  Delete	"$STARTMENU\WinWorkBar.lnk"
  Delete	"$DESKTOP\WinWorkBar.lnk"
  Delete	"$QUICKLAUNCH\WinWorkBar.lnk"

  ;Remove all files and dirs
  RMDir		/r "$INSTDIR"
  RMDir		"$INSTDIR"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinWorkBar"
SectionEnd
; eof

