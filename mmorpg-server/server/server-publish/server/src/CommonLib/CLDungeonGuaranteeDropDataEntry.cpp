#include "CLDungeonGuaranteeDropDataEntry.h"

bool DungeonGuaranteeDropDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	dungeonId = row.ReadUInt32();
	minTimes = row.ReadUInt32();
	maxTimes = row.ReadUInt32();
	dropSetId = row.ReadUInt32();
	resetType = row.ReadUInt32();
	vipLevelLimitMin = 0;
	vipLevelLimitMax = 0;

	std::string vipLevelLimitStr = row.ReadString();
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(vipLevelLimitStr, strs, ",");
	if (strs.size() == 2)
	{
		vipLevelLimitMin = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
		vipLevelLimitMax = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
	}

	return true;
}

std::vector<DungeonGuaranteeDropDataEntry*> DungeonGuaranteeDropDataEntryMgr::GetDataEntryByDungeonID(UInt32 dungeonID)
{
	class DungeonDropVistor : public Avalon::DataEntryVisitor<DungeonGuaranteeDropDataEntry>
	{
	public:
		DungeonDropVistor(UInt32 dungeonID) : dungeonID(dungeonID) {}
		bool operator()(DungeonGuaranteeDropDataEntry* dataEntry)
		{
			if (dataEntry != NULL && dataEntry->dungeonId == dungeonID)
			{
				datas.push_back(dataEntry);
			}
			return true;
		}

		std::vector<DungeonGuaranteeDropDataEntry*> datas;
		UInt32 dungeonID;
	};

	DungeonDropVistor vistor(dungeonID);
	DungeonGuaranteeDropDataEntryMgr::Instance()->Visit(vistor);

	return vistor.datas;
}
