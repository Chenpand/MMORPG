/**
 *@author	 
 *@email	 
 *@date		2011-02-26		
 *@brief	������Singleton����ģ��
 */
#ifndef _AVALON_SINGLETON_H_
#define _AVALON_SINGLETON_H_

#include "AvalonNocopyable.h"

namespace Avalon
{
	/**
	 *@brief ����ģ�����
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
		//Ψһʵ��
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
