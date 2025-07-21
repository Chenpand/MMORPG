#ifndef _CL_ACTIVEEVENT_DATAENTRY_H_
#define _CL_ACTIVEEVENT_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


//活动data
class ActiveTaskDataEntry : public Avalon::DataEntry
{
public:
	ActiveTaskDataEntry();
	~ActiveTaskDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 20; };

public:
	//活动任务id
	UInt32	id;
	//奖励
	std::vector<ItemReward>	rewards;
	//双倍VIP等级
	UInt32	vipLv;
	//活动模板id
	UInt32  templateID;
	//前置任务id
	UInt32  preTaskID;
	//后置任务id
	UInt32  nextTaskID;
	//排序号
	UInt32 sortPriority;
	//领取花费
	UInt32 takeCost;
	//拓展参数
	UInt32 param1;
	//对应副本id
	UInt32 dungeonId;
	//是否废弃
	UInt32 isAbandon;
	//消耗的道具
	std::vector<ItemReward> consumeItems;
	//任务循坏次数
	Int64 taskCycleCount;
	//任务循坏计数key
	std::string taskCycleKey;
	//结束任务计数key
	std::string overTaskNumKey;
	//未结束任务计数key
	std::string failedTaskNumKey;
	//活动应用渠道
	std::string applyChannel;
	//任务归属（1：账号）
	UInt8 taskBelong;
	// 扩展参数2
	std::vector<UInt32> parm2;
	//账号限制次数
	UInt32 accountTotalSubmitLimit;
};

typedef std::map<UInt32, std::vector<ActiveTaskDataEntry*>> TempToActives;

class ActiveTaskDataEntryMgr : public Avalon::DataEntryMgrBase<ActiveTaskDataEntry>
	, public Avalon::Singleton<ActiveTaskDataEntryMgr>
{
public:
	ActiveTaskDataEntryMgr();
	~ActiveTaskDataEntryMgr();

public:
	bool AddEntry(ActiveTaskDataEntry* dataEntry);
	void GetActives(UInt32 tempId, std::vector<ActiveTaskDataEntry*>& vecActives);
private:
	TempToActives			m_actives;
};

//疲劳补偿data
class FatigueMakeUpData : public Avalon::DataEntry
{
public:
	FatigueMakeUpData();
	~FatigueMakeUpData();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	//keyid 等级
	UInt32	id;
	//低档找回经验
	UInt64	lowExp;
	//低档找回需要消耗货币ID
	UInt32	lowNeedMoneyID;
	//高档找回经验
	UInt64	hiExp;
	//高档找回需要消耗货币ID
	UInt32	hiNeedMoneyID;
	//疲劳补偿上限
	UInt32	fatigueMax;
	//vip疲劳补偿上限
	std::vector<UInt32>  vipMaxs;
};

class FatigueMakeUpDataMgr : public Avalon::DataEntryMgrBase<FatigueMakeUpData>
	, public Avalon::Singleton<FatigueMakeUpDataMgr>
{

public:
	FatigueMakeUpDataMgr();
	~FatigueMakeUpDataMgr();
};

//体力找回价格表
class FatigueMakeUpPrice : public Avalon::DataEntry
{
public:

	FatigueMakeUpPrice();
	~FatigueMakeUpPrice();

	UInt32 GetKey() const{ return id; }
	bool Read(Avalon::DataRow& row);

	//key
	UInt32 id;
	//疲劳闭区间
	std::vector<UInt32>	fatigueSection;
	//低档找回单价
	UInt32	lowPrice;
	//高档找回单价
	UInt32	hiPrice;
};

class FatigueMakeUpPriceMgr : public Avalon::DataEntryMgrBase<FatigueMakeUpPrice>
	, public Avalon::Singleton<FatigueMakeUpPriceMgr>
{

public:
	FatigueMakeUpPriceMgr();
	~FatigueMakeUpPriceMgr();

	UInt32	GetPrice(UInt32 fatigue, bool bLow);
};

//副本次数补偿data
class DungeonMakeUpData : public Avalon::DataEntry
{
public:
	DungeonMakeUpData();
	~DungeonMakeUpData();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);
	bool ReadSplitStr(Avalon::DataRow& row, std::vector<UInt64>& vec);
	bool ReadSplitStr(Avalon::DataRow& row, std::vector<UInt32>& vec);
