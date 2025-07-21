#include "TaskScript.h"

TaskScript::TaskScript(UInt32 id, Avalon::ScriptState* state)
:Avalon::ScriptFile(state)
{
	SetId(id);
	m_AdventureTeamLevelChangeEvent = false;
	m_AdventureTeamGradeChangeEvent = false;
	m_PlayerRemoveFatigue = false;
	m_PlayerClearCityMonsterEvent = false;
	m_AdventCompWeeklyTaskEvent = false;
	m_AdventCompWeeklyTaskLevel = 0;
}

TaskScript::~TaskScript()
{
}

void TaskScript::BindKillNpcEvent(UInt32 id, UInt32 dungeon)
{
	//m_killNpcEvents.push_back(id);
	stKillNpcEvent event;
	event.id = id;
	event.type = 0;
	event.dungeon = dungeon;

	m_killNpcEvents.push_back(event);
}

void TaskScript::BindKillNpcTypeEvent(UInt8 type, UInt32 dungeon)
{
	stKillNpcEvent event;
	event.id = 0;
	event.type = type;
	event.dungeon = dungeon;

	m_killNpcEvents.push_back(event);
}

void TaskScript::BindAdventureTeamLevelChangedEvent()
{
	m_AdventureTeamLevelChangeEvent = true;
}

bool TaskScript::CheckAdventureTeamLevelChangedEvent()
{
	return m_AdventureTeamLevelChangeEvent;
}

void TaskScript::BindAdventureTeamGradeChangedEvent()
{
	m_AdventureTeamGradeChangeEvent = true;
}

bool TaskScript::CheckAdventureTeamGradeChangedEvent()
{
	return m_AdventureTeamGradeChangeEvent;
}

void TaskScript::BindRoleNumChangedEvent(UInt16 level, UInt8 occu)
{
	m_RoleNumChangeEvent.level = level;
	m_RoleNumChangeEvent.occu = occu;
}

void TaskScript::GetRoleNumChangedEventCond(UInt16& level, UInt8& occu)
{
	level = m_RoleNumChangeEvent.level;
	occu = m_RoleNumChangeEvent.occu;
}

bool TaskScript::CheckRoleNumChangedEvent(UInt16 level, UInt8 occu, UInt8 reason)
{
	if (reason == crntr_occu && m_RoleNumChangeEvent.occu == 0)
	{
		return false;
	}

	if (m_RoleNumChangeEvent.level == level && (m_RoleNumChangeEvent.occu == 0 || m_RoleNumChangeEvent.occu / 10 == occu / 10))
	{
		return true;
	}

	return false;
}

void TaskScript::BindPlayerClearDungeonEvent(UInt32 id, UInt32 usedTime, LevelScore score)
{
	PlayerClearDungeonEvent tmpEvent;
	tmpEvent.id = id;
	tmpEvent.score = score;
	tmpEvent.usedTime = usedTime;
	m_PlayerClearDungeonEvents.push_back(tmpEvent);
}

