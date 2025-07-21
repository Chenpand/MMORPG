#include "ScriptMgr.h"
#include <AvalonFile.h>

#include <CLSysNotify.h>

#include "SysInterface.h"
#include "SSApplication.h"
#include "SSRouter.h"
#include "Item.h"
#include "Player.h"
#include "PlayerMgr.h"
#include "Task.h"
#include "Scene.h"
#include "Team.h"

#include "TaskScript.h"
#include "ItemScript.h"
#include "SceneScript.h"
#include "AIScript.h"
#include "PlayerScript.h"
#include "FinalEquipScript.h"
#include "EscortScript.h"
#include "EquipCalcScript.h"
#include "ActivityScript.h"
#include "ActiveTask.h"
#include "ScriptUtil.h"

ScriptConfig ScriptMgr::s_scriptConfigs[] = {
	{ TASK_SCRIPT_PATH,			"task_",		NULL,			true,			false },
	{ ITEM_SCRIPT_PATH,			"item_",		NULL,			true,			false },
	{ ACTIVITY_SCRIPT_PATH,		"activity_", NULL, true, false },
	{ COUNTER_SCRIPT_PATH, NULL, "Counter", false, true },
	{ EQUIPCALC_SCRIPT_PATH, NULL, "EquipCalc", false, false },
	{ PLAYER_SCRIPT_PATH, NULL, "Player", false, false },
// 	{ SCENE_SCRIPT_PATH,		"scene_",		NULL,			true,			false },
// 	{ AI_SCRIPT_PATH,			"AI_",			NULL,			true,			false },
// 	{ FINALEQUIP_SCRIPT_PATH, NULL, "FinalEquip", false, false },
// 	{ AI_BIND_SCRIPT_PATH, 		NULL,			"AIBind",		false,			false }
};

class SceneScriptErrorHandler : public CLScriptErrorHandler
{
public:
	void OnError(const char* str)
	{
		SYSLOG_ERROR(str);

		if(SSApplication::Instance()->IsDebugMode())
		{
			class NotifyScriptErrorVisitor : public PlayerVisitor
			{
			public:
				NotifyScriptErrorVisitor(const char* error):m_ErrorMsg(error){}

				bool operator()(Player* player)
				{
					player->Notify(1, 8,	m_ErrorMsg);
					return true;
				}
			private:
				const char* m_ErrorMsg;
			};
			NotifyScriptErrorVisitor visitor(str);
			PlayerMgr::Instance()->VisitNormalPlayers(visitor);
		}
	}
};

std::set<UInt32> ScriptMgr::m_NullSet;

void ScriptMgr::RegisterScriptEvent(TaskScriptEventBase& event, Avalon::ScriptFile& script)
{
	m_EventKeyToScriptMap[event.GetEventKey()].insert(script.GetId());
}

const std::set<UInt32>& ScriptMgr::GetScriptIDSet(UInt64 key)
{
	if (m_EventKeyToScriptMap.count(key) == 0)
	{
		return m_NullSet;
	}
	return m_EventKeyToScriptMap[key];
}

ScriptMgr::ScriptMgr()
:CLScriptMgr(SCENE_SCRIPT_NUM)
{
#ifndef _NEW_SCRIPT_MGR_
	m_pState = new Avalon::ScriptState();
	m_pPlayerScript	= NULL;
	m_pTerritoryScript = NULL;
	m_pFinalEquipScript = NULL;
	m_pEscortScript = NULL;
	m_pActivityGiftbagScript = NULL;
	m_pEquipCalcScript = NULL;
#endif
}

ScriptMgr::~ScriptMgr()
{
	Final();

	SysNotifyMgr::Destroy();

	CL_SAFE_DELETE(m_pState);
}

bool ScriptMgr::Init()
{
	if (!CLScriptMgr::Init())
	{
		return false;
	}

	DefClasses();

	Avalon::ScriptState::SetErrorHandler(new SceneScriptErrorHandler());

	std::string configpath = CONFIG_PATH;
	if(!SysNotifyMgr::Instance()->Load(m_pState, configpath + SYSNOTIFY_SCRIPT_PATH))
	{
		ErrorLogStream << "load sys notifies failed!" << LogStream::eos;
		return false;
	}

	bool ret = LoadScripts();
	if(SCRIPT_ERROR != 0) return false;

	return ret;
}

