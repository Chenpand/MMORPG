#ifndef _WS_TASK_SCRIPT_H_
#define _WS_TASK_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLObjectDefine.h>
#include <CLDungeonDefine.h>
#include <CLWarpStoneDefine.h>
#include <CLMatchDefine.h>
#include <CLErrorCode.h>
#include <CLRelationDefine.h>
#include <CLCityMonsterDataEntry.h>

struct AccountInfo;
class Npc;
class RewardGroup;
class AccountTask;
class Item;

class TaskScript : public Avalon::ScriptFile
{
	typedef std::vector<ItemReward> ItemRewardVec;
	typedef std::vector<UInt32>	IdVec;


	//added by aprilliu.2016.04.13
	struct stKillNpcEvent //击杀怪物的事件
	{
		stKillNpcEvent() :id(0), type(0), dungeon(0)
		{
		}

		UInt32	id;				//怪物id
		UInt8	type;			//怪物类型
		UInt32  dungeon;		//怪物所属的地下城id   added by aprilliu,2016.05.27
	};
	typedef	std::vector<stKillNpcEvent>		KillNPCEventList;

	struct stRoleNumChangeEvent
	{
		stRoleNumChangeEvent():level(0),occu(0)
		{

		}

		// 等级
		UInt16 level;
		// 职业
		UInt8 occu;
	};

	struct PlayerClearDungeonEvent
	{
		UInt32 id;
		LevelScore score;
		UInt32 usedTime;
	};

	typedef std::vector<PlayerClearDungeonEvent> PlayerClearDungeonEventList;

	struct SetTaskItemEvent
	{
		SetTaskItemEvent() :quality(ITEM_QUALITY_INVLID), subType(0), level(0), itemId(0), num(0) {}
		ItemQuality quality;
		UInt8  subType;
		UInt32 level;
		UInt32 itemId;
		UInt32 num;
	};
	typedef std::vector<SetTaskItemEvent> SetTaskItemEventList;

public:
	enum CondRoleNumTaskReason
	{
		crntr_level = 1,
		crntr_occu = 2,
	};

public:
	TaskScript(UInt32 id, Avalon::ScriptState* state);
	~TaskScript();

public:
	const char*  GetScriptParam(){ return m_TaskParams.c_str(); }
	void SetScriptParam(std::string scriptParam) { m_TaskParams = scriptParam; };

public:
	/**
	*@brief 绑定杀死怪物事件
	*/
	void BindKillNpcEvent(UInt32 id, UInt32 dungeon);
	void BindKillNpcTypeEvent(UInt8 type, UInt32 dungeon);
	bool CheckKillNpcEvent(UInt32 id, UInt8 type, UInt32 dungeon) const;

	/**
	*@brief 绑定冒险队等级改变事件
	*/
	void BindAdventureTeamLevelChangedEvent();
	bool CheckAdventureTeamLevelChangedEvent();

	/**
	*@brief 绑定冒险队评级改变事件
	*/
	void BindAdventureTeamGradeChangedEvent();
	bool CheckAdventureTeamGradeChangedEvent();

	/**
	*@brief 绑定角色数量改变
	*/
	void BindRoleNumChangedEvent(UInt16 level, UInt8 occu);
	void GetRoleNumChangedEventCond(UInt16& level, UInt8& occu);
	bool CheckRoleNumChangedEvent(UInt16 level, UInt8 occu, UInt8 reason);

	/**
	*@brief 绑定玩家通关事件
	*/
	void BindPlayerClearDungeonEvent(UInt32 id, UInt32 usedTime, LevelScore score);
	bool CheckPlayerClearDungeonEvent(UInt32 id, UInt32 usedTime, LevelScore score);

	/**
	*@brief 绑定玩家通关事件
	*/
	void BindPlayerClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score);
	bool CheckPlayerClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score);

	/**
	*@brief 绑定玩家完成怪物攻城事件
	*/
	void BindPlayerClearCityMonsterEvent();
	bool CheckPlayerClearCityMonsterEvent();

	/**
	*@brief 绑定玩家消耗疲劳事件
	*/
	void BindPlayerRemoveFatigueEvent();
	bool CheckPlayerRemoveFatigueEvent();

	/**
	*@brief 绑定佣兵团周常任务完成事件
	*/
	void BindPlayerCompAdventWeeklyTaskEvent(UInt32 adventCompWeeklyTaskLevel);
	bool CheckPlayerCompAdventWeeklyTaskEvent(UInt32 taskLevel);

	/**
	*@brief 提交任务物品
	*/
	void BindSetTaskItemEvent(ItemQuality quality, UInt8 subType, UInt32 level);
	void BindSetTaskItemByIdEvent(UInt32 itemId, UInt32 num);
	bool CheckSetTaskItemEvent(ItemQuality quality, UInt8 subType, UInt32 level, UInt32 itemId, UInt32& num, UInt32 checkIndex);
	UInt32 GetSetTaskItemEventNum();

