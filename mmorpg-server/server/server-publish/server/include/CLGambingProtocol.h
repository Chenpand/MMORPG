#ifndef _CL_GAMBING_PROTOCOL_H_
#define _CL_GAMBING_PROTOCOL_H_


#include "CLProtocolDefine.h"
#include "CLItemDefine.h"


namespace CLProtocol
{
	/**
	*@brief 夺宝参与者数据
	*/
	struct GambingParticipantInfo
	{
		GambingParticipantInfo()
		{
			participantId = 0;
			gambingItemId = 0;
			groupId = 0;
			investCopies = 0;
			investCurrencyId = 0;
			investCurrencyNum = 0;
			status = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & participantId & participantENPlatform & participantPlatform & participantServerName
				& participantName & gambingItemId & groupId & investCopies & investCurrencyId & investCurrencyNum 
				& gambingRate & status;
		}

		// roleid
		ObjID_t participantId;
		// 参与者平台(英文)
		std::string participantENPlatform;
		// 参与者平台(中文)
		std::string participantPlatform;
		// 参与者服务器
		std::string participantServerName;
		// 参与者名称
		std::string participantName;
		// 夺宝商品id
		UInt32 gambingItemId;
		// 夺宝组序号
		UInt16 groupId;
		// 投入份数
		UInt32 investCopies;
		// 投入货币id
		UInt32 investCurrencyId;
		// 投入货币
		UInt32 investCurrencyNum;
		// 夺宝几率
		std::string gambingRate;
		// 状态(对应枚举 GambingMineStatus)
		UInt8  status;
	};

	/**
	*@brief 夺宝商品信息
	*/
	struct GambingItemInfo
	{
		GambingItemInfo()
		{
			gambingItemId = 0;
			gambingItemNum = 0;
			sortId = 0;
			itemDataId = 0;
			costCurrencyId = 0;
			unitPrice = 0;
			restGroups = 0;
			totalGroups = 0;
			curGroupId = 0;
			statusOfCurGroup = 0;
			soldCopiesInCurGroup = 0;
			totalCopiesOfCurGroup = 0;
			sellBeginTime = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingItemId & gambingItemNum & sortId & itemDataId & costCurrencyId & unitPrice & restGroups & totalGroups & rewardsPerCopy & curGroupId
				& statusOfCurGroup & soldCopiesInCurGroup & totalCopiesOfCurGroup & sellBeginTime & mineGambingInfo;
		}

		// 夺宝商品id
		UInt32 gambingItemId;
		// 夺宝商品数量
		UInt32 gambingItemNum;
		// 排序
		UInt16 sortId;
		// 道具表id
		UInt32 itemDataId;
		// 花费货币id
		UInt32 costCurrencyId;
		// 单价(一份)
		UInt32 unitPrice;
		// 剩余组数
		UInt16 restGroups;
		// 总的组数
		UInt16 totalGroups;
		// 每份奖励
		std::vector<ItemReward> rewardsPerCopy;

		// 当前组序号
		UInt16 curGroupId;
		// 当前组状态(对应枚举 GambingItemStatus)
		UInt8  statusOfCurGroup;
		// 当前组已售份数
		UInt32 soldCopiesInCurGroup;
		// 当前组总份数
		UInt32 totalCopiesOfCurGroup;
		// 当前组开售时间
		UInt32 sellBeginTime;

		// 我的夺宝数据
		GambingParticipantInfo mineGambingInfo;
	};

	/**
	*@brief 我的夺宝信息
	*/
	struct GambingMineInfo
	{
		GambingMineInfo()
		{
			gambingItemId = 0;
			gambingItemNum = 0;
			sortId = 0;
			itemDataId = 0;
			groupId = 0;
			soldCopies = 0;
			totalCopies = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingItemId & gambingItemNum & sortId & itemDataId & groupId & soldCopies & totalCopies
				& mineGambingInfo & gainersGambingInfo & participantsGambingInfo;
		}

		// 商品id
		UInt32 gambingItemId;
		// 夺宝商品数量
		UInt32 gambingItemNum;
		// 排序
		UInt16 sortId;
		// 道具表id
		UInt32 itemDataId;
		// 组序号
		UInt16 groupId;
		// 已售出份数
		UInt32 soldCopies;
		// 总份数
		UInt32 totalCopies;

		// 我的夺宝数据
		GambingParticipantInfo mineGambingInfo;

