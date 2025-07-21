#ifndef _CL_ACTIVITY_DEFINE_H_
#define _CL_ACTIVITY_DEFINE_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include "CLTaskDefine.h"
#include "CLDropItemDefine.h"


// ��Ӫ����Ͱ��������
#define OPACT_SEND_PACKET_MAX_SIZE 30

/**
*@brief �����
*/
enum ActivityType
{
	AT_DAILY,	// ÿ�ջ
	AT_ONCE,	// һ���Ի
	AT_WEEK,	// ÿ�ܻ
	AT_CIRCLE,	// �Զ���ʱ��ѭ��
	AT_MONTH,	// ÿ�»
};

/**
*@brief �״̬
*/
enum ActivityState
{
	AS_END,		// �����
	AS_IN,		// ���
	AS_PREPARE,	// �׼����
	AS_INVALID,	// ��Ч
};

//��Ӫ�ģ�����
enum OpActivityType
{
	OAT_NONE = 0,
	OAT_DAY_LOGIN = 10,				//ÿ�յ�½
	OAT_PLAYER_LEVEL_UP = 15,		//�������
	OAT_BIND_PHONE = 16,			//�ֻ���

	// 16 - 1000 �ṩ����Ӫ����

	OAT_MALL_DISCOUNT_FOR_NEW_SERVER = 1000,//�·��̳�ʱװ����
	OAI_LEVEL_FIGHTING_FOR_NEW_SERVER,		//�·��弶�������׶�
	OAI_LEVEL_SHOW_FOR_NEW_SERVER,			//�·��弶����ʾ�׶�

	OAT_DUNGEON_DROP_ACTIVITY		= 1100,	// ���³ǵ���
	OAT_DUNGEON_EXP_ADDITION = 1200,		// ���³ǽ��㾭��ӳ�
	OAT_PVP_PK_COIN = 1300,					// �����ҽ���
	OAT_APPOINTMENT_OCCU = 1400,			// ԤԼְҵ�
	OAT_HELL_TICKET_FOR_DRAW_PRIZE = 1500,	// ��ԨƱ���ĵó齱
	OAT_FATIGUE_FOR_BUFF = 1600,			// ƣ�����ĵ�BUFF
	OAT_FATIGUE_FOR_TOKEN_COIN = 1700,		// ƣ�����ĵô���
	OAT_FATIGUE_BURNING = 1800,				// ƣ��ȼ��
	OAT_GAMBING = 1900,						// �ᱦ�
	OAT_DAILY_REWARD = 2000,				// ÿ�ս���
	OAT_MAGPIE_BRIDGE = 2100,				// ��Ϧȵ��
	OAT_MONTH_CARD = 2200,					// �¿��
	OAT_BUFF_ADDITION = 2300,				// buff�ӳɻ
	OAT_DUNGEON_DROP_RATE_ADDITION = 2400,	// ���³ǵ��䱶������
	OAT_CHANGE_FASHION_MERGE = 2500,		// �ٱ任װ
	OAT_CHANGE_FASHION_EXCHANGE = 2600,		// ����һ�
	OAT_CHANGE_FASHION_WELFARE = 2700,	    // ��װ���� 
	OAT_DUNGEON_RANDOM_BUFF = 2800,			// ���³����buff�
	OAT_DUNGEON_CLEAR_GET_REWARD = 2900,	// ���³�ͨ�صý���
	OAT_CHANGE_FASHION_PURCHASE = 3000,     // ��װ����
	OAT_DAILY_BUFF = 3100,					// ÿ��buff�
	OAT_STRENGTHEN_TICKET_SYNTHESIS = 3200,	// ǿ��ȯ�ϳɻ
	OAT_CHANGE_FASHION_ACT = 3300,			// ��װ�ڻ
	OAT_MALL_BUY_GOT = 3400,				// �̳ǹ����û
	OAT_ARTIFACT_JAR = 3500,				// �������ӻ
	OAT_JAR_DRAW_LOTTERY = 3600,			// ���ӳ齱�
	OAT_LIMIT_TIME_HELL = 3700,				// ��ʱ��Ԩ�
	OAT_PET_GIFT_PACK = 3800,				// ��������
	OAT_MALL_BINDINGGOLD = 3900,			// ����̵�

	OAT_BLACK_MARKET_SHOP = 4000,			// �������˻

