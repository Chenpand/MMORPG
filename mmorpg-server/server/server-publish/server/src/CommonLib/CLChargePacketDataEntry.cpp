#include "CLChargePacketDataEntry.h"

bool ChargePacketDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt8();
	sort = row.ReadUInt32();
	name = row.ReadString();
	oldPrice = row.ReadUInt16();
	money = row.ReadUInt16();
	vipScore = row.ReadUInt16();
	startTime = row.ReadUInt32();
	endTime = row.ReadUInt32();
	limitDailyNum = row.ReadUInt16();
	limitTotalNum = row.ReadUInt16();

	std::string rewardsStr = row.ReadString();
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(rewardsStr, strs, "|");
	for (auto rewardStr : strs)
	{
		std::vector<std::string> tmpStrs;
		Avalon::StringUtil::Split(rewardStr, tmpStrs, ":");
		if (tmpStrs.size() < 2)
		{
			return false;
		}

		UInt32 itemId = Avalon::StringUtil::ConvertToValue<UInt32>(tmpStrs[0]);
		UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(tmpStrs[1]);
		UInt8 strength = 0;
		if (tmpStrs.size() > 2)
		{
			strength = Avalon::StringUtil::ConvertToValue<UInt8>(tmpStrs[2]);
		}
		rewards.push_back(ItemReward(itemId, num, 1, strength));
	}

	if (rewards.empty())
	{
		return false;
	}

	icon = row.ReadString();

	return true;
}

