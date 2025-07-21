#ifndef _SCRIPT_MGR_H_
#define _SCRIPT_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonScriptState.h>
#include <AvalonScriptFile.h>
#include <CLScriptDefine.h>

class WorldChallengeScript;

/**
 *@brief 脚本系统                                                                     
 */
class ScriptMgr : public Avalon::Singleton<ScriptMgr>
{
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
	 *@brief 获取跨服挑战赛脚本                                                                     
	 */
	WorldChallengeScript* GetChallengeScript() const{ return m_pChallengeScript; }

private:
	/**
	 *@brief 注册类和函数
	 */
	void DefClasses();

	/**
	 *@brief 加载脚本
	 */
	bool LoadScripts();

private:
	//luastate
	Avalon::ScriptState*	m_pState;

	//跨服挑战配置脚本
	WorldChallengeScript*	m_pChallengeScript;
};


#endif
