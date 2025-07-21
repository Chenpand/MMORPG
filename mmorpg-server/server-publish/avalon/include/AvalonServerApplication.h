/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	服务器应用程序框架
 */
#ifndef _AVALON_SERVERAPPLICATION_H_
#define _AVALON_SERVERAPPLICATION_H_

#include "AvalonApplication.h"

namespace Avalon
{
	class Thread;

	/**
	 *@brief 服务端应用程序类，新参数如下
	 *      -NODAEM  前端运行
	 *		-UNIQUE  唯一实例运行
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
		//信号处理线程
		Thread* m_pSigThread;  //noted by aprilliu, 使用一个独立的线程来检测信号(SIGINT SIGQUIT SIGTERM SIGPIPE)
	};
}

#endif
