#ifndef _CL_ACTIVITY_DEFINE_H_
#define _CL_ACTIVITY_DEFINE_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include "CLTaskDefine.h"
#include "CLDropItemDefine.h"


// 运营活动发送包最大容量
#define OPACT_SEND_PACKET_MAX_SIZE 30

/**
*@brief 活动类型
*/
enum ActivityType
{
	AT_DAILY,	// 每日活动
	AT_ONCE,	// 一次性活动
	AT_WEEK,	// 每周活动
	AT_CIRCLE,	// 自定义时间循环
	AT_MONTH,	// 每月活动
};

/**
*@brief 活动状态
*/
enum ActivityState
{
	AS_END,		// 活动结束
	AS_IN,		// 活动中
	AS_PREPARE,	// 活动准备中
	AS_INVALID,	// 无效
};

//运营活动模板类别
enum OpActivityType
{
	OAT_NONE = 0,
	OAT_DAY_LOGIN = 10,				//每日登陆
	OAT_PLAYER_LEVEL_UP = 15,		//玩家升级
	OAT_BIND_PHONE = 16,			//手机绑定

	// 16 - 1000 提供给运营配置

	OAT_MALL_DISCOUNT_FOR_NEW_SERVER = 1000,//新服商城时装打折
	OAI_LEVEL_FIGHTING_FOR_NEW_SERVER,		//新服冲级赛竞争阶段
	OAI_LEVEL_SHOW_FOR_NEW_SERVER,			//新服冲级赛公示阶段

	OAT_DUNGEON_DROP_ACTIVITY		= 1100,	// 地下城掉落活动
	OAT_DUNGEON_EXP_ADDITION = 1200,		// 地下城结算经验加成
	OAT_PVP_PK_COIN = 1300,					// 决斗币奖励
	OAT_APPOINTMENT_OCCU = 1400,			// 预约职业活动
	OAT_HELL_TICKET_FOR_DRAW_PRIZE = 1500,	// 深渊票消耗得抽奖
	OAT_FATIGUE_FOR_BUFF = 1600,			// 疲劳消耗得BUFF
	OAT_FATIGUE_FOR_TOKEN_COIN = 1700,		// 疲劳消耗得代币
	OAT_FATIGUE_BURNING = 1800,				// 疲劳燃烧
	OAT_GAMBING = 1900,						// 夺宝活动
	OAT_DAILY_REWARD = 2000,				// 每日奖励
	OAT_MAGPIE_BRIDGE = 2100,				// 七夕鹊桥
	OAT_MONTH_CARD = 2200,					// 月卡活动
	OAT_BUFF_ADDITION = 2300,				// buff加成活动
	OAT_DUNGEON_DROP_RATE_ADDITION = 2400,	// 地下城掉落倍率增加
	OAT_CHANGE_FASHION_MERGE = 2500,		// 百变换装
	OAT_CHANGE_FASHION_EXCHANGE = 2600,		// 绝版兑换
	OAT_CHANGE_FASHION_WELFARE = 2700,	    // 换装福利 
	OAT_DUNGEON_RANDOM_BUFF = 2800,			// 地下城随机buff活动
	OAT_DUNGEON_CLEAR_GET_REWARD = 2900,	// 地下城通关得奖励
	OAT_CHANGE_FASHION_PURCHASE = 3000,     // 换装卷购买
	OAT_DAILY_BUFF = 3100,					// 每日buff活动
	OAT_STRENGTHEN_TICKET_SYNTHESIS = 3200,	// 强化券合成活动
	OAT_CHANGE_FASHION_ACT = 3300,			// 换装节活动
	OAT_MALL_BUY_GOT = 3400,				// 商城购买获得活动
	OAT_ARTIFACT_JAR = 3500,				// 神器罐子活动
	OAT_JAR_DRAW_LOTTERY = 3600,			// 罐子抽奖活动
	OAT_LIMIT_TIME_HELL = 3700,				// 限时深渊活动
	OAT_PET_GIFT_PACK = 3800,				// 宠物礼包活动
	OAT_MALL_BINDINGGOLD = 3900,			// 绑金商店活动

