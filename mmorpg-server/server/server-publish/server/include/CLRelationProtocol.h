#ifndef _CL_RELATION_PROTOCOL_H_
#define _CL_RELATION_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLRelationDefine.h"
#include "CLItemDefine.h"
#include "CLRetinueDefine.h"
#include "CLMatchDefine.h"

namespace CLProtocol
{
	/**
	 *@brief client->world server ��ѯһ�����
	 */
	class WorldQueryPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & type & zoneId & name;
		}
		WorldQueryPlayerReq() : roleId(0), type(0), zoneId(0) {}
	public:
		// ��ɫID
		ObjID_t		roleId;
		// ��ѯ����
		UInt32		type;
		// ��ID
		UInt32		zoneId;
		// ����
		std::string name;
	};

	/**
	*@brief world server->scene server ��ѯһ�����
	*/
	class SceneQueryPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_QUERY_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & srcRoleId & tarAccId & targetRoleId;
		}

	public:
		// ��ѯ��ID
		ObjID_t		srcRoleId;
		// Ŀ��AccID
		UInt32		tarAccId;
		// Ŀ��ID
		ObjID_t		targetRoleId;
	};

	/**
	*@brief scene server->world server ���������Ϣ
	*/
	class SceneQueryPlayerRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_QUERY_PLAYER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roldId & success & info;
		}

	public:
		ObjID_t			roldId;
		UInt8			success;
		PlayerWatchInfo info;
	};

	/**
	 *@brief world server->client ���������Ϣ
	 */
	class WorldQueryPlayerRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_PLAYER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & zoneId & info;
		}
		WorldQueryPlayerRet() : type(0), zoneId(0) {}
	public:
		// ��ѯ����
		UInt32		type;
		// ��ID
		UInt32		zoneId;
		PlayerWatchInfo info;
	};

	/**
	*@brief client->world server ��ѯһ�������ϸ��Ϣ
	*/
	class WorldQueryPlayerDetailsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_PLAYER_DETAILS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & type & zoneId & name;
		}
		WorldQueryPlayerDetailsReq() : roleId(0), type(0), zoneId(0) {}

	public:
		// ��ɫID
		ObjID_t		roleId;
		// ��ѯ����
		UInt32		type;
		// ��ID
		UInt32		zoneId;
		// ����
		std::string name;
	};


	/**
	*@brief world server->client ���������ϸ��Ϣ
	*/
	class WorldQueryPlayerDetailsRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_PLAYER_DETAILS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		RacePlayerInfo info;
	};


	/**
	*@brief world server->scene server ��ѯһ�������ϸ��Ϣ
	*/
	class SceneQueryPlayerDetailsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_QUERY_PLAYER_DETAILS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & srcRoleId & targetRoleId;
		}

	public:
		// ��ѯ��ID
		ObjID_t		srcRoleId;
		// Ŀ��ID
		ObjID_t		targetRoleId;
	};


	/**
	*@brief scene server->world server ���������ϸ��Ϣ
	*/
	class SceneQueryPlayerDetailsRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_QUERY_PLAYER_DETAILS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roldId & success & info;
		}

	public:
		ObjID_t			roldId;
		UInt8			success;
		RacePlayerInfo	info;
	};


	/**
	 *@brief client->server ��ӵ�������
	 */
	class WorldAddToBlackList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADDTO_BLACKLIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//���id
		ObjID_t	id;
	};

	/**
	 *@brief client->server �Ƴ���ϵ
	 */
	class WorldRemoveRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REMOVE_RELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id;
		}

	public:
		//��ϵ����
		UInt8	type;
		//�Է�id
		ObjID_t id;
	};

	/**
	 *@brief server->client ֪ͨ�¹�ϵ
	 *	��ʽ: type(UInt8) + id(ObjID_t) + isOnline(UInt8) + data
	 *  data: ����id + ���� + ... + UInt8(0)
	 */
	class WorldNotifyNewRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_NEWRELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	public:
	};

	/**
	 *@brief server->client ֪ͨɾ����ϵ
	 */
	class WorldNotifyDelRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_DELRELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id;
		}

	public:
		//��ϵ����
		UInt8	type;
		//id
		ObjID_t	id;
	};

	/**
	 *@brief server->client ͬ����ϵ���� 
	 * ��ʽ type(UInt8) + id(ObjID_t) + data
	 * data��ʽͬWorldNotifyNewRelation
	 */
	class WorldSyncRelationData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_RELATIONDATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	 *@brief server->client ����ͬ����ϵ�б�
	 *	datalist��ʽ: ObjID_t + isOnline(UInt8) + data + .. + 0(ObjID_t)
	 *	data��ʽͬWorldNotifyNewRelation
	 */
	class WorldSyncRelationList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_RELATION_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	public:
		//��ϵ����
		UInt8	type;
		//datalist
	};

	/**
	 *@brief client->server ׷ɱ���
	 */
	class WorldChaseKill : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHASE_KILL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//�Է�id
		ObjID_t	id;
	};

	/**
	 *@brief server->client ͬ��������״̬
	 */
	class WorldSyncOnOffline : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ONOFFLINE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & isOnline;
		}

	public:
		//id
		ObjID_t	id;
		//1Ϊ���� 0Ϊ����
		UInt8	isOnline;
	};

	/**
	 *@brief client->server ��������֪ͨ
	 */
	class WorldSetOnlineNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_ONLINE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id & notify;
		}

	public:
		//����
		UInt8	type;
		//id
		ObjID_t	id;
		//0Ϊ��֪ͨ 1Ϊ֪ͨ
		UInt8	notify;
	};

	/**
	 *@brief scene->world �������ܶ�
	 */
	class WorldRelationAddIntimacy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_ADD_INTIMACY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & friendId & intimacy;
		}

	public:
		//���id
		ObjID_t id;
		//����id
		ObjID_t friendId;
		//���ܶ�
		UInt16	intimacy;
	};

	/**
	 *@brief world->gate ������ͬ��������
	 */
	class GateSyncBlackList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SYNC_BLACKLIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & blacklist;
		}

	public:
		//���id
		ObjID_t	id;
		//������
		std::vector<ObjID_t>	blacklist;
	};

	/**
	 *@brief world->gate ֪ͨ����������
	 */
	class GateNotifyNewBlackList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_NEWBLACKLIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & target;
		}

	public:
		//���id
		ObjID_t	id;
		//�Է�id
		ObjID_t	target;
	};

	/**
	 *@brief world->gate ֪ͨɾ��������
	 */
	class GateNotifyDelBlackList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_DELBLACKLIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & target;
		}

	public:
		//���id
		ObjID_t	id;
		//�Է�id
		ObjID_t	target;
	};

	/**
	 *@brief world->scene ͬ����ϵ�б�����
	 */
	class SceneRelationSyncList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & relationList;
		}

	public:
		//���id
		ObjID_t id;
		//��ϵ�б�
		std::vector<std::map<ObjID_t, UInt16>>	relationList;
	};

	/**
	 *@brief world->scene ֪ͨ������ϵ
	 */
	class SceneRelationNotifyNew : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_NOTIFY_NEW)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & relationType & tarId;
		}

	public:
		//���id
		ObjID_t id;
		//��ϵ����
		UInt8	relationType;
		//��ϵ����id
		ObjID_t tarId;
	};

	/**
	 *@brief world->scene ֪ͨɾ����ϵ
	 */
	class SceneRelationNotifyDel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_NOTIFY_DEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & relationType & tarId;
		}

	public:
		//���id
		ObjID_t id;
		//��ϵ����
		UInt8	relationType;
		//��ϵ����id
		ObjID_t tarId;
	};

	/**
	 *@brief world->scene ͬ�����ܶ�
	 */
	class SceneRelationSyncIntimacy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_SYNC_INTIMACY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & friendId & intimacy & incIntimacy & isMaster;
		}

	public:
		//���id
		ObjID_t	id;
		//����id
		ObjID_t friendId;
		//���ܶ�
		UInt16	intimacy;
		//����
		UInt16  incIntimacy;
		//�Ƿ�ʦ��
		UInt8   isMaster;
	};

	/**
	 *@brief world->scene ׷ɱʱ��ѯĿ��������ڳ���
	 */
	class SceneRelationFindPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_FIND_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & killer & target;
		}

	public:
		//ɱ����
		ObjID_t	killer;
		//Ŀ�����
		ObjID_t	target;
	};

	/**
	 *@brief scene->world ��ѯĿ����ҷ���
	 */
	class SceneRelationFindPlayerRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_FIND_PLAYER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & killer & target & sceneId & pos;
		}

	public:
		//ɱ����
		ObjID_t killer;
		//Ŀ�����
		ObjID_t	target;
		//���ڳ���
		UInt32	sceneId;
		//λ��
		CLPosition	pos;
	};

	/**
	 *@brief client->server ��������б�
	 */
	class WorldRelationFindPlayersReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_FIND_PLAYERS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		UInt8 type;		//RelationFindType
	};

	/**
	 *@brief server->client ��������б�
	 */
	class WorldRelationFindPlayersRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_FIND_PLAYERS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & players;
		}

		UInt8 type;			//RelationFindType
		//�б�
		std::vector<QuickFriendInfo>	players;
	};

	/**
	*@brief client->server ��������
	*/
	class WorldRelationPresentGiveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_PRESENT_GIVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & friendID;
		}

		ObjID_t  friendID;
	};

	/**
	*@brief world->client �������ͷ���
	*/
	class WorldRelationPresentGiveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_PRESENT_GIVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & friendID;
		}
		UInt32   code;
		ObjID_t  friendID;
	};

	/**
	*@brief client->server ���¹�ϵ
	*/
	class WorldUpdateRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UPDATE_RELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}

	};

	struct OnlineState
	{
	public:
		OnlineState() :uid(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid & bOnline;
		}

		ObjID_t uid;
		UInt8	bOnline;
	};
	/**
	*@brief client->server ������ҹ�ϵ
	*/
	class WorldUpdatePlayerOnlineReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UPDATE_PLAYER_ONLINE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uids;
		}

		std::vector<ObjID_t> uids;
	};

	/**
	*@brief server->client ������ҹ�ϵ����
	*/
	class WorldUpdatePlayerOnlineRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UPDATE_PLAYER_ONLINE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & onlineStates;
		}

		std::vector<OnlineState> onlineStates;
	};
	
	/**
	*@brief client->server ����ʦ������
	*/
	class WorldSetMasterNoteReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_MASTER_NOTE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & note;
		}

		std::string note;
	};
	
	/**
	*@brief server->client ����ʦ�����淵��
	*/
	class WorldSetMasterNoteRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_MASTER_NOTE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & newNote;
		}

		UInt32 code;
		std::string newNote;
	};

	/**
	*@brief client->server ����ʦ���Ƿ���ͽ
	*/
	class WorldSetMasterIsRecvDiscipleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_RECV_DISCIPLE_STATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isRecv;
		}

		UInt8 isRecv;
	};

	/**
	*@brief server->client ����ʦ���Ƿ���ͽ����
	*/
	class WorldSetMasterIsRecvDiscipleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_RECV_DISCIPLE_STATE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & isRecv;
		}

		UInt32 code;
		UInt8 isRecv;
	};

	/**
	*@brief client->server ����ʦ��������Ϣ
	*/
	class WorldQueryMasterSettingReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_MASTER_SETTING_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}

	};

	/**
	*@brief server->client ����ʦ��������Ϣ����
	*/
	class WorldQueryMasterSettingRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_MASTER_SETTING_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterNote & isRecv;
		}
		std::string masterNote;
		UInt8	isRecv;
	};

	/**
	 *@brief client->server һ������
	 */
	class WorldRelationQuickMakeFriends : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_QUICK_MAKE_FRIENDS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & players;
		}

		//�б�
		std::vector<ObjID_t>	players;
	};

	/**
	 *@brief client->world ����Ϊ����                                                                     
	 */
	class WorldRelationSetClostFriend : public Avalon::Protocol
	{	
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_SET_CLOSEFRIEND)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & flag;
		}

		//����id
		ObjID_t playerId;
		//1��ʾ��Ϊ���� 0��ʾȡ������
		UInt8	flag;
	};

	/**
	*@brief client->world �㱨�Լ�������
	*/
	class WorldRelationReportCheat : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_REPORT_CHEAT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief scene->client ͬ��ף������                                                                     
	 */
	class SceneRelationSyncBlessTimes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_SYNC_BLESSTIMES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & times;
		}

		UInt32 times;
	};

	/**
	 *@brief client->scene ������ѻ�����Ϣ                                                                     
	 */
	class SceneRelationPresentInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	 *@brief scene->client ���غ��ѻ�����Ϣ                                                                     
	 */
	class SceneRelationPresentInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_INFO_RET)
	public:
		SceneRelationPresentInfoRet():continuousDays(0), giveState1(0), drawState1(0), giveState2(0), drawState2(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & continuousDays & giveState1 & drawState1 & giveState2 & drawState2;
		}

	public:
		//���Ǻ�������½���� Ϊ0��ʾ��������¼
		UInt32	continuousDays;
		//�����Ƿ������;��� 0��ʾδ��ȡ 1��ʾ����ȡ
		UInt8	giveState1;
		//�����Ƿ�����ȡ���� 0��ʾδ��ȡ 1��ʾ����ȡ
		UInt8	drawState1;
		//15���Ƿ�������
		UInt8	giveState2;
		//15���Ƿ�����ȡ
		UInt8	drawState2;
	};

	/**
	 *@brief client->scene ��ȡ                                                                     
	 */
	class SceneRelationPresentDraw : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_DRAW)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		//���� 1Ϊ���� 2Ϊ15��
		UInt8	type;
	};

	/**
	 *@brief client->scene ����                                                                     
	 */
	class SceneRelationPresentGive : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_GIVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & friendId & type;
		}

		//����
		ObjID_t	friendId;
		UInt8 type;
	};

	/**
	 *@brief scene->world->scene ����������                                                                     
	 */
	class SceneRelationPresentToFriendReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_TOFRIEND_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & playerName & friendId & type & itemId & gamestartDay;
		}

		//���id
		ObjID_t	playerId;
		//�����
		std::string	playerName;
		//����id
		ObjID_t	friendId;
		//����
		UInt8	type;
		//���͵ĵ���
		UInt32	itemId;
		//��Ӧ��
		UInt32  gamestartDay;
	};

	/**
	 *@brief scene->world->scene �����ѷ���                                                                     
	 */
	class SceneRelationPresentToFriendRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_TOFRIEND_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & gamestartDay & result;
		}

		ObjID_t	playerId;
		UInt8	type;
		UInt32	gamestartDay;
		//���ͽ�� 0�ɹ���1���Ѳ����ߣ�2���Ѵ�����
		UInt8	result;
	};

	/**
	*@brief client->scene ʦ������װ��
	*/
	class SceneMasterGiveEquip : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MASTER_GIVE_EQUIP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemuids & disciple;
		}

		std::vector<UInt64>	itemuids;
		UInt64	disciple;
	};


	/**
	*@brief client->scene �������ܸ�(����)
	*/
	class SceneAddonPayReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADDONPAYREQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goodId & tarId & words;
		}

		UInt32	goodId;
		UInt64	tarId;
		std::string words;
	};


	/**
	*@brief scene->world �������ܸ�(����)
	*/
	class WorldAddonPayReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADDONPAYREQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goodId & tarId & tarName & tarOccu & tarLevel & words;
		}

		UInt32	goodId;
		UInt64	tarId;
		std::string tarName;
		UInt8	tarOccu;
		UInt32	tarLevel;
		std::string words;
	};
		
	/**
	*@brief world->client ͬ������һ�����ܸ���Ϣ(����)
	*/
	class WorldSyncAddonPayData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ADDONPAY_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & data;
		}

		AddonPayData data;
	};

	/**
	*@brief world->client ͬ���������ܸ���Ϣ(����)
	*/
	class WorldSyncAllAddonPayDatas : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ALL_ADDONPAY_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & datas;
		}

		std::vector<AddonPayData> datas;
	};
	
	/**
	*@brief world->client �ظ����ܸ�(����)
	*/
	class WorldAddonPayReply : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADDONPAYREPLY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & orderId & agree;
		}

		UInt64	orderId;
		UInt8	agree;
	};
		
	/**
	*@brief scene->world ���ʦ������ͽ�ܴ���
	*/
	class WorldCheckMasterGive : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHECK_MASTER_GIVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId & disciple & items;
		}

		ObjID_t	masterId;
		ObjID_t disciple;
		std::vector<ObjID_t>	items;
	};

	class WorldCheckMasterGiveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHECK_MASTER_GIVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId & disciple & items;
		}

		ObjID_t	masterId;
		ObjID_t disciple;
		std::vector<ObjID_t> items;
	};
	
	/*
	*@brief client->world ����ͽ�ܰ�ʦ�ʾ����
	*/
	class WorldSetDiscipleQuestionnaireReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_DISCIPLE_QUESTIONNAIRE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activeTimeType & masterType & regionId & declaration;
		}

		UInt8			activeTimeType; //��Ծʱ������
		UInt8			masterType;		//ϣ������ʦ��
		UInt8			regionId;		//���ڵ���id
		std::string		declaration;	//��ʦ����
	};

	/*
	*@brief world->client ����ͽ�ܰ�ʦ�ʾ���鷵��
	*/
	class WorldSetDiscipleQuestionnaireRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_DISCIPLE_QUESTIONNAIRE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32 code;
	};

	/*
	*@brief client->world ����ʦ����ͽ�ʾ����
	*/
	class WorldSetMasterQuestionnaireReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_MASTER_QUESTIONNAIRE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activeTimeType & masterType & regionId & declaration;
		}

		UInt8			activeTimeType; //��Ծʱ������
		UInt8			masterType;		//����ʲô����ʦ��
		UInt8			regionId;		//���ڵ���id
		std::string		declaration;	//��ͽ����
	};

	/*
	*@brief world->client ����ʦ����ͽ�ʾ���鷵��
	*/
	class WorldSetMasterQuestionnaireRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_MASTER_QUESTIONNAIRE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32 code;
	};

	/**
	*@brief world->client ֪ͨ��ͬ�Ź�ϵ
	*	��ʽ: id(ObjID_t) + isOnline(UInt8) + data
	*  data: ����id + ���� + ... + UInt8(0)
	*/
	class WorldNotifyNewMasterSectRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_NEW_MASTERSECT_RELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	public:
	};

	/**
	*@brief world->client ͬ��ͬ�Ź�ϵ����
	* ��ʽ  id(ObjID_t) + data
	* data��ʽͬWorldNotifyNewMasterSectRelation
	*/
	class WorldSyncMasterSectRelationData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_MASTERSECT_RELATIONDATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief world->client ����ͬ��ͬ�Ź�ϵ�б�
	*	datalist��ʽ: ObjID_t + isOnline(UInt8) + data + .. + 0(ObjID_t)
	*	data��ʽͬWorldNotifyNewMasterSectRelation
	*/
	class WorldSyncMasterSectRelationList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_MASTERSECT_RELATION_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world ����ͬ�Ź�ϵ
	*/
	class WorldUpdateMasterSectRelationReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UPDATE_MASTERSECT_RELATION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	};

	/**
	*@brief world->client ɾ��ͬ�Ź�ϵͬ��
	*/
	class WorldNotifyDelMasterSectRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_DEL_MASTERSECT_RELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id;
		}

		//��ϵ����
		UInt8	type;
		//id
		ObjID_t	id;

	};

	/*
	*@brief world->client ͬ���ʾ����
	*/
	class WorldSyncRelationQuestionnaire : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_RELATION_QUESTIONNAIRE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activeTimeType & masterType & regionId & declaration;
		}

		UInt8			activeTimeType; //��Ծʱ������
		UInt8			masterType;		//����ʲô����ʦ��
		UInt8			regionId;		//���ڵ���id
		std::string		declaration;	//��ͽ����
	};

	/*
	*@brief client->world ����ͽ�ܳ�ʦ
	*/
	class WorldDiscipleFinishSchoolReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISCIPLE_FINISH_SCHOOL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & discipleId;
		}

		ObjID_t		discipleId;		//ͽ��id
	};

	/*
	*@brief world->client ����ͽ�ܳ�ʦ����
	*/
	class WorldDiscipleFinishSchoolRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISCIPLE_FINISH_SCHOOL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32		code;		
	};

	/*
	*@brief world->client ͬ��ʦͽ��ʦ��ͽ�ͷ�ʱ���
	*/
	class WorldSyncMasterDisciplePunishTime : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_MASTERDISCIPLE_PUNISH_TIME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & apprenticMasterPunishTime & recruitDisciplePunishTime;
		}

		UInt64		apprenticMasterPunishTime;
		UInt64      recruitDisciplePunishTime;
	};

	/*
	*@brief world->client ��ʦ�ɹ�����չʾ
	*/
	class WorldNotifyFinshSchoolReward : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_FINSCHOOL_REWARD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & giftId & masterId & discipleId & masterName & discipleName & discipleGrowth;
		}
		UInt32			giftId;			//��ʦ�������id
		UInt64			masterId;		//ʦ��id
		UInt64			discipleId;		//ͽ��id
		std::string		masterName;		//ʦ������
		std::string		discipleName;	//ͽ������
		UInt32			discipleGrowth;	//ͽ�ܳɳ�ֵ
	};

	/**
	*@brief client->world �Զ���ʦ
	*/
	class WorldAutomaticFinishSchoolReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUTOMATIC_FINSCHOOL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & targetId;
		}

		UInt64		targetId;	//Ŀ���ɫid
	};

	/**
	*@brief world->client �Զ���ʦ����
	*/
	class WorldAutomaticFinishSchoolRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUTOMATIC_FINSCHOOL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32		code;
	};

	/**
	*@brief client->world  ������ұ�ע����
	*/
	class WorldSetPlayerRemarkReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_PLAYER_REMARK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & remark;
		}

		ObjID_t		 roleId;
		std::string  remark;
	};

	/**
	*@brief world->client  ������ұ�ע����
	*/
	class WorldSetPlayerRemarkRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_PLAYER_REMARK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32		code;
	};

	/**
	*@brief client->world ʦͽ����Ƶ������
	*/
	class WorldRelationAnnounceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_MASTERDISCIPLE_ANNOUNCE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}
		UInt32		type;
	};

	/*
	*@brief  world->client ֪ͨ��ʦ�¼�
	*/
	class WorldNotifyFinSchEvent : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_FINSCH_EVENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	};

	class WorldQueryHireInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	class WorldQueryHireInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & identity & code & isBind & isOtherBindMe;
		}

		UInt8 identity = 0;
		std::string code;
		UInt8 isBind = 0;
		UInt8 isOtherBindMe = 0;
	};

	class WorldUseHireCodeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_USE_HIRE_CODE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		std::string code;
	};

	class WorldUseHireCodeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_USE_HIRE_CODE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode;
		}

		UInt32 errorCode = 0;
	};

	class WorldQueryTaskStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_TASK_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	class WorldQueryTaskStatusRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_TASK_STATUS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & hireTaskInfoList;
		}

		std::vector<HireInfoData> hireTaskInfoList;
	};

	class WorldQueryHireTaskAccidListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_TASK_ACCID_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

		UInt32 taskId;
	};

	class WorldQueryHireTaskAccidListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_TASK_ACCID_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nameList;
		}

		std::vector<std::string> nameList;
	};

	class WorldQueryHireListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
		
	};

	class WorldQueryHireListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & hireList;
		}

		std::vector<HirePlayerData> hireList;
	};

	class WorldSubmitHireTaskReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SUBMIT_HIRE_TASK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

		UInt32 taskId;
	};

	class WorldSubmitHireTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SUBMIT_HIRE_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & errorCode;
		}

		UInt32 taskId;
		UInt32 errorCode = 0;
	};

	class WorldOnSceneTrigeTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ON_SCENE_TRIGE_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & type & value & teammateIds;
		}

		UInt32 accid;
		UInt32 type;
		UInt32 value;
		std::vector<UInt32> teammateIds;
	};

	
	class WorldQueryHireCoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_COIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	};

	class WorldQueryHirePushReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_PUSH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}
		UInt8 type; //0 �ǲ�ѯ 1����������
	};

	class SceneQueryHireRedPointReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_HIRE_RED_POINT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	};


	class WorldQueryHireAlreadyBindReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_ALREADY_BIND_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & platform & zoneId;
		}
		//ƽ̨
		std::string platform;
		UInt32 accid;
		//zoneid
		UInt32 zoneId;
	};

	class WorldQueryHireAlreadyBindRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_ALREADY_BIND_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode & accid & zoneId;
		}
		UInt32 errorCode;
		UInt32 accid;
		//zoneid
		UInt32 zoneId;
	};

	

	/**
	*@brief world->globalactivity ����ʺ��Ƿ��Ѱ� ���δ�����
	*/
	class GASCheckHireBindReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_CHECK_HIRE_BIND_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & platform & zoneId & accid & code;
		}
		//ƽ̨
		std::string platform;
		//zoneid
		UInt32 zoneId;
		//�ʺ�
		UInt32 accid;
		//��ļ��
		std::string code;
	};

	/**
	*@brief globalactivity->world ����ʺ��Ƿ��Ѱ󶨷���
	*/
	class GASCheckHireBindRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_CHECK_HIRE_BIND_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & accid & errorCode & code;
		}
		//zoneid
		UInt32 zoneId;
		//�ʺ�
		UInt32 accid;
		//���
		UInt32 errorCode;
		//��ļ��
		std::string code;
	};
	
	/**
	*@brief 
	*/
	class SceneSyncMasterDailyTaskComp : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_MASTER_DAILY_TASK_COMP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t		 roleId;
	};
}

#endif
