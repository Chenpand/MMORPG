#include "CLChargeMallDataEntry.h"

bool ChargeMallDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	sort = row.ReadUInt32();
	desc = row.ReadString();

	tags = 0;
	std::string strTags = row.ReadString();
	if (strTags != "-")
	{
		std::vector<std::string> params;
		Avalon::StringUtil::Split(strTags, params, "|");
		for (auto& str : params)
		{
			UInt32 flag = Avalon::StringUtil::ConvertToValue<UInt32>(str);
			tags |= (1 << flag);
		}
	}

	chargeMoney = row.ReadUInt32();
	vipScore = row.ReadUInt32();
	dailiyLimitNum = row.ReadUInt8();
	itemId = row.ReadUInt32();
	num = row.ReadUInt32();
	firstChargeAddNum = row.ReadUInt32();
	unfirstChargeAddNum = row.ReadUInt32();
	icon = row.ReadString();

	return true;
}