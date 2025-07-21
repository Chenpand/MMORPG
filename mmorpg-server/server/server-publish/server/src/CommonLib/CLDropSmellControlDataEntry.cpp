/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLDropSmellControlDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__

#include "CLItemTypeDefine.h"

// __CUSTOM_HEADER_END__

DropSmellControlDataEntry::DropSmellControlDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

DropSmellControlDataEntry::~DropSmellControlDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 DropSmellControlDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool DropSmellControlDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt32();
	lv = row.ReadUInt32();
	color = row.ReadUInt32();
	color2 = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				dungeonSubType.push_back(value);
			}
		}
	}
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				dungeonHard.push_back(value);
			}
		}
	}
	dungeonID = row.ReadUInt32();
	probability = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
enum SmellType
{
	SMELLTYPE_INVAILD = 0,
	SMELLTYPE_DUNGEON = 1,
	SMELLTYPE_JAR_BEGIN = 100,
	SMELLTYPE_JAR_END = 200,
};
// __CUSTOM_ENTRYFUNCTION_END__

bool DropSmellControlDataEntryMgr::AddEntry(DropSmellControlDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<DropSmellControlDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	if (entry->type == SMELLTYPE_DUNGEON)
	{
		// 根据地下城类型索引
		{
			for (auto subType : entry->dungeonSubType)
			{
				for (auto hard : entry->dungeonHard)
				{
					auto key = _MakeKey(entry->lv, entry->color, entry->color2, subType, hard, 0);
					if (m_DataByDungeonType.find(key) != m_DataByDungeonType.end())
					{
						ErrorLogStream << "dungeon smell data repeat config(" << entry->id << ") with oter(" << m_DataByDungeonType[key]->id << ")" << LogStream::eos;
						return false;
					}
					m_DataByDungeonType[key] = entry;
				}
			}
		}

		// 根据地下城ID索引
		if (entry->dungeonID > 0)
		{
			auto key = _MakeKey(entry->lv, entry->color, entry->color2, 0, 0, entry->dungeonID);
			m_DataByDungeonID[key] = entry;
		}
	}
	else if (entry->type != SMELLTYPE_INVAILD)
	{
		m_Data[SmellControlKey{ entry->type, entry->lv, entry->color, entry->color2 }] = entry;
	}


// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

UInt8 DropSmellControlDataEntryMgr::RandEquipType(UInt32 lv, UInt32 color, UInt32 color2, UInt32 dungeonSubType, UInt32 dungeonHard, UInt32 dungeonID)
{
	// 先根据地下城ID查询
	do {
		auto key = _MakeKey(lv, color, color2, 0, 0, dungeonID);
		auto itr = m_DataByDungeonID.find(key);
		if (itr == m_DataByDungeonID.end())
		{
			break;
		}

		auto data = itr->second;
		if (data == NULL)
		{
			break;
		}

		return Avalon::Random::SelectInTenThousand(data->probability) ? EQUIP_HAVE_SMELL : EQUIP_NORMAL;
	} while(0);

	// 先根据地下城类型查询
	do {
		auto key = _MakeKey(lv, color, color2, dungeonSubType, dungeonHard, 0);
		auto itr = m_DataByDungeonType.find(key);
		if (itr == m_DataByDungeonType.end())
		{
			break;
		}

		auto data = itr->second;
		if (data == NULL)
		{
			break;
		}

		return Avalon::Random::SelectInTenThousand(data->probability) ? EQUIP_HAVE_SMELL : EQUIP_NORMAL;
	} while (0);

	return EQUIP_NORMAL;
}


UInt8 DropSmellControlDataEntryMgr::CommonRandEquipType(UInt32 type, UInt32 lv, UInt32 color, UInt32 color2)
{
	auto itr = m_Data.find(SmellControlKey{ type, lv, color, color2 });
	if (itr == m_Data.end())
	{
		return EQUIP_NORMAL;
	}

	auto data = itr->second;
	if (data == NULL)
	{
		return EQUIP_NORMAL;
	}

	return Avalon::Random::SelectInTenThousand(data->probability) ? EQUIP_HAVE_SMELL : EQUIP_NORMAL;
}


DropSmellControlKey DropSmellControlDataEntryMgr::_MakeKey(UInt32 lv, UInt32 color, UInt32 color2, UInt32 dungeonSubType, UInt32 dungeonHard, UInt32 dungeonID)
{
	DropSmellControlKey key;
	key.lv = lv;
	key.color = color;
	key.color2 = color2;
	key.dungeonSubType = dungeonSubType;
	key.dungeonHard = dungeonHard;
	key.dungeonID = dungeonID;

	return key;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

