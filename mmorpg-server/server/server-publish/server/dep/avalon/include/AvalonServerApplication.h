/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	������Ӧ�ó�����
 */
#ifndef _AVALON_SERVERAPPLICATION_H_
#define _AVALON_SERVERAPPLICATION_H_

#include "AvalonApplication.h"

namespace Avalon
{
	class Thread;

	/**
	 *@brief �����Ӧ�ó����࣬�²�������
	 *      -NODAEM  ǰ������
	 *		-UNIQUE  Ψһʵ������
	 */
	class ServerApplication : public Application
	{
	public:
		ServerApplication();
		virtual ~ServerApplication();

		virtual void OnSignal(int signal);

	protected:
		bool OnInit();

		void OnQuit();

	protected:

		bool Daemonize();

		bool AlreadyRun();
	private:
		//�źŴ����߳�
		Thread* m_pSigThread;  //noted by aprilliu, ʹ��һ���������߳�������ź�(SIGINT SIGQUIT SIGTERM SIGPIPE)
	};
}

#endif
