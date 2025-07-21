#ifndef _CL_SCENE_DATAENTRY_H_
#define _CL_SCENE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum SceneSubType
{
	SCENE_SUB_NULL,
	SCENE_SUB_TRADITION,
	SCENE_SUB_WUDAO,
	SCENE_SUB_GUILD_BATTLE,
	// 赏金联赛
	SCENE_SUB_PREMIUM_LEAGUE,
	//房间
	SCENE_SUB_ROOM,
	// 跨服公会战
	SCENE_SUB_CROSS_GUILD_BATTLE,
	//33积分赛场景
	SCENE_SUB_SCORE_WAR,
	//公会场景
	SCENE_SUB_GUILD,
	//战场场景
	SCENE_SUB_BATTLE,
	//吃鸡准备场景
	SCENE_SUB_CHIJI,
	//公平决斗场准备
	SCENE_EQUAL_PVP,
	//团本准备场景
	SCENE_SUB_TEAM_COPY_PREPARE = 12,
	//团本攻坚场景
	SCENE_SUB_TEAM_COPY_ATTACK = 13,
	//2V2乱斗活动场景
	SCENE_SUB_2V2_ACTIVITY = 14,
	//迷失地牢战场场景
	SCENE_SUB_LOST_DUNGEON_BATTLE = 15,
	//迷失地牢地下城选择场景
	SCENE_SUB_LOST_DUNGEON_ENTERY = 16,
	//冠军赛准备场景
	SCENE_SUB_CHAMPION_PREPARE = 17,
};

/**
 *@brief scene数据项
 */
class SceneDataEntry : public Avalon::DataEntry
{
public:
	SceneDataEntry();
	~SceneDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

	const CLPosition& GetRevivePos() const;

	// 判断是否是跨服场景
	bool IsCrossScene() const { return subType == SCENE_SUB_CROSS_GUILD_BATTLE || subType == SCENE_SUB_SCORE_WAR || subType == SCENE_SUB_ROOM; }

	// 吃鸡战场场景
	bool IsBattleScene() const { return subType == SCENE_SUB_BATTLE; }

	// 吃鸡准备场景
	bool IsChiJiScene() const { return subType == SCENE_SUB_CHIJI; }

	// 团本准备场景
	bool IsTeamCopyPrepareScene() const { return subType == SCENE_SUB_TEAM_COPY_PREPARE; }

	// 团本攻坚场景
	bool IsTeamCopyAttackScene() const { return subType == SCENE_SUB_TEAM_COPY_ATTACK; }

	// 团本场景
	bool IsTeamCopyScene() const { return subType == SCENE_SUB_TEAM_COPY_PREPARE || subType == SCENE_SUB_TEAM_COPY_ATTACK; }

	// 2v2乱斗活动
	bool Is2V2Activity() const { return subType == SCENE_SUB_2V2_ACTIVITY; }

	//冠军赛场景
	bool IsChampionScene() const { return subType == SCENE_SUB_CHAMPION_PREPARE; }


	// 迷失地牢战场场景
	bool IsLostDungeonBattleScene() const { return subType == SCENE_SUB_LOST_DUNGEON_BATTLE || subType == SCENE_SUB_LOST_DUNGEON_ENTERY;}

	// 迷失地牢地下城入口场景
	bool IsLostBattleDungeonEntryScene() const { return  subType == SCENE_SUB_LOST_DUNGEON_ENTERY; }
public:
	//id
	UInt32	id;
	//名字
	std::string	name;
	//类型
	UInt8	type;
	//进入等级
	UInt16	level;
	//选项
	UInt16	option;
	SceneSubType subType;
	//同步范围
	UInt32	syncRange;
    //出生城市
    UInt32	birthCity;
	//主城
	UInt32	majorCity;
	//最大人数
	UInt32	maxPlayerNum;
	//复活时间
	UInt32	reviveTime;
	//场景复活恢复比
	UInt32	recoverPercent;
	//复活点
	CLPosition	revivePos[3];
	//入口点X
	UInt16	enterX;
	//入口点Y
	UInt16	enterY;
	//副本次数
	UInt8	dailytimes;
	//消耗道具
	UInt32	itemId;
    //所有的地下城
    std::vector<UInt32>     dungeonList;
	//等级限制
	UInt16	levelLimit;
	//地下城id
	UInt32	dungeonId;
};

typedef Avalon::DataEntryMgr<SceneDataEntry> SceneDataEntryMgr;

typedef Avalon::DataEntryVisitor<SceneDataEntry> SceneDataEntryVisitor;

#endif
