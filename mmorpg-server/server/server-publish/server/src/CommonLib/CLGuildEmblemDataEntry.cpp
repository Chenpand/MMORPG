/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLGuildEmblemDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

GuildEmblemDataEntry::GuildEmblemDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

GuildEmblemDataEntry::~GuildEmblemDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 GuildEmblemDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool GuildEmblemDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	needHonourLevel = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				if (value > 0) costId.push_back(value);
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
				if (value > 0) costNum.push_back(value);
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
				if (value > 0) useEqual.push_back(value);
			}
		}
	}
	name = row.ReadString();
	stageLevel = row.ReadUInt32();
	titleId = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__

	if (costId.size() != costNum.size())
	{
		ErrorLogStream << "costId size not equal costNum size id:"<< id << LogStream::eos;
		return false;
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool GuildEmblemDataEntryMgr::AddEntry(GuildEmblemDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildEmblemDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
	//获得每个荣耀殿堂等级的最高阶
	if (emblemDataEntryByHonourLevel.count(entry->needHonourLevel) == 0)
	{
		emblemDataEntryByHonourLevel[entry->needHonourLevel] = entry;
	}
	if (emblemDataEntryByHonourLevel[entry->needHonourLevel]->stageLevel < entry->stageLevel)
	{
		emblemDataEntryByHonourLevel[entry->needHonourLevel] = entry;
	}
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
GuildEmblemDataEntry* GuildEmblemDataEntryMgr::GetMaxStageEmblemDataEntry(UInt32 level)
{
	if (emblemDataEntryByHonourLevel.count(level) == 0)
	{
		return NULL;
	}
	return emblemDataEntryByHonourLevel[level];
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

