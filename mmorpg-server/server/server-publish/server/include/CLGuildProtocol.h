#ifndef _CL_GUILD_PROTOCOL_H_
#define _CL_GUILD_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLGuildDefine.h"
#include "CLSkillDefine.h"
#include <CLMaskProperty.h>
#include <AvalonPacket.h>
#include <CLSyncObjectProtocol.h>

struct ItemReward;

namespace CLProtocol
{
	/**
		*@brief client->world ��������
		*/
	class WorldGuildCreateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CREATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & declaration;
		}

	public:
		//������
		std::string	name;
		//����
		std::string declaration;
	};

	/**
		*@brief world->client �������᷵��
		*/
	class WorldGuildCreateRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CREATE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32		result;
	};

	/**
		*@brief client->server �뿪����
		*/
	class WorldGuildLeaveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LEAVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
		*@brief world->client �뿪���᷵��
		*/
	class WorldGuildLeaveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LEAVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief client->world ���빫��
	*/
	class WorldGuildJoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_JOIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		ObjID_t			id;
	};

	/**
	*@brief world->client ���빫�᷵��
	*/
	class WorldJoinGuildRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_JOIN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32		result;
	};

	/**
	*@brief client->server ���󹫻��б�
	*/
	class WorldGuildListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & start & num;
		}

	public:
		//��ʼλ�� 0��ʼ
		UInt16	start;
		//����
		UInt16	num;
	};

	/**
	*@brief world->client ���ع����б�
	*/
	class WorldGuildListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & start & totalnum & guilds;
		}

	public:
		//��ʼλ��
		UInt16	start;
		//����
		UInt16	totalnum;
		//�����б�
		std::vector<GuildEntry>	guilds;
	};

	/**
	*@brief client->world ���������빫����б�
	*/
	class WorldGuildRequesterReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_REQUESTERS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	};

	/**
	*@brief world->client ���������빫����б�
	*/
	class WorldGuildRequesterRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_REQUESTERS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requesters;
		}

		//�������б�
		std::vector<GuildRequesterInfo>	requesters;
	};

	/**
	*@brief world->client ֪ͨ�µ��벿������
	*/
	class WorldGuildNewRequester : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_NEW_REQUESTER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world �������������
	*/
	class WorldGuildProcessRequester : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_PROCESS_REQUESTER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & agree;
		}

	public:
		//id(�����0��������б�)
		ObjID_t	id;
		//ͬ�����(0:��ͬ�⣬1:ͬ��)
		UInt8	agree;
	};

	/**
	*@brief world->client ������������󷵻�
	*/
	class WorldGuildProcessRequesterRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_PROCESS_REQUESTER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & entry;
		}

	public:
		UInt32				result;
		GuildMemberEntry	entry;
	};

	/**
	*@brief client->world ����ְλ
	*/
	class WorldGuildChangePostReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHANGE_POST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & post & replacerId;
		}

	public:
		//id
		ObjID_t id;
		//ְλ
		UInt8	post;
		//���滻����
		ObjID_t	replacerId;
	};

	/**
	*@brief world->client ����ְλ����
	*/
	class WorldGuildChangePostRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHANGE_POST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32		result;
	};

	/**
	*@brief client->world ����
	*/
	class WorldGuildKickReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_KICK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//id
		ObjID_t id;
	};

	/**
	*@brief world->client ���˷���
	*/
	class WorldGuildKickRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_KICK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32		result;
	};



	/**
	*@brief world->client ���߻��¼��빫�ᷢ�ͳ�ʼ����
	*/
	class WorldGuildSyncInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SYNC_BASE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		// ���������Ϣ
		GuildBaseInfo		info;
	};


	/**
	*@brief client->world ���󹫻��Ա�б�
	*/
	class WorldGuildMemberListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MEMBER_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildID;
		}
		//0Ϊ��ѯ���л��Ա ���ֵΪ��ѯ����л��Ա����������沢ʹ��
		UInt64 guildID;
	};

	/**
	*@brief server->client ���ع����Ա�б�
	*/
	class WorldGuildMemberListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MEMBER_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & members;
		}

		//�б�
		std::vector<GuildMemberEntry>	members;
	};

	/**
	*@brief client->world �޸Ĺ�������
	*/
	class WorldGuildModifyDeclaration : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MODIFY_DECLARATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & declaration;
		}

		std::string declaration;
	};

	/**
	*@brief client->world �޸Ĺ�����
	*/
	class WorldGuildModifyName : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MODIFY_NAME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & itemId & itemTypeId;
		}

		std::string name;
		ObjID_t		itemId;
		UInt32		itemTypeId;
	};

	/**
	*@brief world->client �޸Ĺ��ṫ��
	*/
	class WorldGuildModifyAnnouncement : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MODIFY_ANNOUNCEMENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & content;
		}

		std::string content;
	};

	/**
	*@brief client->world ���͹����ʼ�
	*/
	class WorldGuildSendMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SEND_MAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & content;
		}

		std::string content;
	};

	/**
	*@brief world->client ���ͨ�ò�������
	*/
	class WorldGuildOperRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_OPER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & result & param1 & param2;
		}

		// �������ͣ���Ӧö��GuildOperation��
		UInt8	type;
		// ���
		UInt32	result;
		// ����1
		UInt32	param1;
		// ����2
		ObjID_t	param2;
	};

	/**
	*@brief client->world ��������
	*/
	class WorldGuildUpgradeBuilding : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_UPGRADE_BUILDING)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		// �������ͣ���Ӧö��GuildBuildingType��
		UInt8	type;
	};

	/**
	*@brief client->world �������
	*/
	class WorldGuildDonateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DONATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & num;
		}

		// �������ͣ���Ӧö��GuildDonateType��
		UInt8	type;
		// ����
		UInt8	num;
	};

	/**
	*@brief client->world ���������־
	*/
	class WorldGuildDonateLogReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DONATE_LOG_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ���ؾ�����־
	*/
	class WorldGuildDonateLogRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DONATE_LOG_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & logs;
		}

		std::vector<GuildDonateLog> logs;
	};

	/**
	*@brief client->world ��������
	*/
	class WorldGuildUpgradeSkill : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_UPGRADE_SKILL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & skillId;
		}

		// ����id
		UInt16	skillId;
	};

	/**
	*@brief client->world ��ɢ����
	*/
	class WorldGuildDismissReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DISMISS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world ��ɢ����
	*/
	class WorldGuildCancelDismissReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CANCEL_DISMISS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world ����᳤��Ϣ
	*/
	class WorldGuildLeaderInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LEADER_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ���ػ᳤��Ϣ
	*/
	class WorldGuildLeaderInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LEADER_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		GuildLeaderInfo info;
	};

	/**
	*@brief client->world ����Ĥ��
	*/
	class WorldGuildOrzReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_ORZ_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}
		// Ĥ�����ͣ���Ӧö�٣�GuildOrzType��
		UInt8		type;
	};

	/**
	*@brief client->world ����Ĥ��
	*/
	class WorldGuildTableJoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_TABLE_JOIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & seat & help;
		}

		// λ��
		UInt8	seat;
		// �ǲ���Э��
		UInt8	help;
	};

	/**
	*@brief world->client ֪ͨ�ͻ������µ�Բ�������Ա
	*/
	class WorldGuildTableNewMember : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_TABLE_NEW_MEMBER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & member;
		}

		GuildTableMember member;
	};

	/**
	*@brief world->client ֪ͨ�ͻ���ɾ��Բ�������Ա
	*/
	class WorldGuildTableDelMember : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_TABLE_DEL_MEMBER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		ObjID_t		id;
	};

	/**
	*@brief world->client ֪ͨ�ͻ��˵�Բ���������
	*/
	class WorldGuildTableFinish : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_TABLE_FINISH)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world �����ԷѺ��
	*/
	class WorldGuildPayRedPacketReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_PAY_REDPACKET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & packetId & name & num;
		}

		UInt16	packetId;
		std::string name;
		UInt8	num;
	};

	/**
	*@brief client->world ���󹫻�ս����
	*/
	class WorldGuildBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId;
		}

	public:
		//���ID
		UInt8	terrId;
	};

	/**
	*@brief world->client ���󹫻�ս��������
	*/
	class WorldGuildBattleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & terrId & enrollSize;
		}

	public:
		UInt32	result;
		UInt8	terrId;
		UInt32	enrollSize;
	};

	/**
	*@brief client->world �������
	*/
	class WorldGuildBattleInspireReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_INSPIRE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{ }
	};

	/**
	*@brief world->client ������践��
	*/
	class WorldGuildBattleInspireRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_INSPIRE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & inspire;
		}

	public:
		UInt32	result;
		//�������
		UInt8	inspire;
	};

	/**
	*@brief client->world ������ȡ����
	*/
	class WorldGuildBattleReceiveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RECEIVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & boxId;
		}

	public:
		UInt8 boxId;
	};

	/**
	*@brief world->client ��ȡ��������
	*/
	class WorldGuildBattleReceiveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RECEIVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & boxId;
		}

	public:
		UInt32	result;
		UInt8	boxId;
	};

	/**
	*@brief client->world �������ս����¼
	*/
	class WorldGuildBattleRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isSelf & startIndex & count;
		}

	public:
		UInt8 isSelf;
		Int32 startIndex;
		UInt32 count;
	};

	/**
	*@brief world->client ���ս����¼����
	*/
	class WorldGuildBattleRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & records;
		}

	public:
		UInt32	result;
		std::vector<GuildBattleRecord> records;
	};

	/**
	*@brief world->client ���ս����¼ͬ��
	*/
	class WorldGuildBattleRecordSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RECORD_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & record;
		}

	public:
		GuildBattleRecord record;
	};

	/**
	*@brief client->world ���������Ϣ
	*/
	class WorldGuildBattleTerritoryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_TERRITORY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId;
		}
	public:
		UInt8 terrId;
	};

	/**
	*@brief world->client ���������Ϣ
	*/
	class WorldGuildBattleTerritoryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_TERRITORY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & info;
		}
	public:
		UInt32 result;
		//�����Ϣ
		GuildTerritoryBaseInfo info;
	};

	/**
	*@brief world->scene ͬ����ҵĹ�����Ϣ��scene server
	*/
	class WorldGuildSyncToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SYNC_TO_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & guildId & guildLvl & name & post & shopId & enterTime & buildings & occupyCrossTerrId &
				guildBattleNumber & guildBattleScore & guildBattleLotteryStatus & guildBattleRewardMask & occupyTerrId;
		}

		// ID
		ObjID_t		id;
		// ����ID
		ObjID_t		guildId;
		// ����ȼ�
		UInt32		guildLvl;
		// ������
		std::string name;
		// ְλ
		UInt8		post;
		//����ʱ��
		UInt32		enterTime;
		// �̵�id
		UInt8		shopId;
		// ���Ὠ��
		std::vector<GuildBuilding> buildings;
		// ����ս������
		UInt8		occupyCrossTerrId;
		// ����ս����
		UInt32		guildBattleNumber;
		// ����ս����
		UInt32		guildBattleScore;
		// ����ս�齱״̬
		UInt8		guildBattleLotteryStatus;
		//���ά�����
		std::string	guildBattleRewardMask;
		// ����ս���
		UInt8		occupyTerrId;
	};

	/**
	*@brief world��scene֮�以��ͬ�����ṱ��
	*/
	class WorldGuildSyncContribution : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_UPDATE_CONTRI)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & isAdd & value & reason;
		}

		// ID
		ObjID_t		id;
		// �Ƿ����
		UInt8		isAdd;
		// ���ṱ��
		UInt32		value;
		// ԭ��
		std::string	reason;
	};

	/**
	*@brief scene->world ͬ�����Ἴ��
	*/
	class WorldGuildSyncSkill : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SYNC_SKILL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & skills;
		}

		ObjID_t id;
		std::vector<SkillBase> skills;
	};

	/**
	*@brief world->scene ͬ�����Ἴ��
	*/
	class WorldGuildUpdateSkill : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_UPDATE_SKILL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & skillType & skill;
		}

		ObjID_t id;
		UInt32 skillType;
		SkillBase skill;
	};

	/**
	*@brief client->scene ����һ�
	*/
	class SceneGuildExchangeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GUILD_EXCHANGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->scene ֪ͨ��ȡ����
	*/
	class WorldGuildGiveItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_GIVE_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & boxId & itemReward;
		}

		//���ID
		UInt64 playerId;
		//����ID
		UInt32 boxId;
		//������Ʒ
		std::vector<ItemReward> itemReward;
	};

	/**
	*@brief world->scene ��ȡ��������
	*/
	class WorldGuildGiveItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_GIVE_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & playerId & boxId;
		}

		UInt32 result;
		//���ID
		UInt64 playerId;
		//����ID
		UInt32 boxId;
	};

	/**
	*@brief world->client һ������ս����-����.
	*/
	class WorldGuildBattleRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RACE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & oldScore & newScore;
		}

		UInt8 result;
		UInt32 oldScore;
		UInt32 newScore;
	};

	/**
	*@brief world->client ����ս����
	*/
	class WorldGuildBattleEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		std::vector<GuildBattleEndInfo> info;
	};

	/**
	*@brief client->world ��������������
	*/
	class WorldGuildBattleSelfSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_SELF_SORTLIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief world->client ����������������Ӧ
	*/
	class WorldGuildBattleSelfSortListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_SELF_SORTLIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & memberRanking & guildRanking;
		}

		UInt32 result;
		UInt32 memberRanking;
		UInt32 guildRanking;
	};

	/**
	*@brief world->client ��������֪ͨ���б�����������빫��
	*/
	class WorldGuildInviteNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_INVITE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & inviterId & inviterName & inviterOccu & inviterLevel & inviterVipLevel
				& guildId & guildName & playerLabelInfo;
		}

		ObjID_t		inviterId;
		std::string	inviterName;
		UInt8		inviterOccu;
		UInt16		inviterLevel;
		UInt8		inviterVipLevel;
		ObjID_t		guildId;
		std::string guildName;
		//��ұ�ǩ��Ϣ
		PlayerLabelInfo playerLabelInfo;
	};

	/**
	*@brief world->client ����ս״̬ͬ��
	*/
	class WorldGuildBattleStatusSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_STATUS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & status & time & endInfo;
		}

		//����ս����
		UInt8 type;

		//״̬
		UInt8 status;

		//״̬������ʱ���
		UInt32 time;

		//����ս�������
		std::vector<GuildBattleEndInfo> endInfo;
	};

	/**
	*@brief client->world ���󹫻���ս����
	*/
	class WorldGuildChallengeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHALLENGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId & itemId & itemNum;
		}

		//���ID
		UInt8 terrId;
		//������ƷID
		UInt32 itemId;
		//������Ʒ����
		UInt32 itemNum;

	};


	/**
	*@brief world->client ���ع�����ս����
	*/
	class WorldGuildChallengeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHALLENGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32	result;
	};


	/**
	*@brief client->world ���󹫻���ս��Ϣ
	*/
	class WorldGuildChallengeInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHALLENGE_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ������ս��Ϣͬ��
	*/
	class WorldGuildChallengeInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHALLENGE_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & enrollGuildId & enrollGuildName & enrollGuildleaderName & enrollGuildLevel & itemId & itemNum;
		}

	public:
		//�����Ϣ
		GuildTerritoryBaseInfo info;

		//��ս����ID
		UInt64 enrollGuildId;

		//��ս��������
		std::string enrollGuildName;

		//��ս����᳤����
		std::string enrollGuildleaderName;

		//��ս����ȼ�
		UInt8 enrollGuildLevel;

		//���ĵ���id
		UInt32 itemId;

		//���ĵ�������
		UInt32 itemNum;
	};

	/**
	*@brief client->world ���󹫻�ս������Ϣ
	*/
	class WorldGuildBattleInspireInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_INSPIRE_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}

	};
	
	/**
	*@brief world->client ���ع���ս������Ϣ
	*/
	class WorldGuildBattleInspireInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_INSPIRE_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result &terrId & inspireInfos;
		}

		UInt32 result;

		//���ID
		UInt8 terrId;

		//��Ա������Ϣ
		std::vector<GuildBattleInspireInfo> inspireInfos;

	};

	/**
	*@brief client->world ���󹫻�����
	*/
	class WorldGuildStorageSettingReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_STORAGE_SETTING_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & value;
		}

		// ��������
		UInt8 type;

		// ������ֵ
		UInt32 value;

	};

	/**
	*@brief world->client ���ع�������
	*/
	class WorldGuildStorageSettingRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_STORAGE_SETTING_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		//���
		UInt32 result;

	};


	/**
	*@brief world->scene ���빫��ֿ�����
	*/
	class WSAddGuildStorageItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WS_GUILD_ADDSTORAGEITEM_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerUid & guildUid & items;
		}

		UInt64 playerUid;
		UInt64 guildUid;

		//����ĵ���
		std::vector<GuildStorageItemInfo> items;
	};

	/**
	*@brief world->scene �̵깺����빫��ֿⷵ��
	*/
	class WSShopBuyAddGuildStorageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WS_GUILDSTORAGE_ADD_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerUid & code & shopId & shopItemId & num & costItemId & costNum;
		}

		UInt64 playerUid;
		UInt32 code;
		UInt8 shopId;
		UInt32 shopItemId;
		UInt16 num;
		UInt32 costItemId;
		UInt32 costNum;
	};

	/**
	*@brief scene->world ���빫��ֿⷵ��
	*/
	class SWAddGuildStorageItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SW_GUILD_ADDSTORAGEITEM_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}

	};

	/**
	*@brief scene->world �̵깺����빫��ֿ�
	*/
	class SWShopBuyAddGuildStorage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SW_GUILDSTORAGE_ADD_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	};

	
	/**
	*@brief client->world ���󹫻�ս�齱
	*/
	class WorldGuildBattleLotteryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_LOTTERY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{}
	};

	/**
	*@brief world->client ���ع���ս�齱
	*/
	class WorldGuildBattleLotteryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_LOTTERY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & contribution;
		}

		UInt32 result;
		UInt32 contribution;
	};

	/**
	*@brief client->world ���󹫻�ֿ��б�
	*/
	class WorldGuildStorageListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_STORAGE_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{}
	};

	/**
	*@brief world->client ���ع���ֿ��б�
	*/
	class WorldGuildStorageListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_STORAGE_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & maxSize & items & itemRecords;
		}

		UInt32 result;

		UInt32 maxSize;

		std::vector<GuildStorageItemInfo> items;

		std::vector<GuildStorageOpRecord> itemRecords;
	};

	/**
	*@brief world->client ͬ���ֿ���Ʒ����
	*/
	class WorldGuildStorageItemSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_STORAGE_ITEM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items & itemRecords;
		}

		std::vector<GuildStorageItemInfo> items;

		std::vector<GuildStorageOpRecord> itemRecords;
	};

	/**
	*@brief client->world ������빫��ֿ�
	*/
	class WorldGuildAddStorageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_ADD_STORAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items;
		}

		//����ĵ���
		std::vector<GuildStorageItemInfo> items;
	};

	/**
	*@brief world->client ���ط��빫��ֿ�
	*/
	class WorldGuildAddStorageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_ADD_STORAGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief client->world ����ɾ������ֿ���Ʒ
	*/
	class WorldGuildDelStorageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DEL_STORAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items;
		}

		std::vector<GuildStorageDelItemInfo> items;
	};

	/**
	*@brief world->client ����ɾ������ֿ���Ʒ
	*/
	class WorldGuildDelStorageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DEL_STORAGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief client->world �鿴����ֿ������ϸ��Ϣ
	*/
	class WorldWatchGuildStorageItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_WATCH_STORAGE_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid;
		}

		UInt64 uid;
	};

	/**
	*@brief cross->world ͬ���������б���Ϣ
	*/
	class WorldCrossGuildBattleTerritoryListInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_TERRITORY_LIST_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & infos;
		}

		std::vector<GuildTerritoryBaseInfo> infos;
	};

	/**
	*@brief cross->world ͬ����������Ϣ
	*/
	class WorldCrossGuildBattleTerritoryInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_TERRITORY_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		GuildTerritoryBaseInfo info;
	};

	/**
	*@brief cross->world ͬ��������ս����Ϣ
	*/
	class WorldCrossGuildBattleRecordInfoSync :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_RECORD_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId & info;
		}

		//���ID
		UInt8 terrId;
		//ս����¼
		GuildBattleRecord info;
	};

	/**
	*@brief cross->world ͬ������ս������Ϣ
	*/
	class WorldCrossGuildBattleEndInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_END_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		std::vector<GuildBattleEndInfo> info;

	};


	/**
	*@brief cross->world ��world��������Ϣ
	*/
	class WorldCrossGuildBattleEnrollInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_ENROLL_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	};

	/**
	*@brief world->cross ��world��������Ϣ����
	*/
	class WorldCrossGuildBattleEnrollInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_ENROLL_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & terrEnrollInfo & terrBaseInfo;
		}

		UInt32 result;
		std::map<UInt8, std::vector<CSEnrollInfo>> terrEnrollInfo;
		std::vector<GuildTerritoryBaseInfo> terrBaseInfo;
	};

	/**
	*@brief world->cross ͬ�����а���Ϣ
	*/
	class CrossGuildBattleSortListSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_SORT_LIST_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId & entry;
		}

		UInt8 terrId;

		TerritorySortListEntry entry;
	};

	/**
	*@brief world->cross ͬ��GM����
	*/
	class CrossGuildBattleGMSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_GM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isByTime & targetStatus;
		}

		UInt8 isByTime;
		UInt8 targetStatus;
	};

	/**
	*@brief world->cross ֪ͨCross������Ϣ
	*/
	class CrossGuildBattleEnrollReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_ENROLL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		CSEnrollInfo info;
	};

	/**
	*@brief cross->world ֪ͨCross������Ϣ����
	*/
	class CrossGuildBattleEnrollRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_ENROLL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief world->cross ���������ս����¼
	*/
	class CrossGuildBattleRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & terrId & isSelf & startIndex & count;
		}

		UInt64 playerId;
		UInt8 terrId;
		UInt8 isSelf;
		Int32 startIndex;
		UInt32 count;
	};

	/**
	*@brief cross->world ���������ս����¼����
	*/
	class CrossGuildBattleRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result & records;
		}

		UInt64 playerId;
		UInt32 result;
		std::vector<GuildBattleRecord> records;
	};

	/**
	*@brief world->cross ���������а��¼
	*/
	class CrossGuildBattleSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_SORT_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId & playerId & guildId & type & start & num;
		}

		UInt8 terrId;
		//���ID
		ObjID_t playerId;
		//����ID
		ObjID_t guildId;
		//���а�����
		UInt8	type;
		//��ʼλ�� 0��ʼ
		UInt16	start;
		//����
		UInt16	num;
	};

	/**
	*@brief cross->world ������������а񷵻�
	*/
	class CrossGuildBattleSortListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_SORT_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & (*packet);
		}

		CrossGuildBattleSortListRes() { packet = Avalon::Packet::Create(); }
		CrossGuildBattleSortListRes(Avalon::Packet* pack) { packet = pack; }
		~CrossGuildBattleSortListRes() { Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		ObjID_t playerId;
		Avalon::Packet* packet;
	};

	/**
	*@brief world->cross ����������������а�
	*/
	class CrossGuildBattleSelfSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_SELF_SORT_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & guildId & terrId;
		}

		//���ID
		ObjID_t playerId;
		//����ID
		ObjID_t guildId;
		// ���ID
		UInt8 terrId;
	};

	/**
	*@brief cross->world ������������а񷵻�
	*/
	class CrossGuildBattleSelfSortListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_SELF_SORT_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & (*packet);
		}

		CrossGuildBattleSelfSortListRes() { packet = Avalon::Packet::Create(); }
		CrossGuildBattleSelfSortListRes(Avalon::Packet* pack) { packet = pack; }
		~CrossGuildBattleSelfSortListRes() { Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		ObjID_t playerId;
		Avalon::Packet* packet;
	};
	
	/**
	*@brief client->world ���ü��빫��ȼ�����
	*/
	class WorldGuildSetJoinLevelReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SET_JOIN_LEVEL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & joinLevel;
		}

		UInt32 joinLevel;
	};

	/**
	*@brief world->client ���ü��빫��ȼ�����
	*/
	class WorldGuildSetJoinLevelRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SET_JOIN_LEVEL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};


	/**
	*@brief client->world ����ռ���������
	*/
	class WorldGuildEmblemUpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_EMBLEM_UP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ����ռ���������
	*/
	class WorldGuildEmblemUpRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_EMBLEM_UP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & emblemLevel;
		}

		UInt32 result;
		UInt32 emblemLevel;
	};

	/**
	*@brief world->scene ����ռ�ͬ����
	*/
	class WorldGuildEmblemSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_EMBLEM_SYNC_TO_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & emblemLevel;
		}

		UInt64 roleId;
		UInt32 emblemLevel;
	};
	
	/**
	*@brief client->world ���ù��ḱ���Ѷ�����
	*/
	class WorldGuildSetDungeonTypeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SET_DUNGEON_TYPE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonType;
		}

		// ���ḱ���Ѷ�
		UInt32 dungeonType;
	};

	/**
	*@brief world->client ���ù��ḱ���Ѷȷ���
	*/
	class WorldGuildSetDungeonTypeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SET_DUNGEON_TYPE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & dungeonType;
		}

		// ���
		UInt32 result;
		// ���ḱ���Ѷ�
		UInt32 dungeonType;
	};

	/**
	*@brief client->world �������ÿ�ս�������
	*/
	class WorldGuildGetTerrDayRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_GET_TERR_DAY_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief world->client �������ÿ�ս�������
	*/
	class WorldGuildGetTerrDayRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_GET_TERR_DAY_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};


	//********************������³�**************************

	/**
	*@brief client->world ������³���Ϣ����
	*/
	class WorldGuildDungeonInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	struct GuildDungeonBossBlood
	{
		GuildDungeonBossBlood() : dungeonId(0), oddBlood(0), verifyBlood(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & oddBlood & verifyBlood;
		}

		// ���³�id
		UInt32 dungeonId;
		// ʣ��Ѫ��
		UInt64 oddBlood;
		// ����֤Ѫ��
		UInt64 verifyBlood;
	};

	struct GuildDungeonClearGateTime
	{
		GuildDungeonClearGateTime() : spendTime(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildName & spendTime;
		}
		// ��������
		std::string guildName;
		// ͨ����ʱ
		UInt64 spendTime;
	};

	/**
	*@brief world->client ������³���Ϣ����
	*/
	class WorldGuildDungeonInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & dungeonStatus & statusEndStamp & isReward & bossBlood & clearGateVec;
		}

		// ���ؽ��
		UInt32 result;
		// ���³�״̬
		UInt32 dungeonStatus;
		// ״̬������ʱ���
		UInt32 statusEndStamp;
		// �Ƿ��Ѿ���ȡ��������
		UInt32 isReward;
		// bossѪ����Ϣ
		std::vector<GuildDungeonBossBlood> bossBlood;
		// ͨ��ʱ�����а�
		std::vector<GuildDungeonClearGateTime> clearGateVec;
	};
	
	/**
	*@brief client->world ������³��˺���������
	*/
	class WorldGuildDungeonDamageRankReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_DAMAGE_RANK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	// �˺���Ϣ
	struct GuildDungeonDamage
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rank & damageVal & playerId& playerName;
		}

		// ����
		UInt32 rank;
		// �˺�ֵ
		UInt64 damageVal;
		// ���ID
		UInt64 playerId;
		// �������
		std::string playerName;

		GuildDungeonDamage() : rank(0), damageVal(0), playerId(0) {}
	};

	/**
	*@brief world->client ������³��˺����з���
	*/
	class WorldGuildDungeonDamageRankRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_DAMAGE_RANK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & damageVec;
		}

		// �˺��б�
		std::vector<GuildDungeonDamage> damageVec;
	};

	/**
	*@brief client->world ������³Ǹ�����Ϣ����
	*/
	class WorldGuildDungeonCopyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_COPY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ������³Ǹ�����Ϣ����
	*/
	class WorldGuildDungeonCopyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_COPY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & BattleRecordVec;
		}

		// ������³���Ϣ
		std::vector<GuildDungeonBattleRecord> BattleRecordVec;
	};

	/**
	*@brief client->world 
	*/
	class WorldGuildDungeonLotteryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_LOTTERY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ������³ǳ齱����
	*/
	class WorldGuildDungeonLotteryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_LOTTERY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & lotteryItemVec;
		}

		// ���
		UInt32 result;
		// ����
		GuildDungeonRewardVec lotteryItemVec;
	};

	/**
	*@brief world->client ������³�״̬ͬ��
	*/
	class WorldGuildDungeonStatusSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_STATUS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonStatus;
		}

		// ���³�״̬
		UInt32 dungeonStatus;
	};

	/**
	*@brief client->world ������³ǵ�������
	*/
	class WorldGuildDungeonStatueReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_STATUE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ������³ǵ��񷵻�
	*/
	class WorldGuildDungeonStatueRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_STATUE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & figureStatue;
		}

		// Ҫչʾ���������
		std::vector<FigureStatueInfo> figureStatue;
	};

	/**
	*@brief world->client ������³�boss����֪ͨ
	*/
	class WorldGuildDungeonBossDeadNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_BOSS_DEAD_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId;
		}

		// ���³�id
		UInt32 dungeonId;
	};
	
	/**
	*@brief world->client ������³ǽ���֪ͨ
	*/
	class WorldGuildDungeonEndNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_END_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId;
		}

		UInt32 dungeonId;
	};

	/**
	*@brief world->client ������³�bossʣ��Ѫ��
	*/
	class WorldGuildDungeonBossOddBlood : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_BOSS_ODD_BLOOD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bossOddBlood & bossTotalBlood;
		}

		UInt64 bossOddBlood;
		UInt64 bossTotalBlood;
	};

	/**
	*@brief client->world ��������Ϣ����
	*/
	class WorldGuildAuctionItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		// ��������
		UInt32 type;
	};

	/**
	*@brief world->client ��������Ϣ����
	*/
	class WorldGuildAuctionItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & auctionItemVec;
		}

		// ��������
		UInt32 type;
		// ������Ʒ�б�
		std::vector<GuildAuctionItem> auctionItemVec;
	};

	/**
	*@brief client->world �����г�������
	*/
	class WorldGuildAuctionBidReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_BID_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & bidPrice;
		}

		// guid
		UInt64 guid;
		// ����
		UInt32 bidPrice;
	};

	/**
	*@brief world->client �����г��۷���
	*/
	class WorldGuildAuctionBidRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_BID_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		// ������
		UInt32 retCode;
	};

	/**
	*@brief client->world ������һ�ڼ�����
	*/
	class WorldGuildAuctionFixReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_FIX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid;
		}

		// guid
		UInt64 guid;
	};

	/**
	*@brief world->client ������һ�ڼ۷���
	*/
	class WorldGuildAuctionFixRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_FIX_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		// ������
		UInt32 retCode;
	};

	/**
	*@brief world->client ������֪ͨ
	*/
	class WorldGuildAuctionNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & isOpen;
		}

		// ����
		UInt32 type;
		// �Ƿ��
		UInt32 isOpen;
	};


	/**
	*@brief client->world �鿴�ɼ沢�Ĺ����б�����
	*/
	class WorldGuildWatchCanMergerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_WATCH_CAN_MERGER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & start & num;
		}
		//��ʼλ��
		UInt16	start;
		//����
		UInt16	num;
	};

	/**
	*@brief world -> client  �鿴�ɼ沢�Ĺ����б���
	*/
	class WorldGuildWatchCanMergerRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_WATCH_CAN_MERGER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & start & totalNum & guilds;
		}

		//��ʼλ��
		UInt16	start;
		//����
		UInt16	totalNum;
		//�����б�
		std::vector<GuildEntry>	guilds;
	};

	/**
	*@brief client -> world �沢��������������沢��ȡ������沢����
	*/
	class WorldGuildMergerRequestOperatorReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MERGER_REQUEST_OPERATOR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildId & opType;
		}
		UInt64 guildId;
		//�������� 0����沢 1ȡ������沢
		UInt8 opType;
	};

	/**
	*@brief world -> client �沢�������
	*/
	class WorldGuildMergerRequestOperatorRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MERGER_REQUEST_OPERATOR_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode & opType;
		}

		//�������
		UInt32 errorCode;
		//�������� 0����沢 1ȡ������沢
		UInt8 opType;
	};

	/**
	*@brief client ->  world ��ѯ�沢����
	*/
	class WorldGuildReceiveMergerRequestReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_RECEIVE_MERGER_REQUEST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world -> client  ���ؼ沢����
	*/
	class WorldGuildReceiveMergerRequestRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_RECEIVE_MERGER_REQUEST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isHave;
		}
		//�Ƿ��м沢���� 0û�� 1��
		UInt8 isHave;
	};



	/**
	*@brief client ->  world �鿴�������յ��ļ沢�����б�
	*/
	class WorldGuildWatchHavedMergerRequestReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_WATCH_HAVED_MERGER_REQUEST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world -> client  ���ر������յ��ļ沢�����б�
	*/
	class WorldGuildWatchHavedMergerRequestRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_WATCH_HAVED_MERGER_REQUEST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guilds;
		}
		//�����б� �˴���Ҫһ������ṹ
		std::vector<GuildEntry>	guilds;
	};


	/**
	*@brief client ->  world ͬ�⣬�ܾ�����沢���룬ȡ����ͬ�������
	*/
	class WorldGuildAcceptOrRefuseOrCancleMergerRequestReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_ACCEPT_OR_REFUSE_OR_CANCLE_MERGER_REQUEST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildId & opType;
		}
		UInt64 guildId;
		//�������� 0ͬ�� 1�ܾ� 2ȡ�� 3�������
		UInt8 opType;
	};

	/**
	*@brief world -> client  ������������������
	*/
	class WorldGuildAcceptOrRefuseOrCancleMergerRequestRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_ACCEPT_OR_REFUSE_OR_CANCLE_MERGER_REQUEST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode & opType;
		}
		UInt32 errorCode;
		//�������� 0ͬ�� 1�ܾ� 2ȡ�� 3�������
		UInt8 opType;
	};
	//**************************************

	class WorldGuildSyncMergerInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SYNC_MERGER_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & prosperityStatus & mergerRequsetStatus;
		}
		//���ᷱ��״̬ 1��ɢ 2�ͷ��� 3�з��� 4�߷���
		UInt8 prosperityStatus;
		//����״̬ 0������ 1������ 2�ѽ���
		UInt8 mergerRequsetStatus;
	};
	
	/**
	 *@brief  client->world ��ȡ�����¼��б�
	 */
	class WorldGuildEventListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_EVENT_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uptime;
		}

	public:
		//�ϴ���ȡ��ʱ���,0Ϊ��һ����ȡ
		UInt32	uptime;
	};

	/**
	 *@brief  world->client ��ȡ�����¼��б���
	 */
	class WorldGuildEventListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_EVENT_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uptime & guildEvents;
		}

	public:
		//��ǰ�¼���, �����´���ȡ�ο�
		UInt32	uptime;
		//�����¼��б�
		std::vector<GuildEvent>	guildEvents;
	};


	/**
	*@brief server->world ͬ��������־
	*/
	class SyncGuildEventToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYNC_GUILD_EVENT_TO_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & content;
		}

		// guildID
		ObjID_t		id;
		// ������־
		std::string	content;
	};

	/**
	*@brief client->world ���󹫻�����
	*/
	class WorldGuildHonorInfoReq: public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_HONOR_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildId & groupIds;
		}
		//��������
		UInt64			guildId;
		std::vector<UInt32> groupIds;
	};


	class WorldGuildHonorInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_HONOR_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildId & guildName & honors;
		}
		UInt64					guildId;
		std::string				guildName;
		//��������
		std::vector<GuildHonor> honors;
	};
}



#endif // _CL_GUILD_PROTOCOL_H_