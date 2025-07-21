/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLStrengthenTicketSynthesisDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

StrengthenTicketSynthesisDataEntry::StrengthenTicketSynthesisDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

StrengthenTicketSynthesisDataEntry::~StrengthenTicketSynthesisDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 StrengthenTicketSynthesisDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool StrengthenTicketSynthesisDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	strengthenLv = row.ReadUInt32();
	binding = row.ReadUInt32() + 1;
	amplitude = row.ReadUInt32();
	costMaterial = row.ReadString();
	lower = row.ReadUInt32();
	upper = row.ReadUInt32();
	mu = row.ReadUInt32();
	sigmal = row.ReadUInt32();
	synthProbOfPerfect = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(costMaterial, strs, "|");

	for (auto& elem : strs)
	{
		std::vector<std::string> paramStr;
		Avalon::StringUtil::Split(elem, paramStr, "_");

		if (paramStr.size() != 2)
		{
			ErrorLogStream << "Error reward's format(" << costMaterial << ")!" << LogStream::eos;
			return false;
		}

		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[0].c_str());
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[1].c_str());

		costMaterials.push_back(reward);
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool StrengthenTicketSynthesisDataEntryMgr::AddEntry(StrengthenTicketSynthesisDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<StrengthenTicketSynthesisDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