	OAT_BLACK_MARKET_SHOP = 4000,			// 黑市商人活动

	OAT_RETURN_PRESENT = 4100,				// 回归赠礼
	OAT_ENERGY_EXCHANGE = 4200,				// 精力兑换
	OAT_RETURN_CHARGE_REBATE = 4300,		// 回归充值返利
	OAT_CHALLENGE_HELL = 4400,				// 回归挑战深渊
	OAT_RETURN_PRIVILEGE = 4500,			// 回归特权
	OAT_WEEK_DEEP = 4600,					// 周常深渊
	OAT_BUY_PRRSENT = 4700,					// 购买赠送
	OAT_CUMULATE_LOGIN_REWARD = 4800,		// 累计登录领奖
	OAT_CUMULATEPASS_DUNGEON_REWARD = 4900,	// 累计通关地下城天数领奖
	OAT_LIMIT_TIME_GIFT_PACK = 5000,		// 限时礼包
	OAT_BET_HORSE = 5100,					// 赌马活动
	OAT_ARTIFACT_JAR_SHOW = 5200,			// 神器罐展示活动
	OAT_PROMOTE_EQUIP_STRENGTHEN_RATE = 5300,// 提升装备强化成功概率
	OAT_WEEK_SIGN_NEW_PLAYER = 5400,		// 新人周签到活动
	OAT_WEEK_SIGN_ACTIVITY = 5500,			// 活动周签到活动
	OAT_SUMMER_CHALLENGE_WEEKLY = 5600,		// 暑期挑战每周活动
	OAT_SUMMER_CHALLENGE_CUMULATE = 5700,	// 暑期挑战累计活动
	OAT_SECOND_ANNIVERSARY_PRAY = 5800,		// 两周年祈福
	OAT_FLYUP_GIFT = 5900,					// 飞升礼包
	OAT_TEAM_COPY_SUPPORT = 6000,			// 团本扶持
	OAT_CUMULATE_ONLINE = 6100,				// 累计在线
	OAT_HALLOWEEN_PUMPKIN_HELMET = 6300,	// 万圣节南瓜头
	OAT_NEW_YEAR_2020 = 6400,	// 元旦2020
	OAT_MONEY_CONSUME_REBATE = 6500,		// 点券消费返利
	OAT_CHALLENGE_HUB = 6600,               // 挑战者俱乐部
	OAT_EXCHANE_DISCOUNT = 6700,		    // 兑换折扣活动 
	OAT_NEW_SERVER_GIFT_DISCOUNT = 6800,    // 新服礼包折扣

	OAT_SPRING_FESTIVAL_RED_PACKET_RECV = 6900, // 春节红包领取活动
	OAT_SPRING_FESTIVAL_DUNGEON = 7000,			// 春节地下城
	OAT_SPRING_FESTIVAL_RED_PACKET_RAIN = 7100,	// 春节红包雨

	OAT_CHALLENGE_HUB_SCORE = 7200,         //挑战者累计积分活动
	OAT_SPRING_CHALLENGE = 7300,            //春季挑战活动
	OAT_SPRING_CHALLENGE_SCORE = 7400,      //春季挑战积分活动
	OAT_WEEK_SIGN_SPRING = 7500,            //2020春季每周签到

	OAT_ONLINE_GIFT= 7600,            //在线好礼
	OAT_PLANT_TREE = 7700,            //植树大挑战

	OAT_GNOME_COIN_GIFT = 7900,         //地精币回馈大礼

	//8000 ~ 50000 留给页游新做的运营活动，1.0新制作的从50000开始

