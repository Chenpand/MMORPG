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
	 *@brief client->scene ��������
     */
	class SceneChat : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHAT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & channel & targetId & word & bLink & voiceKey & voiceDuration & isShare;
		}

		//Ƶ������
		UInt8		channel;
		//�Է�id
		ObjID_t		targetId;
		//���
		std::string word;
		//�Ƿ�link
		UInt8		bLink;
		//����key
		std::string voiceKey;
		//��������
		UInt8		voiceDuration;
		//�Ƿ����
		UInt8		isShare;
	};

	//GM����ִ�з���
	class SceneNotifyExecGmcmd: public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_EXEC_GMCMD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & suc;
		}

		//�ɹ����
		UInt8 suc;
	};


	/**
	 *@brief scene->client ͬ������
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
		
		//Ƶ��
		UInt8		channel;
		//˵����id
		ObjID_t		objid;
		//�Ա�
		UInt8		sex;
		//ְҵ
		UInt8		occu;
		//�ȼ�
		UInt16		level;
		//vip�ȼ�
		UInt8		viplvl;
		//qq��Ȩ��Ϣ;
		QQVipInfo	qqVipInfo;
		//˵��������
		std::string	objname;
		//������id
		ObjID_t		receiverId;
		//���
		std::string	word;
		//�Ƿ�link
		UInt8		bLink;
		//�Ƿ�gm
		UInt8		isGm;
		//����key
		std::string voiceKey;
		//��������
		UInt8		voiceDuration;
		//mask
		UInt32		mask;
		// ͷ���
		UInt32		headFrame;
		// ����id
		UInt32		zoneId;
	};

	/**
	 *@brief scene->world ���������������� ��ʽ:objid + dataid + ��������� + data(��CLItemProtocol.h)��(CLPetProtocol.h)
	 */
	class WorldCacheChatDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CACHE_CHATDATA_REQ)
	public:

	};

	/**
	 *@brief client->world ����������ϸ����
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
	 *@brief scene->client ������������ ��ʽ:type(UInt8) + objid + dataid + ��������� + data(��CLItemProtocol.h)
	 */
	class WorldChatLinkDataRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHAT_LINKDATA_RET)
	public:
	};

	/**
	 *@brief scene->world ͨ��worldת������Э��
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
	 *@brief server->client ϵͳ��ʾ����������������
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

		//����
		UInt16 type;
		//Ƶ������
		UInt8 channel;
		//��ɫ
		UInt8 color;
		//���
		std::string word;
	};

	/**
	*@brief server->client ϵͳ���棬��������������
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

		//����
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
		//ָ����
		std::string cmdname;
		//����
		CLParameter params;
	};

	/**
	 *@brief client->server ��������
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
		//����
		UInt8	type;
		//�Է�
		ObjID_t	target;
		//�Է�����
		std::string	targetName;
		//����id
		UInt32	id;
	};

	/**
	 *@brief server->client ͬ������
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
		//����
		UInt8	type;
		//������
		ObjID_t	requester;
		//����������
		std::string	requesterName;
		//�������Ա�
		UInt8	requesterOccu;
		//�����ߵȼ�
		UInt16	requesterLevel;
		//��������1
		std::string	param1;
		//vip�ȼ�
		UInt8	vipLv;
		//�����Ϣ
		PlayerAvatar	avatar;
		//����ʱ������
		UInt8		activeTimeType;
		//ʦ������
		UInt8		masterType;
		//����id
		UInt8		regionId;
		//����
		std::string	declaration;
	};

	/**
	 *@brief client->server ��
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
		//����
		UInt8	type;
		//������
		ObjID_t	requester;
		//���	1Ϊ���� 0Ϊ�ܽ�
		UInt8	result;
	};

	/**
	 *@brief scene->world ת������
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
		//����
		UInt8	type;
		//������
		ObjID_t	requester;
		//Ŀ��
		ObjID_t	target;
		//Ŀ����
		std::string	targetName;
		//id
		UInt32	id;
	};

	/**
	 *@brief world->scene ת����
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
		//����;
		UInt8	type;
		//������
		ObjID_t	requester;
		//Ŀ��
		ObjID_t	target;
		//���
		UInt8	result;
	};

	/**
	 *@brief client->scene ����λ��
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
		//Ƶ��
		UInt8	channel;
		//Ŀ��
		ObjID_t	targetId;
	};

	/**
	*@brief client->scene ����������
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
		// ��������
		std::string		content;
		// һ���Է��͵���������
		UInt8			num;
	};

	/**
	*@brief scene->client ���ط������Ƚ��
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
	 *@brief client->server �������߹���                                                                     
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
	 *@brief server->client �������߹���                                                                     
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
	 *@brief server->client ͬ�������߹���                                                                     
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
	 *@brief scene->world ����һ�����߹���                                                                     
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
	*@brief scene->world ����һ��ϵͳ����
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
	*@brief scene->world->client �㲥���ȸ��ͻ���
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
		// ������Ϣ
		HornInfo		info;
	};

	/**
	*@brief client->world ����ͷ�ϵͳǩ��
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
		// ������Ϣ
		std::string		info;
	};
	
	/**
	*@brief world->client �ͷ�ϵͳǩ������
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
		// ǩ��
		std::string		sign;
	};
}

#endif
