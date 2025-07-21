#ifndef _CL_DRAW_PRIZE_DATA_ENTRY_H_
#define _CL_DRAW_PRIZE_DATA_ENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLItemDefine.h>
#include <CLActivityDefine.h>


/**
*@brief 抽奖项
*/
class DrawPrizeDataEntry : public Avalon::DataEntry
{
public:
	DrawPrizeDataEntry() {};
	~DrawPrizeDataEntry() {};

public:
	UInt32 GetKey() const { return dataId; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 19; };

public:
	// 数据项唯一id
	UInt32		dataId;
	// 名称
	std::string	name;
	// 活动id
	ActiveID    activityId;
	// 运营活动类型
	OpActivityType opActivityType;
	// 类型
	UInt32		drawPrizeType;
	// 渠道名
	std::string channelName;
	// 是否开启
	UInt8		isOpen;
	// 抽奖基数
	UInt32		baseCount;
	// 连续抽奖天数
	UInt32		continuousDrawDays;
	// 奖励抽奖次数
	UInt32		rewardCount;
	// 刷新间隔（天）
	UInt32		refreshInterval;
	// 刷新时刻
	UInt8		refreshTime;
	// 已抽奖次数key
	std::string drawPrizeCountKey;
	// 抽奖总次数key
	std::string allCountKey;
	// 连续抽奖计数key
	std::string continuousDrawKey;
	// 剩余抽奖次数key
	std::string restDrawCountKey;
	// 奖品ID key
	std::string rewardIdKey;
	// 抽奖券类型
	ItemSubType raffleTicketType;
	// 扩展参数
	std::vector<UInt32> parms;
};

/**
*@brief 抽奖项管理类
*/
class DrawPrizeDataEntryMgr : public Avalon::DataEntryMgrBase<DrawPrizeDataEntry>
	, public Avalon::Singleton<DrawPrizeDataEntryMgr>
{
public:
	DrawPrizeDataEntryMgr() {};
	~DrawPrizeDataEntryMgr() {};

public:
	bool AddEntry(DrawPrizeDataEntry* dataEntry);
	DrawPrizeDataEntry* GetDrawPrizeDataByType(UInt32 type);

private:
	std::vector<DrawPrizeDataEntry*> m_Datas;
};


/**
*@brief 奖池
*/
class RewardPoolDataEntry : public Avalon::DataEntry
{
public:
	RewardPoolDataEntry() {};
	~RewardPoolDataEntry() {};

public:
	UInt32 GetKey() const { return dataId; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 10; };

public:
	// 唯一id
	UInt32		dataId;
	// 抽奖表id
	UInt32		drawPrizeTableId;
	// 道具id
	UInt32		itemId;
	// 数量
	UInt32		itemNum;
	// 强化等级
	UInt8		strengthenLevel;
	// 权重
	UInt32		itemWeight;
	// 抽奖次数限制
	UInt32		drawPrizeCountLimit;
	// 充值条件
	UInt32		chargeCond;
	// 公告ID
	UInt32		announceNum;
	// 服务器总产出
	UInt32		serverTotalNum;
};

class IRewardPoolRareItemControl
{
public:
	/**
	*@brief 稀有控制物品剩余数量
	*/
	virtual UInt32 RareRewardRemainNum(UInt32 rewardId, UInt32 num) = 0;
};

class RewardPoolDataEntryMgr : public Avalon::DataEntryMgrBase<RewardPoolDataEntry>
	, public Avalon::Singleton<RewardPoolDataEntryMgr>
{
public:
	RewardPoolDataEntryMgr() {};
	~RewardPoolDataEntryMgr() {};

	bool Init(IRewardPoolRareItemControl* control = NULL);

	RewardPoolDataEntry* RandReward(UInt32 drawPrizeId, UInt32 usedDrawCnt = 0, UInt32 totalChargeOnRole = 0);

private:
	IRewardPoolRareItemControl*	m_rareItemControl;
};


/**
*@brief 夺宝商品表
*/
class GambingItemDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return gambingItemId; }

	bool Read(Avalon::DataRow& row);
	bool ReadCustomer(UInt32 _actId, UInt32 _gambingItemId, UInt32 _sortId, const ItemRewardVec& soldItems, const std::vector<std::string>& strVec);

	static UInt32 GetColNum() { return 14; };

public:
	// 夺宝商品id
	UInt32 gambingItemId;
	// 活动id
	UInt32 actId;
	// 夺宝商品数量
	UInt32 gambingItemNum;
	// 排序
	UInt16 sortId;
	// 道具表id
	UInt32 itemDataId;
	// 总组数
	UInt16 totalGroups;
	// 每组份数
	UInt32 copiesOfEachGroup;
	// 消耗货币id
	UInt32 costCurrencyId;
	// 单价（一份）
	UInt32 unitPrice;
	// 组间cd
	UInt32 coolDownIntervalBetweenGroups;
	// 每份奖励
	std::vector<ItemReward> rewardsPerCopy;
	// 加权步长
	UInt32 weightingStepSize;
	// 加权值
	UInt32 weightingValue;
	// 是否公告
	UInt8  isAnnounce;
};

class GambingItemDataEntryMgr : public Avalon::DataEntryMgrBase<GambingItemDataEntry>
	, public Avalon::Singleton<GambingItemDataEntryMgr>
{
public:
	bool AddDataEntryCustomer(UInt32 _actId, UInt32 _gambingItemId, UInt32 _sortId, const ItemRewardVec& soldItems, const std::vector<std::string>& strVec);
};


#endif