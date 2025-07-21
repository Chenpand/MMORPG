#ifndef __TEAM_MATCH_MGR_H__
#define __TEAM_MATCH_MGR_H__

#include "TeamMatchDefine.h"
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <AvalonProtocol.h>

// 地下城匹配池
struct TeamDungeonMatchPool
{
	TeamDungeonMatchPool()
	{
		matchType = TEAM_MATCH_TYPE_TEAM;
		fullPlayerNum = 3;
	}

	UInt32							targetId;
	TeamMatchType					matchType;
	UInt32							fullPlayerNum;
	std::vector<TeamMatchEntry*>	entrys;
};

// 匹配结果
struct TeamMatchResult
{
	TeamMatchResult()
	{
		target = 0;
		dungeonId = 0;
		matchType = TEAM_MATCH_TYPE_TEAM;
		doResultTime = 0;
	}

	UInt32							target;
	UInt32							dungeonId;
	TeamMatchType					matchType;
	std::vector<TeamMatchEntry*>	entrys;
	// 处理匹配结果时间(ms)
	UInt64							doResultTime;
};

class TeamMatchMgr : public Avalon::Singleton<TeamMatchMgr>
{
public:
	TeamMatchMgr();
	~TeamMatchMgr();

	/**
	*@brief 初始化
	*/
	bool OnInit();

	/**
	*@brief 心跳
	*/
	void OnTick(const Avalon::Time& now);

public:
	// 玩家事件

	/**
	*@brief 玩家请求开始匹配
	*/
	UInt32 OnPlayerJoinMatch(WSPlayer* player, UInt32 dungeonId);

	/**
	*@brief 玩家取消匹配
	*/
	UInt32 OnPlayerCancelMatch(WSPlayer* player);

	/**
	*@brief 队伍取消匹配
	*/
	UInt32 OnTeamCancelMatch(WSTeam* team);

private:
	// 匹配算法

	/**
	*@brief 处理匹配结果
	*/
	void _DoMatchResult();

	/**
	*@brief 在匹配池中匹配
	*/
	void _DoMatch();

	/**
	*@brief 尝试加入别的队伍
	*/
	void _DoJoinTeamMatch();

	/**
	*@brief 尝试加入别的队伍，返回成功或失败
	*/
	bool _TryJoinOtherTeam(UInt32 target, UInt32 fullPlayerNum, TeamMatchEntry* entry);

	/**
	*@brief 开始投票进入战斗
	*/
	void _OnStartRaceVote();

	/**
	*@brief 匹配成功
	*/
	bool _OnMatchSuccess(TeamMatchResult result);

	/**
	*@brief 将匹配池按照玩家人数分类
	*/
	std::vector<std::vector<TeamMatchEntry*>> _GenMatchEntryBySize(TeamDungeonMatchPool& pool);

private:
	// 内部函数

	/**
	*@brief 查找匹配单元
	*/
	TeamMatchEntry* _FindEntryByPlayer(WSPlayer* player);

	/**
	*@brief 查找匹配单元
	*/
	TeamMatchEntry* _FindEntryByTeam(WSTeam* team);

	/**
	*@brief 删除匹配单元
	*/
	void _DelEntry(TeamMatchEntry* entry, bool destoryObject);

	/**
	*@brief 广播投票进入比赛消息
	*/
	void _BroadcastNotifyRaceVote(TeamMatchResult& result);
	
	/**
	*@brief 是否相同
	*/
	bool _IsSame(TeamMatchEntry* entry, WSPlayer* player);

	/**
	*@brief 是否相同
	*/
	bool _IsSame(TeamMatchEntry* entry, WSTeam* team);

	/**
	*@brief 广播取消匹配
	*/
	void _BroadMatchCancel(TeamMatchEntry* entry);

	/**
	*@brief 广播消息
	*/
	void _Broadcast(TeamMatchEntry* entry, Avalon::Protocol& protocol);

private:
	// 匹配池
	HashMap<UInt32, TeamDungeonMatchPool>			m_MatchPool;
	// 匹配完成开始战斗投票
	std::vector<TeamMatchResult>					m_Results;
	// 匹配定时器
	Avalon::SimpleTimer								m_MatchTimer;
};

#endif