		// 获得者夺宝数据
		GambingParticipantInfo gainersGambingInfo;

		// 参与者夺宝数据
		std::vector<GambingParticipantInfo> participantsGambingInfo;
	};

	/**
	*@brief 夺宝组记录
	*/
	struct GambingGroupRecordData
	{
		GambingGroupRecordData()
		{
			groupId = 0;
			gainerId = 0;
			investCurrencyId = 0;
			investCurrencyNum = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & groupId & gainerENPlatform & gainerPlatform & gainerServerName
				& gainerId & gainerName & investCurrencyId & investCurrencyNum;
		}

		// 组id
		UInt16 groupId;
		// 参与者平台(英文)
		std::string gainerENPlatform;
		// 赢家平台
		std::string gainerPlatform;
		// 赢家服务器
		std::string gainerServerName;
		// 赢家id
		ObjID_t gainerId;
		// 赢家名字
		std::string gainerName;
		// 赢家投入货币id
		UInt32 investCurrencyId;
		// 赢家投入货币数量
		UInt32 investCurrencyNum;
	};

	/**
	*@brief 夺宝商品记录
	*/
	struct GambingItemRecordData
	{
		GambingItemRecordData()
		{
			gambingItemId = 0;
			gambingItemNum = 0;
			sortId = 0;
			itemDataId = 0;
			soldOutTimestamp = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingItemId & gambingItemNum & sortId & itemDataId & soldOutTimestamp & groupRecordData;
		}

		// 商品id
		UInt32 gambingItemId;
		// 夺宝商品数量
		UInt32 gambingItemNum;
		// 排序
		UInt16 sortId;
		// 道具表id
		UInt32 itemDataId;
		// 售罄时间
		UInt32 soldOutTimestamp;

		// 组记录
		std::vector<GambingGroupRecordData>  groupRecordData;
	};

	/**
	*@brief client->server 购买夺宝商品请求
	*/
	class PayingGambleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_BUY_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingItemId & groupId & investCopies & bBuyAll;
		}

		// 夺宝商品id
		UInt32 gambingItemId;
		// 组序号
		UInt16 groupId;
		// 投入份数
		UInt32 investCopies;
		// 是否购入全部剩余份数
		UInt8  bBuyAll;
	};

	/**
	*@brief server->client 购买夺宝商品返回
	*/
	class PayingGambleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_BUY_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & gambingItemId & groupId & investCopies & costCurrencyId & costCurrencyNum & itemInfo;
		}

		// 返回码
		UInt32 retCode;
		// 夺宝商品id
		UInt32 gambingItemId;
		// 组序号
		UInt16 groupId;
		// 投入份数
		UInt32 investCopies;
		// 花费货币id
		UInt32 costCurrencyId;
		// 花费货币数
		UInt32 costCurrencyNum;

		GambingItemInfo itemInfo;
	};

	/**
	*@brief client->server 夺宝商品数据查询请求
	*/
	class GambingItemQueryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_ITEM_QUERY_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief server->client 夺宝商品数据查询返回
	*/
	class GambingItemQueryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_ITEM_QUERY_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingItems;
		}

		// 全部夺宝商品
		std::vector<GambingItemInfo> gambingItems;
	};

	/**
	*@brief client->server 我的夺宝数据查询请求
	*/
	class GambingMineQueryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_MINE_QUERY_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief server->client 我的夺宝数据查询返回
	*/
	class GambingMineQueryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_MINE_QUERY_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mineGambingInfo;
		}

		// 我的夺宝商品信息
		std::vector<GambingMineInfo> mineGambingInfo;
	};

	/**
	*@brief client->server 夺宝记录查询
	*/
	class GambingRecordQueryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_RECORD_QUERY_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief server->client 夺宝记录查询返回
	*/
	class GambingRecordQueryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_RECORD_QUERY_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingRecordDatas;
		}

		// 夺宝记录
		std::vector<GambingItemRecordData> gambingRecordDatas;
	};

	/**
	*@brief server->client 夺宝中奖同步
	*/
	class GambingLotterySync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_LOTTERY_SYNC)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gainersGambingInfo & participantsGambingInfo;
		}

		// 获得者夺宝数据
		GambingParticipantInfo gainersGambingInfo;

		// 参与者夺宝数据
		std::vector<GambingParticipantInfo> participantsGambingInfo;
	};
}


#endif