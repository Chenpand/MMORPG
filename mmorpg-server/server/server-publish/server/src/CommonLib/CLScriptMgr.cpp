#include "CLScriptMgr.h"
#include <AvalonFile.h>
#include <AvalonStringUtil.h>

CLScriptMgr::CLScriptMgr(UInt32 scriptNum)
{
	m_pState = new Avalon::ScriptState();
	m_scriptsByType.resize(scriptNum);
	m_singleScripts.resize(scriptNum, NULL);
}

CLScriptMgr::~CLScriptMgr()
{
	Final();
}

bool CLScriptMgr::Init()
{
	return true;
}

void CLScriptMgr::Final()
{
	for (UInt32 i = 0; i < m_scriptsByType.size(); i++)
	{
		for (auto itr : m_scriptsByType[i])
		{
			CL_SAFE_DELETE(itr.second);
		}

		m_scriptsByType[i].clear();
	}

	for (UInt32 i = 0; i < m_scriptsByType.size(); i++)
	{
		CL_SAFE_DELETE(m_singleScripts[i]);
	}
}

bool CLScriptMgr::LoadScript(UInt32 type, std::string root, const ScriptConfig& config)
{
	if (type >= m_scriptsByType.size())
	{
		return false;
	}

	if (config.inDir)
	{
		return LoadScriptInDir(type, root + config.path, config.prefix);
	}
	else
	{
		return LoadSingleScript(type, root + config.path, config.env, config.callLuaInitFunc);
	}
}

bool CLScriptMgr::LoadScriptInDir(UInt32 type, std::string scriptPath, std::string scriptPrefix)
{
	auto& scriptMgr = GetScriptMgr(type);
	std::string postfix = ".lua";
	auto prefixLen = scriptPrefix.length();
	auto postfixLen = postfix.length();

	Avalon::Directory scriptDir(scriptPath);
	for (Avalon::Directory::iterator iter = scriptDir.Begin();
		iter != scriptDir.End(); ++iter)
	{
		std::string name = iter->GetName();
		if (name.length() < (prefixLen + postfixLen) || name[0] == '.') continue;


		std::string head = name.substr(0, prefixLen);
		std::string tail = name.substr(name.length() - postfix.length());
		if (head.compare(scriptPrefix) != 0 || tail.compare(postfix) != 0)
		{
			ErrorLogStream << "unknow task script:" << name << "!" << LogStream::eos;
			continue;
		}

		UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(name.substr(prefixLen, name.length() - prefixLen - postfixLen));
		if (id == 0) continue;
		if (GetScriptMgr(type).find(id) != GetScriptMgr(type).end())
		{
			ErrorLogStream << "repeat task script: " << name << "!" << LogStream::eos;
			continue;
		}

		std::string env = name.substr(0, name.length() - postfixLen);
		m_pState->MakeEnv(env.c_str(), "SetSandBox");
		auto script = CreateScript(type, m_pState, id);
		if (!script || !script->Load(iter->GetFullPath().c_str(), env.c_str()))
		{
			// 加载失败，把之前加载好的也删了
			for (auto itr : scriptMgr)
			{
				CL_SAFE_DELETE(itr.second);
			}
			scriptMgr.clear();

			CL_SAFE_DELETE(script);
			ErrorLogStream << "load task script " << name << " failed!" << LogStream::eos;
			return false;
		}

		script->Init();


		if (!scriptMgr.insert(std::make_pair(id, script)).second)
		{
			CL_SAFE_DELETE(script);
			continue;
		}

		OnLoadScriptFinish(type, script);
	}

	return true;
}

bool CLScriptMgr::ReloadScript(UInt32 type)
{
	DebugLogStream << "reload script(" << type << ")." << LogStream::eos;

	// 先从singleScrip里找
	if (m_singleScripts[type])
	{
		auto oldScript = m_singleScripts[type];
		m_singleScripts[type] = NULL;

		if (LoadScript(type))
		{
			// 加载脚本成功，删除原来的脚本
			CL_SAFE_DELETE(oldScript);
			return true;
		}
		else
		{
			// 加载脚本失败，还原原先的脚本
			m_singleScripts[type] = oldScript;
			return false;
		}
	}

	// 从map中找
	auto oldScriptMgr = m_scriptsByType[type];
	m_scriptsByType[type].clear();
	if (LoadScript(type))
	{
		// 加载脚本成功，删除原来的脚本
		for (auto itr : oldScriptMgr)
		{
			CL_SAFE_DELETE(itr.second);
		}
		return true;
	}
	else
	{
		// 加载脚本失败，还原原先的脚本
		m_scriptsByType[type] = oldScriptMgr;
		return false;
	}

	return true;
}

void CLScriptMgr::GC()
{
	if (m_pState)
	{
		m_pState->Call<void>("collectgarbage", "collect");
	}
}

bool CLScriptMgr::LoadSingleScript(UInt32 type, std::string path, std::string env, bool callLuaInitFunc)
{
	auto script = CreateScript(type, m_pState);
	if (!script)
	{
		return false;
	}

	m_pState->MakeEnv(env.c_str(), "SetSandBox");
	if (!script->Load(path.c_str(), env.c_str()))
	{
		ErrorLogStream << "load " << path << " failed!" << LogStream::eos;
		CL_SAFE_DELETE(script);
		return false;
	}

	if (callLuaInitFunc)
	{
		script->Call<void>("Init", this);
	}
	else
	{
		script->Init();
	}

	m_singleScripts[type] = script;
	return true;
}