	OAT_RETURN_PRESENT = 4100,				// �ع�����
	OAT_ENERGY_EXCHANGE = 4200,				// �����һ�
	OAT_RETURN_CHARGE_REBATE = 4300,		// �ع��ֵ����
	OAT_CHALLENGE_HELL = 4400,				// �ع���ս��Ԩ
	OAT_RETURN_PRIVILEGE = 4500,			// �ع���Ȩ
	OAT_WEEK_DEEP = 4600,					// �ܳ���Ԩ
	OAT_BUY_PRRSENT = 4700,					// ��������
	OAT_CUMULATE_LOGIN_REWARD = 4800,		// �ۼƵ�¼�콱
	OAT_CUMULATEPASS_DUNGEON_REWARD = 4900,	// �ۼ�ͨ�ص��³������콱
	OAT_LIMIT_TIME_GIFT_PACK = 5000,		// ��ʱ���
	OAT_BET_HORSE = 5100,					// ����
	OAT_ARTIFACT_JAR_SHOW = 5200,			// ������չʾ�
	OAT_PROMOTE_EQUIP_STRENGTHEN_RATE = 5300,// ����װ��ǿ���ɹ�����
	OAT_WEEK_SIGN_NEW_PLAYER = 5400,		// ������ǩ���
	OAT_WEEK_SIGN_ACTIVITY = 5500,			// ���ǩ���
	OAT_SUMMER_CHALLENGE_WEEKLY = 5600,		// ������սÿ�ܻ
	OAT_SUMMER_CHALLENGE_CUMULATE = 5700,	// ������ս�ۼƻ
	OAT_SECOND_ANNIVERSARY_PRAY = 5800,		// ��������
	OAT_FLYUP_GIFT = 5900,					// �������
	OAT_TEAM_COPY_SUPPORT = 6000,			// �ű�����
	OAT_CUMULATE_ONLINE = 6100,				// �ۼ�����
	OAT_HALLOWEEN_PUMPKIN_HELMET = 6300,	// ��ʥ���Ϲ�ͷ
	OAT_NEW_YEAR_2020 = 6400,	// Ԫ��2020
	OAT_MONEY_CONSUME_REBATE = 6500,		// ��ȯ���ѷ���
	OAT_CHALLENGE_HUB = 6600,               // ��ս�߾��ֲ�
	OAT_EXCHANE_DISCOUNT = 6700,		    // �һ��ۿۻ 
	OAT_NEW_SERVER_GIFT_DISCOUNT = 6800,    // �·�����ۿ�

	OAT_SPRING_FESTIVAL_RED_PACKET_RECV = 6900, // ���ں����ȡ�
	OAT_SPRING_FESTIVAL_DUNGEON = 7000,			// ���ڵ��³�
	OAT_SPRING_FESTIVAL_RED_PACKET_RAIN = 7100,	// ���ں����

	OAT_CHALLENGE_HUB_SCORE = 7200,         //��ս���ۼƻ��ֻ
	OAT_SPRING_CHALLENGE = 7300,            //������ս�
	OAT_SPRING_CHALLENGE_SCORE = 7400,      //������ս���ֻ
	OAT_WEEK_SIGN_SPRING = 7500,            //2020����ÿ��ǩ��

	OAT_ONLINE_GIFT= 7600,            //���ߺ���
	OAT_PLANT_TREE = 7700,            //ֲ������ս

	OAT_GNOME_COIN_GIFT = 7900,         //�ؾ��һ�������

	//8000 ~ 50000 ����ҳ����������Ӫ���1.0�������Ĵ�50000��ʼ

	OAT_WHOLE_BARGAIN_DISCOUNT	= 50001,	//ȫ�񿳼��ۿ�
	OAT_WHOLE_BARGAIN_SHOP		= 50002,	//ȫ�񿳼۹���
	OAT_RECHARGE_CONSUME_REBATE	= 50003,	//��ֵ���ѻ		
	OAT_COMPLETE_PK = 50004,	//���pk
	OAT_CHAMPION_GIFT = 50005,	//�ھ������
	OAT_COMMON_AWARD = 50006,				//��ͨ�콱
	OAT_DAILY_CHALLENGE = 50007,			//ÿ����ս


	OAT_ACCOUNT_ONLINE = 50008, //�ʺ������콱
	OAT_GIFT_RIGHT = 50009,		//������Ȩ

	OAT_SECRET_SELL = 50010,		//��������

	OAT_MONOPOLY = 60003,		//����

	OAT_ABYESS_TICKET_REDUCE = 600010,  //��ԨƱ����
};

//��Ӫ�id
enum OpActivityID
{
	OAID_BEGIN = 999,

	OAID_MALL_DISCOUNT_FOR_NEW_SERVER,	//�·��̳�ʱװ����
	OAID_LEVEL_FIGHTING_FOR_NEW_SERVER,	//�·��弶�������׶�
	OAID_LEVEL_SHOW_FOR_NEW_SERVER,		//�·��弶����ʾ�׶�

	OAID_END,
};

