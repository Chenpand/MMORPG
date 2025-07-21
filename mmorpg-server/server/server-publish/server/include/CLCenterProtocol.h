#ifndef _CL_CENTER_PROTOCOL_H_
#define _CL_CENTER_PROTOCOL_H_

#include <AvalonPacket.h>
#include "CLProtocolDefine.h"
#include <CLEnterGameProtocol.h>
#include <CLAntiAddictDefine.h>

namespace CLProtocol
{
	/**
	 *@brief center->zone 通知已连接                                                                     
	 */
	class CenterNotifyConnected : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_NOTIFY_CONNECTED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	 *@brief zone->center 请求禁言列表
	 */
	class CenterForbidTalkListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_FORBIDTALK_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief center->zone 返回禁言列表
	 */
	class CenterForbidTalkListRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_FORBIDTALK_LIST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & players;
		}

		std::vector<UInt32>	players;
	};

	/**
	 *@brief center->zone 中心服务器通知游戏区新玩家被禁言
	 */
	class CenterNotifyNewForbidTalk : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_NOTIFY_NEW_FORBIDTALK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}

		UInt32 accid;
	};

	/**
	 *@brief center->zone 中心服务器通知游戏区玩家被解禁
	 */
	class CenterNotifyDelForbidTalk : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_NOTIFY_DEL_FORBIDTALK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}

		UInt32 accid;
	};

	/**
	 *@brief zone->center 请求禁言
	 */
	class CenterForbidTalkReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_FORBIDTALK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}

		UInt32	accid;
	};

	/**
	 *@brief zone->center 取消禁言
	 */
	class CenterUnForbidTalkReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_UNFORBIDTALK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}

		UInt32 accid;
	};

	/**
	 *@brief client->center 聊天监控客户端登录
	 */
	class CenterChatMonitorLogin : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_CHATMONITOR_LOGIN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & passwd;
		}

		std::string	passwd;
	};

	/**
	 *@brief client->center 设置监控的区列表
	 */
	class CenterChatMonitorSetServers : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_CHATMONITOR_SETSERVERS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & servers;
		}

		std::vector<UInt32> servers;
	};

	/**
	 *@brief center->client 向监控客户端同步聊天信息
	 */
	class CenterSyncChatInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_SYNC_CHATINFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & serverId & accId & roleUid & channel & time & name & word & tarId & level & tarName & pf & qudao;
		}

		//服务器id
		UInt32	serverId;
		//说话人账号id
		UInt32	accId;
		//说话人角色uid
		ObjID_t roleUid;
		//频道
		UInt8		channel;
		//时间
		UInt32		time;
		//说话人名字
		std::string	name;
		//语句
		std::string	word;
		//说话对象id
		ObjID_t tarId;
		//说话人等级
		UInt32 level;
		//目标名字
		std::string tarName;
		//平台
		std::string pf;
		//渠道
		std::string qudao;
	};

	/**
	 *@brief server->center 转发消息到对应区                                                                     
	 */
	class CenterTransmitToZone : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_TRANSMIT_TO_ZONE)
	public:
		CenterTransmitToZone(Avalon::Packet* _packet){ packet = _packet; }
		CenterTransmitToZone(){ packet = Avalon::Packet::Create(); }
		~CenterTransmitToZone(){ Avalon::Packet::Destroy(packet); }

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & (*packet);
		}

		void Detach(){ packet = NULL; }

		//区id
		UInt32	zoneId;
		//消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief server->center 广播消息到对应区                                                                     
	 */
	class CenterBroadcastToZones : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_BROADCAST_TO_ZONES)
	public:
		CenterBroadcastToZones(Avalon::Packet* _packet){ packet = _packet; }
		CenterBroadcastToZones(){ packet = Avalon::Packet::Create(); }
		~CenterBroadcastToZones(){ Avalon::Packet::Destroy(packet); }

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & (*packet);
		}

		void Detach(){ packet = NULL; }
		
		//消息包
		Avalon::Packet* packet;
	};

	/**
	*@brief server->center 将推送通知发到center
	*/
	class CenterPushNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_PUSH_NOTIFY)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & platform & accid & title & content;
		}

		std::string platform;
		UInt32 accid;
		std::string title;
		std::string content;
	};

	/**
	*@brief server->center 请求客服系统签名
	*/
	class CenterCustomServiceSignReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_CUSTOM_SERVICE_SIGN_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & platform & info;
		}

		ObjID_t roleId;
		std::string platform;
		std::string info;
	};

	/**
	*@brief server->center 封号转发到其他zone
	*/
	class CenterForbidAccountReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(FORBIDACCOUNT_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accidList & time;
		}

		std::vector<UInt32> accidList;
		UInt32 time;
	};
	
	/**
	*@brief center->server 返回客服系统签名
	*/
	class CenterCustomServiceSignRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_CUSTOM_SERVICE_SIGN_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & sign;
		}

		ObjID_t			roleId;
		UInt32			result;
		std::string		sign;
	};


	/**
	*@brief center->server 封号转发到其他zone
	*/
	class CenterForbidAccountNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(FORBIDACCOUNT_NOTIFY)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accidList & time;
		}

		std::vector<UInt32> accidList;
		UInt32 time;
	};

	/**
	*@brief gate->center 上报玩家作弊
	*/
	class CenterReportPlayerCheat : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_REPORT_PLAYER_CHEAT)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneid & platform & openid & accid & name & roleId & occu & level & roleCreateTime & loginTime & ip
				& isForbidLogin & vipLevel & totalCharge & time & reason & param1 & param2;
		}

		UInt32					zoneid;
		std::string				platform;
		std::string				openid;
		UInt32					accid;
		std::string				name;
		ObjID_t					roleId;
		UInt8					occu;
		UInt16					level;
		UInt32					roleCreateTime;
		UInt32					loginTime;
		std::string				ip;
		UInt8					isForbidLogin;
		UInt16					vipLevel;
		UInt32					totalCharge;
		// 时间
		UInt32					time;
		// 作弊原因，对应枚举
		UInt32					reason;
		// 参数1
		UInt32					param1;
		// 参数2
		UInt32					param2;
	};

	/**
	*@brief gate->center 上报角色列表
	*/
	class CenterReportPlayerRoleList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_REPORT_PLAYER_ROLE_LIST)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneid & platform & openid & roles;
		}

		UInt32					zoneid;
		std::string				platform;
		std::string				openid;
		std::vector<RoleInfo>	roles;
	};

	/**
	*@brief world->center 请求GloberServer信息
	*/
	class CenterGlobalServerInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_GLOBAL_SERVER_INFO_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & platform & openid & roleId;
		}

		std::string	platform;
		std::string	openid;
		ObjID_t		roleId;
	};

	/**
	*@brief center->world 请求GloberServer信息返回
	*/
	class CenterGlobalServerInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_GLOBAL_SERVER_INFO_RET)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & platform & openid & roleId & vipLevel;
		}

		std::string	platform;
		std::string	openid;
		ObjID_t		roleId;
		std::string vipLevel;
	};

	/**
	*@brief scene->gate->center 同步玩家沉迷时间到GlobalServer
	*/
	class CenterPlayerAddictTimeSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_SYNC_PLAYER_ADDICT_TIME)
		
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & platform & openid & accid & roleid & addictTime;
		}

		std::string platform;
		std::string openid;
		UInt32 accid;
		ObjID_t	roleid;
		UInt32 addictTime;
	};

	/**
	*@brief scene->gate->center 请求获取转换的账号
	*/
	class CenterQueryConvertAccountReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_QUERY_CONVERT_ACCOUNT_REQ)

	public:
		CenterQueryConvertAccountReq() : fromGate(0), accid(0), roleid(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fromGate & accid & roleid & platform & openid;
		}

		// 是否是gateserver请求的
		UInt8		fromGate;
		UInt32		accid;
		ObjID_t		roleid;
		std::string platform;
		std::string openid;
	};
	
	/**
	*@brief center->server 返回转换的账号
	*/
	class CenterQueryConvertAccountRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_QUERY_CONVERT_ACCOUNT_RES)

	public:
		CenterQueryConvertAccountRes() : fromGate(0), accid(0), roleid(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fromGate & accid & roleid & account & password;
		}

		// 是否是gateserver请求的
		UInt8		fromGate;
		UInt32		accid;
		ObjID_t		roleid;
		std::string account;
		std::string password;
	};

	/**
	*@brief secne->center 
	*/
	class CenterActivityYearSortListSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_SYNC_ACTIVITY_SORT_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & platform & openId & zoneName & zoneId & id & accId & name & occu & level & value1 & value2 & value3;
		}

		UInt8 sortType;
		std::string platform;
		std::string openId;
		std::string zoneName;
		UInt32 zoneId;
		ObjID_t id;
		UInt32 accId;
		std::string name;
		UInt8 occu;
		UInt16 level;
		UInt32 value1;
		UInt32 value2;
		UInt32 value3;
	};

	/**
	*@brief world->center 请求惩罚信息
	*/
	class CenterPunishInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_PUNISH_INFO_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneid & centerPlatform;
		}

		UInt32 zoneid;
		std::string centerPlatform;
	};

	/**
	*@brief scene->center 向中心服务器同步个人总红包信息
	*/
	class CenterSyncRedPacketInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_SYNC_RED_PACKET_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & accId & roleUid & totalMoney & roleName & serverName;
		}

		//服id
		UInt32	zoneId;
		//账号id
		UInt32	accId;
		//角色uid
		ObjID_t	roleUid;
		//红包总点券
		UInt32	totalMoney;
		//玩家名字
		std::string	roleName;
		//服名字
		std::string	serverName;
	};

	/**
	*@brief world->center 删除个人总红包信息
	*/
	class CenterDeleteRedPacketInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_DELETE_RED_PACKET_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & accId;
		}

		//服id
		UInt32	zoneId;
		//账号id
		UInt32	accId;
	};

	/**
	*@brief scene->center 账号登陆时间同步
	*/
	class CenterAccountLoginTimeSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_ACCOUNT_LOGIN_TIME_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & openId & platform & roleId & loginTime;
		}

		//openid
		std::string openId;
		//platform
		std::string platform;
		//角色id
		UInt64 roleId;
		//登录时间
		UInt32 loginTime;
	};

};

#endif
