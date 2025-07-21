#include "BaseTaskMgr.h"

#include <CLTaskDefine.h>
#include <CLSysNotify.h>

#include "TaskScript.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"
#include "EscortScript.h"
#include "Player.h"
#include "Scene.h"
#include "SSRouter.h"
#include "ActivityMgr.h"
#include "CLEqualItemDataEntry.h"

BaseTaskMgr::BaseTaskMgr()
{
	m_pOwner = NULL;
}

BaseTaskMgr::~BaseTaskMgr()
{
	CLDeleteVisitor<BaseTask> delVisitor;
	VisitTasks(delVisitor);
	m_Tasks.clear();
}

bool BaseTaskMgr::AddTask(BaseTask* task)
{
	if (task == NULL) return false;
	if (m_Tasks.insert(std::make_pair(task->GetDataId(), task)).second)
	{
		//SetDirty();
		//task->InsertTask();
		task->UpdateTask();
		return true;
	}
	return false;
}

bool BaseTaskMgr::LoadTask(BaseTask* task)
{
	if (task == NULL) return false;
	if (m_Tasks.insert(std::make_pair(task->GetDataId(), task)).second)
	{
		return true;
	}
	return false;
}

BaseTask* BaseTaskMgr::FindTask(UInt32 id) const
{
	TaskMap::const_iterator iter = m_Tasks.find(id);
	if (iter != m_Tasks.end()) return iter->second;
	return NULL;
}

void BaseTaskMgr::RemoveTask(BaseTask* task)
{
	if (task == NULL) return;
	TaskMap::iterator iter = m_Tasks.find(task->GetDataId());
	if (iter != m_Tasks.end())
	{
		if (iter->second != NULL)
			iter->second->DeleteTask();
		m_Tasks.erase(iter);
		//SetDirty();
	}
}

void BaseTaskMgr::ClearTask()
{
	class ClearTaskVisitor : public BaseTaskVisitor
	{
	public:
		ClearTaskVisitor(){};
		~ClearTaskVisitor(){};

		bool operator()(BaseTask* task)
		{
			task->DeleteTask();
			return true;
		}
	};

	ClearTaskVisitor clearVisitor;
	VisitTasks(clearVisitor);

	CLDeleteVisitor<BaseTask> delVisitor;
	VisitTasks(delVisitor);
	m_Tasks.clear();

	//SetDirty();
}

UInt32 BaseTaskMgr::GetTaskNum() const
{
	return m_Tasks.size();
}

void BaseTaskMgr::VisitTasks(BaseTaskVisitor& visitor)
{
	for (TaskMap::iterator iter = m_Tasks.begin(); iter != m_Tasks.end();)
	{
		BaseTask* task = iter->second;
		++iter;
		if (!visitor(task)) return;
	}
}

void BaseTaskMgr::GetTaskList(std::vector<TaskBriefInfo>& taskList)
{
	GetTaskInfoVisitor visitor(taskList);
	VisitTasks(visitor);
}

//added by aprilliu, 2016.04.20
void BaseTaskMgr::GetDungeonTaskList(UInt32 dungeonId, std::vector<UInt32>& taskList)
{
	class GetDungeonTaskInfoVisitor : public BaseTaskVisitor
	{
	public:
		GetDungeonTaskInfoVisitor(UInt32 dungeonId, std::vector<UInt32>& taskIdList) : m_DungeonId(dungeonId), m_TaskIdList(taskIdList)
		{
			m_TaskIdList.clear();
		}

		bool operator()(BaseTask* task)
		{
			TaskDataEntryMgr::TaskIdVec taskIds;
			taskIds.clear();
			TaskDataEntryMgr::Instance()->GetDungeonTasks(m_DungeonId, taskIds);
			for (auto iter = taskIds.begin(); iter != taskIds.end(); ++iter)
			{
				if (*iter == task->GetDataId() && TASK_UNFINISH == task->GetStatus()) //只关注未完成状态的任务。跟策划确认过！
				{
					m_TaskIdList.push_back(*iter);
				}
			}

			return true;
		}

	protected:
	private:
		UInt32 m_DungeonId;
		std::vector<UInt32>& m_TaskIdList;
	};

	GetDungeonTaskInfoVisitor visitor(dungeonId, taskList);
	VisitTasks(visitor);
}

bool BaseTaskMgr::GetInitTaskBriefInfo(UInt32 id, TaskBriefInfo& info)
{
	TaskDataEntry* dataEntry = TaskDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL) return false;

	info.id = id;
	info.status = TASK_INIT;

	return true;
}

void BaseTaskMgr::CheckFinish()
{
	class CheckFinishVisitor : public BaseTaskVisitor
	{
	public:
		CheckFinishVisitor(){}
		bool operator()(BaseTask* task)
		{
			task->CheckFinish();
			return true;
		}
	};

	CheckFinishVisitor visitor;
	VisitTasks(visitor);

}

UInt32 TASK_VISIT_NUM = 0;
UInt32 TASK_CALL_SCRIPT_NUM = 0;

void BaseTaskMgr::OnGetItem(UInt32 id, UInt8 quality, UInt8 stengthen, UInt16 num)
{
	class OnGetItemVisitor : public BaseTaskVisitor
	{
	public:
		OnGetItemVisitor(UInt32 id, UInt16 num, UInt8 quality, UInt8 strengthen) :m_ItemId(id), m_Num(num), m_Quality(quality), m_Strengthen(strengthen){}

		bool operator()(BaseTask* task)
		{
			TASK_VISIT_NUM++;
			if (task->GetStatus() == TASK_FINISHED || task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckGetItemEvent(m_ItemId, m_Quality, m_Strengthen))
				{
					script->OnGetItem(task->GetOwner(), m_ItemId, m_Num, task);
					TASK_CALL_SCRIPT_NUM++;
				}
			}
			return true;
		}
	private:
		UInt32 m_ItemId;
		UInt16 m_Num;
		UInt8  m_Quality;
		UInt8  m_Strengthen;
	};
	OnGetItemVisitor visitor(id, num, quality, stengthen);
	//VisitTasks(visitor);

	GetItemEvent event;
	event.id = id;
	event.quality = quality;
	event.strengthen = stengthen;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		TASK_VISIT_NUM++;
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_FINISHED || task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckGetItemEvent(event.id, event.quality, event.strengthen))
				{
					script->OnGetItem(task->GetOwner(), event.id, num, task);
					TASK_CALL_SCRIPT_NUM++;
				}
			}
		}
	}

	//如果是等价物。对等价物再做一次
	if (EqualItemDataEntryMgr::Instance()->GetItemIdByEqualItemID(id) != 0)
	{
		GetItemEvent event;
		event.id = EqualItemDataEntryMgr::Instance()->GetItemIdByEqualItemID(id);
		event.quality = quality;
		event.strengthen = stengthen;
		auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
		for (auto scriptID : scriptSet)
		{
			TASK_VISIT_NUM++;
			auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
			auto task = FindTask(scriptID);
			if (script != nullptr && task != nullptr)
			{
				if (task->GetStatus() == TASK_FINISHED || task->GetStatus() == TASK_UNFINISH)
				{
					if (script->CheckGetItemEvent(event.id, event.quality, event.strengthen))
					{
						script->OnGetItem(task->GetOwner(), event.id, num, task);
						TASK_CALL_SCRIPT_NUM++;
					}
				}
			}
		}
	}
}


void BaseTaskMgr::OnRmItem(UInt32 id, UInt8 quality, UInt8 strengthen, UInt16 num)
{
	class OnRmItemVisitor : public BaseTaskVisitor
	{
	public:
		OnRmItemVisitor(UInt32 id, UInt8 quality, UInt8 strengthen, UInt16 num) 
			: m_ItemId(id), m_Num(num), m_Quality(quality), m_Strengthen(strengthen){}

		bool operator()(BaseTask* task)
		{
			//modified by huchenhui, 2016.06.24 (在已接受未完成的状态也关注,因为道具减少必须重新设置数量)
			if (task->GetStatus() == TASK_FINISHED || task->GetStatus() == TASK_UNFINISH) //只有已完成未提交状态才关注（因为只有TASK_FINISHED状态才可能因为道具减少而变成TASK_UNFINISH）
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckGetItemEvent(m_ItemId, m_Quality, m_Strengthen))
				{
					script->OnRmItem(task->GetOwner(), m_ItemId, m_Num, task);
				}
			}

			return true;
		}

	private:
		UInt32 m_ItemId;
		UInt16 m_Num;
		UInt8  m_Quality;
		UInt8  m_Strengthen;
	};

	OnRmItemVisitor visitor(id, quality, strengthen, num);
	//VisitTasks(visitor);


	GetItemEvent event;
	event.id = id;
	event.quality = quality;
	event.strengthen = strengthen;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_FINISHED || task->GetStatus() == TASK_UNFINISH) //只有已完成未提交状态才关注（因为只有TASK_FINISHED状态才可能因为道具减少而变成TASK_UNFINISH）
			{
				if (script->CheckGetItemEvent(event.id, quality, strengthen))
				{
					script->OnRmItem(task->GetOwner(), event.id, num, task);
				}
			}
		}
	}
	//如果是等价物。对等价物再做一次
	if (EqualItemDataEntryMgr::Instance()->GetItemIdByEqualItemID(id) != 0)
	{
		GetItemEvent event;
		event.id = EqualItemDataEntryMgr::Instance()->GetItemIdByEqualItemID(id);
		event.quality = quality;
		event.strengthen = strengthen;
		auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
		for (auto scriptID : scriptSet)
		{
			auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
			auto task = FindTask(scriptID);
			if (script != nullptr && task != nullptr)
			{
				if (task->GetStatus() == TASK_FINISHED || task->GetStatus() == TASK_UNFINISH) //只有已完成未提交状态才关注（因为只有TASK_FINISHED状态才可能因为道具减少而变成TASK_UNFINISH）
				{
					if (script->CheckGetItemEvent(event.id, quality, strengthen))
					{
						script->OnRmItem(task->GetOwner(), event.id, num, task);
					}
				}
			}
		}
	}
}


