#ifndef _CL_BLACKMARKET_ITEM_DATAENTRY_H_
#define _CL_BLACKMARKET_ITEM_DATAENTRY_H_
#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 随机价格
*/
struct PriceItem 
{
	UInt32 price;
	UInt32 weight;
};

/**
*@brief 黑市商人收购物品库
*/
class BlackMarketItemDataEntry : public Avalon::DataEntry
{
public:
	BlackMarketItemDataEntry(){};
	~BlackMarketItemDataEntry(){};

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 11; };
public:
	//唯一ID
	UInt32				iD;
	//支持收购类型
	std::vector<UInt8>	buyTypes;
	//子类型
	UInt8				subType;
	//组id
	UInt8				groupId;
	//物品id
	UInt32				itemId;
	//推荐价格
	UInt32				recomPrice;
	//向下波动价格
	std::vector<PriceItem>  lowerPrices;
	//向上波动价格
	std::vector<PriceItem>  upperPrices;
	//固定收购上限数量
	UInt32				fixBuyLimit;
};

typedef Avalon::DataEntryVisitor<BlackMarketItemDataEntry> BlackMarketItemDataEntryVisitor;

class BlackMarketItemDataEntryMgr : public Avalon::DataEntryMgrBase<BlackMarketItemDataEntry>
	, public Avalon::Singleton<BlackMarketItemDataEntryMgr>
{
public:
	void GetDataEntrysByBuyType(UInt8 buyType, UInt8 subTye, std::map<UInt8, std::vector<BlackMarketItemDataEntry*>>& entrys);
};

#endif