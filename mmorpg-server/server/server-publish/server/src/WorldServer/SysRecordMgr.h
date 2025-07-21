#ifndef __SYS_RECORD_MGR_H__
#define __SYS_RECORD_MGR_H__

/*
	���������Ϊ�ļ�¼���ܣ���ϡ����Ʒ��¼
*/

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <DropItemMgr.h>
#include <AvalonDailyTimer.h>
#include <CLSysRecordDefine.h>
#include <CLSysRecordProtocol.h>
#include <CLRareItemDataEntry.h>

// �����Ϊӵ��������
enum SysBehaviorOwnerType
{
	// ��ɫ
	SYS_BEHAVIOR_OWNER_ROLE,
	// �˺�
	SYS_BEHAVIOR_OWNER_ACCOUNT,
};

// �����Ϊ
enum SysBehavior
{
	SYS_BEHAVIOR_NULL,
	
	// ÿ�յ�����
	SYS_BEHAVIOR_DAILY_BEGIN = 1000,
	SYS_BEHAVIOR_DAILY_RARE_ITEM,				// ϡ����Ʒ
	SYS_BEHAVIOR_DAILY_TEAM_REWARD,				// ��ӽ���
	SYS_BEHAVIOR_DAILY_DUNGEON,					// ÿ�յ��³Ǵ���
	SYS_BEHAVIOR_DAILY_ACTIVITY_MONSTER,		// �����
	SYS_BEHAVIOR_DAILY_DUNGEON_CHEAT,			// ÿ�յ��³����״���
	SYS_BEHAVIOR_DAILY_DUNGEON_MATERIAL_SHOP_ITEM,		// ���³ǲ����̵�ϡ����Ʒ
	SYS_BEHAVIOR_DAILY_DUNGEON_MATERIAL_EQUIP_ITEM,		// ���³�װ���̵�ϡ����Ʒ
	SYS_BEHAVIOR_DAILY_DUNGEON_MATERIAL_JEWELRY_ITEM,	// ���³��鱦�̵�ϡ����Ʒ
	SYS_BEHAVIOR_DAILY_DUNGEON_MONSTER_DROP,	// ÿ�յ��³ǹ���������
	SYS_BEHAVIOR_DAILY_DUNGEON_CHEAT_DEVIL,		// �����³����״���
	SYS_BEHAVIOR_DAILY_DUNGEON_AREA_INDEX,		// ÿ�յ��³Ƿ������
	SYS_BEHAVIOR_DAILY_DUNGEON_GUILD_INDEX,		// ÿ�չ�����³�
	SYS_BEHAVIOR_DAILY_DUNGEON_GUARANTEE_DROP,	// ÿ�յ��³Ǳ��׵���
	SYS_BEHAVIOR_DAILY_VETERAN_RETURN_DROP,		// ���³ǻع����
	SYS_BEHAVIOR_DAILY_DUNGEON_HIDDEN_ROOM_FLAG,// �Ƿ��������³����ط���
	SYS_BEHAVIOR_DAILY_ITEM_GET_NUM,			// ��Ʒ�������
	SYS_BEHAVIOR_DAILY_TEAM_COPY_INDEX,			// ÿ���ŶӸ���
	SYS_BEHAVIOR_DAILY_END,
	

	// ÿ�ܵ�����
	SYS_BEHAVIOR_WEEKLY_BEGIN = 2000,
	SYS_BEHAVIOR_WEEKLY_RARE_ITEM,				// ϡ����Ʒ
	SYS_BEHAVIOR_WEEKLY_DUNGEON_MATERIAL_SHOP_ITEM,		// ���³ǲ����̵�ϡ����Ʒ
	SYS_BEHAVIOR_WEEKLY_DUNGEON_EQUIP_SHOP_ITEM,		// ���³�װ���̵�ϡ����Ʒ
	SYS_BEHAVIOR_WEEKLY_DUNGEON_JEWELRY_SHOP_ITEM,		// ���³��鱦�̵�ϡ����Ʒ
	SYS_BEHAVIOR_WEEKLY_DUNGEON_GUARANTEE_DROP,			// ÿ�ܵ��³Ǳ��׵���
	SYS_BEHAVIOR_WEEKLY_END,

