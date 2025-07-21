/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLUltimateChallengeDungeonDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

UltimateChallengeDungeonDataEntry::UltimateChallengeDungeonDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

UltimateChallengeDungeonDataEntry::~UltimateChallengeDungeonDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 UltimateChallengeDungeonDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return dungeonID;
// __CUSTOM_GETKEY_END__
}

bool UltimateChallengeDungeonDataEntry::Read(Avalon::DataRow &row)
{
	level = row.ReadUInt32();
	dungeonID = row.ReadUInt32();
	randomBuff = row.ReadString();
	limitTime = row.ReadUInt32();
	expendItem = row.ReadString();
	courageBuff = row.ReadString();

// __CUSTOM_READ_BEGIN__
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(randomBuff, strs, "|");

		if (strs.size() == 0)
		{
			return false;
		}

		for (size_t i = 0; i < strs.size(); ++i)
		{
			BuffRandomInfo info;
			std::vector<std::string> pramas;
			Avalon::StringUtil::Split(strs[i], pramas, "_");

			if (pramas.size() != 2)
			{
				return false;
			}

			info.buffId = Avalon::StringUtil::ConvertToValue<UInt32>(pramas[0].c_str());
			info.weight = Avalon::StringUtil::ConvertToValue<UInt32>(pramas[1].c_str());

			buffRandomInfoVec.push_back(info);
		}
	}

	{
		std::vector<std::string> pramas;
		Avalon::StringUtil::Split(expendItem, pramas, "_");

		if (pramas.size() != 2)
		{
			return false;
		}

		dungeonBuffRefreshMat.matId = Avalon::StringUtil::ConvertToValue<UInt32>(pramas[0].c_str());
		dungeonBuffRefreshMat.num = Avalon::StringUtil::ConvertToValue<UInt32>(pramas[1].c_str());
	}

	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(courageBuff, strs, "|");

		if (strs.size() == 0)
		{
			return false;
		}

		for (size_t i = 0; i < strs.size(); ++i)
		{
			InspireBuffInfo info;
			std::vector<std::string> pramas;
			Avalon::StringUtil::Split(strs[i], pramas, "_");

			if (pramas.size() != 2)
			{
				return false;
			}

			info.day = Avalon::StringUtil::ConvertToValue<UInt32>(pramas[0].c_str());
			info.buffId = Avalon::StringUtil::ConvertToValue<UInt32>(pramas[1].c_str());

			inspireBuffInfoVec.push_back(info);
		}
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
UInt32 UltimateChallengeDungeonDataEntry::RandExceptBuffId(UInt32 buffId)
{
	Int32 totalWeight = 0;
	for (size_t i = 0; i < buffRandomInfoVec.size(); ++i)
	{
		if (buffId == buffRandomInfoVec[i].buffId)
		{
			continue;
		}
		
		totalWeight += buffRandomInfoVec[i].weight;
	}

	if (totalWeight <= 0)
	{
		return buffId;
	}
	
	Int32 randWeight = Avalon::Random::RandBetween(1, totalWeight);
	Int32 sumWeight = 0;
	for (size_t i = 0; i < buffRandomInfoVec.size(); ++i)
	{
		if (buffId == buffRandomInfoVec[i].buffId)
		{
			continue;
		}

		sumWeight += buffRandomInfoVec[i].weight;
		if (randWeight <= sumWeight)
		{
			return buffRandomInfoVec[i].buffId;
		}
	}

	return buffId;
}

UInt32 UltimateChallengeDungeonDataEntry::GetNextInspireBuffId(UInt32 day)
{
	for (size_t i = 0; i < inspireBuffInfoVec.size(); ++i)
	{
		if ((day+1) == inspireBuffInfoVec[i].day)
		{
			return inspireBuffInfoVec[i].buffId;
		}
	}
	
	return 0;
}

// __CUSTOM_ENTRYFUNCTION_END__

bool UltimateChallengeDungeonDataEntryMgr::AddEntry(UltimateChallengeDungeonDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<UltimateChallengeDungeonDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
	if (topLevel < entry->level)
	{
		topLevel = entry->level;
	}
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
UltimateChallengeDungeonDataEntryMgr::UltimateChallengeDungeonDataEntryMgr()
{
	topLevel = 0;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

