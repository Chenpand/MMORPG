#include "AvalonServerApplication.h"
#include <signal.h>
#if defined(AVALON_OS_WINDOWS)
#define SIGPIPE -1
#define SIGQUIT -2
#define SIGUSR1 -3		//added by aprilliu
#define SIGUSR2 -4		//added by aprilliu
#else
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif
#include "AvalonThread.h"


namespace Avalon
{
#if defined(AVALON_OS_WINDOWS)
	ServerApplication* g_pApp = NULL;
	void SignalHandler(int signal)
	{
		if(g_pApp)
		{
			g_pApp->OnSignal(signal);
		}
	}
#else
	sigset_t oldmask;
	sigset_t mask;

	//信号处理线程
	class SignalThread : public Thread
	{
	public:
		SignalThread(ServerApplication* app):m_pApp(app){}
		~SignalThread(){}

	public:
		void Run()
		{
			while(m_pApp->IsAlive())
			{
				int signal;
				int err = sigwait(&mask,&signal); //noted by 调用此函数使得SignalThread阻塞，直到有指定的信号产生。由于主线程没有设置信号处理函数，所以只有SignalThread线程捕获到信号
				if(err == 0)
				{
					m_pApp->OnSignal(signal);
				}
			}
		}

	private:
		ServerApplication* m_pApp;
	};
#endif

	ServerApplication::ServerApplication()
	{
#if defined(AVALON_OS_WINDOWS)
		m_pSigThread = NULL;
#else
		m_pSigThread = new SignalThread(this);
#endif
	}

	ServerApplication::~ServerApplication()
	{
		delete m_pSigThread;
	}

	bool ServerApplication::OnInit()
	{
		if(!Application::OnInit()) return false;

		if(!GetProperties()->GetProperty(GetName(),"NODAEM"))
		{
			if(!Daemonize())
			{
				GetSysLog()->Error("daemonize failed!");
				return false;
			}
		}

		if(GetProperties()->GetProperty(GetName(),"UNIQUE"))
		{
			if(AlreadyRun())
			{
				GetSysLog()->Error("An instance is already running!");
				return false;
			}
		}

		//初始化日志追加器们，子线程需要在子进程创建之后激活
		GetSysLog()->InitAppenders();

#if defined(AVALON_OS_WINDOWS)
		g_pApp = this;
		signal(SIGINT,SignalHandler);
		signal(SIGTERM,SignalHandler);
#else
		//阻塞信号
		sigemptyset(&mask);
		sigaddset(&mask, SIGINT);
		sigaddset(&mask, SIGQUIT);
		sigaddset(&mask, SIGTERM);
		sigaddset(&mask, SIGPIPE);
		//added by aprilliu,增加SIGUSR1,SIGUSR2的捕获操作 可以把SIGUSR1,SIGUSR2作为重新加载配置文件的信号(具体实现：在相应信号处理函数中 设置reload标识， 在main loop检测到
		// reload标识被设置后 可以读取固定的配置文件来获取需要具体加载的配置文件信息)
		sigaddset(&mask, SIGUSR1);
		sigaddset(&mask, SIGUSR2);
		//
		if(pthread_sigmask(SIG_BLOCK,&mask,&oldmask) != 0)
		{
			GetSysLog()->Error("pthread_sigmask() failed! errno=%d",errno);
			return false;
		}
		m_pSigThread->Activate();
#endif

		return true;
	}

	void ServerApplication::OnQuit()
	{
#if defined(AVALON_OS_WINDOWS)
		g_pApp = NULL;
#else
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
		m_pSigThread->Stop();
		m_pSigThread->Join();
#endif

		Application::OnQuit();
	}

	void ServerApplication::OnSignal(int signal)
	{
		switch(signal)
		{
		case SIGPIPE:
			{
				//ignore
			}
			break;
		//added by aprilliu, 
		case SIGUSR1:
		case SIGUSR2:
			//TODO：编写相应的信号处理函数
			GetSysLog()->Info("catch user signal %d.", signal);
			break;
		//
		default:
			{
				GetSysLog()->Info("catch signal %d.",signal);
				Exit();
			}
		}
	}

	bool ServerApplication::Daemonize()
	{
#if defined(AVALON_OS_WINDOWS)
		return true;
#else
		umask(0);
		
		pid_t pid;
		if((pid = fork()) < 0)
		{
			return false;
		}
		else if(pid != 0)
		{
			exit(0);
		}
		
		setsid();
		return true;
#endif
	}

	bool ServerApplication::AlreadyRun()
	{
#if defined(AVALON_OS_WINDOWS)
		return false;
#else
		std::string lockFile = GetName() + ".pid";
		int fd = open(lockFile.c_str(), O_RDWR|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
		if(fd < 0)
		{
			exit(-1);
		}

		bool bRun = true;
		int ret = lockf(fd,F_TLOCK,0);	
		if(ret < 0)
		{
			close(fd);
			if(EACCES != errno && EAGAIN != errno)
			{
				exit(-1);
			}
			return false;
		}
		else
		{
			char buf[32] = {0};
			int len = snprintf(buf,32,"%d",(int)getpid());
			if(0 == ftruncate(fd, 0) && write(fd,buf,len) >= 0)	
				bRun = false;
		}

		return bRun;
#endif
	}
}
