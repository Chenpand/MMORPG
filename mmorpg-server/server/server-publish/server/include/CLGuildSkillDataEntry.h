#ifndef _CL_GUILD_SKILL_DATAENTRY_H_
#define _CL_GUILD_SKILL_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ����������
*/
struct GuildSkillDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return MakeKey(skillId, level); }

	bool Read(Avalon::DataRow& row);

	static UInt32 MakeKey(UInt16 skillId, UInt8 level) { return ((UInt32)skillId) * 100 + level; }

public:
	// ����ID
	UInt16	skillId;
	// ���ܵȼ�
	UInt8	level;
	// ��Ҫս�����ȼ�
	UInt8	needBattleLevel;
	// �������Ĺ���
	UInt32	contriCost;
	// �������Ľ��
	UInt32	goldCost;
};

typedef Avalon::DataEntryMgr<GuildSkillDataEntry> GuildSkillDataEntryMgr;

#endif
