/**
*@author
*@email
*@date		2011-03-03
*@brief	�������߳�
*/
#ifndef _AVALON_WORKERTHREAD_H_
#define _AVALON_WORKERTHREAD_H_

#include "AvalonThread.h"

namespace Avalon
{
	class ThreadPool;

	/**
	*@brief �������߳���
	*/
	class WorkerThread : public Thread
	{
	public:
		explicit WorkerThread(ThreadPool* poll);
		virtual ~WorkerThread();

		/**
		*@brief ��ʼ���ӿڣ�����ʵ��
		*/
		virtual bool Init();

		virtual void Run();

	private:
		//�̳߳�
		ThreadPool* m_pPool;
	};
}

#endif
