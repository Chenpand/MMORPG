#include "CLEquipInitAttriOnMakeDataEntry.h"

bool EquipInitAttriOnMakeDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	//唯一id
	id = row.ReadUInt32();
	//道具id
	itemId = row.ReadUInt32();
	//强化等级
	strenthLv = row.ReadUInt32();
	//品级
	qualityLv = row.ReadUInt32();
	//是否租赁
	isLease = row.ReadUInt8();
	//推荐职业
	std::string str = row.ReadString();
	if (str.size() > 0)
	{
		std::vector<std::string>	strv;
		Avalon::StringUtil::Split(str, strv, "|");
		for (size_t i = 0; i < strv.size(); ++i)
		{
			fitOccus.push_back(Avalon::StringUtil::ConvertToValue<Int32>(strv[i]));
		}
	}

	//UInt32		fitOccu = row.ReadUInt32();
	//来源
	sourceId = row.ReadUInt32();
	
	return true;
}