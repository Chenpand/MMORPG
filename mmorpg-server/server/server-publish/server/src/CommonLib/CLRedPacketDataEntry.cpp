#include "CLRedPacketDataEntry.h"

bool RedPacketDataEntry::Read(Avalon::DataRow& row)
{
	reason = row.ReadUInt16();
	desc = row.ReadString();
	type = row.ReadUInt8();
	totalMoney = row.ReadUInt32();
	auto str = row.ReadString();
	std::vector<std::string> params;
	Avalon::StringUtil::Split(str, params, "|");
	for (auto param : params)
	{
		UInt8 num = Avalon::StringUtil::ConvertToValue<UInt8>(param);
		if (!num)
		{
			continue;
		}

		nums.push_back(num);
	}
	if (nums.empty())
	{
		return false;
	}
	minMoney = row.ReadUInt32();
	maxMoney = row.ReadUInt32();
	costMoneyId = row.ReadUInt32();
	getMoneyId = row.ReadUInt32();
	subType = (RedPacketSubType)row.ReadUInt32();
	thrType = row.ReadUInt8();
	return true;
}

bool RedPacketDataEntry::IsValidNum(UInt8 num)
{
	return num == 0 ? true : std::find(nums.begin(), nums.end(), num) != nums.end();
}
