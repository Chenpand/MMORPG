#ifndef _CL_AUCTION_RECOVE_DATAENTRY_H_
#define _CL_AUCTION_RECOVE_DATAENTRY_H_
#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 拍卖行扫货表数据项
*/
class AuctionRecoveItemDataEntry : public Avalon::DataEntry
{
public:
	AuctionRecoveItemDataEntry() {};
	~AuctionRecoveItemDataEntry() {};

public:
	virtual UInt32 GetKey() const { return iD; };
	
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 2; };

public:
	//唯一ID
	UInt32				iD;
	//系统回收价格系数
	UInt32				sysRecoPriceRate;
	
};

typedef Avalon::DataEntryVisitor<AuctionRecoveItemDataEntry> AuctionRecoveItemDataEntryVisitor;

class AuctionRecoveItemDataEntryMgr : public Avalon::DataEntryMgrBase<AuctionRecoveItemDataEntry>
	, public Avalon::Singleton<AuctionRecoveItemDataEntryMgr>
{
};


#endif