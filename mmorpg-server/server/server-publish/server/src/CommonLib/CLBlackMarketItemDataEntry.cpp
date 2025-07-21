#include "CLBlackMarketItemDataEntry.h"
#include "CLBlackMarketDefine.h"

bool BlackMarketItemDataEntry::Read(Avalon::DataRow& row)
{
	if (row.GetColNum() != GetColNum())
	{
		ErrorLogStream << "BlackMarketItemDataEntry::Read" << "  row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}

	iD = row.ReadUInt32();
	
	{
		std::string ss = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(ss, strs, "|");
		for (std::vector<std::string>::iterator iter = strs.begin(); iter != strs.end(); ++iter)
		{
			buyTypes.push_back(Avalon::StringUtil::ConvertToValue<UInt8>(*iter));
		}
	}

	groupId = row.ReadUInt8();
	itemId = row.ReadUInt32();
	recomPrice = row.ReadUInt32();

	{
		std::string ss1 = row.ReadString();
		std::string ss2 = row.ReadString();
		std::vector<std::string> strs1;
		std::vector<std::string> strs2;
		Avalon::StringUtil::Split(ss1, strs1, "|");
		Avalon::StringUtil::Split(ss2, strs2, "|");
		if (strs1.size() != strs2.size() || strs1.size() == 0)
		{
			ErrorLogStream << "lowerPrice, lowerPriceWt is err" << LogStream::eos;
			return false;
		}
		for (UInt32 i = 0; i < strs1.size(); ++i)
		{
			PriceItem priceItem;
			priceItem.price = Avalon::StringUtil::ConvertToValue<UInt32>(strs1[i]);
			priceItem.weight = Avalon::StringUtil::ConvertToValue<UInt32>(strs2[i]);
			lowerPrices.push_back(priceItem);
		}
	}

	{
		std::string ss1 = row.ReadString();
		std::string ss2 = row.ReadString();
		std::vector<std::string> strs1;
		std::vector<std::string> strs2;
		Avalon::StringUtil::Split(ss1, strs1, "|");
		Avalon::StringUtil::Split(ss2, strs2, "|");
		if (strs1.size() != strs2.size() || strs1.size() == 0)
		{
			ErrorLogStream << "upperPrice, upperPriceWt is err" << LogStream::eos;
			return false;
		}
		for (UInt32 i = 0; i < strs1.size(); ++i)
		{
			PriceItem priceItem;
			priceItem.price = Avalon::StringUtil::ConvertToValue<UInt32>(strs1[i]);
			priceItem.weight = Avalon::StringUtil::ConvertToValue<UInt32>(strs2[i]);
			upperPrices.push_back(priceItem);
		}
	}

	subType = row.ReadUInt8();
	fixBuyLimit = row.ReadInt32();
	return true;
}

void BlackMarketItemDataEntryMgr::GetDataEntrysByBuyType(UInt8 buyType, UInt8 subType, std::map<UInt8, std::vector<BlackMarketItemDataEntry*>>& entrys)
{
	class GetDataEntryVisitor : public BlackMarketItemDataEntryVisitor
	{
	public:
		GetDataEntryVisitor(UInt8 buyType, UInt8 subType_, std::map<UInt8, std::vector<BlackMarketItemDataEntry*>>& entrys)
			: buy_type(buyType), buy_subType(subType_), dataEntrys(entrys) {}
		~GetDataEntryVisitor()  {}

		virtual bool operator()(BlackMarketItemDataEntry* dataEntry)
		{
			bool fit = false;
			for (UInt32 i = 0; i < dataEntry->buyTypes.size(); i++)
			{
				if (buy_type == dataEntry->buyTypes[i])
				{
					fit = true;
					break;
				}
			}
			if (!fit)
			{
				return true;
			}

			if (dataEntry->subType != buy_subType)
			{
				return true;
			}

			dataEntrys[dataEntry->groupId].push_back(dataEntry);
			return true;
		}

	public:
		UInt8 buy_type;
		UInt8 buy_subType;
		std::map<UInt8, std::vector<BlackMarketItemDataEntry*>>& dataEntrys;
	};

	GetDataEntryVisitor visitor(buyType, subType, entrys);
	this->Visit(visitor);
}