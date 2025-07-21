#ifndef _CL_AUCTION_SYSVALUE_DATAENTRY_H_
#define _CL_AUCTION_SYSVALUE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum AuctionSystemValueType
{
	ASVT_INVALID = 0,

	ASVT_CALC_AVERPRICE_MINTIMES = 1,			//拍卖行物品计算平均交易价格最低交易次数
	ASVT_NOTTREAN_CALC_AVERPRICE_MAXTIMES = 2,	//拍卖行非珍品物品计算平均交易价格最大交易次数
	ASVT_TREAN_CALC_AVERPRICE_MAXTIMES = 3,		//拍卖行珍品物品计算平均交易价格最大交易次数
	ASVT_BEAD_AVERPRICE_RATE = 4,				//宝珠的平均交易价格系数（百分比）
	ASVT_MAGICCARD_AVERPRICE_RATE = 5,			//粉色品质以上附魔卡平均交易价格系数（百分比）
	ASVT_MINIMUM_GUARANTEE_RATIO = 6,			//品级100保底价值系数（百分比）
	ASVT_GRADE_TOPLIMIT = 7,					//品级100上限价值
	ASVT_VERIFY_INTERVAL_TIAME = 8,				//珍品审核时间长度(单位小时)
	ASVT_AUCTION_CMPBEDS_RATIO = 9,				//裸装价格计算和宝珠平均价格比较系数
	ASVT_AUCTION_CMPMAGIC_RATIO = 10,			//裸装价格计算和附魔卡平均价格比较系数
	ASVT_EXPIRE_TIME_AUTO_OFFSALE = 12,			//过期超过时间自动下架
	SVT_AUCTION_QUERY_RECORD_PRICE_UPLIMIT = 13, //拍卖行查询物品交易记录实时平均交易价格上限系数
	SVT_AUCTION_QUERY_RECORD_PRICE_LOWLIMIT = 14, //拍卖行查询物品交易记录实时平均交易价格下限系数

	SVT_AUCTION_QUERY_RECORD_MAX = 15, //拍卖行查询物品交易记录显示上限
	SVT_AUCTION_TREAS_OVERTIME = 16, //拍卖行珍品交易超时时间
	SVT_AUCTION_NUM_CACHE_TIME = 17, //拍卖行拍卖物品数量缓存时间

	SVT_AUCTION_TREAS_DETER_TIMES = 18, //拍卖行交易冷却时间判定次数
	SVT_AUCTION_TREAS_EXTRA_DAYS = 19,  //拍卖行交易冷却时间额外增加天数
	SVT_AUCTION_TREAS_TIMES_CLEAR_DAYS = 20,  //拍卖行交易次数清空间隔(天)
	ASVT_MAX,
};

struct AuctionSystemValueDataEntry : public Avalon::DataEntry
{
public:
	AuctionSystemValueDataEntry();
	~AuctionSystemValueDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//ID
	UInt32 id;

	//数据类型
	AuctionSystemValueType type;

	//数值
	UInt32 value;

};

class AuctionSystemValueDataEntryMgr : public Avalon::DataEntryMgrBase<AuctionSystemValueDataEntry>, public Avalon::Singleton<AuctionSystemValueDataEntryMgr>
{
public:
	typedef std::map<AuctionSystemValueType, AuctionSystemValueDataEntry*> SystemValueMap;
public:
	AuctionSystemValueDataEntryMgr();
	~AuctionSystemValueDataEntryMgr();

public:
	bool AddEntry(AuctionSystemValueDataEntry* entry);

	void OnReload();

	void OnAddEntry(AuctionSystemValueDataEntry* entry);

	UInt32 GetSystemValue(AuctionSystemValueType type);

private:
	SystemValueMap	m_SystemValues;
};



#endif //_CL_SYSTEM_VALUE_DATAENTRY_H_

