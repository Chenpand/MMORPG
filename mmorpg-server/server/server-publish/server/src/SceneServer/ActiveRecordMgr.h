#ifndef __ACTIVE_RECORD_MGR_H__
#define __ACTIVE_RECORD_MGR_H__

/*
	负责玩家运营活动行为的记录功能
*/

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLDrawPrizeDataEntry.h>

// 运营活动行为拥有者类型
enum OpActivityBehaviorOwnerType
{
	// 角色
	OP_ACTIVITY_BEHAVIOR_OWNER_ROLE,
	// 账号
	OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT,
};

// 运营活动行为
enum OpActivityBehavior
{
	OP_ACTIVITY_BEHAVIOR_NULL,
	
	// 每日的数据
	OP_ACTIVITY_BEHAVIOR_DAILY_BEGIN		= 1000,
	OP_ACTIVITY_BEHAVIOR_DAILY_CHARGE_DAY	= 1001,	// 每日礼包
	OP_ACTIVITY_BEHAVIOR_DAILY_SUBMIT_TASK	= 1002,	// 单个活动任务每日领奖次数
	OP_ACTIVITY_BEHAVIOR_DAILY_DUNGEON_COUNT= 1003,	// 每日地下城次数
	OP_ACTIVITY_BEHAVIOR_DAILY_ACCEPT_TASK	= 1004,	// 活动任务每日接取次数
	OP_ACTIVITY_BEHAVIOR_DAILY_ONLINE_COUNT = 1005,	// 帐号每日在线时间
	OP_ACTIVITY_BEHAVIOR_DAILY_ONLINE_TASK  = 1006,	// 帐号每日在线任务
	OP_ACTIVITY_BEHAVIOR_DAILY_END,

	// 每周的数据
	OP_ACTIVITY_BEHAVIOR_WEEKLY_BEGIN = 2000,
	OP_ACTIVITY_BEHAVIOR_WEEKLY_SUMMER_CHALLENGE = 2001,	//暑期挑战
	OP_ACTIVITY_BEHAVIOR_WEEKLY_SUBMIT_TASK = 2002,			//单个活动任务每周领奖次数
	OP_ACTIVITY_BEHAVIOR_WEEKLY_END,

	// 每月的数据
	OP_ACTIVITY_BEHAVIOR_MONTHLY_BEGIN = 3000,
	OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_MONTH_GET = 3001,		//信用点券月获得
	OP_ACTIVITY_BEHAVIOR_MONTHLY_END,

	// 总的数据
	OP_ACTIVITY_BEHAVIOR_TOTAL_BEGIN = 4000,
	OP_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK		= 4002,	// 单个活动任务总的领奖次数
	OP_ITEM_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK = 4003,	// 单个节日兑换活动任务总的领奖次数
	OP_RECHARGE_PUSH_BEHAVIOR_LAST_WEEK_BEFORE	= 4004,	// 充值推送行为（上上周）
	OP_RECHARGE_PUSH_BEHAVIOR_LAST_WEEK			= 4005,	// 充值推送行为（上周）
	OP_RECHARGE_PUSH_BEHAVIOR_THIS_WEEK			= 4006,	// 充值推送行为（本周）
	OP_ACTIVITY_BEHAVIOR_GNOMECOIN_COUNT		= 4007,	// 累计地精币
	OP_ACTIVITY_BEHAVIOR_MONEY_CONSUME_COUNT	= 4008,	// 累计点券消费
	OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_TRANSFER_TIME = 4009,//信用点券转化时间
	OP_ACTIVITY_BEHAVIOR_TOTAL_SERVER_ITEM_NUM	= 4010,	// 全服抽奖池道具产出总量控制
	OP_ACTIVITY_BEHAVIOR_TOTAL_END,
};

// 运营活动参数
enum OpActivityParam
{
	OP_ACTIVITY_PARAM_FASHION_COMPOSE	= 1,	//时装合成
	OP_ACTIVITY_PARAM_FASHION_COUNT		= 2,	//时装数量
};

