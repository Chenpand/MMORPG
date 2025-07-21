#ifndef _CL_ADVENTURE_TEAM_PROTOCOL_H_
#define _CL_ADVENTURE_TEAM_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLGameDefine.h"

namespace CLProtocol
{
	/**
	*@brief ð�ն���չ��Ϣ
	*/
	struct AdventureTeamExtraInfo : public AdventureTeamInfo
	{
		AdventureTeamExtraInfo() :adventureTeamRanking(0), ownRoleFileds(0), roleTotalScore(0)
		{}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & adventureTeamName & adventureTeamLevel & adventureTeamExp & adventureTeamGrade 
				& adventureTeamRanking & ownRoleFileds & roleTotalScore;
		}

		// ð�ն�����
		std::string	adventureTeamGrade;
		// ð�ն�����
		UInt32 adventureTeamRanking;
		// ӵ�н�ɫ��λ��
		UInt32 ownRoleFileds;
		// ð���Ž�ɫ������
		UInt32 roleTotalScore;
	};

	/**
	*@brief Զ����Ա��Ϣ
	*/
	struct ExpeditionMemberInfo
	{
		ExpeditionMemberInfo()
		: roleid(0), level(0), occu(0), expeditionMapId(0)
		{}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & name & level & occu & avatar & expeditionMapId;
		}

		// ��ɫid
		ObjID_t roleid;
		// ��ɫ��
		std::string name;
		// ��ɫ�ȼ�
		UInt16 level;
		// ��ɫְҵ
		UInt8 occu;
		// ��ɫ���
		PlayerAvatar avatar;
		// Զ����ͼid
		UInt8 expeditionMapId;
	};

	/**
	*@brief Զ����ͼ������Ϣ
	*/
	struct ExpeditionMapBaseInfo
	{
		ExpeditionMapBaseInfo()
		: mapId(0), expeditionStatus(0), memberNum(0)
		{}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & expeditionStatus & memberNum;
		}

		// ��ͼid
		UInt8 mapId;
		// Զ��״̬(��Ӧö��ExpeditionStatus)
		UInt8 expeditionStatus;
		// Զ����Ա��
		UInt16 memberNum;
	};

	/**
	 *@brief server->client ͬ��ð�ն���Ϣ
	 */
	class AdventureTeamInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SYNC_ADVENTURE_TEAM_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		// ð�ն���Ϣ
		AdventureTeamExtraInfo info;
	};

	/**
	*@brief client->world ð�նӶ����޸�����
	*/
	class WorldAdventureTeamRenameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADVENTURE_TEAM_RENAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & newName & costItemUId & costItemDataId;
		}

	public:
		// Ҫ�޸ĵ�������
		std::string newName;
		// ��Ҫ���ĵĵ���
		ObjID_t costItemUId;
		UInt32 costItemDataId;
	};

	/**
	*@brief world->client ð�նӶ����޸ķ���
	*/
	class WorldAdventureTeamRenameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADVENTURE_TEAM_RENAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & newName;
		}

	public:
		// ������
		UInt32 resCode;
		// ������
		std::string newName;
	};

	/**
	*@brief client->world ð�ն���չ��Ϣ����
	*/
	class WorldAdventureTeamExtraInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADVENTURE_TEAM_EXTRA_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ð�ն���չ��Ϣ����
	*/
	class WorldAdventureTeamExtraInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADVENTURE_TEAM_EXTRA_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & extraInfo;
		}

	public:
		// ������
		UInt32 resCode;
		AdventureTeamExtraInfo extraInfo;
	};

	/**
	*@brief client->world �͸�ˮ����Ϣ����
	*/
	class WorldBlessCrystalInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLESS_CRYSTAL_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client �͸�ˮ����Ϣ����
	*/
	class WorldBlessCrystalInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLESS_CRYSTAL_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & ownBlessCrystalNum & ownBlessCrystalExp;
		}

		// ������
		UInt32 resCode;
		// ӵ�еĴ͸�ˮ������
		UInt32 ownBlessCrystalNum;
		// ӵ�еĴ͸�ˮ������
		UInt64 ownBlessCrystalExp;
	};

	/**
	*@brief client->world ����ף����Ϣ����
	*/
	class WorldInheritBlessInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INHERIT_BLESS_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ����ף����Ϣ����
	*/
	class WorldInheritBlessInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INHERIT_BLESS_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & ownInheritBlessNum & ownInheritBlessExp;
		}

		// ������
		UInt32 resCode;
		// ӵ�еĴ���ף������
		UInt32 ownInheritBlessNum;
		// ӵ�еĴ���ף������
		UInt64 ownInheritBlessExp;
	};

	/**
	*@brief client->world ���о�������
	*/
	class WorldInheritExpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INHERIT_EXP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ���о��鷵��
	*/
	class WorldInheritExpRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INHERIT_EXP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & ownInheritBlessNum & ownInheritBlessExp;
		}

		// ������
		UInt32 resCode;
		// ӵ�еĴ���ף������
		UInt32 ownInheritBlessNum;
		// ӵ�еĴ���ף������
		UInt64 ownInheritBlessExp;
	};

	/**
	*@brief client->world Զ����ͼ��ѯ����
	*/
	class WorldQueryExpeditionMapReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_EXPEDITION_MAP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId;
		}

	public:
		// ��ͼid
		UInt8 mapId;
	};

	/**
	*@brief world->client Զ����ͼ��ѯ����
	*/
	class WorldQueryExpeditionMapRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_EXPEDITION_MAP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & mapId & expeditionStatus & durationOfExpedition 
				& endTimeOfExpedition & members;
		}

	public:
		// ������
		UInt32 resCode;
		// ��ͼid
		UInt8 mapId;
		// Զ��״̬(��Ӧö��ExpeditionStatus)
		UInt8 expeditionStatus;

		// Զ������ʱ��(Сʱ)
		UInt32 durationOfExpedition;
		// Զ������ʱ��
		UInt32 endTimeOfExpedition;
		// Զ����Ա��Ϣ
		std::vector<ExpeditionMemberInfo> members;
	};

	/**
	*@brief client->world ��ѡԶ����ɫ��ѯ����
	*/
	class WorldQueryOptionalExpeditionRolesReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_OPTIONAL_EXPEDITION_ROLES_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ��ѡԶ����ɫ��ѯ����
	*/
	class WorldQueryOptionalExpeditionRolesRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_OPTIONAL_EXPEDITION_ROLES_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & roles;
		}

	public:
		// ������
		UInt32 resCode;
		// ��ѡԶ����ɫ�б�
		std::vector<ExpeditionMemberInfo> roles;
	};

	/**
	*@brief client->world ��ǲԶ��������
	*/
	class WorldDispatchExpeditionTeamReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISPATCH_EXPEDITION_TEAM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & members & housOfduration;
		}

	public:
		// ��ͼid
		UInt8 mapId;
		// Զ����Ա(��ɫid�б�)
		std::vector<ObjID_t> members;
		// Զ��ʱ��(����ʱ�� Сʱ)
		UInt32 housOfduration;
	};

	/**
	*@brief world->client ��ǲԶ���ӷ���
	*/
	class WorldDispatchExpeditionTeamRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISPATCH_EXPEDITION_TEAM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & mapId & expeditionStatus & durationOfExpedition
				& endTimeOfExpedition & members;
		}

		// ������
		UInt32 resCode;
		// ��ͼid
		UInt8 mapId;
		// Զ��״̬(��Ӧö��ExpeditionStatus)
		UInt8 expeditionStatus;

		// Զ������ʱ��(Сʱ)
		UInt32 durationOfExpedition;
		// Զ������ʱ��
		UInt32 endTimeOfExpedition;
		// Զ����Ա��Ϣ
		std::vector<ExpeditionMemberInfo> members;
	};

	/**
	*@brief client->world ȡ��Զ������
	*/
	class WorldCancelExpeditionReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CANCEL_EXPEDITION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId;
		}

	public:
		// ��ͼid
		UInt8 mapId;
	};

	/**
	*@brief world->client ȡ��Զ������
	*/
	class WorldCancelExpeditionRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CANCEL_EXPEDITION_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & mapId & expeditionStatus;
		}

		// ������
		UInt32 resCode;
		// ��ͼid
		UInt8 mapId;
		// Զ��״̬
		UInt8 expeditionStatus;
	};

	/**
	*@brief client->world Զ��������ȡ����
	*/
	class WorldGetExpeditionRewardsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_EXPEDITION_REWARDS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId;
		}

	public:
		// ��ͼid
		UInt8 mapId;
	};

	/**
	*@brief world->client Զ��������ȡ����
	*/
	class WorldGetExpeditionRewardsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_EXPEDITION_REWARDS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & mapId & expeditionStatus;
		}

		// ������
		UInt32 resCode;
		// ��ͼid
		UInt8 mapId;
		// Զ��״̬
		UInt8 expeditionStatus;
	};

	/**
	*@brief client->world ��ѯȫ��Զ����ͼ����
	*/
	class WorldQueryAllExpeditionMapsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_ALL_EXPEDITION_MAPS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapIds;
		}

	public:
		// ��ͼid��
		std::vector<UInt8> mapIds;
	};

	/**
	*@brief world->client ��ѯȫ��Զ����ͼ����
	*/
	class WorldQueryAllExpeditionMapsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_ALL_EXPEDITION_MAPS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & mapBaseInfos;
		}

	public:
		// ������
		UInt32 resCode;
		std::vector<ExpeditionMapBaseInfo> mapBaseInfos;
	};

	/**
	*@brief client->world �����ѯӵ�е�ְҵ
	*/
	class WorldQueryOwnOccupationsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_OWN_OCCUPATIONS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & baseOccus;
		}

	public:
		// ����ְҵ
		std::vector<UInt8> baseOccus;
	};

	/**
	*@brief world->client ��ѯӵ�е�ְҵ����
	*/
	class WorldQueryOwnOccupationsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_OWN_OCCUPATIONS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & occus;
		}

	public:
		// ������
		UInt32 resCode;
		// ӵ�е�ְҵ
		std::vector<UInt8> occus;
	};

	/**
	*@brief world->client ͬ��ӵ�е���ְҵ
	*/
	class WorldQueryOwnNewOccupationsSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_OWN_NEW_OCCUPATIONS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ownNewOccus;
		}

	public:
		// ӵ�е���ְҵ
		std::vector<UInt8> ownNewOccus;
	};

	/**
	*@brief client->world ���������������ְҵ
	*/
	class WorldRemoveUnlockedNewOccupationsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REMOVE_UNLOCKED_NEW_OCCUPATIONS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & newOccus;
		}

	public:
		// ְҵ
		std::vector<UInt8> newOccus;;
	};

	/**
	*@brief world->server ͬ��ð�ն���Ϣ
	*/
	class WorldAdventureTeamInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADVENTURE_TEAM_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & info & isCreate;
		}

	public:
		ObjID_t roleId;
		// ð�ն���Ϣ
		AdventureTeamInfo info;
		// �Ƿ񴴽�Ӷ����
		UInt8 isCreate;
	};

	/**
	*@brief world->client ͬ��ȫ��Զ����ͼ��Ϣ
	*/
	class WorldAllExpeditionMapsSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ALL_EXPEDITION_MAPS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapBaseInfos;
		}

	public:
		std::vector<ExpeditionMapBaseInfo> mapBaseInfos;
	};

	/**
	*@brief gate->world ֪ͨ���¼����ɫ������
	*/
	class GateNotifyReCalculateTotalRoleValueScore : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_RECALCULATE_TOTAL_ROLE_VALUE_SCORE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & reason & accid;
		}

	public:
		std::string reason;
		UInt32 accid;
	};

};

#endif