public:
	UInt32	id;
	//等级
	UInt32	level;
	//对应地下城ID
	UInt32	dungeonID;
	//普通找回道具ID
	std::vector<UInt32>	normalItems;
	//单次普通找回补偿数量
	std::vector<UInt64>	normalNum;
	//完美找回道具ID
	std::vector<UInt32>	perfectItems;
	//单次付费找回补偿数量
	std::vector<UInt64>	costNum;
	//普通消耗金币
	UInt32	goldCost;
	//付费消耗点券
	UInt32	pointCost;
	//补偿上限
	UInt32	countMax;
	//vip补偿上限
	UInt32	vipCountMax;
	//vip所需等级
	UInt32	vipLevel;
};

class DungeonMakeUpDataMgr : public Avalon::DataEntryMgrBase<DungeonMakeUpData>
	, public Avalon::Singleton<DungeonMakeUpDataMgr>
{

public:
	DungeonMakeUpDataMgr();
	~DungeonMakeUpDataMgr();

public:
	bool AddEntry(DungeonMakeUpData* dataEntry);
	DungeonMakeUpData* GetData(UInt32 dungeonID, UInt32 level);

private:
	void GetDatas(UInt32 dungeonID, std::vector<DungeonMakeUpData*>& datas);

	std::map<UInt32, std::vector<DungeonMakeUpData*>>	m_dungeonIdToDatas;
};

//阶段礼包表
class GiftPhaseData : public Avalon::DataEntry
{
public:
	GiftPhaseData();
	~GiftPhaseData();

public:
	UInt32 GetKey()	const { return id; }
	bool Read(Avalon::DataRow& row);

	UInt32	id;
	//事件类别
	UInt8	eventType;
	//条件值
	UInt32	value;
	//下一档条件值
	UInt32	nextValue;
	//奖励
	std::vector<ItemReward>	rewards;
};

//阶段礼包管理
class GiftPhaseDataMgr : public Avalon::DataEntryMgrBase<GiftPhaseData>
	, public Avalon::Singleton<GiftPhaseDataMgr>
{
public:
	GiftPhaseDataMgr();
	~GiftPhaseDataMgr();

public:
	bool LoadData(const std::string& filename);
	bool AddEntry(GiftPhaseData* dataEntry);

	//获取type id的下一个礼包
	GiftPhaseData* GetNextGift(UInt8 type, UInt32 id);

private:
	std::map<UInt8, std::vector<GiftPhaseData*>>	m_giftsTypeMap;

};

/**
*@brief 运营活动数据项
*/
class OpActivityDataEntry : public Avalon::DataEntry
{
public:
	OpActivityDataEntry() {}
	~OpActivityDataEntry() {}

	UInt32 GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 31; };

public:
	// 运营活动id
	UInt32 id;
	// 运营模板类别
	UInt32 tmpType;
	// 活动名
	std::string name;
	// 活动标签---废弃
	UInt8 tag;
	// 标签到期时间---废弃
	UInt32 tagEngTime;
	// 准备时间
	UInt32 prepareTime;
	// 开始时间
	UInt32 startTime;
	// 结束时间
	UInt32 endTime;
	// 开启等级限制(玩家等级)
	UInt16 playerLevelLimit;
	// 最小等级
	UInt32	minLevel;
	// 最大等级
	UInt32	maxLevel;
	// 要求最早的开服时间
	UInt32  needStartServiceTime;
	// 新服（合服算新服）第几天开始
	std::string startTimeFromService;
	// 新服（合服算新服）第几天结束
	std::string endTimeFromService;
	// 新服（合服不算新服）第几天开始
	std::string startTimeFromFirstService;
	// 新服（合服不算新服）第几天结束
	std::string endTimeFromFirstService;
	// 开启间隔
	std::string	openInterval;
	// 关闭间隔
	std::string	closeInterval;
	// 描述
	std::string desc;
	// 规则描述
	std::string ruleDesc;
	// 循环类型---废弃
	UInt8 circleType;
	// 扩展参数
	UInt32 parm;
	// 扩展参数2
	std::vector<UInt32> parm2;
	// 任务集
	std::vector<UInt32> taskIds;
	// logo
	std::string logoDesc;
	// 超级新服活动标记
	UInt8 superNewServiceActFlag;
	// count参数
	std::string countParam;
	//扩展参数3
	std::vector<UInt32> parm3;
	//活动预制体路径
	std::string  prefabPath;
	//宣传图路径
	std::string  logoPath;
	//字符串参数
	std::vector<std::string> strParams;
};

