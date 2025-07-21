#ifndef __CL_MONOPOLY_DATA_ENTRY_H__
#define __CL_MONOPOLY_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

#include "CLItemDefine.h"

class MonopolyCardDataEntry : public Avalon::DataEntry
{
public:
	MonopolyCardDataEntry();
	virtual ~MonopolyCardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);


public:
	// 卡牌id
	UInt32                                          card;
	// 兑换硬币数量
	UInt32											coinNum;


};

class MonopolyCardDataEntryMgr : public Avalon::DataEntryMgrBase<MonopolyCardDataEntry>
	, public Avalon::Singleton<MonopolyCardDataEntryMgr>
{
public:
	virtual bool AddEntry(MonopolyCardDataEntry* entry);
	//卡牌id列表
	std::vector<UInt32>								cardIDVec;

};



class MonopolyRandomDataEntry : public Avalon::DataEntry
{
public:
	MonopolyRandomDataEntry();
	virtual ~MonopolyRandomDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);


public:
	//id
	UInt32											id;
	// 格子类型
	UInt32                                          gridType;
	// 类型
	UInt32                                          type;
	// 参数一
	UInt32						                    param;
	// 数量
	UInt32                                          num;
	// 权重
	UInt32                                          weight;
	// 随机类型 0全部 1随机一个
	UInt32                                          randomType;
};


struct MonopolyGridEventDataEntry
{
	UInt32 id;
	UInt32 eventType;
	UInt32 param;
	UInt32 weight;
	UInt32 num;
};

struct MonopolyGridDataEntry
{
	UInt32 randomType;
	UInt32 totalWeight = 0;
	std::vector<MonopolyGridEventDataEntry> events;
};

class MonopolyRandomDataEntryMgr : public Avalon::DataEntryMgrBase<MonopolyRandomDataEntry>
	, public Avalon::Singleton<MonopolyRandomDataEntryMgr>
{
public:
	virtual bool AddEntry(MonopolyRandomDataEntry* entry);
	MonopolyGridDataEntry* GetGridData(UInt32 gridType);
private:
	std::map<UInt32, MonopolyGridDataEntry> m_GridMap;
};


enum MonopolyCardRewardType
{
	MCRT_NORMAL = 1,
	MCRT_FINAL = 2,
};

class MonopolyCardRewardDataEntry : public Avalon::DataEntry
{
public:
	MonopolyCardRewardDataEntry();
	virtual ~MonopolyCardRewardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);



public:
	// ID
	UInt32                                          id;
	// 消耗卡牌
	ItemRewardVec                                   costCard;
	// 奖励
	ItemRewardVec									reward;
	// 限制
	UInt32											limit;
	// 类型
	UInt32											type;

};

class MonopolyCardRewardDataEntryMgr : public Avalon::DataEntryMgrBase<MonopolyCardRewardDataEntry>
	, public Avalon::Singleton<MonopolyCardRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(MonopolyCardRewardDataEntry* entry);

};


class MonopolyMapDataEntry : public Avalon::DataEntry
{
public:
	MonopolyMapDataEntry();
	virtual ~MonopolyMapDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

	//
	UInt32 GetMapGridContent(UInt32 grid) const;

public:
	// 轮数
	UInt32                                          turn;
	// 地图内容
	std::vector<UInt32>								content;


};

class MonopolyMapDataEntryMgr : public Avalon::DataEntryMgrBase<MonopolyMapDataEntry>
	, public Avalon::Singleton<MonopolyMapDataEntryMgr>
{
public:
	virtual bool AddEntry(MonopolyMapDataEntry* entry);

	UInt32 GetMapGridContent(UInt32 turn, UInt32 grid);
	inline const UInt32 GetMaxTurn() const { return m_MaxTurn; }
private:
	UInt32 m_MaxTurn = 0;
};



#endif
