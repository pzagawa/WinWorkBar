
#pragma once

#include "default.h"
#include <string>
#include <vector>


struct PSTaskReminder_TaskItem
{
	std::string sUID;
	int iHour;
	int iMinute;
	PSTaskReminder_TaskItem()
	{
		Clear();
	}
	void Clear()
	{
		sUID.c_str();
		iHour = -1;
		iMinute = -1;
	}
	bool TimeInRange(SYSTEMTIME& stNow, int iMinutesBefore);
	bool TimeInRangeAndAfter(SYSTEMTIME& stNow, int iMinutesBefore);
	bool TimeEqual(PSTaskReminder_TaskItem& TaskItem);
	bool TimeEqual(int _iHour, int _iMinute);
};

typedef std::vector<PSTaskReminder_TaskItem> vecTaskItems;
typedef std::vector<std::string> vecUIDs;


class PSTaskReminder
{
private:
	bool				bCheckEnabled;
	std::vector<PSTaskReminder_TaskItem> vItems;

	std::vector<std::string> vUID_ViewedBeforeTerm;
	std::vector<std::string> vUID_ViewedAtTerm;

	PSTaskReminder_TaskItem* GetItem(std::string& sUID);
	std::string TaskUidCheck(SYSTEMTIME& stNow, int iMinutesBefore, int* iRange);
	bool ItemExists(std::vector<std::string>& vItems, std::string& sUID);
public:
	PSTaskReminder();
	~PSTaskReminder();

	int iMinutesBeforeRemind;

	void Clear();
	void EnableCheck();

	void Add(std::string& sUID, int iHour, int iMinute);
	void ResetForNewDay();
	void SetTaskAsViewed(std::string& sUID, bool bAtTerm);
	void ClearTaskAsNotViewed(std::string& sUID);

	std::string GetTaskUIDToView();
};