class OpActivityDataEntryMgr : public Avalon::DataEntryMgrBase<OpActivityDataEntry>
	, public Avalon::Singleton<OpActivityDataEntryMgr>
{
public:
	OpActivityDataEntryMgr() {}
	~OpActivityDataEntryMgr() {}
};

/**
*@brief 运营活动任务数据项
*/
class OpActivityTaskDataEntry : public Avalon::DataEntry
{
public:
	OpActivityTaskDataEntry() {}
	~OpActivityTaskDataEntry() {}

	UInt32 GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 22; };

public:
	// 任务id
	UInt32 id;
	// 任务名字
	std::string taskName;
	// 运营活动id
	UInt32 opAvtivityId;
	// 运营活动类型
	UInt32 opActivityType;
	// 前置任务id
	UInt32 preTaskId;
	// 后置任务id
	UInt32 nextTaskId;
	// 任务完成计数
	UInt32 completeCount;
	// 任务奖励
	std::vector<ItemReward> rewards;
	// 任务描述
	std::string taskDesc;
	// 任务变量
	std::vector<UInt32>	variables;
	// 任务变量
	std::vector<UInt32>	variables2;
	// counter
	std::vector<CounterItem> counters;
	// 任务变量进度
	std::vector<std::string> varProgressName;
	// 开启等级限制(玩家等级)
	UInt16 playerLevelLimit;
	// 账号每日领奖次数
	UInt32 accountDailySubmitLimit;
	// 账号每日领奖次数
	UInt32 accountTotalSubmitLimit;
	// 刷新类型
	UInt32 restType;
	// 新增项
	UInt32 acceptType;
	UInt32 eventType;
	UInt32 subType;
	// 账户每周领奖限制次数
	UInt32 accountWeeklySubmitLimit;
	// 账号任务
	UInt32 accountTask;
};

/**
*@brief 运营活动任务数据自定义排序
*/
struct CustomLessOpActTaskData
{
	bool operator()(OpActivityTaskDataEntry* lhs, OpActivityTaskDataEntry* rhs)
	{
		return lhs->id < rhs->id;
	}
};

class OpActivityTaskDataEntryMgr : public Avalon::DataEntryMgrBase<OpActivityTaskDataEntry>
	, public Avalon::Singleton<OpActivityTaskDataEntryMgr>
{
public:
	OpActivityTaskDataEntryMgr() {}
	~OpActivityTaskDataEntryMgr() {}
};

/**
*@brief 活动模板表数据项
*/
class ActivityTemplateDataEntry : public Avalon::DataEntry
{
public:
	ActivityTemplateDataEntry() {}
	~ActivityTemplateDataEntry() {}

	UInt32 GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 2; };

public:
	// 活动id
	UInt32 id;
	// 活动类型
	UInt8 type;
};

class ActivityTemplateDataEntryMgr : public Avalon::DataEntryMgrBase<ActivityTemplateDataEntry>
	, public Avalon::Singleton<ActivityTemplateDataEntryMgr>
{
public:
	ActivityTemplateDataEntryMgr() {}
	~ActivityTemplateDataEntryMgr() {}

	bool AddEntry(ActivityTemplateDataEntry* entry);
	UInt8 GetActivityTemplateType(UInt32 activityId);

private:
	std::map<UInt32, UInt8> m_ActivityTempToType;
};


#define OPACT_FATIGUE_BURNING_VARIABLE_SIZE 4


/**
*@brief 活动系统配置表
*/
class ActivitySystemConfigDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return actSytem; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 8; };

public:
	// id(无作用)
	UInt16 id;
	// 活动系统
	UInt32 actSytem;
	// 运营活动列表
	std::vector<UInt32> opActList;
	// 商城道具列表
	std::vector<UInt32> mallItemList;
	// 活动列表
	std::vector<UInt32> actList;
	// 推送信息列表
	std::vector<UInt32> pushInfoList;
	// 是否关闭
	bool isClose;
	// 回归活动id
	UInt32 			returnActId;
};

typedef Avalon::DataEntryMgr<ActivitySystemConfigDataEntry> ActivitySystemConfigDataEntryMgr;

/**
*@brief 活动页签表
*/
class ActivityTabDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 3; };

public:
	// 页签id
	UInt32 id;
	// 页签名
	std::string name;
	// 页签下的活动id
	std::vector<UInt32> actIds;
};

typedef Avalon::DataEntryMgr<ActivityTabDataEntry> ActivityTabDataEntryMgr;


#endif
