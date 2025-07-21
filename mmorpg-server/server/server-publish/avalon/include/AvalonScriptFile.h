/**
 *@author	 
 *@email	 
 *@date		2011-06-24	
 *@brief	脚本文件
 */
#ifndef _AVALON_SCRIPT_FILE_H_
#define _AVALON_SCRIPT_FILE_H_

#include <AvalonDefine.h>
#include "AvalonScriptState.h"
#include <string>

namespace Avalon
{
	class ScriptState;

	using lua_tinker::table_visitor;
	typedef lua_tinker::table ScriptTable;

	/**
	 *@brief 脚本文件
	 */
	class ScriptFile
	{
	public:
		explicit ScriptFile(ScriptState* state)
			:m_Id(0), m_pState(state){}
		virtual ~ScriptFile(){}

		virtual void Init() {}

	public:
		/**
		 *@brief 设置获取id
		 */
		void SetId(UInt32 id){ m_Id = id; }
		UInt32 GetId() const { return m_Id; }

		/**
		 *@brief 加载脚本
		 */
		bool Load(const char* path, const char* env = NULL);

		/**
		 *@brief 调用函数
		 */
		template<typename RT>
		RT Call(const char* name)
		{
			return lua_tinker::call<RT>(m_pState->GetState(), m_Env.empty() ? NULL : m_Env.c_str(), name);
		}
		template<typename RT, typename P1>
		RT Call(const char* name, P1 p1)
		{
			return lua_tinker::call<RT>(m_pState->GetState(), m_Env.empty() ? NULL : m_Env.c_str(), name, p1);
		}
		template<typename RT, typename P1, typename P2>
		RT Call(const char* name, P1 p1, P2 p2)
		{
			return lua_tinker::call<RT>(m_pState->GetState(), m_Env.empty() ? NULL : m_Env.c_str(), name, p1, p2);
		}
		template<typename RT, typename P1, typename P2, typename P3>
		RT Call(const char* name, P1 p1, P2 p2, P3 p3)
		{
			return lua_tinker::call<RT>(m_pState->GetState(), m_Env.empty() ? NULL : m_Env.c_str(), name, p1, p2, p3);
		}
		template<typename RT, typename P1, typename P2, typename P3, typename P4>
		RT Call(const char* name, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			return lua_tinker::call<RT>(m_pState->GetState(), m_Env.empty() ? NULL : m_Env.c_str(), name, p1, p2, p3, p4);
		}
		template<typename RT, typename P1, typename P2, typename P3, typename P4, typename P5>
		RT Call(const char* name, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			return lua_tinker::call<RT>(m_pState->GetState(), m_Env.empty() ? NULL : m_Env.c_str(), name, p1, p2, p3, p4, p5); 
		}

		/**
		 *@brief 获取脚本状态
		 */
		ScriptState* GetScriptState() const { return m_pState; }

	private:
		//id
		UInt32 m_Id;
		//全局状态
		ScriptState* m_pState;
		//环境
		std::string m_Env;
	};
}

#endif
