#ifndef _CL_TEAM_COPY_PROTOCOL_H_
#define _CL_TEAM_COPY_PROTOCOL_H_

#include <CLMatchDefine.h>

#include "CLProtocolDefine.h"
#include "CLTeamCopyDefine.h"

namespace CLProtocol
{
	/**
	*@brief scene->teamcopy ��������ű�����
	*/
	class TeamCopyEnterSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_ENTER_SCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief scene->teamcopy �����˳��ű�����
	*/
	class TeamCopyExitSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_EXIT_SCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t roleId;
	};

	/**
	*@brief client->teamcopy �����Ŷ�����
	*/
	class TeamCopyCreateTeamReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_CRETAE_TEAM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamType & teamGrade & teamModel & equipScore & param;
		}

		// �ű�����
		UInt32 teamType;
		// �Ѷ�
		UInt32 teamGrade;
		// ģʽ
		UInt32 teamModel;
		// װ������
		UInt32 equipScore;
		// param(Ӷ��)
		UInt32 param;
	};

	/**
	*@brief teamcopy->client �����Ŷӷ���
	*/
	class TeamCopyCreateTeamRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_CRETAE_TEAM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & retCode;
		}

		// �Ŷ�Id
		UInt32 teamId;
		UInt32 retCode;
	};

	/**
	*@brief client->teamcopy �Ŷ���������
	*/
	class TeamCopyTeamDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_DATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & teamType;
		}

		// �Ŷ�Id
		UInt32 teamId;
		// �ű�����
		UInt32 teamType;
	};

	/**
	*@brief teamcopy->client �Ŷ����ݷ���
	*/
	class TeamCopyTeamDataRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_DATA_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & equipScore & status & teamName & totalCommission & bonusCommission 
				& autoAgreeGold & teamType & teamModel & teamGrade & voiceRoomId & squadList;
		}
		TeamCopyTeamDataRes() : teamId(0) {} 
		// ����ID
		UInt32 teamId;
		// װ������
		UInt32 equipScore;
		// �Ŷ�״̬
		UInt32 status;
		// �Ŷ�����
		std::string teamName;
		// Ӷ������
		UInt32 totalCommission;
		// �ֳ�Ӷ��
		UInt32 bonusCommission;
		// �Զ�ͬ���������(0����ͬ�⣬1��ͬ��)
		UInt32 autoAgreeGold;
		// �ű�����
		UInt32 teamType;
		// �ű�ģʽ
		UInt32 teamModel;
		// �Ѷ�
		UInt32 teamGrade;
		// ��������
		std::string voiceRoomId;
		// С���б�
		std::vector<TeamCopySquad> squadList;
	};

	/**
	*@brief client->teamcopy �Ŷ��б�����
	*/
	class TeamCopyTeamListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamType & teamModel & pageNum;
		}

		// �ű�����
		UInt32 teamType;
		// ģʽ
		UInt32 teamModel;
		// ����ҳ	
		UInt32 pageNum;
	};

	/**
	*@brief �Ŷ��������
	*/
	struct TeamCopyTeamProperty
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & teamModel & commission & teamName & teamType & teamGrade & memberNum & equipScore & status;
		}

		// id
		UInt32 teamId;
		// ģʽ
		UInt32 teamModel;
		// Ӷ��
		UInt32 commission;
		// ����
		std::string teamName;
		// �ű�����
		UInt32 teamType;
		// �Ѷ�
		UInt32 teamGrade;
		// ����
		UInt32 memberNum;
		// װ������
		UInt32 equipScore;
		// ״̬
		UInt32 status;
	};

	/**
	*@brief teamcopy->client �Ŷ��б���
	*/
	class TeamCopyTeamListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & curPage & totalPageNum & teamPropertyList;
		}

		// ��ǰҳ
		UInt32 curPage;
		// ��ҳ��
		UInt32 totalPageNum;
		// �Ŷ��б�
		std::vector<TeamCopyTeamProperty> teamPropertyList;
	};

	/**
	*@brief client->teamcopy ��������
	*/
	class TeamCopyTeamApplyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_APPLY_REQ)
	public:

		TeamCopyTeamApplyReq() : teamId(0), isGold(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & isGold;
		}

		// �Ŷ�ID
		UInt32 teamId;
		// �Ƿ����(��0�ǽ���)
		UInt32 isGold;
	};
	
	/**
	*@brief teamcopy->client ���ŷ���
	*/
	class TeamCopyTeamApplyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_APPLY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & teamId & expireTime;
		}

		UInt32 retCode;
		UInt32 teamId;
		UInt64 expireTime;
	};

	/**
	*@brief client->teamcopy �˳��Ŷ�����
	*/
	class TeamCopyTeamQuitReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_QUIT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
	
	/**
	*@brief teamcopy->client �˳��Ŷӷ���
	*/
	class TeamCopyTeamQuitRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_QUIT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief client->teamcopy ��ս����
	*/
	class TeamCopyStartBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_START_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & planModel & battlePlanList;
		}

		UInt32 planModel;
		std::vector<TeamCopyBattlePlan> battlePlanList;
	};

	/**
	*@brief teamcopy->client ��ս����
	*/
	class TeamCopyStartBattleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_START_BATTLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleName & retCode;
		}

		std::string roleName;
		UInt32 retCode;
	};

	/**
	*@brief teamcopy->client ��ս֪ͨ
	*/
	class TeamCopyStartBattleNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_START_BATTLE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & voteDurationTime & voteEndTime;
		}

		// ͶƱ����ʱ��
		UInt32 voteDurationTime;
		// ͶƱ��ֹʱ��
		UInt32 voteEndTime;
	};

	/**
	*@brief client->teamcopy ��սͶƱ
	*/
	class TeamCopyStartBattleVote : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_START_BATTLE_VOTE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & vote;
		}

		// ��0ͬ�⣬0�ܾ�
		UInt32 vote;
	};

	/**
	*@brief teamcopy->client ͶƱ֪ͨ
	*/
	class TeamCopyVoteNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_VOTE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & vote;
		}

		UInt64 roleId;
		// ��0ͬ�⣬0�ܾ�
		UInt32 vote;
	};

	/**
	*@brief teamcopy->client ͶƱ���
	*/
	class TeamCopyVoteFinish : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_VOTE_FINISH)
	public:

		TeamCopyVoteFinish() : result(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & notVotePlayer;
		}

		// ���(0����ɹ�����0����ʧ��)
		UInt32 result;
		// δͶƱ���
		std::vector<std::string> notVotePlayer;
	};
	
	/**
	*@brief teamcopy->client �׶�֪ͨ
	*/
	class TeamCopyStageNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_STAGE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & stageId & gameOverTime & squadTarget & teamTarget & fieldList;
		}
		
		// �׶�id
		UInt32 stageId;
		// ��Ϸ����ʱ��
		UInt32 gameOverTime;
		// С��Ŀ��
		TeamCopyTarget squadTarget;
		// �Ŷ�Ŀ��
		TeamCopyTarget teamTarget;
		// �ݵ��б�
		std::vector<TeamCopyField> fieldList;
	};

	/**
	*@brief teamcopy->client �׶ν���
	*/
	class TeamCopyStageEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_STAGE_END)
	public:
		TeamCopyStageEnd() : stageId(0), passType(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & stageId & passType;
		}

		// �׶�id
		UInt32 stageId;
		// ͨ�����ͣ��ű���ʹ�ã�
		UInt32 passType;
	};

	/**
	*@brief teamcopy->client �ݵ�֪ͨ
	*/
	class TeamCopyFieldNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FIELD_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fieldList;
		}

		// �ݵ��б�
		std::vector<TeamCopyField> fieldList;
	};
	
	/**
	*@brief teamcopy->client Ŀ��֪ͨ
	*/
	class TeamCopyTargetNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TARGET_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & squadTarget & teamTarget;
		}

		// С��Ŀ��
		TeamCopyTarget squadTarget;
		// �Ŷ�Ŀ��
		TeamCopyTarget teamTarget;
	};

	/**
	*@brief teamcopy->client С��״̬֪ͨ
	*/
	class TeamCopySquadStatusNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_SQUAD_STATUS_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & squadId & squadStatus;
		}

		// С��id
		UInt32 squadId;
		// С��״̬
		UInt32 squadStatus;
	};

	/**
	*@brief teamcopy->client �������
	*/
	class TeamCopyTeamUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opType & teamId;
		}

		// 1.��ɢ
		UInt32 opType;
		// ����ID
		UInt32 teamId;
	};

	/**
	*@brief client->teamcopy �����б�����
	*/
	class TeamCopyTeamApplyListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_APPLY_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief teamcopy->client �����б���
	*/
	class TeamCopyTeamApplyListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_APPLY_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & applyList;
		}

		std::vector<TeamCopyApplyProperty> applyList;
	};

	/**
	*@brief client->teamcopy ��������
	*/
	class TeamCopyTeamApplyReplyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_APPLY_REPLY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isAgree & playerIds;
		}

		// ��0ͬ�⣬0�ܾ�
		UInt32 isAgree;
		std::vector<UInt64> playerIds;
	};

	/**
	*@brief teamcopy->client 
	*/
	class TeamCopyTeamApplyReplyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_APPLY_REPLY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & isAgree & playerIds;
		}

		UInt32 retCode;
		// ��0ͬ�⣬0�ܾ�
		UInt32 isAgree;
		std::vector<UInt64> playerIds;
	};

	/**
	*@brief client->teamcopy ��ʼ��ս
	*/
	class TeamCopyStartChallengeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_START_CHALLENGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fieldId;
		}

		UInt32 fieldId;
	};

	/**
	*@brief teamcopy->client 
	*/
	class TeamCopyStartChallengeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_START_CHALLENGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief teamcopy->scene ��ѯ������Ϣ
	*/
	class TeamCopyQueryPlayerInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_QUERY_PLAYER_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamSquadId & roleId & dungeonId;
		}

		// ����С��Id
		UInt32 teamSquadId;
		// ��ɫId
		UInt64 roleId;
		// ���³�Id
		UInt32 dungeonId;
	};
	
	/**
	*@brief scene->teamcopy ��ѯ������Ϣ����
	*/
	class TeamCopyQueryPlayerInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_QUERY_PLAYER_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamSquadId & roleId & result & dungeonPlayerInfo;
		}

		// ����С��Id
		UInt32 teamSquadId;
		// ��ɫId
		UInt64 roleId;
		// ���
		UInt32 result;
		// ��Ϣ
		DungeonRacePlayerInfo dungeonPlayerInfo;
	};

	/**
	*@brief scene->teamcopy ����������ű�
	*/
	class TeamCopyPlayerReconnect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PLAYER_RECONNECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & isSendData;
		}

		// ��ɫId
		UInt64 roleId;
		// �Ƿ��·�����
		UInt32 isSendData;
	};

	/**
	*@brief teamcopy->scene �ű�������
	*/
	class TeamCopyPlayerCheckReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PLAYER_CHECK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & roleId & teamGrade & teamMode & forzenCommission & consumeItemVec;
		}

		// ����id
		UInt32 teamId;
		// ��ɫId
		UInt64 roleId;
		// �����Ѷ�
		UInt32 teamGrade;
		// �Ƿ����
		UInt32 teamMode;
		// ����Ӷ��
		TCConsumeItem forzenCommission;
		// �۳��ĵ��߲���
		std::vector<TCConsumeItem> consumeItemVec;
	};
	
	/**
	*@brief scene->teamcopy
	*/
	class TeamCopyPlayerCheckRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PLAYER_CHECK_RES)
	public:
		TeamCopyPlayerCheckRes() : retCode(0), teamId(0), roleId(0), commissionOrder(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & teamId & roleId & commissionOrder & forzenCommission & consumeItemVec;
		}

		// ������
		UInt32 retCode;
		// ����id
		UInt32 teamId;
		// ��ɫId
		UInt64 roleId;
		// Ӷ�𶩵���
		UInt64 commissionOrder;
		// ����Ӷ��
		TCConsumeItem forzenCommission;
		// �۳��ĵ��߲���
		std::vector<TCConsumeItem> consumeItemVec;
	};

	/**
	*@brief client->teamcopy �Ҷ���
	*/
	class TeamCopyFindTeamMateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FIND_TEAM_MATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief teamcopy->client 
	*/
	class TeamCopyFindTeamMateRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FIND_TEAM_MATE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerList;
		}

		// ���
		std::vector<TeamCopyApplyProperty> playerList;
	};

	/**
	*@brief client->teamcopy ����
	*/
	class TeamCopyInvitePlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_INVITE_PLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & inviteList;
		}

		// �����б�
		std::vector<UInt64> inviteList;
	};

	/**
	*@brief teamcopy->client ����֪ͨ
	*/
	class TeamCopyInviteNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_INVITE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->teamcopy ����
	*/
	class TeamCopyStageFlopReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_STAGE_FLOP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & stageId;
		}

		// �׶�id
		UInt32 stageId;
	};

	/**
	*@brief teamcopy->client ����
	*/
	class TeamCopyStageFlopRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_STAGE_FLOP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & stageId & flopList;
		}

		// �׶�id
		UInt32 stageId;
		// ������Ϣ
		std::vector<TeamCopyFlop> flopList;
	};

	/**
	*@brief teamcopy->client �ű�����֪ͨ
	*/
	class TeamCopyReconnectNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_RECONNECT_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sceneId & expireTime;
		}

		UInt32 sceneId;
		UInt64 expireTime;
	};

	/**
	*@brief client->teamcopy ����λ
	*/
	class TeamCopyChangeSeatReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_CHANGE_SEAT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & srcSeat & destSeat;
		}

		UInt32 srcSeat;
		UInt32 destSeat;
	};
	
	/**
	*@brief teamcopy->client 
	*/
	class TeamCopyChangeSeatRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_CHANGE_SEAT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};


	/**
	*@brief client->teamcopy ��������
	*/
	class TeamCopyTeamDetailReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_DETAIL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId;
		}

		// �Ŷ�Id
		UInt32 teamId;
	};

	/**
	*@brief teamcopy->client
	*/
	class TeamCopyTeamDetailRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_DETAIL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & teamType & teamModel & teamName & totalCommission & bonusCommission & squadList;
		}

		// ����ID
		UInt32 teamId;
		// �ű�����
		UInt32 teamType;
		// ����ģʽ
		UInt32 teamModel;
		// �Ŷ�����
		std::string teamName;
		// Ӷ������
		UInt32 totalCommission;
		// �ֳ�Ӷ��
		UInt32 bonusCommission;
		// С���б�
		std::vector<TeamCopySquad> squadList;
	};

	/**
	*@brief client->teamcopy �߳�����
	*/
	class TeamCopyKickReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_KICK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId;
		}

		UInt64 playerId;
	};
	
	/**
	*@brief teamcopy->client �߳�����
	*/
	class TeamCopyKickRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_KICK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief client->teamcopy ����
	*/
	class TeamCopyAppointmentReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_APPOINTMENT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & post;
		}

		UInt64 playerId;
		UInt32 post;
	};

	/**
	*@brief teamcopy->client ����
	*/
	class TeamCopyAppointmentRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_APPOINTMENT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief teamcopy->client ��Ա֪ͨ
	*/
	class TeamCopyMemberNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_MEMBER_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & memberName & flag;
		}

		// ��Ա����
		std::string memberName;
		// ��־(0�����룬1���뿪)
		UInt32 flag;
	};
	
	/**
	*@brief client->teamcopy �Զ�ͬ�����
	*/
	class TeamCopyAutoAgreeGoldReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_AUTO_AGREE_GOLD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isAutoAgree;
		}

		// (0����ͬ�⣬1��ͬ��)
		UInt32 isAutoAgree;
	};
	
	/**
	*@brief teamcopy->client 
	*/
	class TeamCopyAutoAgreeGoldRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_AUTO_AGREE_GOLD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & isAutoAgree;
		}

		UInt32 retCode;
		// (0����ͬ�⣬1��ͬ��)
		UInt32 isAutoAgree;
	};

	/**
	*@brief teamcopy->client ���³Ǹ���
	*/
	class TeamCopyDungeonRevive : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_DUNGEON_REVIVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & targetId & reviveId & reviveCoinNum;
		}

		// ���id
		UInt64 playerId;
		// Ҫ�����Ŀ��
		UInt64 targetId;
		// ÿһ�θ����һ��ID
		UInt32 reviveId;
		// ���ĵĸ��������
		UInt32 reviveCoinNum;
	};
	
	/**
	*@brief client->teamcopy �����б�
	*/
	class TeamCopyInviteListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_INVITE_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief teamcopy->client
	*/
	class TeamCopyInviteListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_INVITE_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & inviteList;
		}

		std::vector<TeamCopyInviteInfo> inviteList;
	};
	

	/**
	*@brief client->teamcopy ����ѡ��
	*/
	class TeamCopyInviteChoiceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_INVITE_CHOICE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isAgree & teamIds;
		}

		// 0�ܾ�����0ͬ��
		UInt32 isAgree;
		// ����Id
		std::vector<UInt32> teamIds;
	};

	/**
	*@brief teamcopy->client
	*/
	class TeamCopyInviteChoiceRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_INVITE_CHOICE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & teamIds;
		}

		UInt32 retCode;
		std::vector<UInt32> teamIds;
	};

	/**
	*@brief teamcopy->client ����֪ͨ
	*/
	class TeamCopyAppointmentNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_APPOINTMENT_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & post & name;
		}

		UInt32 post;
		std::string name;
	};

	struct TCTypeData
	{
		TCTypeData() : dayQuitTeamNum(0), weekQuitTeamNum(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dayNumStr & dayQuitTeamNum & weekNumStr & weekQuitTeamNum &
				modelPassStr & dayFlopStr & weekFlopStr & extraNumStr & rollNumStr;
		}

		// �մ���
		std::string dayNumStr;
		// ���˳�����
		UInt32 dayQuitTeamNum;
		// �ܴ���
		std::string weekNumStr;
		// ���˳�����
		UInt32 weekQuitTeamNum;
		// ģʽͨ�ش���
		std::string modelPassStr;
		// �շ���
		std::string dayFlopStr;
		// �ܷ���
		std::string weekFlopStr;
		// �������
		std::string extraNumStr;
		// roll����
		std::string rollNumStr;
	};
	
	/**
	*@brief teamcopy<->scene �����Ϣͬ��
	*/
	class TeamCopyPlayerInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PLAYER_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & equipScore & tcTypeDataMap;
		}

		TeamCopyPlayerInfoSync() : roleId(0), equipScore(0) {}

		UInt64 roleId;
		// װ������
		UInt32 equipScore;
		// �ű�����
		std::map<UInt32, TCTypeData> tcTypeDataMap;
	};

	/**
	*@brief scene->teamcopy ��Ʊͬ��
	*/
	class TeamCopyPlayerTicketSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PLAYER_TICKET_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & ticketMap;
		}

		TeamCopyPlayerTicketSync() : roleId(0) {}

		UInt64 roleId;
		// (key->�ű�����, val->��Ʊ�Ƿ��㹻)
		std::map<UInt32, UInt32> ticketMap;
	};

	/**
	*@brief scene->teamcopy װ������ͬ��
	*/
	class TeamCopyPlayerEquipScoreSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_EQUIP_SCORE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & equipScore;
		}

		TeamCopyPlayerEquipScoreSync() : roleId(0), equipScore(0) {}

		UInt64 roleId;
		UInt32 equipScore;
	};
	
	/**
	*@brief scene->teamcopy loading
	*/
	class TeamCopyPlayerLoadingSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PLAYER_LOADING_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & isLoading;
		}

		UInt64 roleId;
		UInt32 isLoading;
	};

	/**
	*@brief teamcopy->client ����״̬֪ͨ
	*/
	class TeamCopyTeamStatusNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_STATUS_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & teamStatus;
		}

		UInt32 teamId;
		UInt32 teamStatus;
	};

	/**
	*@brief teamcopy->client ����֪ͨ
	*/
	class TeamCopyApplyNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_APPLY_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & IsHasApply;
		}

		UInt32 IsHasApply;
	};

	/**
	*@brief teamcopy->client ��ҵ��ű�����
	*/
	class TeamCopyPlayerInfoNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PLAYER_INFO_NOTIFY)
	public:
		TeamCopyPlayerInfoNotify(): dayNum(0),dayTotalNum(0),weekNum(0),weekTotalNum(0),diffWeekNum(0),diffWeekTotalNum(0),isCreateGold(0),
			dayFreeQuitNum(0),weekFreeQuitNum(0),ticketIsEnough(0),commonGradePassNum(0),unlockDiffGradeCommonNum(0),rollRewardNum(0),
			rollRewardTotalNum(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dayNum & dayTotalNum & weekNum & weekTotalNum & diffWeekNum & diffWeekTotalNum & isCreateGold & dayFreeQuitNum
				& weekFreeQuitNum & ticketIsEnough & commonGradePassNum & unlockDiffGradeCommonNum & rollRewardNum & rollRewardTotalNum & yetOpenGradeVec;
		}

		// ��ͨ�մ���
		UInt32 dayNum;
		// ��ͨ���ܴ���
		UInt32 dayTotalNum;
		// ��ͨ�ܴ���
		UInt32 weekNum;
		// ��ͨ���ܴ���
		UInt32 weekTotalNum;
		// ج���ܴ���
		UInt32 diffWeekNum;
		// ج�����ܴ���
		UInt32 diffWeekTotalNum;
		// �Ƿ���Դ�������
		UInt32 isCreateGold;
		// ������˳�����
		UInt32 dayFreeQuitNum;
		// ������˳�����
		UInt32 weekFreeQuitNum;
		// ��Ʊ�Ƿ��㹻
		UInt32 ticketIsEnough;
		// ��ͨ�Ѷ�ͨ�ش���
		UInt32 commonGradePassNum;
		// ����ج����Ҫ����ͨ�Ѷȴ���
		UInt32 unlockDiffGradeCommonNum;
		// roll��������
		UInt32 rollRewardNum;
		// roll�����ܴ���
		UInt32 rollRewardTotalNum;
		// �ѿ����Ѷ�
		std::vector<UInt32> yetOpenGradeVec;
	};

	/**
	*@brief client->teamcopy ��ļ
	*/
	class TeamCopyRecruitReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_RECRUIT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief teamcopy->client
	*/
	class TeamCopyRecruitRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_RECRUIT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief client->teamcopy ���Ӽ���
	*/
	class TeamCopyLinkJoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_LINK_JOIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & isGold;
		}

		// ����Id
		UInt32 teamId;
		// �Ƿ����(��0�ǽ���)
		UInt32 isGold;
	};

	/**
	*@brief teamcopy->client
	*/
	class TeamCopyLinkJoinRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_LINK_JOIN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief teamcopy->client �ݵ�״̬֪ͨ
	*/
	class TeamCopyFieldStatusNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FIELD_STATUS_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fieldId & fieldStatus;
		}

		// �ݵ�id
		UInt32 fieldId;
		// �ݵ�״̬
		UInt32 fieldStatus;
	};

	/**
	*@brief teamcopy->client ���뱻�ܾ�֪ͨ
	*/
	class TeamCopyApplyRefuseNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_APPLY_REFUSE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & chiefName;
		}

		std::string chiefName;
	};

	/**
	*@brief teamcopy->client �������֪ͨ
	*/
	class TeamCopyPlayerExpireNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PLAYER_EXPIRE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & expireTime;
		}

		UInt64 playerId;
		UInt64 expireTime;
	};

	/**
	*@brief scene->teamcopy ��ֵ��������
	*/
	class TeamCopyCfgValReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_CFG_VAL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief teamcopy->scene ��ֵ���÷���
	*/
	class TeamCopyCfgValRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_CFG_VAL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & valCfgMap;
		}

		std::map<UInt32, UInt32> valCfgMap;
	};

	/**
	*@brief teamcopy->scene ��һ�ý���
	*/
	class TeamCopyGetItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GET_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & notice & reason & rewards;
		}

		UInt64 roleId;
		UInt8 notice;
		std::string reason;
		ItemRewardVec rewards;
	};

	/**
	*@brief teamcopy->client �ݵ��������
	*/
	class TeamCopyFieldUnlockRate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FIELD_UNLOCK_RATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bossPhase & bossBloodRate & fieldId & unlockRate;
		}

		// boss�׶�
		UInt32 bossPhase;
		// bossѪ������
		UInt32 bossBloodRate;
		// �ݵ�id
		UInt32 fieldId;
		// ����
		UInt32 unlockRate;
	};

	/**
	*@brief client->teamcopy �ͻ����ϱ��׶�boss��Ϣ
	*/
	class TeamCopyPhaseBossInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PHASE_BOSS_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & roleId & curFrame & phase & bossBloodRate;
		}

		// ����Id
		UInt64 raceId;
		// ��ɫId
		UInt64 roleId;
		// ��ǰ֡
		UInt32 curFrame;
		// �׶�
		UInt32 phase;
		// bossѪ���ٷֱ�
		UInt32 bossBloodRate;
	};

	/**
	*@brief client->teamcopy ǿ�ƽ������
	*/
	class TeamCopyForceEndFlag : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FORCE_END_FLAG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief client->teamcopy ǿ�ƽ���
	*/
	class TeamCopyForceEndReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FORCE_END_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief teamcopy->client ǿ�ƽ���
	*/
	class TeamCopyForceEndRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FORCE_END_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief teamcopy->client ǿ�ƽ���ͶƱ֪ͨ
	*/
	class TeamCopyForceEndVoteNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FORCE_END_VOTE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & voteDurationTime & voteEndTime;
		}

		// ͶƱ����ʱ��
		UInt32 voteDurationTime;
		// ͶƱ��ֹʱ��
		UInt32 voteEndTime;
	};

	/**
	*@brief client->teamcopy ͶƱ
	*/
	class TeamCopyForceEndVoteReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FORCE_END_VOTE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & vote;
		}

		// ��0ͬ�⣬0�ܾ�
		UInt32 vote;
	};
	
	/**
	*@brief teamcopy->client ����ͶƱ
	*/
	class TeamCopyForceEndMemberVote : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FORCE_END_MEMBER_VOTE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & vote;
		}

		UInt64 roleId;
		// ��0ͬ�⣬0�ܾ�
		UInt32 vote;
	};

	/**
	*@brief teamcopy->client ǿ��ͶƱ���
	*/
	class TeamCopyForceEndVoteResult : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FORCE_END_VOTE_RESULT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		// ���ؽ��(0�ɹ�����0ʧ��)
		UInt32 result;
	};
	
	/**
	*@brief client->teamcopy �޸Ķ���װ����������
	*/
	class TeamCopyModifyEquipScoreReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_MODIFY_EQUIP_SCORE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipScore;
		}

		UInt32 equipScore;
	};
	
	/**
	*@brief teamcopy->client �޸Ķ���װ�����ַ���
	*/
	class TeamCopyModifyEquipScoreRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_MODIFY_EQUIP_SCORE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief teamcopy->client ���Ӿݵ������·�
	*/
	class TeamCopyGridFieldNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_FIELD_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fieldVec;
		}

		// �ݵ��б�
		std::vector<TCGridObjData> fieldVec;
	};

	/**
	*@brief teamcopy->client ���ӹ��������·�
	*/
	class TeamCopyGridMonsterNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_MONSTER_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & monsterVec & monsterPathVec;
		}

		// �����б�
		std::vector<TCGridObjData> monsterVec;
		// ����·��
		std::vector<TCMonsterPath> monsterPathVec;
	};

	/**
	*@brief teamcopy->client ����С�������·�
	*/
	class TeamCopyGridSquadNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_SQUAD_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & squadId & squadStatus & targetGridId & targetObjId & squadData & pathVec;
		}

		// С��id
		UInt32 squadId;
		// С��״̬(TCGridSquadStatus)
		UInt32 squadStatus;
		// Ŀ�����id
		UInt32 targetGridId;
		// Ŀ�����id
		UInt32 targetObjId;
		// С������
		TCGridObjData squadData;
		// ·��
		std::vector<UInt32> pathVec;
	};

	/**
	*@brief teamcopy->client ����С�ӵ�����
	*/
	class TeamCopyOtherSquadData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_OTHER_SQUAD_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & squadId & squadStatus & squadGridId & squadTargetId & squadMemberNum;
		}

		// С��id
		UInt32 squadId;
		// С��״̬(TCGridSquadStatus)
		UInt32 squadStatus;
		// ����С��λ��
		UInt32 squadGridId;
		// ����С�ӵ�Ŀ��
		UInt32 squadTargetId;
		// ��Ա����
		UInt32 squadMemberNum;
	};

	/**
	*@brief client->teamcopy �ű������ƶ�
	*/
	class TeamCopyGridMoveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_MOVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & targetGridId & targetObjId;
		}

		// Ŀ�����id
		UInt32 targetGridId;
		// Ŀ�����id
		UInt32 targetObjId;
	};

	class TeamCopyGridMoveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_MOVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & squadId & squadStatus & targetGridId & targetObjId & pathVec;
		}

		// ������
		UInt32 retCode;
		// С��id
		UInt32 squadId;
		// ״̬
		UInt32 squadStatus;
		// Ŀ�����id
		UInt32 targetGridId;
		// Ŀ�����id
		UInt32 targetObjId;
		// ·��(����id)
		std::vector<UInt32> pathVec;
	};

	/**
	*@brief teamcopy->client С����ݵ�ս��ͬ��
	*/
	class TeamCopyGridSquadBattleNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_SQUAD_BATTLE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & squadId & squadGridId & fieldGridId & isBattle;
		}

		// С��id
		UInt32 squadId;
		// С�Ӹ���id
		UInt32 squadGridId;
		// �ݵ㡢������ӵ�id
		UInt32 fieldGridId;
		// �Ƿ�ս��(0ս����������0ս����)
		UInt32 isBattle;
	};

	/**
	*@brief teamcopy->client ��������
	*/
	class TeamCopyGridMonsterDead : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_MONSTER_DEAD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & monsterObjId;
		}

		// ����id
		UInt32 monsterObjId;
		// �Ƿ�ɱ(1�ǣ�0����)
		UInt32 isKill;
	};

	/**
	*@brief teamcopy->client �ű�2��������
	*/
	class TeamCopyTwoTeamData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TWO_TEAM_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gameOverTime;
		}

		// ��Ϸ����ʱ��
		UInt32 gameOverTime;
	};

	/**
	*@brief teamcopy->client ���ﴫ����֪ͨ
	*/
	class TeamCopyMonsterTransfer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_MONSTER_TRANSFER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mTransferData;
		}

		// ���ﴫ����
		TCGridObjData mTransferData;
	};

	/**
	*@brief teamcopy->client roll����֪ͨ
	*/
	class TeamCopyRollNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_ROLL_NOTIFY)
	public:
		TeamCopyRollNotify() : rollEndStamp(0), rollTime(0), rollOption(0), rollItemId(0), rollItemNum(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rollEndStamp & rollTime & rollOption & rollItemId & rollItemNum & convertItem;
		}

		// roll����ʱ���
		UInt32 rollEndStamp;
		// rollʱ��
		UInt32 rollTime;
		// rollѡ��
		UInt32 rollOption;

		// roll����
		UInt32 rollItemId;
		UInt32 rollItemNum;

		// ת������
		std::vector<ItemRewardBase> convertItem;
	};
	
	/**
	*@brief client->teamcopy roll����ѡ��
	*/
	class TeamCopyRollChoiceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_ROLL_CHOICE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isNeed;
		}

		// �Ƿ�����(��0����0ǫ��)
		UInt32 isNeed;
	};

	/**
	*@brief teamcopy->client
	*/
	class TeamCopyRollChoiceRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_ROLL_CHOICE_RES)
	public:
		TeamCopyRollChoiceRes() : retCode(0), isNeed(0), rollPoint(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & isNeed & rollPoint;
		}

		UInt32 retCode;
		UInt32 isNeed;
		UInt32 rollPoint;
	};

	/**
	*@brief client->teamcopy roll����·�
	*/
	class TeamCopyRollResult : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_ROLL_RESULT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rewardPlayer & rollVec;
		}

		// �����
		UInt64 rewardPlayer;
		std::vector<TCPlayerRoll> rollVec;
	};

	/**
	*@brief teamcopy server -> world server 
	*/
	class TeamCopyTeamPlayerInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_PLAYER_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerIdVec & teamType & teamGrade & costTime;
		}

		// ����б�
		std::vector<UInt64> playerIdVec;
		//�ű�����
		UInt32 teamType;
		// �ű��Ѷ�
		UInt32 teamGrade;
		//����ʱ��
		UInt32 costTime;
	};
};

#endif