	// ÿ�µ�����
	SYS_BEHAVIOR_MONTHLY_BEGIN = 3000,
	SYS_BEHAVIOR_MONTHLY_RARE_ITEM,				// ϡ����Ʒ
	SYS_BEHAVIOR_MONTHLY_DUNGEON_MATERIAL_SHOP_ITEM,	// ���³ǲ����̵�ϡ����Ʒ
	SYS_BEHAVIOR_MONTHLY_DUNGEON_EQUIP_SHOP_ITEM,		// ���³�װ���̵�ϡ����Ʒ
	SYS_BEHAVIOR_MONTHLY_DUNGEON_JEWELRY_SHOP_ITEM,		// ���³��鱦�̵�ϡ����Ʒ
	SYS_BEHAVIOR_MONTHLY_DUNGEON_GUARANTEE_DROP,		// ÿ�µ��³Ǳ��׵���
	SYS_BEHAVIOR_MONTHLY_END,

	// �ܵ�����
	SYS_BEHAVIOR_TOTAL_BEGIN = 4000,
	SYS_BEHAVIOR_TOTAL_RARE_ITEM,				// ϡ����Ʒ
	SYS_BEHAVIOR_TOTAL_DUNGEON_GUARANTEE_DROP,	// ���³Ǳ��׵���
	SYS_BEHAVIOR_TOTAL_DUNGEON_CHEAT,			// ���³����״���
	SYS_BEHAVIOR_TOTAL_DROP_SOLUTION,			// ���䷽��
	SYS_BEHAVIOR_TOTAL_DUNGEON_MATERIAL_SHOP_ITEM,		// ���³ǲ����̵�ϡ����Ʒ
	SYS_BEHAVIOR_TOTAL_DUNGEON_EQUIP_SHOP_ITEM,			// ���³�װ���̵�ϡ����Ʒ
	SYS_BEHAVIOR_TOTAL_DUNGEON_JEWELRY_SHOP_ITEM,		// ���³��鱦�̵�ϡ����Ʒ
	SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_TIMESTAMP,			//��ֵ���ͽ���ʱ���
	SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_BUYCOUNT,			//��ֵ���͹������
	SYS_BEHAVIOR_TOTAL_RECHARGE_PUSH_LASTTIME,			//��ֵ�����ϴ��ɷ���ʱ���
	SYS_BEHAVIOR_TOTAL_RECHARGE_COOL_TIMESTAMP,			//��ֵ������ȴ����ʱ���
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

	// ΨһID
	ObjID_t					guid;
	// ӵ��������(��ɫ���˺�)
	SysBehaviorOwnerType	ownerType;
	// ӵ����ID�����Ϊ0��ô�����Ƿ������ܼ���
	ObjID_t					owner;
	// ��Ϊ
	SysBehavior				behavior;
	// ����1
	UInt32					param1;
	// ����
	UInt32					num;
	// ʱ��
	UInt32					recordTime;
};

// ϡ�п��Ƶ��߲�ͬʱ��ε���������
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

	// ����ID
	UInt32		itemId;
	// ��ʼʱ��
	UInt32		beginTime;
	// ����ʱ��
	UInt32		endTime;
	// ��������
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
	*@brief ��ʼ��
	*/
	bool Init();

	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief �����ݿ��������
	*/
	void LoadRecordFromDB(UInt32 accid, ObjID_t roleid);