	OAT_WHOLE_BARGAIN_DISCOUNT	= 50001,	//全民砍价折扣
	OAT_WHOLE_BARGAIN_SHOP		= 50002,	//全民砍价购买
	OAT_RECHARGE_CONSUME_REBATE	= 50003,	//充值消费活动		
	OAT_COMPLETE_PK = 50004,	//完成pk
	OAT_CHAMPION_GIFT = 50005,	//冠军赛礼包
	OAT_COMMON_AWARD = 50006,				//普通领奖
	OAT_DAILY_CHALLENGE = 50007,			//每日挑战


	OAT_ACCOUNT_ONLINE = 50008, //帐号在线领奖
	OAT_GIFT_RIGHT = 50009,		//礼遇特权

	OAT_SECRET_SELL = 50010,		//神秘卖场

	OAT_MONOPOLY = 60003,		//大富翁

	OAT_ABYESS_TICKET_REDUCE = 600010,  //深渊票减免
};

//运营活动id
enum OpActivityID
{
	OAID_BEGIN = 999,

	OAID_MALL_DISCOUNT_FOR_NEW_SERVER,	//新服商城时装打折
	OAID_LEVEL_FIGHTING_FOR_NEW_SERVER,	//新服冲级赛竞争阶段
	OAID_LEVEL_SHOW_FOR_NEW_SERVER,		//新服冲级赛公示阶段

	OAID_END,
};

//挑战者活动任务类型
enum OpActivityChallengeType
{
	OACT_MONSTER_ATTACK = 0,	//怪物攻城（通关）
	OACT_ELITE_DUNGEON = 1,		//精英地下城（需消耗精力）（通关）
	OACT_ABYESS_DUNGEON = 2,	//深渊地下城（参与）
	OACT_ASCENT_DUNGEON = 3,	//远古地下城（通关）
	OACT_TEAM_DUNGEON = 4,		//团队地下城（通关）
	OACT_3V3_PK = 5,			//3V3积分赛（参与）
	OACT_GUILD_BATTLE = 6,		//公会战（包括攻城战）（参与）
	OACT_GUILD_DUNGEON = 7,		//公会地下城（参与）
	OACT_2V2_PK = 8,			//2v2（参与）
	OACT_EVERYDAT_CHALLENGE = 9,//每日挑战活动
	OACT_UNDER_SEEK = 10,		//地宫寻宝(通关)
	OACT_MONOPOLY = 11,			//大富翁
	OACT_TEAM_DUNGEON_2 = 12,	//团本2团队地下城（通关）
};

//春季挑战活动任务类型
enum OpActivitySpringType
{
	OAST_LOGIN = 0,	//登陆大礼
	OAST_ONLINE = 1,		//在线好礼
	OAST_WEEK_SIGN = 2,	//每周签到
	OAST_CHALLENGE = 3,	//挑战者
	OAST_SPRING_HAPPY = 4,		//春日狂欢
	OAST_SPRING_TREE = 5,			//春季植树
};

//挑战者积分活动任务类型
enum OpActivityChallengeScoreType
{
	OACT_CHALLENGE_SCORE_DAY = 0,	//每日积分
	OACT_CHALLENGE_SCORE_TOTAL = 1,	//总积分

};

//2020春季周签到活动任务类型
enum OpActivityWeekSignSpringType
{
	OAWSST_EVERY_WEEK = 0,    //每周签到
	OAWSST_TOTAL_WEEK = 1,    //累计周签到
};


// 运营活动id与任务id转换值
static const UInt32 OPACT_ID_TRANS_VALUE = 1000;

