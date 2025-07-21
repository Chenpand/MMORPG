#include "CLLostDungeonDataEntry.h"

LostDungeonFloorDataEntry::LostDungeonFloorDataEntry()
{}

LostDungeonFloorDataEntry::~LostDungeonFloorDataEntry()
{
}

bool LostDungeonFloorDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	floor = row.ReadInt32();

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
			FloorBattleWeight st;
			st.battleDataId = Avalon::StringUtil::ConvertToValue<UInt32>(sv[0]);
			st.weight = Avalon::StringUtil::ConvertToValue<UInt32>(sv[1]);
			
			battlesWt.push_back(st);
		}
	}

	failHandlerId = row.ReadUInt8();
	failDecScore = row.ReadUInt32();
	existMysticShop = row.ReadUInt8();
	nodeNum = row.ReadUInt8();

	{
		std::string ss = row.ReadString();
		std::vector<std::string> ssv;
		Avalon::StringUtil::Split(ss, ssv, ",");
		if (ssv.size() == 4)
		{
			hardBattleGenInfo.num = Avalon::StringUtil::ConvertToValue<UInt8>(ssv[0]);
			hardBattleGenInfo.pro = Avalon::StringUtil::ConvertToValue<UInt32>(ssv[1]);
			hardBattleGenInfo.groupId = Avalon::StringUtil::ConvertToValue<UInt8>(ssv[2]);
			hardBattleGenInfo.groupMaxNum = Avalon::StringUtil::ConvertToValue<UInt8>(ssv[3]);
		}
	}

	{
		std::string ss = row.ReadString();
		std::vector<std::string> ssv;
		Avalon::StringUtil::Split(ss, ssv, ",");
		if (ssv.size() == 4)
		{
			treasChestGenInfo.num = Avalon::StringUtil::ConvertToValue<UInt8>(ssv[0]);
			treasChestGenInfo.pro = Avalon::StringUtil::ConvertToValue<UInt32>(ssv[1]);
			treasChestGenInfo.groupId = Avalon::StringUtil::ConvertToValue<UInt8>(ssv[2]);
			treasChestGenInfo.groupMaxNum = Avalon::StringUtil::ConvertToValue<UInt8>(ssv[3]);
		}
	}

	{
		std::string ss = row.ReadString();
		std::vector<std::string> ssv;
		Avalon::StringUtil::Split(ss, ssv, "|");

		for (auto s : ssv)
		{
			std::vector<std::string> sv;
			Avalon::StringUtil::Split(s, sv, ",");
			TreasChestWeight st;
			st.dataId = Avalon::StringUtil::ConvertToValue<UInt32>(sv[0]);
			st.weight = Avalon::StringUtil::ConvertToValue<UInt32>(sv[1]);
			if (sv.size() != 2)
			{
				continue;
			}
			treasChestWt.push_back(st);
		}
	}

	notFallItemId = row.ReadUInt32();
	againItemId = row.ReadUInt32();
	return true;
}

LostDungeonFloorDataEntryMgr::LostDungeonFloorDataEntryMgr()
{
	m_maxFloor = 0;
}

LostDungeonFloorDataEntryMgr::~LostDungeonFloorDataEntryMgr()
{

}

bool LostDungeonFloorDataEntryMgr::AddEntry(LostDungeonFloorDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<LostDungeonFloorDataEntry>::AddEntry(dataEntry)) return false;
	
	auto it = m_datas.find(dataEntry->floor);
	if (it != m_datas.end())
	{
		return false;
	}
	m_datas.insert(std::make_pair(dataEntry->floor, dataEntry));

	if (dataEntry->floor > m_maxFloor)
	{
		m_maxFloor = dataEntry->floor;
	}
	return true;
}

UInt32  LostDungeonFloorDataEntryMgr::GetMaxFloor()
{
	return m_maxFloor;
}

LostDungeonFloorDataEntry* LostDungeonFloorDataEntryMgr::GetDataEntry(UInt32 floor)
{
	auto it = m_datas.find(floor);
	if (it == m_datas.end())
	{
		return NULL;
	}

	return it->second;
}

LostDungeonElementDataEntry::LostDungeonElementDataEntry()
{
}

LostDungeonElementDataEntry::~LostDungeonElementDataEntry()
{
}

bool LostDungeonElementDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt8();
	param = row.ReadString();
	return true;
}

LostDungeonElementDataEntryMgr::LostDungeonElementDataEntryMgr()
{
}

LostDungeonElementDataEntryMgr::~LostDungeonElementDataEntryMgr()
{

}

bool LostDungeonElementDataEntryMgr::AddEntry(LostDungeonElementDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<LostDungeonElementDataEntry>::AddEntry(dataEntry)) return false;

	m_typeMapId[dataEntry->type] = dataEntry->id;
	return true;
}

UInt32 LostDungeonElementDataEntryMgr::GetElementId(UInt8 elemetType)
{
	return m_typeMapId[elemetType];
}