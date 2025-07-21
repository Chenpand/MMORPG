#include "CLDailyTodoDataEntry.h"
#include "CLDailyTodoDefine.h"
#include "CLDungeonDataEntry.h"

bool DailyTodoItemDataEntry::Read(Avalon::DataRow& row)
{
	if (row.GetColNum() != GetColNum())
	{
		ErrorLogStream <<  " row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}

	iD = row.ReadUInt32();

	type = row.ReadUInt8();

	subType = row.ReadUInt8();

	std::string strline = row.ReadString();
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(strline, strs, "|");
	for (std::vector<std::string>::iterator iter = strs.begin();
		iter != strs.end(); ++iter)
	{
		UInt32 dungeonSubType = Avalon::StringUtil::ConvertToValue<UInt32>(*iter);
		dungeonSubTypes.push_back(dungeonSubType);
	}

	dayRecommendNum = row.ReadUInt32();

	weekRecommendDayNum = row.ReadUInt32();

	refreshWeek = row.ReadUInt32();

	return true;
}

bool DailyTodoItemDataEntryMgr::AddEntry(DailyTodoItemDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<DailyTodoItemDataEntry>::AddEntry(dataEntry)) return false;
	if (dataEntry->subType == 0)
	{
		return true;
	}
	std::pair<std::map<UInt8, DailyTodoItemDataEntry*>::iterator, bool> ret;
	ret = m_DataEntrys.insert(std::pair<UInt8, DailyTodoItemDataEntry*>(dataEntry->subType, dataEntry));
	if (ret.second == false)
	{
		ErrorLogStream << "load entry data fail id = " << dataEntry->iD << LogStream::eos;
		return false;
	}

	if (dataEntry->type == 2 && dataEntry->dungeonSubTypes.size() > 0
		&& dataEntry->subType != DTSTP_COMMON_TEAM_COPY_ONE
		&& dataEntry->subType != DTSTP_DIFF_TEAM_COPY_ONE
		&& dataEntry->subType != DTSTP_COMMON_TEAM_COPY_TWO
		&& dataEntry->subType != DTSTP_DIFF_TEAM_COPY_TWO)
	{
		for (auto dungeonSubType : dataEntry->dungeonSubTypes)
		{
			std::pair<std::map<UInt8, UInt8>::iterator, bool> ret;
			ret = m_DungTpMapDailyTps.insert(std::pair<UInt8, UInt8>(dungeonSubType, dataEntry->subType));
			if (ret.second == false)
			{
				ErrorLogStream << "load entry data fail, DungeonSubType is err id = " << dataEntry->iD << LogStream::eos;
				return false;
			}
		}
	}
	
	return true;
}

UInt8 DailyTodoItemDataEntryMgr::DungeonTypeToDailyToDoSubType(const DungeonDataEntry* dungeonData)
{
	if (!dungeonData)
	{
		return DTSTP_NONE;
	}
	if (dungeonData->subType == DST_CITY_MONSTER && dungeonData->singleDungeon)
	{
		return DTSTP_NONE;
	}

	auto iter = m_DungTpMapDailyTps.find(dungeonData->subType);
	if (iter == m_DungTpMapDailyTps.end())
	{
		return DTSTP_NONE;
	}
	return iter->second;
}

DailyTodoItemDataEntry*	DailyTodoItemDataEntryMgr::GetEntryBySubType(UInt8 subType)
{
	auto iter = m_DataEntrys.find(subType);
	if (iter != m_DataEntrys.end())
	{
		return iter->second;
	}
	return NULL;
}

DailyTodoItemDataEntry* DailyTodoItemDataEntryMgr::GetEntryByDungeonSubType(const DungeonDataEntry* dungeonData)
{
	if (!dungeonData)
	{
		return NULL;
	}
	UInt8 dailyTodoSubType = DungeonTypeToDailyToDoSubType(dungeonData);
	return GetEntryBySubType(dailyTodoSubType);
}