//��ս�߻��������
enum OpActivityChallengeType
{
	OACT_MONSTER_ATTACK = 0,	//���﹥�ǣ�ͨ�أ�
	OACT_ELITE_DUNGEON = 1,		//��Ӣ���³ǣ������ľ�������ͨ�أ�
	OACT_ABYESS_DUNGEON = 2,	//��Ԩ���³ǣ����룩
	OACT_ASCENT_DUNGEON = 3,	//Զ�ŵ��³ǣ�ͨ�أ�
	OACT_TEAM_DUNGEON = 4,		//�Ŷӵ��³ǣ�ͨ�أ�
	OACT_3V3_PK = 5,			//3V3�����������룩
	OACT_GUILD_BATTLE = 6,		//����ս����������ս�������룩
	OACT_GUILD_DUNGEON = 7,		//������³ǣ����룩
	OACT_2V2_PK = 8,			//2v2�����룩
	OACT_EVERYDAT_CHALLENGE = 9,//ÿ����ս�
	OACT_UNDER_SEEK = 10,		//�ع�Ѱ��(ͨ��)
	OACT_MONOPOLY = 11,			//����
	OACT_TEAM_DUNGEON_2 = 12,	//�ű�2�Ŷӵ��³ǣ�ͨ�أ�
};

//������ս���������
enum OpActivitySpringType
{
	OAST_LOGIN = 0,	//��½����
	OAST_ONLINE = 1,		//���ߺ���
	OAST_WEEK_SIGN = 2,	//ÿ��ǩ��
	OAST_CHALLENGE = 3,	//��ս��
	OAST_SPRING_HAPPY = 4,		//���տ�
	OAST_SPRING_TREE = 5,			//����ֲ��
};

//��ս�߻��ֻ��������
enum OpActivityChallengeScoreType
{
	OACT_CHALLENGE_SCORE_DAY = 0,	//ÿ�ջ���
	OACT_CHALLENGE_SCORE_TOTAL = 1,	//�ܻ���

};

//2020������ǩ�����������
enum OpActivityWeekSignSpringType
{
	OAWSST_EVERY_WEEK = 0,    //ÿ��ǩ��
	OAWSST_TOTAL_WEEK = 1,    //�ۼ���ǩ��
};


// ��Ӫ�id������idת��ֵ
static const UInt32 OPACT_ID_TRANS_VALUE = 1000;

/**
*@brief ���Ϣ
*/
struct ActivityInfo
{
	AVALON_DEFINE_OBJECTPOOL(ActivityInfo, Avalon::NullMutex)

	ActivityInfo() : id(0), name(""), state(0), level(0), preTime(0), startTime(0), dueTime(0){}
	ActivityInfo(UInt32 _id, const std::string& _name, UInt16 _level, UInt32 _preTime, UInt32 _startTime, UInt32 _dueTime)
		: id(_id), name(_name), state(1), level(_level), preTime(_preTime), startTime(_startTime), dueTime(_dueTime){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & state & id & name & level & preTime & startTime & dueTime;
	}

	//�id
	UInt32		id;
	//���
	std::string	name;
	//״̬��1 �����У�2 ׼����
	UInt8		state;
	//��Ҫ�ȼ�
	UInt16		level;
	//׼��ʱ��
	UInt32		preTime;
	//��ʼʱ��
	UInt32		startTime;
	//����ʱ��
	UInt32		dueTime;
};


/**
*@brief ��Ӫ�������������
*/
struct OpActTaskData
{
	AVALON_DEFINE_OBJECTPOOL(OpActTaskData, Avalon::NullMutex)

	OpActTaskData()
	{
		dataId = 0;
		completeNum = 0;
		playerLevelLimit = 0;
		accountDailySubmitLimit = 0;
		accountTotalSubmitLimit = 0;
		resetType = 0;
		acceptType = 0;
		eventType = 0;
		subType = 0;
		accountWeeklySubmitLimit = 0;
		accountTask = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & dataId & completeNum & rewards & variables & variables2 & counters & taskName & varProgressName & playerLevelLimit & accountDailySubmitLimit & accountTotalSubmitLimit & resetType & acceptType & eventType & subType & accountWeeklySubmitLimit & accountTask;
	}

	void DecodeString(std::istringstream& is)
	{
		char split = 0;
		is >> dataId >> split >> completeNum;

		while (!is.eof())
		{
			is >> split;
			UInt32 varible;
			is >> varible;
			variables.push_back(varible);
		}
	}
	void EncodeString(std::ostringstream& os) const
	{
		os << dataId << ',' << completeNum;
		for (size_t i = 0; i < variables.size(); ++i)
		{
			os << ',' << variables[i];
		}
		
	}

	UInt32 GetOpActId() const
	{
		return dataId / OPACT_ID_TRANS_VALUE;
	}

	UInt32 dataId;
	UInt32	completeNum;
	std::vector<ItemReward>	rewards;
	std::vector<UInt32>	variables;
	std::vector<UInt32>	variables2;
	std::vector<CounterItem> counters;
	std::string taskName;
	std::vector<std::string> varProgressName;
	UInt16 playerLevelLimit;
	// �˻�ÿ���콱���ƴ���
	UInt32 accountDailySubmitLimit;
	// �˻����콱���ƴ���
	UInt32 accountTotalSubmitLimit;
	UInt32 resetType;
	// ����3��
	UInt32 acceptType;
	UInt32 eventType;
	UInt32 subType;
	// �˻�ÿ���콱���ƴ���
	UInt32 accountWeeklySubmitLimit;
	// �˺�����
	UInt32 accountTask;
};

