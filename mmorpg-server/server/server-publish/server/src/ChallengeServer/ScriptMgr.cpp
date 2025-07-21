#include "ScriptMgr.h"
#include "WorldChallengeScript.h"

ScriptMgr::ScriptMgr()
{
	m_pState = new Avalon::ScriptState();

	m_pChallengeScript = NULL;
}

ScriptMgr::~ScriptMgr()
{
	Final();

	CL_SAFE_DELETE(m_pState);
}

bool ScriptMgr::Init()
{
	DefClasses();

	Avalon::ScriptState::SetErrorHandler(new CLScriptErrorHandler());

	bool ret = LoadScripts();
	if(SCRIPT_ERROR != 0) return false;

	return ret;
}

void ScriptMgr::Final()
{
	CL_SAFE_DELETE(m_pChallengeScript);
}

bool ScriptMgr::ReloadScripts()
{
	Avalon::ScriptState* oldState = m_pState;

	WorldChallengeScript* oldChallengeScript = m_pChallengeScript;
	m_pChallengeScript = NULL;

	m_pState = new Avalon::ScriptState();

	DefClasses();

	if(!LoadScripts())
	{
		Final();
		CL_SAFE_DELETE(m_pState);

		m_pState = oldState;
		m_pChallengeScript = oldChallengeScript;
		return false;
	}

	CL_SAFE_DELETE(oldState);
	CL_SAFE_DELETE(oldChallengeScript);

	return true;
}

void ScriptMgr::DefClasses()
{
	m_pState->DefClass<WorldChallengeScript>("WorldChallengeScript")
		.Def("SetActivityTime", &WorldChallengeScript::SetActivityTime)
		.Def("SetGroupInfo", &WorldChallengeScript::SetGroupInfo);
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

	m_pChallengeScript = new WorldChallengeScript(m_pState);
	m_pState->MakeEnv("WorldChallenge", "SetSandBox");
	std::string worldChallengePath = configpath + WORLD_CHALLENGE_SCRIPT_PATH;
	if(!m_pChallengeScript->Load(worldChallengePath.c_str(), "WorldChallenge"))
	{
		CL_SAFE_DELETE(m_pChallengeScript);
		ErrorLogStream << "load worldchallenge script failed!" << LogStream::eos;
		return false;
	}
	m_pChallengeScript->Init();

	return true;
}
