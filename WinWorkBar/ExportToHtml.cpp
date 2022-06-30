#include "exporttohtml.h"
#include "Libs\PSFile.h"
#include "Libs\PSMFCUtils.h"
#include "Libs\PSUtil.h"

ExportToHtml::ExportToHtml(void)
{
	iNoteIdx = 1;
	iTaskIdx = 1;
}

ExportToHtml::~ExportToHtml(void)
{
}

bool ExportToHtml::Load(std::string Name)
{
	bool Result = false;

	std::string sFileName = File::IncludeBackSlash(Misc::ApplicationFolder());
	sFileName += "ExportStyles\\";
	sFileName += Name;

	sMain.clear();
	if (File::FileExists(sFileName))
	{
		File::TextFile StyleFile;	
		StyleFile.Open((char*)sFileName.c_str(), File::faRead, File::fsRead, OPEN_EXISTING);
		if (StyleFile.NoError())
		{
			StyleFile.SetBuffer(StyleFile.Size() + 1);
			StyleFile.Read();
			if (StyleFile.NoError())
			{
				if (StyleFile.Buffer())
					sMain = (char*)StyleFile.Buffer();
			}
			StyleFile.Close();
		}
	}

	if (!sMain.empty())
	{
		std::size_t iTbegin = sMain.find("[WWB_Task_begin]");
		std::size_t iTend = sMain.find("[WWB_Task_end]");
		std::size_t iNbegin = sMain.find("[WWB_Note_begin]");
		std::size_t iNend = sMain.find("[WWB_Note_end]");

		std::size_t iContSize = (iNend + ((std::string)"[WWB_Note_end]").size()) - iTbegin;

		iTbegin += ((std::string)"[WWB_Task_begin]").size();
		iTend -=((std::string)"[WWB_Task_end]").size();
		iNbegin += ((std::string)"[WWB_Note_begin]").size();
		iNend -=((std::string)"[WWB_Note_end]").size();

		sTask = sMain.substr(iTbegin, (iTend + ((std::string)"[WWB_Task_end]").size() - iTbegin));
		sNote = sMain.substr(iNbegin, (iNend + ((std::string)"[WWB_Note_end]").size() - iNbegin));

		iTbegin = sMain.find("[WWB_Task_begin]");

		sMain.replace(iTbegin, iContSize, "[WWB_CONTENTS]");
		sMainOriginal = sMain;

		Result = true;
	}
	return Result;
}

void ExportToHtml::Replace(std::string& sSrc, const std::string& sToken, const std::string& sWith)
{
	std::size_t pos = sSrc.find(sToken);
	if (pos != std::string::npos)
		sSrc.replace(pos, sToken.size(), sWith);
}

void ExportToHtml::Set(std::string sTitle, std::string sHeader, std::string sDate)
{
	sMain = sMainOriginal;
	Replace(sMain, "[WWB_Title]", sTitle);
	Replace(sMain, "[WWB_Header]", sHeader);
	Replace(sMain, "[WWB_Date]", sDate);
}

void ExportToHtml::AddNote(dataNote& Data)
{
	std::string S = sNote;
	Replace(S, "[WWB_NoteNumber]", Text::IntToStr(iNoteIdx++));
	Replace(S, "[WWB_NoteName]", Data.NoteName);
	Replace(S, "[WWB_NoteDate]", Data.NoteDate);
	Replace(S, "[WWB_NoteDateRange]", Data.NoteDateRange);
	Replace(S, "[WWB_NoteText]", Data.NoteText);
	Replace(S, "[WWB_NoteCategories]", Data.NoteCategories);
	sNoteList += S;
}

void ExportToHtml::AddTask(dataTask& Data)
{
	std::string S = sTask;
	Replace(S, "[WWB_TaskNumber]", Text::IntToStr(iTaskIdx++));
	Replace(S, "[WWB_TaskName]", Data.TaskName);
	Replace(S, "[WWB_TaskDate]", Data.TaskDate); 
	Replace(S, "[WWB_TaskDateRange]", Data.TaskDateRange);
	Replace(S, "[WWB_TaskRepeat]", Data.TaskRepeat); 
	Replace(S, "[WWB_TaskPriority]", Data.TaskPriority);
	Replace(S, "[WWB_TaskProgress]", Data.TaskProgress); 
	Replace(S, "[WWB_TaskText]", Data.TaskText); 
	Replace(S, "[WWB_TaskCategories]", Data.TaskCategories); 
	sTaskList += S;
}

bool ExportToHtml::Save(std::string sFileName)
{
	std::string S = sMain;
	std::string Contents = sTaskList + sNoteList;
	Replace(S, "[WWB_CONTENTS]", Contents);

	File::TextFile OutFile;
	OutFile.Open((char*)sFileName.c_str(), File::faWrite, File::fsRead, CREATE_ALWAYS);
	if (OutFile.NoError())
	{
		OutFile.Write((BYTE*)S.c_str(), (DWORD)S.size());
		OutFile.Close();
		return true;
	}
	return false;
}
