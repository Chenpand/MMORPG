#ifndef _CL_AUCTION_MAGIC_EXPECT_UP_DATAENTRY_H_
#define _CL_AUCTION_MAGIC_EXPECT_UP_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum MagicUpConsType
{
	MUCT_NORMAL = 1,  //正常
	MUCT_CONSELF = 2, //只消耗自己
};

/**
*@brief 拍卖行附魔卡期望升级次数
*/
class AuctionMagicExpectUpDataEntry : public Avalon::DataEntry
{
public:
	AuctionMagicExpectUpDataEntry() {};
	~AuctionMagicExpectUpDataEntry() {};

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 5; };

public:
	//唯一ID
	UInt32				iD;
	//升级消耗类型
	UInt8				type;
	//品质
	UInt8				quality;
	//强化等级数量
	UInt8				strengthenLv;
	//期望升级次数
	UInt32				expectUpTimes;
};

typedef Avalon::DataEntryVisitor<AuctionMagicExpectUpDataEntry> AuctionMagicExpectUpDataEntryVisitor;

class AuctionMagicExpectUpDataEntryMgr : public Avalon::DataEntryMgrBase<AuctionMagicExpectUpDataEntry>
	, public Avalon::Singleton<AuctionMagicExpectUpDataEntryMgr>
{
public:
	UInt32 GetExpectUpTimes(UInt8 quality, UInt8 strengthen, UInt8 type);
};

#endif