#pragma once

#include "..\Default.h"
#include "..\WinWorkBar.h"
#include "..\WinWorkBarDlg.h"
#include "..\Libs\PSLanguage.h"

//-----------------------------------------------------------------------------------------

//Shows messagebox, and logs errors
enum UserMsgType
{
	umtError,
	umtInfo,
	umtWarning,
	umtWarning_YesNoCancel,
	umtWarning_YesNo
};

int ShowUserMsg(CWnd* Win, UserMsgType Type, int TextID, int CaptionID = 0);

void ShowError(std::string S);

void ShowError(CWnd* Win, std::string S);
void ShowError(CWnd* Win, int I);

const Language::TextItem* GetAppTextItems();
