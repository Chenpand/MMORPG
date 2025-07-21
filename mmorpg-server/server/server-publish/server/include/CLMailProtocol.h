#ifndef _CL_MAIL_PROTOCOL_H_
#define _CL_MAIL_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLMailDefine.h"
#include "CLItemDefine.h"

namespace CLProtocol
{
	/**
	 *@brief server->client ͬ��δ���ʼ���
	 */
	class WorldSyncUnreadMailNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_UNREADMAIL_NUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & num;
		}

		UInt32	num;
	};

	/**
	 *@brief client->server �����ʼ��б�
	 */
	class WorldMailListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MAILLIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			//stream & type & start & num;
		}

	public:
		//����	0ȫ�� 1ϵͳ 2���� 3�Ѷ� 4δ��
		//UInt8	type;
		//��ʼλ�� 0��ʼ
		//UInt16	start;
		//����
		//UInt16	num;
	};

	/**
	 *@brief server->client �����ʼ��б�
	 */
	class WorldMailListRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_MAILLIST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			//stream & type & start & totalnum & mails;
			stream & mails;
		}

	public:
		//����  ͬ��
		//UInt8	type;
		//��ʼλ��
		//UInt16	start;
		//�ʼ�����
		//UInt16	totalnum;
		//�б�
		std::vector<MailTitleInfo>	mails;
	};

	/**
	 *@brief client->server �����ȡ�ż�
	 */
	class WorldReadMailReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_READMAIL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//id
		ObjID_t	id;
	};

	/**
	 *@brief server->client �����ʼ���Ϣ
	 * detailItems��ʽ:[itemid(UInt32) + data][itemid + data]...[itemid(0)]
	 * data��ʽ:(��CLItemProtocol.h)
	 */
	class WorldReadMailRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_READMAIL_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			//stream & id & content & silver & items;
			stream & id & content & items;
		}

	public:
		//id
		ObjID_t id;
		//����
		std::string	content;
		//����
		//UInt32	silver;
		//�򵥵���
		std::vector<ItemReward>	items;
		//��ϸ������Ϣ
		//detailItems ...
	};

	/**
	 *@brief client->server ��ȡ�ż�����
	 */
	class WorldGetMailItems : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_MAILITEMS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id & mailType;
		}

	public:
		//���� 0Ϊ��ȡ���� 1Ϊȫ����ȡ
		UInt8	type;
		//id
		ObjID_t	id;
		//�ʼ�����
		UInt32	mailType;
	};

	/**
	 *@brief server->client ͬ���ʼ�״̬
	 */
	class WorldSyncMailStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_MAILSTATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & status & hasitem;
		}
		
	public:
		//id
		ObjID_t	id;
		//״̬	0δ�� 1�Ѷ�
		UInt8	status;
		//�Ƿ��и��� 0û�� 1��
		UInt8	hasitem;
	};

	/**
	 *@brief client->server ����һ���ʼ�
	 */
	class WorldSendMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SEND_MAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & replyId & receiver & title & content & silver & items;
		}

	public:
		//�ظ����ʼ�id 0��ʾ�׷��ʼ�
		ObjID_t		replyId;
		//������
		std::string	receiver;
		//����
		std::string	title;
		//����
		std::string	content;
		//����
		UInt32		silver;
		//����(Ψһid+����)�б�
		std::map<ObjID_t,UInt16> items;
	};

	/**
	 *@brief server->client ֪ͨ���ʼ�
	 */
	class WorldNotifyNewMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_NEWMAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		//�ʼ�������Ϣ
		MailTitleInfo	info;
	};

	/**
	 *@brief client->server ɾ���ʼ�
	 */
	class WorldDeleteMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DELETE_MAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ids;
		}

	public:
		//�ʼ�id�б�
		std::vector<ObjID_t>	ids;
	};

	/**
	 *@brief server->client ֪ͨɾ���ʼ�
	 */
	class WorldNotifyDeleteMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_DELETEMAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ids;
		}

	public:
		//�ʼ�id�б�
		std::vector<ObjID_t>	ids;
	};

	/**
	 *@brief world->scene �ʼ�������
	 */
	class SceneMailCheckPackageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAIL_CHECKPACKAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & items;
		}

	public:
		ObjID_t	playerId;
		std::map<ObjID_t, std::vector<ItemReward> > items;
	};

	/**
	 *@brief scene->world �ʼ�����������
	 */
	class SceneMailCheckPackageRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAIL_CHECKPACKAGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & mailids;
		}

	public:
		ObjID_t	playerId;
		//�ʼ�id
		std::vector<ObjID_t>	mailids;
	};

	/**
	 *@brief world->scene ������
	 * detailItems��ʽ:[Guid_t + itemid(UInt32) + data][Guid_t + itemid + data]...[Guid_t(0)]
	 * data��ʽ:(��CLItemProtocol.h)
	 */
	class SceneMailGiveItems : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAIL_GIVEITEMS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & silver & items;
		}
	public:
		//���
		ObjID_t	playerId;
		//����
		UInt32	silver;
		//�򵥵���
		std::vector<ItemReward>	items;
		//��ϸ������Ϣ
		//detailItems ...
	};

	/**
	 *@brief world->scene �����ʼ�ʱ��������۳����ʺ͸���
	 */
	class SceneMailRemoveCostReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAIL_REMOVECOST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & accid & receiveId & receiveName & replyTimes & title & content & cost & silver & items;
		}

	public:
		//���id
		ObjID_t playerId;
		//������
		ObjID_t	receiveId;
		//�������˺�
		UInt32	accid;
		//����������
		std::string	receiveName;
		//�ظ�����
		UInt8	replyTimes;
		//����
		std::string	title;
		//����
		std::string content;
		//����
		UInt32	cost;
		//����
		UInt32	silver;
		//����(Ψһid+����)�б�
		std::map<ObjID_t,UInt16> items;
	};

	/**
	 *@brief scene->world �����ʼ�ʱ��������۳����ʺ͸�������
	 * detailItems��ʽ:[itemid(UInt32) + data][itemid(UInt32) + data]...[UInt32(0)]
	 * data��ʽ:(��CLItemProtocol.h)
	 */
	class SceneMailRemoveCostRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAIL_REMOVECOST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & receiveId & accid & receiveName & replyTimes & title & content & silver;
		}

	public:
		//���id
		ObjID_t	playerId;
		//������
		ObjID_t	receiveId;
		//�������˺�
		UInt32	accid;
		//����������
		std::string receiveName;
		//�ظ�����
		UInt8 replyTimes;
		//����
		std::string title;
		//����
		std::string content;
		//����
		UInt32 silver;
		//��ϸ������Ϣ
		//detailItems ...
	};

	/**
	 *@brief scene->world ����ϵͳ�ʼ�
	 */
	class WorldSendSysMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SEND_SYSMAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & senderName & title & content & reason & validDay & items;
		}

	public:
		//���
		ObjID_t		playerId;
		//������
		std::string senderName;
		//����
		std::string title;
		//����
		std::string content;
		//��Դ
		std::string reason;
		//��Ч����
		UInt32 validDay;
		//����
		//UInt32	silver;
		//����
		std::vector<ItemReward>	items;
	};

	/**
	 *@brief scene->world ����ϵͳ�ʼ�
	 */
	class WorldSendSysMailToTribeLeader : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SEND_SYSMAIL_TO_TRIBELEADER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tribeId & title & content & silver & items;
		}

	public:
		//����id
		ObjID_t		tribeId;
		//����
		std::string title;
		//����
		std::string content;
		//����
		UInt32		silver;
		//����
		std::vector<ItemReward>	items;
	};

	/**
	*@brief teamcopy->world �����ʼ�
	*/
	class WorldTeamCopySendMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_COPY_SEND_MAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & name & title & content & reason & rewards;
		}

		// ������
		UInt64 roleId;
		// ������
		std::string name;
		// ����
		std::string title;
		// ����
		std::string content;
		// ԭ��
		std::string reason;
		// �ʼ�����
		ItemRewardVec rewards;
	};


	/**
	*@brief union->world �����ʼ�
	*/
	class WorldUnionSendMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UNION_SEND_MAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mailGuid & roleId & name & title & content & reason & rewards;
		}

		// �ʼ�guid
		UInt64 mailGuid;
		// ������
		UInt64 roleId;
		// ������
		std::string name;
		// ����
		std::string title;
		// ����
		std::string content;
		// ԭ��
		std::string reason;
		// �ʼ�����
		ItemRewardVec rewards;
	};
}

#endif
