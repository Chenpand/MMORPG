#ifndef __SYS_RECORD_MGR_H__
#define __SYS_RECORD_MGR_H__

/*
	负责玩家行为的记录功能，如稀有物品记录
*/

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <DropItemMgr.h>
#include <AvalonDailyTimer.h>
#include <CLSysRecordDefine.h>
#include <CLSysRecordProtocol.h>
#include <CLRareItemDataEntry.h>

// 玩家行为拥有者类型
enum SysBehaviorOwnerType
{
	// 角色
	SYS_BEHAVIOR_OWNER_ROLE,
	// 账号
	SYS_BEHAVIOR_OWNER_ACCOUNT,
};

// 玩家行为
enum SysBehavior
{
	SYS_BEHAVIOR_NULL,
	
	// 每日的数据
	SYS_BEHAVIOR_DAILY_BEGIN = 1000,
	SYS_BEHAVIOR_DAILY_RARE_ITEM,				// 稀有物品
	SYS_BEHAVIOR_DAILY_TEAM_REWARD,				// 组队奖励
	SYS_BEHAVIOR_DAILY_DUNGEON,					// 每日地下城次数
	SYS_BEHAVIOR_DAILY_ACTIVITY_MONSTER,		// 活动怪物
	SYS_BEHAVIOR_DAILY_DUNGEON_CHEAT,			// 每日地下城作弊次数
	SYS_BEHAVIOR_DAILY_DUNGEON_MATERIAL_SHOP_ITEM,		// 地下城材料商店稀有物品
	SYS_BEHAVIOR_DAILY_DUNGEON_MATERIAL_EQUIP_ITEM,		// 地下城装备商店稀有物品
	SYS_BEHAVIOR_DAILY_DUNGEON_MATERIAL_JEWELRY_ITEM,	// 地下城珠宝商店稀有物品
	SYS_BEHAVIOR_DAILY_DUNGEON_MONSTER_DROP,	// 每日地下城怪物掉落次数
	SYS_BEHAVIOR_DAILY_DUNGEON_CHEAT_DEVIL,		// 异界地下城作弊次数
	SYS_BEHAVIOR_DAILY_DUNGEON_AREA_INDEX,		// 每日地下城房间进度
	SYS_BEHAVIOR_DAILY_DUNGEON_GUILD_INDEX,		// 每日公会地下城
	SYS_BEHAVIOR_DAILY_DUNGEON_GUARANTEE_DROP,	// 每日地下城保底掉落
	SYS_BEHAVIOR_DAILY_VETERAN_RETURN_DROP,		// 地下城回归掉落
	SYS_BEHAVIOR_DAILY_DUNGEON_HIDDEN_ROOM_FLAG,// 是否计算过地下城隐藏房间
	SYS_BEHAVIOR_DAILY_ITEM_GET_NUM,			// 物品获得数量
	SYS_BEHAVIOR_DAILY_TEAM_COPY_INDEX,			// 每日团队副本
	SYS_BEHAVIOR_DAILY_END,
	

	// 每周的数据
	SYS_BEHAVIOR_WEEKLY_BEGIN = 2000,
	SYS_BEHAVIOR_WEEKLY_RARE_ITEM,				// 稀有物品
	SYS_BEHAVIOR_WEEKLY_DUNGEON_MATERIAL_SHOP_ITEM,		// 地下城材料商店稀有物品
	SYS_BEHAVIOR_WEEKLY_DUNGEON_EQUIP_SHOP_ITEM,		// 地下城装备商店稀有物品
	SYS_BEHAVIOR_WEEKLY_DUNGEON_JEWELRY_SHOP_ITEM,		// 地下城珠宝商店稀有物品
	SYS_BEHAVIOR_WEEKLY_DUNGEON_GUARANTEE_DROP,			// 每周地下城保底掉落
	SYS_BEHAVIOR_WEEKLY_END,

