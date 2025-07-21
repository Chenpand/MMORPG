#include "ScriptMgr.h"

#include <CLSysNotify.h>
#include <CLDungeonDataEntry.h>

#include "SysInterface.h"
#include "WSPlayer.h"
#include "WSScene.h"
#include "TimerScript.h"
#include "SortlistActivityScript.h"
#include "MallScript.h"
#include "WorldPlayerScript.h"
#include "WSSceneMgr.h"
#include "AnnouncementMgr.h"
#include "AccountInfo.h"

ScriptConfig ScriptMgr::s_scriptConfigs[] = {
	{ ACCOUNT_TASK_SCRIPT_PATH, "task_", NULL, true, false },
	{ COUNTER_SCRIPT_PATH,		NULL,			"Counter",		false,			true },
	{ WORLD_TIMER_SCRIPT_PATH,	NULL,			"WorldTimer",	false,			false },
//	{ MALL_SCRIPT_PATH,			NULL,			"Mall",			false,			false },
	{ WORLD_CONFIG_SCRIPT_PATH, NULL,			"WorldConfig",	false,			true },

// 	{ BATTLESCENE_SCRIPT_PATH, 	NULL,			"BattleScene",	false,			false },
// 	{ WORLD_PLAYER_SCRIPT_PATH, NULL,			"WorldPlayer",	false,			false },
// 	
	
};

ScriptMgr::ScriptMgr()
	: CLScriptMgr(WORLD_SCRIPT_NUM)
{
#ifndef _NEW_SCRIPT_MGR_
	m_pState = new Avalon::ScriptState();

	m_pTimerScript = NULL;
	m_pBattleSceneScript = NULL;
	m_pGSActivityScript = NULL;
	m_pMallScript = NULL;
	m_pPlayerScript = NULL;
#else
	m_pGSActivityScript = NULL;
#endif
}

ScriptMgr::~ScriptMgr()
{
#ifndef _NEW_SCRIPT_MGR_
	Final();

	SysNotifyMgr::Destroy();

	CL_SAFE_DELETE(m_pState);
#else
#endif
}

