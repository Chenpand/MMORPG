#ifndef __CL_CHAMPION_DATA_ENTRY_H__
#define __CL_CHAMPION_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLItemDefine.h"


class ChampionTimeDataEntry : public Avalon::DataEntry
{
public:
	ChampionTimeDataEntry();
	virtual ~ChampionTimeDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);



public:
	// ID
	UInt32                                          id;
	// 状态
	UInt32                                          status;
	// 开始时间
	UInt32                                          startTime;
	// 结束时间
	UInt32                                          endTime;
	// 赛前开始时间
	UInt32                                          preStartTime;
	// 准备开始时间
	UInt32                                          prepareStartTime;
	// 战斗开始时间
	UInt32                                          battleStartTime;
	// 战斗结束时间
	UInt32                                          battleEndTime;
};

class ChampionTimeDataEntryMgr : public Avalon::DataEntryMgrBase<ChampionTimeDataEntry>
	, public Avalon::Singleton<ChampionTimeDataEntryMgr>
{
public:
	virtual bool AddEntry(ChampionTimeDataEntry* entry);


};


class ChampionRewardDataEntry : public Avalon::DataEntry
{
public:
	ChampionRewardDataEntry();
	virtual ~ChampionRewardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);


public:
	// ID
	UInt32                                          id;
	// 被发奖玩家阶段
	UInt32                                          playerState;
	// 该阶段结束发奖
	UInt32                                          championState;
	// 奖励
	std::vector<ItemReward>							itemRewards;
	// 标题
	UInt32											title;
	// 发送者
	UInt32											sender;
	// 内容
	UInt32											content;


};

class ChampionRewardDataEntryMgr : public Avalon::DataEntryMgrBase<ChampionRewardDataEntry>
	, public Avalon::Singleton<ChampionRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(ChampionRewardDataEntry* entry);
	std::vector<ChampionRewardDataEntry*> GetChampionStateReward(UInt32 state);
private:
	std::map<UInt32, std::vector<ChampionRewardDataEntry*>> m_ChampionStateRewardMap;
};




class ChampionGambleDataEntry : public Avalon::DataEntry
{
public:
	ChampionGambleDataEntry();
	virtual ~ChampionGambleDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);



public:
	// ID
	UInt32                                          id;
	// 类型
	UInt32                                          type;
	// 关联阶段
	UInt32											state;
	// 开始时间
	UInt32                                          startTime;
	// 结束时间
	UInt32                                          endTime;
	// 关联开始组id
	UInt32											groupStart;
	// 关联结束组id
	UInt32											groupEnd;
};

class ChampionGambleDataEntryMgr : public Avalon::DataEntryMgrBase<ChampionGambleDataEntry>
	, public Avalon::Singleton<ChampionGambleDataEntryMgr>
{
public:
	virtual bool AddEntry(ChampionGambleDataEntry* entry);
	ChampionGambleDataEntry* GetDataEntryByStateAndType(UInt32 state, UInt32 type);
private:
	//类型->阶段->配置
	std::map<UInt32, std::map<UInt32, ChampionGambleDataEntry*>> m_ProjectDataMap;
};


#endif
