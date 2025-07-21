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
		max_protocol_body_size		= 0x8000,	//����Э�������32K
		default_dump_stat_info_interval		= 60000,	//60���ӡһ��ͳ����Ϣ
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
		int16_t m_nMessageID;			//Э��ID
		int8_t  m_cMessageDirection;	//Э�鷽�򣬼�enmMsgDirection
		char	m_szName[max_protocol_string_name];	//Э������
		int32_t m_iCount;		//Э������
		size_t	m_TotalSize;	//Э��������
	};

	int protocol_compare(const void* v1, const void* v2);

	class CProtocolStat
	{
	public:
		CProtocolStat(void);
		~CProtocolStat(void);

		//���̰߳汾ʱ�򣬲���ʹ�õ��µĺ꣬Ҫ��ʼ����־�ļ�ǰ׺��һ���̶߳�Ӧһ��CProtocolStat��Ա����Ӧ���̶߳�����־�ļ�
		//ȷ��ÿ���̵߳���־�ļ�ǰ׺��ͬ����ǰ׺�ַ�����������
		int32_t Init(const char* pszFileNamePrefix);

		//�ڷ�������ʼ����ʱ��ע��Ҫͳ�Ƶ�Э�飬�磺REGISTER_PROTOCOL(MSG_HALL_LOGIN, from_client, "CCSRequestLogin")
		int32_t Register(int16_t nMessageID, enmMsgDirection iDirection, const char* pszMessageName);
		
		//���յ��ͷ���Э�����ʱ�򣬸��´���Э����������磺REFRESH_PROTOCOL(head.m_iMessageID, from_client, (size_t)iInLength)
		int32_t Refresh(int16_t nMessageID, enmMsgDirection iDirection, size_t MessageSize);

		//�ú�����ӡͳ����Ϣ
		//ÿ�����������Լ�����ѭ���м����⣬����μ�����ϴμ���ʱ���(����)����iInterval�������磺DUMP_PROTOCOL(iInterval)
		void	Dump(int32_t iInterval);

		//iInterval:dump��Ϣͳ����Ϣ�ļ��.��λ:ms
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
