#ifndef _CL_BET_HORSE_DATA_ENTRY_H_
#define _CL_BET_HORSE_DATA_ENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <set>


struct BetHorseCfgEntry : public Avalon::DataEntry
{
	BetHorseCfgEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	UInt32 id;
	// 个人最大押注
	UInt32 stakeMax;
	// 每局需要的射手数量
	UInt32 needShooterNum;
	// 刷新赔率时间间隔
	UInt32 refreshOddsInterval;
	// 押注截止时间
	UInt32 stakeEndTime;
	// 调整结束时间
	UInt32 adjustEndTime;
	// 每天开启时间
	std::set<UInt32> startTimeSet;
	// 子弹的ID
	UInt32 bulletID;
	// 押注开始提醒公告ID
	UInt32 startStakePublic;
	// 押注结束提醒公告
	UInt32 endStakePublic;
	// 比赛开始公告
	UInt32 battleStartPublic;
	// 开奖公告
	UInt32 openRewardPublic;
	// 押注结束提醒时间
	std::set<UInt32> endWarnTimeSet;
	// 神秘射手出现的概率
	UInt32 mysteryRate;
	// 射手基础胜率
	UInt32 baseWinRate;
	// 初始赔率最小值
	UInt32 initOddsMin;
	// 初始赔率最大值
	UInt32 initOddsMax;
};

typedef Avalon::DataEntryMgr<BetHorseCfgEntry> BetHorseCfgEntryMgr;

// ***********************************************************

struct BetHorseShooterEntry : public Avalon::DataEntry
{
	BetHorseShooterEntry();

	UInt32 GetKey() const { return shooterId; }

	bool Read(Avalon::DataRow& row);

	// 射手ID
	UInt32 shooterId;
	// 神秘概率
	UInt32 mysteryRate;
	// 出场概率
	UInt32 appearRate;
	// 名字
	std::string shooterName;
	// 职业
	UInt32 occupation;
};

class BetHorseShooterEntryMgr : public Avalon::DataEntryMgrBase<BetHorseShooterEntry>
	, public Avalon::Singleton<BetHorseShooterEntryMgr>
{
public:
	BetHorseShooterEntryMgr();

	// 加载数据
	virtual bool AddEntry(BetHorseShooterEntry* entry);

	// 随机产生神秘射手
	UInt32 GetRandMysteryShooter(UInt32 _appearMysteryRate);

	// 随机产生射手集合
	void RandShooterVec(UInt32 _randNum, std::vector<UInt32>& _shooterVec);

private:

	// 神秘总概率
	UInt32 mysteryTotalRate;
	// 出场总概率
	UInt32 appearTotalRate;
	// 神秘概率列表
	std::map<UInt32, UInt32> mysteryRateMap;
	// 出场概率列表
	std::map<UInt32, UInt32> appearRateMap;
	// 射手列表
	std::map<UInt32, BetHorseShooterEntry*> shooterEntryMap;
};

//**********************************************************

struct BetHorseMapEntry : public Avalon::DataEntry
{
	BetHorseMapEntry();

	UInt32 GetKey() const { return mapId; }

	bool Read(Avalon::DataRow& row);

	// 地图id
	UInt32 mapId;
	// 地图使用的阶段
	UInt32 mapPhase;
	// 下一轮比赛使用的地图
	UInt32 nextMap;
};

typedef Avalon::DataEntryMgr<BetHorseMapEntry> BetHorseMapEntryMgr;

// ******************************

struct BetHorseRandEntry : public Avalon::DataEntry
{
	BetHorseRandEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	// id
	UInt32 id;
	// 天气类型
	UInt32 weatherType;
	// 天气概率
	UInt32 weatherRate;
	// 地形类型
	UInt32 terrainType;
	// 地形概率
	UInt32 terrainRate;
	// 状态类型
	UInt32 statusType;
	// 状态概率
	UInt32 statusRate;
};

class BetHorseRandEntryMgr : public Avalon::DataEntryMgrBase<BetHorseRandEntry>
	, public Avalon::Singleton<BetHorseRandEntryMgr>
{
public:
	BetHorseRandEntryMgr();

	// 加载数据
	virtual bool AddEntry(BetHorseRandEntry* entry);

	// 随机获得天气
	UInt32 GetRandWeather();

	// 随机获得地形
	UInt32 GetRandTerrain();

	// 随机获得状态
	UInt32 GetRandStatus();

private:

	// 天气总权重
	UInt32 weatherTotalRate;
	// 地形总权重
	UInt32 terrainTotalRate;
	// 状态总权重
	UInt32 statusTotalRate;
	// 天气概率列表
	std::map<UInt32, UInt32> weatherRateMap;
	// 地形概率列表
	std::map<UInt32, UInt32> terrainRateMap;
	// 状态概率列表
	std::map<UInt32, UInt32> statusRateMap;
};

