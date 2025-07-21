#ifndef __CL_CHAMPION_DEFINE_H__
#define __CL_CHAMPION_DEFINE_H__

#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLSyncObject.h>
#include <CLSceneObject.h>

const UInt32 CHAMPION_ENROLL_GLOD = 3000;


const UInt32 CHAMPION_GAMBLE_COIN_ITEM_ID = 330000271;


const UInt32 CHAMPION_GAMBLE_OPTION_LIMIT = 1000;

const UInt32 CHAMPION_BATTLE_PACKET_LIMIT_ROLE = 6000;


enum ChampionStatus
{
	// 无状态
	CS_NULL = 0,
	// 报名
	CS_ENROLL = 1,

	CS_PREPARE_BEGIN = 10,
	// 海选准备
	CS_SEA_SELECT_PREPARE = 11,
	// 复赛准备
	CS_RE_SEA_SELECT_PREPARE = 12,
	// 64强准备
	CS_64_SELECT_PREPARE = 13,
	// 16强准备
	CS_16_SELECT_PREPARE = 14,
	// 8强准备
	CS_8_SELECT_PREPARE = 15,
	// 4强准备
	CS_4_SELECT_PREPARE = 16,
	// 半决赛准备
	CS_2_SELECT_PREPARE = 17,
	// 决赛准备
	CS_1_SELECT_PREPARE = 18,
	CS_PREPARE_END,

	CS_BATTLE_BEGIN = 50,
	// 海选战斗
	CS_SEA_SELECT_BATTLE = 51,
	// 复赛战斗
	CS_RE_SEA_SELECT_BATTLE = 52,
	// 64强战斗
	CS_64_SELECT_BATTLE = 53,
	// 16强战斗
	CS_16_SELECT_BATTLE = 54,
	// 8强战斗
	CS_8_SELECT_BATTLE = 55,
	// 4强战斗
	CS_4_SELECT_BATTLE = 56,
	// 半决赛战斗
	CS_2_SELECT_BATTLE = 57,
	// 决赛战斗
	CS_1_SELECT_BATTLE = 58,
	CS_BATTLE_END,

	//下面阶段是给客户端显示用的
	CS_PRE_BEGIN = 80,
	// 海选开启前
	CS_SEA_SELECT_PRE = 81,
	// 复赛开启前
	CS_RE_SEA_SELECT_PRE = 82,
	// 64强开启前
	CS_64_SELECT_PRE = 83,
	// 16强开启前
	CS_16_SELECT_PRE = 84,
	// 8强开启前
	CS_8_SELECT_PRE = 85,
	// 4强开启前
	CS_4_SELECT_PRE = 86,
	// 半决赛开启前
	CS_2_SELECT_PRE = 87,
	// 决赛开启前
	CS_1_SELECT_PRE = 88,
	CS_PRE_END,

	// 比赛结束展示状态
	CS_END_SHOW = 95,

	// 数量
	CS_NUM
};


enum ChampionRaceAttribute
{
	CRA_GROUP_ID,
	CRA_RACE_ID,
	CRA_STATUS,
	CRA_ROLE_ID_1,
	CRA_ROLE_ID_2,
	CRA_IS_END,
	CRA_WINNER_ID,
	CRA_RESULT,
};


enum ChampionRelive
{
	CR_NO = 0,
	CR_CAN = 1,
	CR_ALREADY = 2,
};


enum ChampionAct
{
	CA_ENROLL = 0,
	CA_RELIVE = 1,
	CA_JOIN_RACE = 2,
	CA_RACE_END = 3,
	CA_OBSERVE = 4,
	CA_QUERY_SELF_ATTRIBUTES = 5,
	CA_CHECK_RELIVE = 6,
};

enum ChampionGroupBattleStatus
{
	CGBS_NOT_START = 0,
	CGBS_STARTED = 1,
	CGBS_END = 2,
};

struct ChampionStatusInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & status & startTime & endTime & preStartTime & prepareStartTime & battleStartTime & battleEndTime;
	}

	bool operator!=(const ChampionStatusInfo& rsh) const
	{
		if (status == rsh.status && startTime == rsh.startTime && endTime == rsh.endTime)
		{
			return false;
		}
		return true;
	}

	UInt32		status;
	UInt32		startTime;
	UInt32		endTime;
	UInt32		preStartTime;
	UInt32		prepareStartTime;
	UInt32		battleStartTime;
	UInt32		battleEndTime;
};


