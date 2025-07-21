#include "CLNpcDataEntry.h"

bool NpcDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	name = row.ReadString();
	npcType = row.ReadUInt32();
	dungeonId = row.ReadUInt32();
	times = row.ReadUInt32();
	mustTeam = row.ReadUInt8() != 0;
	hardType = row.ReadUInt32();
	reviveTime = row.ReadUInt32();

	return true;
}

bool NpcDataEntryMgr::AddEntry(NpcDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<NpcDataEntry>::AddEntry(entry))
	{
		return false;
	}

	auto iter = m_NpcType.find(entry->npcType);
	if (iter == m_NpcType.end())
	{
		std::vector<UInt32> tempVec;
		tempVec.push_back(entry->id);
		m_NpcType[entry->npcType] = tempVec;
	}
	else
	{
		iter->second.push_back(entry->id);
	}

	return true;
}

UInt32 NpcDataEntryMgr::GetCityMonsterHardTypeByDungeonId(UInt32 dungeonId)
{
	class MyDataVisitor : public Avalon::DataEntryVisitor<NpcDataEntry>
	{
	public:
		MyDataVisitor(UInt32 dungeonId) :_dungeonId(dungeonId), _hardType(0) {}

		bool operator()(NpcDataEntry* data)
		{
			if (!data) return true;

			if (data->dungeonId == _dungeonId)
			{
				_hardType = data->hardType;
				return false;
			}

			return true;
		}

		UInt32 _dungeonId;
		UInt32 _hardType;
	};

	MyDataVisitor visitor(dungeonId);
	Visit(visitor);
	return visitor._hardType;
}

UInt32 NpcDataEntryMgr::GetRandNpcIdByType(UInt32 npcType)
{
	auto iter = m_NpcType.find(npcType);
	if (iter == m_NpcType.end())
	{
		return 0;
	}

	if (iter->second.empty())
	{
		return 0;
	}

	UInt32 randNum = Avalon::Random::RandBetween(0, iter->second.size() - 1);
	return iter->second[randNum];
}

