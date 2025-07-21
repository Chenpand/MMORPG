#ifndef __ACTIVE_RECORD_MGR_H__
#define __ACTIVE_RECORD_MGR_H__

/*
	���������Ӫ���Ϊ�ļ�¼����
*/

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLDrawPrizeDataEntry.h>

// ��Ӫ���Ϊӵ��������
enum OpActivityBehaviorOwnerType
{
	// ��ɫ
	OP_ACTIVITY_BEHAVIOR_OWNER_ROLE,
	// �˺�
	OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT,
};

// ��Ӫ���Ϊ
enum OpActivityBehavior
{
	OP_ACTIVITY_BEHAVIOR_NULL,
	
	// ÿ�յ�����
	OP_ACTIVITY_BEHAVIOR_DAILY_BEGIN		= 1000,
	OP_ACTIVITY_BEHAVIOR_DAILY_CHARGE_DAY	= 1001,	// ÿ�����
	OP_ACTIVITY_BEHAVIOR_DAILY_SUBMIT_TASK	= 1002,	// ���������ÿ���콱����
	OP_ACTIVITY_BEHAVIOR_DAILY_DUNGEON_COUNT= 1003,	// ÿ�յ��³Ǵ���
	OP_ACTIVITY_BEHAVIOR_DAILY_ACCEPT_TASK	= 1004,	// �����ÿ�ս�ȡ����
	OP_ACTIVITY_BEHAVIOR_DAILY_ONLINE_COUNT = 1005,	// �ʺ�ÿ������ʱ��
	OP_ACTIVITY_BEHAVIOR_DAILY_ONLINE_TASK  = 1006,	// �ʺ�ÿ����������
	OP_ACTIVITY_BEHAVIOR_DAILY_END,

	// ÿ�ܵ�����
	OP_ACTIVITY_BEHAVIOR_WEEKLY_BEGIN = 2000,
	OP_ACTIVITY_BEHAVIOR_WEEKLY_SUMMER_CHALLENGE = 2001,	//������ս
	OP_ACTIVITY_BEHAVIOR_WEEKLY_SUBMIT_TASK = 2002,			//���������ÿ���콱����
	OP_ACTIVITY_BEHAVIOR_WEEKLY_END,

	// ÿ�µ�����
	OP_ACTIVITY_BEHAVIOR_MONTHLY_BEGIN = 3000,
	OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_MONTH_GET = 3001,		//���õ�ȯ�»��
	OP_ACTIVITY_BEHAVIOR_MONTHLY_END,

	// �ܵ�����
	OP_ACTIVITY_BEHAVIOR_TOTAL_BEGIN = 4000,
	OP_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK		= 4002,	// ����������ܵ��콱����
	OP_ITEM_ACTIVITY_BEHAVIOR_TOTAL_SUBMIT_TASK = 4003,	// �������նһ�������ܵ��콱����
	OP_RECHARGE_PUSH_BEHAVIOR_LAST_WEEK_BEFORE	= 4004,	// ��ֵ������Ϊ�������ܣ�
	OP_RECHARGE_PUSH_BEHAVIOR_LAST_WEEK			= 4005,	// ��ֵ������Ϊ�����ܣ�
	OP_RECHARGE_PUSH_BEHAVIOR_THIS_WEEK			= 4006,	// ��ֵ������Ϊ�����ܣ�
	OP_ACTIVITY_BEHAVIOR_GNOMECOIN_COUNT		= 4007,	// �ۼƵؾ���
	OP_ACTIVITY_BEHAVIOR_MONEY_CONSUME_COUNT	= 4008,	// �ۼƵ�ȯ����
	OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_TRANSFER_TIME = 4009,//���õ�ȯת��ʱ��
	OP_ACTIVITY_BEHAVIOR_TOTAL_SERVER_ITEM_NUM	= 4010,	// ȫ���齱�ص��߲�����������
	OP_ACTIVITY_BEHAVIOR_TOTAL_END,
};

// ��Ӫ�����
enum OpActivityParam
{
	OP_ACTIVITY_PARAM_FASHION_COMPOSE	= 1,	//ʱװ�ϳ�
	OP_ACTIVITY_PARAM_FASHION_COUNT		= 2,	//ʱװ����
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

	// ΨһID
	ObjID_t					guid;
	// ӵ��������(��ɫ���˺�)
	OpActivityBehaviorOwnerType	ownerType;
	// ӵ����ID�����Ϊ0��ô�����Ƿ������ܼ���
	ObjID_t					owner;
	// ��Ϊ
	OpActivityBehavior		behavior;
	// ����1
	UInt32					param1;
	// ����
	UInt32					num;
	// ʱ��
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

	void LoadRoleRecordFromDB(ObjID_t roleid);

	void LoadAcctRecordFromDB(UInt32 accid);
public:
	/**
	*@brief ���ؼ�¼
	*/
	bool LoadRecord(OpActivityRecord* record);

	/**
	*@brief ��ȡ��¼
	*/
	OpActivityRecord* GetRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1);

	/**
	*@brief ��ȡ��¼�еļ���
	*/
	UInt32 GetRecordValue(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1);

	/**
	*@brief ���¼�����addNumΪ������ۼ���
	*/
	void UpdateRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int addNum);

	/**
	*@brief ���¼���
	*/
	void SetRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int num);

public:
	/**
	*@brief ϡ�п�����Ʒʣ������
	*/
	virtual UInt32 RareRewardRemainNum(UInt32 rewardId, UInt32 num);

private:
	/**
	*@brief ��Ӽ�¼
	*/
	void _AddRecord(OpActivityRecord* record);

	/**
	*@brief ������¼
	*/
	OpActivityRecord* _CreateRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, UInt32 num);

	/**
	*@brief �жϼ�¼�Ƿ����
	*/
	bool _IsTimeOut(OpActivityBehavior behavior, UInt32 recordTime);

	/**
	*@brief ��¼�����ݿ�
	*/
	void _RecordToDB(OpActivityRecord* record, bool isNew = false);

	/**
	*@brief �����ݿ�ɾ��
	*/
	void _DeleteFromDB(OpActivityRecord* record);

private:
	MapOpActivityRecord				m_Records;						// ��¼
};

#endif