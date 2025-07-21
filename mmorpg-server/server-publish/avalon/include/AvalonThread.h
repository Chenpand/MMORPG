/**
 *@author	 
 *@email	 
 *@date		2011-03-01	
 *@brief	�߳��ඨ��
 */
#ifndef _AVALON_THREAD_H_
#define _AVALON_THREAD_H_

#include <AvalonThreadAPI.h>
#include <AvalonSemaphore.h>

namespace Avalon
{
	class INetEventHandler;
	class INetReactor;

	/**
	 *@brief �߳���
	 */
	class Thread : private Nocopyable
	{
	public:
		Thread();
		virtual ~Thread();

	protected:
		/**
		 *@brief �߳�ִ���壬����ʵ��!! ���� ���磺NetThread
		 */
		virtual void Run() = 0;

	public:
		/**
		 *@brief ��������߳�
		 */
		bool Activate();

		/**
		 *@brief ǿ����ֹ�߳�
		 */
		virtual void Stop();

		/**
		 *@brief �����߳���Դ
		 */
		void Join();

		static void Sleep(UInt32 msecs);

	private:
		static AVALON_THREADFUNC_RET Function(void*);

	private:
		//�߳�
		thread_t	m_Thread;
		//���ź���
		Semaphore	m_Sem;
	};
}

#endif