void BaseTaskMgr::OnUseItem(UInt32 itemid)
{
	class OnUseItemVisitor : public BaseTaskVisitor
	{
	public:
		OnUseItemVisitor(UInt32 id) :m_ItemId(id){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckUseItemEvent(m_ItemId))
				{
					script->OnUseItem(task->GetOwner(), m_ItemId, task);
				}
			}
			return true;
		}
	private:
		UInt32 m_ItemId;
	};
	OnUseItemVisitor visitor(itemid);
	//VisitTasks(visitor);

	UseItemEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckUseItemEvent(itemid))
				{
					script->OnUseItem(task->GetOwner(), itemid, task);
				}
			}
		}
	}
}


void BaseTaskMgr::OnChangeMoney(UInt32 type, Int32 num)
{
	class OnChangeMoneyVisitor : public BaseTaskVisitor
	{
	public:
		OnChangeMoneyVisitor(UInt32 type, Int32 num) : m_MoneyType(type), m_Num(num) {}

		bool operator()(BaseTask* task)
		{
			
			if ((task->GetStatus() == TASK_FINISHED || task->GetStatus() == TASK_UNFINISH) || IsAchievementStatus(task->GetType(), task->GetStatus()))
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckChangeMoneyEvent(m_MoneyType))
				{
					script->OnChangeMoney(task->GetOwner(), m_MoneyType, m_Num, task);
				}
			}
			return true;
		}

	private:
		UInt32 m_MoneyType;
		Int32 m_Num;

	};
	OnChangeMoneyVisitor visitor(type, num);
	//VisitTasks(visitor);

	ChangeMoneyEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if ((task->GetStatus() == TASK_FINISHED || task->GetStatus() == TASK_UNFINISH) || IsAchievementStatus(task->GetType(), task->GetStatus()))
			{
				if (script->CheckChangeMoneyEvent(type))
				{
					script->OnChangeMoney(task->GetOwner(), type, num, task);
				}
			}
		}
	}
}


void BaseTaskMgr::OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon)
{
	class OnOwnedNpcDiedVisitor : public BaseTaskVisitor
	{
	public:
		OnOwnedNpcDiedVisitor(UInt32 npcid, UInt8 npctype, UInt32 dungeonId) :m_NpcId(npcid), m_NpcType(npctype), m_Dungeon(dungeonId){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckKillNpcEvent(m_NpcId, m_NpcType, m_Dungeon))
				{
					script->OnKillNpc(task->GetOwner(), m_NpcId, m_NpcType, task, m_Dungeon);
				}
			}
			return true;
		}

	private:
		UInt32 m_NpcId;
		UInt8 m_NpcType;
		UInt32 m_Dungeon;
	};
	OnOwnedNpcDiedVisitor visitor(npcid, type, dungeon);
	//VisitTasks(visitor);

	stKillNpcEvent event;
	event.id = npcid;
	event.type = type;
	event.dungeon = dungeon;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckKillNpcEvent(event.id, event.type, event.dungeon))
				{
					script->OnKillNpc(task->GetOwner(), event.id, event.type, task, event.dungeon);
				}
			}
		}
	}
}

void BaseTaskMgr::OnKillCityMonster(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid)
{
	class OnKillCityMonsterVisitor : public BaseTaskVisitor
	{
	public:
		OnKillCityMonsterVisitor(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid)
			:m_CityMonsterType(cityMonsterType), m_MonsterId(monsterId), m_MonsterGuid(monsterGuid){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckKillCityMonsterEvent(m_CityMonsterType, m_MonsterId, m_MonsterGuid))
				{
					script->OnKillCityMonster(task->GetOwner(), task, m_CityMonsterType, m_MonsterId, m_MonsterGuid);
				}
			}
			return true;
		}

	private:
		CityMonsterType m_CityMonsterType;
		UInt32 m_MonsterId;
		UInt64 m_MonsterGuid;
	};

	OnKillCityMonsterVisitor visitor(cityMonsterType, monsterId, monsterGuid);
	//VisitTasks(visitor);


	KillCityMonsterEvent event(cityMonsterType, monsterId);
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckKillCityMonsterEvent(event.type, event.monsterId, monsterGuid))
				{
					script->OnKillCityMonster(task->GetOwner(), task, event.type, event.monsterId, monsterGuid);
				}
			}
		}
	}
}

void BaseTaskMgr::OnMoved()
{
	UInt32 mapid = GetOwner()->GetScene()->GetMapID();
	CLPosition pos = GetOwner()->GetPos();

	if (GetOwner()->GetZoneTask() != 0)
	{
		TaskScript* script = ScriptMgr::Instance()->FindTaskScript(GetOwner()->GetZoneTask());
		if (script == NULL)
		{
			GetOwner()->SetZoneTask(0);
			return;
		}

		if (!script->CheckInTriggerZone(mapid, pos) && !script->CheckInEventZone(mapid, pos))
		{
			GetOwner()->SetZoneTask(0);
			return;
		}
	}
	else
	{
		//进入区域事件触发
		class OnMovedVisitor : public BaseTaskVisitor
		{
		public:
			OnMovedVisitor(UInt32 mapid, const CLPosition& pos)
				:m_MapId(mapid), m_Pos(pos){}

			bool operator()(BaseTask* task)
			{
				if (task->GetStatus() == TASK_UNFINISH)
				{
					TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
					if (script != NULL && script->CheckInEventZone(m_MapId, m_Pos))
					{
						task->GetOwner()->SetZoneTask(task->GetDataId());
						script->OnEnterZone(task->GetOwner());
						return false;
					}
				}
				return true;
			}

		private:
			UInt32		m_MapId;
			CLPosition	m_Pos;
		};
		OnMovedVisitor visitor(mapid, pos);
		//VisitTasks(visitor);

		MovedEvent event;
		auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
		for (auto scriptID : scriptSet)
		{
			auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
			auto task = FindTask(scriptID);
			if (script != nullptr && task != nullptr)
			{
				if (task->GetStatus() == TASK_UNFINISH)
				{
					if (script->CheckInEventZone(mapid, pos))
					{
						task->GetOwner()->SetZoneTask(task->GetDataId());
						script->OnEnterZone(task->GetOwner());
					}
				}
			}
		}

		if (GetOwner()->GetZoneTask() != 0) return;
	}
}


void BaseTaskMgr::OnRemakeEquip(UInt32 id, UInt8 type)
{
	class OnRemakeEquipVisitor : public BaseTaskVisitor
	{
	public:
		OnRemakeEquipVisitor(UInt32 id, UInt8 type) :m_ItemId(id), m_RemakeType(type){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckRemakeEquipEvent(m_ItemId, m_RemakeType))
				{
					script->OnRemakeEquip(task->GetOwner(), task, m_ItemId, m_RemakeType);
				}
			}
			return true;
		}
	private:
		UInt32	m_ItemId;
		UInt8	m_RemakeType;
	};
	OnRemakeEquipVisitor visitor(id, type);
	//VisitTasks(visitor);


	EquipRemakeEvent event;
	event.id = id;
	event.remakeType = type;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckRemakeEquipEvent(event.id, event.remakeType))
				{
					script->OnRemakeEquip(task->GetOwner(), task, event.id, event.remakeType);
				}
			}
		}
	}
}

