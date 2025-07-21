/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLEquipInscriptionHoleDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

EquipInscriptionHoleDataEntry::EquipInscriptionHoleDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
	matId = 0;
	matNum = 0;
	moneyId = 0;
	moneyNum = 0;
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

EquipInscriptionHoleDataEntry::~EquipInscriptionHoleDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 EquipInscriptionHoleDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return MakeKey(color, color2, subType);
// __CUSTOM_GETKEY_END__
}

bool EquipInscriptionHoleDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	color = row.ReadUInt32();
	color2 = row.ReadUInt32();
	subType = row.ReadUInt32();
	inscriptionHoleNum = row.ReadUInt32();
	inscriptionHoleColor = row.ReadString();
	itemConsume = row.ReadString();
	goldConsume = row.ReadString();

// __CUSTOM_READ_BEGIN__
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(inscriptionHoleColor, strs, "|");
		if (strs.size() == 0)
		{
			return false;
		}

		for (size_t i = 0; i < strs.size(); ++i)
		{
			HoleDataConfig holeInfo;

			std::vector<std::string> subPram;
			Avalon::StringUtil::Split(strs[i], subPram, "_");

			if (subPram.size() != 2)
			{
				return false;
			}

			holeInfo.index = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[0].c_str());
			holeInfo.type = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[1].c_str());

			holeVec.push_back(holeInfo);
		}
	}

	if (itemConsume != "")
	{
		std::vector<std::string> subPram;
		Avalon::StringUtil::Split(itemConsume, subPram, "_");

		if (subPram.size() != 2)
		{
			return false;
		}

		matId = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[0].c_str());
		matNum = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[1].c_str());
	}

	if (goldConsume != "")
	{
		std::vector<std::string> subPram;
		Avalon::StringUtil::Split(goldConsume, subPram, "_");

		if (subPram.size() != 2)
		{
			return false;
		}

		moneyId = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[0].c_str());
		moneyNum = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[1].c_str());
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
UInt32 EquipInscriptionHoleDataEntry::MakeKey(UInt32 color, UInt32 color2, UInt32 subType)
{
	UInt32 key = (color << 24) + (color2 << 16) + subType;
	return key;
}
// __CUSTOM_ENTRYFUNCTION_END__

bool EquipInscriptionHoleDataEntryMgr::AddEntry(EquipInscriptionHoleDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<EquipInscriptionHoleDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

