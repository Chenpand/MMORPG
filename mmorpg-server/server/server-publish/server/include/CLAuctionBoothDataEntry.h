#ifndef _CL_AUCTION_BOOTH_DATAENTRY_H_
#define _CL_AUCTION_BOOTH_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 拍卖行栏位表
*/
class AuctionBoothDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32	id;
	// 货币ID
	UInt32	costItemId;
	// 数量
	UInt32	num;
};

typedef Avalon::DataEntryMgr<AuctionBoothDataEntry> AuctionBoothDataEntryMgr;

#endif