void BaseTaskMgr::OnCalculateValuedScore(ItemSubType subType, ItemQuality quality, UInt32 score)
{
	class OnCalculateValuedScoreVisitor : public BaseTaskVisitor
	{
	public:
		OnCalculateValuedScoreVisitor(ItemSubType subType, ItemQuality quality, UInt32 score) {}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckCalculateEquipScoreEvent(m_ItemSubType, m_ItemQuality, m_Score))
				{
					script->OnCalculateValuedScore(task->GetOwner(), task, m_ItemSubType, m_ItemQuality, m_Score);
				}
			}
			return true;
		}

	private:
		ItemSubType m_ItemSubType;
		ItemQuality m_ItemQuality;
		UInt32 m_Score;
	};

	OnCalculateValuedScoreVisitor visitor(subType, quality, score);
	//VisitTasks(visitor);

	EquitScoreEvent event;
	event.subType = subType;
	event.quality = quality;
	event.score = score;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckCalculateEquipScoreEvent(subType, quality, score))
				{
					script->OnCalculateValuedScore(task->GetOwner(), task, subType, quality, score);
				}
			}
		}
	}
}

void BaseTaskMgr::OnAddPreciousBead()
{
	class OnAddPreciousBeadVisitor : public BaseTaskVisitor
	{
	public:
		OnAddPreciousBeadVisitor(){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckAddPreciousBeadEvent())
				{
					script->OnAddPreciousBead(task->GetOwner(), task);
				}
			}
			return true;
		}
	};
	OnAddPreciousBeadVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnStrengthenEquip(UInt32 errorCode, UInt8 oldStrengthen, UInt8 newStrengthen)
{
	class OnStrengthenEquipVisitor : public BaseTaskVisitor
	{
	public:
		OnStrengthenEquipVisitor(UInt32 errorCode, UInt8 oldStrengthen, UInt8 newStrengthen)
			:m_ErrorCode(errorCode), m_OldStrengthen(oldStrengthen), m_NewStrengthen(newStrengthen){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckStrengthenEquipEvent(m_ErrorCode, m_OldStrengthen, m_NewStrengthen))
				{
					script->OnStrengthenEquip(task->GetOwner(), task, m_ErrorCode, m_OldStrengthen, m_NewStrengthen);
				}
			}
			return true;
		}
	private:
		UInt32 m_ErrorCode;
		UInt8 m_OldStrengthen;
		UInt8 m_NewStrengthen;
	};
	OnStrengthenEquipVisitor visitor(errorCode, oldStrengthen, newStrengthen);
	//VisitTasks(visitor);


	StrengthenEquipEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckStrengthenEquipEvent(errorCode, oldStrengthen, newStrengthen))
				{
					script->OnStrengthenEquip(task->GetOwner(), task, errorCode, oldStrengthen, newStrengthen);
				}
			}
		}
	}
}


void BaseTaskMgr::OnDecomposeEquip()
{
	class OnDecomposeEquipVisitor : public BaseTaskVisitor
	{
	public:
		OnDecomposeEquipVisitor() {}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckDecomposeEquipEvent())
				{
					script->OnDecomposeEquip(task->GetOwner(), task);
				}
			}
			return true;
		}
	};
	OnDecomposeEquipVisitor visitor;
	VisitTasks(visitor);
}


void BaseTaskMgr::OnPutEquip(UInt16 level, UInt32 subType, UInt8 color, UInt32 strengthen)
{
	class OnPutEquipVisitor : public BaseTaskVisitor
	{
	public:
		OnPutEquipVisitor(UInt16 level, UInt32 subType, UInt8 color, UInt32 strengthen)
			:m_Level(level), m_SubType(subType), m_Color(color), m_Strengthen(strengthen) { }

		bool operator()(BaseTask* task)
		{
			if (IsAchievementStatus(task->GetType(), task->GetStatus()))
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckPutEquipEvent(m_Level, m_Color, m_Strengthen))
				{
					script->OnPutEquip(task->GetOwner(), task, m_Level, m_SubType, m_Strengthen);
				}
			}
			return true;
		}

	private:
		UInt16 m_Level;
		UInt32 m_SubType;
		UInt8 m_Color;
		UInt32 m_Strengthen;
	};
	OnPutEquipVisitor visitor(level, subType, color, strengthen);
	//VisitTasks(visitor);

	PutEquipEvent event;
	event.level = level;
	event.strengthen = strengthen;
	event.color = color;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (IsAchievementStatus(task->GetType(), task->GetStatus()))
			{
				if (script->CheckPutEquipEvent(event.level, event.color, event.strengthen))
				{
					script->OnPutEquip(task->GetOwner(), task, event.level, event.color, event.strengthen);
				}
			}
		}
	}
}

void BaseTaskMgr::OnPutFashion(ItemSubType subType, UInt8 color, UInt8 isTransparent)
{
	class OnPutFashionVisitor : public BaseTaskVisitor
	{
	public:
		OnPutFashionVisitor(ItemSubType subType, UInt8 color, UInt8 isTrans) 
			: m_ItemSubType(subType), m_Color(color), m_IsTransparent(isTrans){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckPutFashionEvent(m_ItemSubType, m_Color, m_IsTransparent))
				{
					script->OnPutFashion(task->GetOwner(), task, m_ItemSubType, m_Color);
				}
			}
			return true;
		}

	private:
		ItemSubType m_ItemSubType;
		UInt8 m_Color;
		UInt8 m_IsTransparent;
	};

	OnPutFashionVisitor visitor(subType, color, isTransparent);
	//VisitTasks(visitor);



	PutFashionEvent event(subType, color, isTransparent);
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckPutFashionEvent(subType, color, isTransparent))
				{
					script->OnPutFashion(task->GetOwner(), task, subType, color);
				}
			}
		}
	}

}

void BaseTaskMgr::OnCatchPet(UInt32 id)
{
	class OnCatchPetVisitor : public BaseTaskVisitor
	{
	public:
		OnCatchPetVisitor(UInt32 id) :m_Id(id){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckCatchPetEvent(m_Id))
				{
					script->OnCatchPet(task->GetOwner(), m_Id);
				}
			}
			return true;
		}
	private:
		UInt32	m_Id;
	};
	OnCatchPetVisitor visitor(id);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnAddRetinue(RetinueQuality quality)
{
	class OnAddRetinueVisitor : public BaseTaskVisitor
	{
	public:
		OnAddRetinueVisitor(RetinueQuality quality) :m_RetinueQuality(quality){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckAddRetinueEvent(m_RetinueQuality))
				{
					script->OnAddRetinue(task->GetOwner(), task, m_RetinueQuality);
				}
			}
			return true;
		}
	private:
		RetinueQuality m_RetinueQuality;
	};

	OnAddRetinueVisitor visitor(quality);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnRetinueChangeSkill(std::vector<RetinueSkill> newSkills)
{
	class OnRetinueChangeSkillVisitor : public BaseTaskVisitor
	{
	public:
		OnRetinueChangeSkillVisitor(std::vector<RetinueSkill> newSkills) :m_Skills(newSkills){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());

				for (UInt32 i = 0; i < m_Skills.size(); ++i)
				{
					if (script != NULL && script->CheckRetinueChangeSkillEvent(m_Skills[i].buffId))
					{
						script->OnRetinueChangeSkill(pTask->GetOwner(), pTask, m_Skills[i].buffId);
						break;
					}
				}
			}
			return true;
		}
	private:
		std::vector<RetinueSkill> m_Skills;
	};

	OnRetinueChangeSkillVisitor visitor(newSkills);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnRetinueUpLevel(UInt32 retinueId, UInt8 retinueLevel)
{
	class OnRetinueUpLevelVisitor : public BaseTaskVisitor
	{
	public:
		OnRetinueUpLevelVisitor(UInt32 retinueId, UInt8 retinueLevel) :m_RetinueId(retinueId), m_RetinueLevel(retinueLevel){};
		~OnRetinueUpLevelVisitor(){};

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckRetinueUpLevelSkillEvent(m_RetinueId, m_RetinueLevel))
				{
					script->OnRetinueUpLevel(pTask->GetOwner(), pTask, m_RetinueId, m_RetinueLevel);
				}
			}
			return true;
		}
	private:
		UInt32 m_RetinueId;
		UInt8 m_RetinueLevel;
	};

	OnRetinueUpLevelVisitor visitor(retinueId, retinueLevel);
	//VisitTasks(visitor);

	RetinueUpLevelEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckRetinueUpLevelSkillEvent(retinueId, retinueLevel))
				{
					script->OnRetinueUpLevel(task->GetOwner(), task, retinueId, retinueLevel);
				}
			}
		}
	}
}

