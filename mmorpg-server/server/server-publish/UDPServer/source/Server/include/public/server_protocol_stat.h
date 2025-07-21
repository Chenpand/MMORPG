#ifndef __SERVER_PROTOCOL_STAT_H__
#define __SERVER_PROTOCOL_STAT_H__

#include "gameplatform_types.h"
#include "Common/common_singleton.h"
#include "Common/common_logengine.h"

namespace KingNet { namespace Server { 

	enum enmProtocolStatistical
	{
		max_protocol_stat_count		= 2048,
		max_protocol_string_name	= 32,
		max_stat_file_size			= 0x40000000,
		max_stat_file_count			= 16,
		max_protocol_body_size		= 0x8000,	//单个协议体最大32K
		default_dump_stat_info_interval		= 60000,	//60秒打印一次统计信息
	};

	enum enmMsgDirection
	{
		to_client		= 0,	
		from_client		= 1,
		to_logic		= 2,
		from_logic		= 3,
		to_hall			= 4,
		from_hall		= 5,
		to_catalog		= 6,
		from_catalog	= 7,
		to_gamedb		= 8,
		from_gamedb		= 9,
		to_itemdb		= 10,
		from_itemdb		= 11,
		to_frienddb		= 12,
		from_frienddb	= 13,
		to_score		= 14,
		from_score		= 15,
		to_profile		= 16,
		from_profile	= 17,
		to_state		= 18,
		from_state		= 19,
		to_51pay		= 20,
		from_51pay		= 21,
		to_config		= 22,
		from_config		= 23,
		to_monitor		= 24,
		from_monitor	= 25,
		to_gm			= 26,
		from_gm			= 27,
		to_hotnews		= 28,
		from_hotnews	= 29,
		to_market       = 30,
		from_market     = 31,
		to_chest        = 32,
		from_chest      = 33,	
		to_webserver    = 34,
		from_webserver  = 35,
		to_miscserver	= 36,
		from_miscserver	= 37,
		to_groupdb		= 38,
		from_groupdb	= 39,
		to_action		= 40,
		from_action		= 41,
		to_vip			= 42,
		from_vip		= 43,
		to_magic		= 44,
		from_magic		= 45,
		to_ad			= 46,
		from_ad			= 47,
	};

	class CProtocolInfo
	{
	public:
		CProtocolInfo();
		~CProtocolInfo();
		void clear();

	public:
		int16_t m_nMessageID;			//协议ID
		int8_t  m_cMessageDirection;	//协议方向，见enmMsgDirection
		char	m_szName[max_protocol_string_name];	//协议名称
		int32_t m_iCount;		//协议数量
		size_t	m_TotalSize;	//协议总流量
	};

	int protocol_compare(const void* v1, const void* v2);

	class CProtocolStat
	{
	public:
		CProtocolStat(void);
		~CProtocolStat(void);

		//多线程版本时候，不能使用底下的宏，要初始化日志文件前缀，一个线程对应一个CProtocolStat成员和相应的线程独立日志文件
		//确保每个线程的日志文件前缀不同，且前缀字符串长度适中
		int32_t Init(const char* pszFileNamePrefix);

		//在服务器初始化的时候注册要统计的协议，如：REGISTER_PROTOCOL(MSG_HALL_LOGIN, from_client, "CCSRequestLogin")
		int32_t Register(int16_t nMessageID, enmMsgDirection iDirection, const char* pszMessageName);
		
		//在收到和发送协议包的时候，更新此条协议的流量，如：REFRESH_PROTOCOL(head.m_iMessageID, from_client, (size_t)iInLength)
		int32_t Refresh(int16_t nMessageID, enmMsgDirection iDirection, size_t MessageSize);

		//该函数打印统计信息
		//每个服务器都自己在主循环中加入检测，把这次检测至上次检测的时间差(毫秒)传入iInterval参数，如：DUMP_PROTOCOL(iInterval)
		void	Dump(int32_t iInterval);

		//iInterval:dump消息统计信息的间隔.单位:ms
		void SetDumpInterval(int32_t iInterval);

	private:
		CProtocolInfo* find(int16_t nMessageID, enmMsgDirection iDirection);
		int32_t sort();
		int32_t log(const char* szContent, ...);

	private:
		int16_t m_nCount;
		CProtocolInfo m_stProtoalInfos[max_protocol_stat_count];
		CLogEngine m_stLogEngine;
		int32_t m_iPrintDelay;

		int32_t m_iDumpInterval;
		char m_szFileNamePrefix[max_file_name_length];
	};

	typedef Singleton<CProtocolStat>	PROTOCOLSTAT;

	#define REGISTER_PROTOCOL	PROTOCOLSTAT::Instance().Register
	#define REFRESH_PROTOCOL	PROTOCOLSTAT::Instance().Refresh
	#define DUMP_PROTOCOL		PROTOCOLSTAT::Instance().Dump
	#define SET_DUMP_PROTOCOL_INTERVAL	PROTOCOLSTAT::Instance().SetDumpInterval
}}

#endif  //__SERVER_PROTOCOL_STAT_H__
