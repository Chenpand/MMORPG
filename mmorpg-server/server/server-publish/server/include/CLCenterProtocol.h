#ifndef _CL_CENTER_PROTOCOL_H_
#define _CL_CENTER_PROTOCOL_H_

#include <AvalonPacket.h>
#include "CLProtocolDefine.h"
#include <CLEnterGameProtocol.h>
#include <CLAntiAddictDefine.h>

namespace CLProtocol
{
	/**
	 *@brief center->zone ֪ͨ������                                                                     
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
	 *@brief zone->center ��������б�
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
	 *@brief center->zone ���ؽ����б�
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
	 *@brief center->zone ���ķ�����֪ͨ��Ϸ������ұ�����
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
	 *@brief center->zone ���ķ�����֪ͨ��Ϸ����ұ����
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
	 *@brief zone->center �������
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
	 *@brief zone->center ȡ������
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
	 *@brief client->center �����ؿͻ��˵�¼
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
	 *@brief client->center ���ü�ص����б�
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
	 *@brief center->client ���ؿͻ���ͬ��������Ϣ
	 */
	class CenterSyncChatInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_SYNC_CHATINFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & serverId & accId & roleUid & channel & time & name & word & tarId & level & tarName & pf & qudao;
		}

		//������id
		UInt32	serverId;
		//˵�����˺�id
		UInt32	accId;
		//˵���˽�ɫuid
		ObjID_t roleUid;
		//Ƶ��
		UInt8		channel;
		//ʱ��
		UInt32		time;
		//˵��������
		std::string	name;
		//���
		std::string	word;
		//˵������id
		ObjID_t tarId;
		//˵���˵ȼ�
		UInt32 level;
		//Ŀ������
		std::string tarName;
		//ƽ̨
		std::string pf;
		//����
		std::string qudao;
	};

	/**
	 *@brief server->center ת����Ϣ����Ӧ��                                                                     
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

		//��id
		UInt32	zoneId;
		//��Ϣ��
		Avalon::Packet* packet;
	};

	/**
	 *@brief server->center �㲥��Ϣ����Ӧ��                                                                     
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
		
		//��Ϣ��
		Avalon::Packet* packet;
	};

	/**
	*@brief server->center ������֪ͨ����center
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
	*@brief server->center ����ͷ�ϵͳǩ��
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
	*@brief server->center ���ת��������zone
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
	*@brief center->server ���ؿͷ�ϵͳǩ��
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
	*@brief center->server ���ת��������zone
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
	*@brief gate->center �ϱ��������
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
		// ʱ��
		UInt32					time;
		// ����ԭ�򣬶�Ӧö��
		UInt32					reason;
		// ����1
		UInt32					param1;
		// ����2
		UInt32					param2;
	};

	/**
	*@brief gate->center �ϱ���ɫ�б�
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
	*@brief world->center ����GloberServer��Ϣ
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
	*@brief center->world ����GloberServer��Ϣ����
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
	*@brief scene->gate->center ͬ����ҳ���ʱ�䵽GlobalServer
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
	*@brief scene->gate->center �����ȡת�����˺�
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

		// �Ƿ���gateserver�����
		UInt8		fromGate;
		UInt32		accid;
		ObjID_t		roleid;
		std::string platform;
		std::string openid;
	};
	
	/**
	*@brief center->server ����ת�����˺�
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

		// �Ƿ���gateserver�����
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
	*@brief world->center ����ͷ���Ϣ
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
	*@brief scene->center �����ķ�����ͬ�������ܺ����Ϣ
	*/
	class CenterSyncRedPacketInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_SYNC_RED_PACKET_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & accId & roleUid & totalMoney & roleName & serverName;
		}

		//��id
		UInt32	zoneId;
		//�˺�id
		UInt32	accId;
		//��ɫuid
		ObjID_t	roleUid;
		//����ܵ�ȯ
		UInt32	totalMoney;
		//�������
		std::string	roleName;
		//������
		std::string	serverName;
	};

	/**
	*@brief world->center ɾ�������ܺ����Ϣ
	*/
	class CenterDeleteRedPacketInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CENTER_DELETE_RED_PACKET_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & accId;
		}

		//��id
		UInt32	zoneId;
		//�˺�id
		UInt32	accId;
	};

	/**
	*@brief scene->center �˺ŵ�½ʱ��ͬ��
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
		//��ɫid
		UInt64 roleId;
		//��¼ʱ��
		UInt32 loginTime;
	};

};

#endif
