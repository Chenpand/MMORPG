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
	//队伍数组
	typedef std::vector<LostDungeonTeam*> TeamVec;
	//队伍map <地下城场景id，队伍数组>
	typedef std::map<UInt32, TeamVec> TeamMap;
	//队伍map
	typedef std::map<UInt32, LostDungeonTeam*> IdMapTeam;
	//最大队伍数量
	const static UInt32 MAX_TEAM_SIZE = 4;
public:
	LostDungeonMgr();
	~LostDungeonMgr();

public://地下城关卡相关 
	//创建队伍
	void CreateTeamsOfScene(UInt32 sceneId);
	//是否有空队伍
	bool IsSceneHaveEmptyTeam(UInt32 sceneId);

	LostDungeonTeam* FindTeamById(UInt32 teamId);

	//获取第一个空的队伍
	LostDungeonTeam* GetFirstEmptyTeam(UInt32 sceneId);

	//获取第一个有位置的队伍
	LostDungeonTeam* GetFirstHavePosTeam(UInt32 sceneId);

	//获取玩家所在队伍
	LostDungeonTeam* GetPlayerTeam(UInt32 sceneId, UInt64 playerId);

	//玩家进入关卡选择场景
	void OnPlayerEnterDungeonEntry(Player* player, DungeonEntrySceneDynamic* scene);

	void OnPlayerLeaveDungeonEntry(Player* player, DungeonEntrySceneDynamic* scene);

	//玩家切换地下城关卡挑战模式
	UInt32 OnPlayerSwitchChallengeMode(Player* player, UInt8 mode);

	//遍历队伍
	void VistTeams(UInt32 sceneId, const std::function<bool(LostDungeonTeam* team)>& ff);

	//遍历队员
	void VisitTeamMember(UInt32 sceneId, const std::function<bool(LostDungeonTeam* team, LostDungTeamMember* member)>& ff);

	//广播消息给地下城内队伍成员
	void BroadcastMsgToTeamMembersInScene(UInt32 sceneId, Avalon::Protocol& protocol, UInt64 exceptId = 0);

	//设置队伍状态
	void SetTeamState(UInt32 battleId, UInt32 teamId, UInt8 st);

	//队伍比赛结束
	void OnTeamRaceEnd(UInt32 teamId);

	//处理玩家异常离开
	void OnPlayerAbnExit(Player* player);
public:
	//PVP
	UInt32 OnPlayerPkReq(Player* attacker, Player* attackeder, UInt32	dungeonId);
private:
	//id生成器
	UInt32				m_TeamIdSerial;
	//队伍
	TeamMap				m_teams;
	IdMapTeam			m_idTeams;

};