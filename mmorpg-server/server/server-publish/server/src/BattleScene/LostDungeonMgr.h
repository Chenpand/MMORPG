#pragma once

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <functional>
#include "LostDungeonMgr.h"

class Player;
class DungeonEntryScene;
class LostDungeonTeam;
struct LostDungTeamMember;
class DungeonEntrySceneDynamic;

namespace Avalon
{
	class Protocol;
};

class LostDungeonMgr : public Avalon::Singleton<LostDungeonMgr>
{
	//��������
	typedef std::vector<LostDungeonTeam*> TeamVec;
	//����map <���³ǳ���id����������>
	typedef std::map<UInt32, TeamVec> TeamMap;
	//����map
	typedef std::map<UInt32, LostDungeonTeam*> IdMapTeam;
	//����������
	const static UInt32 MAX_TEAM_SIZE = 4;
public:
	LostDungeonMgr();
	~LostDungeonMgr();

public://���³ǹؿ���� 
	//��������
	void CreateTeamsOfScene(UInt32 sceneId);
	//�Ƿ��пն���
	bool IsSceneHaveEmptyTeam(UInt32 sceneId);

	LostDungeonTeam* FindTeamById(UInt32 teamId);

	//��ȡ��һ���յĶ���
	LostDungeonTeam* GetFirstEmptyTeam(UInt32 sceneId);

	//��ȡ��һ����λ�õĶ���
	LostDungeonTeam* GetFirstHavePosTeam(UInt32 sceneId);

	//��ȡ������ڶ���
	LostDungeonTeam* GetPlayerTeam(UInt32 sceneId, UInt64 playerId);

	//��ҽ���ؿ�ѡ�񳡾�
	void OnPlayerEnterDungeonEntry(Player* player, DungeonEntrySceneDynamic* scene);

	void OnPlayerLeaveDungeonEntry(Player* player, DungeonEntrySceneDynamic* scene);

	//����л����³ǹؿ���սģʽ
	UInt32 OnPlayerSwitchChallengeMode(Player* player, UInt8 mode);

	//��������
	void VistTeams(UInt32 sceneId, const std::function<bool(LostDungeonTeam* team)>& ff);

	//������Ա
	void VisitTeamMember(UInt32 sceneId, const std::function<bool(LostDungeonTeam* team, LostDungTeamMember* member)>& ff);

	//�㲥��Ϣ�����³��ڶ����Ա
	void BroadcastMsgToTeamMembersInScene(UInt32 sceneId, Avalon::Protocol& protocol, UInt64 exceptId = 0);

	//���ö���״̬
	void SetTeamState(UInt32 battleId, UInt32 teamId, UInt8 st);

	//�����������
	void OnTeamRaceEnd(UInt32 teamId);

	//��������쳣�뿪
	void OnPlayerAbnExit(Player* player);
public:
	//PVP
	UInt32 OnPlayerPkReq(Player* attacker, Player* attackeder, UInt32	dungeonId);
private:
	//id������
	UInt32				m_TeamIdSerial;
	//����
	TeamMap				m_teams;
	IdMapTeam			m_idTeams;

};