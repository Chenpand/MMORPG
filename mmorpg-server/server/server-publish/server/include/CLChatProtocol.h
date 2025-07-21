#ifndef _CL_CHAT_PROTOCOL_H_
#define _CL_CHAT_PROTOCOL_H_

#include <AvalonPacket.h>
#include <CLProtocolDefine.h>
#include <CLGameDefine.h>
#include <CLChatDefine.h>
#include <CLParameter.h>

namespace CLProtocol
{
	/**
	 *@brief client->scene 请求聊天
     */
	class SceneChat : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & channel & targetId & word & bLink & voiceKey & voiceDuration & isShare;
		}

		//频道类型
		UInt8		channel;
		//对方id
		ObjID_t		targetId;
		//语句
		std::string word;
		//是否link
		UInt8		bLink;
		//语音key
		std::string voiceKey;
		//语音长度
		UInt8		voiceDuration;
		//是否分享
		UInt8		isShare;
	};

	//GM命令执行返回
	class SceneNotifyExecGmcmd: public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_EXEC_GMCMD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & suc;
		}

		//成功与否
		UInt8 suc;
	};


	/**
	 *@brief scene->client 同步聊天
	 */
	class SceneSyncChat : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_CHAT)
	public:
		SceneSyncChat()
		{
			channel = 0;
			objid = 0;
			sex = 0;
			occu = 0;
			level = 0;
			viplvl = 0;
			receiverId = 0;
			bLink = 0;
			isGm = 0;
			voiceDuration = 0;
			mask = 0;
			headFrame = 0;
			zoneId = ZONE_ID;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & channel & objid & sex & occu & level & viplvl/* & qqVipInfo*/ & objname & receiverId & 
				word & bLink & isGm & voiceKey & voiceDuration & mask & headFrame & zoneId;
		}
		
		//频道
		UInt8		channel;
		//说话人id
		ObjID_t		objid;
		//性别
		UInt8		sex;
		//职业
		UInt8		occu;
		//等级
		UInt16		level;
		//vip等级
		UInt8		viplvl;
		//qq特权信息;
		QQVipInfo	qqVipInfo;
		//说话人名字
		std::string	objname;
		//接收者id
		ObjID_t		receiverId;
		//语句
		std::string	word;
		//是否link
		UInt8		bLink;
		//是否gm
		UInt8		isGm;
		//语音key
		std::string voiceKey;
		//语音长度
		UInt8		voiceDuration;
		//mask
		UInt32		mask;
		// 头像框
		UInt32		headFrame;
		// 区服id
		UInt32		zoneId;
	};

	/**
	 *@brief scene->world 缓存聊天链接数据 格式:objid + dataid + 所属玩家名 + data(见CLItemProtocol.h)或(CLPetProtocol.h)
	 */
	class WorldCacheChatDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CACHE_CHATDATA_REQ)
	public:

	};

	/**
	 *@brief client->world 请求链接详细数据
	 */
	class WorldChatLinkDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHAT_LINKDATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream  & type & id & queryType & zoneId;
		}

		UInt8	type;
		ObjID_t	id;
		UInt32 queryType;
		UInt32 zoneId;
	};

	/**
	 *@brief scene->client 返回链接数据 格式:type(UInt8) + objid + dataid + 所属玩家名 + data(见CLItemProtocol.h)
	 */
	class WorldChatLinkDataRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHAT_LINKDATA_RET)
	public:
	};

	/**
	 *@brief scene->world 通过world转发聊天协议
	 */
	class WorldTransmitChat : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TRANSMIT_CHAT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & targetId & syncChat;
		}

		ObjID_t targetId;
		SceneSyncChat syncChat;
	};

	/**
	 *@brief server->client 系统提示，服务器主动发出
	 */
	class SysNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_NOTIFY_INFO)
	public:
		SysNotify():channel(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			//by huchenhui
			//stream & type & channel & color & word;
			stream & type & word;
		}

		//类型
		UInt16 type;
		//频道类型
		UInt8 channel;
		//颜色
		UInt8 color;
		//语句
		std::string word;
	};

	/**
	*@brief server->client 系统公告，服务器主动发出
	*/
	class SysAnnouncement : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_ANNOUNCEMENT_INFO)
	public:
		SysAnnouncement(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & word;
		}

		//ID
		UInt32 id;

		//内容
		std::string word;

	};

	/**
	 *@brief scene->world
	 */
	class SysTransmitGmCommand : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_GMCOMMAND)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream  & playerid & cmdname & params;
		}

		//gm guid
		ObjID_t	playerid;
		//指令名
		std::string cmdname;
		//参数
		CLParameter params;
	};

	/**
	 *@brief client->server 发出请求
	 */
	class SceneRequest : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REQUEST)
	public:
		SceneRequest():type(0), target(0), id(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & target & targetName & id;
		}

	public:
		//类型
		UInt8	type;
		//对方
		ObjID_t	target;
		//对方名字
		std::string	targetName;
		//附件id
		UInt32	id;
	};

	/**
	 *@brief server->client 同步请求
	 */
	class SceneSyncRequest : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_REQUEST)
	public:
		SceneSyncRequest():type(0), requester(0), requesterOccu(0), requesterLevel(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & requester & requesterName & requesterOccu & requesterLevel & param1 & vipLv & avatar & activeTimeType & masterType & regionId & declaration;
		}

	public:
		//类型
		UInt8	type;
		//请求者
		ObjID_t	requester;
		//请求者名字
		std::string	requesterName;
		//请求者性别
		UInt8	requesterOccu;
		//请求者等级
		UInt16	requesterLevel;
		//附带参数1
		std::string	param1;
		//vip等级
		UInt8	vipLv;
		//外观信息
		PlayerAvatar	avatar;
		//在线时间类型
		UInt8		activeTimeType;
		//师傅类型
		UInt8		masterType;
		//地区id
		UInt8		regionId;
		//宣言
		std::string	declaration;
	};

	/**
	 *@brief client->server 答复
	 */
	class SceneReply : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REPLY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & requester & result;
		}

	public:
		//类型
		UInt8	type;
		//请求者
		ObjID_t	requester;
		//结果	1为接收 0为拒接
		UInt8	result;
	};

	/**
	 *@brief scene->world 转发请求
	 */
	class WorldTransmitRequest : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TRANSMIT_REQUEST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & requester & target & targetName & id;
		}

	public:
		//类型
		UInt8	type;
		//请求者
		ObjID_t	requester;
		//目标
		ObjID_t	target;
		//目标名
		std::string	targetName;
		//id
		UInt32	id;
	};

	/**
	 *@brief world->scene 转发答复
	 */
	class WorldTransmitReply : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TRANSMIT_REPLY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & requester & target & result;
		}

	public:
		//类型;
		UInt8	type;
		//请求者
		ObjID_t	requester;
		//目标
		ObjID_t	target;
		//结果
		UInt8	result;
	};

	/**
	 *@brief client->scene 发送位置
	 */
	class SceneChatSendPos : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAT_SENDPOS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & channel & targetId;
		}

	public:
		//频道
		UInt8	channel;
		//目标
		ObjID_t	targetId;
	};

	/**
	*@brief client->scene 请求发送喇叭
	*/
	class SceneChatHornReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAT_HORN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & content & num;
		}

	public:
		// 喇叭内容
		std::string		content;
		// 一次性发送的喇叭数量
		UInt8			num;
	};

	/**
	*@brief scene->client 返回发送喇叭结果
	*/
	class SceneChatHornRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAT_HORN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32			result;
	};

	/**
	 *@brief client->server 请求离线公告                                                                     
	 */
	class WorldOfflineNotifyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_OFFLINENOTIFY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

	public:
		std::string type;
	};

	/**
	 *@brief server->client 返回离线公告                                                                     
	 */
	class WorldfflineNotifyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_OFFLINENOTIFY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & notifys;
		}

	public:
		std::string	type;
		std::vector<std::string> notifys;
	};

	/**
	 *@brief server->client 同步新离线公告                                                                     
	 */
	class WorldSyncOfflineNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_OFFLINENOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & word;
		}
	public:
		std::string	type;
		std::string	word;
	};

	/**
	 *@brief scene->world 发送一个离线公告                                                                     
	 */
	class WorldSendOfflineNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SEND_OFFLINENOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & word;
		}

	public:
		std::string	type;
		std::string	word;
	};

	/**
	*@brief scene->world 发送一个系统公告
	*/
	class WorldSendAnnouncement : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SEND_ANNOUNCEMENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildId & id & content & startTime;
		}

	public:
		UInt64 guildId;
		UInt32 id;
		std::string content;
		UInt32 startTime;
	};

	/**
	*@brief scene->world->client 广播喇叭给客户端
	*/
	class WorldChatHorn : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHAT_HORN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		// 喇叭信息
		HornInfo		info;
	};

	/**
	*@brief client->world 请求客服系统签名
	*/
	class WorldCustomServiceSignReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CUSTOM_SERVICE_SIGN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		// 加密信息
		std::string		info;
	};
	
	/**
	*@brief world->client 客服系统签名返回
	*/
	class WorldCustomServiceSignRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CUSTOM_SERVICE_SIGN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & sign;
		}

	public:
		UInt32			result;
		// 签名
		std::string		sign;
	};
}

#endif
