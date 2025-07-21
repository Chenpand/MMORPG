#ifndef _CL_GIFTBAG_PROTOCOL_H_
#define _CL_GIFTBAG_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLItemDefine.h"

namespace CLProtocol
{
	/**
	 *@brief server->client 推送在线礼包奖励
	 */
	class SceneSyncOnlineGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_ONLINE_GIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & time;
		}
		//领取时间(秒)，0表示没有礼包了
		UInt32	time;
	};

	/**
	 *@brief server->client 推送等级礼包奖励
	 */
	class SceneSyncLevelGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_LEVEL_GIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & level;
		}
		//可领取等级，0表示没有礼包了
		UInt16	level;
	};

	/**
	 *@brief client->server 领取礼包奖励
	 */
	class SceneGetGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GET_GIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}
		//礼包类型 1为在线礼包 2为等级礼包
		UInt8	type;
	};

	/**
	*@brief server->client 同步活动礼包
	*/
	class SceneSyncActiveGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_ACTIVEGIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapGiftBags;
		}
	public:
		// 礼包信息，id + 状态，状态：1 可领，2 已领
		std::map<UInt32, UInt8>	mapGiftBags;
	};

	/**
	*@brief client->server 领取活动礼包
	*/
	class SceneGetActiveGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GET_ACTIVEGIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}
	public:
		// 礼包id
		UInt32	id;
	};

	/**
	*@brief server->client 同步Q点消费量
	*/
	class SceneSyncPointUsed : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_POINTUSED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uPoint;
		}
	public:
		//Q点
		UInt32	uPoint;
	};
	
	/**
	*@brief server->client 同步节日计数
	*/
	class SceneSyncFestivalCount : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_FESTIVAL_COUNT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapInfo;
		}
	public:
		// 节日名称 + 次数，中秋节为"mid_autumn"和"mid_autumn_sum"
		std::map<std::string, UInt32>	mapInfo;
	};

	/** 
	 *@brief client->server 节日活动计数请求
	 */
	class SceneFestivalCounterReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FESTIVAL_COUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name;
		}

	public:
		//计数名
		std::string	name;
	};

	/**
	 *@brief server->client 返回节日活动计数
	 */
	class SceneFestivalCounterRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FESTIVAL_COUNT_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & value;
		}

	public:
		//计数名
		std::string	name;
		//数值
		UInt32	value;
	};

	/**
	*@brief client -> server 请求购买礼包
	*/
	class SceneBuyGiftBag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BUY_GIFTBAG)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//礼包ID
		UInt32 id;
	};

	struct GiftSyncInfo
	{
		GiftSyncInfo() : id(0), itemId(0), itemNum(0), weight(0), equipType(0), strengthen(0), isTimeLimit(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & itemId & itemNum & recommendJobs & weight & validLevels & equipType & strengthen & isTimeLimit;
		}

		// 礼品id
		UInt32 id;
		// 道具id
		UInt32 itemId;
		// 道具数量
		UInt32 itemNum;
		// 推荐职业
		std::vector<UInt8> recommendJobs;
		// 获得权重
		UInt32 weight;
		// 有效等级
		std::vector<UInt32> validLevels;
		// 装备类型
		UInt8	equipType;
		// 强化等级
		UInt32	strengthen;
		// 是否时限的
		UInt8	isTimeLimit;
	};

	struct GiftPackSyncInfo
	{
		GiftPackSyncInfo() : id(0), filterType(0), filterCount(-1), uiType(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & filterType & filterCount & gifts & uiType & description;
		}

		// 礼包ID
		UInt32 id;
		// 筛选类型
		UInt32 filterType;
		// 过滤出的礼品数量
		Int32  filterCount;
		// 礼品
		std::vector<GiftSyncInfo> gifts;
		// UI 类型
		UInt8 uiType;
		// 描述
		std::string description;
	};

	/**
	*@brief client -> scene 礼包信息请求
	*/
	class SceneGiftPackInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GIFT_PACK_INFO_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & giftPackIds;
		}

	public:
		// 礼包ID
		std::vector<UInt32> giftPackIds;
	};

	/**
	*@brief scene -> client 礼包信息返回
	*/
	class SceneGiftPackInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GIFT_PACK_INFO_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & giftPacks;
		}

	public:
		std::vector<GiftPackSyncInfo> giftPacks;
	};
}

#endif
