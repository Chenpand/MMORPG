/**
 *@author	 
 *@email	 
 *@date		2011-03-03	
 *@brief	�������̳߳�
 */
#ifndef _AVALON_THREADPOOL_H_
#define _AVALON_THREADPOOL_H_

#include "AvalonWorkerThread.h"
#include "AvalonCondition.h"
#include "AvalonTask.h"

#include <vector>
#include <queue>

namespace Avalon
{
	/**
	 *@brief �������̳߳�
	 */
	class ThreadPool : private Nocopyable
	{
		typedef std::vector<WorkerThread*> ThreadList;
		typedef std::queue<Task*> TaskQueue;

	public:
		ThreadPool();
		~ThreadPool();

		/**
		 *@brief �����̳߳�
		 *@param threadNum �߳���
		 */
		bool Open(UInt32 threadNum);

		/**
		 *@brief �ر��̳߳�
		 */
		void Shutdown();

		/**
		 *@brief ���һ������
		 */
		bool PostTask(Task* task);

		/**
		 *@brief ��ȡһ���̣߳�����������
		 *@return ��Ҫִ�е���������̳߳عرջ���󷵻�NULL
		 */
		Task* GetTask();

	protected:
		/**
		 *@brief ���������߳�
		 */
		virtual WorkerThread* CreateWorkerThread();

	private:
		//�����߳��б�
		ThreadList	m_ThreadList;
		//�������
		TaskQueue	m_TaskQueue;
		//��������
		Condition	m_Cond;
		//������
		Mutex		m_Mutex;
		//�Ƿ�������
		bool		m_bRun;
	};
}

#endif
