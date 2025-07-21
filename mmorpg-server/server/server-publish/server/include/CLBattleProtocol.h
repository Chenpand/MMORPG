#ifndef _CL_BATTLE_PROTOCOL_H_
#define _CL_BATTLE_PROTOCOL_H_

#include <AvalonSocket.h>
#include "CLProtocolDefine.h"
#include "CLItemDefine.h"
#include "CLSkillDefine.h"

namespace CLProtocol
{

	/**
	*@brief scene->bscene �������ս������
	*/
	class BattleEnterSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_ENTER_SCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief bscene->scene ����ս����������
	*/
	class BattleEnterSceneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_ENTER_SCENE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & battleSceneNodeId & battleId & battleType;
		}

		ObjID_t roleId;
		UInt32 result;
		UInt32 battleSceneNodeId;
		UInt32 battleId;
		UInt8	battleType;
	};

	/**
	*@brief bscene->scene �˳�ս������
	*/
	class BattleExitSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_EXIT_SCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & battleType & result & battleId & floor & battleDead;
		}

		BattleExitSceneReq()
		{
			roleId = 0;
			battleType = 0;
			result = 0;
			battleId = 0;
			floor = 0;
			battleDead = 0;
		}

		ObjID_t roleId;
		UInt8	battleType;

		UInt8   result;
		UInt32	battleId;
		UInt32	floor;
		UInt8	battleDead;
	};

	/**
	*@brief scene->bscene �˳�ս������
	*/
	class BattleExitSceneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_EXIT_SCENE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & battleType & battleResult & battleId & revive & reviveItem;
		}

		BattleExitSceneRes()
		{
			revive = 0;
		}

		ObjID_t roleId;
		UInt32 result;

		UInt8	battleType;
		UInt8   battleResult;
		UInt32	battleId;
		UInt8   revive; //(���ս��)
		UInt32	reviveItem; //�������id
	};

	/**
	*@brief client->battle ս��PK����
	*/
	class BattlePkSomeOneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_PK_SOMEONE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dstId & battleID & dungeonID;
		}

		ObjID_t roleId;
		ObjID_t dstId;
		UInt32 battleID;
		UInt32 dungeonID;
	};

	/**
	*@brief battle->client ս��PK����
	*/
	class BattlePkSomeOneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_PK_SOMEONE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & roleId & dstId;
		}

		UInt32 retCode;
		ObjID_t roleId;
		ObjID_t dstId;
	};

	/**
	*@brief client->battle �Լ�����Ŀ������
	*/
	class BattleLockSomeOneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_LOCK_SOMEONE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dstId & battleID;
		}

		ObjID_t roleId;
		ObjID_t dstId;
		UInt32 battleID;
	};

	/**
	*@brief battle->client �Լ�����Ŀ�귵��
	*/
	class BattleLockSomeOneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_LOCK_SOMEONE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & roleId & dstId;
		}

		UInt32 retCode;
		ObjID_t roleId;
		ObjID_t dstId;
	};

	/**
	*@brief battle->client �Լ�����
	*/
	class SceneBattleBalanceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_BALANCE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & rank & playerNum & kills & survivalTime & score & battleID & getHonor;
		}

	public:
		UInt64 roleId;
		UInt32 rank;
		UInt32 playerNum;
		UInt32 kills;
		UInt32 survivalTime;
		UInt32 score;
		UInt32 battleID;
		UInt32 getHonor;
	};

	/**
	*@brief client->battle ս����������
	*/
	class BattleEnrollReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_ENROLL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isMatch & accId & roleId & playerName & playerOccu;
		}

		UInt32 isMatch;
		UInt32 accId;
		ObjID_t roleId;
		std::string playerName;
		UInt8 playerOccu;
	};

	/**
	*@brief battle->client ս����������
	*/
	class BattleEnrollRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_ENROLL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isMatch & retCode;
		}

		// 0ȡ����������0����
		UInt32 isMatch;
		UInt32 retCode;
	};

	/**
	*@brief client->bscene ʰȡ������������
	*/
	class SceneBattlePickUpItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PICK_UP_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid & useAll & param1 & param2 & battleID;
		}

	public:
		//����guid
		ObjID_t	itemid;
		//�Ƿ�һ��ʹ��ȫ��
		UInt8 useAll;
		//Ԥ������1
		UInt32 param1;
		//Ԥ������2
		UInt32 param2;
		//
		UInt32 battleID;
	};

	// ʰȡ�������߷���
	class SceneBattlePickUpItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PICK_UP_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errcode;
		}

	public:
		UInt32 errcode;
	};

	class SceneBattleChangeSkillsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_USE_SKILL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & changeSkills;
		}

	public:

		// ������������
		UInt8 configType;

		//�仯�ļ����б�
		std::vector<ChangeSkill> changeSkills;
	};

	class SceneBattleChangeSkillsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_USE_SKILL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32	result;
	};

	struct PlayerSubjectInfo
	{
		PlayerSubjectInfo()
		{
			m_AccId = 0;
			m_PlayerId = 0;
			m_PlayerOccu = 0;
		}

		~PlayerSubjectInfo()
		{
			
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_AccId & m_PlayerId & m_PlayerName & m_PlayerOccu;
		}

		//�˺�ID
		UInt32 m_AccId;
		//���ID
		UInt64 m_PlayerId;
		//�������
		std::string m_PlayerName;
		//���ְҵ
		UInt8 m_PlayerOccu;
	};

	class SceneBattleMatchSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_MATCH_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & suvivalNum & players & sceneNodeID;
		}

	public:
		UInt32 battleID;
		UInt32 suvivalNum;
		std::vector<PlayerSubjectInfo> players;
		UInt32 sceneNodeID;
	};

	class SceneBattleCreateBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CREATE_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleType & battleID & playerIDs & sceneId & battleDataId & lostDungeonFloor;
		}

	public:
		UInt32				battleType;
		UInt32				battleID;
		std::vector<UInt64> playerIDs;
		UInt32				sceneId;
		UInt32				battleDataId;
		UInt32				lostDungeonFloor;
	};

	class SceneBattleCreateBattleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CREATE_BATTLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleType & battleID & sceneId & result;
		}

	public:
		UInt32				battleType;
		UInt32				battleID;
		UInt32				sceneId;
		UInt32				result;
	};

	class SceneBattleDestroyBattle : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_DESTROY_BATTLE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleType & battleID & reason;
		}

	public:
		UInt32		battleType;
		UInt32		battleID;
		UInt8		reason;
	};

	class BattleNotifyPrepareInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_NOTIFY_PREPARE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerNum & totalNum;
		}

		UInt32 playerNum;
		UInt32 totalNum;
	};

	class BattleNotifyBattleInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_NOTIFY_BATTLE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerNum;
		}

	public:
		UInt32 battleID;
		UInt32 playerNum;
	};

	class BattleServerSpecifyPkRobot : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_SEPCIFY_PK_ROBOT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & hard & occu & ai;
		}

	public:
		// �Ѷ�
		UInt8		hard;
		// ְҵ
		UInt8		occu;
		// ai
		UInt8		ai;
	};

	class BattleServerAddPkRobot : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_ADD_PK_ROBOT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	class SceneBattleKillSomeOne : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_KILL_SOMEONE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & killerID & remainHp & remainMp & otherID;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		UInt64 killerID;
		// ʤ��ʣ��Ѫ��
		Int32  remainHp;
		// ʤ��ʣ������
		Int32  remainMp;
		//
		UInt64 otherID;
	};

	class SceneBattleSyncPoisonRing : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_SYNC_POISON_RING)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & x & y & radius & beginTimestamp & interval & x1 & y1 & radius1;
		}

	public:
		UInt32  battleID;
		UInt32	x;
		UInt32	y;
		UInt32	radius;
		UInt32	beginTimestamp;
		UInt32	interval;
		UInt32	x1;
		UInt32	y1;
		UInt32	radius1;
	};

	class SceneBattlePickUpSpoilsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PICKUP_SPOILS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & itemGuid;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		UInt64 itemGuid;
	};

	class SceneBattlePickUpSpoilsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PICKUP_SPOILS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & itemGuid;
		}

	public:
		UInt32 retCode;
		UInt64 itemGuid;
	};

	class SceneBattleSelectOccuReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_SELECT_OCCU_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & occu;
		}

	public:
		UInt8 occu;
	};

	class SceneBattleSelectOccuRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_SELECT_OCCU_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

	public:
		UInt32 retCode;
	};

	class SceneBattleNotifySpoilsItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_SPOILS_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & detailItems;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		//����ĵ���
		std::vector<DetailItem>	detailItems;
	};

	/**
	*@brief client->scene ��������
	*/
	class SceneBattleBirthTransfer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_BIRTH_TRANSFER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & regionID;
		}

	public:
		UInt32 battleID;
		UInt32 regionID;
	};

	class SceneBattleBirthTransferNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_BIRTH_TRANSFER_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & birthPosX & birthPosY;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		UInt32 birthPosX;
		UInt32 birthPosY;
	};

	class SceneBattleNotifyWaveInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_WAVE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & waveID;
		}

	public:
		UInt32 battleID;
		UInt32 waveID;
	};

	class SceneBattleNotifySomeoneDead : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_SOMEONE_DEAD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & killerID & reason & kills & suvivalNum;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		UInt64 killerID;
		UInt32 reason;
		UInt32 kills;
		UInt32 suvivalNum;
	};

	class SceneBattleThrowSomeoneItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_THROW_SOMEONE_TIEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & targetID & itemGuid;
		}

	public:
		UInt32 battleID;
		UInt64 targetID;
		UInt64 itemGuid;
	};

	class SceneBattleThrowSomeoneItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_THROW_SOMEONE_TIEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & attackID & targetID & itemGuid & itemDataID & targetName & param;
		}

		SceneBattleThrowSomeoneItemRes() : param(0) {}
	public:
		UInt32 retCode;
		UInt64 attackID;
		UInt64 targetID;
		UInt64 itemGuid;
		UInt32 itemDataID;
		std::string targetName;
		UInt32 param;
	};

	/**
	*@brief world->battle ����Լ����а�
	*/
	class BattleSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_SORT_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & start & num;
		}

		//���ID
		ObjID_t playerId;
		//���а�����
		UInt8 type;
		//��ʼλ�� 0��ʼ
		UInt16 start;
		//����
		UInt16 num;
	};

	/**
	*@brief battle->scene ս��
	*/
	class SceneBattleEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_END)
	public:
		SceneBattleEnd()
		{
			playerID = 0;
			score = 0;
			getHonor = 0;
		}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerID & score & getHonor;
		}

	public:
		UInt64 playerID;
		UInt32 score;
		UInt32 getHonor;
	};

	/**
	*@brief client->scene ɾ����Ʒ����
	*/
	class SceneBattleDelItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_DEL_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemGuid;
		}

	public:
		UInt64 itemGuid;
	};

	/**
	*@brief scene->client ɾ����Ʒ����
	*/
	class SceneBattleDelItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_DEL_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

	public:
		UInt32 retCode;
	};

	/**
	*@brief client->scene ����Լ�ս��
	*/
	class SceneBattleEnterBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_ENTER_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID;
		}

	public:
		UInt32 battleID;
	};

	class SceneBattleEnterBattleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_ENTER_BATTLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & retCode;
		}

	public:
		UInt32 battleID;
		UInt32 retCode;
	};

	/**
	*@brief battle->scene �Լ�������־
	*/
	class SceneBattleBalanceLog : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_BALANCE_LOG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rank & playerNum & kills & survivalTime & score & reason & roleId;
		}

	public:
		UInt32 rank;
		UInt32 playerNum;
		UInt32 kills;
		UInt32 survivalTime;
		UInt32 score;
		UInt32 reason;
		UInt64 roleId;
	};

	struct BattleNpc
	{
		BattleNpc() : npcGuid(0), npcId(0), opType(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & npcGuid & npcId & opType & scenePos;
		}

		UInt64 npcGuid;
		// npcId
		UInt32 npcId;
		// 1ˢ����0���
		UInt32 opType;
		// λ��
		ScenePos scenePos;
	};

	/**
	*@brief scene->client ˢ��NPC
	*/
	class SceneBattleNpcNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NPC_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleNpcList;
		}

	public:

		std::vector<BattleNpc> battleNpcList;
	};

	/**
	*@brief client->scene ��NPC��������
	*/
	class SceneBattleNpcTradeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NPC_TRADE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & npcGuid & npcId & paramVec;
		}

	public:
		UInt64 npcGuid;
		UInt32 npcId;
		std::vector<UInt64> paramVec;
	};

	/**
	*@brief scene->client ��NPC���׷���
	*/
	class SceneBattleNpcTradeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NPC_TRADE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & npcId;
		}

	public:
		UInt32 retCode;
		UInt32 npcId;
	};

	/**
	*@brief client->scene �Լ�֪ͨ��������
	*/
	class SceneBattleNotifyBeTraped : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_BE_TRAPED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & playerID & ownerID & x & y;
		}

	public:
		UInt32 battleID;
		UInt64 playerID;
		UInt64 ownerID;
		UInt32 x;
		UInt32 y;
	};

	/**
	*@brief client->scene �Լ�������������
	*/
	class SceneBattlePlaceTrapsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PLACE_TRAPS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemGuid & itemCount & x & y;
		}

	public:
		UInt64 itemGuid;
		UInt32 itemCount;
		UInt32 x;
		UInt32 y;
	};

	/**
	*@brief scene->client �Լ��������巵��
	*/
	class SceneBattlePlaceTrapsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_PLACE_TRAPS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

	public:
		UInt32 retCode;
	};

	/**
	*@brief client->scene �Լ�����������
	*/
	class SceneBattleOpenBoxReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_OPEN_BOX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemGuid & param;
		}

	public:
		UInt64 itemGuid;
		// ���� 1���򿪣�2��ȡ����3��ʰȡ
		UInt32 param;
	};

	/**
	*@brief scene->client �Լ������䷵��
	*/
	class SceneBattleOpenBoxRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_OPEN_BOX_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemGuid & param & retCode & openTime;
		}

	public:
		UInt64 itemGuid;
		UInt32 param;
		UInt32 retCode;
		UInt32 openTime;
	};

	/**
	*@brief battle->client �������֪ͨ
	*/
	class BattleNotifyBestRank : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_BEST_RANK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rank;
		}

	public:
		UInt32 rank;
	};

	/**
	*@brief battle->scene �����������֪ͨ
	*/
	class BattleNotifyRankAward : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NOTIFY_RANK_AWARD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & rank;
		}

	public:
		UInt64 playerId;
		UInt32 rank;
	};

	/**
	*@brief world->battle �Լ�֪ͨ�ʱ��
	*/
	class BattleNotifyActivityInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_NOTIFY_ACTIVITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & actName & preTime & startTime & endTime;
		}

	public:
		std::string actName;
		UInt32 preTime;
		UInt32 startTime;
		UInt32 endTime;
	};

	struct ChiJiSkill
	{
		ChiJiSkill() : skillId(0), skillLvl(0){}
		ChiJiSkill(UInt32 _id, UInt32 _lvl) : skillId(_id), skillLvl(_lvl) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & skillId & skillLvl;
		}
		UInt32 skillId;	
		UInt32 skillLvl;
	};

	/**
	*@brief scene->client �·�����ѡ���б�
	*/
	class BattleSkillChoiceListNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_SKILL_CHOICE_LIST_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & chiJiSkillVec;
		}

	public:
		std::vector<ChiJiSkill> chiJiSkillVec;
	};

	/**
	*@brief client->scene ѡ��������
	*/
	class BattleChoiceSkillReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CHOICE_SKILL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & skillId;
		}

	public:
		UInt32 skillId;
	};

	/**
	*@brief scene->client ѡ���ܷ���
	*/
	class BattleChoiceSkillRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CHOICE_SKILL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & skillId & skillLvl;
		}

	public:

		UInt32 retCode;
		UInt32 skillId;
		UInt32 skillLvl;
	};

	/**
	*@brief client->battle �Լ���ս����
	*/
	class BattleObserveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_OBSERVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & dstId & battleId;
		}

	public:
		UInt64 playerId;
		UInt64 dstId;
		UInt32 battleId;
	};

	/**
	*@brief battle->client �Լ���ս����
	*/
	class BattleObserveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_OBSERVE_RES)
	public:

		BattleObserveRes()
		{
			retCode = 0;
			playerId = 0;
			dstId = 0;
			raceId = 0;
		}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & playerId & dstId & raceId & addr;
		}

	public:
		UInt32 retCode;
		UInt64 playerId;
		UInt64 dstId;
		UInt64 raceId;
		Avalon::SockAddr addr;
	};

	/**
	*@brief battle->scene �Լ���ս���
	*/
	class BattleCheckNoWarReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_CHECK_NO_WAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dstId & battleID & dungeonID;
		}

		ObjID_t roleId;
		ObjID_t dstId;
		UInt32 battleID;
		UInt32 dungeonID;
	};

	/**
	*@brief scene->battle �Լ���ս��鷵��
	*/
	class BattleCheckNoWarRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_CHECK_NO_WAR_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isNoWar & roleId & dstId & battleID & dungeonID;
		}

		UInt32 isNoWar;
		UInt64 roleId;
		UInt64 dstId;
		UInt32 battleID;
		UInt32 dungeonID;
	};

	/**
	*@brief battle->client ֪ͨ�Լ���������
	*/
	class BattleNotifyChijiMisc : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_NOTIFY_CHIJI_MISC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & moveIntervalMs;
		}

		// �ƶ������ͼ��(ms)
		UInt32 moveIntervalMs;
	};

	/**
	*@brief scene->client ��սѡ��
	*/
	class SceneBattleNoWarOption : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NO_WAR_OPTION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & enemyRoleId & enemyName;
		}

		UInt64 enemyRoleId;
		std::string enemyName;
	};

	/**
	*@brief client->scene ��սѡ������
	*/
	class SceneBattleNoWarChoiceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NO_WAR_CHOICE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isNoWar;
		}
		 
		UInt32 isNoWar;
	};

	/**
	*@brief scene->client ��սѡ�񷵻�
	*/
	class SceneBattleNoWarChoiceRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NO_WAR_CHOICE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}
		UInt32 retCode;
	};

	/**
	*@brief scene->client ��ս֪ͨ
	*/
	class SceneBattleNoWarNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NO_WAR_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief scene->client ��ս�ȴ�֪ͨ
	*/
	class SceneBattleNoWarWait : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_NO_WAR_WAIT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief client->scene ְҵ�б�����
	*/
	class SceneBattleOccuListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_OCCU_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client ְҵ�б���
	*/
	class SceneBattleOccuListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_OCCU_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & occuVec;
		}

		std::vector<UInt32> occuVec;
	};

	/**
	*@brief battle->bscene �Լ��̵�֪ͨ
	*/
	class BattleShopNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_SHOP_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleId & shopWave;
		}

		UInt32 battleId;
		UInt32 shopWave;
	};
	
	/**
	*@brief scene->client �·�װ��ѡ���б�
	*/
	class BattleEquipChoiceListNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_EQUIP_CHOICE_LIST_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipVec;
		}

	public:
		std::vector<UInt32> equipVec;
	};

	/**
	*@brief client->scene ѡ��װ������
	*/
	class BattleChoiceEquipReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CHOICE_EQUIP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipId;
		}

	public:
		UInt32 equipId;
	};

	/**
	*@brief scene->client ѡ��װ������
	*/
	class BattleChoiceEquipRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BATTLE_CHOICE_EQUIP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & equipId;
		}

	public:

		UInt32 retCode;
		UInt32 equipId;
	};

	/**
	*@brief battle->scene �Լ�����֪ͨ
	*/
	class BattleOpenNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_OPEN_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & openTime;
		}

	public:

		UInt32 openTime;
	};

}

#endif