/**
*@brief 活动信息
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

	//活动id
	UInt32		id;
	//活动名
	std::string	name;
	//状态，1 进行中，2 准备中
	UInt8		state;
	//需要等级
	UInt16		level;
	//准备时间
	UInt32		preTime;
	//开始时间
	UInt32		startTime;
	//到期时间
	UInt32		dueTime;
};


/**
*@brief 运营活动任务配置数据
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
	// 账户每日领奖限制次数
	UInt32 accountDailySubmitLimit;
	// 账户总领奖限制次数
	UInt32 accountTotalSubmitLimit;
	UInt32 resetType;
	// 新增3项
	UInt32 acceptType;
	UInt32 eventType;
	UInt32 subType;
	// 账户每周领奖限制次数
	UInt32 accountWeeklySubmitLimit;
	// 账号任务
	UInt32 accountTask;
};

/**
*@brief 运营活动任务配置数据
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
*@brief 运营活动配置数据来源
*/
enum OpActivityDataSource
{
	OADS_DB,	// 从数据库加载
	OADS_TABLE, // 从表格加载
	OADS_FROM_GLOBAL_ACTIVITY, // 从全局活动加载
	OADS_FROM_GM,	// 从gm处加载
};

/**
 *@brief 运营活动配置数据
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

	// 检查可变数据是否不同
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

	// 拷贝可变数据
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

	//唯一id
	UInt32  dataId;
	//状态
	UInt8	state;
	//模板类别(OpActivityType)
	UInt32	tmpType;
	//活动名
	std::string name;
	//活动标签
	UInt8	tag;
	//标签到期时间
	UInt32	tagEndTime;
	//准备时间
	UInt32  prepareTime;
	//开始时间
	UInt32	startTime;
	//结束时间
	UInt32	endTime;
	//开启等级限制(玩家等级)
	UInt16  playerLevelLimit;
	//最小等级
	UInt16	minLevel;
	//最大等级
	UInt16	maxLevel;
	//要求最早的开服时间
	UInt32  needStartServiceTime;
	//描述
	std::string desc;
	//规则描述
	std::string ruleDesc;
	//循环类型
	UInt8	circleType;
	//任务信息
	std::vector<OpActTaskData>	tasks;
	//任务描述
	std::string taskDesc;
	//扩展参数
	UInt32	parm;
	//扩展参数2
	std::vector<UInt32>	parm2;
	//数据来源（对应枚举 OpActivityDataSource)
	UInt8  dataSource;
	//logo
	std::string logoDesc;
	//页签id
	UInt32 tabId;
	//counter参数
	std::string countParam;
	//扩展参数3
	std::vector<UInt32> parm3;
	//活动预制体路径
	std::string  prefabPath;
	//宣传图路径
	std::string logoPath;
	//字符串参数
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

	// 名字
	std::string				name;
	// 是否在活动中
	UInt8					activity;
	// 活动ID
	UInt32					id;
	// 刷怪点类型
	UInt8					pointType;
	// 开始时间
	UInt32					startTime;
	// 结束
	UInt32					endTime;
	// 剩余数量
	UInt32					remainNum;
	// 下一轮时间
	UInt32					nextRollStartTime;
	// 可能掉落
	std::vector<DropItem>	drops;
};
typedef std::vector<ActivityMonsterInfo> VActivityMonsterInfo;

struct ActivityAccRecord
{
	ActivityAccRecord() : guid(0), actId(0), curNum(0), state(0) {}

	UInt64 guid;
	// 活动id
	UInt32 actId;
	// 次数
	UInt32 curNum;
	// 任务状态
	UInt32 state;
};

enum ActiveID
{
	AID_SIGN_IN = 3000,				//签到
	AID_SIGN_IN_BONUS = 3001,		//签到累计奖励
	AID_DAY_ONLINE = 5000,			//累计日在线
	AID_MONTH_CARD = 6000,			//月卡

	//七日
	AID_ONE_DAY = 7100,
	AID_TWO_DAY = 7200,
	AID_THREE_DAY = 7300,
	AID_FOUR_DAY = 7400,
	AID_FIVE_DAY = 7500,
	AID_SIX_DAY = 7600,
	AID_SEVEN_DAY = 7700,
	AID_SEVEN_CHARGE = 7800,

	//找回
	AID_MAKEUP_FATIGUE = 8100,
	AID_MAKEUP_DUNGEN = 8200,

	//充值
	AID_CHARGE_FIRST = 8300,	//首充
	AID_CHARGE_TOT = 8400,		//累充
	AID_CHARGE_DAY = 8500,		//日充
	AID_CHARGE_DAY_NEW = 8700,	//日充(新)

	//罐子活动
	AID_JAR_ACT_1 = 9001,
	AID_JAR_ACT_2 = 9002,
	AID_JAR_ACT_3 = 9003,
	AID_JAR_ACT_4 = 9004,
	AID_JAR_ACT_5 = 9005,
	AID_JAR_ACT_6 = 9006,
	AID_JAR_ACT_7 = 9007,
	AID_JAR_ACT_MAX = 9007,

	//oppo登录特权
	AID_LOGIN_PRIVILEGE = 12000,

	//道具兑换 added by yangwenhao
	AID_ITEM_CHANGE = 13000,

	//七日签到
	AID_SIGN_IN_SEVEN = 15000,
	//七日签到累计奖励
	AID_SIGN_IN_SEVEN_FINISH = 16000,
	//抽奖
	AID_DRAW_PRIZE = 17000,
	//琥珀特权
	AID_AMBER_PRIVILEGE = 20000,

	//排行榜
	AID_HUAWEI_YEAR_SORTLIST = 21100,
	AID_OPPO_YEAR_SORTLIST = 21200,
	AID_VIVO_YEAR_SORTLIST = 21300,
	AID_XIAOMI_YEAR_SORTLIST = 21400,
	AID_MEIZU_YEAR_SORTLIST = 21500,

	//vivo登陆特权
	AID_VIVO_LOGIN_PRIVILEGE = 23000,
	//账号充值返利
	AID_ACCOUNT_CHARGE_REBATE = 24000,
	//限时深渊
	AID_LIMIT_TIME_HELL = 25000,

	// 账号累计充值返利
	AID_ACCOUNT_TOTAL_CHARGE = 26000,
	// OPPO特权等级奖励
	AID_OPPO_VIP_REWARD = 27000,

	// 迷失地牢
	AID_LOST_DUEGON = 10088,
};

/**
*@brief 活动模板类型
*/
enum ActivityTemplateType
{
	ATT_NONE = 0,
	//道具兑换
	ATT_ITEM_EXCHANGE = 1,
	// 新道具兑换
	ATT_ITEM_EXCHANGE2 = 4,
};

