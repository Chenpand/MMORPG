/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLEquieUpdateDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

EquieUpdateDataEntry::EquieUpdateDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
	equieID = 0;
	jobID = 0;
	nextLvEquieID = 0;
	announceID = 0;
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

EquieUpdateDataEntry::~EquieUpdateDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 EquieUpdateDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return equieID*100 + jobID;
// __CUSTOM_GETKEY_END__
}

bool EquieUpdateDataEntry::Read(Avalon::DataRow &row)
{
	equieID = row.ReadUInt32();
	jobID = row.ReadUInt32();
	nextLvEquieID = row.ReadUInt32();
	materialConsume = row.ReadString();
	increaseMaterialConsume = row.ReadString();
	announceID = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(materialConsume, strs, "|");

		if (strs.size() == 0)
		{
			return false;
		}

		for (size_t i = 0; i < strs.size(); ++i)
		{
			MatInfoLimit infoLimit;

			std::vector<std::string> subPram;
			Avalon::StringUtil::Split(strs[i], subPram, ",");

			if (subPram.size() < 3)
			{
				return false;
			}

			infoLimit.minStrenthLevel = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[0].c_str());
			infoLimit.maxStrenthLevel = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[1].c_str());

			for (size_t j = 2; j < subPram.size(); ++j)
			{
				MatInfo info;
				std::vector<std::string> pram;
				Avalon::StringUtil::Split(subPram[j], pram, "_");

				if (pram.size() != 2)
				{
					return false;
				}

				info.matId = Avalon::StringUtil::ConvertToValue<UInt32>(pram[0].c_str());
				info.num = Avalon::StringUtil::ConvertToValue<UInt32>(pram[1].c_str());

				infoLimit.needMats.push_back(info);
			}

			needMatLimits.push_back(infoLimit);
		}
	}

	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(increaseMaterialConsume, strs, "|");

		if (strs.size() == 0)
		{
			return false;
		}

		for (size_t i = 0; i < strs.size(); ++i)
		{
			MatInfoLimit infoLimit;

			std::vector<std::string> subPram;
			Avalon::StringUtil::Split(strs[i], subPram, ",");

			if (subPram.size() < 3)
			{
				return false;
			}

			infoLimit.minStrenthLevel = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[0].c_str());
			infoLimit.maxStrenthLevel = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[1].c_str());

			for (size_t j = 2; j < subPram.size(); ++j)
			{
				MatInfo info;
				std::vector<std::string> pram;
				Avalon::StringUtil::Split(subPram[j], pram, "_");

				if (pram.size() != 2)
				{
					return false;
				}

				info.matId = Avalon::StringUtil::ConvertToValue<UInt32>(pram[0].c_str());
				info.num = Avalon::StringUtil::ConvertToValue<UInt32>(pram[1].c_str());

				infoLimit.needMats.push_back(info);
			}

			increaseNeedMatLimits.push_back(infoLimit);
		}
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool EquieUpdateDataEntryMgr::AddEntry(EquieUpdateDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<EquieUpdateDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