/**
*@brief ��Ӫ�������������
*/
struct OpActTask
{
	typedef std::map<std::string, UInt32> ParamVarMap;
	AVALON_DEFINE_OBJECTPOOL(OpActTask, Avalon::NullMutex)

	OpActTask()
	{
		//memset(this, 0, sizeof(*this));
		uid = 0;
		ownerId = 0;
		dataId = 0;
		curNum = 0;
		state = 0;
		isOverdue = false;
	}

	OpActTask(UInt32 taskId)
	{
		//memset(this, 0, sizeof(*this));
		uid = 0;
		ownerId = 0;
		dataId = 0;
		curNum = 0;
		state = 0;
		isOverdue = false;
		dataId = taskId;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & dataId & curNum & state & params;
	}

	void DecodeString(std::istringstream& is)
	{
		char split = 0;
		UInt32 tmpState = 0;
		is >> dataId >> split >> curNum >> split >> tmpState;
		state = (UInt8)tmpState;
	}
	void EncodeString(std::ostringstream& os) const
	{
		if (state == TASK_OVER)
		{
			os << dataId << ',' << -1 << "," << (UInt32)state;
		}
		else
		{
			os << dataId << ',' << curNum << "," << (UInt32)state;
		}
		
	}

	bool AddCurNum(UInt32 addNum, UInt32 completeNum, bool overlay)
	{
		if (state == TASK_FINISHED ||
			state == TASK_OVER)
		{
			return false;
		}

		if (overlay)
		{
			curNum += addNum;
		}
		else
		{
			curNum = addNum;
		}
		
		if (curNum >= completeNum)
		{
			state = TASK_FINISHED;
			curNum = completeNum;
		}
		else
		{
			state = TASK_UNFINISH;
		}

		return true;
	}

	void Rest()
	{
		curNum = 0;
		state = TASK_INIT;
	}

	void RefreshState(UInt32 completeNum)
	{
		if ((Int32)curNum == -1)
		{
			state = TASK_OVER;
			curNum = completeNum;
		}
		else if (completeNum <= curNum)
		{
			state = TASK_FINISHED;
			curNum = completeNum;
		}
		else
		{
			state = TASK_UNFINISH;
		}
	}

	std::string LogStr() const
	{
		std::ostringstream oss;
		oss << "OpActTask(" << uid << ", " << dataId << ")'s status=" << (UInt32)state;
		return oss.str();
	}

	UInt32 GetOpActId()
	{
		return dataId / OPACT_ID_TRANS_VALUE;
	}

	UInt32 GetParamVar(const std::string& key)
	{
		auto elem = params.find(key);
		if (elem == params.end())
		{
			return 0;
		}
		return elem->second;
	}

	void SetParamVar(const std::string& key, UInt32 value)
	{
		params[key] = value;
	}

	ObjID_t uid;
	ObjID_t ownerId;
	UInt32	dataId;
	UInt32	curNum;
	UInt8	state;
	bool	isOverdue;
	ParamVarMap	params;
};

/**
*@brief ��Ӫ�����������Դ
*/
enum OpActivityDataSource
{
	OADS_DB,	// �����ݿ����
	OADS_TABLE, // �ӱ�����
	OADS_FROM_GLOBAL_ACTIVITY, // ��ȫ�ֻ����
	OADS_FROM_GM,	// ��gm������
};

/**
 *@brief ��Ӫ���������
 */
struct OpActivityRegInfo
{
	AVALON_DEFINE_OBJECTPOOL(OpActivityRegInfo, Avalon::NullMutex)

	OpActivityRegInfo()
	{
		dataId = 0;
		state = 0;
		tmpType = 0;
		name = "";
		tag = 0;
		tagEndTime = 0;
		prepareTime = 0;
		startTime = 0;
		endTime = 0;
		desc = "";
		ruleDesc = "";
		circleType = 0;
		tasks.clear();
		taskDesc = "";
		parm = 0;
		dataSource = 0;
		minLevel = 0;
		maxLevel = MAX_LEVEL;
		playerLevelLimit = 0;
		needStartServiceTime = 0;
		tabId = 0;
		countParam = "";
		prefabPath = "";
		logoPath = "";
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & dataId & state & tmpType & name & tag & prepareTime & startTime & \
			endTime & desc & ruleDesc & circleType & tasks & taskDesc & parm & parm2 & \
			playerLevelLimit & logoDesc & countParam & parm3 & prefabPath & logoPath & strParams;
	}

