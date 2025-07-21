#ifndef _CL_SYS_PROTOCOL_H_
#define _CL_SYS_PROTOCOL_H_

#include <AvalonPacket.h>
#include <string>
#include <CLProtocolDefine.h>
#include <CLNetAddress.h>
#include <CLDungeonConfigMgr.h>
#include <CLGlobalActivityServerDefine.h>

namespace CLProtocol
{
	/**
	 *@brief 心跳包
	 */
	class SysHeartbeat : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(0)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief server->server 服务器向另一个服务器请求建立连接
	 */
	class SysLoginReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_LOGIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid;
		}

		//节点id
		UInt32 nodeid;
	};

	/**
	 *@brief server->server 返回登录结果
	 */
	class SysLoginRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_LOGIN_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & nodeid;
		}

		//结果
		UInt32 result;
		//对方的节点id
		UInt32 nodeid;
	};

	/**
	 *@brief all->admin 请求网络地址信息
	 */
	class SysNetAddressReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_NETADDRESS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid;
		}

		//节点
		UInt32 nodeid;
	};

	/**
	 *@brief admin->all 返回网络地址信息
	 */
	class SysNetAddressRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_NETADDRESS_RET)
	public:
		SysNetAddressRet(CLNetAddress* address)
		{
			netaddress = address;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & (*netaddress);
		}

		//内网地址列表
		CLNetAddress* netaddress;
	};

	/**
	 *@brief net->logic 通知连接建立
	 */
	class SysNotifyConnected : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_NOTIFY_CONNECTED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & type;
		}

		//连接id
		UInt32 id;
		//连接类型
		UInt32 type;
	};

	/**
	 *@brief net->logic 通知连接断开
	 */
	class SysNotifyDisconnected : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_NOTIFY_DISCONNECTED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & type;
		}

		//连接id
		UInt32 id;
		//连接类型
		UInt32 type;
	};

	/**
	 *@brief server->server 通知停服
	 */
	class SysNotifyShutdown : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_NOTIFY_SHUTDOWN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief node->node 服务器之间中转协议
	 */
	class SysTransmitInNodes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_INNODES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & destnode & (*packet);
		}

		SysTransmitInNodes(){ packet = Avalon::Packet::Create(); }
		SysTransmitInNodes(Avalon::Packet* pack){ packet = pack; }
		~SysTransmitInNodes(){ Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		//目标节点
		UInt32 destnode;
		//转发的消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief node->nodes 中转到某种类型的服务器
	 */
	class SysTransmitInNodesType : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_INNODES_TYPE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & destnodetype & (*packet);
		}

		SysTransmitInNodesType(){ packet = Avalon::Packet::Create(); }
		SysTransmitInNodesType(Avalon::Packet* pack){ packet = pack; }
		~SysTransmitInNodesType(){ Avalon::Packet::Destroy(packet); }

		void Detach(){ packet = NULL; }

		//目标节点类型
		UInt8 destnodetype;
		//转发的消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief node->node 网络日志
	 */
	class SysNetLogger : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_NET_LOGGER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & now & priority & content;
		}

	public:
		//日志名
		std::string	name;
		//时间(毫秒)
		UInt64	now;
		//优先级
		UInt8	priority;
		//内容
		std::string	content;
	};

	/**
	 *@brief node->admin 数据库日志
	 */
	class SysDBLogger : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_DB_LOGGER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & accid & playerid & playername & timestamp & params;
		}

	public:
		//日志名
		std::string name;
		//账号
		UInt32		accid;
		//角色id
		ObjID_t		playerid;
		//角色名
		std::string	playername;
		//时间戳
		UInt32		timestamp;
		//参数
		std::map<std::string, std::string>	params;
	};

	/**
	 *@brief node->node 转发消息给玩家
	 */
	class SysTransmitToPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_TO_PLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & (*packet);
		}

		SysTransmitToPlayer(){	packet = Avalon::Packet::Create(); }
		SysTransmitToPlayer(Avalon::Packet* pack){ packet = pack; }
		~SysTransmitToPlayer(){ Avalon::Packet::Destroy(packet); }

		void Detach(){ packet = NULL; }

		//目标玩家
		ObjID_t	playerId;
		//消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief server->server  在服务器之间转发玩家协议                                                                    
	 */
	class SysTransmitPlayerProtocolInNode : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_PLAYERPROTOCOL_INNODES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & playerId & (*packet);
		}

		SysTransmitPlayerProtocolInNode(){ packet = Avalon::Packet::Create(); }
		SysTransmitPlayerProtocolInNode(Avalon::Packet* pack){ packet = pack; }
		~SysTransmitPlayerProtocolInNode(){ Avalon::Packet::Destroy(packet); }

		void Detach(){ packet = NULL; }

		//节点id
		UInt32	nodeId;
		//玩家id
		ObjID_t	playerId;
		//消息包
		Avalon::Packet* packet;
	};

	//added by aprilliu, 2016.2.23
	/* 服务器间注册协议， 用于Relay Server向 Match Server注册 */
	class SysRegisterRequest	: public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_REGISTER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & address;
		}

	public:

		UInt32 nodeid;				//节点ID 含服务器类型以及具体服务器ID
		UInt32 ability;				//待注册服务器的能力，比如：Relay Server 表示可以承载的用户数量等（可以用作计算权重，比如 match server据空闲度(最大承载量-当前承载量)作为调度权重来进行调度）
		Avalon::SockAddr address;	//待注册server的地址信息



	protected:
	private:
	};

	class SysRegisterResponse : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_REGISTER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & option;
		}

	public:
		UInt32 result;				//0 -- 成功， >0  失败代码

		UInt32 option;				//可选信息：比如可以返回 Match Server的状态（停服 还是正常）
	protected:
	private:
	};

    class SysAsyncWaitMsg : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SYS_ASYNC_WAIT_MSG)
    public:
        SysAsyncWaitMsg(Avalon::Packet* packet_ = NULL)
        {
            packet = packet_;
            if (!packet)
            {
                packet = Avalon::Packet::Create();
            }
            
        }
        ~SysAsyncWaitMsg()
        {
            if (packet)
            {
                Avalon::Packet::Destroy(packet);
            }
        }

        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & id & (*packet);
        }

    public:
        ObjID_t             id;
        Avalon::Packet*     packet;
    };

    class SysAsyncWaitMsgRet : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SYS_ASYNC_WAIT_MSG_RET)
    public:
        SysAsyncWaitMsgRet(Avalon::Packet* packet_ = NULL)
        {
            packet = packet_;
            if (!packet)
            {
                packet = Avalon::Packet::Create();
            }

        }
        ~SysAsyncWaitMsgRet()
        {
            if (packet)
            {
                Avalon::Packet::Destroy(packet);
            }
        }

        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & id & (*packet);
        }

    public:
        ObjID_t             id;
        Avalon::Packet*     packet;
    };

	/**
	*@brief world->scene 修改系统开关
	*/
	class SysServiceSwitchChangeStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_SERVICE_SWITCH_STATUS_CHANGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & open;
		}

		UInt8 type;
		UInt8 open;
	};

	/**
	*@brief world->scene 地下城配置
	*/
	class SysDungeonConfigUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_DUNGEON_CONFIG_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & config;
		}

		UInt32					dungeonId;
		DungeonWeeklyConfig		config;
	};
	
	/**
	*@brief world->scene 修改地下城开放状态
	*/
	class SysDungeonOpenStatusChange : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_DUNGEON_OPEN_STATUS_CHANGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & open & maxTimes;
		}

		UInt32	dungeonId;
		UInt8	open;
		UInt8	maxTimes;
	};

	/**
	*@brief node->transfer->node 消息中转到其他区服
	*/
	class SysTransferProtocol : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSFER_PROTOCOL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tHeader & (*packet);
		}

		SysTransferProtocol() { packet = Avalon::Packet::Create(); }
		SysTransferProtocol(Avalon::Packet* pack) { packet = pack; }
		~SysTransferProtocol() { Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		std::string LogStr()
		{
			std::stringstream ss;
			ss << tHeader.LogStr();
			ss << " packid:" << packet->GetID();
			return ss.str();
		}

		// 转发头
		TransferHeader tHeader;
		// 转发的消息包
		Avalon::Packet* packet;
	};
	
	/**
	*@brief world->scene 玩家被删除了
	*/
	class SysPlayerForeverDeleted : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_PLAYER_FOREVER_DELETED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t roleId;
	};

	/**
	*@brief globalActivity->node 全局活动服务器->节点中转协议
	*/
	class SysTransmitFromGlobalToNode : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_FROM_GLOBAL_TO_NODE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & destServer & (*packet);
		}

		SysTransmitFromGlobalToNode(){ packet = Avalon::Packet::Create(); }
		SysTransmitFromGlobalToNode(Avalon::Packet* pack){ packet = pack; }
		~SysTransmitFromGlobalToNode(){ Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		//目标服务器(对应枚举ServerType)
		UInt32 destServer;
		//转发的消息包
		Avalon::Packet* packet;
	};

	/**
	*@brief node->globalActivity 节点->全局活动服务器中转玩家相关协议
	*/
	class SysTransmitPlayerFormNodesToGlobal : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_PLAYER_FORM_NODE_TO_GLOBAL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerInfo & destNode & (*packet);
		}

		SysTransmitPlayerFormNodesToGlobal(){ packet = Avalon::Packet::Create(); }
		SysTransmitPlayerFormNodesToGlobal(Avalon::Packet* pack){ packet = pack; }
		~SysTransmitPlayerFormNodesToGlobal(){ Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		PlayerTranmitInfo playerInfo;
		//目标节点
		UInt32 destNode;
		//转发的消息包
		Avalon::Packet* packet;
	};

	/**
	*@brief player->teamcopy player->团本服务器相关协议
	*/
	class SysTransmitPlayerToTeamCopy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_PLAYER_TO_TEAM_COPY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & (*packet);
		}

		SysTransmitPlayerToTeamCopy(){ packet = Avalon::Packet::Create(); }
		SysTransmitPlayerToTeamCopy(Avalon::Packet* pack){ packet = pack; }
		~SysTransmitPlayerToTeamCopy(){ Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		// 玩家ID
		UInt64 playerId;
		//转发的消息包
		Avalon::Packet* packet;
	};

	/*
	*@brief player->UnionServer player->Union服务器相关协议
	*/
	class SysTransmitPlayerToUnion : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_PLAYER_TO_UNION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & funType & accId & playerId & sourceNode & (*packet);
		}

		SysTransmitPlayerToUnion(){ packet = Avalon::Packet::Create(); }
		SysTransmitPlayerToUnion(Avalon::Packet* pack){ packet = pack; }
		~SysTransmitPlayerToUnion(){ Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}
		//目标功能
		UInt32 funType;
		// 账号ID
		UInt32 accId;
		// 玩家ID
		UInt64 playerId;
		//源节点
		std::map<UInt32, UInt32> sourceNode;
		//转发的消息包
		Avalon::Packet* packet;
	};


	/**
	 *@brief node->union 向union中转协议
	 */
	class SysTransmitToUnion : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_TRANSMIT_TO_UNION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & funType & (*packet);
		}

		SysTransmitToUnion() { packet = Avalon::Packet::Create(); }
		SysTransmitToUnion(Avalon::Packet* pack) { packet = pack; }
		~SysTransmitToUnion() { Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		//目标功能
		UInt32 funType;
		//转发的消息包
		Avalon::Packet* packet;
	};

	/**
	 *@brief node->node 消息确认请求
	 */
	class SysMsgConfirmReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_MSG_CONFIRM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & srcServer & (*packet);
		}

		SysMsgConfirmReq() { packet = Avalon::Packet::Create(); }
		SysMsgConfirmReq(Avalon::Packet* pack) { packet = pack; }
		~SysMsgConfirmReq() { Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		UInt64 guid;
		// 源服务器
		UInt32 srcServer;
		// 确认的消息
		Avalon::Packet* packet;
	};

	/**
	 *@brief node->node 消息确认回复
	 */
	class SysMsgConfirmRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_MSG_CONFIRM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid;
		}

		//目标功能
		UInt64 guid;
	};
};

#endif
