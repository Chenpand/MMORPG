/**
 *@author	 
 *@email	 
 *@date		2011-06-24	
 *@brief	方便c++类导出的辅助类
 */
#ifndef _AVALON_SCRIPT_CLASS_H_
#define _AVALON_SCRIPT_CLASS_H_

#include "lua/lua_tinker.h"

namespace Avalon
{
	/**
	 *@brief 脚本类
	 */
	template<typename T>
	class ScriptClass
	{
		friend class ScriptState;
	private:
		ScriptClass(lua_State* state):m_pState(state){}
	public:
		~ScriptClass(){}

	public:
		/**
		 *@brief 定义构造函数
		 */
		ScriptClass& Con()
		{
			lua_tinker::class_con<T>(m_pState, lua_tinker::constructor<T>);
			return *this;
		}
		template<typename P1>
		ScriptClass& Con()
		{
			lua_tinker::class_con<T>(m_pState, lua_tinker::constructor<T, P1>);
			return *this;
		}
		template<typename P1, typename P2>
		ScriptClass& Con()
		{
			lua_tinker::class_con<T>(m_pState, lua_tinker::constructor<T, P1, P2>);
			return *this;
		}
		template<typename P1, typename P2, typename P3>
		ScriptClass& Con()
		{
			lua_tinker::class_con<T>(m_pState, lua_tinker::constructor<T, P1, P2, P3>);
			return *this;
		}
		template<typename P1, typename P2, typename P3, typename P4>
		ScriptClass& Con()
		{
			lua_tinker::class_con<T>(m_pState, lua_tinker::constructor<T, P1, P2, P3, P4>);
			return *this;
		}
		template<typename P1, typename P2, typename P3, typename P4, typename P5>
		ScriptClass& Con()
		{
			lua_tinker::class_con<T>(m_pState, lua_tinker::constructor<T, P1, P2, P3, P4, P5>);
			return *this;
		}

		/**
		 *@brief 定义成员函数
		 */
		template<typename F>
		ScriptClass& Def(const char* name, F func)
		{
			lua_tinker::class_def<T>(m_pState, name, func);
			return *this;
		}

		/**
		 *@brief 定义成员变量
		 */
		template<typename BASE, typename VAR>
		ScriptClass& Mem(const char* name, VAR BASE::*val)
		{
			lua_tinker::class_mem<T>(m_pState, name, val);
			return *this;
		}

	private:
		//lua
		lua_State* m_pState;
	};
}

#endif
