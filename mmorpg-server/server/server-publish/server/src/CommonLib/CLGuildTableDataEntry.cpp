#include "CLGuildTableDataEntry.h"

bool GuildTableDataEntry::Read(Avalon::DataRow& row)
{
	type = row.ReadUInt8();
	maxTimes = row.ReadUInt32();
	std::string awardStr = row.ReadString();
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(awardStr, strs, "|");
	for (auto str : strs)
	{
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, ",");
		if (params.size() != 2)
		{
			return false;
		}

		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
		rewards.push_back(reward);
	}
	mailContentId = row.ReadUInt32();
	return true;
}