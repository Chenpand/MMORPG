#ifndef _CL_LOST_DUNGEON_DEFINE_H_
#define _CL_LOST_DUNGEON_DEFINE_H_
#include <CLDefine.h>
#include <AvalonNetStream.h>
#include <CLLostDungeonDataEntry.h>

//迷失地牢状态
enum  LostDungeonState
{
	LDS_CLOSE = 0,   //关闭中
	LDS_PROGRESSING = 1,  //进行中
	LDS_END_CAN_REWARD = 2, //结束可领取奖励
};

//迷失地牢玩家状态(battleServer)
enum LostDungeonPlayerState
{
	LDBPS_NONE,
	LDBPS_ENTERING, //进入战场中
	LDBPS_NORAML,	//正常状态
	LDBPS_MATCHING,	//匹配比赛中
	LDBPS_RACING,	//正在比赛
	LDBPS_LEAVE,	//离开战场
};

//迷失地牢玩家状态(battleScene)
enum LostDungeonPlayerBattleSt
{
	LDPBT_NORMAL = 0, //非战斗
	LDPBT_BATTLE_PVE = 1, //PVE战斗中
	LDPBT_BATTLE_PVP = 2, //PVP战斗中
};

//迷失地牢战场难度类型
enum LostDungeonBattleHardType
{
	LDBHT_NORMAL = 1, //普通的
	LDBHT_HARD	= 2,  //困难的
};

//迷失地牢楼层节点类型
enum LostDungeonElementType
{
	LDET_NONE = 0,			//无效
	LDET_NORMAL_BATTLE = 1,	//普通战场
	LDET_HARD_BATTLE = 2,	//困难战场
	LDET_TREAS_CHEST = 3,	//宝箱
	LDET_NPC = 4,			//npc
};

//迷失地牢战斗比赛类型
enum LostDungeonRaceType
{
	LDRT_NONE = 0,		//无效
	LDRT_PVP = 1,		//pvp
	LDRT_MONSTER = 2,	//击杀小怪
	LDRT_DUNGEON = 3,	//地下城关卡
};

enum LostDungeonFloorState
{
	LDFS_NONE  = 0, //
	LDFS_LOCK = 1, //封锁
	LDFS_UNLOCK_UNPASS = 2, //解锁未通关
	LDFS_UNLOCK_PASS = 3, //已通关
};

enum LostDungeonNodeState
{
	LDNS_NONE = 0, 
	LDNS_LOCK = 1, //封锁
	LDNS_CLOSE = 2, //关闭
	LDNS_OPEN = 3, //打开
	LDNS_HALF_OPEN = 4, //半开
};

enum LostDungeonBoxState
{
	LDBXS_NONE = 0,
	LDBXS_UNOPENED = 1, //未开过
	LDBXS_OPENED = 2,	//打开过
};

enum LostDungeonCloseBattleReason
{
	LDCBR_NONE = 0,
	LDCBR_ACT_CLOSE = 1,  //活动结束
	LDCBR_BAT_DYNC_ADMIN = 2,  //战场动态管理
};

static UInt8 GetFloorElementId(UInt8 elemetType)
{
	return LostDungeonElementDataEntryMgr::Instance()->GetElementId(elemetType);
}

//楼层节点
struct LostDungeonNode
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & id & treasChestId & state;
	}
	LostDungeonNode() : id(0), type(0), treasChestId(0), state(0)
	{}
	LostDungeonNode(UInt8 type_) : type(type_)
	{
		id = GetFloorElementId(type);
		treasChestId = 0;
		state = LDNS_LOCK;
	}
	UInt32  id;
	UInt8   type;
	//宝箱id(宝箱的话)
	UInt32	treasChestId;
	//状态（战场门或宝箱状态）
	UInt8   state;
};

//迷失地牢楼层数据
struct LostDungeonFloorData
{
	LostDungeonFloorData()
		:floor(0), state(0)
	{
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & floor & nodes & state;
	}

	std::string GetStrForDB()
	{
		std::stringstream ss;
		ss << floor << ";" << (UInt32)state;
		for (auto node : nodes)
		{
			ss << ";" << node.id << "," << (UInt32)node.type << "," << node.treasChestId << "," << (UInt32)node.state;
		}
		return ss.str();
	}

