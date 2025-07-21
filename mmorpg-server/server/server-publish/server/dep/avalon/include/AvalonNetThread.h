/**
 *@author	 
 *@email	 
 *@date		2011-03-27	
 *@brief	�����߳�
 */
#ifndef _AVALON_NETTHREAD_H_
#define _AVALON_NETTHREAD_H_

#include <map>
#include <vector>
#include "AvalonThread.h"
#include "AvalonMutex.h"
#include "AvalonProcessUnit.h"

namespace Avalon
{
	class INetEventHandler;
	class INetReactor;

	/**
	 *@brief �����߳�
	 */
	class NetThread : public Thread, public ProcessUnit
	{
		typedef std::map<INetEventHandler*,UInt32> HANDLER_MAP;

	public:
		explicit NetThread(UInt32 maxHandler);
		~NetThread();

	public:
		/**
		 *@brief ע��һ�����
		 */
		void RegisterHandler(INetEventHandler* handler,UInt32);

		/**
		 *@brief ��ȡ��ǰ�߳���
		 */
		UInt32 GetHandlerNum() const;

		void Stop(UInt32 inputTimeout);
		
	protected:
		bool OnInit();

		void OnTick(const Time& now);

		void OnQuit();

		void Run(); //noted by aprilliu, ���̺߳����б�����

	private:
		//��Ӧ��  noted by aprilliu, �����Ǽ����̻߳���IO�߳� �������Լ���Reactor�����ڼ����̵߳�reactorһ��ֻ��һ��Acceptor ��handler
		INetReactor* m_pReactor;
		//�����뷴Ӧ�ѵĴ���������
		HANDLER_MAP m_WaitQueue;
		//��
		Mutex m_Mutex;  //���ڻ������ m_WaitQueue
		//�������
		UInt32 m_MaxHandlerNum;
	};

	/**
	 *@brief �����߳���  noted by aprilliu ѭ�����������Ϊ�̶���2ms
	 */
	class NetThreadGroup
	{
		typedef std::vector<NetThread*> NETTHREAD_VEC;

	public:
		NetThreadGroup(UInt32 threadNum, UInt32 handlerPerThread);
		~NetThreadGroup();

		/**
		 *@brief ���������߳�
		 */
		bool Activate();

		/**
		 *@brief ֹͣ�����߳�
		 */
		void Stop(UInt32 inputTimeout);

		/**
		 *@brief �ȴ������߳���ֹ
		 */
		void Join();

		/**
		 *��ȡһ��ӵ�����ٴ��������������߳�
		 */
		NetThread* GetMinHandlerThread();

	private:
		//�߳�����
		NETTHREAD_VEC m_Threads;

	};
}

#endif
