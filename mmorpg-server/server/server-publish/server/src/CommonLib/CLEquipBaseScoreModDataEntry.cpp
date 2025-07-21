/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLEquipBaseScoreModDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

EquipBaseScoreModDataEntry::EquipBaseScoreModDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

EquipBaseScoreModDataEntry::~EquipBaseScoreModDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 EquipBaseScoreModDataEntry::GetKey() const
{
	return id;
// __CUSTOM_GETKEY_BEGIN__
// __CUSTOM_GETKEY_END__
}

bool EquipBaseScoreModDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	itemSubType = row.ReadUInt8();
	itemQuality = row.ReadUInt8();
	attrMod1 = row.ReadUInt32();
	attrMod2 = row.ReadUInt32();
	powerMod1 = row.ReadInt32();
	powerMod2 = row.ReadInt32();
	powerMod3 = row.ReadInt32();
	defMod1 = row.ReadInt32();
	defMod2 = row.ReadInt32();
	strenthQualityMod1 = row.ReadInt32();
	strenthQualityMod2 = row.ReadInt32();

	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				strengthMod1.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(param));
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
				strengthMod2.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(param));
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
				strengthMod3.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(param));
			}
		}
	}

// __CUSTOM_READ_BEGIN__

	for (auto param : strengthMod3 )
	{
		if (param >= 1000)
		{
			ErrorLogStream << "strengthMod3 data is err id : " << GetKey() << LogStream::eos;
			return false;
		}
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool EquipBaseScoreModDataEntryMgr::AddEntry(EquipBaseScoreModDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<EquipBaseScoreModDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
	EquipBaseScoreModEntryKey key(entry->itemSubType, entry->itemQuality);
	std::pair<std::map<EquipBaseScoreModEntryKey, EquipBaseScoreModDataEntry*>::iterator, bool> ret;
	ret = m_Entrys.insert(std::pair<EquipBaseScoreModEntryKey, EquipBaseScoreModDataEntry*>(key, entry));
	if (ret.second == false)
	{
		ErrorLogStream << "load entry " << LogStream::eos;
		return false;
	}
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

EquipBaseScoreModDataEntry*	EquipBaseScoreModDataEntryMgr::GetEntry(UInt8 subType, UInt8 quailty)
{
	EquipBaseScoreModEntryKey key(subType, quailty);
	auto iter = m_Entrys.find(key);
	if (iter == m_Entrys.end())
	{
		return NULL;
	}
	return iter->second;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