	void copy(OpActivityRegInfo& info)
	{
		dataId = info.dataId;
		state = info.state;
		tmpType = info.tmpType;
		name = info.name;
		tag = info.tag;
		tagEndTime = info.tagEndTime;
		prepareTime = info.prepareTime;
		startTime = info.startTime;
		endTime = info.endTime;
		playerLevelLimit = info.playerLevelLimit;
		minLevel = info.minLevel;
		maxLevel = info.maxLevel;
		desc = info.desc;
		ruleDesc = info.ruleDesc;
		circleType = info.circleType;
		tasks = info.tasks;
		taskDesc = info.taskDesc;
		parm = info.parm;
		dataSource = info.dataSource;
		parm2 = info.parm2;
		needStartServiceTime = info.needStartServiceTime;
		logoDesc = info.logoDesc;
		tabId = info.tabId;
		countParam = info.countParam;
		parm3 = info.parm3;
		prefabPath = info.prefabPath;
		logoPath = info.logoPath;
		strParams = info.strParams;
	}

	bool operator!=(const OpActivityRegInfo& rhs)
	{
		if (state != rhs.state)
			return true;

		if (desc != rhs.desc)
			return true;

		if (ruleDesc != rhs.ruleDesc)
			return true;

		if (taskDesc != rhs.taskDesc)
			return true;

		if (logoDesc != rhs.logoDesc)
			return true;
		
		if (name != rhs.name)
			return true;

		if (tasks.size() != rhs.tasks.size())
			return true;

		if (prepareTime != rhs.prepareTime)
			return true;

		if (startTime != rhs.startTime)
			return true;

		if (endTime != rhs.endTime)
			return true;

		if (tabId != rhs.tabId)
			return true;

		if (countParam != rhs.countParam)
			return true;

		if (prefabPath != rhs.prefabPath)
			return true;

		if (logoPath != rhs.logoPath)
		{
			return true;
		}
		return false;
	}

	// ���ɱ������Ƿ�ͬ
	bool CheckUnlikeOfChangeableData(const OpActivityRegInfo& rhs)
	{
		if (name != rhs.name)
			return true;

		if (prepareTime != rhs.prepareTime)
			return true;

		if (startTime != rhs.startTime)
			return true;

		if (endTime != rhs.endTime)
			return true;

		if (playerLevelLimit != rhs.playerLevelLimit)
			return true;

		if (desc != rhs.desc)
			return true;

		if (ruleDesc != rhs.ruleDesc)
			return true;

		if (taskDesc != rhs.taskDesc)
			return true;

		if (logoDesc != rhs.logoDesc)
			return true;

		if (tabId != rhs.tabId)
			return true;

		if (parm != rhs.parm)
			return true;

		if (parm2 != rhs.parm2)
			return true;

		if (parm3 != rhs.parm3)
			return true;

		if (prefabPath != rhs.prefabPath)
			return true;

		if (logoPath != rhs.logoPath)
		{
			return true;
		}
		return false;
	}

	// �����ɱ�����
	void CopyChangeableData(const OpActivityRegInfo& rhs)
	{
		name = rhs.name;
		prepareTime = rhs.prepareTime;
		startTime = rhs.startTime;
		endTime = rhs.endTime;
		playerLevelLimit = rhs.playerLevelLimit;
		desc = rhs.desc;
		ruleDesc = rhs.ruleDesc;
		taskDesc = rhs.taskDesc;
		logoDesc = rhs.logoDesc;
		tabId = rhs.tabId;
		parm = rhs.parm;
		parm2 = rhs.parm2;
		parm3 = rhs.parm3;
		prefabPath = rhs.prefabPath;
		logoPath = rhs.logoPath;
		strParams = rhs.strParams;
	}

	std::string LogStr() const
	{
		std::ostringstream oss;
		oss << "OpAct(" << dataId << ", " << name << ")'s status=" << (UInt32)state;
		return oss.str();
	}

	//Ψһid
	UInt32  dataId;
	//״̬
	UInt8	state;
	//ģ�����(OpActivityType)
	UInt32	tmpType;
	//���
	std::string name;
	//���ǩ
	UInt8	tag;
	//��ǩ����ʱ��
	UInt32	tagEndTime;
	//׼��ʱ��
	UInt32  prepareTime;
	//��ʼʱ��
	UInt32	startTime;
	//����ʱ��
	UInt32	endTime;
	//�����ȼ�����(��ҵȼ�)
	UInt16  playerLevelLimit;
	//��С�ȼ�
	UInt16	minLevel;
	//���ȼ�
	UInt16	maxLevel;
	//Ҫ������Ŀ���ʱ��
	UInt32  needStartServiceTime;
	//����
	std::string desc;
	//��������
	std::string ruleDesc;
	//ѭ������
	UInt8	circleType;
	//������Ϣ
	std::vector<OpActTaskData>	tasks;
	//��������
	std::string taskDesc;
	//��չ����
	UInt32	parm;
	//��չ����2
	std::vector<UInt32>	parm2;
	//������Դ����Ӧö�� OpActivityDataSource)
	UInt8  dataSource;
	//logo
	std::string logoDesc;
	//ҳǩid
	UInt32 tabId;
	//counter����
	std::string countParam;
	//��չ����3
	std::vector<UInt32> parm3;
	//�Ԥ����·��
	std::string  prefabPath;
	//����ͼ·��
	std::string logoPath;
	//�ַ�������
	std::vector<std::string> strParams;
};

