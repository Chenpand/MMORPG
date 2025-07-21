#include "CLDungeonHellDataEntry.h"

DungeonHellDataEntry::DungeonHellDataEntry()
{

}

DungeonHellDataEntry::~DungeonHellDataEntry()
{

}

bool DungeonHellDataEntry::Read(Avalon::DataRow& row)
{
    id = row.ReadUInt32();
	dungeonLevel = row.ReadUInt16();
    mode = (DungeonHellMode)row.ReadUInt8();

    std::string monsterGroupStr = row.ReadString();
    if (monsterGroupStr != "-")
    {
        std::vector<std::string> strs;
        Avalon::StringUtil::Split(monsterGroupStr, strs, "|");
        for (auto& str : strs)
        {
            monsterGroups.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
        }
    }
	prob = row.ReadUInt32();

	std::string dungeonIDsStr = row.ReadString();
	if (dungeonIDsStr != "")
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(dungeonIDsStr, strs, "|");
		for (auto& str : strs)
		{
			dungeonIDs.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}

	choiceType = row.ReadInt32();

    return true;
}

bool DungeonHellDataEntryMgr::AddEntry(DungeonHellDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<DungeonHellDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}

UInt32 DungeonHellDataEntryMgr::RandDungeonID(UInt16 dungeonLevel, DungeonHardType dungeonHard)
{
	class DungeonHellVistor : public Avalon::DataEntryVisitor<DungeonHellDataEntry>
	{
	public:
		DungeonHellVistor(UInt16 dungeonLevel, DungeonHardType dungeonHard) : level(dungeonLevel), hard(dungeonHard), totalProb(0) {}
		bool operator()(DungeonHellDataEntry *dataEntry)
		{
			if (!dataEntry || dataEntry->choiceType == 0)
			{
				return true;
			}

			//只有王者才有机会随机出终极困难
			if (hard < LHT_KING && dataEntry->mode > HELL_MODE_HARD)
			{
				return true;
			}

			if (dataEntry->dungeonLevel == level)
			{
				totalProb += dataEntry->prob;
				datas.push_back(dataEntry);
			}

			return true;
		}

		UInt16 level;
		DungeonHardType hard;
		UInt32 totalProb;
		std::vector<DungeonHellDataEntry*> datas;
	};

	DungeonHellVistor vistor(dungeonLevel, dungeonHard);
	Visit(vistor);

	if (vistor.datas.empty() || vistor.totalProb == 0)
	{
		ErrorLogStream << "dungeon level(" << dungeonLevel << ") gen hell mode failed." << LogStream::eos;
		return 0;
	}

	UInt32 prob = Avalon::Random::RandBetween(0, vistor.totalProb - 1);
	for (auto data : vistor.datas)
	{
		if (prob < data->prob)
		{
			if (data->dungeonIDs.size() <= 0)
			{
				return 0;
			}

			UInt32 index = Avalon::Random::RandBetween(0, data->dungeonIDs.size() - 1);
			if (index < data->dungeonIDs.size())
			{
				return data->dungeonIDs[index];
			}
			else
			{
				return 0;
			}
		}
		prob -= data->prob;
	}

	return 0;
}
