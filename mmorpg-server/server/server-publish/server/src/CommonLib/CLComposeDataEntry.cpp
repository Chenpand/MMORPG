#include "CLComposeDataEntry.h"

ComposeDataEntry::ComposeDataEntry()
{
}

ComposeDataEntry::~ComposeDataEntry()
{
}

bool ComposeDataEntry::Read(Avalon::DataRow& row)
{
	itemid = row.ReadUInt32();
	quality = row.ReadUInt8();
	std::string ratestr = row.ReadString();
	if(!ratestr.empty() && ratestr != "0")
	{
		std::vector<std::string> ratepairs;
		Avalon::StringUtil::Split(ratestr, ratepairs, "|");
		for(std::vector<std::string>::iterator iter = ratepairs.begin();
			iter != ratepairs.end(); ++iter)
		{
			std::vector<std::string> ratepair;
			Avalon::StringUtil::Split(*iter, ratepair, ":");
			if(ratepair.size() >= 2)
			{
				UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(ratepair[0]);
				UInt32 rate = Avalon::StringUtil::ConvertToValue<UInt32>(ratepair[1]);
				rates.insert(std::make_pair(num, rate));
			}
		}
	}
	maxNum = row.ReadUInt32();
	for(int i = 0; i < MAX_ITEM_COMPOSE_MATERIAL_NUM; ++i)
	{
		materials[i] = row.ReadUInt32();
		materialNum[i] = row.ReadUInt32();
	}
	return true;
}