typedef std::vector<ActivityInfo>	ActivityVec;
typedef std::map<ObjID_t, ActivityVec>	TribeActivityMap;

typedef std::map<UInt32, OpActivityRegInfo>	OpActRegMap;
typedef std::vector<OpActTask*>	OpActTaskVec;
typedef std::map<UInt32, OpActTaskVec>	OpActTasksMap;

struct ActivityMonsterInfo
{
	ActivityMonsterInfo() : activity(0), id(0), pointType(0), startTime(0), endTime(0), remainNum(0), nextRollStartTime(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & name & activity & id & pointType & startTime & endTime & remainNum & nextRollStartTime & drops;
	}

	// ����
	std::string				name;
	// �Ƿ��ڻ��
	UInt8					activity;
	// �ID
	UInt32					id;
	// ˢ�ֵ�����
	UInt8					pointType;
	// ��ʼʱ��
	UInt32					startTime;
	// ����
	UInt32					endTime;
	// ʣ������
	UInt32					remainNum;
	// ��һ��ʱ��
	UInt32					nextRollStartTime;
	// ���ܵ���
	std::vector<DropItem>	drops;
};
typedef std::vector<ActivityMonsterInfo> VActivityMonsterInfo;

struct ActivityAccRecord
{
	ActivityAccRecord() : guid(0), actId(0), curNum(0), state(0) {}

	UInt64 guid;
	// �id
	UInt32 actId;
	// ����
	UInt32 curNum;
	// ����״̬
	UInt32 state;
};

enum ActiveID
{
	AID_SIGN_IN = 3000,				//ǩ��
	AID_SIGN_IN_BONUS = 3001,		//ǩ���ۼƽ���
	AID_DAY_ONLINE = 5000,			//�ۼ�������
	AID_MONTH_CARD = 6000,			//�¿�

	//����
	AID_ONE_DAY = 7100,
	AID_TWO_DAY = 7200,
	AID_THREE_DAY = 7300,
	AID_FOUR_DAY = 7400,
	AID_FIVE_DAY = 7500,
	AID_SIX_DAY = 7600,
	AID_SEVEN_DAY = 7700,
	AID_SEVEN_CHARGE = 7800,

	//�һ�
	AID_MAKEUP_FATIGUE = 8100,
	AID_MAKEUP_DUNGEN = 8200,

	//��ֵ
	AID_CHARGE_FIRST = 8300,	//�׳�
	AID_CHARGE_TOT = 8400,		//�۳�
	AID_CHARGE_DAY = 8500,		//�ճ�
	AID_CHARGE_DAY_NEW = 8700,	//�ճ�(��)

	//���ӻ
	AID_JAR_ACT_1 = 9001,
	AID_JAR_ACT_2 = 9002,
	AID_JAR_ACT_3 = 9003,
	AID_JAR_ACT_4 = 9004,
	AID_JAR_ACT_5 = 9005,
	AID_JAR_ACT_6 = 9006,
	AID_JAR_ACT_7 = 9007,
	AID_JAR_ACT_MAX = 9007,

	//oppo��¼��Ȩ
	AID_LOGIN_PRIVILEGE = 12000,

	//���߶һ� added by yangwenhao
	AID_ITEM_CHANGE = 13000,

	//����ǩ��
	AID_SIGN_IN_SEVEN = 15000,
	//����ǩ���ۼƽ���
	AID_SIGN_IN_SEVEN_FINISH = 16000,
	//�齱
	AID_DRAW_PRIZE = 17000,
	//������Ȩ
	AID_AMBER_PRIVILEGE = 20000,

	//���а�
	AID_HUAWEI_YEAR_SORTLIST = 21100,
	AID_OPPO_YEAR_SORTLIST = 21200,
	AID_VIVO_YEAR_SORTLIST = 21300,
	AID_XIAOMI_YEAR_SORTLIST = 21400,
	AID_MEIZU_YEAR_SORTLIST = 21500,

	//vivo��½��Ȩ
	AID_VIVO_LOGIN_PRIVILEGE = 23000,
	//�˺ų�ֵ����
	AID_ACCOUNT_CHARGE_REBATE = 24000,
	//��ʱ��Ԩ
	AID_LIMIT_TIME_HELL = 25000,

	// �˺��ۼƳ�ֵ����
	AID_ACCOUNT_TOTAL_CHARGE = 26000,
	// OPPO��Ȩ�ȼ�����
	AID_OPPO_VIP_REWARD = 27000,

