#ifndef _CL_NPC_DATAENTRY_H_
#define _CL_NPC_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum NpcSubType
{
	NPC_SUB_TRADER = 7,	// 吃鸡商人
	NPC_SUB_MONSTER = 8,	// 吃鸡怪物
};

/**
*@brief npc表
*/
class NpcDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32	id;
	// 名字
	std::string name;
	// npc类型
	UInt32 npcType;
	// 地下城ID
	UInt32	dungeonId;
	// 可挑战次数
	UInt32	times;
	// 是否一定要组队
	bool	mustTeam;
	// 难度
	UInt32 hardType;
	// 死亡复活时间
	UInt32 reviveTime;
};

class NpcDataEntryMgr : public Avalon::DataEntryMgrBase<NpcDataEntry>
	, public Avalon::Singleton<NpcDataEntryMgr>
{
public:
	virtual bool AddEntry(NpcDataEntry* entry);

	UInt32 GetCityMonsterHardTypeByDungeonId(UInt32 dungeonId);

	// 根据npc类型获取一个npcId
	UInt32 GetRandNpcIdByType(UInt32 npcType);

private:

	// key->npcType,val->NpcId
	std::map<UInt32, std::vector<UInt32>> m_NpcType;
};

#endif
