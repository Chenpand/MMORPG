#include <CLLostDungeonTreasureChestDataEntry.h>

LostDungeonTreasureDataEntry::LostDungeonTreasureDataEntry()
{
}

LostDungeonTreasureDataEntry::~LostDungeonTreasureDataEntry()
{

}

bool LostDungeonTreasureDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt8();
	{
		std::string ss = row.ReadString();
		std::vector<std::string> ssv;
		Avalon::StringUtil::Split(ss, ssv, "|");
		for (auto s : ssv)
		{
			std::vector<std::string> sv;
			Avalon::StringUtil::Split(s, sv, ",");
			if (sv.size() != 2)
			{
				continue;
			}
			GiftWeight gift;
			gift.dataId = Avalon::StringUtil::ConvertToValue<UInt32>(sv[0]);
			gift.weight = Avalon::StringUtil::ConvertToValue<UInt32>(sv[1]);
			
			gifts.push_back(gift);
		}
	}
	return true;
}

LostDungeonTreasureDataEntryMgr::LostDungeonTreasureDataEntryMgr()
{
}

LostDungeonTreasureDataEntryMgr::~LostDungeonTreasureDataEntryMgr()
{

}