struct OpActivityRecordKey  
{
	OpActivityRecordKey(OpActivityBehavior behavior = OP_ACTIVITY_BEHAVIOR_NULL, OpActivityBehaviorOwnerType ownerType = OP_ACTIVITY_BEHAVIOR_OWNER_ROLE, ObjID_t owner = 0, UInt32 param1 = 0)
	{
		this->behavior = behavior;
		this->ownerType = ownerType;
		this->owner = owner;
		this->param1 = param1;
	}

	bool operator<(const OpActivityRecordKey& other) const
	{
		if (behavior != other.behavior)
		{
			return behavior < other.behavior;
		}

		if (ownerType != other.ownerType)
		{
			return ownerType < other.ownerType;
		}

		if (owner != other.owner)
		{
			return owner < other.owner;
		}

		return param1 < other.param1;
	}

	OpActivityBehavior			behavior;
	OpActivityBehaviorOwnerType	ownerType;
	ObjID_t					owner;
	UInt32					param1;
};

struct OpActivityRecord
{
	AVALON_DEFINE_OBJECTPOOL(OpActivityRecord, Avalon::NullMutex);

	OpActivityRecord()
	{
		guid = 0;
		owner = 0;
		behavior = OP_ACTIVITY_BEHAVIOR_NULL;
		param1 = 0;
		num = 0;
		recordTime = 0;
	}

	OpActivityRecordKey GetKey()
	{
		return OpActivityRecordKey(behavior, ownerType, owner, param1);
	}

	// 唯一ID
	ObjID_t					guid;
	// 拥有者类型(角色、账号)
	OpActivityBehaviorOwnerType	ownerType;
	// 拥有者ID，如果为0那么代表是服务器总计数
	ObjID_t					owner;
	// 行为
	OpActivityBehavior		behavior;
	// 参数1
	UInt32					param1;
	// 计数
	UInt32					num;
	// 时间
	UInt32					recordTime;
};

class OpActivityRecordMgr : public Avalon::Singleton<OpActivityRecordMgr>, public IRewardPoolRareItemControl
{
public:
	typedef std::map<OpActivityRecordKey, OpActivityRecord*> MapOpActivityRecord;
public:
	OpActivityRecordMgr();
	virtual ~OpActivityRecordMgr();

	/**
	*@brief 初始化
	*/
	bool Init();

	/**
	*@brief 心跳
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 从数据库加载数据
	*/
	void LoadRecordFromDB(UInt32 accid, ObjID_t roleid);

	void LoadRoleRecordFromDB(ObjID_t roleid);

	void LoadAcctRecordFromDB(UInt32 accid);
public:
	/**
	*@brief 加载记录
	*/
	bool LoadRecord(OpActivityRecord* record);

	/**
	*@brief 获取记录
	*/
	OpActivityRecord* GetRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1);

	/**
	*@brief 获取记录中的计数
	*/
	UInt32 GetRecordValue(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1);

	/**
	*@brief 更新计数，addNum为负代表扣计数
	*/
	void UpdateRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int addNum);

	/**
	*@brief 更新计数
	*/
	void SetRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int num);

public:
	/**
	*@brief 稀有控制物品剩余数量
	*/
	virtual UInt32 RareRewardRemainNum(UInt32 rewardId, UInt32 num);

private:
	/**
	*@brief 添加记录
	*/
	void _AddRecord(OpActivityRecord* record);

	/**
	*@brief 创建记录
	*/
	OpActivityRecord* _CreateRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, UInt32 num);

	/**
	*@brief 判断记录是否过期
	*/
	bool _IsTimeOut(OpActivityBehavior behavior, UInt32 recordTime);

	/**
	*@brief 记录到数据库
	*/
	void _RecordToDB(OpActivityRecord* record, bool isNew = false);

	/**
	*@brief 从数据库删除
	*/
	void _DeleteFromDB(OpActivityRecord* record);

private:
	MapOpActivityRecord				m_Records;						// 记录
};

#endif