#ifndef _CL_GAMESESSION_DEFINE_H_
#define _CL_GAMESESSION_DEFINE_H_

#include <CLDefine.h>

enum _resultBitmask  //结果是互斥的！
{
	resultBitmask_win = 0x01,					//胜
	resultBitmask_lose = 0x02,					//负
	resultBitmask_draw = 0x04,					//平
	resultBitmask_invalid = 0x08,				//无效局

};


typedef enum _gameSessionDismissType
{
	gamesessionDismissType_normal,					    //
	gamesessionDismissType_timeout,					    //对战持续时间超时
	gamesessionDismissType_wait_timeout,			    //等待开始超时
	gamesessionDismissType_errro,					    //异常结束
	gamesessionDismissType_system,					    //系统解散
	gamesessionDismissType_end_timeout,			        //等待结束超时
	gamesessionDismissType_offline,					    //由于参战方离线
	gamesessionDismissType_framechecksum_timeout,		//帧校验超时
	gamesessionDismissType_framechecksum_different,		//帧校验不一致
	gamesessionDismissType_teamcopy_destory_dungeon,	//团本歼灭据点

    // 服务器之间使用
    gamesessionDismissType_rs_shutdown = 30,            //relayserver崩溃
    gamesessionDismissType_ms_terminate,                //ms通知结束比赛
}EGameSessionDismissType;

class FightergResult
{
public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & resultFlag & pos & accId & roleId & remainHp & remainMp;
	}

	FightergResult()
	{
		resultFlag = 0;
		pos = 0xff;
		accId = 0;
		roleId = 0;
		remainHp = HP_MP_FULL_VALUE;
		remainMp = HP_MP_FULL_VALUE;
	}

	FightergResult& operator = (const FightergResult& origin)
	{
		if (this == &origin)
		{
			return *this;
		}
		resultFlag = origin.resultFlag;
		pos = origin.pos;
		accId = origin.accId;
		roleId = origin.roleId;
		remainHp = origin.remainHp;
		remainMp = origin.remainMp;

		return *this;
	}

	FightergResult(const FightergResult& origin)
	{
		resultFlag = origin.resultFlag;
		pos = origin.pos;
		accId = origin.accId;
		roleId = origin.roleId;
		remainHp = origin.remainHp;
		remainMp = origin.remainMp;
	}


	UInt8			resultFlag;					//取值：_resultBitmask
	UInt8			pos;						//占位0-9  最多支持5V5 PK
	UInt32			accId;						//帐号id
	ObjID_t			roleId;						//角色id
	UInt32			remainHp;					//剩余血量
	UInt32			remainMp;					//剩余魔量


protected:
private:
};

enum FrameCommandType
{
	// 比赛开始
	FRAME_COMMAND_GAME_START,
	// 移动
	FRAME_COMMAND_MOVE,
	// 停止
	FRAME_COMMAND_STOP,
	// 放技能
	FRAME_COMMAND_SKILL,
	// 玩家离开战斗
	FRAME_COMMAND_LEAVE,
	// 复活
	FRAME_COMMAND_REBORN,
	// 重连开始
	FRAME_COMMAND_RECONNECT_BEGIN,
	// 重连结束
	FRAME_COMMAND_RECONNECT_END,
	// 使用物品
	FRAME_COMMAND_USE_ITEM,
	// 玩家升级
	FRAME_COMMAND_LEVEL_UP,
	// 自动战斗
	FRAME_COMMAND_AUTO_FIGHT,
	// 双击配置
	FRAME_COMMAND_DOUBLE_PRESS_CONFIG,
	// 玩家退出战斗
	FRAME_COMMAND_QUIT,
	// 战斗结束
	FRAME_COMMAND_RACE_END,
	// 网络质量
	FRAME_COMMAND_NET_QUALITY,
	// 暂停帧，目前给单局使用
	FRAME_COMMAND_RACE_PAUSE,
	// 场景切换的帧，用于死亡之塔的验证服务器
	FRAME_COMMAND_SCENE_CHANGE_AREA,
	// 中断技能
	FRAME_COMMAND_STOP_SKILL,
	// 技能产生攻击
	FRAME_COMMAND_DO_ATTACK,
	// 匹配玩家投票
	FRAME_COMMAND_MATCH_ROUND_VOTE,
	// 经过传送门
	FRAME_COMMAND_PASS_DOOR,
	// 切换武器
	FRAME_COMMAND_CHANGE_WEAPON,
	// 同步镜头
	FRAME_COMMAND_SYNC_SIGHT,
	// boss阶段变化
	FRAME_COMMAND_BOSS_PHASE_CHANGE,
	// 地下城被歼灭
	FRAME_COMMAND_DESTORY_DUNGEON,
	// 团本结束比赛
	FRAME_COMMAND_TEAMCOPY_RACE_END,
	// 团本贝西莫斯之心进度
	FRAME_COMMAND_TEAMCOPY_BIMS_PROGRESS,
	// 团本地下城复活次数
	FRAME_COMMAND_TEAMCOPY_REVIVE_CNT,
};

