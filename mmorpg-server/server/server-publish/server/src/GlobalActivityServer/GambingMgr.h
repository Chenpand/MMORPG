#ifndef _GAMBING_MGR_
#define _GAMBING_MGR_


#include <AvalonObjectPool.h>
#include <AvalonSimpleTimer.h>
#include <CLGambingProtocol.h>
#include <CLActivityDefine.h>

#include "GASPlayer.h"
#include "GambingRecord.h"

struct OpActivityInfo;

/**
*@brief 夺宝商品状态
*/
enum GambingItemStatus
{
	// 初始值
	GIS_INIT = 0,
	// 准备
	GIS_PREPARE,
	// 在售
	GIS_SELLING,
	// 售完
	GIS_SOLD_OUE,
	// 开奖
	GIS_LOTTERY,
	// 下架
	GIS_OFF_SALE,
};

/**
*@brief 我的夺宝状态
*/
enum GambingMineStatus
{
	// 初始值
	GMS_INIT = 0,
	// 夺宝成功
	GMS_SUCCESS,
	// 夺宝失败
	GMS_FAILE,
	// 等待揭晓
	GMS_WAIT,
};

/**
*@brief 夺宝中奖情况
*/
enum GambingLotteryFlag
{
	// 没有中奖
	GLF_NONE = 0,
	// 中奖
	GLF_GET_IT,
};

/**
*@brief 夺宝商品池子
*/
struct GambingItemPool
{
	AVALON_DEFINE_OBJECTPOOL(GambingItemPool, Avalon::NullMutex)

	GambingItemPool()
	{
		gambingItemId = 0;
		gambingItemNum = 0;
		sortId = 0;
		itemDataId = 0;
		totalGroups = 0;
		copiesOfEachGroup = 0;
		costCurrencyId = 0;
		unitPrice = 0;
		coolDownIntervalBetweenGroups = 0;
		weightingStepSize = 0;
		weightingValue = 0;
		restGroups = 0;
		itemPoolStatus = GIS_INIT;
	}

	std::string LogFormatStr() const
	{
		std::ostringstream oss;
		oss << "GambingItemPool(id=" << gambingItemId << ", restGroups=" << restGroups 
			<< ", itemPoolStatus=" << (UInt32)itemPoolStatus << ")";
		return oss.str();
	}

	// 商品id
	UInt32 gambingItemId;
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

	// 剩余组数
	UInt16 restGroups;
	// 状态
	GambingItemStatus itemPoolStatus;
};

/**
*@brief 夺宝商品key
*/
struct GambingItemGroupKey
{
	GambingItemGroupKey()
	{
		gambingItemId = 0;
		groupId = 0;
	}

	GambingItemGroupKey(UInt32 itemId, UInt16 groupId) : gambingItemId(itemId), groupId(groupId) {}

	bool operator<(const GambingItemGroupKey& other) const
	{
		if (gambingItemId < other.gambingItemId)
		{
			return true;
		}
		if (other.gambingItemId < gambingItemId)
		{
			return false;
		}

		if (groupId < other.groupId)
		{
			return true;
		}
		if (other.groupId < groupId)
		{
			return false;
		}

		return false;
	}

	UInt32 gambingItemId;
	UInt16 groupId;
};

/**
*@brief 夺宝商品组信息
*/
class GambingItemGroupInfo
{
	AVALON_DEFINE_OBJECTPOOL(GambingItemGroupInfo, Avalon::NullMutex)

	GambingItemGroupInfo()
	{
		gambingItemId = 0;
		groupId = 0;
		restCopies = 0;
		groupStatus = GIS_INIT;
		sellBeginTime = 0;
		soldOutTimestamp = 0;
	}

	std::string LogFormatStr() const
	{
		std::ostringstream oss;
		oss << "GambingItemGroupInfo(id=" << gambingItemId << ", gId=" << groupId << ", rCs=" << restCopies << ", status=" 
			<< (UInt32)groupStatus << ", bT=" << sellBeginTime << ")";
		return oss.str();
	}

	// 商品id
	UInt32 gambingItemId;
	// 组id
	UInt16 groupId;
	// 剩余份数
	UInt32 restCopies;
	// 状态(对应枚举 GambingItemStatus)
	GambingItemStatus groupStatus;
	// 开始出售时间
	UInt32 sellBeginTime;
	// 售罄时间
	UInt32 soldOutTimestamp;
};

/**
*@brief 夺宝购买上下文
*/
struct PayingGambleContext
{
	PayingGambleContext()
	{
		itemPool = NULL;
		groupInfo = NULL;
	}

	// 夺宝商品池子
	GambingItemPool* itemPool;
	// 夺宝商品组信息
	GambingItemGroupInfo* groupInfo;
	// 请求协议
	CLProtocol::PayingGambleReq payingReqProtocol;
	// 返回协议
	CLProtocol::PayingGambleRes payingResProtocol;
};

typedef std::vector<GambingRecord*> GambingRecordVec;

/**
*@brief 夺宝管理器
*/
class GambingMgr : public Avalon::Singleton<GambingMgr>
{
	typedef std::vector<std::string> GambingParticipantVec;

public:
	GambingMgr();
	~GambingMgr();

public:
	bool OnInit(const OpActivityInfo* opAct);
	void Final();

	/**
	*@brief 加载表格
	*/
	bool LoadTable(const OpActivityInfo* opAct, bool bReload = false);

	/**
	*@brief 重载表格
	*/
	bool ReloadTable();

	/**
	*@brief 加载数据库
	*/
	bool LoadDB(const OpActivityInfo* opAct);

