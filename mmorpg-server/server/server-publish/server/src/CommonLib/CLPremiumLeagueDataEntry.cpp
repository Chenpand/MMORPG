#include "CLPremiumLeagueDataEntry.h"

bool PremiumLeagueDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	status = (PremiumLeagueStatus)row.ReadUInt8();
	week = row.ReadUInt8();
	{
		std::string str = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, ":");
		if (params.size() != 2)
		{
			return false;
		}

		hour = Avalon::StringUtil::ConvertToValue<UInt8>(params[0]);
		minute = Avalon::StringUtil::ConvertToValue<UInt8>(params[1]);
	}
	durningMin = row.ReadUInt32();

	return true;
}