/**
*@brief oppo用户琥珀等级
*/
enum OppoAmberLevel
{
	// 普通用户
	OAL_NONE = 0,
	// 绿珀1星
	OAL_GREEN_ONE,
	// 绿珀2星
	OAL_GREEN_TWO,
	// 绿珀3星
	OAL_GREEN_THREE,
	// 蓝珀1星
	OAL_BLUE_ONE,
	// 蓝珀2星
	OAL_BLUE_TWO,
	// 蓝珀3星
	OAL_BLUE_THREE,
	// 金珀1星
	OAL_GOLD_ONE,
	// 金珀2星
	OAL_GOLD_TWO,
	// 金珀3星
	OAL_GOLD_THREE,
	// 华贵红珀
	OAL_RED,
	// 至尊紫珀
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
	// 老兵回归
	ACTEM_VETERAN_RETURN,
	// 新回归
	ACTEM_NEW_RETURN,
	// 新人周签到
	ACTEM_WEEK_SIGN_NEW_PLAYER,
};

// 祈福活动BUFF类型
enum PrayActivityBuffType
{
	PABT_INVALID = 0,
	// 合成强化券降低材料
	PABT_SYNTH_STREN_TICKET_REDUCE_MAT,
	// 掉落加成
	PABT_DUNGEON_DORP_ADD,
};

