#ifndef _CL_LOST_DUNGEON_PROTOCOL_H_
#define _CL_LOST_DUNGEON_PROTOCOL_H_

#include <AvalonSocket.h>
#include <CLProtocolDefine.h>
#include <CLLostDungeonDefine.h>
#include <CLLostDungeonDefine.h>
#include <CLMatchDefine.h>
#include <CLObjectDefine.h>

namespace CLProtocol
{
	/**
	*@brief client->scene ��ս��ʧ��������
	*/
	class LostDungeonChallengeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_CHAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & floor & hardType;
		}

		//����
		UInt32  floor;
		//ս���Ѷ�����(LostDungeonBattleHardType)
		UInt8	hardType;
	};

	/**
	*@brief battle->client ��ս��ʧ���η���
	*/
	class LostDungeonChallengeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_CHAGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & battleId & battleDataId & sceneId & floor & hardType;
		}

		//���
		UInt32  code;
		//ƥ���ս��id
		UInt32  battleId;
		//ƥ���ս���ı�id
		UInt32  battleDataId;
		//ƥ���ս���ĳ���id
		UInt32	sceneId;
		//¥��
		UInt32	floor;
		//ս���Ѷ�����(LostDungeonBattleHardType)
		UInt8	hardType;
	};

	/**
	*brief scene->battle ƥ��ս��
	*/
	class LostDungeonMathBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_MATH_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & srcSceneNodeId & floor & hardType;
		}

		//ƥ������
		UInt64	playerId;
		//Դ����id
		UInt32  srcSceneNodeId;
		//����
		UInt32  floor;
		//ƥ���ս���Ѷ�(ö��LostDungeonBattleHardType)
		UInt8	hardType;
	};

	/**
	*@brief client->scene ������ʧ��������
	*/
	class LostDungeonOpenReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_OPEN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	};

	/**
	*@brief scene->client ������ʧ���η���
	*/
	class LostDungeonOpenRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_OPEN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & floorDatas;
		}

		//���
		UInt32  code;
		//���ɵ�¥������
		std::vector<LostDungeonFloorData>	floorDatas;
	};

	/**
	*@brief client->scene ���ο��������� 
	*/
	class LostDungeonOpenBoxReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_OPENBOX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream  & floor & boxId;
		}
		UInt32	floor;
		//����id
		UInt32  boxId;
	};

	/**
	*@brief scene->client ���ο����䷵��
	*/
	class LostDungeonOpenBoxRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_OPENBOX_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & itemVec;
		}
		//���
		UInt32  code;
		//��ѡ����
		std::vector<ItemReward> itemVec;
	};

	/**
	*@brief scene->client ͬ��¥������ 
	*/
	class LostDungeonSyncFloor : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SYNC_FLOOR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & floorDatas;
		}
		//¥������
		std::vector<LostDungeonFloorData>	floorDatas;
	};

	/**
	*@brief client->battleScene ��ʧ����ս��PK����
	*/
	class SceneLostDungeonPkReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_LOSTDUNGEON_PK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dstId & battleID & dungeonID;
		}
		ObjID_t dstId;
		UInt32 battleID;
		UInt32 dungeonID;
	};

	/**
	*@brief battle->client ս��PK����
	*/
	class SceneLostDungeonPkRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_LOSTDUNGEON_PK_RES)
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
	*@brief scene->battle ����PK
	*/
	class BattleLostDungeonPkReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_PVP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & attacker & attackeder & dungeonId & battleId;
		}

		RacePlayerInfo attacker;
		RacePlayerInfo attackeder;
		UInt32	dungeonId;
		UInt32  battleId;
	};

	/**
	*@brief client->scene �����б�����
	*/
	class SceneLostDungeonTaskListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_TASK_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & grade;
		}

		UInt32 grade;
	};

	/**
	*@brief scene->client �����б���
	*/
	class SceneLostDungeonTaskListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_TASK_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskVec;
		}

		std::vector<UInt32> taskVec;
	};

	/**
	*@brief client->scene ѡ����������
	*/
	class SceneLostDungeonChoiceTaskReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_CHOICE_TASK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

		UInt32 taskId;
	};

	/**
	*@brief scene->client ѡ�����񷵻�
	*/
	class SceneLostDungeonChoiceTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_CHOICE_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & taskId;
		}

		UInt32 retCode;
		UInt32 taskId;
	};

	/**
	*@brief scene->client ��������֪ͨ
	*/
	class SceneLostDungeonTaskDataNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_TASK_DATA_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & status & vars;
		}

		UInt32 taskId;
		UInt32 status;
		std::map<std::string, std::string> vars;	//�������
	};

	/**
	*@brief scene->client ������֪ͨ
	*/
	class SceneLostDungeonTaskResultNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_TASK_RESULT_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & result;
		}

		UInt32 taskId;
		UInt32 result;
	};
		
	/**
	*@brief client->scene ѡ����³���սģʽ����
	*/
	class SceneLostDungeonSwitchChageModeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SWITCH_DUNGE_CHAGEMODE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & chageMode;
		}

		UInt8 chageMode;
	};

	/**
	*@brief scene->client ѡ����³���սģʽ����
	*/
	class SceneLostDungeonSwitchChageModeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SWITCH_DUNGE_CHAGEMODE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32 code;
	};


	/**
	*@brief scene->client ͬ�����³������Ϣ
	*/
	class SceneLostDungeonSyncDungeonTeamInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SYNC_DUNGEON_TEAM_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & challengeMode & teamMemebers & addMember & leavePlayerId & teamInfos;
		}
	
		SceneLostDungeonSyncDungeonTeamInfo()
		{
			type = 0;
			challengeMode = 0;
			leavePlayerId = 0;
		}
		
		UInt8   type; //SyncDungeonEnterTeamInfoType
		UInt8	challengeMode;
		std::vector<LostDungTeamMember> teamMemebers;
		LostDungTeamMember addMember;
		UInt64	leavePlayerId;
		//����״̬
		std::vector<LostDungTeamInfo> teamInfos;
	};

	/**
	*@brief battle->client ֪ͨ��ʼ���³�ͶƱ
	*/
	class BattleTeamRaceVoteNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_BATTLE_TEAM_RACE_VOTE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & teamId;
		}

	public:
		// ���³�ID
		UInt32			dungeonId;
		UInt32			teamId;
	};

	/**
	*@brief client->battle  ����ϱ�ͶƱѡ��
	*/
	class BattleTeamReportVoteChoice : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_TEAM_REPORT_VOTE_CHOICE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & agree;
		}

	public:
		// ���id
		UInt64  playerId;
		// �Ƿ�ͬ��
		UInt8	agree;
	};

	/**
	*@brief battle->client �㲥������ͶƱѡ��
	*/
	class BattleTeamVoteChoiceNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_TEAM_VOTE_CHOICE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & agree;
		}

	public:
		// ��ɫID
		ObjID_t	roleId;
		// �Ƿ�ͬ��
		UInt8	agree;
	};

	/**
	*@breif scene->battle ����뿪ս��
	*/
	class BattleExitNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_EXIT_NOTIFY)

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & battleId & battleType & battleResult & score & btState;
		}
	public:
		// ��ɫID
		ObjID_t	roleId;
		// ս��id
		UInt32	battleId;
		// ս������
		UInt8	battleType;
		// ���(LostDungeonBattleReasult)
		UInt8	battleResult;
		// ��������
		UInt32	score;
		// �뿪ʱ��ս��״̬
		UInt8	btState;
	};

	/**
	*@brief scene->client ��Դ�б�
	*/
	class SceneLostDungeonSyncResourcesList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SYNC_RES_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & infoes;
		}

		UInt32 battleID;
		std::vector<SceneItemInfo>		infoes;
	};

	/**
	*@brief scene->client ������Դ
	*/
	class  SceneLostDungeonResourceAdd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SYNC_RES_ADD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & data;
		}

		void AddItem(UInt32 sceneId, const SceneItem& item)
		{
			for (auto& info : data)
			{
				if (info.sceneId == sceneId)
				{
					info.items.push_back(item);
					return;
				}
			}

			SceneItemInfo info;
			info.sceneId = sceneId;
			info.items.push_back(item);
			data.push_back(info);
		}

		UInt32 battleID;
		std::vector<SceneItemInfo>		data;
	};

	/**
	*@brief scene->client ɾ����Դ
	*/
	class SceneLostDungeonResourceDel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SYNC_RES_DEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & guids;
		}

		UInt32 battleID;
		std::vector<ObjID_t>		guids;
	};

	/**
	*@brief client->server ����ѡ���������
	*/
	class SceneLostDungeonBoxChoiceItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_BOX_CHOICE_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId;
		}

		UInt32 itemId;
	};

	/**
	*@brief server->client ����ѡ����߷���
	*/
	class SceneLostDungeonBoxChoiceItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_BOX_CHOICE_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & item;
		}

		UInt32 retCode;
		ItemReward item;
	};

	/**
	*@brief battle->scene ����������һ��
	*/
	class SceneLostDungeonSettleFloor : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SETTLE_FLOOR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & floor & battleResult & score & btState;
		}

		// ��ɫID
		ObjID_t	roleId;
		// ¥��
		UInt32	floor;
		// ���(LostDungeonBattleReasult)
		UInt8	battleResult;
		// ��������
		UInt32	score;
		// ս��״̬
		UInt8   btState;
	};

	/**
	*@brief scene->client ֪ͨ�ͻ�������¥��ͨ��
	*/
	class SceneLostDungeonSettleFlooorNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_SETTLE_FLOOR_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & floor & battleResult & addScore & decScore & score & againItemId;
		}
		// ¥��
		UInt32	floor;
		// ���(LostDungeonBattleReasult)
		UInt8	battleResult;
		// ��û���
		UInt32	addScore;
		// �۳�����
		UInt32	decScore;
		// ��ǰ����
		UInt32  score;
		// ʧ�ܿ���������ս����id
		UInt32 againItemId;
	};

	/**
	*@brief client->scene ������ȡ����
	*/
	class SceneLostDungeonGetRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_GET_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client ������ȡ��������
	*/
	class SceneLostDungeonGetRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_GET_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32 code;
	};

	/**
	*@brief client->scene ������������
	*/
	class SceneLostDungeonGiveUpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_GIVEUP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->scene ������������
	*/
	class SceneLostDungeonGiveUpRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_GIVEUP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32 code;
	};

	/**
	*@brief scene->scene �˳�ս����Ҫͬ��������
	*/
	class BattleLostDungSyncDataToSelfScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_LOSTDUNG_SYNCDATA_TOSELFSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & lostDungeonHp & lostDungeonMp;
		}

		// ��ɫID
		ObjID_t	roleId;
		// ����ʣ��Ѫ��(ǧ�ֱ�)
		Int32   lostDungeonHp;
		// ����ʣ������(ǧ�ֱ�)
		Int32   lostDungeonMp;
	};

	/**
	*@brief scene->client ͬ��ս������
	*/
	class BattleLostDungSyncBattleData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_LOSTDUNGEON_SYNC_BATTLEDATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleId & playerNum;
		}
		//ս��id
		UInt32	battleId;
		// ս������
		ObjID_t	playerNum;
	};

	/**
	*@brief client->battleScene  �鿴����鱨
	*/
	class BattleLostDungSeeIntellReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_LOSTDUNGEON_SEE_INTELL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId;
		}
		//���id
		UInt64	playerId;
	};


	/**
	*@brief battleScene->client  �鿴����鱨����
	*/
	class BattleLostDungSeeIntellRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_LOSTDUNGEON_SEE_INTELL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & playerId & artifacts;
		}
		UInt32 code;
		//���id
		UInt64	playerId;
		//����
		std::vector<ItemReward>  artifacts;
	};

	/**
	*@brief battleScene->client  �����Ƿ��ڵ��³�
	*/
	class SceneLostDungeonEnemyInDungeon : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LOSTDUNGEON_ENEMY_IN_DUNGEON)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & enemyId & dungeonId;
		}

		// �з�id
		UInt64 enemyId;
		// ���³�id
		UInt32 dungeonId;
	};

	/**
	*@brief battle->scene ͬ�����ζ���״̬
	*/
	class BattleSynLostDungTeamSt : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_LOSTDUNG_SYN_TEAM_ST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleId & teamId & state & isEnd;
		}
		//ս��id
		UInt32	battleId;
		//����id
		UInt32	teamId;
		//����״̬
		UInt8	state;
		//�Ƿ��������
		UInt8   isEnd;
	};

	/**
	*@brief scene->battle ս�����ٷ���
	*/
	class BattleDestoryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_DESTROY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleType & battleId;
		}
		//ս������
		UInt8   battleType;
		//ս��id
		UInt32	battleId;
		
	};

	/**
	*@brief  battleScene->client  ֪ͨ�ͻ����뿪ս��
	*/
	class SceneNotifyClientLeaveBattle : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_CLIENT_LEAVE_BATTLE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	
	};

	/**
	*@brief client->scene ��ѯ��ɱĿ����������
	*/
	class SceneQueryKillTargetPosReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_KILL_TAEGET_POS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerIds;
		}
		//���
		std::vector<UInt64>  playerIds;
	};

	/**
	*@brief scene->client ��ѯ��ɱĿ�����귵��
	*/
	class SceneQueryKillTargetPosRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_KILL_TAEGET_POS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerPos;
		}
		std::vector<LostDungeonPlayerPos> playerPos;
	};

	/**
	*@brief node->battle ��ѯһ�������ϸ��Ϣ
	*/
	class BattleQuerylayerDetailOne : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_QUERY_PLAYER_DETAIL_ONE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & srcRoleId & targetRoleId & queryType;
		}

		UInt64 srcRoleId;
		UInt64 targetRoleId;
		UInt32 queryType;
	};

	/**
	*@brief battle->scene ֪ͨһ�ζ�����³Ǳ�������
	*/
	class BattleNotifyTeamRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_NOTIFY_TEAM_RACE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream  & teamId & reason ;
		}
		//����id
		UInt32	teamId;
		//ԭ��
		UInt8	reason;
	
	};

	/**
	*@brief scene->battle ͬ�����ս������
	*/
	class BattleSyncRacePlayerInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(BATTLE_SYNC_RACE_PLAYER_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream  & racePlayerInfo;
		}
		//���ս������
		RacePlayerInfo	racePlayerInfo;

	};

};

#endif