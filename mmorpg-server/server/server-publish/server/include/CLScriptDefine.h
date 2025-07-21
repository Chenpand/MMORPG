#ifndef _CL_SCRIPTDEFINE_H_
#define _CL_SCRIPTDEFINE_H_

#include "CLDefine.h"
#include <AvalonScriptState.h>

/**
 *@brief 脚本错误处理
 */
class CLScriptErrorHandler : public Avalon::ScriptErrorHandler
{
public:
	void OnError(const char* str)
	{
		SYSLOG_ERROR(str);
		++SCRIPT_ERROR;
	}
};

//共用包路径
#define PACKAGE_PATH		"Scripts/Common/?.lua"


//系统提示
#define SYSNOTIFY_SCRIPT_PATH	"Scripts/SysNotify.lua"

//npc脚本
#define NPC_SCRIPT_PATH		"Scripts/Npc/"
//任务
#define TASK_SCRIPT_PATH	"Scripts/Task/"
//爬塔任务
#define LOST_DUNGEON_TASK_SCRIPT_PATH	"Scripts/LostDungeonTask/"
//账号任务
#define ACCOUNT_TASK_SCRIPT_PATH	"Scripts/AccountTask/"
//道具
#define ITEM_SCRIPT_PATH	"Scripts/Item/"
//场景
#define SCENE_SCRIPT_PATH	"Scripts/Scene/"
//AI
#define AI_SCRIPT_PATH		"Scripts/AI/"
//vehicle ai
#define VEHICLEAI_SCRIPT_PATH	"Scripts/VehicleAI/"
//AI bind
#define AI_BIND_SCRIPT_PATH	"Scripts/AI/AI_Bind.lua"

//沙箱
#define SANDBOX_SCRIPT_PATH		"Scripts/SandBox.lua"

//计数
#define COUNTER_SCRIPT_PATH	"Scripts/Counter.lua"
//日常
#define ROUTINE_SCRIPT_PATH	"Scripts/Routine.lua"
//玩家
#define PLAYER_SCRIPT_PATH	"Scripts/Player.lua"
//礼包
#define GIFTBAG_SCRIPT_PATH	"Scripts/GiftBag.lua"
//部落领土
#define TERRITORY_SCRIPT_PATH "Scripts/Territory.lua"
//终极装备
#define FINALEQUIP_SCRIPT_PATH "Scripts/FinalEquip.lua"
//装备计算
#define EQUIPCALC_SCRIPT_PATH "Scripts/EquipCalc.lua"
//护送脚本路径
#define ESCORT_SCRIPT_PATH		"Scripts/Escort.lua"
//活动礼包脚本
#define ACGIFTBAG_SCRIPT_PASH	"Scripts/ActivityGiftbag.lua"

//world定时器
#define WORLD_TIMER_SCRIPT_PATH "Scripts/WorldTimer.lua"
//战场
#define BATTLESCENE_SCRIPT_PATH "Scripts/BattleScene.lua"
//擂台赛
#define CHALLENGE_SCRIPT_PATH "Scripts/Challenge.lua"
//开服活动
#define GSACTIVITY_SCRIPT_PATH "Scripts/SortlistActivity.lua"
//新版开服活动
#define NEW_GSACTIVITY_SCRIPT_PATH "Scripts/SortlistActivity_new.lua"
//商城
#define MALL_SCRIPT_PATH "Scripts/Mall.lua"
//world玩家
#define WORLD_PLAYER_SCRIPT_PATH "Scripts/WorldPlayer.lua"
//world配置
#define WORLD_CONFIG_SCRIPT_PATH "Scripts/WorldConfig.lua"

//跨服挑战
#define WORLD_CHALLENGE_SCRIPT_PATH "Scripts/WorldChallenge.lua"

//活动
#define ACTIVITY_SCRIPT_PATH "Scripts/Activity/"

//数据库补丁
#define DBPATCH_SCRIPT_PATH	"Scripts/DBPatch.lua"

#endif
