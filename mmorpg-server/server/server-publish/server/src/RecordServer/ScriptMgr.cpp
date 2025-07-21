#include "ScriptMgr.h"
#include "SysInterface.h"
#include "DBPatchScript.h"
#include "DBHandler.h"

#include <CLScriptDefine.h>


ScriptConfig ScriptMgr::s_scriptConfigs[] = {
	{ DBPATCH_SCRIPT_PATH, NULL, "DBPatch", false, true },
};

class RecordScriptErrorHandler : public CLScriptErrorHandler
{
public:
	void OnError(const char* str)
	{
		SYSLOG_ERROR(str);
	}
};


ScriptMgr::ScriptMgr() 
:CLScriptMgr(RECORD_SCRIPT_NUM)
{
};

ScriptMgr::~ScriptMgr()
{
	Final();
	CL_SAFE_DELETE(m_pState);
}

bool ScriptMgr::Init(Avalon::PropertySet* properties)
{
	if (properties == NULL) return false;
	
	if (!CLScriptMgr::Init())
	{
		return false;
	}

	DBHandler::InitLocalDbAddress(properties);

	DefClasses();

	Avalon::ScriptState::SetErrorHandler(new RecordScriptErrorHandler());

	bool ret = LoadScripts();

	if (SCRIPT_ERROR != 0) return false;

	return ret;
}


void ScriptMgr::DefClasses()
{
	m_pState->DefClass<SysInterface>("SysClass")
		.Def("CreateDBHandler", &SysInterface::CreateDBHandler)
		.Def("CloseDBHandler", &SysInterface::CloseDBHandler)
		.Def("GetLocalDbHost", &SysInterface::GetLocalDbHost)
		.Def("GetLocalDbPort", &SysInterface::GetLocalDbPort)
		.Def("GetLocalDbUser", &SysInterface::GetLocalDbUser)
		.Def("GetLocalDbPasswd", &SysInterface::GetLocalDbPasswd)
		.Def("GetLocalDbName", &SysInterface::GetLocalDbName)
		;

	m_pState->Set("Sys", &sys);

	m_pState->DefClass<DBHandler>("DBHandler")
		.Def("Connect", &DBHandler::Connect)
		.Def("TryQuery", &DBHandler::TryQuery)
		.Def("Update", &DBHandler::Update)
		;
}

bool ScriptMgr::LoadScripts()
{
	std::string configpath = CONFIG_PATH;
	std::string packagePath = configpath + PACKAGE_PATH;
	m_pState->AddPackagePath(packagePath.c_str());
	Avalon::ScriptFile sandBoxScript(m_pState);
	std::string sandBoxPath = configpath + SANDBOX_SCRIPT_PATH;
	if (!sandBoxScript.Load(sandBoxPath.c_str()))
	{
		ErrorLogStream << "load sandbox script " << SANDBOX_SCRIPT_PATH << " failed!" << LogStream::eos;
		return false;
	}
	for (UInt32 i = 0; i < RECORD_SCRIPT_NUM; i++)
	{
		if (!LoadScript((RSScriptType)i))
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


	return CLScriptMgr::LoadScript(type, CONFIG_PATH, s_scriptConfigs[type]);	
}

Avalon::ScriptFile* ScriptMgr::CreateScript(UInt32 type, Avalon::ScriptState* state, UInt32 id)
{
	switch (type)
	{
	case RECORD_SCRIPT_DBPATCH:
		return new DBPatchScript(state);
		break;
	default:
		break;
	}

	return NULL;
}