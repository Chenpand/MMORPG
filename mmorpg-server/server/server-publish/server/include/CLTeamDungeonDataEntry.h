#ifndef _CL_TEAM_DUNGEON_DATAENTRY_H_
#define _CL_TEAM_DUNGEON_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum TeamDungeonEntryType
{
	// �ؿ�
	TEAM_DUNGEON_ELEM,
	// �˵�
	TEAM_DUNGEON_MENU,
	// �������
	TEAM_CITY_MONSTER
};

//�data
class TeamDungeonDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	// id
	UInt32					id;
	// ����
	TeamDungeonEntryType	type;
	// ���³�ID
	UInt32					dungeonId;
	// ƥ������
	UInt8					matchType;
	// ��С�ȼ�
	UInt16					minLevel;
	// ��������
	UInt8					minPlayerNum;
	// �������
	UInt8					maxPlayerNum;
	// �Ƿ��Զ���ɢ
	bool                    autoKick;
};

class TeamDungeonDataEntryMgr : public Avalon::DataEntryMgrBase<TeamDungeonDataEntry>
	, public Avalon::Singleton<TeamDungeonDataEntryMgr>
{
public:
	TeamDungeonDataEntryMgr();

	virtual bool AddEntry(TeamDungeonDataEntry* entry);

	TeamDungeonDataEntry* FindEntryByDungeonID(UInt32 dungeonId);

	UInt32 GetCityMonsterTeamTargetID() const { return m_CityMonsterTeamTargetId; }
private:
	UInt32			m_CityMonsterTeamTargetId;
};

#endif