void ScriptMgr::Final()
{
#ifndef _NEW_SCRIPT_MGR_
	for(NpcScriptMap::iterator iter = m_NpcScripts.begin(); iter != m_NpcScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_NpcScripts.clear();

	for(TaskScriptMap::iterator iter = m_TaskScripts.begin(); iter != m_TaskScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_TaskScripts.clear();

	for (ActivityScriptMap::iterator iter = m_ActivityScripts.begin(); iter != m_ActivityScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_ActivityScripts.clear();

	for(ItemScriptMap::iterator iter = m_ItemScripts.begin(); iter != m_ItemScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_ItemScripts.clear();

	for(SceneScriptMap::iterator iter = m_SceneScripts.begin(); iter != m_SceneScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_SceneScripts.clear();

	for(AIScriptMap::iterator iter = m_AIScripts.begin(); iter != m_AIScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_AIScripts.clear();

	for(VehicleAIScriptMap::iterator iter = m_VehicleAIScripts.begin(); iter != m_VehicleAIScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_VehicleAIScripts.clear();

	CL_SAFE_DELETE(m_pPlayerScript);
	CL_SAFE_DELETE(m_pTerritoryScript);
	CL_SAFE_DELETE(m_pFinalEquipScript);
	CL_SAFE_DELETE(m_pEscortScript);
	CL_SAFE_DELETE(m_pActivityGiftbagScript);

#endif
}

bool ScriptMgr::LoadScripts()
{
	std::string configpath = CONFIG_PATH;

	std::string packagePath = configpath + PACKAGE_PATH;
	m_pState->AddPackagePath(packagePath.c_str());
	Avalon::ScriptFile sandBoxScript(m_pState);
	std::string sandBoxPath = configpath + SANDBOX_SCRIPT_PATH;
	if(!sandBoxScript.Load(sandBoxPath.c_str()))
	{
		ErrorLogStream << "load sandbox script " << SANDBOX_SCRIPT_PATH << " failed!" << LogStream::eos;
		return false;
	}

#ifndef _NEW_SCRIPT_MGR_
	Avalon::Directory taskDir(configpath + TASK_SCRIPT_PATH);
	for(Avalon::Directory::iterator iter = taskDir.Begin();
		iter != taskDir.End(); ++iter)
	{
		std::string name = iter->GetName();
		if(name.length() < 10 || name[0] == '.') continue;

		std::string head = name.substr(0,5);
		std::string tail = name.substr(name.length() - 3);
		if(head.compare("task_") != 0 || tail.compare("lua") != 0)
		{
			ErrorLogStream << "unknow task script:" << name << "!" << LogStream::eos;
			continue;
		}

		UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(name.substr(5, name.length() - 9));
		if(id == 0) continue;
		if(m_TaskScripts.find(id) != m_TaskScripts.end())
		{
			ErrorLogStream << "repeat task script: " << name << "!" << LogStream::eos;
			continue;
		}

		std::string env = name.substr(0, name.length() - 4);
		m_pState->MakeEnv(env.c_str(), "SetSandBox");
		TaskScript* script = new TaskScript(id, m_pState);
		if(!script->Load(iter->GetFullPath().c_str(), env.c_str()))
		{
			CL_SAFE_DELETE(script);
			ErrorLogStream << "load task script " << name << " failed!" << LogStream::eos;
			return false;
		}

		if(!m_TaskScripts.insert(std::make_pair(id, script)).second)
		{
			CL_SAFE_DELETE(script);
			continue;
		}

		script->Init();

		const std::vector<CLZone>& triggerZones = script->GetTriggerZones();
		for(std::vector<CLZone>::const_iterator iter = triggerZones.begin();
			iter != triggerZones.end(); ++iter)
		{
			m_SceneTasks[iter->mapid].push_back(script);
		}
	}

	Avalon::Directory npcDir(configpath + NPC_SCRIPT_PATH);
	for(Avalon::Directory::iterator iter = npcDir.Begin();
		iter != npcDir.End(); ++iter)
	{
		std::string name = iter->GetName();
		if(name.length() < 9 || name[0] == '.') continue;

		std::string head = name.substr(0, 4);
		std::string tail = name.substr(name.length() - 3);
		if(head.compare("npc_") != 0 || tail.compare("lua") != 0)
		{
			ErrorLogStream << "unknow npc script: " << name << "!" << LogStream::eos;
			continue;
		}

		UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(name.substr(4, name.length() - 8));
		if(id == 0) continue;
		if(m_NpcScripts.find(id) != m_NpcScripts.end())
		{
			ErrorLogStream << "repeat npc script: " << name << "!" << LogStream::eos;
			continue;
		}

		std::string env = name.substr(0, name.length() - 4);
		m_pState->MakeEnv(env.c_str(), "SetSandBox");
		NpcScript* script = new NpcScript(id, m_pState);
		if(!script->Load(iter->GetFullPath().c_str(), env.c_str()))
		{
			CL_SAFE_DELETE(script);
			ErrorLogStream << "load npc script " << name << " failed!" << LogStream::eos;
			return false;
		}
		m_NpcScripts.insert(std::make_pair(id, script));
		script->Init();
	}

	Avalon::Directory itemDir(configpath + ITEM_SCRIPT_PATH);
	for(Avalon::Directory::iterator iter = itemDir.Begin();
		iter != itemDir.End(); ++iter)
	{
		std::string name = iter->GetName();
		if(name.length() < 10 || name[0] == '.') continue;

		std::string head = name.substr(0, 5);
		std::string tail = name.substr(name.length() - 3);
		if(head.compare("item_") != 0 || tail.compare("lua") != 0)
		{
			ErrorLogStream << "unknow item script: " << name << "!" << LogStream::eos;
			continue;
		}

		UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(name.substr(5, name.length() - 9));
		if(id == 0) continue;
		if(m_ItemScripts.find(id) != m_ItemScripts.end())
		{
			ErrorLogStream << "repeat item script: " << name << "!" << LogStream::eos;
			continue;
		}

		std::string env = name.substr(0, name.length() - 4);
		m_pState->MakeEnv(env.c_str(), "SetSandBox");
		ItemScript* script = new ItemScript(id, m_pState);
		if(!script->Load(iter->GetFullPath().c_str(), env.c_str()))
		{
			CL_SAFE_DELETE(script);
			ErrorLogStream << "load item script " << name << " failed!" << LogStream::eos;
			return false;
		}
		m_ItemScripts.insert(std::make_pair(id, script));
	}

	Avalon::Directory sceneDir(configpath + SCENE_SCRIPT_PATH);
	for(Avalon::Directory::iterator iter = sceneDir.Begin();
		iter != sceneDir.End(); ++iter)
	{
		std::string name = iter->GetName();
		if(name.length() < 11 || name[0] == '.') continue;

		std::string head = name.substr(0,6);
		std::string tail = name.substr(name.length() - 3);
		if(head.compare("scene_") != 0 || tail.compare("lua") != 0)
		{
			ErrorLogStream << "unknow scene script:" << name << "!" << LogStream::eos;
			continue;
		}

		UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(name.substr(6, name.length() - 10));
		if(id == 0) continue;
		if(m_SceneScripts.find(id) != m_SceneScripts.end())
		{
			ErrorLogStream << "repeat scene script: " << name << "!" << LogStream::eos;
			continue;
		}

		std::string env = name.substr(0, name.length() - 4);
		m_pState->MakeEnv(env.c_str(), "SetSandBox");
		SceneScript* script = new SceneScript(id, m_pState);
		if(!script->Load(iter->GetFullPath().c_str(), env.c_str()))
		{
			CL_SAFE_DELETE(script);
			ErrorLogStream << "load scene script " << name << " failed!" << LogStream::eos;
			return false;
		}

		if(!m_SceneScripts.insert(std::make_pair(id, script)).second)
		{
			CL_SAFE_DELETE(script);
			continue;
		}
	}

	Avalon::ScriptFile aiBindScript(m_pState);
	m_pState->MakeEnv("AIBind", "SetSandBox");
	std::string aiBindPath = configpath + AI_BIND_SCRIPT_PATH;
	if(!aiBindScript.Load(aiBindPath.c_str(), "AIBind"))
	{
		ErrorLogStream << "load aibind script failed!" << LogStream::eos;
		return false;
	}
	aiBindScript.Call<void>("Init", this);

	Avalon::Directory aiDir(configpath + AI_SCRIPT_PATH);
	for(Avalon::Directory::iterator iter = aiDir.Begin();
		iter != aiDir.End(); ++iter)
	{
		std::string name = iter->GetName();
		if(name.length() < 8 || name[0] == '.') continue;

		std::string head = name.substr(0,3);
		std::string tail = name.substr(name.length() - 3);
		if(head.compare("AI_") != 0 || tail.compare("lua") != 0)
		{
			ErrorLogStream << "unknow ai script:" << name << "!" << LogStream::eos;
			continue;
		}

		UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(name.substr(3, name.length() - 7));
		if(id == 0) continue;
		if(m_AIScripts.find(id) != m_AIScripts.end())
		{
			ErrorLogStream << "repeat ai script: " << name << "!" << LogStream::eos;
			continue;
		}

		std::string env = name.substr(0, name.length() - 4);
		m_pState->MakeEnv(env.c_str(), "SetSandBox");
		AIScript* script = new AIScript(id, m_pState);
		if(!script->Load(iter->GetFullPath().c_str(), env.c_str()))
		{
			CL_SAFE_DELETE(script);
			ErrorLogStream << "load ai script " << name << " failed!" << LogStream::eos;
			return false;
		}

		if(!m_AIScripts.insert(std::make_pair(id, script)).second)
		{
			CL_SAFE_DELETE(script);
			continue;
		}
	}

	Avalon::Directory vehicleAiDir(configpath + VEHICLEAI_SCRIPT_PATH);
	for(Avalon::Directory::iterator iter = vehicleAiDir.Begin();
		iter != vehicleAiDir.End(); ++iter)
	{
		std::string name = iter->GetName();
		if(name.length() < 15 || name[0] == '.') continue;

		std::string head = name.substr(0,10);
		std::string tail = name.substr(name.length() - 3);
		if(head.compare("VehicleAI_") != 0 || tail.compare("lua") != 0)
		{
			ErrorLogStream << "unknow ai script:" << name << "!" << LogStream::eos;
			continue;
		}

		UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(name.substr(10, name.length() - 14));
		if(id == 0) continue;
		if(m_VehicleAIScripts.find(id) != m_VehicleAIScripts.end())
		{
			ErrorLogStream << "repeat vehicle ai script: " << name << "!" << LogStream::eos;
			continue;
		}

		std::string env = name.substr(0, name.length() - 4);
		m_pState->MakeEnv(env.c_str(), "SetSandBox");
		VehicleAIScript* script = new VehicleAIScript(id, m_pState);
		if(!script->Load(iter->GetFullPath().c_str(), env.c_str()))
		{
			CL_SAFE_DELETE(script);
			ErrorLogStream << "load  vehicle ai script " << name << " failed!" << LogStream::eos;
			return false;
		}

		if(!m_VehicleAIScripts.insert(std::make_pair(id, script)).second)
		{
			CL_SAFE_DELETE(script);
			continue;
		}
	}

	//加载活动脚本
	Avalon::Directory activityDir(configpath + ACTIVITY_SCRIPT_PATH);
	for (Avalon::Directory::iterator iter = activityDir.Begin();
		iter != activityDir.End(); ++iter)
	{
		std::string name = iter->GetName();
		if (name.length() < 10 || name[0] == '.') continue;

		std::string head = name.substr(0, 9);
		std::string tail = name.substr(name.length() - 3);
		if (head.compare("activity_") != 0 || tail.compare("lua") != 0)
		{
			ErrorLogStream << "unknow task script:" << name << "!" << LogStream::eos;
			continue;
		}

		UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(name.substr(9, name.length() - 13));
		if (id == 0) continue;
		if (m_ActivityScripts.find(id) != m_ActivityScripts.end())
		{
			ErrorLogStream << "repeat activity script: " << name << "!" << LogStream::eos;
			continue;
		}

		std::string env = name.substr(0, name.length() - 4);
		m_pState->MakeEnv(env.c_str(), "SetSandBox");
		ActivityScript* script = new ActivityScript(id, m_pState);
		if (!script->Load(iter->GetFullPath().c_str(), env.c_str()))
		{
			CL_SAFE_DELETE(script);
			ErrorLogStream << "load activity script " << name << " failed!" << LogStream::eos;
			return false;
		}

		if (!m_ActivityScripts.insert(std::make_pair(id, script)).second)
		{
			CL_SAFE_DELETE(script);
			continue;
		}

		script->Init();
	}

	

	Avalon::ScriptFile counterScript(m_pState);
	m_pState->MakeEnv("Counter", "SetSandBox");
	std::string counterPath = configpath + COUNTER_SCRIPT_PATH;
	if(!counterScript.Load(counterPath.c_str(), "Counter"))
	{
		ErrorLogStream << "load counter script failed!" << LogStream::eos;
		return false;
	}
	counterScript.Call<void>("Init");

	m_pPlayerScript = new PlayerScript(m_pState);
	m_pState->MakeEnv("Player", "SetSandBox");
	std::string playerPath = configpath + PLAYER_SCRIPT_PATH;
	if(!m_pPlayerScript->Load(playerPath.c_str(), "Player"))
	{
		ErrorLogStream << "load player script failed!" << LogStream::eos;
		return false;
	}
	m_pPlayerScript->Init();

	m_pTerritoryScript = new TerritoryScript(m_pState);
	m_pState->MakeEnv("Territory", "SetSandBox");
	std::string territoryPath = configpath + TERRITORY_SCRIPT_PATH;
	if(!m_pTerritoryScript->Load(territoryPath.c_str(), "Territory"))
	{
		ErrorLogStream << "load territory script failed!" << LogStream::eos;
		return false;
	}
	m_pTerritoryScript->Init();

	m_pFinalEquipScript = new FinalEquipScript(m_pState);
	m_pState->MakeEnv("FinalEquip", "SetSandBox");
	std::string finalEquipPath = configpath + FINALEQUIP_SCRIPT_PATH;
	if(!m_pFinalEquipScript->Load(finalEquipPath.c_str(), "FinalEquip"))
	{
		ErrorLogStream << "load finalequip script failed!" << LogStream::eos;
		return false;
	}
	m_pFinalEquipScript->Init();

	m_pEquipCalcScript = new EquipCalcScript(m_pState);
	m_pState->MakeEnv("EquipCalc", "SetSandBox");
	std::string equipCalcPath = configpath + EQUIPCALC_SCRIPT_PATH;
	if (!m_pEquipCalcScript->Load(equipCalcPath.c_str(), "EquipCalc"))
	{
		ErrorLogStream << "load equipCalc script failed!" << LogStream::eos;
		return false;
	}

	m_pEscortScript = new EscortScript(m_pState);
	m_pState->MakeEnv("Escort", "SetSandBox");
	std::string escortPath = configpath  + ESCORT_SCRIPT_PATH;
	if(!m_pEscortScript->Load(escortPath.c_str(), "Escort"))
	{
		ErrorLogStream << "load escort script failed!" << LogStream::eos;
		return false;
	}
	m_pEscortScript->Init();

	m_pActivityGiftbagScript = new ActivityGiftbagScript(m_pState);
	m_pState->MakeEnv("acGiftbag", "SetSandBox");
	std::string	acgiftbagPath = configpath + ACGIFTBAG_SCRIPT_PASH;
	if(!m_pActivityGiftbagScript->Load(acgiftbagPath.c_str(), "acGiftbag"))
	{
		ErrorLogStream << "load activitygiftbag script failed!" << LogStream::eos;
		return false;
	}
	m_pActivityGiftbagScript->Init();
#endif

	for (UInt32 i = 0; i < SCENE_SCRIPT_NUM; i++)
	{
		if (!LoadScript((SScriptType)i))
		{
			return false;
		}
	}

	return true;
}

bool ScriptMgr::LoadScript(UInt32 type)
{
	if (type >= sizeof(s_scriptConfigs) / sizeof(s_scriptConfigs[0]))
	{
		return false;
	}

	if (SScriptType::SCENE_SCRIPT_TASK == (SScriptType)type)
	{
		return LoadTaskScript();
	}
	else
	{
		return CLScriptMgr::LoadScript(type, CONFIG_PATH, s_scriptConfigs[type]);
	}
}

bool ScriptMgr::LoadTaskScript()
{
	const ScriptConfig& config = s_scriptConfigs[0];
	std::string root = CONFIG_PATH;
	std::string scriptPath = root + config.path;
	std::string scriptPrefix = config.prefix;
	SScriptType type = SScriptType::SCENE_SCRIPT_TASK;

	auto& scriptMgr = GetScriptMgr(type);
	std::string postfix = ".lua";

	class TaskScriptVisiotr : public Avalon::DataEntryVisitor<TaskDataEntry>
	{
	public:
		TaskScriptVisiotr(){}
		~TaskScriptVisiotr(){}

		bool operator()(TaskDataEntry* entry)
		{
			if (entry->open && !IsAccountTask(entry->type))
			{
				m_Tasks.push_back(entry);
			}
			return true;
		}

		std::vector<TaskDataEntry*> GetTasks() { return m_Tasks; }

	private:
		std::vector<TaskDataEntry*> m_Tasks;
	};

	TaskScriptVisiotr visitor;
	TaskDataEntryMgr::Instance()->Visit(visitor);

	std::vector<TaskDataEntry*> tasks = visitor.GetTasks();
	std::vector<TaskDataEntry*>::iterator iter = tasks.begin();
	while (iter != tasks.end())
	{
		UInt32 id = (*iter)->id;
		UInt32 templateId = (*iter)->templateId;
		if (templateId == 0) templateId = id;
		if (scriptMgr.find(id) != scriptMgr.end())
		{
			ErrorLogStream << "repeat task script: " << id << "!" << LogStream::eos;
			continue;
		}

		std::string env = scriptPrefix + Avalon::StringUtil::ConvertToString(id);
		std::string fileName = scriptPrefix + Avalon::StringUtil::ConvertToString(templateId) + postfix;
		m_pState->MakeEnv(env.c_str(), "SetSandBox");
		TaskScript* script = (TaskScript*)CreateScript(type, m_pState, id);
		std::string fullPath = scriptPath + fileName;
		if (!script || !script->Load(fullPath.c_str(), env.c_str()))
		{
			// 加载失败，把之前加载好的也删了
			for (auto itr : scriptMgr)
			{
				CL_SAFE_DELETE(itr.second);
			}
			scriptMgr.clear();

			CL_SAFE_DELETE(script);
			ErrorLogStream << "load task script " << id << " failed!" << LogStream::eos;
			return false;
		}
		if (templateId != id)
		{
			script->SetScriptParam((*iter)->scriptParam);
		}
		script->Init();

		if (!scriptMgr.insert(std::make_pair(id, script)).second)
		{
			CL_SAFE_DELETE(script);
			continue;
		}

		OnLoadScriptFinish(type, script);

		iter++;
	}

	return true;
}

void ScriptMgr::OnLoadScriptFinish(UInt32 type, Avalon::ScriptFile* script)
{
	if (!script)
	{
		return;
	}

	if (type == SCENE_SCRIPT_TASK)
	{
		TaskScript* taskScript = (TaskScript*)script;
		const std::vector<CLZone>& triggerZones = taskScript->GetTriggerZones();
		for (std::vector<CLZone>::const_iterator iter = triggerZones.begin();
			iter != triggerZones.end(); ++iter)
		{
			m_SceneTasks[iter->mapid].push_back(taskScript);
		}
	}
}

Avalon::ScriptFile* ScriptMgr::CreateScript(UInt32 type, Avalon::ScriptState* state, UInt32 id)
{
// 	switch (type)
// 	{
// 	case SCENE_SCRIPT_TASK:
// 		return new TaskScript(id, state);
// 		break;
// 	case SCENE_SCRIPT_ITEM:
// 		return new ItemScript(id, state);
// 		break;
// 	case SCENE_SCRIPT_SCENE:
// 		return new SceneScript(id, state);
// 		break;
// 	case SCENE_SCRIPT_AI:
// 		return new AIScript(id, state);
// 		break;
// 	case SCENE_SCRIPT_ACTIVITY:
// 		return new ActivityScript(id, state);
// 		break;
// 	case SCENE_SCRIPT_FINALEQUIP:
// 		return new FinalEquipScript(state);
// 		break;
// 	case SCENE_SCRIPT_EQUIPCALC:
// 		return new EquipCalcScript(state);
// 		break;
// 	case SCENE_SCRIPT_PLAYER:
// 		return new PlayerScript(state);
// 		break;
// 	case SCENE_SCRIPT_AIBIND:
// 	case SCENE_SCRIPT_COUNTER:
// 		return new Avalon::ScriptFile(state);
// 		break;
// 	default:
// 		break;
// 	}
// 
// 	return NULL;

	switch (type)
	{
	case SCENE_SCRIPT_TASK:
		return new TaskScript(id, state);
		break;
	case SCENE_SCRIPT_ITEM:
		return new ItemScript(id, state);
		break;
	case SCENE_SCRIPT_ACTIVITY:
		return new ActivityScript(id, state);
		break;
	case SCENE_SCRIPT_EQUIPCALC:
		return new EquipCalcScript(state);
		break;
	case SCENE_SCRIPT_PLAYER:
		return new PlayerScript(state);
		break;
	case SCENE_SCRIPT_COUNTER:
		return new Avalon::ScriptFile(state);
		break;
	default:
		break;
	}

	return NULL;
}

bool ScriptMgr::ReloadScripts()
{
#ifndef _NEW_SCRIPT_MGR_
	Avalon::ScriptState* oldState = m_pState;

	NpcScriptMap oldNpcScripts = m_NpcScripts;
	m_NpcScripts.clear();
	TaskScriptMap oldTaskScripts = m_TaskScripts;
	m_TaskScripts.clear();
	ActivityScriptMap oldActivityScripts = m_ActivityScripts;
	m_ActivityScripts.clear();
	ItemScriptMap oldItemScripts = m_ItemScripts;
	m_ItemScripts.clear();
	SceneScriptMap oldSceneScripts = m_SceneScripts;
	m_SceneScripts.clear();
	AIScriptMap oldAIScripts = m_AIScripts;
	m_AIScripts.clear();
	SceneTaskMap oldSceneTasks = m_SceneTasks;
	m_SceneTasks.clear();
	AIBindMap oldAIBindMap = m_AIBindMap;
	m_AIBindMap.clear();
	VehicleAIScriptMap oldVehicleAIScripts = m_VehicleAIScripts;
	m_VehicleAIScripts.clear();

	PlayerScript* oldPlayerScript = m_pPlayerScript;
	m_pPlayerScript = NULL;
	TerritoryScript	*pOldTerritoryScript = m_pTerritoryScript;
	m_pTerritoryScript = NULL;
	FinalEquipScript *pOldFinalEquipScript = m_pFinalEquipScript;
	m_pFinalEquipScript = NULL;
	EscortScript*	pOldEscortScript = m_pEscortScript;
	m_pEscortScript = NULL;
	ActivityGiftbagScript	*pOldAcGiftbagScript = m_pActivityGiftbagScript;
	m_pActivityGiftbagScript = NULL;

	m_pState = new Avalon::ScriptState();

	DefClasses();

	if(!LoadScripts())
	{
		Final();
		CL_SAFE_DELETE(m_pState);

		m_pState = oldState;
		m_NpcScripts = oldNpcScripts;
		m_TaskScripts = oldTaskScripts;
		m_ActivityScripts = oldActivityScripts;
		m_ItemScripts = oldItemScripts;
		m_SceneScripts = oldSceneScripts;
		m_AIScripts = oldAIScripts;
		m_AIBindMap = oldAIBindMap;
		m_VehicleAIScripts = oldVehicleAIScripts;

		m_pPlayerScript = oldPlayerScript;
		m_pTerritoryScript = pOldTerritoryScript;
		m_pFinalEquipScript = pOldFinalEquipScript;
		m_pEscortScript = pOldEscortScript;
		m_pActivityGiftbagScript = pOldAcGiftbagScript;
		
		m_SceneTasks = oldSceneTasks;
		return false;
	}

	CL_SAFE_DELETE(oldState);
	for(NpcScriptMap::iterator iter = oldNpcScripts.begin(); iter != oldNpcScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	oldNpcScripts.clear();

	for(TaskScriptMap::iterator iter = oldTaskScripts.begin(); iter != oldTaskScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	oldTaskScripts.clear();
	oldSceneTasks.clear();

	for (ActivityScriptMap::iterator iter = oldActivityScripts.begin(); iter != oldActivityScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	oldActivityScripts.clear();

	for(ItemScriptMap::iterator iter = oldItemScripts.begin(); iter != oldItemScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	oldItemScripts.clear();

	for(SceneScriptMap::iterator iter = oldSceneScripts.begin(); iter != oldSceneScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	oldSceneScripts.clear();

	for(AIScriptMap::iterator iter = oldAIScripts.begin(); iter != oldAIScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	oldAIScripts.clear();

	for(VehicleAIScriptMap::iterator iter = oldVehicleAIScripts.begin(); iter != oldVehicleAIScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	oldVehicleAIScripts.clear();

	CL_SAFE_DELETE(oldPlayerScript);
	m_pTerritoryScript->InheritOwner(pOldTerritoryScript);
	CL_SAFE_DELETE(pOldTerritoryScript);
	CL_SAFE_DELETE(pOldFinalEquipScript);
	CL_SAFE_DELETE(pOldEscortScript);
	CL_SAFE_DELETE(pOldAcGiftbagScript);
#endif

	for (UInt32 i = 0; i < SCENE_SCRIPT_NUM; i++)
	{
		if (!ReloadScript((SScriptType)i))
		{
			return false;
		}
	}

	return true;
}

void ScriptMgr::BindAI(UInt32 id1, UInt32 id2)
{
	m_AIBindMap[id1] = id2;
}

TaskScript* ScriptMgr::FindTaskScript(UInt32 id)
{
#ifndef _NEW_SCRIPT_MGR_
	TaskScriptMap::iterator iter = m_TaskScripts.find(id);
	if (iter != m_TaskScripts.end()) return iter->second;
	return NULL;
#else
	return GetScript<TaskScript>(SCENE_SCRIPT_TASK, id);
#endif
}

ActivityScript* ScriptMgr::FindActivityScript(UInt32 id)
{
#ifndef _NEW_SCRIPT_MGR_
	ActivityScriptMap::iterator iter = m_ActivityScripts.find(id);
	if (iter != m_ActivityScripts.end()) return iter->second;
	return NULL;
#else
	return GetScript<ActivityScript>(SCENE_SCRIPT_ACTIVITY, id);
#endif
}

ItemScript* ScriptMgr::FindItemScript(UInt32 id)
{
#ifndef _NEW_SCRIPT_MGR_
	ItemScriptMap::iterator iter = m_ItemScripts.find(id);
	if(iter != m_ItemScripts.end()) return iter->second;
	return NULL;
#else
	return GetScript<ItemScript>(SCENE_SCRIPT_ITEM, id);
#endif
}

SceneScript* ScriptMgr::FindSceneScript(UInt32 id)
{
#ifndef _NEW_SCRIPT_MGR_
	SceneScriptMap::iterator iter = m_SceneScripts.find(id);
	if(iter != m_SceneScripts.end()) return iter->second;
	return NULL;
#else
	//return GetScript<SceneScript>(SCENE_SCRIPT_SCENE, id);
	return NULL;
#endif
}

const ScriptMgr::TaskScriptVec& ScriptMgr::GetMapTriggerTasks(UInt32 mapid) const
{
	static TaskScriptVec NullVec;
	SceneTaskMap::const_iterator iter = m_SceneTasks.find(mapid);
	if(iter != m_SceneTasks.end()) return iter->second;
	return NullVec;
}

PlayerScript* ScriptMgr::GetPlayerScript()  const
{
#ifndef _NEW_SCRIPT_MGR_
	return m_pPlayerScript;
#else
	return GetSingleScript<PlayerScript>(SCENE_SCRIPT_PLAYER); 
#endif
}

FinalEquipScript* ScriptMgr::GetFinalEquipScript() const 
{
#ifndef _NEW_SCRIPT_MGR_
	return m_pFinalEquipScript;
#else
	//return GetSingleScript<FinalEquipScript>(SCENE_SCRIPT_FINALEQUIP);
	return NULL;
#endif
}

EquipCalcScript* ScriptMgr::GetEquipCalcScript() const 
{
#ifndef _NEW_SCRIPT_MGR_
	return m_pEquipCalcScript;
#else
	return GetSingleScript<EquipCalcScript>(SCENE_SCRIPT_EQUIPCALC);
#endif
}

void ScriptMgr::DefClasses()
{
	m_pState->DefClass<SysInterface>("SysClass")
		.Def("LogError", &SysInterface::LogError)
		.Def("LogInfo", &SysInterface::LogInfo)
		.Def("LogDebug", &SysInterface::LogDebug)
		.Def("LogTrace", &SysInterface::LogTrace)
		.Def("LogFatal", &SysInterface::LogFatal)
		.Def("Now", &SysInterface::Now)
		.Def("IsSameDay", &SysInterface::IsSameDay)
		.Def("RegCounter", &SysInterface::RegCounter)
		.Def("Year", &SysInterface::Year)
		.Def("Month", &SysInterface::Month)
		.Def("Day", &SysInterface::Day)
		.Def("Hour", &SysInterface::Hour)
		.Def("Min", &SysInterface::Min)
		.Def("Sec", &SysInterface::Sec)
		.Def("WeekDay", &SysInterface::WeekDay)
		.Def("Notify", &SysInterface::Notify)
		.Def("BroadcastNotify", &SysInterface::BroadcastNotify)
		.Def("SelectInHundred", &SysInterface::SelectInHundred)
		.Def("SelectInThousand", &SysInterface::SelectInThousand)
		.Def("SelectInTenThousand", &SysInterface::SelectInTenThousand)
		.Def("SelectInHundredThousand", &SysInterface::SelectInHundredThousand)
		.Def("RandBetween", &SysInterface::RandBetween)
		.Def("OnSortValueChanged", &SysInterface::OnSortValueChanged)
		.Def("OnWorldSortValueChanged", &SysInterface::OnWorldSortValueChanged)
		.Def("SendSysMail", &SysInterface::SendSysMail)
		.Def("IsInActivity", &SysInterface::IsInActivity)
		.Def("GetRewardGroup", &SysInterface::GetRewardGroup)
		.Def("GetMapName", &SysInterface::GetMapName)
		.Def("GetRandDirection", &SysInterface::GetRandDirection)
		.Def("GetBaseExp", &SysInterface::GetBaseExp)
		.Def("GetBaseSilver", &SysInterface::GetBaseSilver)
		.Def("GetWorldLevel", &SysInterface::GetWorldLevel)
		.Def("BroadcastEffectEvent", &SysInterface::BroadcastEffectEvent)
		.Def("GetZoneId", &SysInterface::GetZoneId)
		.Def("SendOfflineNotify", &SysInterface::SendOfflineNotify)
		.Def("SyncCounter", &SysInterface::SyncCounter)
		.Def("GetNeutralZoneId", &SysInterface::GetNeutralZoneId)
		.Def("GetNeutralRandMajorCity", &SysInterface::GetNeutralRandMajorCity)
		.Def("GenActivityCityMonster", &SysInterface::GenActivityCityMonster)
		.Def("RemoveTaskCityMonster", &SysInterface::RemoveTaskCityMonster)
		.Def("GetEnergyExchangeExp", &SysInterface::GetEnergyExchangeExp)
		;
	m_pState->Set("Sys", &sys);

	m_pState->DefClass<ScriptMgr>("ScriptMgr")
		.Def("BindAI", &ScriptMgr::BindAI);

	m_pState->DefClass<CLPosition>("Position")
		.Def("GetX",&CLPosition::GetX)
		.Def("GetY",&CLPosition::GetY);

	m_pState->DefClass<RewardGroup>("RewardGroup")
		.Def("AddReward",&RewardGroup::AddRewardScript);

	m_pState->DefClass<SceneObject>("SceneObject")
		.Def("GetID",		&SceneObject::GetID)
		.Def("GetName",		&SceneObject::ScriptGetName)
		.Def("SetName",		&SceneObject::ScriptSetName)
		.Def("GetSceneID",	&SceneObject::GetSceneID)
		.Def("GetScene",	&SceneObject::GetScene);

	m_pState->DefClass<Creature, SceneObject>("Creature")
		.Def("ToPlayer", &Creature::ToPlayer)
		.Def("HasBuff", &Creature::HasBuff)
		.Def("GetDir", &Creature::GetDir)
		.Def("SetLevel", &Creature::SetLevel)
		.Def("GetLevel", &Creature::GetLevel)
		.Def("Distance", &Creature::Distance)
		.Def("Speak", &Creature::ScriptSpeak)
		.Def("UseSkillDirect", &Creature::ScriptUseSkillDirect)
		.Def("ClearSkills", &Creature::ClearSkills)
		.Def("ForgetAllSkills", &Creature::ForgetAllSkills)
		;

	m_pState->DefClass<Team>("Team")
		.Def("GetID",	&Team::GetId)
		.Def("GetSize", &Team::GetSize)
		.Def("GetMemberNumAround", &Team::GetMemberNumAround)
		.Def("CheckMembersLevel", &Team::CheckMembersLevel)
		.Def("CheckTaskAcceptable", &Team::CheckTaskAcceptable)
		.Def("AcceptTask", &Team::AcceptTask)
		.Def("Notify",	&Team::Notify)
		.Def("GetMaster", &Team::GetMaster)
		.Def("GetMemberNumInScene", &Team::GetMemberNumInScene)
		.Def("AcceptTask", &Team::AcceptTask)
		.Def("SetTaskVar", &Team::SetTaskVar)
		.Def("SubmitTask", &Team::SubmitTask)
		.Def("CheckPackGridNum", &Team::CheckPackGridNum)
		.Def("GetMembersMaxLevel", &Team::GetMembersMaxLevel)
		.Def("GetMembersAverageLevel", &Team::GetMembersAverageLevel)
		.Def("IsMixedTeamInScene", &Team::IsMixedTeamInScene);

	m_pState->DefClass<Player,Creature>("Player")
		.Def("GetBase64ID", &Player::GetBase64ID)
		.Def("FindTask",&Player::FindTask)
		.Def("GetExp",&Player::GetExp)
		.Def("IncExp",&Player::IncExp)
		.Def("DecExp",&Player::DecExp)
		.Def("AddExpFromRewardAdapter", &Player::AddExpFromRewardAdapter)
		.Def("AddExpByLevelPercent", &Player::AddExpByLevelPercent)
		.Def("GetSex",&Player::GetSex)
		.Def("GetLevel", &Player::GetLevel)
		.Def("GetOccu",&Player::GetOccu)
		.Def("GetMainOccu",&Player::GetMainOccu)
		.Def("IsInitOccu", &Player::IsInitOccu)
		.Def("GetVipLvl",&Player::GetVipLvl)
		.Def("GetItemNum",&Player::GetItemNum)
		.Def("RemoveItem",&Player::RemoveItem)
		.Def("RemoveItemDirect", &Player::RemoveItemDirect)
		.Def("CheckAddItem",&Player::CheckAddItem)
		.Def("GetPackGridLeft",&Player::GetPackGridLeft)
		.Def("AddItem",&Player::AddItem)
		.Def("ReduceItemNum",&Player::ReduceItemNum)
		.Def("GetRewardGroup",&Player::GetRewardGroup)
		.Def("GetRewardGroupNoClear", &Player::GetRewardGroupNoClear)
		.Def("CheckAddRewards",&Player::CheckAddRewardsScript)
		.Def("AddRewards",&Player::AddRewards)
		.Def("AddGold",&Player::AddGold)
		.Def("GetGold",&Player::GetGold)
		.Def("RemoveGold",&Player::RemoveGold)
		.Def("AddPoint",&Player::AddPoint)
		.Def("GetPoint",&Player::GetPoint)
		.Def("RemovePoint",&Player::RemovePoint)
		.Def("AddCreditPoint", &Player::AddCreditPoint)
		.Def("GetCreditPoint", &Player::GetCreditPoint)
		.Def("RemoveCreditPoint", &Player::RemoveCreditPoint)
		.Def("AddBindGold",&Player::AddBindGold)
		.Def("GetBindGold",&Player::GetBindGold)
		.Def("RemoveBindGold",&Player::RemoveBindGold)
		.Def("GetGoldBoth", &Player::GetGoldBoth)
		.Def("GetPointBoth", &Player::GetPointBoth)
		.Def("RemoveGoldBoth", &Player::RemoveGoldBoth)
		.Def("RemovePointBoth", &Player::RemovePointBoth)
		.Def("Notify",&Player::Notify)
		.Def("SendSysMail",&Player::SendSysMail)
		.Def("GetTeam", &Player::GetTeam)
		.Def("IsTeamMaster", &Player::IsTeamMaster)
		.Def("IncCounter",&Player::IncCounter)
		.Def("GetCounter",&Player::GetCounter)
		.Def("SetCounter",&Player::SetCounter)
		.Def("SubmitTask",&Player::SubmitTask)
		.Def("AcceptTask",&Player::ScriptAcceptTask)
		.Def("AbandonTask",&Player::AbandonTask)
		.Def("GetDailyTaskNum",&Player::GetDailyTaskNum)
		.Def("GetDailyTaskSelectorNum", &Player::GetDailyTaskSelectorNum)
		.Def("CheckTaskFinished",&Player::CheckTaskFinished)
		.Def("SetItemBar", &Player::SetItemBar)
		.Def("OnActiveEventFinished", &Player::OnActiveEventFinished)
		.Def("CreateTeam", &Player::CreateTeam)
		.Def("GetDailyOnlineTime", &Player::GetDailyOnlineTime)
		.Def("StartStory", &Player::StartStory)
		.Def("CancelStory", &Player::CancelStory)
		.Def("GetCreateTime", &Player::GetCreateTime)
		.Def("AddIntimacy", &Player::AddIntimacy)
		.Def("IncActionCounter", &Player::IncActionCounter)
		.Def("TestActionFlag", &Player::TestActionFlag)
		.Def("GetPf", &Player::ScriptGetPf)
		.Def("GetSrcZoneId", &Player::GetSrcZoneId)
		.Def("GetGameStatus", &Player::GetGameStatus)
		.Def("ChangeZone", &Player::ChangeZone)
		.Def("EquipItem", &Player::EquipItem)
		.Def("OnGetRewardGroup", &Player::OnGetRewardGroup)
		.Def("AddBuff", &Player::AddBuff)
		.Def("IsDungeonAnyHardPassed", &Player::IsDungeonAnyHardPassed)		//adder by huchenhui 2016.06.29
		.Def("IsDungeonPassed", &Player::IsDungeonPassed)					//adder by huchenhui 2017.02.08
		.Def("GetDungeonBestScore", &Player::GetDungeonBestScore)			//adder by huchenhui 2019.03.27
		.Def("GetTeamSize", &Player::GetTeamSize)							//adder by huchenhui 2016.07.12
		.Def("AddRetinue", &Player::AddRetinue)								//adder by huchenhui 2016.07.29
		.Def("GetRetinueSize", &Player::GetRetinueSize)						//adder by huchenhui 2016.08.04
		.Def("GetRetinueSizeByQuality", &Player::GetRetinueSizeByQuality)	//adder by huchenhui 2016.09.07
		.Def("SendMagicJarResult", &Player::SendMagicJarResult)
		.Def("OnOpenMagicJar", &Player::OnOpenMagicJar)						//adder by huchenhui 2016.08.20
		.Def("GetMaxWinStreak", &Player::GetMaxWinStreak)					//adder by huchenhui 2016.08.22
		.Def("GetCurWinStreak", &Player::GetCurWinStreak)					//adder by huchenhui 2018.04.09
		.Def("GetTotalWinNum", &Player::GetTotalWinNum)						//adder by huchenhui 2017.04.26
		.Def("GetMatchScore", &Player::GetMatchScore)						//adder by huchenhui 2016.08.22
		.Def("GetBestMatchScore", &Player::GetBestMatchScore)				//adder by huchenhui 2016.08.22
		.Def("GetSeasonLevel", &Player::GetSeasonLevel)						//adder by huchenhui 2017.04.22
		.Def("GetSeasonStar", &Player::GetSeasonStar)						//adder by huchenhui 2017.04.22
		.Def("GetPkLevelType", &Player::GetPkLevelType)						//adder by huchenhui 2016.08.25
		.Def("GetPkLevel", &Player::GetPkLevel)								//adder by huchenhui 2016.08.25
		.Def("GetWarpStoneSize", &Player::GetWarpStoneSize)					//adder by huchenhui 2016.08.25
		.Def("GetUseFatigue", &Player::GetUseFatigue)						//adder by huchenhui 2016.08.29
		.Def("GetLostFatigue", &Player::GetLostFatigue)						//adder by huchenhui 2016.10.28
		.Def("GetFatigue", &Player::GetFatigue)								//adder by panxirun  2018.7.25
		.Def("RemoveFatigue", &Player::RemoveFatigue)						//adder by panxirun  2018.7.25
		.Def("GetCount", &Player::GetCount)
		.Def("GetAwaken", &Player::GetAwaken)								//adder by huchenhui 2016.09.02
		.Def("GetEquipSize", &Player::GetEquipSize)							//adder by huchenhui 2016.09.13
		.Def("GetGreenEquipSize", &Player::GetGreenEquipSize)				//adder by huchenhui 2016.09.13
		.Def("GetFashionSize", &Player::GetFashionSize)						//adder by huchenhui 2018.03.20
		.Def("AddFatigue", &Player::AddFatigue)								//adder by huchenhui 2016.09.18
		.Def("AddSP", &Player::AddSP)										//adder by huchenhui 2016.11.05
		.Def("GetContinuousOnlineDays", &Player::GetContinuousOnlineDays)
		.Def("GetCreateFromNowOn", &Player::GetCreateFromNowOn)
		.Def("GetCreateFromNowOnDay", &Player::GetCreateFromNowOnDay)
		.Def("SendNotifyById", &Player::SendNotifyById)
		.Def("GetAllEquipLvMin", &Player::GetAllEquipLvMin)
		.Def("GetAllEquipQualityMin", &Player::GetAllEquipQualityMin)
		.Def("GetWearFashionNum", &Player::GetWearFashionNum)
		.Def("GetEquipNumByStrLv", &Player::GetEquipNumByStrLv)
		.Def("GetStrMaxLv", &Player::GetStrMaxLv)
		.Def("GetStrMaxLvAll", &Player::GetStrMaxLvAll)
		.Def("GetEquipNumByLv", &Player::GetEquipNumByLv)
		.Def("GetTitleNum", &Player::GetTitleNum)
		.Def("GetWearEquipNumByQua", &Player::GetWearEquipNumByQua)
		.Def("GetSkillCount", &Player::GetSkillCount)							//adder by huchenhui 2016.10.10
		.Def("GetSkillCountById", &Player::GetSkillCountById)					//adder by huchenhui 2016.10.10
		.Def("GetSkillCountByLevel", &Player::GetSkillCountByLevel)				//adder by huchenhui 2016.10.10
		.Def("GetChangeSkillTotal", &Player::GetChangeSkillTotal)				//adder by huchenhui 2016.10.10
		.Def("GetDayOnlineTime", &Player::GetDayOnlineTime)
		.Def("GetSignInCount", &Player::GetSignInCount)							//adder by huchenhui 2016.12.08
		.Def("GetWarpStoneMinLevel", &Player::GetWarpStoneMinLevel)				//adder by huchenhui 2016.12.30
		.Def("GetMaxRetinueLevel", &Player::GetMaxRetinueLevel)					//adder by huchenhui 2016.12.30
		.Def("GetMaxRetinueStar", &Player::GetMaxRetinueStar)					//adder by huchenhui 2016.12.30
		.Def("GetTotChargeNum", &Player::GetTotChargeNum)		
		.Def("GetPlayerChargeTotal", &Player::GetPlayerChargeTotal)
		.Def("OnBuyMonthCard", &Player::OnBuyMonthCard)
		.Def("OnBuyMoneyManageCard", &Player::OnBuyMoneyManageCard)
		.Def("OpenJar", &Player::OpenJar)
		.Def("AddPet", &Player::AddPet)													//adder by huchenhui 2017.08.01
		.Def("GetPetSize", &Player::GetPetSize)											//adder by huchenhui 2018.03.21
		.Def("AcceptLegendTask", &Player::AcceptLegendTask)								//adder by huchenhui 2017.08.24
		.Def("CheckLegendTaskFinished", &Player::CheckLegendTaskFinished)				//adder by huchenhui 2017.08.24
		.Def("CheckAchievementTaskFinished", &Player::CheckAchievementTaskFinished)		//adder by huchenhui 2018.03.27
		.Def("GetEnlargePackageSize", &Player::GetEnlargePackageSize)					//adder by huchenhui 2018.06.11
		.Def("GetEnlargeStoragePackSize", &Player::GetEnlargeStoragePackSize)			//adder by huchenhui 2018.06.11
		.Def("GetDeathTowerTopFloor", &Player::GetDeathTowerTopFloor)					//adder by huchenhui 2018.06.12
		.Def("SummonTaskNpc", &Player::SummonTaskNpc)
		.Def("GetDailyCityMonsterTimes", &Player::GetDailyCityMonsterTimes)				//adder by huchenhui 2018.07.23
		.Def("GetDungeonDailyCount", &Player::GetDungeonDailyCount)						//adder by huchenhui 2018.08.07
		.Def("IsMaster", &Player::IsMaster)												//是否是师傅
		.Def("GetDailyTaskScore", &Player::GetDailyTaskScore)
		.Def("AddDungeonTimes", &Player::AddDungeonTimes)
		.Def("OpenFunction", &Player::OpenFunction)										//adder by huchenhui 2018.08.29
		.Def("GetFinSchDiscipleSum", &Player::GetFinSchDiscipleSum)
		.Def("GetDiscipleNum", &Player::GetDiscipleNum)
		.Def("FindAchievementTaskTask", &Player::FindAchievementTaskTask)
		.Def("GetDiscOpTaskStByTmpAndExcTaskId", &Player::GetDiscOpTaskStByTmpAndExcTaskId)
		.Def("SetDiscOpTaskStByTmpAndExcTaskId", &Player::SetDiscOpTaskStByTmpAndExcTaskId)
		.Def("NotifyIncExp", &Player::NotifyIncExp)
		.Def("GetItemNumWithEqualItem",&Player::GetItemNumWithEqualItem)
		.Def("RemoveItemWithEqualItem", &Player::RemoveItemWithEqualItem)
		;

	m_pState->DefClass<TaskScript>("TaskScript")
		.Def("GetScriptParam", &TaskScript::GetScriptParam)
		.Def("BindGetItemEvent", &TaskScript::BindGetItemEvent)
		.Def("BindGetItemQualityEvent", &TaskScript::BindGetItemQualityEvent)
		.Def("BindGetItemStrengthenEvent", &TaskScript::BindGetItemStrengthenEvent)
		.Def("BindRmItemEvent", &TaskScript::BindRmItemEvent)
		.Def("BindItemChangeEvent", &TaskScript::BindItemChangeEvent)
		.Def("BindKillNpcEvent", &TaskScript::BindKillNpcEvent)
		.Def("BindKillNpcTypeEvent", &TaskScript::BindKillNpcTypeEvent)
		.Def("BindKillLevelNpcEvent", &TaskScript::BindKillLevelNpcEvent)
		.Def("BindKillCityMonsterEvent", &TaskScript::BindKillCityMonsterEvent)
		.Def("BindUseItemEvent", &TaskScript::BindUseItemEvent)
		.Def("BindTriggerZone", &TaskScript::BindTriggerZone)
		.Def("BindEnterZoneEvent", &TaskScript::BindEnterZoneEvent)
		.Def("BindRemakeEquipEvent", &TaskScript::BindRemakeEquipEvent)
		.Def("BindCatchPetEvent", &TaskScript::BindCatchPetEvent)
		.Def("BindLevelupEvent", &TaskScript::BindLevelupEvent)
		.Def("BindEscortSuccedEvent", &TaskScript::BindEscortSuccedEvent)
		.Def("BindCopyScenePassedEvent", &TaskScript::BindCopyScenePassedEvent)
		.Def("BindClearDungeonEvent", &TaskScript::BindClearDungeonEvent)						//added by aprilliu, 其实和BindCopyScenePassedEvent一样
		.Def("BindClearDungeonEventByRevive", &TaskScript::BindClearDungeonEventByRevive)		//added by huchenhui, 根据复活次数限定
		.Def("BindClearActivityDungeonEvent", &TaskScript::BindClearActivityDungeonEvent)		//added by huchenhui, 监听活动地下城
		.Def("BindBabelPassedEvent", &TaskScript::BindBabelPassedEvent)
		.Def("BindBuyMallItemEvent", &TaskScript::BindBuyMallItemEvent)
		.Def("BindEnterTribeEvent", &TaskScript::BindEnterTribeEvent)
		.Def("BindStoryEndEvent", &TaskScript::BindStoryEndEvent)
		.Def("BindTaskSuccedEvent", &TaskScript::BindTaskSuccedEvent)
		.Def("BindSubmitTaskSucceedEvent", &TaskScript::BindSubmitTaskSucceedEvent)				//added by aprilliu
		.Def("BindTaskFailedEvent", &TaskScript::BindTaskFailedEvent)
		.Def("BindDiedEvent", &TaskScript::BindDiedEvent)
		.Def("BindDailyTaskSubmittedEvent", &TaskScript::BindDailyTaskSubmittedEvent)
		.Def("BindTimeEvent", &TaskScript::BindTimeEvent)
		.Def("BindStrengthenEquipEvent", &TaskScript::BindStrengthenEquipEvent)							//added by huchenhui 2016.06.24 强化
		.Def("BindDecomposeEquipEvent", &TaskScript::BindDecomposeEquipEvent)							//added by huchenhui 2016.06.24 分解
		.Def("BindChangeMoneyEvent", &TaskScript::BindChangeMoneyEvent)									//added by huchenhui 2016.06.24 货币变化
		.Def("BindPutEquipEvent", &TaskScript::BindPutEquipEvent)										//added by huchenhui 2016.06.29 穿装备
		.Def("BindPutFashionEvent", &TaskScript::BindPutFashionEvent)									//added by huchenhui 2018.03.20 穿时装
		.Def("BindAddRetinueEvent", &TaskScript::BindAddRetinueEvent)									//added by huchenhui 2016.08.04 解锁随从
		.Def("BindRetinueChangeSkillEvent", &TaskScript::BindRetinueChangeSkillEvent)					//added by huchenhui 2016.08.04 随从洗练
		.Def("BindRetinueUpLevelSkillEvent", &TaskScript::BindRetinueUpLevelSkillEvent)					//added by huchenhui 2016.11.08 随从升级
		.Def("BindPVPEvent", &TaskScript::BindPVPEvent)													//added by huchenhui 2016.08.13 PVP
		.Def("BindChangeOccuEvent", &TaskScript::BindChangeOccuEvent)									//added by huchenhui 2016.08.23 转职
		.Def("BindAddMagicEvent", &TaskScript::BindAddMagicEvent)										//added by huchenhui 2016.08.23 附魔
		.Def("BindAddMagicCardEvent", &TaskScript::BindAddMagicCardEvent)								//added by huchenhui 2016.09.14 合成附魔卡
		.Def("BindOpenMagicJarEvent", &TaskScript::BindOpenMagicJarEvent)								//added by huchenhui 2016.08.23 开罐子
		.Def("BindFriendGiveEvent", &TaskScript::BindFriendGiveEvent)									//added by huchenhui 2016.08.23 好友赠送
		.Def("BindWarpStoneUpLevelEvent", &TaskScript::BindWarpStoneUpLevelEvent)						//added by huchenhui 2016.08.23 次元石升级
		.Def("BindWarpStoneAddEnergyEvent", &TaskScript::BindWarpStoneAddEnergyEvent)					//added by huchenhui 2016.09.06 次元石充能
		.Def("BindRefreshShopEvent", &TaskScript::BindRefreshShopEvent)									//added by huchenhui 2016.08.25 商店刷新
		.Def("BindRemoveFatigueEvent", &TaskScript::BindRemoveFatigueEvent)								//added by huchenhui 2016.08.29 消耗疲劳
		.Def("BindBeginHellEvent", &TaskScript::BindBeginHellEvent)										//added by huchenhui 2018.01.08 开始深渊
		.Def("BindClearHellEvent", &TaskScript::BindClearHellEvent)										//added by huchenhui 2016.08.29 深渊
		.Def("BindDeathTowerBeginEvent", &TaskScript::BindDeathTowerBeginEvent)							//added by huchenhui 2016.10.22 死亡之塔开始
		.Def("BindDeathTowerWipeoutBeginEvent", &TaskScript::BindDeathTowerWipeoutBeginEvent)			//added by huchenhui 2017.05.18 死亡之塔扫荡开始
		.Def("BindDeathTowerEvent", &TaskScript::BindDeathTowerEvent)									//added by huchenhui 2016.08.29 死亡之塔
		.Def("BindAwakenEvent", &TaskScript::BindAwakenEvent)											//added by huchenhui 2016.09.02 觉醒
		.Def("BindReviveCoinEvent", &TaskScript::BindReviveCoinEvent)									//added by huchenhui 2016.09.06 复活币
		.Def("BindAddAuctionEvent", &TaskScript::BindAddAuctionEvent)									//added by huchenhui 2016.09.14 拍卖行 上架
		.Def("BindSellAuctionEvent", &TaskScript::BindSellAuctionEvent)									//added by huchenhui 2016.09.14 拍卖行 售出
		.Def("BindBuyAuctionEvent", &TaskScript::BindBuyAuctionEvent)									//added by huchenhui 2018.03.17 拍卖行 购买
		.Def("BindSubmitActiveTaskSucceedEvent", &TaskScript::BindSubmitActiveTaskSucceedEvent)			//added by huchenhui 2016.10.22 活动任务成功
		.Def("BindSetTaskItemEvent", &TaskScript::BindSetTaskItemEvent)									//added by huchenhui 2016.11.02 提交任务 根据品质
		.Def("BindSetTaskItemByIdEvent", &TaskScript::BindSetTaskItemByIdEvent)							//added by huchenhui 2016.11.02 提交任务具体到ID
		.Def("BindSubmitCycleTaskSucceedEvent", &TaskScript::BindSubmitCycleTaskSucceedEvent)			//added by huchenhui 2016.11.07 循环任务成功
		.Def("BindShopBuyEvent", &TaskScript::BindShopBuyEvent)											//added by huchenhui 2017.01.02 商店够买
		.Def("BindFeedPetEvent", &TaskScript::BindFeedPetEvent)											//added by huchenhui 2017.09.26 宠物喂食
		.Def("BindAddPetEvent", &TaskScript::BindAddPetEvent)											//added by huchenhui 2018.03.15 获得宠物
		.Def("BindPetUpLevelEvent", &TaskScript::BindPetUpLevelEvent)									//added by huchenhui 2018.03.21 宠物升级
		.Def("BindAddRelationEvent", &TaskScript::BindAddRelationEvent)									//added by huchenhui 2017.10.23 增加社会关系
		.Def("BindCalculateEquipScoreEvent", &TaskScript::BindCalculateEquipScoreEvent)					//added by huchenhui 2018.03.14 装备评分
		.Def("BindCalculateEquipScoreEvent", &TaskScript::BindAddPreciousBeadEvent)						//added by huchenhui 2018.03.15 镶嵌宝珠
		.Def("BindSendHornEvent", &TaskScript::BindSendHornEvent)										//added by huchenhui 2018.03.17 发送喇叭
		.Def("BindJoinGuildEvent", &TaskScript::BindJoinGuildEvent)										//added by huchenhui 2018.03.20 加入工会
		.Def("BindIncGuildBattleScoreEvent", &TaskScript::BindIncGuildBattleScoreEvent)					//added by huchenhui 2018.06.06 增加工会战积分
		.Def("BindGuildOccupyTerritoryEvent", &TaskScript::BindGuildOccupyTerritoryEvent)				//added by huchenhui 2018.06.07 占领领地
		.Def("BindWinWudaodahuiPkEndEvent", &TaskScript::BindWinWudaodahuiPkEndEvent)					//added by huchenhui 2018.03.20 武道大会
		.Def("BindEnlargePackageEvent", &TaskScript::BindEnlargePackageEvent)							//added by huchenhui 2018.06.11 扩展背包
		.Def("BindEnlargeStorageEvent", &TaskScript::BindEnlargeStorageEvent)							//added by huchenhui 2018.06.11 扩展仓库
		.Def("BindTeamClearDungeonEvent", &TaskScript::BindTeamClearDungeonEvent)						//added by pxr 2018.8.10 组队通关地下城
		.Def("BindTeamClearActivityDungeonEvent", &TaskScript::BindTeamClearActivityDungeonEvent)		//added by pxr 2018.8.10 组队通关活动地下城
		.Def("BindTeamClearHellEvent", &TaskScript::BindTeamClearHellEvent)								//added by pxr 2018.8.10 组队通关深渊地下城
		.Def("BindIncIntimacyEvent", &TaskScript::BindIncIntimacyEvent)
		.Def("BindIncDailyTaskScoreEvent", &TaskScript::BindIncDailyTaskScoreEvent)
		.Def("BindApprentDiscipleEvent", &TaskScript::BindApprentDiscipleEvent)
		.Def("BindDiscipleFinschEvent", &TaskScript::BindDiscipleFinschEvent)
		.Def("BindClearDungeonEventCareScore", &TaskScript::BindClearDungeonEventCareScore)
		.Def("BindGuildEmblemEvent", &TaskScript::BindGuildEmblemEvent)
		.Def("BindClearTeamCopyEvent", &TaskScript::BindClearTeamCopyEvent)
		;

	m_pState->DefClass<ActivityScript>("ActivityScript")
		.Def("BindGetItemEvent", &ActivityScript::BindGetItemEvent)
		.Def("CheckGetItemEvent", &ActivityScript::CheckGetItemEvent)
		.Def("BindDayChange", &ActivityScript::BindDayChange)
		.Def("CheckDayChange", &ActivityScript::CheckDayChange)
		.Def("BindLevelUpEvent", &ActivityScript::BindLevelUpEvent)
		.Def("CheckLevelUpEvent", &ActivityScript::CheckLevelUpEvent)
		.Def("BindTickEvent", &ActivityScript::BindTickEvent)
		.Def("CheckTickEvent", &ActivityScript::CheckTickEvent)
		.Def("BindClearDungeonEvent", &ActivityScript::BindClearDungeonEvent)
		.Def("CheckClearDungeonEvent", &ActivityScript::CheckClearDungeonEvent)
		.Def("BindPkLvUpEvent", &ActivityScript::BindPkLvUpEvent)
		.Def("CheckPkLvUpEvent", &ActivityScript::CheckPkLvUpEvent)
		.Def("BindDeathTower", &ActivityScript::BindDeathTower)
		.Def("CheckDeathTower", &ActivityScript::CheckDeathTower)
		.Def("BindWearEquip", &ActivityScript::BindWearEquip)
		.Def("CheckWearEquip", &ActivityScript::CheckWearEquip)
		.Def("BindWarpStoneUpLevel", &ActivityScript::BindWarpStoneUpLevel)
		.Def("CheckWarpStoneUpLevel", &ActivityScript::CheckWarpStoneUpLevel)
		.Def("BindBuyBlackShop", &ActivityScript::BindBuyBlackShop)
		.Def("CheckBuyBlackShop", &ActivityScript::CheckBuyBlackShop)
		.Def("BindEquipStrenth", &ActivityScript::BindEquipStrenth)
		.Def("CheckEquipStrenth", &ActivityScript::CheckEquipStrenth)
		.Def("BindPk", &ActivityScript::BindPk)
		.Def("CheckPk", &ActivityScript::CheckPk)
		.Def("BindRetinueChangeSkill", &ActivityScript::BindRetinueChangeSkill)
		.Def("CheckRetinueChangeSkill", &ActivityScript::CheckRetinueChangeSkill)
		.Def("BindRetinueUpStarEvent", &ActivityScript::BindRetinueUpStarEvent)
		.Def("CheckRetinueUpStarEvent", &ActivityScript::CheckRetinueUpStarEvent)
		.Def("BindRetinueUpLevelEvent", &ActivityScript::BindRetinueUpLevelEvent)
		.Def("CheckRetinueUpStarEvent", &ActivityScript::CheckRetinueUpLevelEvent)
		.Def("BindWearFashionEquip", &ActivityScript::BindWearFashionEquip)
		.Def("CheckWearFashionEquip", &ActivityScript::CheckWearFashionEquip)
		.Def("BindOpenJar", &ActivityScript::BindOpenJar)
		.Def("CheckOpenJar", &ActivityScript::CheckOpenJar)
		.Def("BindAddMagic", &ActivityScript::BindAddMagic)
		.Def("CheckAddMagic", &ActivityScript::CheckAddMagic)
		.Def("BindAddClearHell", &ActivityScript::BindAddClearHell)
		.Def("CheckAddClearHell", &ActivityScript::CheckAddClearHell)
		.Def("BindGetTitleEvent", &ActivityScript::BindGetTitleEvent)
		.Def("CheckGetTitleEvent", &ActivityScript::CheckGetTitleEvent)
		.Def("BindCharge", &ActivityScript::BindCharge)
		.Def("BindRandQL", &ActivityScript::BindRandQL)
		.Def("BindShopBuyEvent", &ActivityScript::BindShopBuyEvent)
		.Def("BindBuyMallItemEvent", &ActivityScript::BindBuyMallItemEvent)
		;
	/*
	m_pState->DefClass<Task>("Task")
		.Def("GetID",&Task::GetID)
		.Def("GetType",&Task::GetType)
		.Def("SetStatus",&Task::SetStatus)
		.Def("GetStatus",&Task::GetStatus)
		.Def("GetVar",&Task::GetVar)
		.Def("SetVar",&Task::SetVar)
		.Def("GetQuality",&Task::GetQuality)
		.Def("GetNameVar",&Task::GetNameVar)
		.Def("SetNameVar",&Task::SetNameVar)
		.Def("AddTaskItem",&Task::AddTaskItem);
	*/

	m_pState->DefClass<BaseTask>("BaseTask")
		//.Def("GetID", &BaseTask::GetID) 
		.Def("GetID", &BaseTask::GetDataId)
		.Def("GetType", &BaseTask::GetType)
		.Def("SetStatus", &BaseTask::SetStatus)
		.Def("GetStatus", &BaseTask::GetStatus)
		.Def("GetVar", &BaseTask::GetVar)
		.Def("SetVar", &BaseTask::SetVar)
		.Def("GetQuality", &BaseTask::GetQuality)
		.Def("GetNameVar", &BaseTask::GetNameVar)
		.Def("SetNameVar", &BaseTask::SetNameVar)
		.Def("AddTaskItem", &BaseTask::AddTaskItem)
		.Def("GetRewardsReason", &BaseTask::GetRewardsReason)
		;

	m_pState->DefClass<Task, BaseTask>("Task")
		;

	m_pState->DefClass<ActiveTask>("ActiveTask")
		.Def("GetID", &ActiveTask::GetID)
		.Def("SetStatus", &ActiveTask::SetStatus)
		.Def("SetStatusNoSync", &ActiveTask::SetStatusNoSync)
		.Def("GetStatus", &ActiveTask::GetStatus)
		.Def("GetVar", &ActiveTask::GetVar)
		.Def("SetVar", &ActiveTask::SetVar)
		.Def("SetVarNoSync", &ActiveTask::SetVarNoSync)
		.Def("GetNameVar", &ActiveTask::GetNameVar)
		.Def("SetNameVar", &ActiveTask::SetNameVar)
		.Def("AddTaskItem", &ActiveTask::AddTaskItem);

	m_pState->DefClass<Item>("Item")
		.Def("GetQuality", &Item::GetQuality)
		.Def("GetBind", &Item::GetBind)
		.Def("GetDataID", &Item::GetDataID)
		.Def("SetParam1", &Item::SetParam1)
		.Def("GetParam1", &Item::GetParam1)
		.Def("SetParam2", &Item::SetParam2)
		.Def("GetParam2", &Item::GetParam2)
		//.Def("GetEffectParam", &Item::GetEffectParam)
		.Def("AddRandAttr", &Item::AddRandAttr)
		.Def("SyncProperty", &Item::ScriptSyncProperty)
		.Def("GetNeedLevel", &Item::GetNeedLevel)
		;

	m_pState->DefClass<Scene>("Scene")
		.Def("GetMapID", &Scene::GetMapID)
		.Def("GetID", &Scene::GetID)
		.Def("SetName", &Scene::ScriptSetName)
		.Def("GetName", &Scene::ScriptGetName)
		.Def("GetType", &Scene::GetType)
		.Def("FindPlayer", &Scene::FindPlayer)
		.Def("FindCreature", &Scene::FindCreature)
		.Def("VisitPlayers", &Scene::ScriptVisitPlayers)
		.Def("BroadcastNotify", &Scene::BroadcastNotify)
		.Def("UnLoad", &Scene::UnLoad)
		.Def("ClearItems", &Scene::ClearItems)
		.Def("GetLevel", &Scene::GetLevel)
		.Def("GetPlayerNum", &Scene::GetPlayerNum)
		.Def("GetMaxPlayerNum", &Scene::GetMaxPlayerNum)
		.Def("GetWaitPlayerNum", &Scene::GetWaitPlayerNum)
		.Def("KickPlayersOut", &Scene::KickPlayersOut)
		.Def("KickPlayerOut", &Scene::KickPlayerOut)
		.Def("GetWidth", &Scene::GetWidth)
		.Def("GetLength", &Scene::GetLength)
		;
	m_pState->DefClass<PlayerScript>("PlayerScript")
		.Def("SetDailyTaskNpc", &PlayerScript::SetDailyTaskNpc)
		.Def("SetForbidPattern", &PlayerScript::SetForbidPattern)
		.Def("SetExchangeItem", &PlayerScript::SetExchangeItem)
		.Def("AddEquipShopPrice", &PlayerScript::AddEquipShopPrice)
		.Def("AddEquipShopRefreshTime", &PlayerScript::AddEquipShopRefreshTime)
		;

	m_pState->DefClass<FinalEquipScript>("FinalEquipScript")
		.Def("AddEffect", &FinalEquipScript::AddEffect);

	m_pState->DefClass<EscortScript>("EscortScript")
		.Def("SetRefreshRate",	&EscortScript::SetRefreshRate)
		.Def("SetIncRefreshRate",	&EscortScript::SetIncRefreshRate);

	m_pState->DefClass<ScriptVector64>("ScriptVector64")
		.Def("Size", &ScriptVector64::Size)
		.Def("Get", &ScriptVector64::Get);

}

