#pragma once
#include "BSBaseRace.h"
#include <CLGameSessionProtocol.h>
#include <CLLostDungeonDefine.h>

class BSLostBattle;
class BSPlayer;

// 组队开始地下城投票
struct TeamDungeonStartVote
{
	TeamDungeonStartVote()
	{
		Clear();
	}

	void Clear()
	{
		dungeonId = 0;
		agreePlayerIds.clear();
		startTime = 0;
	}

	bool IsTimeout()
	{
		return dungeonId > 0 && startTime + 20 < (UInt32)CURRENT_TIME.Sec();
	}

	bool HasPlayer(ObjID_t id)
	{
		return std::find(agreePlayerIds.begin(), agreePlayerIds.end(), id) != agreePlayerIds.end();
	}

	// 地下城ID
	UInt32					dungeonId;
	// 所有同意了的玩家
	std::vector<ObjID_t>	agreePlayerIds;
	// 开始时间
	UInt32					startTime;
};


//迷失地牢组队打关卡比赛对象
class BSLostTeamDungeonRace : public BSBaseRace
{
public:
	BSLostTeamDungeonRace();
	BSLostTeamDungeonRace(UInt8 raceType);
	~BSLostTeamDungeonRace();

	void OnCreate(CLProtocol::DungenServerTeamStartRaceReq& teamStartRaceReq);

	inline void SetLostBattle(BSLostBattle*  battle) { m_ownerBattle = battle; }
	BSLostBattle* GetLostBattle();

	inline UInt32 GetTeamId() { return m_TeamId; }
	inline UInt64 GetMaster() { return m_Master; }

	inline UInt8 GetMemberSize() { return m_teamMembers.size(); }

	/**
	 *@brief 广播给所有成员
	 */
	void Broadcast(Avalon::Protocol& protocol);

	/**
	*@brief 开始地下城投票
	*/
	UInt32 OnStartRaceVote(UInt32 dungeonId);
	
	/**
	*@brief 玩家上传开始地下城投票
	*/
	void OnPlayerReportVote(BSPlayer* player, bool agree);

	/**
	*@brief 所有玩家都同意
	*/
	void OnStartRaceVoteAllAgree();

	/**
	*@brief 有玩家拒绝
	*/
	void OnStartRaceVoteFailure(ObjID_t roleId = 0, UInt32 noticeId = 0);

	/**
	 *@brief 广播公告给所有成员
	 */
	void Notify(UInt16 type, UInt8 color, const char* word);

	void SyncTeamState(LostDungTeamBattleSt st, UInt8 isEnd = 0);

	virtual UInt32 StartRace();
	virtual void OnStartRaceFail();
	virtual void OnStartDungeonRes(UInt32 result, UInt64 gamesessionID);
	virtual void OnDungeonEnd(UInt8 result, DungeonRaceEndInfo& endInfo);
	virtual void OnTick(const Avalon::Time& now);
	virtual BSBaseBattle* GetBattle();

private:
	std::vector<LostDungRaceMember> m_teamMembers;
	BSLostBattle*  m_ownerBattle; //所在战场
	//开始比赛投票
	TeamDungeonStartVote	m_Vote;
	UInt64	m_Master;
	UInt32  m_TeamId;
	CLProtocol::DungenServerTeamStartRaceReq m_teamStartRaceReq;
};