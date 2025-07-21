#include "CLDungeonTimesDataEntry.h"

bool DungeonTimesDataEntry::Read(Avalon::DataRow& row)
{
	type = (DungeonSubType)row.ReadUInt32();
	baseTimes = row.ReadUInt8();
	baseBuyTimes = row.ReadUInt8();
	buyTimesVipPrivilege = (VipPrivilegeType)row.ReadUInt32();
	buyTimesCostItemId = row.ReadUInt32();
	{
		auto costStr = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(costStr, strs, "|");
		for (auto str : strs)
		{
			UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(str);
			buyTimesCosts.push_back(num);
		}
	}
	usedTimesCounter = row.ReadString();
	buyTimesCounter = row.ReadString();
	weekUsedTimesCounter = row.ReadString();
	weekTimesLimit = row.ReadInt32();
	accountDailyTimesLimit = row.ReadInt32();
	return true;
}