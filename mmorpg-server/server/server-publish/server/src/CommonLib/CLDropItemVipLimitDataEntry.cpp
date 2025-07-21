#include "CLDropItemVipLimitDataEntry.h"

DropItemVipLimitDataEntry::DropItemVipLimitDataEntry()
{

}

DropItemVipLimitDataEntry::~DropItemVipLimitDataEntry()
{

}

bool DropItemVipLimitDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	std::string ss = row.ReadString();
	std::vector<std::string> sv;
	Avalon::StringUtil::Split(ss, sv, "|");
	if (sv.size() != 12)
	{
		ErrorLogStream << "vip size err id : " << id << LogStream::eos;
		return false;
	}
	for (auto s : sv)
	{
		vipLimits.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(s));
	}

	return true;
}