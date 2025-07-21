#include "ASApplication.h"

#include <json/writer.h>
#include <AvalonLogger.h>
#include <AvalonMd5.h>
#include <CLGameLoggerMgr.h>
#include <AvalonHttpUtil.h>
#include <AvalonStringUtil.h>

#include "ASInternalNetwork.h"
#include "ASExternalNetwork.h"
#include "ASConsoleNetwork.h"
#include "ASBillingNetwork.h"
#include "DBLoggerMgr.h"
#include "GateSessionMgr.h"

#ifdef _PROFILER_
#include <gperftools/profiler.h>
#endif

ASApplication::ASApplication()
{
    m_Version = 0;
	m_ReloadConfig = false;
	m_StartProfiler = m_IsStartProfiler = false;
#ifdef _DEBUG
	m_ReportTimer.SetInterval(1 * Avalon::Time::MSECS_OF_SEC);
#else
	m_ReportTimer.SetInterval(30 * Avalon::Time::MSECS_OF_SEC);
#endif
	m_WorldPlayerNum = 0;
	m_WorldMaxPlayerNum = 0;
	m_CheckTableMd5 = true;
	m_InQueuePlayerNum = 0;
	m_OpenBugly = false;
	m_VoiceFlag = 0;
	m_BattleUseTcp = false;
	m_AccVerifyParamLength = 0;
}

ASApplication::~ASApplication()
{
}

bool ASApplication::OnInit()
{
	if(!CLApplication::OnInit()) return false;

	std::string configpath = CONFIG_PATH;
	std::string rootpath = ROOT_PATH;
	if(!GetProperties()->LoadFromFile(rootpath + "Game.cfg"))
	{
		ErrorLogStream << "load game config failed!" << LogStream::eos;
		return false;
	}

	if (!GetProperties()->LoadFromFile(rootpath + "MD5.cfg"))
	{
		ErrorLogStream << "load md5 config failed!" << LogStream::eos;
		return false;
	}

	m_CryptKey1 = GetProperties()->GetProperty("Crypt", "crypt_key1").GetString();
	m_CryptKey2 = GetProperties()->GetProperty("Crypt", "crypt_key2").GetString();
	m_DirCryptKey = GetProperties()->GetProperty("Crypt", "dir_crypt_key").GetString();
	m_BillingKey = GetProperties()->GetProperty("Billing", "billing_key").GetString();
	m_DirServerAddrs = GetNetAddress()->GetDirServerAddresses();

	LoadDynamicConfig();

	// 如果开放登录，根据输入参数重新设置这个值
	if (!m_ForbidLogin)
	{
		m_ForbidLogin = GetProperties()->GetProperty("AdminServer", "forbid_login");
	}

	GateSessionMgr::Instance()->Init();

	if(!GameLoggerMgr::Instance()->Init(GAME_LOGGER_CFG, IsDebugMode()))
	{
		ErrorLogStream << "game loggermgr init failed!" << LogStream::eos;
		return false;
	}

	if(!DBLoggerMgr::Instance()->Init(GAME_LOGGER_CFG))
	{
		ErrorLogStream << "db loggermgr init failed!" << LogStream::eos;
		return false;
	}

	if(!ASInternalNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "internal network startup failed!" << LogStream::eos;
		return false;
	}

	if(!BillingNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream <<"billing network startup failed!" << LogStream::eos;
		return false;
	}

	if(!ConsoleNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "console network startup failed!" << LogStream::eos;
		return false;
	}

	if(!ASExternalNetwork::Instance()->Startup(GetProperties(), GetNetAddress()))
	{
		ErrorLogStream << "external network startup failed!" << LogStream::eos;
		return false;
	}

	if (!m_DirUdpSocket.Create(SOCK_DGRAM))
	{
		ErrorLogStream << "create dir server udp connection failed!" << LogStream::eos;
		return false;
	}

	return true;
}

