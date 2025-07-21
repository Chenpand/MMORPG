/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLDropStrengthenControlDataEntry.h"
#include <json/reader.h>
//#include <CLLogger.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include "CLItemDataEntry.h"
// __CUSTOM_HEADER_END__

DropStrengthenControlDataEntry::DropStrengthenControlDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

DropStrengthenControlDataEntry::~DropStrengthenControlDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 DropStrengthenControlDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return GenKey(lv, color, color2);
// __CUSTOM_GETKEY_END__
}

bool DropStrengthenControlDataEntry::Read(Avalon::DataRow &row)
{
	lv = row.ReadUInt32();
	color = row.ReadUInt32();
	color2 = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				strengthen.push_back(value);
			}
		}
	}
	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				weight.push_back(value);
			}
		}
	}

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
UInt32 DropStrengthenControlDataEntry::GenKey(UInt32 level, UInt32 color, UInt32 color2)
{
	return ((level << 16) | (color << 8) | color2);
}
// __CUSTOM_ENTRYFUNCTION_END__

bool DropStrengthenControlDataEntryMgr::AddEntry(DropStrengthenControlDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<DropStrengthenControlDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
UInt8 DropStrengthenControlDataEntryMgr::RandomItemStrenth(UInt32 itemId)
{
	ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(itemId);
	if (!itemEntry || itemEntry->type != ITEM_EQUIP)
	{
		return 0;
	}

	UInt32 tmpKey = DropStrengthenControlDataEntry::GenKey(itemEntry->needLevel, itemEntry->color, itemEntry->color2);
	auto controlDataEntry = FindEntry(tmpKey);
	if (!controlDataEntry || controlDataEntry->strengthen.size() != controlDataEntry->weight.size())
	{
		return 0;
	}

	UInt32 totalWeight = 0;
	for (size_t i = 0; i < controlDataEntry->weight.size(); ++i)
	{
		totalWeight += controlDataEntry->weight[i];
	}

	UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);
	for (size_t i = 0; i < controlDataEntry->weight.size(); ++i)
	{
		if (controlDataEntry->weight[i] >= randWeight)
		{
			return controlDataEntry->strengthen[i];
		}
		else
		{
			randWeight -= controlDataEntry->weight[i];
		}
	}

	return 0;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

