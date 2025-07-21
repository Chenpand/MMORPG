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
	// �ļ����µĽű�
	SCENE_SCRIPT_TASK,
	SCENE_SCRIPT_ITEM,
	SCENE_SCRIPT_ACTIVITY,

	// �����ű�
	SCENE_SCRIPT_COUNTER,
	SCENE_SCRIPT_EQUIPCALC,
	SCENE_SCRIPT_PLAYER,
	SCENE_SCRIPT_LOST_DUNGEON_TASK,

	SCENE_SCRIPT_NUM,

	//δ�õ���
	SCENE_SCRIPT_SCENE,
	SCENE_SCRIPT_AI,
	SCENE_SCRIPT_FINALEQUIP,
	SCENE_SCRIPT_AIBIND,
	
};

/**
 *@brief �ű�ϵͳ �������нű�
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
	 *@brief ���¼��ؽű�
	 */
	bool ReloadScripts();

public:
	/**
	 *@brief ��npc ai
	 */
	void BindAI(UInt32 id1, UInt32 id2);

	/**
	 *@brief ��ȡ��ջ��С                                                                     
	 */
	int GetStackSize() const{ return m_pState->GetStackSize(); }

public:
	/**
	 *@brief ��ȡtask�ű�
	 */
	TaskScript* FindTaskScript(UInt32 id);

	/**
	 *@brief ��ȡ��������task�ű�
	 */
	TaskScript* FindLostDungeonTaskScript(UInt32 id);

	/**
	*@brief ��ȡactivity�ű�
	*/
	ActivityScript* FindActivityScript(UInt32 id);

	/**
	 *@brief ��ȡitem�ű�
	 */
	ItemScript* FindItemScript(UInt32 id);

	/**
	 *@brief ��ȡscene�ű�
	 */
	SceneScript* FindSceneScript(UInt32 id);

	/**
	 *@brief ���ݵ�ͼid��ȡ���������б�
	 */
	const TaskScriptVec& GetMapTriggerTasks(UInt32 mapid) const;

	/** 
	 *@brief ��ȡ��ҽű�
	 */
	PlayerScript* GetPlayerScript() const;

	/**
	*@brief ����ռ�װ���ű�
	*/
	FinalEquipScript* GetFinalEquipScript() const;

	/**
	*@brief ���װ������ű�
	*/
	EquipCalcScript *GetEquipCalcScript() const;
	
private:
	/**
	 *@brief ע����ͺ���
	 */
	void DefClasses();

	/**
	 *@brief ���ؽű�
	 */
	bool LoadScripts();

	/**
	*@brief ���ؽű�
	*/
	bool LoadScript(UInt32 type);

	/**
	*@brief ��������ű�
	*/
	bool LoadTaskScript();

	/**
	*@brief ���ؽű����֮��
	*/
	virtual void OnLoadScriptFinish(UInt32 type, Avalon::ScriptFile* script);

	/**
	*@brief �����ű�����
	*/
	Avalon::ScriptFile* CreateScript(UInt32 type, Avalon::ScriptState* state, UInt32 id = 0);

	/**
	*@brief ������������ű�
	*/
	bool LoadLostDungeonTaskScript();

private:
#ifndef _NEW_SCRIPT_MGR_
	//luastate
	Avalon::ScriptState*	m_pState;
	//npc�ű�
	NpcScriptMap	m_NpcScripts;
	//task�ű�
	TaskScriptMap	m_TaskScripts;
	//item�ű�
	ItemScriptMap	m_ItemScripts;
	//scene�ű�
	SceneScriptMap	m_SceneScripts;
	
	//ai�ű�
	AIScriptMap		m_AIScripts;
	//��ͨ����ai�ű�
	VehicleAIScriptMap	m_VehicleAIScripts;
	//��ҽű�
	PlayerScript*	m_pPlayerScript;
	//��������Ч���ű�
	TerritoryScript	*m_pTerritoryScript;
	//�ռ�װ���ű�
	FinalEquipScript	*m_pFinalEquipScript;
	//Ѻ�ڽű�
	EscortScript	*m_pEscortScript;
	//�����ű�
	ActivityGiftbagScript	*m_pActivityGiftbagScript;
	//װ������ű�
	EquipCalcScript	*m_pEquipCalcScript;
	//��ű�
	ActivityScriptMap m_ActivityScripts;
#else
	//ai��
	AIBindMap		m_AIBindMap;

	//�������򴥷������б�
	SceneTaskMap			m_SceneTasks;

	// �ű�����
	static ScriptConfig		s_scriptConfigs[SCENE_SCRIPT_NUM];
#endif
};


#endif