void BaseTaskMgr::OnRetinueUpStar(UInt32 retinueId, UInt8 retinueStar)
{
	class OnRetinueUpStarVisitor : public BaseTaskVisitor
	{
	public:
		OnRetinueUpStarVisitor(UInt32 retinueId, UInt8 retinueStar) :m_RetinueId(retinueId), m_RetinueStar(retinueStar){};
		~OnRetinueUpStarVisitor(){};

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckRetinueUpStarEvent(m_RetinueId, m_RetinueStar))
				{
					script->OnRetinueUpStar(pTask->GetOwner(), pTask, m_RetinueId, m_RetinueStar);
				}
			}
			return true;
		}
	private:
		UInt32 m_RetinueId;
		UInt8 m_RetinueStar;
	};

	OnRetinueUpStarVisitor visitor(retinueId, retinueStar);
	VisitTasks(visitor);

	RetinueUpLevelEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckRetinueUpStarEvent(retinueId, retinueStar))
				{
					script->OnRetinueUpStar(task->GetOwner(), task, retinueId, retinueStar);
				}
			}
		}
	}
}

void BaseTaskMgr::OnCopyScenePassed(UInt32 id)
{
	class OnCopyScenePassedVisitor : public BaseTaskVisitor
	{
	public:
		OnCopyScenePassedVisitor(UInt32 id) :m_Id(id){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckCopyScenePassedEvent(m_Id, LevelScore::LS_C, 0, 0))
				{
					script->OnCopyScenePassed(task->GetOwner(), m_Id, task);
				}
			}
			return true;
		}
	private:
		UInt32	m_Id;
	};
	OnCopyScenePassedVisitor visitor(id);
	//VisitTasks(visitor);

	ClearDungeonEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckCopyScenePassedEvent(id, LevelScore::LS_C, 0, 0))
				{
					script->OnCopyScenePassed(task->GetOwner(), id, task);
				}
			}
		}
	}
}

void BaseTaskMgr::OnClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
{
	class OnClearDungeonVisitor : public BaseTaskVisitor
	{
	public:
		OnClearDungeonVisitor(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
			:m_DungeonId(dungeonId), m_Beated(beated), m_Areas(areas), m_Score(score), m_UsedTime(usedTime), m_ReviveCount(reviveCount) { }

		bool operator()(BaseTask* task)
		{
			if (TASK_UNFINISH == task->GetStatus())
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckCopyScenePassedEvent(m_DungeonId, m_Score, m_UsedTime, m_ReviveCount))
				{
					//script->OnCopyScenePassed(task->GetOwner(), m_Id);
					script->OnClearDungeon(task->GetOwner(), task, m_DungeonId, m_Beated, m_Areas);
				}
			}
			return true;
		}

	protected:
	private:
		UInt32 m_DungeonId;
		UInt16 m_Beated;
		UInt16 m_Areas;
		LevelScore m_Score;
		UInt32 m_UsedTime;
		UInt32 m_ReviveCount;
	};

	OnClearDungeonVisitor visitor(dungeonId, beated, areas, score, usedTime, reviveCount);
	//VisitTasks(visitor);

	ClearDungeonEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (TASK_UNFINISH == task->GetStatus())
			{
				if (script->CheckCopyScenePassedEvent(dungeonId, score, usedTime, reviveCount))
				{
					script->OnClearDungeon(task->GetOwner(), task, dungeonId, beated, areas);
				}
			}
		}
	}
}

void BaseTaskMgr::OnClearActivityDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
{
	class OnClearActivityDungeonVisitor : public BaseTaskVisitor
	{
	public:
		OnClearActivityDungeonVisitor(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
			:m_DungeonId(dungeonId), m_Beated(beated), m_Areas(areas), m_Score(score), m_UsedTime(usedTime), m_ReviveCount(reviveCount) { }

		bool operator()(BaseTask* task)
		{
			if (TASK_UNFINISH == task->GetStatus())
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckClearActivityDungeonEvent(m_DungeonId, m_Score, m_UsedTime, m_ReviveCount))
				{
					//script->OnCopyScenePassed(task->GetOwner(), m_Id);
					script->OnClearActivityDungeon(task->GetOwner(), task, m_DungeonId, m_Beated, m_Areas);
				}
			}
			return true;
		}

	protected:
	private:
		UInt32 m_DungeonId;
		UInt16 m_Beated;
		UInt16 m_Areas;
		LevelScore m_Score;
		UInt32 m_UsedTime;
		UInt32 m_ReviveCount;
	};

	OnClearActivityDungeonVisitor visitor(dungeonId, beated, areas, score, usedTime, reviveCount);
	//VisitTasks(visitor);


	ClearDungeonEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (TASK_UNFINISH == task->GetStatus())
			{
				if (script->CheckClearActivityDungeonEvent(dungeonId, score, usedTime, reviveCount))
				{
					//script->OnCopyScenePassed(task->GetOwner(), m_Id);
					script->OnClearActivityDungeon(task->GetOwner(), task, dungeonId, beated, areas);
				}
			}
		}
	}
}


void BaseTaskMgr::OnBabelPassed(UInt32 id)
{
	class OnBabelPassedVisitor : public BaseTaskVisitor
	{
	public:
		OnBabelPassedVisitor(UInt32 id) :m_Id(id){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckBabelPassedEvent(m_Id))
				{
					script->OnBabelPassed(task->GetOwner(), m_Id);
				}
			}
			return true;
		}
	private:
		UInt32	m_Id;
	};
	OnBabelPassedVisitor visitor(id);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnBuyMallItem(UInt32 mallItemId, UInt32 mallItemNum)
{
	class OnBuyMallItemVisitor : public BaseTaskVisitor
	{
	public:
		OnBuyMallItemVisitor(UInt32 mallItemId, UInt32 mallItemNum) : m_MallItemId(mallItemId), m_MallItemNum(mallItemNum){}

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckBuyMallItemEvent(m_MallItemId))
				{
					script->OnBuyMallItem(pTask->GetOwner(), pTask, m_MallItemId, m_MallItemNum);
				}
			}
			return true;
		}

	private:
		UInt32	m_MallItemId;
		UInt32	m_MallItemNum;
	};

	OnBuyMallItemVisitor visitor(mallItemId, mallItemNum);
	VisitTasks(visitor);
}


void BaseTaskMgr::OnEnterTribe()
{
	class OnEnterTribeVisitor : public BaseTaskVisitor
	{
	public:
		OnEnterTribeVisitor(){}

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckEnterTribeEvent())
				{
					script->OnEnterTribe(pTask->GetOwner());
				}
			}
			return true;
		}
	};
	OnEnterTribeVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnStoryEnd(UInt32 id)
{
	class OnStoryEndVisitor : public BaseTaskVisitor
	{
	public:
		OnStoryEndVisitor(UInt32 storyId) :m_StoryId(storyId){}

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH || pTask->GetStatus() == TASK_FINISHED)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckStoryEndEvent(m_StoryId))
				{
					script->OnStoryEnd(pTask->GetOwner(), m_StoryId, pTask);
				}
			}
			return true;
		}

	private:
		UInt32 m_StoryId;
	};
	OnStoryEndVisitor visitor(id);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnTaskSucced(BaseTask* task)
{
	if (task == NULL) return;
	class OnTaskSuccedVisitor : public BaseTaskVisitor
	{
	public:
		OnTaskSuccedVisitor(UInt32 taskId) :m_TaskId(taskId){}

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckTaskSuccedEvent(m_TaskId))
				{
					script->OnTaskSucced(pTask->GetOwner(), m_TaskId);
				}
			}
			return true;
		}

	private:
		UInt32 m_TaskId;
	};
	OnTaskSuccedVisitor visitor(task->GetDataId());
	VisitTasks(visitor);
}

void BaseTaskMgr::OnSubmitTaskSucceed(UInt32 taskId)
{
	class OnSubmitTaskVisitor : public BaseTaskVisitor
	{
	public:
		OnSubmitTaskVisitor(UInt32 taskId) :m_TaskId(taskId)
		{
		}

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (NULL != script && script->CheckSubmitTaskSucceedEvent(m_TaskId))
				{
					script->OnSubmitTaskSucceed(pTask->GetOwner(), m_TaskId, pTask);
				}
			}
			return true;
		}

	protected:
	private:
		UInt32 m_TaskId;
	};

	//
	OnSubmitTaskVisitor visitor(taskId);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnSubmitActiveTaskSucceed(UInt32 templateId, UInt32 taskId)
{
	class OnSubmitActiveTaskVisitor : public BaseTaskVisitor
	{
	public:
		OnSubmitActiveTaskVisitor(UInt32 templateId, UInt32 taskId) :m_TemplateId(templateId), m_TaskId(taskId) { }

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (NULL != script && script->CheckSubmitActiveTaskSucceedEvent(m_TemplateId, m_TaskId))
				{
					script->OnSubmitActiveTaskSucceed(pTask->GetOwner(), pTask, m_TemplateId, m_TaskId);
				}
			}
			return true;
		}

	protected:
	private:
		UInt32 m_TemplateId;
		UInt32 m_TaskId;
	};

	OnSubmitActiveTaskVisitor visitor(templateId, taskId);
	//VisitTasks(visitor);

	ActiveTaskEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckSubmitActiveTaskSucceedEvent(templateId, taskId))
				{
					script->OnSubmitActiveTaskSucceed(task->GetOwner(), task, templateId, taskId);
				}
			}
		}
	}
}

