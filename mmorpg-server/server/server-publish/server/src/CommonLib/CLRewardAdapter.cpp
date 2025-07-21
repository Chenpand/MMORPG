#include "CLRewardAdapter.h"

RewardAdapterDataEntry::RewardAdapterDataEntry()
{
}

RewardAdapterDataEntry::~RewardAdapterDataEntry()
{
}

bool RewardAdapterDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	name = row.ReadString();
	for (UInt32 i = 1; i <= MAX_LEVEL; ++i)
	{
		std::istringstream stream(row.ReadString());
		char split = 0;
		while (stream)
		{
			ItemReward item;
			item.DecodeString(stream);
			if (item.id == 0) break;
			levelRewards[i].push_back(item);
			stream >> split;
		}
	}
	return true;
}
