#ifndef _CL_AUCTION_NOTREAS_AVERPRICE_MINNUM_DATAENTRY_H_
#define _CL_AUCTION_NOTREAS_AVERPRICE_MINNUM_DATAENTRY_H_
#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 拍卖行非珍品平均交易价格计算最低数量数据项目
*/
class AuctionNoTreasAverPriceMinNumDataEntry : public Avalon::DataEntry
{
public:
	AuctionNoTreasAverPriceMinNumDataEntry() {};
	~AuctionNoTreasAverPriceMinNumDataEntry() {};

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 2; };

public:
	//唯一ID
	UInt32				iD;
	//最低数量
	UInt32				minNum;
};


typedef Avalon::DataEntryVisitor<AuctionNoTreasAverPriceMinNumDataEntry> AuctionNoTreasAverPriceMinNumDataEntryVisitor;

class AuctionNoTreasAverPriceMinNumDataEntryMgr : public Avalon::DataEntryMgrBase<AuctionNoTreasAverPriceMinNumDataEntry>
	, public Avalon::Singleton<AuctionNoTreasAverPriceMinNumDataEntryMgr>
{
};

#endif