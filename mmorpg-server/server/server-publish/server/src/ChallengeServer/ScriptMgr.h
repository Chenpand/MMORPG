#ifndef _SCRIPT_MGR_H_
#define _SCRIPT_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonScriptState.h>
#include <AvalonScriptFile.h>
#include <CLScriptDefine.h>

class WorldChallengeScript;

/**
 *@brief �ű�ϵͳ                                                                     
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
	 *@brief ���¼��ؽű�
	 */
	bool ReloadScripts();

	/**
	 *@brief ��ȡ�����ս���ű�                                                                     
	 */
	WorldChallengeScript* GetChallengeScript() const{ return m_pChallengeScript; }

private:
	/**
	 *@brief ע����ͺ���
	 */
	void DefClasses();

	/**
	 *@brief ���ؽű�
	 */
	bool LoadScripts();

private:
	//luastate
	Avalon::ScriptState*	m_pState;

	//�����ս���ýű�
	WorldChallengeScript*	m_pChallengeScript;
};


#endif
