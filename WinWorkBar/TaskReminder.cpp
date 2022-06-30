
#include "taskreminder.h"

//PSTaskReminder_TaskItem
//-----------------------------------------------------------------------------
bool PSTaskReminder_TaskItem::TimeInRange(SYSTEMTIME& stNow, int iMinutesBefore)
{
	int iTaskMinutes = (iHour * 60) + iMinute;
	int iNowMinutes = (stNow.wHour * 60) + stNow.wMinute;
	int iRange = (iTaskMinutes - iNowMinutes);
	return ((iRange >= 0) && (iRange <= iMinutesBefore));
}

bool PSTaskReminder_TaskItem::TimeInRangeAndAfter(SYSTEMTIME& stNow, int iMinutesBefore)
{
	int iTaskMinutes = (iHour * 60) + iMinute;
	int iNowMinutes = (stNow.wHour * 60) + stNow.wMinute;
	int iRange = (iTaskMinutes - iNowMinutes);
	return (iRange <= iMinutesBefore);
}

bool PSTaskReminder_TaskItem::TimeEqual(PSTaskReminder_TaskItem& TaskItem)
{
	return ((TaskItem.iHour == iHour) && (TaskItem.iMinute == iMinute));
}

bool PSTaskReminder_TaskItem::TimeEqual(int _iHour, int _iMinute)
{
	return ((_iHour == iHour) && (_iMinute == iMinute));
}

bool Compare_TasksTimes(PSTaskReminder_TaskItem ti1, PSTaskReminder_TaskItem ti2)
{
	if (ti1.iHour < ti2.iHour)
		return true;
	if (ti1.iHour == ti2.iHour)
		return (ti1.iMinute < ti2.iMinute);
  return false;
}


//PSTaskReminder
//-----------------------------------------------------------------------------
PSTaskReminder::PSTaskReminder()
{
	Clear();
}

PSTaskReminder::~PSTaskReminder()
{
}

void PSTaskReminder::Clear()
{
	iMinutesBeforeRemind = 15;
	bCheckEnabled = false;
	vItems.clear();
}

void PSTaskReminder::EnableCheck()
{
	std::sort(vItems.begin(), vItems.end(), Compare_TasksTimes);
	bCheckEnabled = true;
}

void PSTaskReminder::Add(std::string& sUID, int iHour, int iMinute)
{
	PSTaskReminder_TaskItem tiTime;
	tiTime.sUID = sUID;
	tiTime.iHour = iHour;
	tiTime.iMinute = iMinute;
	vItems.push_back(tiTime);
}

void PSTaskReminder::ResetForNewDay()
{
	vUID_ViewedBeforeTerm.clear();
	vUID_ViewedAtTerm.clear();
}

std::string PSTaskReminder::TaskUidCheck(SYSTEMTIME& stNow, int iMinutesBefore, int* iRange)
{
	*iRange = 10;
	for (int i = 0; i < (int)vItems.size(); i++)
	{
		PSTaskReminder_TaskItem& tItem = vItems[i];
		if (tItem.TimeEqual(stNow.wHour, stNow.wMinute))
		{
			*iRange = 0;
			return tItem.sUID;
		}
		if (tItem.TimeInRange(stNow, iMinutesBefore))
		{
			*iRange = -1;
			return tItem.sUID; 
		}
		if (tItem.TimeInRangeAndAfter(stNow, iMinutesBefore))
		{
			*iRange = 1;
			return tItem.sUID;
		}
	}
	return "";
}

bool PSTaskReminder::ItemExists(std::vector<std::string>& vItems, std::string& sUID)
{
	for (int i = 0; i < (int)vItems.size(); i++)
		if (vItems[i] == sUID)
			return true;
	return false;
}

void PSTaskReminder::SetTaskAsViewed(std::string& sUID, bool bAtTerm)
{
	if (!ItemExists(vUID_ViewedBeforeTerm, sUID))
		vUID_ViewedBeforeTerm.push_back(sUID);
	if (bAtTerm)
		if (!ItemExists(vUID_ViewedAtTerm, sUID))
			vUID_ViewedAtTerm.push_back(sUID);
}

void PSTaskReminder::ClearTaskAsNotViewed(std::string& sUID)
{
	for (vecUIDs::iterator idx = vUID_ViewedBeforeTerm.begin(); idx != vUID_ViewedBeforeTerm.end(); idx++)
		if (*idx == sUID)
		{
			vUID_ViewedBeforeTerm.erase(idx);
			break;
		}
	for (vecUIDs::iterator idx = vUID_ViewedAtTerm.begin(); idx != vUID_ViewedAtTerm.end(); idx++)
		if (*idx == sUID)
		{
			vUID_ViewedAtTerm.erase(idx);
			break;
		}
}

std::string PSTaskReminder::GetTaskUIDToView()
{
	if (!bCheckEnabled)
		return "";

	static SYSTEMTIME	stNow = { 0, 0, 0, 0, 0, 0, 0, 0 };
	GetLocalTime(&stNow);

	int iRange = 10;
	std::string sUID = TaskUidCheck(stNow, iMinutesBeforeRemind, &iRange);
	if (!sUID.empty())
	{
		if ((iRange < 0) && (!ItemExists(vUID_ViewedBeforeTerm, sUID)))
			return sUID;
		if ((iRange >= 0) && (!ItemExists(vUID_ViewedAtTerm, sUID)))
		{
			SetTaskAsViewed(sUID, true);
			return sUID;
		}
	}

	return "";
}

