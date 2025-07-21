#include "CLTeamRewardDataEntry.h"

bool TeamRewardDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	type = (TeamRewardType)row.ReadUInt32();
	rewardId = row.ReadUInt32();
	num = row.ReadUInt32();
	prob = row.ReadUInt32();
	dailyLimit = row.ReadUInt32();

	return true;
}