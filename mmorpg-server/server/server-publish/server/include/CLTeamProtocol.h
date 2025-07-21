#ifndef _CL_TEAM_PROTOCOL_H_
#define _CL_TEAM_PROTOCOL_H_

#include <CLProtocolDefine.h>
#include <CLTeamDefine.h>
#include <CLObjectDefine.h>

namespace CLProtocol
{
	/**
	 *@brief client->server ���󴴽�����
	 */
	class WorldCreateTeam : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CREATE_TEAM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
            stream & target;
		}

        // ����Ŀ��
        UInt32          target;
	};

    /**
    *@brief server->client �������鷵��
    */
    class WorldCreateTeamRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_CREATE_TEAM_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & result;
        }

        UInt32      result;
    };

    /**
    *@brief server->client ������鷵��
    */
    class WorldJoinTeamRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_JOIN_TEAM_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & result & inTeam;
        }

        UInt32      result;
		UInt8		inTeam;
    };

    /**
    *@brief client->server->client ͬ���ӳ�����
    */
    class WorldTeamMasterOperSync : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_MASTER_OPER_SYNC)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & type & param;
        }

        UInt8       type;
        UInt32      param;
    };
    
	/**
	 *@brief server->client ͬ��������Ϣ
	 */
	class WorldSyncTeamInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAMINFO)
	public:

		WorldSyncTeamInfo()
		{
			id = 0;
			autoAgree = true;
			options = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & target & autoAgree & master & members & options;
		}

	public:
		//������
		UInt16	id;
        //����Ŀ��
        UInt32  target;
		//�Ƿ��Զ�ͬ��
		UInt8	autoAgree;
		//�ӳ�
		ObjID_t	master;
		//�����Ա����
		std::vector<TeammemberInfo>	members;
		//����ѡ��(��ӦTeamOption��λ���)
		UInt32	options;
	};

    /**
    *@brief client->server �����޸�λ��״̬���򿪻�رգ�
    */
    class WorldTeamChangePosStatusReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_CHANGE_POS_STATUS_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & pos & open;
        }

        // λ��
        UInt8       pos;
        // 1�����λ�ã�0����ر�λ��
        UInt8       open;
    };
    
    /**
    *@brief server->client ͬ��λ��״̬�ı�
    */
    class WorldTeamChangePosStatusSync : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_CHANGE_POS_STATUS_SYNC)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & pos & open;
        }

        // λ��
        UInt8       pos;
        // 1�����λ�ã�0����ر�λ��
        UInt8       open;
    };

    /**
    *@brief server->client ͬ��λ��״̬�ı�
    */
    class WorldTeamReadyReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_READY_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & ready;
        }

        // �Ƿ�׼����(0:ȡ�� 1:׼��)
        UInt8       ready;
    };

    /**
    *@brief server->client ͬ��λ��״̬�ı�
    */
    class WorldSyncTeamMemberStatusToScene : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAMMEMBER_STATUS_TOSCENE)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & id & roleId & statusMask;
        }

        UInt32        id;
        ObjID_t       roleId;
        UInt8         statusMask;
    };
    
    
	/**
	 *@brief server->client ֪ͨ�³�Ա����
	 */
	class WorldNotifyNewTeamMember : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_NEW_TEAMMEMBER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		//��Ա��Ϣ
		TeammemberInfo	info;
	};

	/**
	 *@brief client->server �����뿪����
	 */
	class WorldLeaveTeamReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_LEAVETEAM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}
		//���˻��Լ�
		ObjID_t	id;
	};

	/**
	 *@brief server->client ֪ͨ��Ա�뿪
	 */
	class WorldNotifyMemberLeave : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_TEAMMEMBER_LEAVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//��Աid
		ObjID_t	id;
	};

	/**
	 *@brief server->client ֪ͨ��Ա������
	 */
	class WorldSyncTeamMemberStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAMMEMBER_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & statusMask;
		}

	public:
		//��Աid
		ObjID_t	id;
        // ״̬���루��Ӧö��TeamMemberStatusMask��
        UInt8   statusMask;
	};

	/**
	 *@brief client->server ������ķ���ģʽ
	 */
	class WorldSetDistributeMode : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_TEAMDISTRIBUTE_MODE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mode;
		}

	public:
		//��ģʽ
		UInt8	mode;
	};

	/**
	 *@brief server->client ͬ������ģʽ
	 */
	class WorldSyncDistributeMode : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAMDISTRIBUTE_MODE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mode;
		}

	public:
		//��ģʽ
		UInt8	mode;
	};

    /**
    *@brief client->server ���ö�������
    */
    class WorldSetTeamOption : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_SET_TEAM_OPTION)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & type & str & param1 & param2;
        }

    public:
        // �������ͣ�TEAM_OPTION_TARGET��
        UInt8           type;
        // ���治ͬ����´���ͬ������
        std::string     str;
        UInt32          param1;
        UInt32          param2;
    };

    /**
    *@brief server->client ͬ����������
    */
    class WorldSyncTeamOption : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAM_OPTION)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & type & str & param1 & param2;
        }

    public:
        // �������ͣ�TEAM_OPTION_TARGET��
        UInt8           type;
        // ���治ͬ����´���ͬ������
        std::string     str;
        UInt32          param1;
        UInt32          param2;
    };

	/**
	 *@brief client->server ����ת�öӳ�
	 */
	class WorldTransferTeammaster : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TRANSFER_TEAMMASTER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//��Աid
		ObjID_t	id;
	};

	/**
	 *@brief server->client ͬ���¶ӳ�
	 */
	class WorldSyncTeammaster : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAMMASTER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & master;
		}

	public:
		//�ӳ�
		ObjID_t	master;
	};

	/**
	 *@brief client->server ��ɢ����
	 */
	class WorldDismissTeam : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISMISS_TEAM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/** 
	 *@brief server->cilent ������������뷢����������
	 */
	class WorldTeamPasswdReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_PASSWD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & target;
		}
		//Ŀ��
		ObjID_t	target;
	};

	/**
	 *@brief world->scene ͬ��������Ϣ������
	 */
	class WorldSyncTeamInfoToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAMINFO_TOSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & master & members;
		}

	public:
		//����id
		UInt32	id;
		//�ӳ�
		ObjID_t	master;
		//��Ա�б�
		std::vector<SceneTeamMember>	members;
	};

	/**
	 *@brief world->scene ֪ͨ����ɾ������
	 */
	class WorldNotifyDelTeamToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_DELTEAM_TOSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//����id
		UInt32 id;
	};

	/**
	 *@brief world->scene ͬ�����������Ϣ������
	 */
	class WorldSyncTeamBaseToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAMBASE_TOSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & master;
		}

	public:
		//����id
		UInt32 id;
		//�ӳ�
		ObjID_t	master;
	};

	/** 
	 *@brief world->scene ֪ͨ��Ա����
	 */
	class WorldNotifyMemberInToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_MEMBERIN_TOSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & memberId;
		}

	public:
		//����id
		UInt32	id;
		//��Աid
		ObjID_t	memberId;
	};

	/**
	 *@brief world->scene ֪ͨ��Ա�뿪
	 */
	class WorldNotifyMemberOutToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_MEMBEROUT_TOSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & memberId;
		}

	public:
		//����id
		UInt32 id;
		//��Աid
		ObjID_t	memberId;
	};

	/**
	 *@brief world->scene ֪ͨ���ʱ�����ܶȼ���
	 */
	class WorldSyncTeamIntimacy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAM_INTIMACYLEVEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & intimacyLevel;
		}

	public:
		//���id
		ObjID_t	id;
		//���ܶȼ���
		UInt8	intimacyLevel;
	};

	/**
	 *@brief scene->client ֪ͨ�����뿪����
	 */
	class SceneNotifyTeamMemberLeaveScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_TEAMMEMBER_LEAVESCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//����id
		ObjID_t id;
	};

	/**
	 *@brief scene->client ͬ������λ�ñ仯�������ھ����򲻷���
	 */
	class SceneSyncTeamMemberPos : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_TEAMMEMBER_POS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & pos;
		}

	public:
		//����id
		ObjID_t	id;
		//λ��
		CLPosition	pos;
	};

	/**
	*@brief scene->client ͬ������λ�ñ仯�������ھ����򲻷���
	*/
	class SceneSyncTeamMemberAvatar : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_TEAMMEMBER_AVATAR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & avatar;
		}

	public:
		//����id
		ObjID_t			id;
		//���
		PlayerAvatar	avatar;
	};

	/**
	*@brief client->scene ����ʼ���ƥ��
	*/
	class SceneTeamMatchStartReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TEAM_MATCH_START_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId;
		}

	public:
		UInt32		dungeonId;
	};
	
	/**
	*@brief scene->client ��ʼ���ƥ�䷵��
	*/
	class SceneTeamMatchStartRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TEAM_MATCH_START_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32		result;
	};

	/**
	 *@brief scene->world ���鹫��
	 */
	class WorldTeamBroadcastNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_BROADCAST_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & type & color & word;
		}

	public:
		//����id
		UInt32	teamId;
		//����
		UInt16	type;
		//��ɫ
		UInt8	color;
		//���
		std::string	word;
	};

	/** 
	 *@brief scene->world �������󴴽�����
	 */
	class WorldSceneCreateTeamReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SCENE_CREATETEAM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId;
		}

	public:
		//���id
		ObjID_t	playerId;
	};

    /**
    *@brief client->world ��ѯ�����б�
    */
    class WorldQueryTeamList : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_TEAM_LIST)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & teamId & targetId & targetList & startPos & num & param;
        }

    public:
        // ���ݶ���������
        UInt16				teamId;
		// ����Ŀ��
		UInt32				targetId;
        // ����Ŀ������
        std::vector<UInt32>	targetList;
        // ��ѯ��ʼλ��
        UInt16				startPos;
        // �������
        UInt8				num;
		// ����1(��ʱֻ�ù��ḱ�����)
		UInt8				param;
    };

    /**
    *@brief world->client ���ض����б�
    */
    class WorldQueryTeamListRet : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_TEAM_LIST_RET)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & targetId & teamList & startPos & maxNum & param;
        }

    public:
		// ����Ŀ��
		UInt32							targetId;
        std::vector<TeamBaseInfo>       teamList;
        UInt16                          startPos;
        UInt16                          maxNum;
		// ����1(��ʱֻ�ù��ḱ�����)
		UInt8							param;
    };

	/**
	*@brief world->client ���ض����б�
	*/
	class WorldSyncTeammemberAvatar : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAMMEMBER_AVATAR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & memberId & avatar;
		}

	public:
		ObjID_t							memberId;
		PlayerAvatar					avatar;
	};

	/**
	*@brief world->client ֪ͨ����������ӵ���
	*/
	class WorldTeamNotifyNewRequester : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_NOTIFY_NEW_REQUESTER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	public:
	};
	

	/**
	*@brief client->world ��ȡ�����б�
	*/
	class WorldTeamRequesterListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_REQUESTER_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	public:
	};

	/**
	*@brief world->client ���������б�
	*/
	class WorldTeamRequesterListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_REQUESTER_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requesters;
		}

	public:
		std::vector<TeamRequester> requesters;
	};

	/**
	*@brief client->world �����������ߣ�ͬ�⡢�ܾ���
	*/
	class WorldTeamProcessRequesterReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_PROCESS_REQUESTER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & targetId & agree;
		}

	public:
		// Ŀ��ID
		ObjID_t		targetId;
		// �Ƿ�ͬ��
		UInt8		agree;
	};

	/**
	*@brief world->client ���������߷���
	*/
	class WorldTeamProcessRequesterRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_PROCESS_REQUESTER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & targetId & result;
		}

	public:
		// Ŀ��ID
		ObjID_t		targetId;
		// ���
		UInt32		result;
	};

	/**
	*@brief world->client ֪ͨ��ʼ���³�ͶƱ
	*/
	class WorldTeamRaceVoteNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_RACE_VOTE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId;
		}

	public:
		// ���³�ID
		UInt32			dungeonId; 
	};

	/**
	*@brief client->world ����ϱ�ͶƱѡ��
	*/
	class WorldTeamReportVoteChoice : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_REPORT_VOTE_CHOICE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & agree;
		}

	public:
		// �Ƿ�ͬ��
		UInt8	agree;
	};

	/**
	*@brief world->client �������뷵��
	*/
	class WorldTeamInviteRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_INVITE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & targetId & result;
		}

	public:
		ObjID_t		targetId;
		UInt32		result;
	};

	/**
	*@brief world->client ֪ͨ��Ҷ�������
	*/
	class WorldTeamInviteNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_INVITE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		TeamBaseInfo info;
	};

	/**
	*@brief world->client ֪ͨ��Ҷ�����������
	*/
	class WorldTeamRequestResultNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_REQUEST_RESULT_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & agree;
		}

	public:
		UInt8		agree;
	};

	/**
	*@brief world->client �㲥������ͶƱѡ��
	*/
	class WorldTeamVoteChoiceNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_VOTE_CHOICE_NOTIFY)
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
	*@brief world->client ֪ͨ�����ӿ���ƥ����
	*/
	class WorldTeamMatchResultNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_MATCH_RESULT_NOTFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & players;
		}

	public:
		UInt32						dungeonId;
		std::vector<PlayerIcon>		players;
	};

	/**
	*@brief scene->world ����ʼ���ƥ��
	*/
	class WorldTeamMatchStartReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_MATCH_START_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dungeonId;
		}

	public:
		ObjID_t		roleId;
		UInt32		dungeonId;
	};

	/**
	*@brief client->world ����ȡ�����ƥ��
	*/
	class WorldTeamMatchCancelReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_MATCH_CANCEL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client ȡ�����ƥ�䷵��
	*/
	class WorldTeamMatchCancelRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_MATCH_CANCEL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32		result;
	};

	/**
	*@brief world->scene ��scene�������Ŀ��������Ϣ��ʣ�������
	*/
	class WorldTeamQueryTargetCondReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_QUERY_COND_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & teamTargetId;
		}

	public:
		ObjID_t		roleId;
		UInt32		teamTargetId;
	};

	/**
	*@brief scene->client ��scene������鿪��������Ϣ��ʣ�������
	*/
	class WorldTeamQueryTargetCondRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_QUERY_COND_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & teamTargetId & remainTimes;
		}

	public:
		ObjID_t		roleId;
		UInt32		teamTargetId;
		UInt32		remainTimes;
	};

	/**
	*@brief world->client ͬ����Ա����
	*/
	class WorldSyncTeamMemberProperty : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_TEAM_MEMBER_PROPERTY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & memberId & type & value & playerLabelInfo;
		}

	public:
		ObjID_t			memberId;
		// ��Ӧö��TeamMemberProperty
		UInt8			type;
		ObjID_t			value;
		//��ұ�ǩ��Ϣ
		PlayerLabelInfo playerLabelInfo;
	};

	/**
	*@brief client->world �޸���սѡ��
	*/
	class WorldChangeAssistModeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHANGE_ASSIST_MODE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isAssist;
		}

	public:
		UInt8			isAssist;
	};

	/**
	*@brief world->client �����������
	*/
	class WorldTeamInviteClearNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_INVITE_CLEAR_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId;
		}

	public:
		UInt32		teamId;
	};

	/**
	*@brief world->client ͬ���������Ը���������
	*/
	class WorldTeamInviteSyncAttr : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_INVITE_SYNC_ATTR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & target;
		}

	public:
		UInt32		teamId;
		UInt32		target;
	};


	/**
	*@brief world->scene ��ʼ����³�
	*/
	class WorldTeamDungeonRaceStart : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_DUNGEON_RACE_START)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & teamId & cityMonster;
		}

	public:
		ObjID_t			raceId;
		UInt32			teamId;
		SceneNpc		cityMonster;
	};

	/**
	*@brief world->scene ���³ǽ���
	*/
	class WorldTeamDungeonRaceOver : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_DUNGEON_RACE_OVER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & teamId & cityMonster;
		}

	public:
		ObjID_t			raceId;
		UInt32			teamId;
		SceneNpc		cityMonster;
	};
	

	/**
	*@brief world->client ��ӽ�ɢ����ʱ״̬
	*/
	class WorldNotifyTeamKick : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_TEAM_KICK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & endTime;
		}

	public:
		UInt64			endTime;
	};
	
}

#endif
