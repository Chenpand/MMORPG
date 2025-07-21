#include "AvalonServerApplication.h"
#include "AvalonSingleton.h"
#include "AvalonTimer.h"
#include "AvalonStringUtil.h"
#include "AvalonHttpUtil.h"
#include <iostream>

/*
Avalon::Logger* gLogger = NULL;

class TestTimer1 : public Avalon::TimerHandler
{
public:
	void OnTimeout(const Avalon::Time& now)
	{
		gLogger->Info("test timer1");
	}
};

class TestTimer2 : public Avalon::TimerHandler
{
public:
	void OnTimeout(const Avalon::Time& now)
	{
		gLogger->Info("test timer2");
	}
};

class TestApplication : public Avalon::ServerApplication, public Avalon::Singleton<TestApplication>
{
public:
	bool OnInit()
	{
		gLogger = GetSysLog();



		m_Reactor.Schedule(new TestTimer1,10000);
		m_Reactor.Schedule(new TestTimer2,8000);
		return true;
	}

	void OnTick(const Avalon::Time& now)
	{
		m_Reactor.HandleEvent(now);
	}

	void OnQuit()
	{
	}

private:
	Avalon::TimerReactor m_Reactor;
};
*/

int main(int argc,char* argv[])
{
	/*
	TestApplication::Instance()->Init("TestCore","TestCore.cfg");
	TestApplication::Instance()->SetInterval(100);
	TestApplication::Instance()->Main(argc,argv);
	TestApplication::Destroy();
	*/
	/*
	Avalon::HttpParser parser;
	const char request[] = "GET / HTTP/1.1\r\nHost:example.org\r\nUser-Agent:Mozilla/5.0\r\n\r\n";
	int len = strlen(request);
	int result = parser.Parse(request, len);
	if(result <= 0)
	{
		std::cout << "parse error:" << result;
		return -1;
	}

	std::cout << "Host:" << parser.GetHeadParam("Host") << std::endl;
	std::cout << "User-Agent:" << parser.GetHeadParam("User-Agent") << std::endl;
	std::cout << parser.GetUri() << std::endl;
	std::cout << parser.GetMethodType() << std::endl;
	*/
	::getchar();
	return 0;
}

