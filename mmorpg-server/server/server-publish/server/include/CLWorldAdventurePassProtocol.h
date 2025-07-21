#ifndef _CL_WORLD_ADVENTURE_PASS_PROTOCOL_H_
#define _CL_WORLD_ADVENTURE_PASS_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->world 查询冒险通行证情况
	*/
	class WorldAventurePassStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	public:

	};

	/**
	*@brief world -> client 查询冒险通行证情况返回
	*/
	class WorldAventurePassStatusRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_STATUS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lv & startTime & endTime & seasonID & exp & type & activity & normalReward & highReward;
		}

		WorldAventurePassStatusRet()
		{

			lv = 0;
			startTime = 0;
			endTime = 0;
			seasonID = 0;
			normalReward = "";
			highReward = "";
		}
	public:
		//冒险通行证等级
		UInt32 lv;
		//开始时间
		UInt32 startTime;
		//结束时间
		UInt32 endTime;
		//当前赛季id
		UInt32 seasonID;
		//经验
		UInt32 exp;
		//通行证类型
		UInt8 type;
		//账号活跃度情况
		UInt32 activity;
		//普通奖励领取情况
		std::string	normalReward;
		//高级奖励领取情况
		std::string highReward;
	};

	/**
	*@brief client->world 购买冒险通行证
	*/
	class WorldAventurePassBuyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_BUY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

	public:
		//通行证购买类型
		UInt8 type;
	};

	/**
	*@brief world -> client 购买冒险通行证返回
	*/
	class WorldAventurePassBuyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_BUY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

	public:
		//冒险通行证类型
		UInt8 type;
	};


	/**
	*@brief client->world 购买通行证等级
	*/
	class WorldAventurePassBuyLvReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_BUY_LV_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lv;
		}

	public:
		//购买的等级档次
		UInt32 lv;
	};

	/**
	*@brief world -> client 购买冒险通行证等级返回
	*/
	class WorldAventurePassBuyLvRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_BUY_LV_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lv;
		}

	public:
		//购买的等级档次 0为失败
		UInt32 lv;
	};
	

	/**
	*@brief client->world 领取查询经验包
	*/
	class WorldAventurePassExpPackReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_EXP_PACK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & op;
		}

	public:
		//0 是查询 1是领取
		UInt8 op;
	};

	/**
	*@brief world -> client 领取查询经验包返回
	*/
	class WorldAventurePassExpPackRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_EXP_PACK_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

	public:
		//0 未解锁 1已解锁未领取 2已领取
		UInt8 type;
	};

	/**
	*@brief client->world 领取通行证等级奖励
	*/
	class WorldAventurePassRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lv;
		}

	public:
		UInt32 lv;
	};

	/**
	*@brief world -> client 领取通行证等级奖励返回
	*/
	class WorldAventurePassRewardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AVENTURE_PASS_REWARD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lv & errorCode;
		}

	public:
		//请求领取的等级
		UInt32 lv;
		//领取结果
		UInt32 errorCode;
	};

}

#endif