void BaseTaskMgr::OnSubmitCycleTaskSucceed(UInt32 taskId)
{
	class OnSubmitCycleTaskVisitor : public BaseTaskVisitor
	{
	public:
		OnSubmitCycleTaskVisitor(UInt32 taskId) : m_TaskId(taskId) { }

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (NULL != script && script->CheckSubmitCycleTaskSucceedEvent(m_TaskId))
				{
					script->OnSubmitCycleTaskSucceed(pTask->GetOwner(), pTask, m_TaskId);
				}
			}
			return true;
		}

	private:
		UInt32 m_TaskId;
	};

	OnSubmitCycleTaskVisitor visitor(taskId);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnTaskFailed(UInt32 id)
{
	class OnTaskFailedVisitor : public BaseTaskVisitor
	{
	public:
		OnTaskFailedVisitor(UInt32 taskId) :m_TaskId(taskId){}

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckTaskFailedEvent(m_TaskId))
				{
					script->OnTaskFailed(pTask->GetOwner(), m_TaskId);
				}
			}
			return true;
		}

	private:
		UInt32 m_TaskId;
	};

	OnTaskFailedVisitor visitor(id);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnDied()
{
	class OnDiedVisitor : public BaseTaskVisitor
	{
	public:
		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckDiedEvent())
				{
					script->OnOwnerDied(pTask->GetOwner());
				}
			}
			return true;
		}
	};

	OnDiedVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnDailyTaskSubmitted(UInt32 taskId, UInt8 type)
{
	class OnDailyTaskSubmittedVisitor : public BaseTaskVisitor
	{
	public:
		OnDailyTaskSubmittedVisitor(UInt32 taskId, UInt8 type) :m_DailyTaskId(taskId), m_DailyTaskType(type){}

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckDailyTaskSubmittedEvent(m_DailyTaskId))
				{
					script->OnDailyTaskSubmitted(pTask->GetOwner(), pTask, m_DailyTaskType);
				}
			}
			return true;
		}

	private:
		UInt32	m_DailyTaskId;
		UInt8	m_DailyTaskType;
	};

	OnDailyTaskSubmittedVisitor visitor(taskId, type);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnPVP(PkType type, PkRaceResult status)
{
	class OnPVPVisitor :public BaseTaskVisitor
	{
	public:
		OnPVPVisitor(PkType type, PkRaceResult status) :m_Type(type), m_Status(status){}

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckPVPEvent(m_Type, m_Status))
				{
					script->OnPVP(pTask->GetOwner(), pTask, m_Type, m_Status);
				}
			}
			return true;
		}

	private:
		PkType m_Type;
		PkRaceResult m_Status;
	};

	OnPVPVisitor visitor(type, status);
	//VisitTasks(visitor);

	PVPEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckPVPEvent(type, status))
				{
					script->OnPVP(task->GetOwner(), task, type, status);
				}
			}
		}
	}
}


void BaseTaskMgr::OnChangeOccu()
{
	class OnChangeOccuVisitor : public BaseTaskVisitor
	{
	public:
		OnChangeOccuVisitor(){ }

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckChangeOccuEvent())
				{
					script->OnChangeOccu(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	};
	OnChangeOccuVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnAddMagic()
{
	class OnAddMagicVisitor : public BaseTaskVisitor
	{
	public:
		OnAddMagicVisitor(){}

		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckAddMagicEvent())
				{
					script->OnAddMagic(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	};
	OnAddMagicVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnAddMagicCard()
{
	class OnAddMagicCardVisitor : public BaseTaskVisitor
	{
	public:
		OnAddMagicCardVisitor() {}
		bool operator()(BaseTask *pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckAddMagicCardEvent())
				{
					script->OnAddMagicCard(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	};
	OnAddMagicCardVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnOpenMagicJar(JarType type, UInt32 combo)
{
	class OnOpenMagicJarVisitor : public BaseTaskVisitor
	{
	public:
		OnOpenMagicJarVisitor(JarType type, UInt32 combo) :m_JarType(type), m_Combo(combo) { }

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckOpenMagicJarEvent(m_JarType, m_Combo))
				{
					script->OnOpenMagicJar(pTask->GetOwner(), pTask, m_JarType, m_Combo);
				}
			}
			return true;
		}
	private:
		JarType m_JarType;
		UInt32 m_Combo;
	};
	OnOpenMagicJarVisitor visitor(type, combo);
	//VisitTasks(visitor);

	OpenMagicJarEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto pTask = FindTask(scriptID);
		if (script != nullptr && pTask != nullptr)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckOpenMagicJarEvent(type, combo))
				{
					script->OnOpenMagicJar(pTask->GetOwner(), pTask, type, combo);
				}
			}
		}
	}
}

void BaseTaskMgr::OnFriendGive()
{
	class OnFriendGiveVisitor : public BaseTaskVisitor
	{
	public:
		OnFriendGiveVisitor(){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckFriendGiveEvent())
				{
					script->OnFriendGive(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	};
	OnFriendGiveVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnWarpStoneUpLevel(WarpStoneType type, UInt32 level)
{
	class OnWarpStoneUpLevelVisitor : public BaseTaskVisitor
	{
	public:
		OnWarpStoneUpLevelVisitor(WarpStoneType type, UInt32 level) :m_Type(type), m_Level(level) { }

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckWarpStoneUpLevelEvent(m_Type, m_Level))
				{
					script->OnWarpStoneUpLevel(pTask->GetOwner(), pTask, m_Type, m_Level);
				}
			}
			return true;
		}

	private:
		WarpStoneType m_Type;
		UInt32 m_Level;
	};
	OnWarpStoneUpLevelVisitor visitor(type, level);
	//VisitTasks(visitor);

	WarpStoneUpLevelEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckWarpStoneUpLevelEvent(type, level))
				{
					script->OnWarpStoneUpLevel(task->GetOwner(), task, type, level);
				}
			}
		}
	}
}

void BaseTaskMgr::OnWarpStoneAddEnergy(UInt32 energy)
{
	class OnWarpStoneAddEnergyVisitor : public BaseTaskVisitor
	{
	public:
		OnWarpStoneAddEnergyVisitor(UInt32 energy) : m_Energy(energy) { }

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckWarpStoneAddEnergyEvent())
				{
					script->OnWarpStoneAddEnergy(pTask->GetOwner(), pTask, m_Energy);
				}
			}
			return true;
		}

	private:
		UInt32 m_Energy;
	};
	OnWarpStoneAddEnergyVisitor visitor(energy);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnRefreshShop()
{
	class OnRefreshShopVisitor : public BaseTaskVisitor
	{
	public:
		OnRefreshShopVisitor() {}
		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckRefreshShopEvent())
				{
					script->OnRefreshShop(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	};
	OnRefreshShopVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnShopBuy(UInt32 shopId, UInt32 shopItemId, UInt32 num)
{
	class OnShopBuyVisitor :public BaseTaskVisitor
	{
	public:
		OnShopBuyVisitor(UInt32 shopId, UInt32 shopItemId, UInt32 num) :m_ShopId(shopId), m_ShopItemId(shopItemId), m_Num(num) {}
		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckShopBuyEvent(m_ShopId, m_ShopItemId))
				{
					script->OnShopBuy(pTask->GetOwner(), pTask, m_ShopId, m_ShopItemId, m_Num);
				}
			}
			return true;
		}

	private:
		UInt32 m_ShopId;
		UInt32 m_ShopItemId;
		UInt32 m_Num;
	};

	OnShopBuyVisitor visitor(shopId, shopItemId, num);
	//VisitTasks(visitor);

	ShopBuyEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckShopBuyEvent(shopId, shopItemId))
				{
					script->OnShopBuy(task->GetOwner(), task, shopId, shopItemId, num);
				}
			}
		}
	}
}

