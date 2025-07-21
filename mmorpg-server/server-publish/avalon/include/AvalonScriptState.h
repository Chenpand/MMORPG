/**
 *@author	 
 *@email	 
 *@date		2011-06-24	
 *@brief	�ű�ȫ��״̬��
 */
#ifndef _AVALON_SCRIPT_STATE_H_
#define _AVALON_SCRIPT_STATE_H_

#include "AvalonScriptClass.h"

namespace Avalon
{
	/**
	 *@brief ����ص�
	 */
	class ScriptErrorHandler
	{
	public:
		virtual ~ScriptErrorHandler(){}
		virtual void OnError(const char* str) = 0;
	};

	/**
	 *@brief �ű�ȫ��״̬
	 */
	class ScriptState
	{
	public:
		ScriptState();
		~ScriptState();

	public:
		/**
		 *@brief ���ô�����
		 */
		static void SetErrorHandler(ScriptErrorHandler* handler);
		static void HandleError(const char* str);

		/**
		 *@brief ���ð�·��
		 */
		void AddPackagePath(const char* path);

		/**
		 *@brief ��������
		 */
		void MakeEnv(const char* env, const char* sandboxFunc){
			lua_tinker::make_env(m_pState, env, sandboxFunc);
		}

		/**
		 *@brief ע����
		 */
		template<typename T>
		ScriptClass<T> DefClass(const char* name)
		{
			lua_tinker::class_add<T>(m_pState, name);
			return m_pState;
		}
		template<typename T, typename BASE>
		ScriptClass<T> DefClass(const char* name)
		{
			lua_tinker::class_add<T>(m_pState, name);
			lua_tinker::class_inh<T, BASE>(m_pState);
			return m_pState;
		}

		/**
		 *@brief ע��ȫ�ֺ���
		 */
		template<typename F>
		void DefFunc(const char* name, F f)
		{
			lua_tinker::def(m_pState, name, f);
		}

		/**
		 *@brief ����ȫ�ֱ���
		 */
		template<typename T>
		void Set(const char* name, T object)
		{
			lua_tinker::set(m_pState, name, object);
		}

		/**
		 *@brief ��ȡȫ�ֱ���
		 */
		template<typename T>
		T Get(const char* name)
		{
			return lua_tinker::get<T>(m_pState, name);
		}

		/**
		 *@brief ���ú���
		 */
		template<typename RT>
		RT Call(const char* name)
		{
			return lua_tinker::call<RT>(m_pState, NULL, name);
		}
		template<typename RT, typename P1>
		RT Call(const char* name, P1 p1)
		{
			return lua_tinker::call<RT>(m_pState, NULL, name, p1);
		}
		template<typename RT, typename P1, typename P2>
		RT Call(const char* name, P1 p1, P2 p2)
		{
			return lua_tinker::call<RT>(m_pState, NULL, name, p1, p2);
		}
		template<typename RT, typename P1, typename P2, typename P3>
		RT Call(const char* name, P1 p1, P2 p2, P3 p3)
		{
			return lua_tinker::call<RT>(m_pState, NULL, name, p1, p2, p3);
		}

		lua_State* GetState() const { return m_pState; }

		int GetStackSize() const{ return lua_gettop(m_pState); }

	private:
		//lua
		lua_State* m_pState;
		//����ص�
		static ScriptErrorHandler* m_pErrorHandler;
	};
}

#endif