	// ��ʧ����
	AID_LOST_DUEGON = 10088,
};

/**
*@brief �ģ������
*/
enum ActivityTemplateType
{
	ATT_NONE = 0,
	//���߶һ�
	ATT_ITEM_EXCHANGE = 1,
	// �µ��߶һ�
	ATT_ITEM_EXCHANGE2 = 4,
};

/**
*@brief oppo�û�����ȼ�
*/
enum OppoAmberLevel
{
	// ��ͨ�û�
	OAL_NONE = 0,
	// ����1��
	OAL_GREEN_ONE,
	// ����2��
	OAL_GREEN_TWO,
	// ����3��
	OAL_GREEN_THREE,
	// ����1��
	OAL_BLUE_ONE,
	// ����2��
	OAL_BLUE_TWO,
	// ����3��
	OAL_BLUE_THREE,
	// ����1��
	OAL_GOLD_ONE,
	// ����2��
	OAL_GOLD_TWO,
	// ����3��
	OAL_GOLD_THREE,
	// �������
	OAL_RED,
	// ��������
	OAL_PURPLE,
};

inline UInt32 GetTaskIdByOppoAmberLevel(UInt8 level)
{
	switch ((OppoAmberLevel)level)
	{
	case OAL_NONE:
		return 20001;

	case OAL_GREEN_ONE:
	case OAL_GREEN_TWO:
	case OAL_GREEN_THREE:
		return 20002;

	case OAL_BLUE_ONE:
	case OAL_BLUE_TWO:
	case OAL_BLUE_THREE:
		return 20003;

	case OAL_GOLD_ONE:
	case OAL_GOLD_TWO:
	case OAL_GOLD_THREE:
		return 20004;

	case OAL_RED:
		return 20005;

	case OAL_PURPLE:
		return 20006;

	default: return 0;
	}
}

#define SEVEN_END_DAY		7
#define SEVEN_CLOSE_DAY		10

static const char* const OPACT_COMMON_VAR_KEY = "common_var";
static const char* const OPACT_COMMON_VAR = "common_var=";
static const std::string OPACT_TIMES_VAR_KEY = "times_var";
static const std::string OPACT_UPDATETIME_VAR_KEY = "updatetime_var";
static const std::string OPACT_CHARGE_NUM_VAR_KEY = "chargeNum";
static const std::string OPACT_CONSUME_NUM_VAR_KEY = "consumeNum";

enum ActivitySystem
{
	ACTEM_NONE = 0,
	// �ϱ��ع�
	ACTEM_VETERAN_RETURN,
	// �»ع�
	ACTEM_NEW_RETURN,
	// ������ǩ��
	ACTEM_WEEK_SIGN_NEW_PLAYER,
};

// ���BUFF����
enum PrayActivityBuffType
{
	PABT_INVALID = 0,
	// �ϳ�ǿ��ȯ���Ͳ���
	PABT_SYNTH_STREN_TICKET_REDUCE_MAT,
	// ����ӳ�
	PABT_DUNGEON_DORP_ADD,
};

// ��ǩ���
enum WeekSignActType
{
	// ÿ��ǩ��
	WEEK_SIGN_ACT_DAILY	 = 1,
	// ��Ծ��
	WEEK_SIGN_ACT_ACTIVE = 2,
	// ͨ�ص��³�
	WEEK_SIGN_ACT_DUNGEON = 3,
};

struct WeekSignBox
{
	AVALON_DEFINE_OBJECTPOOL(WeekSignBox, Avalon::NullMutex)

	WeekSignBox() : opActId(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & opActId & itemVec;
	}

	UInt32 opActId;
	std::vector<ItemReward> itemVec;
};

// ��ǩ������
struct WeekSignData
{
	AVALON_DEFINE_OBJECTPOOL(WeekSignData, Avalon::NullMutex)

	WeekSignData() : actId(0), weekTime(0), weekNum(0) {}

	// �id
	UInt32 actId;
	// ��ǰ��ʱ��
	UInt32 weekTime;
	// ǩ������
	UInt32 weekNum;
	// ����ȡ���ܽ���
	std::vector<UInt32> weekIdVec;
	// ���佱��
	std::vector<WeekSignBox> signBoxVec;

	enum 
	{
		WEEK_TIME_SAVE = 1 << 0,
		WEEK_NUM_SAVE = 1 << 1,
		ID_VEC_SAVE = 1 << 2,
		BOX_VEC_SAVE = 1 << 3,
	};

	void Clear()
	{
		actId = 0;
		weekTime = 0;
		weekNum = 0;
		weekIdVec.clear();
		signBoxVec.clear();
	}

	std::string DeCodeWeekId()
	{
		std::ostringstream os;
		for (auto& id : weekIdVec)
		{
			os << id << "|";
		}

		return os.str();
	}