// *************************************

struct BetHorseShooterAttrEntry : public Avalon::DataEntry
{
	BetHorseShooterAttrEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	// id
	UInt32 id;
	// 射手1属性
	UInt32 shooter_1;
	// 射手2状态
	UInt32 shooter_2;
	// 射手1的取胜概率
	Int32 winRate;
};

struct BetHorseShooterAttrGroup
{
	BetHorseShooterAttrGroup()
	{
		shooterAttr_1 = 0;
		shooterAttr_2 = 0;
	}
	
	bool operator<(const BetHorseShooterAttrGroup& status) const
	{
		if (status.shooterAttr_1 < shooterAttr_1)
			return true;

		if (shooterAttr_1 < status.shooterAttr_1)
			return false;

		if (status.shooterAttr_2 < shooterAttr_2)
			return true;

		if (shooterAttr_2 < status.shooterAttr_2)
			return false;

		return false;
	}

	// 射手1状态
	UInt32 shooterAttr_1;
	// 射手2状态
	UInt32 shooterAttr_2;
};

class BetHorseShooterStatusEntryMgr : public Avalon::DataEntryMgrBase<BetHorseShooterAttrEntry>
	, public Avalon::Singleton<BetHorseShooterStatusEntryMgr>
{
public:
	BetHorseShooterStatusEntryMgr(){}

	// 加载数据
	virtual bool AddEntry(BetHorseShooterAttrEntry* entry);

	// 获取两状态下的胜率
	Int32 GetShooterStatusWinRate(UInt32 _shooterStatus_1, UInt32 _shooterStatus_2);

private:

	std::map<BetHorseShooterAttrGroup, Int32> shooterRateMap;
};

// ****************************************************

struct BetHorseRateEntry : public Avalon::DataEntry
{
	BetHorseRateEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	// id
	UInt32 id;
	// 天气类型
	UInt32 weatherType;
	// 天气下职业1
	UInt32 weatherOccupation_1;
	// 天气下职业2
	UInt32 weatherOccupation_2;
	// 天气取胜概率
	Int32 weatherWinRate;

	// 地形类型
	UInt32 terrainType;
	// 地形下职业1
	UInt32 terrainOccupation_1;
	// 地形下职业2
	UInt32 terrainOccupation_2;
	// 地形下取胜概率
	Int32 terrainWinRate;
};

struct BetHorseAmbient
{
	BetHorseAmbient()
	{
		ambientType = 0;
		occupation_1 = 0;
		occupation_2 = 0;
	}

	bool operator<(const BetHorseAmbient& rhs) const
	{
		if (rhs.ambientType < ambientType)
			return true;

		if (ambientType < rhs.ambientType)
			return false;

		if (rhs.occupation_1 < occupation_1)
			return true;

		if (occupation_1 < rhs.occupation_1)
			return false;

		if (rhs.occupation_2 < occupation_2)
			return true;

		if (occupation_2 < rhs.occupation_2)
			return false;

		return false;
	}

	// 环境类型(天气，地形)
	UInt32 ambientType;
	// 职业1
	UInt32 occupation_1;
	// 职业2
	UInt32 occupation_2;
};

class BetHorseRateEntryMgr : public Avalon::DataEntryMgrBase<BetHorseRateEntry>
	, public Avalon::Singleton<BetHorseRateEntryMgr>
{
public:
	BetHorseRateEntryMgr(){}

	// 加载数据
	virtual bool AddEntry(BetHorseRateEntry* entry);

	// 获取某天气下两职业的胜率
	Int32 GetWeatherOccpationWinRate(UInt32 _weather, UInt32 _occupation_1, UInt32 _occupation_2);

	// 获取某地形下两职业的胜率
	Int32 GetTerrainOccpationWinRate(UInt32 _terrain, UInt32 _occupation_1, UInt32 _occupation_2);

private:

	std::map<BetHorseAmbient, Int32> weatherOccupationRateMap;
	std::map<BetHorseAmbient, Int32> terrainOccupationRateMap;
};

//*************************************

class BetHorseOccuSuppressEntryMgr : public Avalon::DataEntryMgrBase<BetHorseShooterAttrEntry>
	, public Avalon::Singleton<BetHorseOccuSuppressEntryMgr>
{
public:
	BetHorseOccuSuppressEntryMgr(){}

	// 加载数据
	virtual bool AddEntry(BetHorseShooterAttrEntry* entry);

	// 获取两职业下的胜率
	Int32 GetOccuSuppressWinRate(UInt32 _occu_1, UInt32 _occu_2);

private:

	std::map<BetHorseShooterAttrGroup, Int32> occuSuppressMap;
};
#endif