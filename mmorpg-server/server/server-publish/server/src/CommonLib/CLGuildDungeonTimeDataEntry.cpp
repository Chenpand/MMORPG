/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLGuildDungeonTimeDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

GuildDungeonTimeDataEntry::GuildDungeonTimeDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
	hour = 0;
	min = 0;
	sec = 0;
	timeInterval = 0;
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

GuildDungeonTimeDataEntry::~GuildDungeonTimeDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 GuildDungeonTimeDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool GuildDungeonTimeDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	week = row.ReadUInt32();
	status = row.ReadUInt32();
	time = row.ReadString();
	isOpen = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__

	std::string split = ":";
	std::vector<std::string> timeValues;
	Avalon::StringUtil::Split(time, timeValues, split);
	if (timeValues.size() >= 3)
	{
		hour = Avalon::StringUtil::ConvertToValue<UInt32>(timeValues[0]);
		min = Avalon::StringUtil::ConvertToValue<UInt32>(timeValues[1]);
		sec = Avalon::StringUtil::ConvertToValue<UInt32>(timeValues[2]);
		timeInterval = hour * 3600 + min * 60 + sec;
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool GuildDungeonTimeDataEntryMgr::AddEntry(GuildDungeonTimeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildDungeonTimeDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	// 开关关闭
	if (entry->isOpen == 0)
	{
		return true;
	}

	if (entry->timeInterval == 0)
	{
		ErrorLogStream << "timeInterval Error!" << LogStream::eos;
		return false;
	}

	auto iter = m_GuildDungeonTimeMap.find(entry->week);
	if (iter == m_GuildDungeonTimeMap.end())
	{
		TimeStatusMap tempMap;
		tempMap[entry->timeInterval] = entry->status;

		m_GuildDungeonTimeMap[entry->week] = tempMap;
	}
	else
	{
		iter->second[entry->timeInterval] = entry->status;
	}

	if (entry->status == GUILD_DUNGEON_PREPARE)
	{
		m_PrepareTimeMap.insert(std::make_pair(entry->week, entry));
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

UInt32 GuildDungeonTimeDataEntryMgr::GetCurDungeonStatus(const Avalon::Time &now, UInt64& stamp)
{
	stamp = 0;
	Avalon::Date curDate(now);
	auto iter = m_GuildDungeonTimeMap.find(curDate.WDay());
	if (iter == m_GuildDungeonTimeMap.end())
		return GUILD_DUNGEON_END;

	UInt64 day_start = GetDayBeginTime(now) / 1000;
	UInt64 cur_offset = now.Sec() - day_start;
	auto it = iter->second.lower_bound(cur_offset);
	if (it == iter->second.begin())
		return GUILD_DUNGEON_END;

	if (it == iter->second.end())
		return GUILD_DUNGEON_END;

	stamp = it->first + day_start;

	UInt32 status = it->second;
	return --status;
}

TimeStatusMap* GuildDungeonTimeDataEntryMgr::GetCurTimeStatusMap(UInt32 week)
{
	auto iter = m_GuildDungeonTimeMap.find(week);
	return iter != m_GuildDungeonTimeMap.end() ? &(iter->second) : NULL;
}

UInt64 GuildDungeonTimeDataEntryMgr::GetLastStatusTimeStamp(UInt64 time, UInt32 status)
{
	TimeStatusMap* statusMap = GetCurTimeStatusMap(CURRENT_DATE.WDay());
	if (NULL == statusMap)
	{
		return 0;
	}

	UInt64 day_start = GetDayBeginTime(time * 1000) / 1000;
	auto iter = statusMap->lower_bound(time - day_start);
	if (iter == statusMap->end())
	{
		return 0;
	}

	for (;iter != statusMap->begin();iter--)
	{
		if (iter->second == status)
		{
			return iter->first + day_start;
		}
	}

	// 特殊处理，当iter等于begin
	if (iter->second == status)
	{
		return iter->first + day_start;
	}

	return 0;
}

UInt32 GuildDungeonTimeDataEntryMgr::GetLastBattleTim(Avalon::Time now)
{
	Avalon::Date nowDate(now);
	std::vector<UInt32> timesv;
	for (auto iter : m_PrepareTimeMap)
	{
		GuildDungeonTimeDataEntry* entry = iter.second;
		if (!entry || entry->status != GUILD_DUNGEON_PREPARE)
		{
			continue;
		}

		Avalon::Date tmpDate(now);
		tmpDate.Hour(entry->hour);
		tmpDate.Min(entry->min);
		tmpDate.Sec(entry->sec);

		UInt32 tmpTime = (UInt32)tmpDate.ToTime().Sec();
		if (entry->week != (UInt32)tmpDate.WDay())
		{
			tmpTime = UInt32(tmpDate.ToTime().Sec()) - (tmpDate.WDay() - entry->week) * DAY_TO_SEC;
		}
		timesv.push_back(tmpTime);
	}

	UInt32 curr_time = (UInt32)now.Sec();
	for (size_t i = 0; i < timesv.size(); ++i)
	{
		if (i == 0)
		{
			if (curr_time < timesv[i])
			{
				return timesv[timesv.size() - 1] - 7 * DAY_TO_SEC;
			}
		}

		if (i + 1 < timesv.size())
		{
			if (curr_time >= timesv[i] && curr_time < timesv[i + 1])
			{
				return timesv[i];
			}
		}
		else
		{
			if (curr_time >= timesv[i])
			{
				return timesv[i];
			}
		}
	}

	return 0;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