void BaseTaskMgr::OnRemoveFatigue(UInt16 usedFatigue)
{
	class OnRemoveFatigueVisitor : public BaseTaskVisitor
	{
	public:
		OnRemoveFatigueVisitor(UInt16 usedFatigue) :m_UsedFatigue(usedFatigue) { }

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				Player* player = pTask->GetOwner();
				if (script != NULL && script->CheckRemoveFatigueEvent(player->GetUseFatigue()))
				{
					script->OnRemoveFatigue(player, pTask, m_UsedFatigue);
				}
			}
			return true;
		}
	private:
		UInt16 m_UsedFatigue;
	};
	OnRemoveFatigueVisitor visitor(usedFatigue);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnBeginHell(UInt32 dungeonId)
{
	class OnBeginHellVisitor : public BaseTaskVisitor
	{
	public:
		OnBeginHellVisitor(UInt32 dungeonId) { }

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckBeginHellEvent(m_DungeonId))
				{
					script->OnBeginHell(pTask->GetOwner(), pTask, m_DungeonId);
				}
			}
			return true;
		}

	private:
		UInt32 m_DungeonId;
	};
	OnBeginHellVisitor visitor(dungeonId);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnClearHell(UInt32 dungeonId, UInt32 usedTime, LevelScore score)
{
	class OnClearHellVisitor : public BaseTaskVisitor
	{
	public:
		OnClearHellVisitor(UInt32 dungeonId, UInt32 usedTime, LevelScore score) 
			:m_DungeonId(dungeonId), m_UsedTime(usedTime), m_LevelScore(score){}
		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckClearHellEvent(m_DungeonId, m_UsedTime, m_LevelScore))
				{
					script->OnClearHell(pTask->GetOwner(), pTask, m_DungeonId, m_UsedTime);
				}
			}
			return true;
		}
	private:
		UInt32 m_DungeonId;
		UInt32 m_UsedTime;
		LevelScore m_LevelScore;
	};
	OnClearHellVisitor visitor(dungeonId, usedTime, score);
	//VisitTasks(visitor);

	ClearDungeonEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckClearHellEvent(dungeonId, usedTime, score))
				{
					script->OnClearHell(task->GetOwner(), task, dungeonId, usedTime);
				}
			}
		}
	}
}

void BaseTaskMgr::OnDeathTowerBegin(UInt32 towerId)
{
	class OnDeathTowerBeginVisitor : public BaseTaskVisitor
	{
	public:
		OnDeathTowerBeginVisitor(UInt32 towerId) :m_TowerId(towerId){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckDeathTowerBeginEvent(m_TowerId))
				{
					script->OnDeathTowerBegin(pTask->GetOwner(), pTask, m_TowerId);
				}
			}
			return true;
		}

	private:
		UInt32 m_TowerId;
	};
	OnDeathTowerBeginVisitor visitor(towerId);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnDeathTowerWipeoutBegin()
{
	class OnDeathTowerWipeoutBeginVisitor : public BaseTaskVisitor
	{
	public:
		OnDeathTowerWipeoutBeginVisitor() {}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckDeathTowerWipeoutBeginEvent())
				{
					script->OnDeathTowerWipeoutBegin(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	};
	OnDeathTowerWipeoutBeginVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnDeathTower(UInt32 towerId, UInt32 layer, UInt32 usedTime)
{
	class OnDeathTowerVisitor : public BaseTaskVisitor
	{
	public:
		OnDeathTowerVisitor(UInt32 towerId, UInt32 layer, UInt32 usedTime) :m_TowerId(towerId), m_Layer(layer), m_UsedTime(usedTime){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckDeathTowerEvent(m_TowerId, m_Layer, m_UsedTime))
				{
					script->OnDeathTower(pTask->GetOwner(), pTask, m_TowerId, m_Layer, m_UsedTime);
				}
			}
			return true;
		}

	private:
		UInt32 m_TowerId;
		UInt32 m_Layer;
		UInt32 m_UsedTime;
	};
	OnDeathTowerVisitor visitor(towerId, layer, usedTime);
	//VisitTasks(visitor);

	DeathTowerEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckDeathTowerEvent(towerId, layer, usedTime))
				{
					script->OnDeathTower(task->GetOwner(), task, towerId, layer, usedTime);
				}
			}
		}
	}
}

void BaseTaskMgr::OnAwaken()
{
	class OnAwakenVisitor : public BaseTaskVisitor
	{
	public:
		OnAwakenVisitor(){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckAwakenEvent())
				{
					script->OnAwaken(pTask->GetOwner(), pTask, (AwakenStatus)pTask->GetOwner()->GetAwaken());
				}
			}
			return true;
		}
	};
	OnAwakenVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnReviveCoin(bool isSelf)
{
	class OnReviveCoinVisitor : public BaseTaskVisitor
	{
	public:
		OnReviveCoinVisitor(bool isSelf) :m_IsSelf(isSelf) {}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckReviveCoinEvent())
				{
					script->OnReviveCoin(pTask->GetOwner(), pTask, m_IsSelf);
				}
			}
			return true;
		}

	private:
		bool m_IsSelf;
	};

	OnReviveCoinVisitor visitor(isSelf);
	VisitTasks(visitor);
}

/**
*@brief 拍卖行 寄卖
*/
void BaseTaskMgr::OnAddAuction()
{
	class OnAddAuctionVisitor : public BaseTaskVisitor
	{
	public:
		OnAddAuctionVisitor(){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckAddAuctionEvent())
				{
					script->OnAddAuction(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	};

	OnAddAuctionVisitor visitor;
	VisitTasks(visitor);
}

/**
*@brief 拍卖行 卖出
*/
void BaseTaskMgr::OnSellAuction(UInt32 money)
{
	class OnSellAuctionVisitor : public BaseTaskVisitor
	{
	public:
		OnSellAuctionVisitor(UInt32 money) : m_Money(money){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckSellAuctionEvent())
				{
					script->OnSellAuction(pTask->GetOwner(), pTask, m_Money);
				}
			}
			return true;
		}

	public:
		UInt32 m_Money;
	};

	OnSellAuctionVisitor visitor(money);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnBuyAuction(UInt32 money)
{
	class OnBuyAuctionVisitor : public BaseTaskVisitor
	{
	public:
		OnBuyAuctionVisitor(UInt32 money) : m_Money(money) {}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckBuyAuctionEvent())
				{
					script->OnBuyAuction(pTask->GetOwner(), pTask, m_Money);
				}
			}
			return true;
		}

	public:
		UInt32 m_Money;
	};

	OnBuyAuctionVisitor visitor(money);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnFeedPet(PetFeedType type)
{
	class OnFeedPetVisitor : public BaseTaskVisitor
	{
	public:
		OnFeedPetVisitor(PetFeedType type):m_Type(type) { }

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckFeedPetEvent(m_Type))
				{
					script->OnFeedPet(pTask->GetOwner(), pTask, m_Type);
				}
			}
			return true;
		}

	private:
		PetFeedType m_Type;
	};

	OnFeedPetVisitor visitor(type);
	//VisitTasks(visitor);

	FeedPetEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto pTask = FindTask(scriptID);
		if (script != nullptr && pTask != nullptr)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckFeedPetEvent(type))
				{
					script->OnFeedPet(pTask->GetOwner(), pTask, type);
				}
			}
		}
	}
}

void BaseTaskMgr::OnAddPet(PetType petType, PetQuality petQuality)
{
	class OnAddPetVisitor : public BaseTaskVisitor
	{
	public:
		OnAddPetVisitor(PetType petType, PetQuality petQuality) :m_PetType(petType), m_PetQuality(petQuality){}

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckAddPetEvent(m_PetType, m_PetQuality))
				{
					script->OnAddPet(task->GetOwner(), task, m_PetType, m_PetQuality);
				}
			}
			return true;
		}

	private:
		PetType m_PetType;
		PetQuality m_PetQuality;
	};

	OnAddPetVisitor visitor(petType, petQuality);
	//VisitTasks(visitor);


	AddPetEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckAddPetEvent(petType, petQuality))
				{
					script->OnAddPet(task->GetOwner(), task, petType, petQuality);
				}
			}
		}
	}
}

void BaseTaskMgr::OnPetUpLevel(PetType petType, PetQuality petQuality, UInt16 level)
{
	class OnPetUpLevelVisitor : public BaseTaskVisitor
	{
	public:
		OnPetUpLevelVisitor(PetType petType, PetQuality petQuality, UInt16 level)
			: m_PetType(petType), m_PetQuality(petQuality), m_Level(level) { }

		bool operator()(BaseTask* task)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckPetUpLevelEvent(m_PetType, m_PetQuality, m_Level))
				{
					script->OnPetUpLevel(task->GetOwner(), task, m_PetType, m_PetQuality, m_Level);
				}
			}
			return true;
		}

	private:
		PetType m_PetType;
		PetQuality m_PetQuality;
		UInt16 m_Level;
	};

	OnPetUpLevelVisitor visitor(petType, petQuality, level);
	//VisitTasks(visitor);


	PetUpLevelEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckPetUpLevelEvent(petType, petQuality, level))
				{
					script->OnPetUpLevel(task->GetOwner(), task, petType, petQuality, level);
				}
			}
		}
	}
}