	void EncodeWeekId(std::string& str)
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");
		for (auto& param : strs)
		{
			if (param.empty())
				continue;

			UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(param);
			weekIdVec.push_back(id);
		}
	}

	std::string DeCodeBoxData()
	{
		std::ostringstream os;
		for (auto& it : signBoxVec)
		{
			if (it.opActId == 0 || it.itemVec.empty())
			{
				continue;
			}

			os << it.opActId << ",";

			for (auto& tt : it.itemVec)
			{
				os << tt.id << "," << tt.num;
			}

			os << "|";
		}

		return os.str();
	}

	void EncodeBoxData(std::string& str)
	{
		std::vector<std::string> param_1;
		Avalon::StringUtil::Split(str, param_1, "|");
		for (auto& iter : param_1)
		{
			if (iter.empty())
				continue;

			std::vector<std::string> param_2;
			Avalon::StringUtil::Split(iter, param_2, ",");

			if (param_2.size() < 3)
			{
				ErrorLogStream << "EncodeBoxData Error!" << LogStream::eos;
				continue;
			}

			WeekSignBox signBox;
			signBox.opActId = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);

			for (UInt32 k = 1; k < param_2.size();k+=2)
			{
				ItemReward itemReward;
				try
				{
					itemReward.id = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[k]);
					itemReward.num = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[k + 1]);
				}
				catch (std::string str)
				{
					ErrorLogStream << str << LogStream::eos;
					continue;
				}
				
				signBox.itemVec.push_back(itemReward);
			}

			signBoxVec.push_back(signBox);
		}
	}
};

// ���������BUFF����
enum SecondAnniversaryPrayActivityBuffType
{
	SAPABT_INVALID = 0,
	//���³Ǿ���ӳ�
	SAPABT_DUNGEON_EXP_ADD = 1,
	//��Ԩ��ս��Ʊ���ļ���
	SAPABT_HELL_TICKET_REDUCE = 2,
	//Զ����ս��Ʊ���ļ���
	SAPABT_YUANGU_TICKET_REDUCE = 3,
	//����ѷ��������
	SAPABT_XUKONG_TIMES_ADD = 4,
	//��������������
	SAPABT_HUNDUN_DROP_ADD = 5,
	//��Ԩ���³�ʷʫװ�������������
	SAPABT_HELL_EPIC_EQUIP_DROP_ADD = 6,
	//Զ�ŵ����������
	SAPABT_YUANGU_PINK_EQUIP_DROP_ADD = 7,
	//���³�BUFF�ӳ�
	SAPABT_DUNGEON_BUFF_ADD = 8,
};

// Ԫ��2020�������
enum NewYear2020Type
{
	// ÿ�յ�¼
	NEW_YEAR_2020_LOGIN = 1,
	// ÿ������
	NEW_YEAR_2020_ONLINE = 2,
	// ÿ�ջ�Ծ��
	NEW_YEAR_2020_ACTIVE_VAL = 3,
	// ÿ�վ�������
	NEW_YEAR_2020_ENERGY = 4,
	// ÿ��1v1����
	NEW_YEAR_2020_1V1 = 5,
};

enum SpringRedPacketTaskType
{
	//�¿�
	SRPTT_MONTH_CARD = 1,
	//��ֵ
	SRPTT_CHARGE = 2,
};

enum OnlineGiftTaskType
{
	//����ʱ��
	OGTT_TIME = 1,
	//��������
	SRPTT_DAY = 2,
};

//ֲ������ս�״̬
enum PlantOpActSate
{
	POPS_NONE = 0, //δ��ֲ
	POPS_PLANTING = 1, //�ɳ���
	POPS_CAN_APP = 2,  //�ɼ���
	POPS_ALLGET = 3,	//�Ѽ�������
};


//������Ȩ��������
enum OpActivityGiftRightTaskType
{
	OAGRTT_EVERY_DAY = 0,    //ÿ�յ�½����
	OAGRTT_TOTAL_DAY = 1,    //�ۼƵ�½����
	OAGRTT_LEND_EQUIP = 2,	 //����װ������
	OAGRTT_TEAM_DUNGEON_NORMAL = 3, //�ű���ͨ���³�����
	OAGRTT_TEAM_DUNGEON_HARD = 4, //�ű����ѵ��³�����
	OAGRTT_TEAM_DUNGEON_2 = 5,		//�ű�2
};


#define ACTIVITY_LOST_DUNGEON   "lostDungeon"

//��Ӫ������������
enum OpActivityTaskAcceptType
{
	OATAT_ACCEPT_AUTO	= 0,    //�Զ���
	OATAT_ACCEPT_CANT	= 1,    //���ܽ�
	OATAT_ACCEPT_MANUAL	= 2,	//�ֶ���
};

#define ACTIVITY_BLACK_MARKET_1   "blackMarket1"
#define ACTIVITY_BLACK_MARKET_2		 "blackMarket2"
#define ACTIVITY_BLACK_MARKET_3		 "blackMarket3"
#endif
