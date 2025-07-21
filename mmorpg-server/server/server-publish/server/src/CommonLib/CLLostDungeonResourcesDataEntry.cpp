#include "CLLostDungeonResourcesDataEntry.h"

LostDungeonResDataEntry::LostDungeonResDataEntry()
{}

LostDungeonResDataEntry::~LostDungeonResDataEntry()
{}

bool LostDungeonResDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	resGroupId = row.ReadUInt32();
	{
		std::string ss = row.ReadString();
		std::vector<std::string> ssv;
		Avalon::StringUtil::Split(ss, ssv, "|");
		for (auto s : ssv)
		{
			items.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(s));
		}
	}
	initRefreshTime = row.ReadUInt32();
	intervalRefreshTime = row.ReadUInt32();
	mapId = row.ReadUInt32();
	return true;
}

bool LostDungeonResDataEntryMgr::AddEntry(LostDungeonResDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<LostDungeonResDataEntry>::AddEntry(dataEntry)) return false;

	auto it = m_datas.find(dataEntry->mapId);
	if (it == m_datas.end())
	{
		std::vector<LostDungeonResDataEntry*> tempv;
		tempv.push_back(dataEntry);
		m_datas.insert(std::make_pair(dataEntry->mapId, tempv));
	}
	else
	{
		it->second.push_back(dataEntry);
	}
	return true;
}

std::vector<LostDungeonResDataEntry*>& LostDungeonResDataEntryMgr::GetDatasByMapId(UInt32 mapId)
{
	static std::vector<LostDungeonResDataEntry*> temp;
	auto it = m_datas.find(mapId);
	if (it == m_datas.end())
	{
		return temp;
	}
	else
	{
		return it->second;
	}
}