// 周签到活动
enum WeekSignActType
{
	// 每日签到
	WEEK_SIGN_ACT_DAILY	 = 1,
	// 活跃度
	WEEK_SIGN_ACT_ACTIVE = 2,
	// 通关地下城
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

// 周签到数据
struct WeekSignData
{
	AVALON_DEFINE_OBJECTPOOL(WeekSignData, Avalon::NullMutex)

	WeekSignData() : actId(0), weekTime(0), weekNum(0) {}

	// 活动id
	UInt32 actId;
	// 当前周时间
	UInt32 weekTime;
	// 签到周数
	UInt32 weekNum;
	// 已领取的周奖励
	std::vector<UInt32> weekIdVec;
	// 宝箱奖励
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

// 两周年祈福活动BUFF类型
enum SecondAnniversaryPrayActivityBuffType
{
	SAPABT_INVALID = 0,
	//地下城经验加成
	SAPABT_DUNGEON_EXP_ADD = 1,
	//深渊挑战门票消耗减少
	SAPABT_HELL_TICKET_REDUCE = 2,
	//远古挑战门票消耗减少
	SAPABT_YUANGU_TICKET_REDUCE = 3,
	//虚空裂缝次数增加
	SAPABT_XUKONG_TIMES_ADD = 4,
	//混沌掉落个数增加
	SAPABT_HUNDUN_DROP_ADD = 5,
	//深渊地下城史诗装备掉落概率提升
	SAPABT_HELL_EPIC_EQUIP_DROP_ADD = 6,
	//远古掉落概率提升
	SAPABT_YUANGU_PINK_EQUIP_DROP_ADD = 7,
	//地下城BUFF加成
	SAPABT_DUNGEON_BUFF_ADD = 8,
};

// 元旦2020，活动类型
enum NewYear2020Type
{
	// 每日登录
	NEW_YEAR_2020_LOGIN = 1,
	// 每日在线
	NEW_YEAR_2020_ONLINE = 2,
	// 每日活跃度
	NEW_YEAR_2020_ACTIVE_VAL = 3,
	// 每日精力消耗
	NEW_YEAR_2020_ENERGY = 4,
	// 每日1v1决斗
	NEW_YEAR_2020_1V1 = 5,
};

enum SpringRedPacketTaskType
{
	//月卡
	SRPTT_MONTH_CARD = 1,
	//充值
	SRPTT_CHARGE = 2,
};

enum OnlineGiftTaskType
{
	//在线时间
	OGTT_TIME = 1,
	//在线天数
	SRPTT_DAY = 2,
};

//植树大挑战活动状态
enum PlantOpActSate
{
	POPS_NONE = 0, //未种植
	POPS_PLANTING = 1, //成长中
	POPS_CAN_APP = 2,  //可鉴定
	POPS_ALLGET = 3,	//已鉴定所有
};


//礼遇特权任务类型
enum OpActivityGiftRightTaskType
{
	OAGRTT_EVERY_DAY = 0,    //每日登陆任务
	OAGRTT_TOTAL_DAY = 1,    //累计登陆任务
	OAGRTT_LEND_EQUIP = 2,	 //租赁装备任务
	OAGRTT_TEAM_DUNGEON_NORMAL = 3, //团本普通地下城任务
	OAGRTT_TEAM_DUNGEON_HARD = 4, //团本困难地下城任务
	OAGRTT_TEAM_DUNGEON_2 = 5,		//团本2
};


#define ACTIVITY_LOST_DUNGEON   "lostDungeon"

//运营活动任务接受类型
enum OpActivityTaskAcceptType
{
	OATAT_ACCEPT_AUTO	= 0,    //自动接
	OATAT_ACCEPT_CANT	= 1,    //不能接
	OATAT_ACCEPT_MANUAL	= 2,	//手动接
};

#define ACTIVITY_BLACK_MARKET_1   "blackMarket1"
#define ACTIVITY_BLACK_MARKET_2		 "blackMarket2"
#define ACTIVITY_BLACK_MARKET_3		 "blackMarket3"
#endif
