#include "AvalonScriptState.h"
#include <string>

namespace Avalon
{
	ScriptErrorHandler* ScriptState::m_pErrorHandler = NULL;

	void ScriptState::SetErrorHandler(ScriptErrorHandler* handler)
	{
		m_pErrorHandler = handler;
	}

	void ScriptState::HandleError(const char* str)
	{
		if(m_pErrorHandler != NULL)
		{
			m_pErrorHandler->OnError(str);
		}
	}


	ScriptState::ScriptState()
	{
		m_pState = lua_open();
		luaL_openlibs(m_pState);

		DefFunc("_ALERT", HandleError);
	}

	ScriptState::~ScriptState()
	{
		lua_close(m_pState);
	}

	void ScriptState::AddPackagePath(const char* path)
	{
		if(path == NULL) return;
		lua_tinker::table pack = lua_tinker::get<lua_tinker::table>(m_pState, "package");
		char* oldPath = pack.get<char*>("path");
		if(oldPath != NULL)
		{
			std::string newpath = path;
			newpath.append(";");
			newpath.append(oldPath);
			pack.set("path", newpath.c_str());
		}
	}
}
