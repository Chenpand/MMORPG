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
	

	//未使用
	WORLD_SCRIPT_MALL,
	WORLD_SCRIPT_BATTLE_SCENE,
	WORLD_SCRIPT_PLAYER,
	
};

/**
 *@brief 脚本系统
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
	 *@brief 重新加载脚本
	 */
	bool ReloadScripts();

	/**
	 *@brief 获取定时器脚本
	 */
	TimerScript* GetTimerScript() const;

	/**
	 *@brief 获取战场脚本
	 */
	Avalon::ScriptFile* GetBattleSceneScript() const;

	/**
	*@brief 加载排行榜活动脚本
	*/
	bool LoadGSActivityScript(UInt32 uVerTime);

	/**
	*@brief 获得排行榜活动脚本
	*/
	SortlistActivityScript* GetGSActivityScript() const { return m_pGSActivityScript; }

	/**
	*@brief 活动商城脚本
	*/
	MallScript* GetMallScript() const;

	/**
	 *@brief 获取玩家脚本                                                                     
	 */
	WorldPlayerScript* GetPlayerScript() const;

	/**
	*@brief 获取task脚本
	*/
	TaskScript* FindTaskScript(UInt32 id);

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
	virtual bool LoadScript(UInt32 type);

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
	virtual Avalon::ScriptFile* CreateScript(UInt32 type, Avalon::ScriptState* state, UInt32 id = 0);

public://事件
	/**
	 *@brief 定时事件
	 */
	void OnTick(const Avalon::Time& now);

private:
#ifndef _NEW_SCRIPT_MGR_
	//luastate
	Avalon::ScriptState*	m_pState;

	//task脚本
	TaskScriptMap		m_TaskScripts;

	//定时器脚本
	TimerScript*		m_pTimerScript;
	//战场脚本
	Avalon::ScriptFile*	m_pBattleSceneScript;
	// 商城脚本
	MallScript			*m_pMallScript;
	//玩家脚本
	WorldPlayerScript	*m_pPlayerScript;
#else
	// 排行榜活动脚本
	SortlistActivityScript	*m_pGSActivityScript;

	// 脚本配置
	static ScriptConfig		s_scriptConfigs[WORLD_SCRIPT_NUM];
#endif
};

#endif
