#ifndef _WS_TEAM_MGR_H_
#define _WS_TEAM_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <AvalonDailyTimer.h>
#include <map>
#include <set>
#include "WSTeam.h"

/**
 *@brief 队伍管理器
 */
class WSTeamMgr : public Avalon::Singleton<WSTeamMgr>
{
	typedef std::map<UInt32, WSTeam*>	TeamMap;

    // 一次查询最多队伍数量
    static const UInt32 QUERY_MAX_TEAM_NUM_ONCE = 10;
	static const UInt32 MAX_TEAM_NUMBER = 9999;

public:
	WSTeamMgr();
	~WSTeamMgr();

public:
	/**
	 *@brief 添加一个队伍
	 */
	bool AddTeam(WSTeam* team);
	void RemoveTeam(WSTeam* team);
	WSTeam* FindTeam(UInt32 id);

	/**
	 *@brief 遍历所有队伍
	 */
	void VisitTeams(WSTeamVisitor& visitor);

    /**
    *@brief 查询队伍列表
    */
    void OnQueryTeamList(WSPlayer* player, const CLProtocol::WorldQueryTeamList& req);

	/**
	*@brief 查询一个指定目标和人数的队伍
	*/
	WSTeam* OnQueryTeamByTargetAndNum(WSTeam* srcTeam, UInt32 target, UInt32 num);

	/**
	*@brief 合并两个队伍
	*/
	WSTeam* CombineTeam(std::vector<WSTeam*> teams);

public:// 事件
	/**
	 *@brief 玩家上线
	 */
	void OnPlayerOnline(WSPlayer* player);

	/**
	 *@brief tick事件
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 过了一天
	 */
	void OnDayChanged();

protected:
    /**
    *@brief 是否匹配查询条件
    */
	bool IsMatchQueryCond(WSPlayer* player, WSTeam* team, const CLProtocol::WorldQueryTeamList& req);

	/**
	*@brief 公会副本组队的匹配
	*/
	bool GuildDungeonCond(WSPlayer* player, WSTeam* team);

	/**
	*@brief 获取队伍编号
	*/
	UInt16 GenTeamNumber();

private:
	//id生成器
	UInt32				m_TeamIdSerial;
	//队伍列表
	TeamMap				m_Teams;
	//队伍编号(这个编号可重复使用)
	std::set<UInt16>	m_TeamNumbers;
	// 每日定时器
	Avalon::DailyTimer	m_DailyTimer;
};

#endif
