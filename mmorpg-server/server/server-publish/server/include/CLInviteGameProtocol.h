#ifndef _CL_INVITEGAME_PROTOCOL_H_
#define _CL_INVITEGAME_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief 被邀请者信息
	*/
	struct InviteeInfo
	{
		InviteeInfo():uid(0), bYesterdayOnline(0), level(0), offlineTime(0), onlineAwardTime(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid & openid & bYesterdayOnline & level & offlineTime & onlineAwardTime;
		}

		//玩家id
		UInt32		uid;
		//openid
		std::string	openid;
		//昨日是否在线
		UInt8		bYesterdayOnline;
		//当前等级
		UInt16		level;
		//离线时间(0表示在线)
		UInt32		offlineTime;
		//登陆奖励领取时间
		UInt32		onlineAwardTime;
	};

	/**
	 *@brief zone->SocialServer 上报玩家在线列表
	 */
	class SocialInviteReportOnline : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_REPORT_ONLINE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & players;
		}

	public:
		//玩家列表
		std::vector<UInt32>	players;
	};

	/**
	 *@brief zone->SocialServer 通知玩家升到某个等级段
	 */
	class SocialInviteNotifyLevelup : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_NOTIFY_LEVELUP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & accId & playerId & level;
		}

	public:
		//节点id
		UInt32	nodeId;
		//uid
		UInt32	accId;
		//玩家id
		ObjID_t playerId;
		//等级
		UInt16	level;
	};

	/**
	 *@brief zone->SocialServer 通知消费
	 */
	class SocialInviteNotifyConsume : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_NOTIFY_CONSUME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid & gold;
		}

	public:
		//玩家id
		UInt32	uid;
		//返还的金贝数
		UInt32	gold;
	};

	/**
	 *@brief zone->SocialServer 通知被邀请者进入游戏
	 */
	class SocialNotifyInviteeEnterGame : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_NOTIFY_INVITEE_ENTERGAME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & inviter & invitee & openid;
		}

	public:
		//邀请者
		UInt32	inviter;
		//被邀请者
		UInt32	invitee;
		//被邀请者openid
		std::string	openid;
	};

	/**
	 *@brief zone->SocialServer 请求被邀请者列表
	 */
	class SocialInviteInviteeListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_INVITEELIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & inviterId & playerId;
		}

	public:
		//服务器节点id
		UInt32	nodeId;
		//邀请者id
		UInt32	inviterId;
		//邀请者角色id
		ObjID_t	playerId;
	};

	/**
	 *@brief SocialServer->zone 返回被邀请者列表
	 */
	class SocialInviteInviteeListRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_INVITEELIST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & totalGold & invitees;
		}

	public:
		//角色id
		ObjID_t	playerId;
		//今日可领取返利
		UInt32	totalGold;
		//列表
		std::vector<InviteeInfo>	invitees;
	};

	/**
	 *@brief zone->SocialServer 请求检查礼包领取条件是否满足
	 */
	class SocialInviteCheckGiftbagReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_GIFTBAG_REQ)
	public:
		SocialInviteCheckGiftbagReq()
			:nodeId(0), inviterId(0), playerId(0), type(0), minLevel(0), maxLevel(-1), playerNum(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & inviterId & playerId & type & minLevel & maxLevel & playerNum;
		}

	public:
		//服务器节点id
		UInt32	nodeId;
		//邀请者id
		UInt32	inviterId;
		//邀请者角色id
		ObjID_t	playerId;
		//礼包类型
		UInt8	type;
		//玩家等级下限
		UInt16	minLevel;
		//玩家等级上限
		UInt16	maxLevel;
		//人数
		UInt32	playerNum;
	};

	/**
	 *@brief SocialServer->zone 请求检查礼包领取条件返回
	 */
	class SocialInviteCheckGiftbagRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_GIFTBAG_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & result;
		}

	public:
		//玩家id
		ObjID_t playerId;
		//礼包类型
		UInt8	type;
		//结果 1表示满足 0表示不满足
		UInt8	result;
	};

	/**
	 *@brief zone->SocialServer 检查召回奖励请求
	 */
	class SocialInviteCheckRecallAwardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_RECALLAWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & inviterId & playerId & inviteeId;
		}

	public:
		//服务器节点id
		UInt32	nodeId;
		//邀请者id
		UInt32	inviterId;
		//邀请者角色id
		ObjID_t	playerId;
		//被邀请者id
		UInt32	inviteeId;
	};

	/**
	 *@brief SocialServer->zone 检查召回奖励返回
	 */
	class SocialInviteCheckRecallAwardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_RECALLAWARD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & invitees;
		}

	public:
		//玩家id
		ObjID_t	playerId;
		//被领取的列表
		std::vector<InviteeInfo> invitees;
	};

	/**
	 *@brief client->scene 请求今日邀请信息
	 */
	class SceneInviteDailyInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_DAILY_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief scene->client 返回今日邀请信息
	 */
	class SceneInviteDailyInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_DAILY_INFO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & inviteTimes & inviteGold & giftbagFlag;
		}

	public:
		//次数
		UInt32	inviteTimes;
		//今日已领取返利
		UInt32	inviteGold;
		//礼包领取标识  0为邀请礼包 1-5为活跃礼包 11+为升级礼包 25,26为恐龙礼包
		UInt32	giftbagFlag;
	};

	/**
	 *@brief client->scene 请求邀请好友列表
	 */
	class SceneInviteInviteeListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_INVITEELIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief scene->client 返回邀请好友列表
	 */
	class SceneInviteInviteeListRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_INVITEELIST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & totalGold & invitees;
		}

		UInt32	totalGold;
		std::vector<InviteeInfo> invitees;
	};

	/**
	 *@brief client->scene 通知服务器已发出新的邀请
	 */
	class SceneInviteNotifyInvited : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_NOTIFY_INVITED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief client->scene 领取礼包
	 */
	class SceneInviteGetGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_GET_GIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		//礼包领取标识  0为邀请礼包 1-5为活跃礼包 11+为升级礼包 25,26为恐龙礼包
		UInt8	type;
	};

	/**
	 *@brief client->scene 领取召回奖励请求
	 */
	class SceneInviteGetRecallAwardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_GET_RECALLAWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid;
		}

	public: 
		//好友id 0-表示一键领取  否则领取单个好友的
		UInt32 uid;
	};

	/**
	 *@brief scene->client 刷新单个邀请者状态
	 */
	class SceneInviteSyncInviteeData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_SYNC_INVITEE_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		//好友信息
		InviteeInfo	info;
	};

	/**
	 *@brief client->scene 领取消费反馈
	 */
	class SceneInviteGetConsumeAwardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_GET_CONSUMEAWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief scene->client 领取消费返利返回
	 */
	class SceneInviteGetConsumeAwardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_GET_CONSUMEAWARD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & todayGoldAward & totalGold;
		}

		//今日领取数
		UInt32	todayGoldAward;
		//总金贝数
		UInt32	totalGold;
	};

	/**
	 *@brief zone->SocialServer 检查消费返利
	 */
	class SocialInviteCheckConsumeAwardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_CONSUMEAWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & inviterId & playerId & maxGold;
		}

		//服务器节点id
		UInt32	nodeId;
		//邀请者id
		UInt32	inviterId;
		//邀请者角色id
		ObjID_t	playerId;
		//领取额度
		UInt32	maxGold;
	};

	/**
	 *@brief SocialServer->zone 检查消费返利返回
	 */
	class SocialInviteCheckConsumeAwardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_CONSUMEAWARD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & gold & totalGold;
		}

		//玩家id
		ObjID_t	playerId;
		//领取额度
		UInt32	gold;
		//总额度
		UInt32	totalGold;
	};

	/**
	 *@brief server->SocialServer 请求检查恐龙活动                                                                     
	 */
	class SocialInviteCheckKLActivityReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_KLACTIVITY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & accId & playerId;
		}

		//节点id
		UInt32 nodeId;
		//账号id
		UInt32 accId;
		//玩家id
		ObjID_t	playerId;
	};

	/**
	 *@brief SocialServer->server 请求检查恐龙活动返回                                                                     
	 */
	class SocialInviteCheckKLActivityRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_KLACTIVITY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & klopenid & finishFlag;
		}

		//玩家id
		ObjID_t	playerId;
		//恐龙openid
		std::string klopenid;
		//活动完成标志
		UInt8	finishFlag;
	};

	/**
	 *@brief server->SocialServer 绑定恐龙账号                                                                     
	 */
	class SocialInviteBindKLActivity : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_BIND_KLACTIVITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & accId & playerId & level & openid & klopenid;
		}

		//节点id
		UInt32 nodeId;
		//账号id
		UInt32 accId;
		//玩家id
		ObjID_t	playerId;
		//等级
		UInt16 level;
		//openid
		std::string openid;
		//恐龙openid
		std::string klopenid;
	};

	/**
	 *@brief client->server 绑定恐龙账号                                                                     
	 */
	class SceneInviteBindKLPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_BIND_KLPLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & klopenid;
		}

		//恐龙openid
		std::string klopenid;
	};

	/**
	 *@brief server->client 同步恐龙活动完成信息                                                                     
	 */
	class SceneInviteSyncKLActivity : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_SYNC_KLACTIVITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & klopenid & finishFlag;
		}

		//恐龙openid
		std::string	klopenid;
		//任务完成标志 1-30级任务完成 2-45级任务完成 3-邀请两人完成
		UInt8	finishFlag;
	};

	/**
	 *@brief client->server 兑换cdkey                                                                     
	 */
	class SceneInviteExchangeCDKey : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_EXCHANGE_CDKEY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cdkey;
		}

		//cdkey
		std::string cdkey;
	};

	/**
	 *@brief server->SocialServer 请求开始兑换cdkey                                                                     
	 */
	class SceneInviteBeginExchangeCDKeyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_BEGIN_EXCHANGECDKEY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cdkey & nodeId  & playerId;
		}

		//cdkey
		std::string cdkey;
		//nodeid
		UInt32	nodeId;
		//playerid
		ObjID_t	playerId;
	};

	/**
	 *@brief SocialServer->server 请求开始兑换cdkey返回                                                                     
	 */
	class SceneInviteBeginExchangeCDKeyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_BEGIN_EXCHANGECDKEY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cdkey & playerId & itemId;
		}

		//cdkey
		std::string cdkey;
		//playerid
		ObjID_t	playerId;
		//礼包id
		UInt32	itemId;
	};

	/**
	 *@brief server->SocialServer 结束兑换cdkey                                                                     
	 */
	class SceneInviteEndExchangeCDKeyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_END_EXCHANGECDKEY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cdkey & openId;
		}

		//cdkey
		std::string	cdkey;
		//openid
		std::string	openId;
	};
}

#endif
