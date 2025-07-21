#include "AvalonScriptFile.h"
#include "AvalonScriptState.h"

namespace Avalon
{
	bool ScriptFile::Load(const char* path, const char* env)
	{
		lua_State* luaState = m_pState->GetState();

		if(!lua_tinker::dofile(luaState, path, env))
		{
			return false;
		}

		m_Env.clear();
		if(env != NULL) m_Env = env;

		return true;
	}
}
