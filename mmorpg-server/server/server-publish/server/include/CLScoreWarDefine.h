#ifndef _CL_SCORE_WAR_DEFINE_H_
#define _CL_SCORE_WAR_DEFINE_H_

#include <CLDefine.h>
#include <AvalonNetStream.h>
#include <CLGameDefine.h>
#include <CLSortListDefine.h>

#define SCORE_WAR_REWARD_SIZE 64

//积分争霸赛状态
enum ScoreWarStatus
{
	//未开始
	SWS_INVALID = 0,
	//准备状态
	SWS_PREPARE = 1,
	//战斗状态
	SWS_BATTLE = 2,
	//等待结束
	SWS_WAIT_END = 3,
	//最大
	SWS_MAX,
};

struct ScoreWarTimeInfo
{
	ScoreWarTimeInfo(): preTime(0), battleTime(0), waitEndTime(0), endTime(0) {}

	UInt32 preTime;
	UInt32 battleTime;
	UInt32 waitEndTime;
	UInt32 endTime;
};

struct ScoreWarSortListEntry
{
	ScoreWarSortListEntry() : sortType(0), playerId(0), score(0), connId(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & sortType & serverName & playerId & playerName & score;
	}

	UInt8			sortType;
	std::string		serverName;
	UInt64			playerId;
	std::string		playerName;
	UInt32			score;
	UInt32			connId;


	void SerializeForClient(Avalon::NetOutputStream& stream)
	{
		stream & playerId & playerName & serverName & score;
	}

	UInt64 GetSortFactor() const
	{
		return GetSortFactor(sortType, playerId, score);
	}

	static UInt64 GetSortFactor(UInt8 sortType, UInt64 id, UInt32 score)
	{
		return score;
	}
};

#endif //_CL_SCORE_WAR_DEFINE_H_
