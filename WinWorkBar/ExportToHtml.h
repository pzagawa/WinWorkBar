#pragma once

#include "default.h"
#include "resource.h"		// main symbols

#include <string>

#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"

class ExportToHtml
{
public:
	struct dataNote
	{
		std::string NoteName;
		std::string NoteDate;
		std::string NoteDateRange;
		std::string	NoteText;
		std::string	NoteCategories;
	};
	struct dataTask
	{
		std::string TaskName;
		std::string TaskDate; 
		std::string TaskDateRange;
		std::string TaskRepeat; 
		std::string TaskPriority;
		std::string TaskProgress; 
		std::string TaskText; 
		std::string TaskCategories; 
	};
private:
	std::string sMainOriginal;
	std::string sMain;
	std::string	sNote;
	std::string	sTask;
	std::string sNoteList;
	std::string sTaskList;

	int	iNoteIdx;
	int	iTaskIdx;

	void Replace(std::string& sSrc, const std::string& sToken, const std::string& sWith);
public:
	ExportToHtml(void);
	~ExportToHtml(void);

	bool Load(std::string Name);
	bool Save(std::string sFileName);

	void Set(std::string sTitle, std::string sHeader, std::string sDate);
	void AddNote(dataNote& Data);
	void AddTask(dataTask& Data);
};