bool TaskScript::CheckPlayerClearDungeonEvent(UInt32 id, UInt32 usedTime, LevelScore score)
{
	for (PlayerClearDungeonEventList::const_iterator iter = m_PlayerClearDungeonEvents.begin(); iter != m_PlayerClearDungeonEvents.end(); ++iter)
	{
		if ((iter->id == 0 || iter->id == id) &&
			(iter->usedTime == 0 || iter->usedTime > usedTime) &&
			(score >= iter->score))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindPlayerClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score)
{
	PlayerClearDungeonEvent tmpEvent;
	tmpEvent.id = id;
	tmpEvent.score = score;
	tmpEvent.usedTime = usedTime;
	m_PlayerClearHellEvents.push_back(tmpEvent);
}

bool TaskScript::CheckPlayerClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score)
{
	for (PlayerClearDungeonEventList::const_iterator iter = m_PlayerClearHellEvents.begin(); iter != m_PlayerClearHellEvents.end(); ++iter)
	{
		if ((iter->id == 0 || iter->id == id) &&
			(iter->usedTime == 0 || iter->usedTime > usedTime) &&
			(score >= iter->score))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindPlayerClearCityMonsterEvent()
{
	m_PlayerClearCityMonsterEvent = true;
}

bool TaskScript::CheckPlayerClearCityMonsterEvent()
{
	return m_PlayerClearCityMonsterEvent;
}

void TaskScript::BindPlayerRemoveFatigueEvent()
{
	m_PlayerRemoveFatigue = true;
}

bool TaskScript::CheckPlayerRemoveFatigueEvent()
{
	return m_PlayerRemoveFatigue;
}

bool TaskScript::CheckKillNpcEvent(UInt32 id, UInt8 type, UInt32 dungeon) const
{
	for (KillNPCEventList::const_iterator iter = m_killNpcEvents.begin();
		iter != m_killNpcEvents.end(); ++iter)
	{
		if ((iter->dungeon == 0 || dungeon == iter->dungeon)
			&& (iter->type == 0 || iter->type == type)
			&& (iter->id == 0 || iter->id == id))
		{
			return true;
		}
	}
	return false;
}

void TaskScript::BindPlayerCompAdventWeeklyTaskEvent(UInt32 adventCompWeeklyTaskLevel)
{
	m_AdventCompWeeklyTaskEvent = true;
	m_AdventCompWeeklyTaskLevel = adventCompWeeklyTaskLevel;
}

bool TaskScript::CheckPlayerCompAdventWeeklyTaskEvent(UInt32 taskLevel)
{
	if (!m_AdventCompWeeklyTaskEvent)
	{
		return false;
	}
	return taskLevel >= m_AdventCompWeeklyTaskLevel;
}

void TaskScript::BindSetTaskItemEvent(ItemQuality quality, UInt8 subType, UInt32 level)
{
	SetTaskItemEvent taskItemEvent;
	taskItemEvent.quality = quality;
	taskItemEvent.subType = subType;
	taskItemEvent.level = level;
	m_SetTaskItemEvents.push_back(taskItemEvent);
}

void TaskScript::BindSetTaskItemByIdEvent(UInt32 itemId, UInt32 num)
{
	SetTaskItemEvent taskItemEvent;
	taskItemEvent.itemId = itemId;
	taskItemEvent.num = num;
	m_SetTaskItemEvents.push_back(taskItemEvent);
}

bool TaskScript::CheckSetTaskItemEvent(ItemQuality quality, UInt8 subType, UInt32 level, UInt32 itemId, UInt32& num, UInt32 checkIndex)
{
	if (checkIndex >= m_SetTaskItemEvents.size())
	{
		return false;
	}

	SetTaskItemEvent& itemEvent = m_SetTaskItemEvents[checkIndex];
	if (itemEvent.itemId == itemId && itemId != 0)
	{
		num = itemEvent.num;
		return true;
	}
	if ((quality != ITEM_QUALITY_INVLID && itemEvent.quality <= quality)
		&& (itemEvent.subType == subType || itemEvent.subType == 0)
		&& itemEvent.level <= level)
	{
		num = 1;
		return true;
	}
	return false;

	//for (SetTaskItemEventList::const_iterator iter = m_SetTaskItemEvents.begin(); iter != m_SetTaskItemEvents.end(); ++iter)
	//{
	//	if (iter->itemId == itemId && itemId != 0)
	//	{
	//		num = iter->num;
	//		checkIndex = iter - m_SetTaskItemEvents.begin();
	//		return true;
	//	}
	//	if ((quality != ITEM_QUALITY_INVLID && iter->quality <= quality)
	//		&& iter->subType == subType && iter->level <= level)
	//	{
	//		num = 1;
	//		checkIndex = iter - m_SetTaskItemEvents.begin();
	//		return true;
	//	}
	//}
	//return false;
}

UInt32 TaskScript::GetSetTaskItemEventNum()
{
	return m_SetTaskItemEvents.size();
}

void TaskScript::Init()
{
	Call<void>("Init", this);
}

bool TaskScript::CheckAccept(AccountInfo* accountInfo)
{
	return Call<bool>("CheckAccept", accountInfo);
}

void TaskScript::OnAccepted(AccountInfo* accountInfo, AccountTask* task)
{
	if (task)
	{
		Call<void>("OnAccepted", accountInfo, task);
	}
}

bool TaskScript::CheckSubmit(AccountInfo* accountInfo)
{
	return Call<bool>("CheckSubmit", accountInfo);
}

void TaskScript::OnSubmitted(AccountInfo* accountInfo, AccountTask* task)
{
	if (task)
	{
		Call<void>("OnSubmitted", accountInfo, task);
	}
}

void TaskScript::OnAbandomed(AccountInfo* accountInfo, AccountTask* task)
{
	if (task)
	{
		Call<void>("OnAbandomed", accountInfo, task);
	}
}

void TaskScript::CheckFinish(AccountInfo* accountInfo, AccountTask* task)
{
	if (task)
	{
		Call<void>("CheckFinish", accountInfo, task);
	}
}

void TaskScript::OnKillNpc(AccountInfo* player, AccountTask* task, UInt32 npcid, UInt8 type, UInt32 dungeonId)
{
	if (task)
	{
		Call<void>("OnKillNpc", player, task, npcid, type, dungeonId);
	}
}

void TaskScript::OnAdventureTeamLevelChanged(AccountInfo* player, AccountTask* task)
{
	Call<void>("OnAdventureTeamLevelChange", player, task);
}

void TaskScript::OnAdventureTeamGradeChanged(AccountInfo* player, AccountTask* task)
{
	Call<void>("OnAdventureTeamGradeChange", player, task);
}

void TaskScript::OnRoleNumChanged(AccountInfo* player, AccountTask* task, UInt32 num)
{
	Call<void>("OnRoleNumChanged", player, task, num);
}

void TaskScript::OnInitRoleNum(AccountInfo* player, AccountTask* task, UInt32 num)
{
	Call<void>("OnInitRoleNum", player, task, num);
}

void TaskScript::OnCompAdventWeeklyTask(AccountInfo* player, AccountTask* task, UInt32 taskLevel)
{
	Call<void>("OnCompAdventWeeklyTask", player, task, taskLevel);
}

void TaskScript::OnPlayerClearDungeon(AccountInfo* player, AccountTask* task, const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime)
{
	Call<void>("OnPlayerClearDungeon", player, task, dungeonData, score, usedTime);
}

void TaskScript::OnPlayerClearHell(AccountInfo* player, AccountTask* task, const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime)
{
	Call<void>("OnPlayerClearHell", player, task, dungeonData, score, usedTime);
}

void TaskScript::OnPlayerClearCityMonster(AccountInfo* player, AccountTask* task, UInt32 hardType)
{
	Call<void>("OnPlayerClearCityMonster", player, task, hardType);
}

void TaskScript::OnPlayerRemoveFatigue(AccountInfo* player, AccountTask* task, UInt32 removeFatigue)
{
	Call<void>("OnPlayerRemoveFatigue", player, task, removeFatigue);
}