void ASApplication::OnTick(const Avalon::Time &now)
{
	CLApplication::OnTick(now);

	ASInternalNetwork::Instance()->OnTick(now);
	ConsoleNetwork::Instance()->OnTick(now);
	BillingNetwork::Instance()->OnTick(now);
	ASExternalNetwork::Instance()->OnTick(now);
	DBLoggerMgr::Instance()->OnTick(now);

	// 白名单状态下属于维护状态，不需要发送服务器状态
	if (!m_ForbidLogin && m_ReportTimer.IsTimeout(now))
	{
		ReportServerStatus();
	}

	if (m_ReloadConfig)
	{
		m_ReloadConfig = false;

		if (!GetProperties()->LoadFromFile(std::string(ROOT_PATH) + "Game.cfg"))
		{
			ErrorLogStream << "load Game.cfg failed!" << LogStream::eos;
			return;
		}

		if (!GetProperties()->LoadFromFile(std::string(ROOT_PATH) + "MD5.cfg"))
		{
			ErrorLogStream << "load MD5.cfg failed!" << LogStream::eos;
			return;
		}

		if (!GetProperties()->LoadFromFile("AdminServer.cfg"))
		{
			ErrorLogStream << "load AdminServer.cfg failed!" << LogStream::eos;
			return;
		}

		LoadDynamicConfig();
	}

#ifdef _PROFILER_
	if (m_StartProfiler != m_IsStartProfiler)
	{
		m_IsStartProfiler = m_StartProfiler;
		if (m_StartProfiler)
		{
			ProfilerStart("AdminServerDetail.prof");
		}
		else
		{
			ProfilerStop();
		}
	}
#endif
}

void ASApplication::OnSignal(int signal)
{
	InfoLogStream << "recv signal " << signal << LogStream::eos;

	switch (signal)
	{
	case SIG_RELOAD_CONFIG:
		{
			m_ReloadConfig = true;
			break;
		}
	case 12:
		{
			m_StartProfiler = !m_StartProfiler;
			DebugLogStream << "start profiler(" << (UInt8)m_StartProfiler << ")." << LogStream::eos;
			break;
		}
	default:
		{
			ServerApplication::OnSignal(signal);
			break;
		}
	}
}

void ASApplication::OnQuit()
{
	ASExternalNetwork::Instance()->Shutdown(0);
	ConsoleNetwork::Instance()->Shutdown(0);
	BillingNetwork::Instance()->Shutdown(0);
	ASInternalNetwork::Instance()->Shutdown(100);
	DBLoggerMgr::Instance()->Final();
	GameLoggerMgr::Instance()->Final();

	GateSessionMgr::Instance()->Final();

	CLApplication::OnQuit();

	m_DirUdpSocket.Close();
}

std::string ASApplication::GetActivityYearSortListUrl(const std::string& pf) const
{
	if (m_ActivityYearSortListUrl.empty())
	{
		return "";
	}

	if (m_ActivityYearSortListUrl.find('?') == std::string::npos)
	{
		return m_ActivityYearSortListUrl + "?pf=" + pf;
	}
	else
	{
		return m_ActivityYearSortListUrl + "&pf=" + pf;
	}
}

std::string ASApplication::GetWebActivityUrl(const std::string& pf) const
{
	if (m_WebActivityUrl.empty())
	{
		return "";
	}

	/*if (m_WebActivityUrl.find('?') == std::string::npos)
	{
		return m_WebActivityUrl + "?pf=" + pf;
	}
	else
	{
		return m_WebActivityUrl + "&pf=" + pf;
	}*/
	return m_WebActivityUrl;
}

