#include "CLRareItemDataEntry.h"

bool RareItemDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	type = (RareControlType)row.ReadUInt8();
	valueId = row.ReadUInt32();
	minPayCost = row.ReadUInt32();
	auto timeLimitStr = row.ReadString();
	timeLimitTotalMin = 0;
	if (timeLimitStr != "-")
	{
		// 17:00-18:00|20:00-21:00
		std::vector<std::string> dayOpenTimeStrs;
		Avalon::StringUtil::Split(timeLimitStr, dayOpenTimeStrs, "|");
		for (auto& dayOpenTimeStr : dayOpenTimeStrs)
		{
			std::vector<std::string> dayTimeStrs;
			Avalon::StringUtil::Split(dayOpenTimeStr, dayTimeStrs, "-");
			if (dayTimeStrs.size() != 2)
			{
				return false;
			}

			DayOpenTime openTime;
			for (int i = 0; i < 2; i++)
			{
				std::vector<std::string> params;
				Avalon::StringUtil::Split(dayTimeStrs[i], params, ":");
				if (params.size() != 2)
				{
					return false;
				}

				DayTime dayTime;
				dayTime.hour = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
				dayTime.minute = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);

				if (i == 0)
				{
					openTime.startTime = dayTime;
				}
				else
				{
					openTime.endTime = dayTime;
				}
			}

			timeLimitTotalMin += (openTime.endTime.hour * 60 + openTime.endTime.minute) - (openTime.startTime.hour * 60 + openTime.startTime.minute);

			timeLimit.push_back(openTime);
		}
	}
	persionalDailyNum = row.ReadUInt32();
	persionalWeeklyNum = row.ReadUInt32();
	persionalMonthlyNum = row.ReadUInt32();
	persionalTotalNum = row.ReadUInt32();

	serverDailyNum = row.ReadUInt32();
	serverWeeklyNum = row.ReadUInt32();
	serverMonthlyNum = row.ReadUInt32();
	serverTotalNum = row.ReadUInt32();

	return true;
}

bool RareItemDataEntry::IsInOpenTime(const Avalon::Date& now)
{
	if (timeLimit.empty())
	{
		return true;
	}

	for (auto& openTime : timeLimit)
	{
		Avalon::Date startDate = now;
		startDate.Hour(openTime.startTime.hour);
		startDate.Min(openTime.startTime.minute);
		startDate.Sec(openTime.startTime.second);

		Avalon::Date endDate = now;
		endDate.Hour(openTime.endTime.hour);
		endDate.Min(openTime.endTime.minute);
		endDate.Sec(openTime.endTime.second);

		auto curSec = now.ToTime().Sec();
		if (curSec >= startDate.ToTime().Sec() && curSec <= endDate.ToTime().Sec())
		{
			return true;
		}
	}

	return false;
}

bool RareItemDataEntryMgr::AddEntry(RareItemDataEntry* dataEntry)
{
	if (!dataEntry)
	{
		return false;
	}

	auto key = std::make_pair(dataEntry->type, dataEntry->valueId);
	if (m_rareItems.find(key) != m_rareItems.end())
	{
		ErrorLogStream << "duplicate rare item(" << (UInt32)dataEntry->type << ", " << dataEntry->valueId << ")." << LogStream::eos;
		return false;
	}

	m_rareItems[key] = dataEntry;
	return true;
}

RareItemDataEntry* RareItemDataEntryMgr::FindEntryByType(RareControlType type, UInt32 id)
{
	auto key = std::make_pair(type, id);
	auto itr = m_rareItems.find(key);
	if (itr == m_rareItems.end())
	{
		return NULL;
	}
	return itr->second;
}