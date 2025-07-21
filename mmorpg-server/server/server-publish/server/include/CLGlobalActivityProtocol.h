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
	*@brief globalActivity->node 服务器不存在
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
	*@brief world->activity 活动信息请求
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
	*@brief globalActivity->node 同步活动
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
	*@brief globalActivity->world 发送公告
	*/
	class GASSendAnnounceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_SEND_ANNOUNCE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & announcements;
		}

		// 公告
		std::vector<AnnouncementData> announcements;
	};

	/**
	*@brief globalActivity->world 发送邮件
	*/
	class GASSendMailReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_SEND_MAIL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & receiverUId & senderName & title & content & reason & rewards;
		}

		// 接收者
		std::string receiverUId;
		// 发送者
		std::string senderName;
		// 标题
		std::string title;
		// 内容
		std::string content;
		// 日志相关
		std::string reason;
		// 邮件内容
		ItemRewardVec rewards;
	};

	/**
	*@brief world->globalActivity GM命令请求
	*/
	class GASGMCommand : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GM_COMMAND)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cmdName & param;
		}

		// 命令名称
		std::string cmdName ;
		// 命令参数
		CLParameter param;
	};

	/**
	*@brief world->globalActivity 收到来自globalActivity赌马奖励邮件回复
	*/
	class GASBetHorseMailRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BET_HORSE_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerUid;
		}

		// 玩家id
		std::string playerUid;
	};

	/**
	*@brief scene->globalActivity 全民抢购数量上报
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
	*@brief globalActivity->scene 全民抢购折扣下发
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
	*@brief client->globalActivity 全民抢购数据请求
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
	*@brief globalActivity->client 全民抢购数据返回
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
	*@brief scene->globalActivity 全民抢购折扣查询
	*/
	class GASWholeBargainDiscountQuery : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WHOLE_BARGAIN_DISCOUNT_QUERY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & actId;
		}

		// 活动id
		UInt32 actId;
	};
}

#endif