	// 每月的数据
	SYS_BEHAVIOR_MONTHLY_BEGIN = 3000,
	SYS_BEHAVIOR_MONTHLY_RARE_ITEM,				// 稀有物品
	SYS_BEHAVIOR_MONTHLY_DUNGEON_MATERIAL_SHOP_ITEM,	// 地下城材料商店稀有物品
	SYS_BEHAVIOR_MONTHLY_DUNGEON_EQUIP_SHOP_ITEM,		// 地下城装备商店稀有物品
	SYS_BEHAVIOR_MONTHLY_DUNGEON_JEWELRY_SHOP_ITEM,		// 地下城珠宝商店稀有物品
	SYS_BEHAVIOR_MONTHLY_DUNGEON_GUARANTEE_DROP,		// 每月地下城保底掉落
	SYS_BEHAVIOR_MONTHLY_END,

	// 总的数据
	SYS_BEHAVIOR_TOTAL_BEGIN = 4000,
	SYS_BEHAVIOR_TOTAL_RARE_ITEM,				// 稀有物品
	SYS_BEHAVIOR_TOTAL_DUNGEON_GUARANTEE_DROP,	// 地下城保底掉落
	SYS_BEHAVIOR_TOTAL_DUNGEON_CHEAT,			// 地下城作弊次数
	SYS_BEHAVIOR_TOTAL_DROP_SOLUTION,			// 掉落方案
	SYS_BEHAVIOR_TOTAL_DUNGEON_MATERIAL_SHOP_ITEM,		// 地下城材料商店稀有物品
	SYS_BEHAVIOR_TOTAL_DUNGEON_EQUIP_SHOP_ITEM,			// 地下城装备商店稀有物品
	SYS_BEHAVIOR_TOTAL_DUNGEON_JEWELRY_SHOP_ITEM,		// 地下城珠宝商店稀有物品
	SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_TIMESTAMP,			//充值推送截至时间戳
	SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT,			//充值推送购买次数
	SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_LASTTIME,			//充值推送上次派发的时间戳
	SYS_BEHAVIOR_TOTAL_RECHARGE_COOL_TIMESTAMP,			//充值推送冷却截至时间戳
	SYS_BEHAVIOR_TOTAL_END,
};

struct SysRecordKey
{
	SysRecordKey(SysBehavior behavior = SYS_BEHAVIOR_NULL, SysBehaviorOwnerType ownerType = SYS_BEHAVIOR_OWNER_ROLE, ObjID_t owner = 0, UInt32 param1 = 0)
	{
		this->behavior = behavior;
		this->ownerType = ownerType;
		this->owner = owner;
		this->param1 = param1;
	}

	bool operator<(const SysRecordKey& other) const
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

	SysBehavior				behavior;
	SysBehaviorOwnerType	ownerType;
	ObjID_t					owner;
	UInt32					param1;
};

struct SysRecord
{
	AVALON_DEFINE_OBJECTPOOL(SysRecord, Avalon::NullMutex);

	SysRecord()
	{
		guid = 0;
		owner = 0;
		behavior = SYS_BEHAVIOR_NULL;
		param1 = 0;
		num = 0;
		recordTime = 0;
	}

	SysRecordKey GetKey()
	{
		return SysRecordKey(behavior, ownerType, owner, param1);
	}

	// 唯一ID
	ObjID_t					guid;
	// 拥有者类型(角色、账号)
	SysBehaviorOwnerType	ownerType;
	// 拥有者ID，如果为0那么代表是服务器总计数
	ObjID_t					owner;
	// 行为
	SysBehavior				behavior;
	// 参数1
	UInt32					param1;
	// 计数
	UInt32					num;
	// 时间
	UInt32					recordTime;
};

// 稀有控制道具不同时间段的数量上限
struct RareItemLimitByTime
{
	RareItemLimitByTime()
	{
		itemId = 0;
		beginTime = 0;
		endTime = 0;
		maxNum = 0;
	}

	bool operator<(const RareItemLimitByTime& other) const
	{
		if (itemId != other.itemId)
		{
			return itemId < other.itemId;
		}
		
		if (beginTime != other.beginTime)
		{
			return beginTime < other.beginTime;
		}

		if (endTime != other.endTime)
		{
			return endTime < other.endTime;
		}

		return maxNum < other.maxNum;
	}

	// 道具ID
	UInt32		itemId;
	// 开始时间
	UInt32		beginTime;
	// 结束时间
	UInt32		endTime;
	// 数量上限
	UInt32		maxNum;
};

