#include "CLApplication.h"
#include "CLErrorCode.h"

#ifdef _PROFILER_
#include <gperftools/profiler.h>
#endif

UInt32 ZONE_ID = 0;
UInt32 NEUTRAL_ZONE_ID = 0;
UInt32 SERVER_TYPE = 0;
UInt32 SERVER_ID = 0;
UInt32 SCRIPT_ERROR = 0;
Avalon::Time CURRENT_TIME = Avalon::Time::CurrentTime();
Avalon::Date CURRENT_DATE = Avalon::Date(CURRENT_TIME);
Avalon::Logger* gSysLogger = NULL;
UInt32 GAMESTART_TIME = 0;
UInt32 WORLD_LEVEL = 0;
std::string GAME_VERSION = "";
std::string PLATFORM = "";
UInt32 GAME_FIRST_START_TIME = 0;
std::string SERVER_NAME = "";
std::string MERGED_SERVERS = "";
UInt32 MAX_PACKET_PERTICK = 1000;

CLApplication::CLApplication()
{
	m_bDebugMode = false;
}

CLApplication::~CLApplication()
{
}

bool CLApplication::OnInit()
{
	gSysLogger = GetSysLog();
	Avalon::net_setlogger(gSysLogger);

	if(!ServerApplication::OnInit())
	{
		return false;
	}

	SERVER_TYPE = GetProperties()->GetProperty("Server","server_type");
	SERVER_ID = GetProperties()->GetProperty("Server","server_id");
	if(IsAdminServer(SERVER_TYPE))
	{
		ZONE_ID = GetProperties()->GetProperty("Server","zone_id");	
	}
	
	//noted by aprilliu , 格式：netaddress_config=NetAddress.xml,PlatformNetAddress.xml 一般只有Admin server有类似的配置， 其他服务器从admin server拉取自己的IP:Port设置信息
	std::string netaddresscfg = GetProperties()->GetProperty("Server", "netaddress_config").GetString();
	if(!netaddresscfg.empty())
	{
		std::vector<std::string> configs;
		Avalon::StringUtil::Split(netaddresscfg, configs, ",");
		for(std::vector<std::string>::iterator iter = configs.begin();
			iter != configs.end(); ++iter)
		{
			if(!m_Address.Load(*iter))
			{
				GetSysLog()->Error("can not load netaddresses! config = %s.", iter->c_str());
				return false;
			}
		}
	}
	
	if(GetProperties()->GetProperty(GetName(), "DEBUG"))
	{
		m_bDebugMode = true;
	}

#ifdef _PROFILER_
	std::string name = GetName() + ".prof";
	ProfilerStart(name.c_str());
#endif
	
	return true;
}

void CLApplication::OnTick(const Avalon::Time& now)
{
	CURRENT_TIME = now;
	CURRENT_DATE = Avalon::Date(now);
}

void CLApplication::OnQuit()
{
	gSysLogger = NULL;
	Avalon::net_setlogger(gSysLogger);
	ServerApplication::OnQuit();

#ifdef _PROFILER_
	ProfilerStop();
#endif
}
