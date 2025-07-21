#include "CLFunctionDataEntry.h"

FuncNotifyDataEntry::FuncNotifyDataEntry()
{
}

FuncNotifyDataEntry::~FuncNotifyDataEntry()
{

}

bool FuncNotifyDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt8();
	type = (UnlockType)row.ReadInt32();
	beginLevel = row.ReadUInt16();
	endLevel = row.ReadUInt16();
	beginTask = row.ReadUInt32();
	endTask = row.ReadUInt32();

	std::string split = "|";
	std::string split1 = "_";
	std::string strReward = row.ReadString();
	std::vector<std::string> rewardVec;
	Avalon::StringUtil::Split(strReward, rewardVec, split);
	for (UInt32 i = 0; i < rewardVec.size(); ++i)
	{
		std::vector<std::string> tmpVec;
		Avalon::StringUtil::Split(rewardVec[i], tmpVec, split1);
		if (tmpVec.size() < 3) continue;
		OccuItemReward reward;
		reward.occu = (Occupation)Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[0]);
		reward.itemReward.id = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[1]);
		reward.itemReward.num = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[2]);
		rewards.push_back(reward);
	}
	return true;
}

bool FuncNotifyDataEntry::GetRewards(const Occupation occu, std::vector<ItemReward>& itemRewards)
{
	bool result = false;
	for (UInt32 i = 0; i < rewards.size(); ++i)
	{
		if (rewards[i].occu == occu || rewards[i].occu == OCCU_NONE)
		{
			itemRewards.push_back(rewards[i].itemReward);
			result = true;
		}
	}
	return result;
}
