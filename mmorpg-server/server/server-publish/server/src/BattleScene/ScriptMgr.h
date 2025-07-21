#ifndef _SCRIPT_SYSTEM_H_
#define _SCRIPT_SYSTEM_H_

#include <AvalonSingleton.h>
#include <AvalonScriptState.h>
#include <map>
#include <vector>
#include <CLScriptDefine.h>
#include <AvalonScriptFile.h>
#include <CLScriptMgr.h>

class TaskScript;
class ItemScript;
class SceneScript;
class PlayerScript;
class FinalEquipScript;
class EquipCalcScript;
class ActivityScript;

enum SScriptType
{
	// 文件夹下的脚本
	SCENE_SCRIPT_TASK,
	SCENE_SCRIPT_ITEM,
	SCENE_SCRIPT_ACTIVITY,

	// 单个脚本
	SCENE_SCRIPT_COUNTER,
	SCENE_SCRIPT_EQUIPCALC,
	SCENE_SCRIPT_PLAYER,
	SCENE_SCRIPT_LOST_DUNGEON_TASK,

	SCENE_SCRIPT_NUM,

	//未用到的
	SCENE_SCRIPT_SCENE,
	SCENE_SCRIPT_AI,
	SCENE_SCRIPT_FINALEQUIP,
	SCENE_SCRIPT_AIBIND,
	
};

/**
 *@brief 脚本系统 管理所有脚本
 */
class ScriptMgr : public CLScriptMgr, public Avalon::Singleton<ScriptMgr>
{
	typedef std::map<UInt32, UInt32>	AIBindMap;

public:
	typedef std::map<UInt32, TaskScript*>	TaskScriptMap;
	typedef std::map<UInt32, ItemScript*>	ItemScriptMap;
	typedef std::map<UInt32, SceneScript*>	SceneScriptMap;
	typedef std::map<UInt32, ActivityScript*> ActivityScriptMap;

	typedef std::vector<TaskScript*>		TaskScriptVec;
	typedef std::map<UInt32,TaskScriptVec>	SceneTaskMap;

	typedef std::map<UInt32, Avalon::ScriptFile*>	ScriptMap;
	
public:
	ScriptMgr();
	~ScriptMgr();

public:
	virtual bool Init();
	virtual void Final();

	/**
	 *@brief 重新加载脚本
	 */
	bool ReloadScripts();

public:
	/**
	 *@brief 绑定npc ai
	 */
	void BindAI(UInt32 id1, UInt32 id2);

	/**
	 *@brief 获取堆栈大小                                                                     
	 */
	int GetStackSize() const{ return m_pState->GetStackSize(); }

public:
	/**
	 *@brief 获取task脚本
	 */
	TaskScript* FindTaskScript(UInt32 id);

	/**
	 *@brief 获取爬塔任务task脚本
	 */
	TaskScript* FindLostDungeonTaskScript(UInt32 id);

	/**
	*@brief 获取activity脚本
	*/
	ActivityScript* FindActivityScript(UInt32 id);

	/**
	 *@brief 获取item脚本
	 */
	ItemScript* FindItemScript(UInt32 id);

	/**
	 *@brief 获取scene脚本
	 */
	SceneScript* FindSceneScript(UInt32 id);

	/**
	 *@brief 根据地图id获取触发任务列表
	 */
	const TaskScriptVec& GetMapTriggerTasks(UInt32 mapid) const;

	/** 
	 *@brief 获取玩家脚本
	 */
	PlayerScript* GetPlayerScript() const;

	/**
	*@brief 获得终极装备脚本
	*/
	FinalEquipScript* GetFinalEquipScript() const;

	/**
	*@brief 获得装备计算脚本
	*/
	EquipCalcScript *GetEquipCalcScript() const;
	
private:
	/**
	 *@brief 注册类和函数
	 */
	void DefClasses();

	/**
	 *@brief 加载脚本
	 */
	bool LoadScripts();

	/**
	*@brief 加载脚本
	*/
	bool LoadScript(UInt32 type);

	/**
	*@brief 加载任务脚本
	*/
	bool LoadTaskScript();

	/**
	*@brief 加载脚本完成之后
	*/
	virtual void OnLoadScriptFinish(UInt32 type, Avalon::ScriptFile* script);

	/**
	*@brief 创建脚本对象
	*/
	Avalon::ScriptFile* CreateScript(UInt32 type, Avalon::ScriptState* state, UInt32 id = 0);

	/**
	*@brief 加载爬塔任务脚本
	*/
	bool LoadLostDungeonTaskScript();

private:
#ifndef _NEW_SCRIPT_MGR_
	//luastate
	Avalon::ScriptState*	m_pState;
	//npc脚本
	NpcScriptMap	m_NpcScripts;
	//task脚本
	TaskScriptMap	m_TaskScripts;
	//item脚本
	ItemScriptMap	m_ItemScripts;
	//scene脚本
	SceneScriptMap	m_SceneScripts;
	
	//ai脚本
	AIScriptMap		m_AIScripts;
	//交通工具ai脚本
	VehicleAIScriptMap	m_VehicleAIScripts;
	//玩家脚本
	PlayerScript*	m_pPlayerScript;
	//领土奖励效果脚本
	TerritoryScript	*m_pTerritoryScript;
	//终极装备脚本
	FinalEquipScript	*m_pFinalEquipScript;
	//押镖脚本
	EscortScript	*m_pEscortScript;
	//活动礼包脚本
	ActivityGiftbagScript	*m_pActivityGiftbagScript;
	//装备计算脚本
	EquipCalcScript	*m_pEquipCalcScript;
	//活动脚本
	ActivityScriptMap m_ActivityScripts;
#else
	//ai绑定
	AIBindMap		m_AIBindMap;

	//场景区域触发任务列表
	SceneTaskMap			m_SceneTasks;

	// 脚本配置
	static ScriptConfig		s_scriptConfigs[SCENE_SCRIPT_NUM];
#endif
};


#endif
