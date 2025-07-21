#ifndef _SCRIPT_MGR_H_
#define _SCRIPT_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonScriptState.h>
#include <AvalonScriptFile.h>
#include <CLScriptDefine.h>
#include <CLScriptMgr.h>
#include "TaskScript.h"

class TimerScript;
class ChallengeScript;
class SortlistActivityScript;
class MallScript;
class WorldPlayerScript;

enum WScriptType
{
	WORLD_SCRIPT_TASK,
	WORLD_SCRIPT_COUNTER,
	WORLD_SCRIPT_TIMER,
	
	WORLD_SCRIPT_CONFIG,

	WORLD_SCRIPT_NUM,
	

	//δʹ��
	WORLD_SCRIPT_MALL,
	WORLD_SCRIPT_BATTLE_SCENE,
	WORLD_SCRIPT_PLAYER,
	
};

/**
 *@brief �ű�ϵͳ
 */
class ScriptMgr : public CLScriptMgr, public Avalon::Singleton<ScriptMgr>
{
public:
	typedef std::map<UInt32, TaskScript*>	TaskScriptMap;

public:
	ScriptMgr();
	~ScriptMgr();

public:
	bool Init();
	void Final();

	/**
	 *@brief ���¼��ؽű�
	 */
	bool ReloadScripts();

	/**
	 *@brief ��ȡ��ʱ���ű�
	 */
	TimerScript* GetTimerScript() const;

	/**
	 *@brief ��ȡս���ű�
	 */
	Avalon::ScriptFile* GetBattleSceneScript() const;

	/**
	*@brief �������а��ű�
	*/
	bool LoadGSActivityScript(UInt32 uVerTime);

	/**
	*@brief ������а��ű�
	*/
	SortlistActivityScript* GetGSActivityScript() const { return m_pGSActivityScript; }

	/**
	*@brief ��̳ǽű�
	*/
	MallScript* GetMallScript() const;

	/**
	 *@brief ��ȡ��ҽű�                                                                     
	 */
	WorldPlayerScript* GetPlayerScript() const;

	/**
	*@brief ��ȡtask�ű�
	*/
	TaskScript* FindTaskScript(UInt32 id);

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
	virtual bool LoadScript(UInt32 type);

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
	virtual Avalon::ScriptFile* CreateScript(UInt32 type, Avalon::ScriptState* state, UInt32 id = 0);

public://�¼�
	/**
	 *@brief ��ʱ�¼�
	 */
	void OnTick(const Avalon::Time& now);

private:
#ifndef _NEW_SCRIPT_MGR_
	//luastate
	Avalon::ScriptState*	m_pState;

	//task�ű�
	TaskScriptMap		m_TaskScripts;

	//��ʱ���ű�
	TimerScript*		m_pTimerScript;
	//ս���ű�
	Avalon::ScriptFile*	m_pBattleSceneScript;
	// �̳ǽű�
	MallScript			*m_pMallScript;
	//��ҽű�
	WorldPlayerScript	*m_pPlayerScript;
#else
	// ���а��ű�
	SortlistActivityScript	*m_pGSActivityScript;

	// �ű�����
	static ScriptConfig		s_scriptConfigs[WORLD_SCRIPT_NUM];
#endif
};

#endif
