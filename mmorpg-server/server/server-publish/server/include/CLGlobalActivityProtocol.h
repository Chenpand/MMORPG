#ifndef _CL_GLOBAL_ACTIVITY_PROTOCOL_H_
#define _CL_GLOBAL_ACTIVITY_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLItemDefine.h"
#include "CLAnnouncementDefine.h"
#include "CLGlobalActivityServerDefine.h"
#include <CLParameter.h>

namespace CLProtocol
{

	/**
	*@brief globalActivity->node ������������
	*/
	class GASNonExistent : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_NON_EXISTENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->activity ���Ϣ����
	*/
	class GASActivityInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ACTIVIYT_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & adminNodeId;
		}

		UInt32 adminNodeId;
	};

	/**
	*@brief globalActivity->node ͬ���
	*/
	class GASActivitySync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ACTIVITY_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActs;
		}

		OpActivityInfoObjectVec opActs;
	};

	/**
	*@brief globalActivity->world ���͹���
	*/
	class GASSendAnnounceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_SEND_ANNOUNCE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & announcements;
		}

		// ����
		std::vector<AnnouncementData> announcements;
	};

	/**
	*@brief globalActivity->world �����ʼ�
	*/
	class GASSendMailReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_SEND_MAIL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & receiverUId & senderName & title & content & reason & rewards;
		}

		// ������
		std::string receiverUId;
		// ������
		std::string senderName;
		// ����
		std::string title;
		// ����
		std::string content;
		// ��־���
		std::string reason;
		// �ʼ�����
		ItemRewardVec rewards;
	};

	/**
	*@brief world->globalActivity GM��������
	*/
	class GASGMCommand : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GM_COMMAND)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cmdName & param;
		}

		// ��������
		std::string cmdName ;
		// �������
		CLParameter param;
	};

	/**
	*@brief world->globalActivity �յ�����globalActivity�������ʼ��ظ�
	*/
	class GASBetHorseMailRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BET_HORSE_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerUid;
		}

		// ���id
		std::string playerUid;
	};

	/**
	*@brief scene->globalActivity ȫ�����������ϱ�
	*/
	class GASWholeBargainJoinNumSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WHOLE_BARGAIN_JOIN_NUM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & joinNum;
		}

		UInt32 joinNum;
	};

	/**
	*@brief globalActivity->scene ȫ�������ۿ��·�
	*/
	class GASWholeBargainDiscountSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WHOLE_BARGAIN_DISCOUNT_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & discount;
		}

		UInt32 discount;
	};
	
	/**
	*@brief client->globalActivity ȫ��������������
	*/
	class GASWholeBargainReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WHOLE_BARGAIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief globalActivity->client ȫ���������ݷ���
	*/
	class GASWholeBargainRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WHOLE_BARGAIN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerJoinNum & joinNum & maxNum & discount;
		}

		UInt32 playerJoinNum;
		UInt32 joinNum;
		UInt32 maxNum;
		UInt32 discount;
	};

	/**
	*@brief scene->globalActivity ȫ�������ۿ۲�ѯ
	*/
	class GASWholeBargainDiscountQuery : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WHOLE_BARGAIN_DISCOUNT_QUERY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & actId;
		}

		// �id
		UInt32 actId;
	};
}

#endif
