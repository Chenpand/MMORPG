/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLInscriptionExtractDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

InscriptionExtractDataEntry::InscriptionExtractDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
	matId = 0;
	matNum = 0;
	destroyMatId = 0;
	destroyMatNum = 0;
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

InscriptionExtractDataEntry::~InscriptionExtractDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 InscriptionExtractDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return color;
// __CUSTOM_GETKEY_END__
}

bool InscriptionExtractDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	color = row.ReadUInt32();
	extractItemConsume = row.ReadString();
	extractProbability = row.ReadUInt32();
	isExtract = row.ReadUInt32();
	encapsuleProbability = row.ReadInt32();
	destroyConsume = row.ReadString();
	destroyProbability = row.ReadInt32();

// __CUSTOM_READ_BEGIN__
	{
		std::vector<std::string> subPram;
		Avalon::StringUtil::Split(extractItemConsume, subPram, "_");

		if (subPram.size() == 2)
		{
			matId = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[0].c_str());
			matNum = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[1].c_str());
		}
	}
	
	{
		std::vector<std::string> subPram;
		Avalon::StringUtil::Split(destroyConsume, subPram, "_");

		if (subPram.size() == 2)
		{
			destroyMatId = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[0].c_str());
			destroyMatNum = Avalon::StringUtil::ConvertToValue<UInt32>(subPram[1].c_str());
		}
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool InscriptionExtractDataEntryMgr::AddEntry(InscriptionExtractDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<InscriptionExtractDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