	/**
	*@brief 加载夺宝记录
	*/
	bool OnLoadGambingRecord(CLRecordCallback* callback);

	/**
	*@brief 加载夺宝开奖记录
	*/
	bool OnLoadGambingLotteryRecord(CLRecordCallback* callback);

public:
	/**
	*@brief 设置获取夺宝活动状态
	*/
	void SetGambingActStatus(ActivityState status) { m_GambingActivityStatus = status; }
	ActivityState GetGambingActStatus() const { return m_GambingActivityStatus; }

public:
	/**
	*@brief 添加夺宝记录
	*/
	bool AddGambingRecord(GambingRecord* record);

	/**
	*@brief 添加夺宝开奖记录
	*/
	bool AddGambingLotteryRecord(const GambingItemPool* itemPool, const GambingItemGroupInfo* groupInfo, const std::string& gainer);
	void AddGambingLotteryRecord(GambingLotteryRecord* record);

	/**
	*@brief 添加夺宝组信息
	*/
	bool AddGambingItemGroupInfo(GambingItemGroupInfo* group);

public:
	/**
	*@brief 查找一项夺宝商品池子
	*/
	GambingItemPool* FindGambingItemPool(UInt32 gambingItemId);

	/**
	*@brief 查找一项夺宝商品
	*/
	GambingItemGroupInfo* FindGambingGroupInfo(UInt32 gambingItemId, UInt16 groupId);

	/**
	*@brief 查找夺宝记录
	*/
	GambingRecord* FindGambingRecord(const std::string& ownerUId, UInt32 gambingItemId, UInt16 groupId);
	void FindGambingRecords(const std::string& ownerUId, GambingRecordVec& records);

	/**
	*@brief 查找参与者
	*/
	void FindGambingParticipants(UInt32 gambingItemId, UInt16 groupId, GambingParticipantVec& participants);

	/**
	*@brief 查找一项夺宝中奖数据
	*/
	GambingLotteryRecord* FindGambingLotteryRecord(UInt32 gambingItemId, UInt16 groupId);

public:
	/**
	*@brief tick
	*/
	void OnTick(const Avalon::Time &now);

	/**
	*@brief 活动准备
	*/
	void OnActivityPrepare(const OpActivityInfo* opAct);

	/**
	*@brief 活动开启
	*/
	void OnActivityStart(const OpActivityInfo* opAct);

	/**
	*@brief 活动关闭
	*/
	void OnActivityEnd(const OpActivityInfo* opAct);

public:
	/**
	*@brief 夺宝商品组售罄(用来开启下一组)
	*/
	void OnGambingItemGroupSoldOut(PayingGambleContext& payingGambleContext, GASPlayer *player);

	/**
	*@brief 夺宝商品开奖
	*/
	void OnGambingItemGroupLottery(GambingItemGroupInfo* gambingItem);

	/**
	*@brief 更新参与者信息
	*/
	void UpdateParticipantInfo(GambingRecord* gambingRecord);

	/**
	*@brief 计算夺宝权重
	*/
	void CalculateGambingWeight(GambingItemPool* itemPool, GambingRecord* gambingRecord);

	/**
	*@brief 改名
	*/
	//void OnParticipantNameChanged(ObjID_t roleId, const std::string& name);

public:
	/**
	*@brief 查询夺宝商品
	*/
	void QueryGambingItems(GASPlayer* player);

	/**
	*@brief 获取单个商品信息
	*/
	void GetGambingItemInfo(const GambingItemPool* itemPool, const GambingItemGroupInfo* groupInfo, const std::string& playerUId, CLProtocol::GambingItemInfo& itemInfo);

	/**
	*@brief 查询我的夺宝商品
	*/
	void QueryMineGambingItems(GASPlayer* player);

	/**
	*@brief 购买夺宝商品
	*/
	void PayingGamble(GASPlayer* player, const CLProtocol::PayingGambleReq& req);

	/**
	*@brief 查询夺宝记录
	*/
	void QueryGambingRecordData(GASPlayer* player);

private:
	/**
	*@brief 回收
	*/
	void _Destroy();

	/**
	*@brief 创建夺宝组
	*/
	GambingItemGroupInfo* _CreateGambingItemGroup(GambingItemPool* itemPool);

	/**
	*@brief 初始化各项数据
	*/
	bool _OnInitDataFromDB(GambingRecord* gambingRecord);

	/**
	*@brief 初始化第一组夺宝
	*/
	void _OnInitGambingFirstGroup(const OpActivityInfo* opAct, GambingItemPool* itemPool);

	/**
	*@brief 计算出售时间
	*/
	void _CalculateGroupSellBeginTimeSec(GambingItemPool* itemPool, GambingItemGroupInfo* groupInfo);

private:
	// 夺宝池子
	std::vector<GambingItemPool*> m_GambingItemPool;
	// 重载数据时的临时池子
	std::vector<GambingItemPool*> m_TmpGambingItemPool;

	// 夺宝商品组信息
	std::map<GambingItemGroupKey, GambingItemGroupInfo*> m_GambingItemGroups;

	// 夺宝记录
	HashMap<std::string, GambingRecordVec> m_GambingRecords;

	// 参与者
	std::map<GambingItemGroupKey, GambingParticipantVec> m_ParticipantsMap;

	// 开奖记录
	std::vector<GambingLotteryRecord*> m_GambingLotteryRecord;


	// 每秒定时器
	Avalon::SimpleTimer m_SecondPerTimer;
	// 夺宝活动状态
	ActivityState m_GambingActivityStatus;
};

#endif