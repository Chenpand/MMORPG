#ifndef __CL_REPLAY_DEFINE_H__
#define __CL_REPLAY_DEFINE_H__

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLMatchDefine.h>
#include <CLRelayServerProtocol.h>
#include <memory>

enum ReplayListType
{
	REPLAY_LIST_TYPE_INVALID,
	// 自己对战记录
	REPLAY_LIST_TYPE_SELF,
	// 高手对决
	REPLAY_LIST_TYPE_MASTER,
	// 收藏
	REPLAY_LIST_TYPE_COLLECTION,
	// 高手对决等待列表(等时间到，就会加到高手对决中)
	REPLAY_LIST_TYPE_MASTER_WAIT,
	// 赏金联赛淘汰赛
	REPLAY_LIST_TYPE_PREMIUM_LEAGUE_BATTLE,
	// 数量
	REPLAY_LIST_NUM,
};

struct ReplayFighterInfo
{
	ReplayFighterInfo()
	{
		guid = 0;
		roleId = 0;
		occu = 0;
		level = 0;
		pos = 0;
		seasonLevel = 0;
		seasonStars = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleId & name & occu & level & pos & seasonLevel & seasonStars;
	}

	ObjID_t		guid;
	ObjID_t     roleId;
	std::string	name;
	UInt8       occu;
	UInt16		level;
	UInt8		pos;
	// 赛季段位
	UInt32		seasonLevel;
	// 赛季星星
	UInt8		seasonStars;
};

// 录像评价
enum ReplayEvaluation
{
	REPLAY_EVA_INVALID,
};

struct ReplayInfo
{
	AVALON_DEFINE_OBJECTPOOL(ReplayInfo, Avalon::NullMutex)
	ReplayInfo()
	{
		guid = 0;
		version = 0;
		raceId = 0;
		type = 0;
		evaluation = 0;
		recordTime = 0;
		result = 0;
		score = 0;
		viewNum = 0;
		dbRefNum = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & version & raceId & type & evaluation & recordTime & result & fighters & viewNum & score;
	}

	UInt64				guid;
	// 版本号
	UInt32				version;
	// 比赛ID
	ObjID_t				raceId;
	// 比赛类型(对应枚举PkType)
	UInt8				type;
	// 评价(对应枚举ReplayEvaluation)
	UInt8				evaluation;
	// 记录时间
	UInt32				recordTime;
	// 战斗结果（第一个玩家的结果，对应枚举PkRaceResult）
	UInt8				result;
	// 战斗双方信息
	std::vector<ReplayFighterInfo>	fighters;
	// 观看次数
	UInt32				viewNum;
	// 录像积分
	UInt32				score;
	// 被引用次数(这是数据库中的被引用次数)
	UInt32				dbRefNum;
};
typedef std::shared_ptr<ReplayInfo> ReplayInfoPtr;

// 录像文件头
struct ReplayHeader
{
	ReplayHeader()
	{
		battleFlag = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & magicCode & version & startTime & sessionId & raceType & pkDungeonId & duration & players & battleFlag;
	}

	UInt32							magicCode;
	// 版本号
	UInt32							version;
	// 时间
	UInt32							startTime;
	// 比赛ID
	ObjID_t							sessionId;
	// 比赛类型，对应枚举(RaceType)
	UInt8							raceType;
	// pk场景
	UInt32							pkDungeonId;
	// 持续时间
	UInt32							duration;
	// 玩家信息
	std::vector<RacePlayerInfo>		players;
	// 战斗标记
	UInt64							battleFlag;
};

// 录像中的战斗结果
struct ReplayRaceResult
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & pos & result;
	}

	// 座位号
	UInt8		pos;
	// 战斗结果
	UInt8		result;
};

// 录像文件
struct ReplayFile
{
	ReplayFile() : addedToDB(false) {}

	void Input(Avalon::NetInputStream& stream) 
	{
		stream & header;

		UInt32 num = 0;
		stream & num;

		CLProtocol::Frame frame;
		for (UInt32 i = 0; i < num; i++)
		{
			stream & frame;
			frames.push_back(frame);
		}

		stream & results;
	}

	void Output(Avalon::NetOutputStream& stream) 
	{
		stream & header;
		stream & (UInt32)frames.size();
		stream.SeriaArray((CLProtocol::Frame*)&frames[0], frames.size());
		stream & results;
	}

	// 是否已经插入到数据库
	bool									addedToDB;
	// 文件头
	ReplayHeader							header;
	// 有操作的帧
	std::vector<CLProtocol::Frame>			frames;
	// 战斗结果
	std::vector<ReplayRaceResult>			results;
};

#endif