#ifndef _CL_VIP_PROTOCOL_H_
#define _CL_VIP_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	class SceneVipBuyItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_VIP_BUY_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & vipLevel;
		}

		UInt8 vipLevel;
	};

	class SceneVipBuyItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_VIP_BUY_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};


	class SceneGiveMoneyManageRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GIVE_MONEY_MANAGE_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rewardId;
		}

		UInt8 rewardId;
	};

	class SceneGiveMoneyManageRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GIVE_MONEY_MANAGE_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

		

		
}

#endif
