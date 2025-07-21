#ifndef _CL_NPC_DATAENTRY_H_
#define _CL_NPC_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum NpcSubType
{
	NPC_SUB_TRADER = 7,	// �Լ�����
	NPC_SUB_MONSTER = 8,	// �Լ�����
};

/**
*@brief npc��
*/
class NpcDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32	id;
	// ����
	std::string name;
	// npc����
	UInt32 npcType;
	// ���³�ID
	UInt32	dungeonId;
	// ����ս����
	UInt32	times;
	// �Ƿ�һ��Ҫ���
	bool	mustTeam;
	// �Ѷ�
	UInt32 hardType;
	// ��������ʱ��
	UInt32 reviveTime;
};

class NpcDataEntryMgr : public Avalon::DataEntryMgrBase<NpcDataEntry>
	, public Avalon::Singleton<NpcDataEntryMgr>
{
public:
	virtual bool AddEntry(NpcDataEntry* entry);

	UInt32 GetCityMonsterHardTypeByDungeonId(UInt32 dungeonId);

	// ����npc���ͻ�ȡһ��npcId
	UInt32 GetRandNpcIdByType(UInt32 npcType);

private:

	// key->npcType,val->NpcId
	std::map<UInt32, std::vector<UInt32>> m_NpcType;
};

#endif