public:
	/**
	*@brief ���ؼ�¼
	*/
	bool LoadSysRecord(SysRecord* record);

	/**
	*@brief ��ȡ��¼
	*/
	SysRecord* GetRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1);

	/**
	*@brief ��ȡ��¼�еļ���
	*/
	UInt32 GetRecordValue(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1);

	/**
	*@brief ���¼�����addNumΪ������ۼ���
	*/
	void UpdateRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int addNum);

	/**
	*@brief ���¼���
	*/
	void SetRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int num);

public:
	// ϡ�п������

	/**
	*@brief ϡ�п�����Ʒʣ������
	*/
	virtual UInt32 RareItemRemainNum(ObjID_t roleId, UInt32 itemId, UInt32 num);

	/**
	*@brief ϡ�п�����Ʒʣ������
	*/
	void RecordGotRareItem(ObjID_t roleId, UInt32 itemId, int num);
	void RecordGotRareItem(RareControlType type, ObjID_t roleId, UInt32 parm1, int num);

	/**
	*@brief ϡ�п��Ʒ���ʣ������
	*/
	virtual UInt32 RareDropSolutionRemainNum(ObjID_t roleId, UInt32 solutionId, UInt32 num);

	/**
	*@brief ��¼ϡ�п��Ʒ���
	*/
	void RecordGotRareDropSolution(ObjID_t roleId, UInt32 solutionId, int num);

	/**
	*@brief sysrecord��������
	*/
	void OnSysRecordDataReq(CLProtocol::SceneSysRecordDataReq& req);

public:
	/**
	*brief vip����������Ʒʣ������
	*/
	UInt32 VipLimitItemRemainNum(ObjID_t roleId, UInt32 itemId, UInt32 num, UInt32 vipLimitId, UInt8 vipLv);

	/**
	*@brief vip����������Ʒʣ������
	*/
	void RecordGotVipLimitItem(ObjID_t roleId, UInt32 itemId, int num);

private:
	/**
	*@brief ��Ӽ�¼
	*/
	void _AddRecord(SysRecord* pkRecord);

	/**
	*@brief ������¼
	*/
	SysRecord* _CreateRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, UInt32 num);

	/**
	*@brief �жϼ�¼�Ƿ����
	*/
	bool _IsTimeOut(SysBehavior behavior, UInt32 recordTime);

	/**
	*@brief ��¼�����ݿ�
	*/
	void _RecordToDB(SysRecord* record, bool isNew = false);

	/**
	*@brief ��ȡ��ͬ���͵�ϡ�п�������µ�ϡ�е���ʣ������
	*/
	UInt32 _GetRareItemRemainNum(SysBehavior behavior, ObjID_t roleId, UInt32 itemId, UInt32 num, UInt32 maxNum);
	UInt32 _GetRareItemRemainNum(SysBehavior behavior, ObjID_t roleId, UInt32 itemId, UInt32 maxNum);

	/**
	*@brief ���㲻ͬʱ���ϡ�е����������
	*/
	void _CountRareItemMaxNumByTime();
	void _CountRareItemMaxNumByTime(UInt32 itemId, UInt32 startTime, UInt32 endTime, UInt32 beginNum, UInt32 maxNum);

	/**
	*@brief ��ȡ��ǰʱ��ϡ�е����������,����-1����û������
	*/
	UInt32 _GetRareItemMaxNumByTime(UInt32 itemId);

	/**
	*@brief ��ȡ���յĿ�ʼ
	*/
	UInt32 _CountTodayStartTime();

	std::string _GetTimeStr(UInt32 time);

	/**
	*@brief ����ϡ�п������ͺ���Ϊ�����ȡ������Ϊ
	*/
	SysBehavior _GetSysBehaviorByRareControlType(RareControlType rareType, SysBehavior behaviorType);

private:
	Avalon::DailyTimer				m_dailyTimer;

	MapSysRecord					m_sysRecords;						// ��¼

	// ��ͬʱ��ε�ϡ�е�����������
	std::map<RareItemLimitByTime, UInt32>	m_rareItemNumLimitByTime;
};

#endif