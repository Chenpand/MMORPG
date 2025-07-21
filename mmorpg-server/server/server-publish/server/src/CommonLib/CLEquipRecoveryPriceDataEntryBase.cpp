#include "CLEquipRecoveryPriceDataEntryBase.h"
#include "CLItemDefine.h"

EquipRecoveryPriceDataEntryBase::EquipRecoveryPriceDataEntryBase()
{
}

EquipRecoveryPriceDataEntryBase::~EquipRecoveryPriceDataEntryBase()
{
}

bool EquipRecoveryPriceDataEntryBase::Read(Avalon::DataRow& row)
{
	if(row.GetColNum() != GetColNum())
	{
		return false;
	}	

	iD = row.ReadUInt32();
	blue = row.ReadString();
	purple = row.ReadString();
	pink = row.ReadString();

	return InternalRead();
}

bool EquipRecoveryPriceDataEntry::InternalRead()
{
	std::vector<std::string> params;
	UInt32 begin = 0;
	UInt32 end = 0;

	Avalon::StringUtil::Split(blue, params, "-");
	if (params.size() != 2)
	{
		return false;
	}
	begin = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
	end = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
	std::vector<UInt32>& priceblue = prices[(UInt8)ITEM_QUALITY_BLUE];
	for (UInt32 i = begin; i <= end; ++i)
	{
		priceblue.push_back(i);
	}

	params.clear();
	Avalon::StringUtil::Split(purple, params, "-");
	if (params.size() != 2)
	{
		return false;
	}
	begin = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
	end = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
	std::vector<UInt32>& pricepurple = prices[(UInt8)ITEM_QUALITY_PURPLE];
	for (UInt32 i = begin; i <= end; ++i)
	{
		pricepurple.push_back(i);
	}

	params.clear();
	Avalon::StringUtil::Split(pink, params, "-");
	if (params.size() != 2)
	{
		return false;
	}
	begin = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
	end = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
	std::vector<UInt32>& pricepink = prices[(UInt8)ITEM_QUALITY_PINK];
	for (UInt32 i = begin; i <= end; ++i)
	{
		pricepink.push_back(i);
	}
	return true;
}

UInt32 EqRecoveryPriceDataEntryMgr::getEquipMaxPrice(UInt32 equipLv, UInt32 color)
{
	EquipRecoveryPriceDataEntry* entry = FindEntry(equipLv);
	if (!equipLv)
	{
		return 0;
	}

	std::map<UInt8, std::vector<UInt32>>::iterator it = entry->prices.find(color);
	if (it == entry->prices.end())
	{
		return 0;
	}

	std::vector<UInt32>& price = it->second;
	if (price.size() < 1)
	{
		return 0;
	}
	return price[price.size() - 1];

}