	bool LoadFromDB(const std::string& str)
	{
		if (str == "")
		{
			return false;
		}
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(str, ss, ";");
		if (ss.size() >= 3)
		{
			floor = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0]);
			state = Avalon::StringUtil::ConvertToValue<UInt8>(ss[1]);
			for (UInt32 i = 2; i < ss.size(); ++i)
			{
				std::string nodeStr = ss[i];
				std::vector<std::string> sss;
				Avalon::StringUtil::Split(nodeStr, sss, ",");
				if (sss.size() == 4)
				{
					LostDungeonNode node;
					node.id = Avalon::StringUtil::ConvertToValue<UInt32>(sss[0]);
					node.type = Avalon::StringUtil::ConvertToValue<UInt8>(sss[1]);
					node.treasChestId = Avalon::StringUtil::ConvertToValue<UInt32>(sss[2]);
					node.state = Avalon::StringUtil::ConvertToValue<UInt8>(sss[3]);
					nodes.push_back(node);
				}
				else
				{
					ErrorLogStream << "LostDungeonFloorData err  sss size = " << (UInt32)ss.size()
						<<", floor : " << floor << LogStream::eos;
					return false;
				}
			}
		}
		else
		{
			ErrorLogStream << "LostDungeonFloorData err  ss size = " << (UInt32)ss.size() 
				<< ", floor : " << floor << LogStream::eos;
			return false;
		}

		return true;
	}

	//楼层
	UInt32  floor;
	//生成的节点
	std::vector<LostDungeonNode>	nodes;
	//楼层状态
	UInt8	state;
};

//地下城调整模式
enum LostDungChangeMode
{
	LDCM_SINGLE = 1, //单人
	LDCM_TEAM = 2, //组队
};

struct LostDungTeamMember
{
	LostDungTeamMember()
	{
		roleId = 0;
		teamId = 0;
		pos = 0;
		name = "";
		level = 0;
		occu = 0;
		timeStamp = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & roleId & teamId & pos & name & level & occu;
	}
	ObjID_t		roleId;
	UInt32		teamId;
	UInt8		pos;
	std::string name;
	UInt16		level;
	UInt8		occu;
	UInt32		timeStamp; //加入队伍时间戳
};

enum LostDungTeamBattleSt
{
	LDTBS_NORMAL = 0, //非战斗状态
	LDTBS_MATCH = 1, //匹配中,（不一定成功）
	LDTBS_BATTLE = 2, //战斗状态
	LDTBS_MAX,
};

//地下城关卡队伍信息操作
enum  SyncDungeonEnterTeamInfoType
{
	SDETIT_NONE		  = 0,
	SDETIT_ENTER_SYNC = 1, //进入时候同步
	SDETIT_ADD		  = 2, //添加队员
	SDETIT_LEAVE      = 3, //队员离开
	SDETIT_UPDATE     = 4, //刷新队员信息
	SDETIT_CHG_MODE	  = 5, //刷新挑战模式
	SDETIT_TEAM_STATE = 6, //刷新队伍状态
	SDETIT_MAX,
}; 

struct LostDungRaceMember
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & roleId & name & level;
	}

	ObjID_t		roleId;
	std::string name;
	UInt16		level;
};

struct LostDungTeamInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & teamId & index & battleState;
	}
	LostDungTeamInfo()
	{
		teamId = 0;
		index = 0;
		battleState = 0;
	};
	UInt32	teamId;
	UInt8	index;
	UInt8   battleState;
};

enum LostDungBattleState
{
	LDBS_NONE		= 0,
	LDBS_CREATING	= 1, //创建中
	LDBS_NORMAL     = 2, //正常状态
	LDBS_CLOSEING   = 3, //正在关闭中
	LDBS_WAIT_DESTORY = 4, // 等待销毁
};

struct WaitEnterBatPlayer
{
	UInt64	roleId;
	UInt32	startTime;	//开始时间
};

//迷失战场匹配进战场超时时间
#define LOST_BATTLE_MATCH_OVERTIME  10

//宝箱类型
enum LostDungeonTreasCheatType
{
	LDTCT_NONE = 0,
	LDTCT_GIFT = 1, //礼包
	LDTCT_EVENT_CHANGEBATTLE = 2,
};

//战场结果
enum LostDungeonBattleReasult
{
	LDBR_NONE = 0,
	LDBR_PASS = 1, //通关
	LDBR_FAIL = 2, //失败
	LDBR_OVER = 3, //结束
	LDBR_ABN_EXIT = 4, //异常退出
};

//楼层失败处罚类型
enum LostDungeonFailHandle
{
	LDFH_NONE = 0, //无影响
	LDFH_FALL = 1, //掉层扣积分
	LDFH_SETTLE = 2, //失败并结算
};

struct LostDungeonPlayerPos
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream  & playerId & sceneId & posX & posY;
	}
	LostDungeonPlayerPos()
	{
		playerId = 0;
		sceneId = 0;
		posX = 0;
		posY = 0;
	}
	UInt64  playerId;
	UInt32	sceneId;
	UInt32	posX;
	UInt32	posY;
};

#endif