bool ScriptMgr::Init()
{
	DefClasses();

	Avalon::ScriptState::SetErrorHandler(new CLScriptErrorHandler());

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

	for(TaskScriptMap::iterator iter = m_TaskScripts.begin(); iter != m_TaskScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_TaskScripts.clear();

	CL_SAFE_DELETE(m_pTimerScript);
	CL_SAFE_DELETE(m_pBattleSceneScript);
	CL_SAFE_DELETE(m_pGSActivityScript);
	CL_SAFE_DELETE(m_pMallScript);
	CL_SAFE_DELETE(m_pPlayerScript);
#else
	CL_SAFE_DELETE(m_pGSActivityScript);
#endif
}

bool ScriptMgr::LoadScripts()
{
	std::string configpath = CONFIG_PATH;

	std::string packagePath = configpath + PACKAGE_PATH;
	m_pState->AddPackagePath(packagePath.c_str());
	Avalon::ScriptFile sandBoxScript(m_pState);
	std::string sandboxPath = configpath + SANDBOX_SCRIPT_PATH;
	if(!sandBoxScript.Load(sandboxPath.c_str()))
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

	Avalon::ScriptFile counterScript(m_pState);
	m_pState->MakeEnv("Counter", "SetSandBox");//为将要加载的脚本中的函数设置新环境，新环境表以"Counter"为key，存放在Register表中，脚本函数"SetSandBox"是具体构建新环境表的，其参数t就是新环境表，底层通过调用lua_tinker::make_env把新环境表存储在Register注册表中(以"Counter"为key)
	std::string counterPath = configpath + COUNTER_SCRIPT_PATH;
	if(!counterScript.Load(counterPath.c_str(), "Counter"))//底层lua_tinker::dofile调用时就以"Counter"为key 在注册表中获得了新环境表，并调用lua_setfenv设置为加载的脚本中的函数的新环境
	{
		ErrorLogStream << "load counter script failed!" << LogStream::eos;
		return false;
	}
	counterScript.Call<void>("Init");

	m_pTimerScript = new TimerScript(m_pState);
	m_pState->MakeEnv("WorldTimer", "SetSandBox");
	std::string timerPath = configpath + WORLD_TIMER_SCRIPT_PATH;
	if(!m_pTimerScript->Load(timerPath.c_str(), "WorldTimer"))
	{
		CL_SAFE_DELETE(m_pTimerScript);
		ErrorLogStream << "load world timer script failed!" << LogStream::eos;
		return false;
	}

	m_pBattleSceneScript = new Avalon::ScriptFile(m_pState);
	m_pState->MakeEnv("BattleScene", "SetSandBox");
	std::string battleScenePath = configpath + BATTLESCENE_SCRIPT_PATH;
	if(!m_pBattleSceneScript->Load(battleScenePath.c_str(), "BattleScene"))
	{
		CL_SAFE_DELETE(m_pBattleSceneScript);
		ErrorLogStream << "load battlescene script failed!" << LogStream::eos;
		return false;
	}

	m_pMallScript = new MallScript(m_pState);
	m_pState->MakeEnv("Mall", "SetSandBox");
	std::string mallPath = configpath + MALL_SCRIPT_PATH;
	if(!m_pMallScript->Load(mallPath.c_str(), "Mall"))
	{
		CL_SAFE_DELETE(m_pMallScript);
		ErrorLogStream << "load mall script failed!" << LogStream::eos;
		return false;
	}

	m_pPlayerScript = new WorldPlayerScript(m_pState);
	m_pState->MakeEnv("WorldPlayer", "SetSandBox");
	std::string playerPath = configpath + WORLD_PLAYER_SCRIPT_PATH;
	if(!m_pPlayerScript->Load(playerPath.c_str(), "WorldPlayer"))
	{
		CL_SAFE_DELETE(m_pPlayerScript);
		ErrorLogStream << "load player script failed!" << LogStream::eos;
		return false;
	}
	m_pPlayerScript->Init();
#else
	for (UInt32 i = 0; i < WORLD_SCRIPT_NUM; i++)
	{
		if (!LoadScript((WScriptType)i))
		{
			return false;
		}
	}
#endif

	return true;
}

bool ScriptMgr::LoadGSActivityScript(UInt32 uVerTime)
{
// 	std::string configpath = CONFIG_PATH;
// 	std::string	strScriptName;
// 	if(uVerTime > GAMESTART_TIME)
// 		strScriptName = configpath + GSACTIVITY_SCRIPT_PATH;
// 	else
// 		strScriptName = configpath + NEW_GSACTIVITY_SCRIPT_PATH;
// 	
// 	m_pGSActivityScript = new SortlistActivityScript(m_pState);
// 	m_pState->MakeEnv("SortlistActivity", "SetSandBox");
// 	if(!m_pGSActivityScript->Load(strScriptName.c_str(), "SortlistActivity"))
// 	{
// 		CL_SAFE_DELETE(m_pGSActivityScript);
// 		ErrorLogStream << "load sortlist activity script failed!" << LogStream::eos;
// 		return false;
// 	}
// 		
// 	DebugLogStream << "GSNewVerTime is " << uVerTime <<  ".GameStartTime is " << GAMESTART_TIME << LogStream::eos;
	return true;
}

void ScriptMgr::OnTick(const Avalon::Time& now)
{
#ifndef _NEW_SCRIPT_MGR_
	if(m_pTimerScript != NULL)
	{
		m_pTimerScript->OnTick(now);
	}
#else
	TimerScript* timerScript = GetSingleScript<TimerScript>(WORLD_SCRIPT_TIMER);
	if (timerScript)
	{
		timerScript->OnTick(now);
	}
#endif
}

bool ScriptMgr::ReloadScripts()
{
#ifndef _NEW_SCRIPT_MGR_
	Avalon::ScriptState* oldState = m_pState;

	TimerScript* oldTimerScript = m_pTimerScript;
	m_pTimerScript = NULL;
	Avalon::ScriptFile* oldBattleSceneScript = m_pBattleSceneScript;
	m_pBattleSceneScript = NULL;
	SortlistActivityScript	*oldGSActivityScript = m_pGSActivityScript;
	m_pGSActivityScript = NULL;
	MallScript	*pOldMallScript = m_pMallScript;
	m_pMallScript = NULL;
	WorldPlayerScript *pOldPlayerScript = m_pPlayerScript;
	m_pPlayerScript = NULL;

	TaskScriptMap oldTaskScripts = m_TaskScripts;
	m_TaskScripts.clear();

	m_pState = new Avalon::ScriptState();

	DefClasses();

	if(!LoadScripts() || !LoadGSActivityScript(oldTimerScript->GetGSNewVerTime()))
	{
		Final();
		CL_SAFE_DELETE(m_pState);

		m_pState = oldState;
		m_pTimerScript = oldTimerScript;
		m_pBattleSceneScript = oldBattleSceneScript;
		m_pGSActivityScript = oldGSActivityScript;
		m_pMallScript = pOldMallScript;
		m_pPlayerScript = pOldPlayerScript;
		m_TaskScripts = oldTaskScripts;

		return false;
	}

	CL_SAFE_DELETE(oldState);
	CL_SAFE_DELETE(oldTimerScript);
	CL_SAFE_DELETE(oldBattleSceneScript);
	CL_SAFE_DELETE(oldGSActivityScript);
	CL_SAFE_DELETE(pOldMallScript);
	CL_SAFE_DELETE(pOldPlayerScript);

	for(TaskScriptMap::iterator iter = oldTaskScripts.begin(); iter != oldTaskScripts.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	oldTaskScripts.clear();
	
	m_pMallScript->Init();
	m_pTimerScript->Init();
	// 排行榜活动脚本的init必须在timer脚本后
	m_pGSActivityScript->Init();
#else
	m_pState = new Avalon::ScriptState();

	DefClasses();

	for (UInt32 i = 0; i < WORLD_SCRIPT_NUM; i++)
	{
		if (!ReloadScript(i))
		{
			return false;
		}
	}

// 	SortlistActivityScript	*oldGSActivityScript = m_pGSActivityScript;
// 	m_pGSActivityScript = NULL;
// 	if (!LoadGSActivityScript(GetTimerScript()->GetGSNewVerTime()))
// 	{
// 		m_pGSActivityScript = oldGSActivityScript;
// 		return false;
// 	}
// 
// 	CL_SAFE_DELETE(oldGSActivityScript);
// 	m_pGSActivityScript->Init();

	return true;
#endif
}

TimerScript* ScriptMgr::GetTimerScript() const
{
#ifndef _NEW_SCRIPT_MGR_
	return m_pTimerScript; 
#else
	return GetSingleScript<TimerScript>(WORLD_SCRIPT_TIMER);
#endif
}

Avalon::ScriptFile* ScriptMgr::GetBattleSceneScript() const
{ 
#ifndef _NEW_SCRIPT_MGR_
	return m_pBattleSceneScript; 
#else
	//return GetSingleScript<Avalon::ScriptFile>(WORLD_SCRIPT_BATTLE_SCENE);
	return NULL;
#endif
}

MallScript* ScriptMgr::GetMallScript() const 
{ 
#ifndef _NEW_SCRIPT_MGR_
	return m_pMallScript; 
#else
	//return GetSingleScript<MallScript>(WORLD_SCRIPT_MALL);
	return NULL;
#endif
}

WorldPlayerScript* ScriptMgr::GetPlayerScript() const
{ 
#ifndef _NEW_SCRIPT_MGR_
	return m_pPlayerScript; 
#else
	//return GetSingleScript<WorldPlayerScript>(WORLD_SCRIPT_PLAYER);
	return NULL;
#endif
}

TaskScript* ScriptMgr::FindTaskScript(UInt32 id)
{
#ifndef _NEW_SCRIPT_MGR_
	TaskScriptMap::iterator iter = m_TaskScripts.find(id);
	if (iter != m_TaskScripts.end()) return iter->second;
	return NULL;
#else
	return GetScript<TaskScript>(WORLD_SCRIPT_TASK, id);
#endif
}

void ScriptMgr::DefClasses()
{
	m_pState->DefClass<SysInterface>("SysClass")
		.Def("RegCounter", &SysInterface::RegCounter)
		.Def("LogError", &SysInterface::LogError)
		.Def("LogInfo", &SysInterface::LogInfo)
		.Def("LogDebug", &SysInterface::LogDebug)
		.Def("LogTrace", &SysInterface::LogTrace)
		.Def("LogFatal", &SysInterface::LogFatal)
		.Def("MakeTime", &SysInterface::MakeTime)
		.Def("Now", &SysInterface::Now)
		.Def("Year", &SysInterface::Year)
		.Def("Month", &SysInterface::Month)
		.Def("Day", &SysInterface::Day)
		.Def("Hour", &SysInterface::Hour)
		.Def("Min", &SysInterface::Min)
		.Def("Sec", &SysInterface::Sec)
		.Def("WeekDay", &SysInterface::WeekDay)
		.Def("TimeOfDay", &SysInterface::TimeOfDay)
		.Def("RandBetween", &SysInterface::RandBetween)
		.Def("FindScene", &SysInterface::FindScene)
		.Def("SelectDynSceneServer", &SysInterface::SelectDynSceneServer)
		.Def("CreatePublicDynScene", &SysInterface::CreatePublicDynScene)
		.Def("BroadcastNotify", &SysInterface::BroadcastNotify)
		.Def("BroadcastNotifyByLvl", &SysInterface::BroadcastNotifyByLvl)
		.Def("GetRewardGroup", &SysInterface::GetRewardGroup)
		.Def("AddReward", &SysInterface::AddReward)
		.Def("SendSysMail", &SysInterface::SendSysMail)
		.Def("ClearActivityRegInfos", &SysInterface::ClearActivityRegInfos)
		.Def("RegDailyActivity", &SysInterface::RegisterDailyActivity)
		.Def("RegOnceActivity", &SysInterface::RegisterOnceActivity)
		.Def("RegOpenServerOnceActivity", &SysInterface::RegisterOpenServerOnceActivity)
		.Def("RegWeekActivity", &SysInterface::RegisterWeekActivity)
		.Def("RegMonthActivity", &SysInterface::RegisterMonthActivity)
		.Def("RegCustomCircleActivity", &SysInterface::RegisterCustomCircleActivity)
		.Def("RegGSActivity", &SysInterface::RegisterGSActivity)
		.Def("RegGSActivityEndTime", &SysInterface::RegisterGSActivityEndTime)
		.Def("RegGSActivityIntervalTime", &SysInterface::RegisterGSActivityIntervalTime)
		.Def("RegGSActivityIntervalDay", &SysInterface::RegisterGSActivityIntervalDay)
		.Def("RegConsistActivity", &SysInterface::RegisterConsistActivity)
		.Def("SetActGameStartLimit", &SysInterface::SetActGameStartLimit)
		.Def("CloseActivity", &SysInterface::CloseActivity)
		.Def("IsInActivity", &SysInterface::IsInActivity)
		.Def("AddGlobalActivity", &SysInterface::AddGlobalActivity)
		.Def("ClearSortList", &SysInterface::ClearSortList)
		.Def("OnNewYearRedPacketEnd", &SysInterface::OnNewYearRedPacketEnd)
		.Def("SelectCounterValue", &SysInterface::SelectCounterValue)
		.Def("VisitSortList", &SysInterface::VisitSortList)
		.Def("RegisterNpc", &SysInterface::RegisterNpc)
		.Def("GetGamevalue", &SysInterface::GetGameValue)
		.Def("SetGamevalue", &SysInterface::SetGameValue)
		.Def("SyncActivityCounter", &SysInterface::SyncActivityCounter)
		.Def("VisitPlayer", &SysInterface::VisitPlayer)
		.Def("RegisterNpc", &SysInterface::RegisterNpc)
		.Def("BroadcastEffectEvent", &SysInterface::BroadcastEffectEvent)
		.Def("GetZoneId", &SysInterface::GetZoneId)
		.Def("GetNeutralZoneId", &SysInterface::GetNeutralZoneId)
		.Def("GetPlatform", &SysInterface::GetPlatform)
		.Def("SendAnnouncementNow", &SysInterface::SendAnnouncementNow)
		.Def("SendAnnouncement", &SysInterface::SendAnnouncement)
		.Def("PublicAnnouncementList", &SysInterface::PublicAnnouncementList)
		.Def("RemoveAnnouncement", &SysInterface::RemoveAnnouncement)
		.Def("SetActivityMonsterOpen", &SysInterface::SetActivityMonsterOpen)
		.Def("OpenService", &SysInterface::OpenService)
		.Def("CloseService", &SysInterface::CloseService)
		.Def("SetAutoBattleMode", &SysInterface::SetAutoBattleMode)
		.Def("ChangeGuildBattleStatus", &SysInterface::ChangeGuildBattleStatus)
		.Def("OnSaleMallWaitGoods", &SysInterface::OnSaleMallWaitGoods)
		.Def("OffSaleMallWaitGoods", &SysInterface::OffSaleMallWaitGoods)
		.Def("SummonCityMonster", &SysInterface::SummonCityMonster)
		.Def("ChijiOpen", &SysInterface::ChijiOpen)
		.Def("GoldConsignmentClose", &SysInterface::GoldConsignmentClose)
		;
	m_pState->Set("Sys", &sys);

	m_pState->DefClass<TimerScript>("TimerScript")
		.Def("RegisterTimerEvent", &TimerScript::RegisterTimerEvent)
		.Def("RegisterHourEvent", &TimerScript::RegisterHourEvent)
		.Def("RegisterDailyEvent", &TimerScript::RegisterDailyEvent)
		.Def("RegisterWeeklyEvent", &TimerScript::RegisterWeeklyEvent)
		.Def("SetGSNewVerTime", &TimerScript::SetGSNewVerTime);

	m_pState->DefClass<WSPlayer>("WSPlayer")
		.Def("Notify", &WSPlayer::Notify)
		.Def("GetID", &WSPlayer::GetID)
		.Def("GetLevel", &WSPlayer::GetLevel)
		.Def("GetSrcZoneId", &WSPlayer::GetSrcZoneId)
		;

	m_pState->DefClass<TaskScript>("TaskScript")
		.Def("GetScriptParam", &TaskScript::GetScriptParam)
		.Def("BindKillNpcEvent", &TaskScript::BindKillNpcEvent)
		.Def("BindKillNpcTypeEvent", &TaskScript::BindKillNpcTypeEvent)
		.Def("BindAdventureTeamLevelChangedEvent", &TaskScript::BindAdventureTeamLevelChangedEvent)
		.Def("BindAdventureTeamGradeChangedEvent", &TaskScript::BindAdventureTeamGradeChangedEvent)
		.Def("BindPlayerClearDungeonEvent", &TaskScript::BindPlayerClearDungeonEvent)
		.Def("BindPlayerClearHellEvent", &TaskScript::BindPlayerClearHellEvent)
		.Def("BindPlayerClearCityMonsterEvent", &TaskScript::BindPlayerClearCityMonsterEvent)
		.Def("BindPlayerRemoveFatigueEvent", &TaskScript::BindPlayerRemoveFatigueEvent)
		.Def("BindRoleNumChangedEvent", &TaskScript::BindRoleNumChangedEvent)
		.Def("BindPlayerCompAdventWeeklyTaskEvent", &TaskScript::BindPlayerCompAdventWeeklyTaskEvent)
		.Def("BindSetTaskItemEvent", &TaskScript::BindSetTaskItemEvent)
		.Def("BindSetTaskItemByIdEvent", &TaskScript::BindSetTaskItemByIdEvent)
		;

	m_pState->DefClass<AccountTask>("AccountTask")
		//.Def("GetID", &BaseTask::GetID) 
		.Def("GetID", &AccountTask::GetDataId)
		.Def("GetType", &AccountTask::GetType)
		.Def("SetStatus", &AccountTask::SetStatus)
		.Def("GetStatus", &AccountTask::GetStatus)
		.Def("GetVar", &AccountTask::GetVar)
		.Def("SetVar", &AccountTask::SetVar)
		.Def("GetQuality", &AccountTask::GetQuality)
		.Def("SetNameVar", &AccountTask::SetNameVar)
		.Def("GetRewardsReason", &AccountTask::GetRewardsReason)
		.Def("GetScriptParm", &AccountTask::GetScriptParm)
		;

	m_pState->DefClass<AccountInfo>("AccountInfo")
		.Def("GetAdventureTeamLevel", &AccountInfo::GetAdventureTeamLevel)
		.Def("GetAdventureTeamGrade", &AccountInfo::GetAdventureTeamGrade)
		.Def("GetOverAdventureTeamTasks", &AccountInfo::GetOverAdventureTeamTasks)
		;

	m_pState->DefClass<DungeonDataEntry>("DungeonDataEntry")
		.Mem("dungeonId", &DungeonDataEntry::id)
		.Mem("dungeonType", &DungeonDataEntry::type)
		.Mem("dungeonSubType", &DungeonDataEntry::subType)
		.Mem("dungeonHardType", &DungeonDataEntry::hardType)
		;

	m_pState->DefClass<WSScene>("WSScene")
		.Def("GetName", &WSScene::ScriptGetName)
		.Def("PullPlayer", &WSScene::PullPlayer)
		.Def("GetMapID", &WSScene::GetMapID)
		.Def("SetParam", &WSScene::SetParam);
	 
	m_pState->DefClass<MallScript>("MallScript")
		.Def("AddNormalLimitItem", &MallScript::AddNormalLimitItem)
		.Def("AddGSLimitItem", &MallScript::AddGSLimitItem)
		.Def("AddFestivalLimitItem", &MallScript::AddFestivalLimitItem)
		.Def("AddGoldStoneItem", &MallScript::AddGoldStoneItem)
		.Def("AddSilverStoneItem", &MallScript::AddSilverStoneItem)
		.Def("SetCurLimitedItems", &MallScript::SetCurLimitedItems)
		.Def("AddMallItem", &MallScript::AddMallItem)
		;

	m_pState->DefClass<WorldPlayerScript>("WorldPlayerScript")
		.Def("RegOfflineNotify", &WorldPlayerScript::RegOfflineNotify);

	m_pState->DefFunc("ActivityNotice", &AnnouncementMgr::PublicActivityAnnouncement);
}

bool ScriptMgr::LoadScript(UInt32 type)
{
	if (type >= sizeof(s_scriptConfigs) / sizeof(s_scriptConfigs[0]))
	{
		return false;
	}

	if (WScriptType::WORLD_SCRIPT_TASK == (WScriptType)type)
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
	WScriptType type = WScriptType::WORLD_SCRIPT_TASK;

	auto& scriptMgr = GetScriptMgr(type);
	std::string postfix = ".lua";

	class TaskScriptVisiotr : public Avalon::DataEntryVisitor<TaskDataEntry>
	{
	public:
		TaskScriptVisiotr(){}
		~TaskScriptVisiotr(){}

		bool operator()(TaskDataEntry* entry)
		{
			if (entry->open && IsAccountTask(entry->type))
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
			++iter;
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

}

Avalon::ScriptFile* ScriptMgr::CreateScript(UInt32 type, Avalon::ScriptState* state, UInt32 id)
{
// 	switch (type)
// 	{
// 	case WORLD_SCRIPT_TIMER:
// 		return new TimerScript(state);
// 		break;
// 	case WORLD_SCRIPT_MALL:
// 		return new MallScript(state);
// 		break;
// 	case WORLD_SCRIPT_PLAYER:
// 		return new WorldPlayerScript(state);
// 		break;
// 	case WORLD_SCRIPT_BATTLE_SCENE:
// 	case WORLD_SCRIPT_COUNTER:
// 	case WORLD_SCRIPT_CONFIG:
// 		return new Avalon::ScriptFile(state);
// 		break;
// 	default:
// 		break;
// 	}
// 
// 	return NULL;

	switch (type)
	{
	case WORLD_SCRIPT_TASK:
		return new TaskScript(id, state);
	case WORLD_SCRIPT_TIMER:
		return new TimerScript(state);
		break;
	case WORLD_SCRIPT_MALL:
		return new MallScript(state);
		break;
	case WORLD_SCRIPT_COUNTER:
	case WORLD_SCRIPT_CONFIG:
		return new Avalon::ScriptFile(state);
		break;
	default:
		break;
	}

	return NULL;
}