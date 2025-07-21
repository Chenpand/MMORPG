#include "CLBuffDataEntry.h"

#include "CLSkillDefine.h"

BuffDataEntry::BuffDataEntry()
{
	id = 0;
	buffType = 0;
	workType = 0;
	overLay = 0;
	overLayLimit = 0;
	taskDropNumRate = 0;
	pinkDropRate = 0;
	durationType = 0;
	duration = 0;
	effectTimes = 0;
	applyDungeons.clear();
}

BuffDataEntry::~BuffDataEntry()
{
}

bool BuffDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	buffType = row.ReadInt16();
	workType = row.ReadUInt8();
	overLay = row.ReadUInt8();
	overLayLimit = row.ReadUInt8();
	std::string expAddStr = row.ReadString();
	if (!expAddRate.DecodeFromString(expAddStr))
	{
		return false;
	}
	std::string goldAddStr = row.ReadString();
	if (!goldAddRate.DecodeFromString(goldAddStr))
	{
		return false;
	}
	taskDropNumRate = row.ReadUInt8();
	pinkDropRate = row.ReadUInt8();
	chestDropRate = row.ReadUInt8();
	durationType = row.ReadUInt8();
	duration = row.ReadUInt32();
	effectTimes = row.ReadUInt32();

	if (durationType == BDT_DEADLINE_CALC && effectTimes == 0)
	{
		ErrorLogStream << "Duration type not match effect times!" << LogStream::eos;
		return false;
	}

	if (effectTimes > 0 && durationType != BDT_DEADLINE_CALC)
	{
		ErrorLogStream << "Effect times not match duration type!" << LogStream::eos;
		return false;
	}

	{
		std::string str = row.ReadString();
		if (!str.empty())
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");

			for (auto& elem : strs)
			{
				applyDungeons.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
			}
		}
	}

	param1 = row.ReadInt32();
	param2 = row.ReadInt32();

	return true;
}

bool BuffInfoEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	bufferId = row.ReadUInt32();

	return true;
}