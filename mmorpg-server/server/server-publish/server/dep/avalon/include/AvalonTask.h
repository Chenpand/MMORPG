/**
 *@author	 
 *@email	 
 *@date		2011-03-03	
 *@brief	�߳���������
 */
#ifndef _AVALON_TASK_H_
#define _AVALON_TASK_H_


namespace Avalon
{
	/**
	 *@brief ���̵߳��ȵ�����
	 */
	class WorkerThread;
	class Task
	{
	public:
		/**
		 *@brief ����ִ�нӿڣ�����ʵ��
		 */
		virtual void Run(WorkerThread* thread) = 0;

		/**
		 *@brief �ͷ��������
		 */
		virtual void Release() = 0;
	};
}

#endif
