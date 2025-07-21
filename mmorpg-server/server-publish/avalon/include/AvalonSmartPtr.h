/**
 *@author	 
 *@email	 
 *@date		2011-03-28	
 *@brief	����ָ�붨��
 */
#ifndef _AVALON_SMARTPTR_H_
#define _AVALON_SMARTPTR_H_

#include "AvalonMutex.h"

namespace Avalon
{
	/**
	 *@brief �ɱ�����ָ�����õĽӿ��ඨ��
	 */
	class IReferenceable
	{
	public:
		IReferenceable():m_RefCount(0){}
		virtual ~IReferenceable(){}

		void IncRef()
		{
			m_Mutex.Acquire();
			++m_RefCount;
			m_Mutex.Release();
		}

		void DecRef()
		{
			m_Mutex.Acquire();
			if(--m_RefCount == 0)
			{
				m_Mutex.Release();
				Release();
				return;
			}
			m_Mutex.Release();
		}

		virtual void Release(){ delete this; }

	private:
		//���ü���
		size_t m_RefCount;
		//������
		Mutex m_Mutex;
	};



	/**
	 *@brief ����ָ��
	 */
	template<typename T>
	class SmartPtr
	{
	public:
		SmartPtr():m_ptr(NULL)
		{
		}
		SmartPtr(T* ptr):m_ptr(ptr)
		{
			if(m_ptr != NULL) m_ptr->IncRef();
		}
		SmartPtr(const SmartPtr& ptr):m_ptr(ptr.m_ptr)
		{
			if(m_ptr != NULL) m_ptr->IncRef();
		}
		~SmartPtr()
		{
			if(m_ptr != NULL) m_ptr->DecRef();
		}

	public:
		T* operator->(){ return m_ptr; }
		const T* operator->() const{ return m_ptr; }

		T& operator*(){ return *m_ptr; }
		const T& operator*() const{ return *m_ptr; }

		const SmartPtr& operator=(T* ptr)
		{
			if(m_ptr == ptr) return *this;
			if(m_ptr != NULL) m_ptr->DecRef();
			m_ptr = ptr;
			if(m_ptr != NULL) m_ptr->IncRef();
			return *this;
		}

		const SmartPtr& operator=(const SmartPtr& ptr)
		{
			if(m_ptr == ptr.m_ptr) return *this;
			if(m_ptr != NULL) m_ptr->DecRef();
			m_ptr = ptr.m_ptr;
			if(m_ptr != NULL) m_ptr->IncRef();
			return *this;
		}

		bool operator!=(const T* ptr) const{ return m_ptr != ptr; }
		bool operator==(const T* ptr) const{ return m_ptr == ptr; }

	private:
		//ָ��
		T* m_ptr;
	};
}

#endif
