#ifndef __CL_SCRIPT_MGR_H__
#define __CL_SCRIPT_MGR_H__

#include <CLDefine.h>
#include <AvalonScriptFile.h>

struct ScriptConfig
{
	const char* path;
	const char* prefix;
	const char* env;
	bool inDir;
	bool callLuaInitFunc;
};


//template<class SCRIPT_NUM>
class CLScriptMgr
{
public:
	typedef std::map<UInt32, Avalon::ScriptFile*>	ScriptMap;

	CLScriptMgr(UInt32 scriptNum);
	virtual ~CLScriptMgr();

	virtual bool Init();
	virtual void Final();

public:
	/**
	*@brief ���¼��ؽű�
	*/
	bool ReloadScript(UInt32 type);

	/**
	*@brief gc
	*/
	void GC();

protected:
	/**
	*@brief �����ű�����
	*/
	virtual Avalon::ScriptFile* CreateScript(UInt32 type, Avalon::ScriptState* state, UInt32 id = 0) = 0;

	/**
	*@brief ���ؽű�
	*/
	virtual bool LoadScript(UInt32 type) = 0;

protected:
	/**
	*@brief ���ؽű�
	*/
	bool LoadScript(UInt32 type, std::string root, const ScriptConfig& config);

	/**
	*@brief ���ؽű����֮��
	*/
	virtual void OnLoadScriptFinish(UInt32 type, Avalon::ScriptFile* script) {}

	/**
	*@brief �����ļ��������з���Ҫ��Ľű�
	*/
	bool LoadScriptInDir(UInt32 type, std::string scriptPath, std::string scriptPrefix);

	/**
	*@brief ���ص����ű�
	*/
	bool LoadSingleScript(UInt32 type, std::string path, std::string env, bool callLuaInitFunc);

	inline ScriptMap& GetScriptMgr(UInt32 type) { return m_scriptsByType[type]; }

	template<typename T>
	T* GetSingleScript(UInt32 type) const
	{
		return dynamic_cast<T*>(m_singleScripts[type]);
	}

	template<typename T>
	T* GetScript(UInt32 type, UInt32 id) const
	{
		if (type >= m_scriptsByType.size())
		{
			return NULL;
		}

		auto& scriptMgr = m_scriptsByType[type];
		auto iter = scriptMgr.find(id);
		if (iter != scriptMgr.end())
		{
			return dynamic_cast<T*>(iter->second);
		}
		return NULL;
	}

protected:
	//luastate
	Avalon::ScriptState*				m_pState;

	// ���нű�
	std::vector<ScriptMap>				m_scriptsByType;
	std::vector<Avalon::ScriptFile*>	m_singleScripts;
};

#endif