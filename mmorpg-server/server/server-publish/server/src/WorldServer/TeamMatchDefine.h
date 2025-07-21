#ifndef __TEAM_MATCH_DEFINE_H__
#define __TEAM_MATCH_DEFINE_H__

#include <CLDefine.h>
#include <AvalonObjectPool.h>

class WSPlayer;
class WSTeam;

// ���ƥ�䵥λ����
enum TeamMatchEntryType
{
	// ���
	TEAM_MATCH_ENTRY_PLAYER,
	// ����
	TEAM_MATCH_ENTRY_TEAM,
};

enum TeamMatchType
{
	// ����ƥ��
	TEAM_MATCH_TYPE_RACE,
	// �������
	TEAM_MATCH_TYPE_TEAM,
};

// �������ƥ��
struct TeamMatchEntry
{
	AVALON_DEFINE_OBJECTPOOL(TeamMatchEntry, Avalon::NullMutex)

	TeamMatchEntry(TeamMatchType matchType_, UInt32 dungeonId_, TeamMatchEntryType type_, WSPlayer* player_, WSTeam* team_)
	: matchType(matchType_), type(type_), dungeonId(dungeonId_)
	{
		if (type == TEAM_MATCH_ENTRY_TEAM)
		{
			player = NULL;
			team = team_;
		}
		else if (type == TEAM_MATCH_ENTRY_PLAYER)
		{
			player = player_;
			team = NULL;
		}

		startTime = CURRENT_TIME.MSec();
		dirty = false;
	}

	UInt32 GetSize();

	// �Ƿ�ʼ�������
	UInt32 IsStartJoinTeam(UInt64 curTime)
	{
		if (matchType == TEAM_MATCH_TYPE_TEAM && curTime >= startTime + 1 * Avalon::Time::MSECS_OF_SEC)
		{
			return true;
		}
		else if (matchType == TEAM_MATCH_TYPE_RACE && curTime >= startTime + 20 * Avalon::Time::MSECS_OF_SEC)
		{
			return true;
		}

		return false;
	}

	TeamMatchType		matchType;
	TeamMatchEntryType	type;
	WSPlayer*			player;
	WSTeam*				team;
	UInt32				dungeonId;
	// ��ʼʱ��(ms)
	UInt64				startTime;
	// �Ƿ��Ѿ�ƥ�䵽��
	bool				dirty;
};

#endif