#include "CLEquipTransMapDataEntry.h"

EquipTransMapDataEntryBase::EquipTransMapDataEntryBase()
{
}

EquipTransMapDataEntryBase::~EquipTransMapDataEntryBase()
{
}

bool EquipTransMapDataEntryBase::Read(Avalon::DataRow& row)
{
	if(row.GetColNum() != GetColNum())
	{
		ErrorLogStream << "EquipTransMapDataEntryBase::Read(Avalon::DataRow& row)" << "  row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}	

	iD = row.ReadUInt32();
	itemId = row.ReadUInt32();

	{
		minLev = 0;
		maxLev = 0;
		std::string str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");
		if (strs.size() == 2)
		{
			minLev = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
			maxLev = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
		}
	}


	{
		std::string str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");
		for (std::vector<std::string>::iterator iter = strs.begin(); iter != strs.end(); ++iter)
		{
			UInt32 var = Avalon::StringUtil::ConvertToValue<UInt32>(*iter);
			subTypes.push_back(var);
		}
	}


	return InternalRead();
}