struct ChampionTop16Player
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleID & pos & name & occu  & avatar & server & zoneID;
	}

	UInt64			roleID;
	UInt32			pos;
	std::string		name;
	UInt8			occu;
	PlayerAvatar	avatar;
	std::string		server;
	UInt32			zoneID;
};

struct ChampionGambleOption
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & option & num & odds;
	}

	UInt64 option;
	UInt64 num;
	UInt32 odds;
};

struct ChampionGambleInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & type & optionVec  & startTime & endTime & param;
	}

	UInt32 id;
	UInt8 type;
	std::vector<ChampionGambleOption> optionVec;
	UInt32 startTime;
	UInt32 endTime;
	UInt64 param;
};

struct ChampionGambleRecord
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & option & result  & reward & time & num;
	}

	UInt32 id;
	UInt64 option;
	UInt64 result;
	UInt64 reward;
	UInt32 time;
	UInt64 num;
};


struct RankRole
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleID & name & score & server;
	}
	UInt64 roleID;
	std::string name;
	UInt32 score = 0;
	std::string server;
};

//比赛记录
struct ChampionBattleRecord 
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & order & raceID & winner & isEnd;
	}
	//第几场比赛
	UInt32 order;
	//比赛ID
	UInt64 raceID;
	//胜者id
	UInt64 winner;
	//是否结束 0是未结束 1是已结束
	UInt8 isEnd;
};

//淘汰赛战斗对象
struct ChampionKnockoutPlayer {
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleID & name & occu & server & score;
	}
	UInt64 roleID;
	std::string name;
	UInt8 occu;
	std::string server;
	UInt32 score;
};

//积分赛战斗记录
struct ChampionScoreBattleRecord {
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleIDA & nameA  & roleIDB & nameB & result;
	}
	UInt64 roleIDA;
	UInt64 roleIDB;
	std::string nameA;
	std::string nameB;
	UInt8 result;
};

struct ChampionGroupStatus {
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & groupID & roleA & roleB & scoreA & scoreB & status;
	}
	bool IsChanged(const ChampionGroupStatus& other)
	{
		if (roleA == other.roleA)
		{
			if (scoreA != other.scoreA || scoreB != other.scoreB || status != other.status)
			{
				return true;
			}
		}
		else if (roleA == other.roleB)
		{
			if (scoreA != other.scoreB || scoreB != other.scoreA || status != other.status)
			{
				return true;
			}
		}
		return false;
	}
	UInt32 groupID;
	UInt64 roleA;
	UInt64 roleB;
	UInt32 scoreA;
	UInt32 scoreB;
	//0 未开始 1已开始 2已结束
	UInt8 status;
};

struct ChampionRaceData : public CLSyncObject
{
	ObjUInt32 order;
	ObjUInt32 groupID;
	ObjUInt64 raceID;
	ObjUInt8 championStatus;
	ObjUInt64 roleID1;
	ObjUInt64 roleID2;
	ObjUInt8 isEnd;
	ObjUInt64 winnerID;
	ObjUInt32 result;
	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("order", order)
		CL_DBPROPERTY("group_id", groupID)
		CL_DBPROPERTY("race_id", raceID)
		CL_DBPROPERTY("status", championStatus)
		CL_DBPROPERTY("roleid_1", roleID1)
		CL_DBPROPERTY("roleid_2", roleID2)
		CL_DBPROPERTY("is_end", isEnd)
		CL_DBPROPERTY("winner_id", winnerID)
		CL_DBPROPERTY("result", result)
	CL_DBPROPERTY_END()

	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
		CL_SYNC_PROPERTY(championStatus, CRA_STATUS, SST_TEST)
		CL_SYNC_PROPERTY(roleID1, CRA_ROLE_ID_1, SST_TEST)
		CL_SYNC_PROPERTY(roleID2, CRA_ROLE_ID_2, SST_TEST)
		CL_SYNC_PROPERTY(winnerID, CRA_WINNER_ID, SST_TEST)
		CL_SYNC_PROPERTY(isEnd, CRA_IS_END, SST_TEST)
		CL_SYNC_PROPERTY(result, CRA_RESULT, SST_TEST)
	CL_SYNC_PROPERTY_END()
};

#endif