public: //脚本事件
	/**
	*@brief 初始化
	*/
	void Init();

	/**
	*@brief 检查是否可接
	*/
	bool CheckAccept(AccountInfo* accountInfo);

	/**
	*@brief 接到任务后调用
	*/
	void OnAccepted(AccountInfo* accountInfo, AccountTask* task);

	/**
	*@brief 检查是否可提交
	*/
	bool CheckSubmit(AccountInfo* accountInfo);

	/**
	*@brief 提交任务
	*/
	void OnSubmitted(AccountInfo* accountInfo, AccountTask* task);

	/**
	*@brief 取消任务
	*/
	void OnAbandomed(AccountInfo* accountInfo, AccountTask* task);

	/**
	*@brief 检查任务是否完成
	*/
	void CheckFinish(AccountInfo* accountInfo, AccountTask* task);

public:
	/**
	*@brief 杀死npc事件
	*/
	void OnKillNpc(AccountInfo* player, AccountTask* task, UInt32 npcid, UInt8 type, UInt32 dungeonId);

	/**
	*@brief 冒险队(佣兵团)等级改变
	*/
	void OnAdventureTeamLevelChanged(AccountInfo* player, AccountTask* task);

	/**
	*@brief 冒险队(佣兵团)评级改变
	*/
	void OnAdventureTeamGradeChanged(AccountInfo* player, AccountTask* task);

	/**
	*@brief 角色数量改变
	*/
	void OnRoleNumChanged(AccountInfo* player, AccountTask* task, UInt32 num);

	/**
	*@brief 初始化角色数量
	*/
	void OnInitRoleNum(AccountInfo* player, AccountTask* task, UInt32 num);

	/**
	*@brief 完成佣兵团周常任务
	*/
	void OnCompAdventWeeklyTask(AccountInfo* player, AccountTask* task, UInt32 taskLevel);
public:
	/**
	*@brief 玩家通关
	*/
	void OnPlayerClearDungeon(AccountInfo* player, AccountTask* task, const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime);

	/**
	*@brief 玩家通关深渊
	*/
	void OnPlayerClearHell(AccountInfo* player, AccountTask* task, const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime);

	/**
	*@brief 玩家完成怪物攻城
	*/
	void OnPlayerClearCityMonster(AccountInfo* player, AccountTask* task, UInt32 hardType);

	/**
	*@brief 玩家消耗疲劳
	*/
	void OnPlayerRemoveFatigue(AccountInfo* player, AccountTask* task, UInt32 removeFatigue);

private:

	std::string m_TaskParams;

	/***************** 成就部分 *****************/

	//杀死npc事件列表
	KillNPCEventList	m_killNpcEvents;
	//冒险队(佣兵团)等级改变事件
	bool m_AdventureTeamLevelChangeEvent;
	//冒险队(佣兵团)评级改变事件
	bool m_AdventureTeamGradeChangeEvent;
	//角色数量改变事件
	stRoleNumChangeEvent m_RoleNumChangeEvent;
	//完成冒险团任务事件
	bool m_AdventCompWeeklyTaskEvent;
	UInt32	m_AdventCompWeeklyTaskLevel;
	/***************** 任务部分 *****************/

	//通关事件
	PlayerClearDungeonEventList m_PlayerClearDungeonEvents;
	//通关深渊事件
	PlayerClearDungeonEventList m_PlayerClearHellEvents;
	//完成怪物攻城事件
	bool m_PlayerClearCityMonsterEvent;
	//疲劳消耗事件
	bool m_PlayerRemoveFatigue;
	//上交任务物品
	SetTaskItemEventList m_SetTaskItemEvents;
};



#endif //_WS_TASK_SCRIPT_H_