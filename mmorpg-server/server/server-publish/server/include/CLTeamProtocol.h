#ifndef _CL_TEAM_PROTOCOL_H_
#define _CL_TEAM_PROTOCOL_H_

#include <CLProtocolDefine.h>
#include <CLTeamDefine.h>
#include <CLObjectDefine.h>

namespace CLProtocol
{
	/**
	 *@brief client->server 请求创建队伍
	 */
	class WorldCreateTeam : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CREATE_TEAM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
            stream & target;
		}

        // 队伍目标
        UInt32          target;
	};

    /**
    *@brief server->client 创建队伍返回
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
    *@brief server->client 加入队伍返回
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
    *@brief client->server->client 同步队长操作
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
	 *@brief server->client 同步队伍信息
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
		//队伍编号
		UInt16	id;
        //队伍目标
        UInt32  target;
		//是否自动同意
		UInt8	autoAgree;
		//队长
		ObjID_t	master;
		//队伍成员链表
		std::vector<TeammemberInfo>	members;
		//队伍选项(对应TeamOption的位组合)
		UInt32	options;
	};

    /**
    *@brief client->server 请求修改位置状态（打开或关闭）
    */
    class WorldTeamChangePosStatusReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_CHANGE_POS_STATUS_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & pos & open;
        }

        // 位置
        UInt8       pos;
        // 1代表打开位置，0代表关闭位置
        UInt8       open;
    };
    
    /**
    *@brief server->client 同步位置状态改变
    */
    class WorldTeamChangePosStatusSync : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_CHANGE_POS_STATUS_SYNC)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & pos & open;
        }

        // 位置
        UInt8       pos;
        // 1代表打开位置，0代表关闭位置
        UInt8       open;
    };

    /**
    *@brief server->client 同步位置状态改变
    */
    class WorldTeamReadyReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_READY_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & ready;
        }

        // 是否准备好(0:取消 1:准备)
        UInt8       ready;
    };

    /**
    *@brief server->client 同步位置状态改变
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
	 *@brief server->client 通知新成员加入
	 */
	class WorldNotifyNewTeamMember : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_NEW_TEAMMEMBER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		//队员信息
		TeammemberInfo	info;
	};

	/**
	 *@brief client->server 请求离开队伍
	 */
	class WorldLeaveTeamReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_LEAVETEAM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}
		//踢人或自己
		ObjID_t	id;
	};

	/**
	 *@brief server->client 通知成员离开
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
		//队员id
		ObjID_t	id;
	};

	/**
	 *@brief server->client 通知成员上下线
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
		//队员id
		ObjID_t	id;
        // 状态掩码（对应枚举TeamMemberStatusMask）
        UInt8   statusMask;
	};

	/**
	 *@brief client->server 请求更改分配模式
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
		//新模式
		UInt8	mode;
	};

	/**
	 *@brief server->client 同步分配模式
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
		//新模式
		UInt8	mode;
	};

    /**
    *@brief client->server 设置队伍属性
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
        // 操作类型（TEAM_OPTION_TARGET）
        UInt8           type;
        // 下面不同情况下代表不同的意义
        std::string     str;
        UInt32          param1;
        UInt32          param2;
    };

    /**
    *@brief server->client 同步队伍属性
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
        // 操作类型（TEAM_OPTION_TARGET）
        UInt8           type;
        // 下面不同情况下代表不同的意义
        std::string     str;
        UInt32          param1;
        UInt32          param2;
    };

	/**
	 *@brief client->server 请求转让队长
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
		//成员id
		ObjID_t	id;
	};

	/**
	 *@brief server->client 同步新队长
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
		//队长
		ObjID_t	master;
	};

	/**
	 *@brief client->server 解散队伍
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
	 *@brief server->cilent 队伍如果有密码发起请求密码
	 */
	class WorldTeamPasswdReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_TEAM_PASSWD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & target;
		}
		//目标
		ObjID_t	target;
	};

	/**
	 *@brief world->scene 同步队伍信息到场景
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
		//队伍id
		UInt32	id;
		//队长
		ObjID_t	master;
		//成员列表
		std::vector<SceneTeamMember>	members;
	};

	/**
	 *@brief world->scene 通知场景删除队伍
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
		//队伍id
		UInt32 id;
	};

	/**
	 *@brief world->scene 同步队伍基础信息到场景
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
		//队伍id
		UInt32 id;
		//队长
		ObjID_t	master;
	};

	/** 
	 *@brief world->scene 通知成员进入
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
		//队伍id
		UInt32	id;
		//成员id
		ObjID_t	memberId;
	};

	/**
	 *@brief world->scene 通知成员离开
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
		//队伍id
		UInt32 id;
		//成员id
		ObjID_t	memberId;
	};

	/**
	 *@brief world->scene 通知组队时的亲密度级别
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
		//玩家id
		ObjID_t	id;
		//亲密度级别
		UInt8	intimacyLevel;
	};

	/**
	 *@brief scene->client 通知队友离开场景
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
		//队友id
		ObjID_t id;
	};

	/**
	 *@brief scene->client 同步队友位置变化，队友在九屏则不发送
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
		//队友id
		ObjID_t	id;
		//位置
		CLPosition	pos;
	};

	/**
	*@brief scene->client 同步队友位置变化，队友在九屏则不发送
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
		//队友id
		ObjID_t			id;
		//外观
		PlayerAvatar	avatar;
	};

	/**
	*@brief client->scene 请求开始组队匹配
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
	*@brief scene->client 开始组队匹配返回
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
	 *@brief scene->world 队伍公告
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
		//队伍id
		UInt32	teamId;
		//类型
		UInt16	type;
		//颜色
		UInt8	color;
		//语句
		std::string	word;
	};

	/** 
	 *@brief scene->world 场景请求创建队伍
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
		//玩家id
		ObjID_t	playerId;
	};

    /**
    *@brief client->world 查询队伍列表
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
        // 根据队伍编号搜索
        UInt16				teamId;
		// 队伍目标
		UInt32				targetId;
        // 根据目标搜索
        std::vector<UInt32>	targetList;
        // 查询起始位置
        UInt16				startPos;
        // 请求个数
        UInt8				num;
		// 参数1(暂时只用公会副本组队)
		UInt8				param;
    };

    /**
    *@brief world->client 返回队伍列表
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
		// 队伍目标
		UInt32							targetId;
        std::vector<TeamBaseInfo>       teamList;
        UInt16                          startPos;
        UInt16                          maxNum;
		// 参数1(暂时只用公会副本组队)
		UInt8							param;
    };

	/**
	*@brief world->client 返回队伍列表
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
	*@brief world->client 通知有新申请入队的人
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
	*@brief client->world 获取申请列表
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
	*@brief world->client 返回申请列表
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
	*@brief client->world 请求处理请求者（同意、拒绝）
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
		// 目标ID
		ObjID_t		targetId;
		// 是否同意
		UInt8		agree;
	};

	/**
	*@brief world->client 处理请求者返回
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
		// 目标ID
		ObjID_t		targetId;
		// 结果
		UInt32		result;
	};

	/**
	*@brief world->client 通知开始地下城投票
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
		// 地下城ID
		UInt32			dungeonId; 
	};

	/**
	*@brief client->world 玩家上报投票选项
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
		// 是否同意
		UInt8	agree;
	};

	/**
	*@brief world->client 队伍邀请返回
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
	*@brief world->client 通知玩家队伍邀请
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
	*@brief world->client 通知玩家队伍请求处理结果
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
	*@brief world->client 广播玩家玩家投票选项
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
		// 角色ID
		ObjID_t	roleId;
		// 是否同意
		UInt8	agree;
	};

	/**
	*@brief world->client 通知玩家组队快速匹配结果
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
	*@brief scene->world 请求开始组队匹配
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
	*@brief client->world 请求取消组队匹配
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
	*@brief scene->client 取消组队匹配返回
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
	*@brief world->scene 向scene请求队伍目标条件信息（剩余次数）
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
	*@brief scene->client 向scene请求队伍开启条件信息（剩余次数）
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
	*@brief world->client 同步队员属性
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
		// 对应枚举TeamMemberProperty
		UInt8			type;
		ObjID_t			value;
		//玩家标签信息
		PlayerLabelInfo playerLabelInfo;
	};

	/**
	*@brief client->world 修改助战选项
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
	*@brief world->client 清除队伍邀请
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
	*@brief world->client 同步队伍属性给被邀请者
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
	*@brief world->scene 开始打地下城
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
	*@brief world->scene 地下城结束
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
	*@brief world->client 组队解散倒计时状态
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
