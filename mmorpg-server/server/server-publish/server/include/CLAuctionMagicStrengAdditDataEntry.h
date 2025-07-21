#ifndef _CL_AUCTION_MAGIC_STRENG_ADDIT_DATAENTRY_H_
#define _CL_AUCTION_MAGIC_STRENG_ADDIT_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum  AuctionMagicStrengType
{
	AMSTP_NONE = 0,		//无效
	AMSTP_MAGIC = 1,		//附魔卡
	AMSTP_STRENGTH = 2,		//装备强化
	AMSTP_ZENFU = 3,		//装备增幅
};

/**
*@brief 拍卖行附魔卡强化加成系数
*/
class AuctionMagicStrengAdditDataEntry : public Avalon::DataEntry
{
public:
	AuctionMagicStrengAdditDataEntry() {};
	~AuctionMagicStrengAdditDataEntry() {};

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 5; };

public:
	//唯一ID
	UInt32				iD;
	//类型
	UInt8				type;
	//强化等级数量
	UInt8				strengthenLv;
	//品质
	UInt8				color;
	//强化加成系数
	UInt32				addition;
};

typedef Avalon::DataEntryVisitor<AuctionMagicStrengAdditDataEntry> AuctionMagicStrengAdditDataEntryVisitor;

class AuctionMagicStrengAdditDataEntryMgr : public Avalon::DataEntryMgrBase<AuctionMagicStrengAdditDataEntry>
	, public Avalon::Singleton<AuctionMagicStrengAdditDataEntryMgr>
{
public:
	UInt32 GetAddition(UInt8 color, UInt8 strengthen, UInt8 type);
};

#endif