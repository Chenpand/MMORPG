#ifndef _CL_INVITEGAME_PROTOCOL_H_
#define _CL_INVITEGAME_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief ����������Ϣ
	*/
	struct InviteeInfo
	{
		InviteeInfo():uid(0), bYesterdayOnline(0), level(0), offlineTime(0), onlineAwardTime(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid & openid & bYesterdayOnline & level & offlineTime & onlineAwardTime;
		}

		//���id
		UInt32		uid;
		//openid
		std::string	openid;
		//�����Ƿ�����
		UInt8		bYesterdayOnline;
		//��ǰ�ȼ�
		UInt16		level;
		//����ʱ��(0��ʾ����)
		UInt32		offlineTime;
		//��½������ȡʱ��
		UInt32		onlineAwardTime;
	};

	/**
	 *@brief zone->SocialServer �ϱ���������б�
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
		//����б�
		std::vector<UInt32>	players;
	};

	/**
	 *@brief zone->SocialServer ֪ͨ�������ĳ���ȼ���
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
		//�ڵ�id
		UInt32	nodeId;
		//uid
		UInt32	accId;
		//���id
		ObjID_t playerId;
		//�ȼ�
		UInt16	level;
	};

	/**
	 *@brief zone->SocialServer ֪ͨ����
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
		//���id
		UInt32	uid;
		//�����Ľ���
		UInt32	gold;
	};

	/**
	 *@brief zone->SocialServer ֪ͨ�������߽�����Ϸ
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
		//������
		UInt32	inviter;
		//��������
		UInt32	invitee;
		//��������openid
		std::string	openid;
	};

	/**
	 *@brief zone->SocialServer �����������б�
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
		//�������ڵ�id
		UInt32	nodeId;
		//������id
		UInt32	inviterId;
		//�����߽�ɫid
		ObjID_t	playerId;
	};

	/**
	 *@brief SocialServer->zone ���ر��������б�
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
		//��ɫid
		ObjID_t	playerId;
		//���տ���ȡ����
		UInt32	totalGold;
		//�б�
		std::vector<InviteeInfo>	invitees;
	};

	/**
	 *@brief zone->SocialServer �����������ȡ�����Ƿ�����
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
		//�������ڵ�id
		UInt32	nodeId;
		//������id
		UInt32	inviterId;
		//�����߽�ɫid
		ObjID_t	playerId;
		//�������
		UInt8	type;
		//��ҵȼ�����
		UInt16	minLevel;
		//��ҵȼ�����
		UInt16	maxLevel;
		//����
		UInt32	playerNum;
	};

	/**
	 *@brief SocialServer->zone �����������ȡ��������
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
		//���id
		ObjID_t playerId;
		//�������
		UInt8	type;
		//��� 1��ʾ���� 0��ʾ������
		UInt8	result;
	};

	/**
	 *@brief zone->SocialServer ����ٻؽ�������
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
		//�������ڵ�id
		UInt32	nodeId;
		//������id
		UInt32	inviterId;
		//�����߽�ɫid
		ObjID_t	playerId;
		//��������id
		UInt32	inviteeId;
	};

	/**
	 *@brief SocialServer->zone ����ٻؽ�������
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
		//���id
		ObjID_t	playerId;
		//����ȡ���б�
		std::vector<InviteeInfo> invitees;
	};

	/**
	 *@brief client->scene �������������Ϣ
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
	 *@brief scene->client ���ؽ���������Ϣ
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
		//����
		UInt32	inviteTimes;
		//��������ȡ����
		UInt32	inviteGold;
		//�����ȡ��ʶ  0Ϊ������� 1-5Ϊ��Ծ��� 11+Ϊ������� 25,26Ϊ�������
		UInt32	giftbagFlag;
	};

	/**
	 *@brief client->scene ������������б�
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
	 *@brief scene->client ������������б�
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
	 *@brief client->scene ֪ͨ�������ѷ����µ�����
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
	 *@brief client->scene ��ȡ���
	 */
	class SceneInviteGetGiftbag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_GET_GIFTBAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		//�����ȡ��ʶ  0Ϊ������� 1-5Ϊ��Ծ��� 11+Ϊ������� 25,26Ϊ�������
		UInt8	type;
	};

	/**
	 *@brief client->scene ��ȡ�ٻؽ�������
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
		//����id 0-��ʾһ����ȡ  ������ȡ�������ѵ�
		UInt32 uid;
	};

	/**
	 *@brief scene->client ˢ�µ���������״̬
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
		//������Ϣ
		InviteeInfo	info;
	};

	/**
	 *@brief client->scene ��ȡ���ѷ���
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
	 *@brief scene->client ��ȡ���ѷ�������
	 */
	class SceneInviteGetConsumeAwardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_GET_CONSUMEAWARD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & todayGoldAward & totalGold;
		}

		//������ȡ��
		UInt32	todayGoldAward;
		//�ܽ���
		UInt32	totalGold;
	};

	/**
	 *@brief zone->SocialServer ������ѷ���
	 */
	class SocialInviteCheckConsumeAwardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_CONSUMEAWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & inviterId & playerId & maxGold;
		}

		//�������ڵ�id
		UInt32	nodeId;
		//������id
		UInt32	inviterId;
		//�����߽�ɫid
		ObjID_t	playerId;
		//��ȡ���
		UInt32	maxGold;
	};

	/**
	 *@brief SocialServer->zone ������ѷ�������
	 */
	class SocialInviteCheckConsumeAwardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_CONSUMEAWARD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & gold & totalGold;
		}

		//���id
		ObjID_t	playerId;
		//��ȡ���
		UInt32	gold;
		//�ܶ��
		UInt32	totalGold;
	};

	/**
	 *@brief server->SocialServer ����������                                                                     
	 */
	class SocialInviteCheckKLActivityReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_KLACTIVITY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & accId & playerId;
		}

		//�ڵ�id
		UInt32 nodeId;
		//�˺�id
		UInt32 accId;
		//���id
		ObjID_t	playerId;
	};

	/**
	 *@brief SocialServer->server ��������������                                                                     
	 */
	class SocialInviteCheckKLActivityRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_CHECK_KLACTIVITY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & klopenid & finishFlag;
		}

		//���id
		ObjID_t	playerId;
		//����openid
		std::string klopenid;
		//���ɱ�־
		UInt8	finishFlag;
	};

	/**
	 *@brief server->SocialServer �󶨿����˺�                                                                     
	 */
	class SocialInviteBindKLActivity : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SOCIAL_INVITE_BIND_KLACTIVITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & accId & playerId & level & openid & klopenid;
		}

		//�ڵ�id
		UInt32 nodeId;
		//�˺�id
		UInt32 accId;
		//���id
		ObjID_t	playerId;
		//�ȼ�
		UInt16 level;
		//openid
		std::string openid;
		//����openid
		std::string klopenid;
	};

	/**
	 *@brief client->server �󶨿����˺�                                                                     
	 */
	class SceneInviteBindKLPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_BIND_KLPLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & klopenid;
		}

		//����openid
		std::string klopenid;
	};

	/**
	 *@brief server->client ͬ������������Ϣ                                                                     
	 */
	class SceneInviteSyncKLActivity : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INVITE_SYNC_KLACTIVITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & klopenid & finishFlag;
		}

		//����openid
		std::string	klopenid;
		//������ɱ�־ 1-30��������� 2-45��������� 3-�����������
		UInt8	finishFlag;
	};

	/**
	 *@brief client->server �һ�cdkey                                                                     
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
	 *@brief server->SocialServer ����ʼ�һ�cdkey                                                                     
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
	 *@brief SocialServer->server ����ʼ�һ�cdkey����                                                                     
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
		//���id
		UInt32	itemId;
	};

	/**
	 *@brief server->SocialServer �����һ�cdkey                                                                     
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