class SysRecordMgr : public Avalon::Singleton<SysRecordMgr>, public IRareItemControl
{
public:
	typedef std::map<SysRecordKey, SysRecord*> MapSysRecord;
public:
	SysRecordMgr();
	virtual ~SysRecordMgr();

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

public:
	/**
	*@brief 加载记录
	*/
	bool LoadSysRecord(SysRecord* record);

	/**
	*@brief 获取记录
	*/
	SysRecord* GetRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1);

	/**
	*@brief 获取记录中的计数
	*/
	UInt32 GetRecordValue(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1);

	/**
	*@brief 更新计数，addNum为负代表扣计数
	*/
	void UpdateRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int addNum);

	/**
	*@brief 更新计数
	*/
	void SetRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int num);

public:
	// 稀有控制相关

	/**
	*@brief 稀有控制物品剩余数量
	*/
	virtual UInt32 RareItemRemainNum(ObjID_t roleId, UInt32 itemId, UInt32 num);

	/**
	*@brief 稀有控制物品剩余数量
	*/
	void RecordGotRareItem(ObjID_t roleId, UInt32 itemId, int num);
	void RecordGotRareItem(RareControlType type, ObjID_t roleId, UInt32 parm1, int num);

	/**
	*@brief 稀有控制方案剩余数量
	*/
	virtual UInt32 RareDropSolutionRemainNum(ObjID_t roleId, UInt32 solutionId, UInt32 num);

	/**
	*@brief 记录稀有控制方案
	*/
	void RecordGotRareDropSolution(ObjID_t roleId, UInt32 solutionId, int num);

	/**
	*@brief sysrecord数据请求
	*/
	void OnSysRecordDataReq(CLProtocol::SceneSysRecordDataReq& req);

public:
	/**
	*brief vip控制上限物品剩余数量
	*/
	UInt32 VipLimitItemRemainNum(ObjID_t roleId, UInt32 itemId, UInt32 num, UInt32 vipLimitId, UInt8 vipLv);

	/**
	*@brief vip控制上限物品剩余数量
	*/
	void RecordGotVipLimitItem(ObjID_t roleId, UInt32 itemId, int num);

private:
	/**
	*@brief 添加记录
	*/
	void _AddRecord(SysRecord* pkRecord);

	/**
	*@brief 创建记录
	*/
	SysRecord* _CreateRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, UInt32 num);

	/**
	*@brief 判断记录是否过期
	*/
	bool _IsTimeOut(SysBehavior behavior, UInt32 recordTime);

	/**
	*@brief 记录到数据库
	*/
	void _RecordToDB(SysRecord* record, bool isNew = false);

	/**
	*@brief 获取不同类型的稀有控制情况下的稀有道具剩余数量
	*/
	UInt32 _GetRareItemRemainNum(SysBehavior behavior, ObjID_t roleId, UInt32 itemId, UInt32 num, UInt32 maxNum);
	UInt32 _GetRareItemRemainNum(SysBehavior behavior, ObjID_t roleId, UInt32 itemId, UInt32 maxNum);

	/**
	*@brief 计算不同时间段稀有道具最大数量
	*/
	void _CountRareItemMaxNumByTime();
	void _CountRareItemMaxNumByTime(UInt32 itemId, UInt32 startTime, UInt32 endTime, UInt32 beginNum, UInt32 maxNum);

	/**
	*@brief 获取当前时间稀有道具最大数量,返回-1代表没有限制
	*/
	UInt32 _GetRareItemMaxNumByTime(UInt32 itemId);

	/**
	*@brief 获取当日的开始
	*/
	UInt32 _CountTodayStartTime();

	std::string _GetTimeStr(UInt32 time);

	/**
	*@brief 根据稀有控制类型和行为大类获取具体行为
	*/
	SysBehavior _GetSysBehaviorByRareControlType(RareControlType rareType, SysBehavior behaviorType);

private:
	Avalon::DailyTimer				m_dailyTimer;

	MapSysRecord					m_sysRecords;						// 记录

	// 不同时间段的稀有道具数量上限
	std::map<RareItemLimitByTime, UInt32>	m_rareItemNumLimitByTime;
};

#endif