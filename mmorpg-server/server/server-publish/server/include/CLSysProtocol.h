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
	 *@brief ������
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
	 *@brief server->server ����������һ������������������
	 */
	class SysLoginReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_LOGIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid;
		}

		//�ڵ�id
		UInt32 nodeid;
	};

	/**
	 *@brief server->server ���ص�¼���
	 */
	class SysLoginRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_LOGIN_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & nodeid;
		}

		//���
		UInt32 result;
		//�Է��Ľڵ�id
		UInt32 nodeid;
	};

	/**
	 *@brief all->admin ���������ַ��Ϣ
	 */
	class SysNetAddressReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_NETADDRESS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid;
		}

		//�ڵ�
		UInt32 nodeid;
	};

	/**
	 *@brief admin->all ���������ַ��Ϣ
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

		//������ַ�б�
		CLNetAddress* netaddress;
	};

	/**
	 *@brief net->logic ֪ͨ���ӽ���
	 */
	class SysNotifyConnected : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_NOTIFY_CONNECTED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & type;
		}

		//����id
		UInt32 id;
		//��������
		UInt32 type;
	};

	/**
	 *@brief net->logic ֪ͨ���ӶϿ�
	 */
	class SysNotifyDisconnected : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_NOTIFY_DISCONNECTED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & type;
		}

		//����id
		UInt32 id;
		//��������
		UInt32 type;
	};

	/**
	 *@brief server->server ֪ͨͣ��
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
	 *@brief node->node ������֮����תЭ��
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

		//Ŀ��ڵ�
		UInt32 destnode;
		//ת������Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief node->nodes ��ת��ĳ�����͵ķ�����
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

		//Ŀ��ڵ�����
		UInt8 destnodetype;
		//ת������Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief node->node ������־
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
		//��־��
		std::string	name;
		//ʱ��(����)
		UInt64	now;
		//���ȼ�
		UInt8	priority;
		//����
		std::string	content;
	};

	/**
	 *@brief node->admin ���ݿ���־
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
		//��־��
		std::string name;
		//�˺�
		UInt32		accid;
		//��ɫid
		ObjID_t		playerid;
		//��ɫ��
		std::string	playername;
		//ʱ���
		UInt32		timestamp;
		//����
		std::map<std::string, std::string>	params;
	};

	/**
	 *@brief node->node ת����Ϣ�����
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

		//Ŀ�����
		ObjID_t	playerId;
		//��Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief server->server  �ڷ�����֮��ת�����Э��                                                                    
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

		//�ڵ�id
		UInt32	nodeId;
		//���id
		ObjID_t	playerId;
		//��Ϣ��
		Avalon::Packet* packet;
	};

	//added by aprilliu, 2016.2.23
	/* ��������ע��Э�飬 ����Relay Server�� Match Serverע�� */
	class SysRegisterRequest	: public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_REGISTER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & address;
		}

	public:

		UInt32 nodeid;				//�ڵ�ID �������������Լ����������ID
		UInt32 ability;				//��ע������������������磺Relay Server ��ʾ���Գ��ص��û������ȣ�������������Ȩ�أ����� match server�ݿ��ж�(��������-��ǰ������)��Ϊ����Ȩ�������е��ȣ�
		Avalon::SockAddr address;	//��ע��server�ĵ�ַ��Ϣ



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
		UInt32 result;				//0 -- �ɹ��� >0  ʧ�ܴ���

		UInt32 option;				//��ѡ��Ϣ��������Է��� Match Server��״̬��ͣ�� ����������
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
	*@brief world->scene �޸�ϵͳ����
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
	*@brief world->scene ���³�����
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
	*@brief world->scene �޸ĵ��³ǿ���״̬
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
	*@brief node->transfer->node ��Ϣ��ת����������
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

		// ת��ͷ
		TransferHeader tHeader;
		// ת������Ϣ��
		Avalon::Packet* packet;
	};
	
	/**
	*@brief world->scene ��ұ�ɾ����
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
	*@brief globalActivity->node ȫ�ֻ������->�ڵ���תЭ��
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

		//Ŀ�������(��Ӧö��ServerType)
		UInt32 destServer;
		//ת������Ϣ��
		Avalon::Packet* packet;
	};

	/**
	*@brief node->globalActivity �ڵ�->ȫ�ֻ��������ת������Э��
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
		//Ŀ��ڵ�
		UInt32 destNode;
		//ת������Ϣ��
		Avalon::Packet* packet;
	};

	/**
	*@brief player->teamcopy player->�ű����������Э��
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

		// ���ID
		UInt64 playerId;
		//ת������Ϣ��
		Avalon::Packet* packet;
	};

	/*
	*@brief player->UnionServer player->Union���������Э��
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
		//Ŀ�깦��
		UInt32 funType;
		// �˺�ID
		UInt32 accId;
		// ���ID
		UInt64 playerId;
		//Դ�ڵ�
		std::map<UInt32, UInt32> sourceNode;
		//ת������Ϣ��
		Avalon::Packet* packet;
	};


	/**
	 *@brief node->union ��union��תЭ��
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

		//Ŀ�깦��
		UInt32 funType;
		//ת������Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief node->node ��Ϣȷ������
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
		// Դ������
		UInt32 srcServer;
		// ȷ�ϵ���Ϣ
		Avalon::Packet* packet;
	};

	/**
	 *@brief node->node ��Ϣȷ�ϻظ�
	 */
	class SysMsgConfirmRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYS_MSG_CONFIRM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid;
		}

		//Ŀ�깦��
		UInt64 guid;
	};
};

#endif