void ASApplication::LoadDynamicConfig()
{
	DebugLogStream << "load adminserver dynamic config." << LogStream::eos;

	// 性能统计
	CLNetworkPerfMgr::Instance()->Init(GetProperties());

	// 版本号
	LoadVersion();

	m_ReplayAgentAddress = GetProperties()->GetProperty("ReplayAgent", "address").GetString();

	// 是否限制登录
	std::string fuck = GetProperties()->GetProperty("Login", "forbid_login").GetString();
	m_ForbidLogin = GetProperties()->GetProperty("Login", "forbid_login");

	// 白名单
	{
		m_LoginWhiteList.clear();
		std::string whiteListStr = GetProperties()->GetProperty("Login", "white_list").GetString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(whiteListStr, strs, "|");
		for (auto str : strs)
		{
			m_LoginWhiteList.insert(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}

	// bugly白名单
	{
		m_BuglyWhiteList.clear();
		std::string whiteListStr = GetProperties()->GetProperty("Bugly", "white_list").GetString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(whiteListStr, strs, "|");
		for (auto str : strs)
		{
			m_BuglyWhiteList.insert(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}
	
	m_CheckTableMd5 = !GetProperties()->GetProperty("Game", "close_md5_check");
	m_AccVerifyParamLength = GetProperties()->GetProperty("Game", "acc_verify_param_length");
	if (m_AccVerifyParamLength == 0)
	{
		m_AccVerifyParamLength = 400;
	}

	// 表格md5
	m_TableMd5 = GetProperties()->GetProperty("MD5", "table").GetString();
	m_SkillMd5 = GetProperties()->GetProperty("MD5", "skill").GetString();

	m_Md5Infoes.clear();
	for (int i = 0; i < 5; i++) {
		std::string section = std::string("MD5_") + Avalon::StringUtil::ConvertToString(i + 1);

		ClientMD5Info info;
		info.tableMd5 = GetProperties()->GetProperty(section, "table").GetString();
		info.skillMd5 = GetProperties()->GetProperty(section, "skill").GetString();

		if (info.tableMd5 == "" || info.skillMd5 == "")
		{
			continue;
		}

		m_Md5Infoes.push_back(info);
	}

	m_SoMd5 = GetProperties()->GetProperty("MD5", "so").GetString();
	m_SoKey = GetProperties()->GetProperty("MD5", "so_key").GetString();
	m_OpenBugly = GetProperties()->GetProperty("Game", "open_bugly");

	// 语音开关
	m_VoiceFlag = GetProperties()->GetProperty("Voice", "voice_flag");
	// 活动排行榜URL
	m_ActivityYearSortListUrl = GetProperties()->GetProperty("HttpUrl", "activity_year_sortlist_url").GetString();
	m_WebActivityUrl = GetProperties()->GetProperty("HttpUrl", "web_activity_url").GetString();
	m_RedPacketRankUrl = GetProperties()->GetProperty("HttpUrl", "red_packet_rank_url").GetString();

	// 是否开启新的重连机制
	m_OpenNewReconnectAlgo = GetProperties()->GetProperty("Switch", "open_new_reconn_algo");
	// 是否开启新的发送帧算法
	m_OpenNewReportFrameAlgo = GetProperties()->GetProperty("Switch", "open_new_report_frame_algo");

	// 装备图鉴评论服url
	m_CommentServerUrl = GetProperties()->GetProperty("CommentServer", "comment_server_url").GetString();

	SERVER_NAME = GetProperties()->GetProperty("Server", "name").GetString();

	InfoLogStream << "activity_year_sortlist_url = " << m_ActivityYearSortListUrl << LogStream::eos;

	{
		std::string str = GetProperties()->GetProperty("AccountConvert", "ConvertPf").GetString();
		Avalon::StringUtil::Split(str, m_ConvertPfs, "|");
		m_ConvertUrl = GetProperties()->GetProperty("AccountConvert", "ConvertUrl").GetString();
		InfoLogStream << "need convert pfs: " << str << " url:" << m_ConvertUrl << LogStream::eos;
	}

	m_ReportWebUrl = GetProperties()->GetProperty("HttpUrl", "report_url").GetString();
	m_WriteQuestionnaireUrl = GetProperties()->GetProperty("HttpUrl", "questionnaire_url").GetString();

	m_LogTeamCopyProtocol = GetProperties()->GetProperty("Game", "log_teamcopy_protocol");

	InfoLogStream << "LogTeamCopyProtocol(" << m_LogTeamCopyProtocol << ")." << LogStream::eos;

	// 
	m_BattleUseTcp = GetProperties()->GetProperty("Battle", "use_tcp");

	InfoLogStream << "battle use tcp(" << m_BattleUseTcp << ")." << LogStream::eos;

	UInt8 tmpPriority = GetProperties()->GetProperty("Log", "priority");
	if (tmpPriority >= Avalon::LOG_DEBUG && tmpPriority < Avalon::LOG_MAX)
	{
		GetSysLog()->SetPriority((Avalon::LogPriority)tmpPriority);
	}
}

void ASApplication::LoadVersion()
{
	std::string versionStr = GetProperties()->GetProperty("Server", "version").GetString();
	if (!ReadVersionFromStr(versionStr))
	{
		ErrorLogStream << "invalid server version(" << versionStr << ")." << LogStream::eos;
		return;
	}
	else
	{
		UInt32 major = 0;
		UInt32 minor = 0;
		UInt32 develop = 0;
		FetchVersion(m_Version, major, minor, develop);
		InfoLogStream << "server new version(" << major << "." << minor << "." << develop << ")." << LogStream::eos;
	}
}

bool ASApplication::ReadVersionFromStr(std::string str)
{
    std::vector<std::string> strs;
    Avalon::StringUtil::Split(str, strs, ".");
    if (strs.size() != 3)
    {
        return false;
    }

    UInt32 major = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
    UInt32 minor = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
    UInt32 develop = Avalon::StringUtil::ConvertToValue<UInt32>(strs[2]);
    m_Version = MakeVersion(major, minor, develop);
	return true;
}

bool ASApplication::CheckVersion(UInt32 version)
{
    return ((version & 0xFFFF0000) == (m_Version & 0xFFFF0000));
}

bool ASApplication::IsForbidLogin(UInt32 accid)
{
	if (!m_ForbidLogin)
	{
		return false;
	}

	if (m_LoginWhiteList.find(accid) != m_LoginWhiteList.end())
	{
		return false;
	}

	return true;
}

bool ASApplication::IsOpenBugly(UInt32 accid)
{
	if (m_OpenBugly)
	{
		return true;
	}

	if (m_BuglyWhiteList.find(accid) != m_BuglyWhiteList.end())
	{
		return true;
	}

	return false;
}

UInt32 ASApplication::MakeVersion(UInt32 major, UInt32 minor, UInt32 develop)
{
    return major << 24 | minor << 16 | (develop & (0xFFFF));
}

void ASApplication::FetchVersion(UInt32 version, UInt32& major, UInt32& minor, UInt32& develop)
{
    major = version >> 24;
    minor = (version >> 16) & (0x00FF);
    develop = version & 0xFFFF;
}

void ASApplication::ReportServerStatus()
{
	std::ostringstream oss;
	json::Object jsonobj;
	jsonobj["id"] = (json::String)Avalon::StringUtil::ConvertToString(ZONE_ID);
	jsonobj["num"] = (json::Number)m_WorldPlayerNum;
	jsonobj["name"] = (json::String)SERVER_NAME;
	Avalon::SockAddr addr;
	GetNetAddress()->FindExternalAddress(NODE_ID, addr);
	jsonobj["ip"] = (json::String)addr.GetIP();
	jsonobj["port"] = (json::Number)addr.GetPort();

	jsonobj["queue"] = (json::Number)m_InQueuePlayerNum;
	try
	{
		json::Writer::Write(jsonobj, oss);
	}
	catch (const json::Exception& e)
	{
		ErrorLogStream << "make server status failed: " << e.what() << LogStream::eos;
		return;
	}

	auto str = oss.str();
	for (auto& addr : m_DirServerAddrs)
	{
		m_DirUdpSocket.SendTo(str.c_str(), str.length(), addr);
	}
}