/**
*@brief 增加社会关系
*/
void BaseTaskMgr::OnAddRelation(RelationType type)
{
	class OnAddMasterVisitor : public BaseTaskVisitor
	{
	public:
		OnAddMasterVisitor(RelationType type) :m_Type(type){ }

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckAddRelationEvent(m_Type))
				{
					script->OnAddRelation(pTask->GetOwner(), pTask, m_Type);
				}
			}
			return true;
		}

	private:
		RelationType m_Type;
	};

	OnAddMasterVisitor visitor(type);
	//VisitTasks(visitor);

	RelationEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto pTask = FindTask(scriptID);
		if (script != nullptr && pTask != nullptr)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckAddRelationEvent(type))
				{
					script->OnAddRelation(pTask->GetOwner(), pTask, type);
				}
			}
		}
	}
}

void BaseTaskMgr::OnSendHorn()
{
	class OnSendHornVisitor : public BaseTaskVisitor
	{
	public:
		OnSendHornVisitor(){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckSendHornEvent())
				{
					script->OnSendHorn(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	};

	OnSendHornVisitor visitor;
	VisitTasks(visitor);

}

void BaseTaskMgr::OnJoinGuild()
{
	class OnJoinGuildVisitor : public BaseTaskVisitor
	{
	public:
		OnJoinGuildVisitor() { }

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckJoinGuildEvent())
				{
					script->OnJoinGuild(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	};

	OnJoinGuildVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnIncGuildBattleScore(UInt32 score)
{
	class OnIncGuildBattleScoreVisitor : public BaseTaskVisitor
	{
	public:
		OnIncGuildBattleScoreVisitor( UInt32 score ):m_Score(score) { }

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckIncGuildBattleScoreEvent())
				{
					script->OnIncGuildBattleScore(pTask->GetOwner(), pTask, m_Score);
				}
			}
			return true;
		}

	private:
		UInt32 m_Score;
	};

	OnIncGuildBattleScoreVisitor visitor(score);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnGuildOccupyTerritory(UInt8 terrId)
{
	class OnGuildOccupyTerritoryVisitor : public BaseTaskVisitor
	{
	public:
		OnGuildOccupyTerritoryVisitor(UInt8 terrId) : m_TerritoryId(terrId) { }

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckGuildOccupyTerritoryEvent((UInt32)m_TerritoryId))
				{
					script->OnGuildOccupyTerritory(pTask->GetOwner(), pTask, (UInt32)m_TerritoryId);
				}
			}
			return true;
		}

	private:
		UInt8 m_TerritoryId;
	};

	OnGuildOccupyTerritoryVisitor visitor(terrId);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnWinWudaodahuiPkEnd(UInt32 winNum)
{
	class OnWinWudaodahuiPkEndVisitor : public BaseTaskVisitor
	{
	public:
		OnWinWudaodahuiPkEndVisitor(UInt32 winNum) :m_WinNum(winNum) {}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckWinWudaodahuiPkEndEvent(m_WinNum))
				{
					script->OnWinWudaodahuiPkEnd(pTask->GetOwner(), pTask, m_WinNum);
				}
			}
			return true;
		}

	private:
		UInt32 m_WinNum;
	};

	OnWinWudaodahuiPkEndVisitor visitor(winNum);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnEnlargePackage(UInt32 size)
{
	class OnEnlargePackageVisitor : public BaseTaskVisitor
	{
	public:
		OnEnlargePackageVisitor(UInt32 size):m_Size(size) {}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckEnlargePackageEvent())
				{
					script->OnEnlargePackage(pTask->GetOwner(), pTask, m_Size);
				}
			}
			return true;
		}

	private:
		UInt32 m_Size;
	};

	OnEnlargePackageVisitor visitor(size);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnEnlargeStorage(UInt32 size)
{
	class OnEnlargeStorageVisitor : public BaseTaskVisitor
	{
	public:
		OnEnlargeStorageVisitor(UInt32 size):m_Size(size) {}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckEnlargeStorageEvent())
				{
					script->OnEnlargeStorage(pTask->GetOwner(), pTask, m_Size);
				}
			}
			return true;
		}
	private:
		UInt32 m_Size;
	};

	OnEnlargeStorageVisitor visitor(size);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnIncIntimacy(ObjID_t targetId, UInt32 incIntimacy, UInt32 finalIntimacy)
{
	class OnIncIntimacyVisitor : public BaseTaskVisitor
	{
	public:
		OnIncIntimacyVisitor(ObjID_t targetId, UInt32 incIntimacy, UInt32 finalIntimacy) 
			: m_targetId(targetId), m_incIntimacy(incIntimacy), m_finalIntimacy(finalIntimacy){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckIncIntimacyEvent())
				{
					script->OnIncIntimacy(pTask->GetOwner(), m_targetId, pTask, m_incIntimacy, m_finalIntimacy);
				}
			}
			return true;
		}
	private:
		ObjID_t m_targetId;
		UInt32 m_incIntimacy;
		UInt32 m_finalIntimacy;
	};

	OnIncIntimacyVisitor visitor(targetId, incIntimacy, finalIntimacy);
	VisitTasks(visitor);
}

void  BaseTaskMgr::OnIncDailyTaskScore(UInt32 totalScore)
{
	class OnIncDailyTaskScoreVisitor : public BaseTaskVisitor
	{
	public:
		OnIncDailyTaskScoreVisitor(UInt32 totalScore)
			: m_totalScore(totalScore){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckIncDailyTaskScoreEvent())
				{
					script->OnIncDailyTaskScore(pTask->GetOwner(), pTask, m_totalScore);
				}
			}
			return true;
		}
	private:
		UInt32 m_totalScore;
	};

	OnIncDailyTaskScoreVisitor visitor(totalScore);
	VisitTasks(visitor);
}

void BaseTaskMgr::OnTeamClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, std::vector<ObjID_t>& teammateIds)
{
	class OnClearDungeonVisitor : public BaseTaskVisitor
	{
	public:
		OnClearDungeonVisitor(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, std::vector<ObjID_t>& teammateIds)
			:m_DungeonId(dungeonId), m_Beated(beated), m_Areas(areas), m_Score(score), m_UsedTime(usedTime), m_ReviveCount(reviveCount), m_TeammateIds(teammateIds){}

		bool operator()(BaseTask* task)
		{
			if (TASK_UNFINISH == task->GetStatus())
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckTeamClearDungeonEvent(m_DungeonId, m_Score, m_UsedTime, m_ReviveCount))
				{
					//script->OnCopyScenePassed(task->GetOwner(), m_Id);
					script->OnTeamClearDungeon(task->GetOwner(), task, m_DungeonId, m_Beated, m_Areas, m_TeammateIds);
				}
			}
			return true;
		}

	protected:
	private:
		UInt32 m_DungeonId;
		UInt16 m_Beated;
		UInt16 m_Areas;
		LevelScore m_Score;
		UInt32 m_UsedTime;
		UInt32 m_ReviveCount;
		std::vector<ObjID_t> m_TeammateIds;
	};

	OnClearDungeonVisitor visitor(dungeonId, beated, areas, score, usedTime, reviveCount, teammateIds);
	//VisitTasks(visitor);


	ClearDungeonEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (TASK_UNFINISH == task->GetStatus())
			{
				if (script->CheckTeamClearDungeonEvent(dungeonId, score, usedTime, reviveCount))
				{
					script->OnTeamClearDungeon(task->GetOwner(), task, dungeonId, beated, areas, teammateIds);
				}
			}
		}
	}
}

void BaseTaskMgr::OnTeamClearActivityDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, std::vector<ObjID_t>& teammateIds)
{
	class OnClearActivityDungeonVisitor : public BaseTaskVisitor
	{
	public:
		OnClearActivityDungeonVisitor(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, std::vector<ObjID_t>& teammateIds)
			:m_DungeonId(dungeonId), m_Beated(beated), m_Areas(areas), m_Score(score), m_UsedTime(usedTime), m_ReviveCount(reviveCount), m_TeammateIds(teammateIds){}

		bool operator()(BaseTask* task)
		{
			if (TASK_UNFINISH == task->GetStatus())
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckTeamClearActivityDungeonEvent(m_DungeonId, m_Score, m_UsedTime, m_ReviveCount))
				{
					//script->OnCopyScenePassed(task->GetOwner(), m_Id);
					script->OnTeamClearActivityDungeon(task->GetOwner(), task, m_DungeonId, m_Beated, m_Areas, m_TeammateIds);
				}
			}
			return true;
		}

	protected:
	private:
		UInt32 m_DungeonId;
		UInt16 m_Beated;
		UInt16 m_Areas;
		LevelScore m_Score;
		UInt32 m_UsedTime;
		UInt32 m_ReviveCount;
		std::vector<ObjID_t> m_TeammateIds;
	};

	OnClearActivityDungeonVisitor visitor(dungeonId, beated, areas, score, usedTime, reviveCount, teammateIds);
	//VisitTasks(visitor);


	ClearDungeonEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (TASK_UNFINISH == task->GetStatus())
			{
				if (script->CheckTeamClearActivityDungeonEvent(dungeonId, score, usedTime, reviveCount))
				{
					//script->OnCopyScenePassed(task->GetOwner(), m_Id);
					script->OnTeamClearActivityDungeon(task->GetOwner(), task, dungeonId, beated, areas, teammateIds);
				}
			}
		}
	}
}

