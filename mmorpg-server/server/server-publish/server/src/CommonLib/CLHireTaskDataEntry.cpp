/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLHireTaskDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

HireTaskDataEntry::HireTaskDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

HireTaskDataEntry::~HireTaskDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 HireTaskDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool HireTaskDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt32();
	fullCnt = row.ReadUInt32();

	std::string str = row.ReadString();

// __CUSTOM_READ_BEGIN__
	std::string split1 = ",";
	std::string split2 = "_";
	std::vector<std::string> stdRewardVec;
	Avalon::StringUtil::Split(str, stdRewardVec, split1);
	for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split2);

		ItemReward reward;
		if (strTmp.size() == 2)
		{
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		}
		else
		{
			continue;
		}
		rewards.push_back(reward);
	}

	refreshType = row.ReadUInt32();
	identity = row.ReadUInt32();

	str = row.ReadString();
	std::vector<std::string> stdParamVec;
	Avalon::StringUtil::Split(str, stdParamVec, "|");
	for (UInt32 i = 0; i < stdParamVec.size(); ++i)
	{
		param.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(stdParamVec[i]));
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool HireTaskDataEntryMgr::AddEntry(HireTaskDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<HireTaskDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

// __CUSTOM_MGRFUNCTIONDEFINE_END__

