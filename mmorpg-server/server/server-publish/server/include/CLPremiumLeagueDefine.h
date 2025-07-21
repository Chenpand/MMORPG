#ifndef __CL_PREMIUM_LEAGUE_DEFINE_H__
#define __CL_PREMIUM_LEAGUE_DEFINE_H__

#include <AvalonNetStream.h>
#include <CLDefine.h>
#include <AvalonSocket.h>

enum PremiumLeagueStatus
{
	// 初始状态
	PLS_INIT,
	// 报名
	PLS_ENROLL,
	// 预赛
	PLS_PRELIMINAY,
	// 八强准备
	PLS_FINAL_EIGHT_PREPARE,
	// 八强赛
	PLS_FINAL_EIGHT,
	// 四强赛
	PLS_FINAL_FOUR,
	// 决赛
	PLS_FINAL,
	// 决赛结束等待清理状态
	PLS_FINAL_WAIT_CLEAR,
	// 数量
	PLS_NUM
};

struct PremiumLeagueStatusInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & status & startTime & endTime;
	}

	UInt8		status;
	UInt32		startTime;
	UInt32		endTime;
};

// 淘汰赛玩家信息
struct PremiumLeagueBattleGamer
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleId & name & occu & ranking & winNum & isLose;
	}

	ObjID_t			roleId;
	std::string		name;
	UInt8			occu;
	UInt32			ranking;
	UInt32			winNum;
	UInt8			isLose;
};

// 自己的赏金联赛信息
struct PremiumLeagueSelfInfo
{
	PremiumLeagueSelfInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & winNum & score & ranking & enrollCount & loseNum & preliminayRewardNum;
	}

	UInt32		winNum;
	UInt32		score;
	UInt32		ranking;
	UInt32		enrollCount;
	UInt32		loseNum;
	UInt32		preliminayRewardNum;
};

enum PremiumLeagueRewardType
{
	// 第1名的奖励
	PL_REWARD_NO_1,
	// 第2名的奖励
	PL_REWARD_NO_2,
	// 3-4名的奖励
	PL_REWARD_NO_3_4,
	// 5-8名的奖励
	PL_REWARD_NO_5_8,
	// 9-20名的奖励
	PL_REWARD_NO_9_20,

	PL_REWARD_NUM
};

class BattleRecordEntry : public Avalon::INetSerializable
{
public:
	BattleRecordEntry() {}
	virtual ~BattleRecordEntry() {}

public:
	UInt32 GetIndex() const { return m_Index; }
	void SetIndex(UInt32 index) { m_Index = index; }

	UInt32 GetTime() const { return m_Time; }
	void SetTime(UInt32 time) { m_Time = time; }

public:
	// 需要继承的接口

	/**
	*@brief 判断这条记录是不是属于这个玩家
	*/
	virtual bool IsOwner(ObjID_t roleId) = 0;

protected:
	UInt32	m_Index;
	UInt32	m_Time;
};

struct PremiumLeagueRecordFighter
{
	PremiumLeagueRecordFighter() :id(0), winStreak(0), gotScore(0), totalScore(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & winStreak & gotScore & totalScore;
	}

	// ID
	ObjID_t			id;
	// 名字
	std::string		name;
	// 输:战斗前的连胜数 赢:战斗后连胜
	UInt8			winStreak;
	// 获得积分
	UInt16			gotScore;
	//总积分
	UInt16			totalScore;
};

class PremiumLeagueRecordEntry : public BattleRecordEntry
{
public:
	PremiumLeagueRecordFighter& GetWinner() { return m_Winner; }
	void SetWinner(PremiumLeagueRecordFighter& fighter) { m_Winner = fighter; }

	PremiumLeagueRecordFighter& GetLoser() { return m_Loser; }
	void SetLoser(PremiumLeagueRecordFighter& fighter) { m_Loser = fighter; }

	/**
	*@brief 判断这条记录是不是属于这个玩家
	*/
	virtual bool IsOwner(ObjID_t roleId)
	{
		if (m_Winner.id == roleId || m_Loser.id == roleId)
		{
			return true;
		}
		return  false;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Index & m_Time & m_Winner & m_Loser;
	}

protected:
	// 胜利者
	PremiumLeagueRecordFighter	m_Winner;
	// 失败者
	PremiumLeagueRecordFighter	m_Loser;

};

struct PremiumLeagueBattleFighter
{
	PremiumLeagueBattleFighter()
	{
		id = 0;
		occu = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & occu;
	}

	ObjID_t		id;
	std::string name;
	UInt8		occu;
};

struct CLPremiumLeagueBattle
{
	CLPremiumLeagueBattle()
	{
		guid = 0;
		raceId = 0;
		type = 0;
		isEnd = 0;
		winnerId = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & raceId & type & fighter1 & fighter2 & isEnd & winnerId & relayAddr;
	}

	ObjID_t							guid;
	ObjID_t							raceId;
	UInt8							type;
	PremiumLeagueBattleFighter		fighter1;
	PremiumLeagueBattleFighter		fighter2;
	UInt8							isEnd;
	ObjID_t							winnerId;
	Avalon::SockAddr				relayAddr;
};

#endif