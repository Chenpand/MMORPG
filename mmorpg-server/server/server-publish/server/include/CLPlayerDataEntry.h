#ifndef _CL_PLAYER_DATAENTRY_H_
#define _CL_PLAYER_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLTaskDefine.h"

#define CL_MAKE_PLAYERDATA_KEY(Level,Occu) ((UInt32(Level) << 16) + Occu)

/**
 *@brief 玩家等级数据项
 */
struct PlayerDataEntry : public Avalon::DataEntry
{
public:
	PlayerDataEntry();
	~PlayerDataEntry();

public:
	UInt32 GetKey() const
	{
		return level;
	}

	bool Read(Avalon::DataRow& row);

public:
	//等级
	UInt16	level;
	//升级经验
	UInt32	upgradeexp;
	//升级奖励的SP
	UInt32 sp;

	//日常随机池
	std::vector<DailyTaskSelector> dailyTaskPool;

	//技能栏位数
	UInt32 skillGridSize;
	//物品栏位数
	UInt32 itemGridSize;

	//随从槽位数量
	UInt32 retinueSize;


	//生命
	UInt32	maxhp;
	//攻击
	UInt32	attack;
	//防御
	UInt32	defense;
	//命中
	UInt8	hit;
	//闪避
	UInt8	dodge;
	//暴击
	UInt32	thump;
	//抵挡
	UInt32	holdoff;
	//暴击效果
	UInt32	thumpeffect;
	//混沌攻击
	UInt32	chaosAttack;
	//混沌防御
	UInt32	chaosDefense;
	//眩晕
	UInt32	swim;
	//减速
	UInt32	speeddown;
	//定身
	UInt32	immobile;
	//沉默
	UInt32	silent;
	//眩晕抗性
	UInt32	swimDef;
	//减速抗性
	UInt32	speeddownDef;
	//定身抗性
	UInt32	immobileDef;
	//沉默抗性
	UInt32	silentDef;
	//复活银贝消耗
	UInt32	reviveCost;
	//冒险队经验加成百分比
	UInt32 adventureTeamAdditionRate;
	//评分
	UInt32 score;
	//信用点券月获得上限
	UInt32 creditPointMonthGetLimit;
	//信用点券拥有上限
	UInt32 creditPointHoldLimit;
	//信用点券转化上限
	UInt32 creditPointTransferLimit;
};

//typedef Avalon::DataEntryMgr<PlayerDataEntry> PlayerDataEntryMgr;
class PlayerDataEntryMgr : public Avalon::DataEntryMgrBase<PlayerDataEntry>, public Avalon::Singleton<PlayerDataEntryMgr>
{
public:
	typedef std::map<UInt16, UInt64>	SumExpMap;
	typedef std::map<UInt16, UInt32>	SumSPMap;

	PlayerDataEntryMgr();
	~PlayerDataEntryMgr();

public:
	bool AddEntry(PlayerDataEntry* entry);

	UInt64 GetSumExp(UInt16 level);

	UInt32 GetSumSP(UInt16 level);

	UInt32 GetMaxSkillGridSize();

	UInt16 GetMaxLevel() { return m_MaxLevel; }

	UInt32 GetCreditPointMonthGetLimit(UInt32 level);

	UInt32 GetCreditPointHoldLimit(UInt32 level);

	UInt32 GetCreditPointTransferLimit(UInt32 level);

private:
	SumExpMap			m_SumExpMap;

	SumSPMap			m_SumSPMap;

	UInt32				m_MaxSkillGridSize;

	UInt16				m_MaxLevel;

};

#endif