void BaseTaskMgr::OnTeamClearHell(UInt32 dungeonId, UInt32 usedTime, LevelScore score, std::vector<ObjID_t>& teammateIds)
{
	class OnClearHellVisitor : public BaseTaskVisitor
	{
	public:
		OnClearHellVisitor(UInt32 dungeonId, UInt32 usedTime, LevelScore score, std::vector<ObjID_t>& teammateIds)
			:m_DungeonId(dungeonId), m_UsedTime(usedTime), m_LevelScore(score), m_TeammateIds(teammateIds){}
		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckTeamClearHellEvent(m_DungeonId, m_UsedTime, m_LevelScore))
				{
					script->OnTeamClearHell(pTask->GetOwner(), pTask, m_DungeonId, m_UsedTime, m_TeammateIds);
				}
			}
			return true;
		}
	private:
		UInt32 m_DungeonId;
		UInt32 m_UsedTime;
		LevelScore m_LevelScore;
		std::vector<ObjID_t> m_TeammateIds;
	};
	OnClearHellVisitor visitor(dungeonId, usedTime, score, teammateIds);
	//VisitTasks(visitor);


	ClearDungeonEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (task->GetStatus() == TASK_UNFINISH)
			{
				if (script->CheckTeamClearHellEvent(dungeonId, usedTime, score))
				{
					script->OnTeamClearHell(task->GetOwner(), task, dungeonId, usedTime, teammateIds);
				}
			}
		}
	}
}

/*
*@brief 收徒事件
*/
void BaseTaskMgr::OnApprentDisciple()
{
	class OnApprentDiscipleVisitor : public BaseTaskVisitor
	{
	public:
		OnApprentDiscipleVisitor(){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckApprentDiscipleEvent())
				{
					script->OnApprentDisciple(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	private:
		UInt32 m_totalScore;
	};

	OnApprentDiscipleVisitor visitor;
	VisitTasks(visitor);
}

/*
*@brief 徒弟出师事件
*/
void BaseTaskMgr::OnDiscipleFinsch()
{
	class OnDiscipleFinschVisitor : public BaseTaskVisitor
	{
	public:
		OnDiscipleFinschVisitor(){}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckDiscipleFinschEvent())
				{
					script->OnDiscipleFinsch(pTask->GetOwner(), pTask);
				}
			}
			return true;
		}
	private:
		UInt32 m_totalScore;
	};

	OnDiscipleFinschVisitor visitor;
	VisitTasks(visitor);
}

void BaseTaskMgr::OnGuildEmblemUp(UInt32 lvl)
{
	class OnGuildEmblemVisitor : public BaseTaskVisitor
	{
	public:
		OnGuildEmblemVisitor(UInt32 lvl) : m_EmblemLvl(lvl) {}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckGuildEmblemEvent())
				{
					script->OnGuildEmblemUp(pTask->GetOwner(), pTask, m_EmblemLvl);
				}
			}
			return true;
		}

	private:
		UInt32 m_EmblemLvl;
	};

	OnGuildEmblemVisitor visitor(lvl);
	VisitTasks(visitor);
}

/**
*@breif 通关地下城 关注分数
*/
void BaseTaskMgr::OnClearDungeonCareScore(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
{
	class OnClearDungeonVisitor : public BaseTaskVisitor
	{
	public:
		OnClearDungeonVisitor(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount)
			:m_DungeonId(dungeonId), m_Beated(beated), m_Areas(areas), m_Score(score), m_UsedTime(usedTime), m_ReviveCount(reviveCount) { }

		bool operator()(BaseTask* task)
		{
			if (TASK_UNFINISH == task->GetStatus())
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(task->GetDataId());
				if (script != NULL && script->CheckCopyScenePassedCareScoreEvent(m_DungeonId, m_Score, m_UsedTime, m_ReviveCount))
				{
					//script->OnCopyScenePassed(task->GetOwner(), m_Id);
					script->OnClearDungeonCareScore(task->GetOwner(), task, m_DungeonId, m_Score);
				}
			}
			return true;
		}

	protected:
	private:
		UInt32 m_DungeonId;
		UInt16 m_Beated;
		UInt16 m_Areas;
		LevelScore m_Score;
		UInt32 m_UsedTime;
		UInt32 m_ReviveCount;
	};

	OnClearDungeonVisitor visitor(dungeonId, beated, areas, score, usedTime, reviveCount);
	//VisitTasks(visitor);


	ClearDungeonEvent event;
	auto& scriptSet = ScriptMgr::Instance()->GetScriptIDSet(event.GetEventKey());
	for (auto scriptID : scriptSet)
	{
		auto script = ScriptMgr::Instance()->FindTaskScript(scriptID);
		auto task = FindTask(scriptID);
		if (script != nullptr && task != nullptr)
		{
			if (TASK_UNFINISH == task->GetStatus())
			{
				if (script->CheckCopyScenePassedCareScoreEvent(dungeonId, score, usedTime, reviveCount))
				{
					script->OnClearDungeonCareScore(task->GetOwner(), task, dungeonId, score);
				}
			}
		}
	}
}

void BaseTaskMgr::OnClearTeamCopy(UInt32 teamType, UInt32 grade, UInt32 costTime)
{
	class OnTeamCopyVisitor : public BaseTaskVisitor
	{
	public:
		OnTeamCopyVisitor(UInt32 _teamType, UInt32 _grade, UInt32 _costTime) : 
			m_TeamType(_teamType), m_Grade(_grade), m_CostTime(_costTime) {}

		bool operator()(BaseTask* pTask)
		{
			if (pTask->GetStatus() == TASK_UNFINISH)
			{
				TaskScript* script = ScriptMgr::Instance()->FindTaskScript(pTask->GetDataId());
				if (script != NULL && script->CheckClearTeamCopyEvent())
				{
					script->OnClearTeamCopy(pTask->GetOwner(), pTask, m_TeamType, m_Grade, m_CostTime);
				}
			}
			return true;
		}

	private:
		UInt32 m_TeamType;
		UInt32 m_Grade;
		UInt32 m_CostTime;
	};

	OnTeamCopyVisitor visitor(teamType, grade, costTime);
	VisitTasks(visitor);
}

void BaseTaskMgr::Input(Avalon::NetInputStream& stream)
{
	UInt32 taskId = 0;
	stream & taskId;
	while (taskId != 0)
	{
		Task* task = new Task(GetOwner(), taskId);
		stream & (*task);

		if (!task->SetupBase() || !m_Tasks.insert(std::make_pair(taskId, task)).second)
		{
			CL_SAFE_DELETE(task);
			throw Avalon::NetException("decode tasks failed!");
			return;
		}
		stream & taskId;
	}
}

void BaseTaskMgr::Output(Avalon::NetOutputStream& stream)
{
	for (TaskMap::iterator iter = m_Tasks.begin(); iter != m_Tasks.end(); ++iter)
	{
		stream & iter->first & *(iter->second);
	}
	stream & UInt32(0);
}

void BaseTaskMgr::DecodeString(std::istringstream& is)
{
	char split;
	UInt32 taskId = 0;
	while ((is >> taskId) && taskId != 0)
	{
		is >> split; //','

		Task* task = new Task(GetOwner(), taskId);
		//task->DecodeString(is);

		if (!task->SetupBase() || !m_Tasks.insert(std::make_pair(taskId, task)).second)
		{
			GameErrorLogStream("Player") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") has invalid task "
				<< task->GetDataId() << ".";
			CL_SAFE_DELETE(task);
		}
	}
	is >> split;
}

void BaseTaskMgr::EncodeString(std::ostringstream& os)
{
	for (TaskMap::iterator iter = m_Tasks.begin(); iter != m_Tasks.end(); ++iter)
	{
		os << iter->first << ',';
		//iter->second->EncodeString(os);
	}
	os << UInt32(0) << ';';
}

void BaseTaskMgr::SaveToDB()
{
	class SaveTaskVisitor : public BaseTaskVisitor
	{
	public:
		SaveTaskVisitor(Player* pPlayer) : m_pPlayer(pPlayer){}
		~SaveTaskVisitor(){}

		bool operator()(BaseTask *pTask)
		{
			CLUpdateCommand	*pCmd = CLRecordClient::Instance()->PrepareUpdateCommand(m_pPlayer->GetSrcZoneId(), "t_task", pTask->GetID(), true);
			pTask->GetDBData(pCmd, true);
			CLRecordClient::Instance()->SendCommand(pCmd, NULL, m_pPlayer->GetID());
			return true;
		}

	private:
		Player	*m_pPlayer;
	};

	SaveTaskVisitor visitor(GetOwner());
	VisitTasks(visitor);
}

