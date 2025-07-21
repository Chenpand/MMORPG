#include "CLEquipRecoScUpConsRtiDataEntryBase.h"

EquipRecoScUpConsRtiDataEntryBase::EquipRecoScUpConsRtiDataEntryBase()
{
}

EquipRecoScUpConsRtiDataEntryBase::~EquipRecoScUpConsRtiDataEntryBase()
{
}

bool EquipRecoScUpConsRtiDataEntryBase::Read(Avalon::DataRow& row)
{
	if(row.GetColNum() != GetColNum())
	{
		return false;
	}	

	iD = row.ReadUInt32();

	{
		std::string str = row.ReadString();
		Avalon::StringUtil::Split(str, timesSection, "|");
	}

	ratio = row.ReadUInt32();

	return InternalRead();
}

bool EquipRecoScUpConsRtiDataEntry::InternalRead()
{
	if (timesSection.size() != 2)
	{
		return false;
	}
	for (size_t i = 0; i < timesSection.size(); ++i)
	{
		timesSpans.push_back(Avalon::StringUtil::ConvertToValue<Int32>(timesSection[i]));
	}
	return true;
}

//UInt32 EquipRecoScUpConsRtiDataEntryMgr::getRatio(UInt32 times)
//{
//
//}