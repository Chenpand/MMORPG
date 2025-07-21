/**
 *@author	 
 *@email	 
 *@date		2011-02-26		
 *@brief	定义了Singleton单例模板
 */
#ifndef _AVALON_SINGLETON_H_
#define _AVALON_SINGLETON_H_

#include "AvalonNocopyable.h"

namespace Avalon
{
	/**
	 *@brief 单例模板基类
	 */
	template<typename T>
	class Singleton : public Nocopyable
	{
	public:
		static T* Instance()
		{
			if(s_pInstance == 0)
			{
				s_pInstance = new T;
			}
			return s_pInstance;
		}

		static void Destroy()
		{
			delete s_pInstance;
			s_pInstance = 0;
		}

	private:
		//唯一实例
		static T* s_pInstance;

		class DestroyHelper
		{
			~DestroyHelper()
			{
				Singleton<T>::Destroy();
			}
		};
		static DestroyHelper s_Helper;
	};

	template<typename T>
	T* Singleton<T>::s_pInstance = 0;

	template<typename T>
	typename Singleton<T>::DestroyHelper Singleton<T>::s_Helper;
}

#endif