// 玩家事件
enum SceneServerSyncPlayerEvent
{
	// 升级
	SYNC_PLAYER_EVENT_LEVELUP,
	// 复活
	SYNC_PLAYER_EVENT_REVIVE,
	// 地下城被歼灭
	SYNC_DESTORY_DUNGEON,
	// 结束比赛
	SYNC_END_RACE,
	// 团本贝希摩斯之心能量蓄积进度
	SYNC_TEAMCOPY_BIMS_ENERY_PROGRESS,
	// 使用道具
	SYNC_USE_ITEM,
	// 团本增加复活次数
	SYNC_ADD_REVIVE_CNT,
};

struct PkPlayerRaceEndInfo
{
	PkPlayerRaceEndInfo() : roleId(0), resultFlag(0),remainHp(0),remainMp(0),damagePercent(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleId & pos & resultFlag & remainHp & remainMp & damagePercent;
	}

	bool operator ==(const PkPlayerRaceEndInfo& info) const
	{
		return roleId == info.roleId && resultFlag == info.resultFlag;
	}

	std::string ToString() const
	{
		std::stringstream ss;
		ss << "roleid:" << roleId << ", pos:" << pos << ", resultFlag:" << resultFlag << ", remainHp:" << remainHp << ", remainMp:" << remainMp << ", damage:" << damagePercent;
		return ss.str();
	}

	ObjID_t			roleId;
	UInt8			pos;
	UInt8			resultFlag;
	UInt32			remainHp;
	UInt32			remainMp;
	// 伤害百分比，乘10000倍
	UInt32			damagePercent;
};

struct Pk3V3PlayerRaceEndInfo
{
	Pk3V3PlayerRaceEndInfo() {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roomId & info & seasonLevel & seasonStar & seasonExp & matchScore & matchScoreChange & scoreWarBaseScore & scoreWarContriScore;
	}

	// 房间ID
	UInt32			roomId;
	// 结算信息
	PkPlayerRaceEndInfo info;
	// 段位
	UInt32			seasonLevel;
	// 赛季星星
	UInt32			seasonStar;
	// 赛季经验
	UInt32			seasonExp;
	// 积分
	UInt32			matchScore;
	// 积分变化
	Int32			matchScoreChange;
	// 3v3积分赛基础分
	UInt32			scoreWarBaseScore;
	// 3v3积分赛贡献分
	UInt32			scoreWarContriScore;
};

struct PkRaceEndInfo
{
	PkRaceEndInfo() : gamesessionId(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & gamesessionId & results & replayScore;
	}

	bool operator ==(const PkRaceEndInfo& other) const
	{
		return gamesessionId == other.gamesessionId && results == other.results && replayScore == other.replayScore;
	}

	bool operator !=(const PkRaceEndInfo& other) const
	{
		return !(*this == other);
	}

	bool empty() const { return gamesessionId == 0 || results.empty(); }

	std::string ToString()
	{
		std::stringstream ss;
		ss << "session:" << gamesessionId << ", replayScore:" << replayScore << ", infos:";
		for (UInt32 i = 0; i < results.size(); i++)
		{
			auto& info = results[i];
			if (i != 0)
			{
				ss << "|";
			}
			ss << info.ToString();
		}

		return ss.str();
	}

	ObjID_t                                 gamesessionId;
	std::vector<PkPlayerRaceEndInfo>		results;
	UInt32									replayScore;
};

#endif
