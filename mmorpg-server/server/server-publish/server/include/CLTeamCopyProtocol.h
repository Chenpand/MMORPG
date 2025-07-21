#ifndef _CL_TEAM_COPY_PROTOCOL_H_
#define _CL_TEAM_COPY_PROTOCOL_H_

#include <CLMatchDefine.h>

#include "CLProtocolDefine.h"
#include "CLTeamCopyDefine.h"

namespace CLProtocol
{
	/**
	*@brief scene->teamcopy 请求进入团本场景
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
	*@brief scene->teamcopy 请求退出团本场景
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
	*@brief client->teamcopy 创建团队请求
	*/
	class TeamCopyCreateTeamReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_CRETAE_TEAM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamType & teamGrade & teamModel & equipScore & param;
		}

		// 团本类型
		UInt32 teamType;
		// 难度
		UInt32 teamGrade;
		// 模式
		UInt32 teamModel;
		// 装备评分
		UInt32 equipScore;
		// param(佣金)
		UInt32 param;
	};

	/**
	*@brief teamcopy->client 创建团队返回
	*/
	class TeamCopyCreateTeamRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_CRETAE_TEAM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & retCode;
		}

		// 团队Id
		UInt32 teamId;
		UInt32 retCode;
	};

	/**
	*@brief client->teamcopy 团队数据请求
	*/
	class TeamCopyTeamDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_DATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & teamType;
		}

		// 团队Id
		UInt32 teamId;
		// 团本类型
		UInt32 teamType;
	};

	/**
	*@brief teamcopy->client 团队数据返回
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
		// 队伍ID
		UInt32 teamId;
		// 装备评分
		UInt32 equipScore;
		// 团队状态
		UInt32 status;
		// 团队名字
		std::string teamName;
		// 佣金总数
		UInt32 totalCommission;
		// 分成佣金
		UInt32 bonusCommission;
		// 自动同意金主入团(0：不同意，1：同意)
		UInt32 autoAgreeGold;
		// 团本类型
		UInt32 teamType;
		// 团本模式
		UInt32 teamModel;
		// 难度
		UInt32 teamGrade;
		// 语音房间
		std::string voiceRoomId;
		// 小队列表
		std::vector<TeamCopySquad> squadList;
	};

	/**
	*@brief client->teamcopy 团队列表请求
	*/
	class TeamCopyTeamListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamType & teamModel & pageNum;
		}

		// 团本类型
		UInt32 teamType;
		// 模式
		UInt32 teamModel;
		// 请求页	
		UInt32 pageNum;
	};

	/**
	*@brief 团队面板属性
	*/
	struct TeamCopyTeamProperty
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & teamModel & commission & teamName & teamType & teamGrade & memberNum & equipScore & status;
		}

		// id
		UInt32 teamId;
		// 模式
		UInt32 teamModel;
		// 佣金
		UInt32 commission;
		// 名字
		std::string teamName;
		// 团本类型
		UInt32 teamType;
		// 难度
		UInt32 teamGrade;
		// 人数
		UInt32 memberNum;
		// 装备评分
		UInt32 equipScore;
		// 状态
		UInt32 status;
	};

	/**
	*@brief teamcopy->client 团队列表返回
	*/
	class TeamCopyTeamListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & curPage & totalPageNum & teamPropertyList;
		}

		// 当前页
		UInt32 curPage;
		// 总页数
		UInt32 totalPageNum;
		// 团队列表
		std::vector<TeamCopyTeamProperty> teamPropertyList;
	};

	/**
	*@brief client->teamcopy 申请入团
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

		// 团队ID
		UInt32 teamId;
		// 是否金主(非0是金主)
		UInt32 isGold;
	};
	
	/**
	*@brief teamcopy->client 入团返回
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
	*@brief client->teamcopy 退出团队请求
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
	*@brief teamcopy->client 退出团队返回
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
	*@brief client->teamcopy 开战请求
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
	*@brief teamcopy->client 开战返回
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
	*@brief teamcopy->client 开战通知
	*/
	class TeamCopyStartBattleNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_START_BATTLE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & voteDurationTime & voteEndTime;
		}

		// 投票持续时间
		UInt32 voteDurationTime;
		// 投票截止时间
		UInt32 voteEndTime;
	};

	/**
	*@brief client->teamcopy 开战投票
	*/
	class TeamCopyStartBattleVote : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_START_BATTLE_VOTE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & vote;
		}

		// 非0同意，0拒绝
		UInt32 vote;
	};

	/**
	*@brief teamcopy->client 投票通知
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
		// 非0同意，0拒绝
		UInt32 vote;
	};

	/**
	*@brief teamcopy->client 投票完成
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

		// 结果(0代表成功，非0代表失败)
		UInt32 result;
		// 未投票玩家
		std::vector<std::string> notVotePlayer;
	};
	
	/**
	*@brief teamcopy->client 阶段通知
	*/
	class TeamCopyStageNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_STAGE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & stageId & gameOverTime & squadTarget & teamTarget & fieldList;
		}
		
		// 阶段id
		UInt32 stageId;
		// 游戏结束时间
		UInt32 gameOverTime;
		// 小队目标
		TeamCopyTarget squadTarget;
		// 团队目标
		TeamCopyTarget teamTarget;
		// 据点列表
		std::vector<TeamCopyField> fieldList;
	};

	/**
	*@brief teamcopy->client 阶段结束
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

		// 阶段id
		UInt32 stageId;
		// 通关类型（团本二使用）
		UInt32 passType;
	};

	/**
	*@brief teamcopy->client 据点通知
	*/
	class TeamCopyFieldNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FIELD_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fieldList;
		}

		// 据点列表
		std::vector<TeamCopyField> fieldList;
	};
	
	/**
	*@brief teamcopy->client 目标通知
	*/
	class TeamCopyTargetNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TARGET_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & squadTarget & teamTarget;
		}

		// 小队目标
		TeamCopyTarget squadTarget;
		// 团队目标
		TeamCopyTarget teamTarget;
	};

	/**
	*@brief teamcopy->client 小队状态通知
	*/
	class TeamCopySquadStatusNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_SQUAD_STATUS_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & squadId & squadStatus;
		}

		// 小队id
		UInt32 squadId;
		// 小队状态
		UInt32 squadStatus;
	};

	/**
	*@brief teamcopy->client 队伍更新
	*/
	class TeamCopyTeamUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opType & teamId;
		}

		// 1.解散
		UInt32 opType;
		// 队伍ID
		UInt32 teamId;
	};

	/**
	*@brief client->teamcopy 申请列表请求
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
	*@brief teamcopy->client 申请列表返回
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
	*@brief client->teamcopy 处理申请
	*/
	class TeamCopyTeamApplyReplyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_APPLY_REPLY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isAgree & playerIds;
		}

		// 非0同意，0拒绝
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
		// 非0同意，0拒绝
		UInt32 isAgree;
		std::vector<UInt64> playerIds;
	};

	/**
	*@brief client->teamcopy 开始挑战
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
	*@brief teamcopy->scene 查询个人信息
	*/
	class TeamCopyQueryPlayerInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_QUERY_PLAYER_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamSquadId & roleId & dungeonId;
		}

		// 队伍小队Id
		UInt32 teamSquadId;
		// 角色Id
		UInt64 roleId;
		// 地下城Id
		UInt32 dungeonId;
	};
	
	/**
	*@brief scene->teamcopy 查询个人信息返回
	*/
	class TeamCopyQueryPlayerInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_QUERY_PLAYER_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamSquadId & roleId & result & dungeonPlayerInfo;
		}

		// 队伍小队Id
		UInt32 teamSquadId;
		// 角色Id
		UInt64 roleId;
		// 结果
		UInt32 result;
		// 信息
		DungeonRacePlayerInfo dungeonPlayerInfo;
	};

	/**
	*@brief scene->teamcopy 玩家重连回团本
	*/
	class TeamCopyPlayerReconnect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PLAYER_RECONNECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & isSendData;
		}

		// 角色Id
		UInt64 roleId;
		// 是否下发数据
		UInt32 isSendData;
	};

	/**
	*@brief teamcopy->scene 团本检查玩家
	*/
	class TeamCopyPlayerCheckReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PLAYER_CHECK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & roleId & teamGrade & teamMode & forzenCommission & consumeItemVec;
		}

		// 队伍id
		UInt32 teamId;
		// 角色Id
		UInt64 roleId;
		// 队伍难度
		UInt32 teamGrade;
		// 是否金团
		UInt32 teamMode;
		// 冻结佣金
		TCConsumeItem forzenCommission;
		// 扣除的道具材料
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

		// 返回码
		UInt32 retCode;
		// 队伍id
		UInt32 teamId;
		// 角色Id
		UInt64 roleId;
		// 佣金订单号
		UInt64 commissionOrder;
		// 冻结佣金
		TCConsumeItem forzenCommission;
		// 扣除的道具材料
		std::vector<TCConsumeItem> consumeItemVec;
	};

	/**
	*@brief client->teamcopy 找队友
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

		// 玩家
		std::vector<TeamCopyApplyProperty> playerList;
	};

	/**
	*@brief client->teamcopy 邀请
	*/
	class TeamCopyInvitePlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_INVITE_PLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & inviteList;
		}

		// 邀请列表
		std::vector<UInt64> inviteList;
	};

	/**
	*@brief teamcopy->client 邀请通知
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
	*@brief client->teamcopy 翻牌
	*/
	class TeamCopyStageFlopReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_STAGE_FLOP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & stageId;
		}

		// 阶段id
		UInt32 stageId;
	};

	/**
	*@brief teamcopy->client 翻牌
	*/
	class TeamCopyStageFlopRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_STAGE_FLOP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & stageId & flopList;
		}

		// 阶段id
		UInt32 stageId;
		// 翻牌信息
		std::vector<TeamCopyFlop> flopList;
	};

	/**
	*@brief teamcopy->client 团本重连通知
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
	*@brief client->teamcopy 换座位
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
	*@brief client->teamcopy 队伍详情
	*/
	class TeamCopyTeamDetailReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TEAM_DETAIL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId;
		}

		// 团队Id
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

		// 队伍ID
		UInt32 teamId;
		// 团本类型
		UInt32 teamType;
		// 队伍模式
		UInt32 teamModel;
		// 团队名字
		std::string teamName;
		// 佣金总数
		UInt32 totalCommission;
		// 分成佣金
		UInt32 bonusCommission;
		// 小队列表
		std::vector<TeamCopySquad> squadList;
	};

	/**
	*@brief client->teamcopy 踢出队伍
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
	*@brief teamcopy->client 踢出队伍
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
	*@brief client->teamcopy 任命
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
	*@brief teamcopy->client 任命
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
	*@brief teamcopy->client 队员通知
	*/
	class TeamCopyMemberNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_MEMBER_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & memberName & flag;
		}

		// 队员名字
		std::string memberName;
		// 标志(0：加入，1：离开)
		UInt32 flag;
	};
	
	/**
	*@brief client->teamcopy 自动同意金主
	*/
	class TeamCopyAutoAgreeGoldReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_AUTO_AGREE_GOLD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isAutoAgree;
		}

		// (0：不同意，1：同意)
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
		// (0：不同意，1：同意)
		UInt32 isAutoAgree;
	};

	/**
	*@brief teamcopy->client 地下城复活
	*/
	class TeamCopyDungeonRevive : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_DUNGEON_REVIVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & targetId & reviveId & reviveCoinNum;
		}

		// 玩家id
		UInt64 playerId;
		// 要复活的目标
		UInt64 targetId;
		// 每一次复活都有一个ID
		UInt32 reviveId;
		// 消耗的复活币数量
		UInt32 reviveCoinNum;
	};
	
	/**
	*@brief client->teamcopy 邀请列表
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
	*@brief client->teamcopy 邀请选择
	*/
	class TeamCopyInviteChoiceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_INVITE_CHOICE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isAgree & teamIds;
		}

		// 0拒绝，非0同意
		UInt32 isAgree;
		// 队伍Id
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
	*@brief teamcopy->client 任命通知
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

		// 日次数
		std::string dayNumStr;
		// 日退出次数
		UInt32 dayQuitTeamNum;
		// 周次数
		std::string weekNumStr;
		// 周退出次数
		UInt32 weekQuitTeamNum;
		// 模式通关次数
		std::string modelPassStr;
		// 日翻牌
		std::string dayFlopStr;
		// 周翻牌
		std::string weekFlopStr;
		// 额外次数
		std::string extraNumStr;
		// roll次数
		std::string rollNumStr;
	};
	
	/**
	*@brief teamcopy<->scene 玩家信息同步
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
		// 装备评分
		UInt32 equipScore;
		// 团本数据
		std::map<UInt32, TCTypeData> tcTypeDataMap;
	};

	/**
	*@brief scene->teamcopy 门票同步
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
		// (key->团本类型, val->门票是否足够)
		std::map<UInt32, UInt32> ticketMap;
	};

	/**
	*@brief scene->teamcopy 装备评分同步
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
	*@brief teamcopy->client 队伍状态通知
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
	*@brief teamcopy->client 申请通知
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
	*@brief teamcopy->client 玩家的团本数据
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

		// 普通日次数
		UInt32 dayNum;
		// 普通日总次数
		UInt32 dayTotalNum;
		// 普通周次数
		UInt32 weekNum;
		// 普通周总次数
		UInt32 weekTotalNum;
		// 噩梦周次数
		UInt32 diffWeekNum;
		// 噩梦周总次数
		UInt32 diffWeekTotalNum;
		// 是否可以创建金团
		UInt32 isCreateGold;
		// 日免费退出次数
		UInt32 dayFreeQuitNum;
		// 周免费退出次数
		UInt32 weekFreeQuitNum;
		// 门票是否足够
		UInt32 ticketIsEnough;
		// 普通难度通关次数
		UInt32 commonGradePassNum;
		// 解锁噩梦需要的普通难度次数
		UInt32 unlockDiffGradeCommonNum;
		// roll奖励次数
		UInt32 rollRewardNum;
		// roll奖励总次数
		UInt32 rollRewardTotalNum;
		// 已开的难度
		std::vector<UInt32> yetOpenGradeVec;
	};

	/**
	*@brief client->teamcopy 招募
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
	*@brief client->teamcopy 链接加入
	*/
	class TeamCopyLinkJoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_LINK_JOIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & teamId & isGold;
		}

		// 队伍Id
		UInt32 teamId;
		// 是否金主(非0是金主)
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
	*@brief teamcopy->client 据点状态通知
	*/
	class TeamCopyFieldStatusNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FIELD_STATUS_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fieldId & fieldStatus;
		}

		// 据点id
		UInt32 fieldId;
		// 据点状态
		UInt32 fieldStatus;
	};

	/**
	*@brief teamcopy->client 申请被拒绝通知
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
	*@brief teamcopy->client 玩家离线通知
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
	*@brief scene->teamcopy 数值配置请求
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
	*@brief teamcopy->scene 数值配置返回
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
	*@brief teamcopy->scene 玩家获得奖励
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
	*@brief teamcopy->client 据点解锁比例
	*/
	class TeamCopyFieldUnlockRate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FIELD_UNLOCK_RATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bossPhase & bossBloodRate & fieldId & unlockRate;
		}

		// boss阶段
		UInt32 bossPhase;
		// boss血量比例
		UInt32 bossBloodRate;
		// 据点id
		UInt32 fieldId;
		// 比例
		UInt32 unlockRate;
	};

	/**
	*@brief client->teamcopy 客户端上报阶段boss信息
	*/
	class TeamCopyPhaseBossInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_PHASE_BOSS_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & roleId & curFrame & phase & bossBloodRate;
		}

		// 比赛Id
		UInt64 raceId;
		// 角色Id
		UInt64 roleId;
		// 当前帧
		UInt32 curFrame;
		// 阶段
		UInt32 phase;
		// boss血量百分比
		UInt32 bossBloodRate;
	};

	/**
	*@brief client->teamcopy 强制结束标记
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
	*@brief client->teamcopy 强制结束
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
	*@brief teamcopy->client 强制结束
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
	*@brief teamcopy->client 强制结束投票通知
	*/
	class TeamCopyForceEndVoteNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FORCE_END_VOTE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & voteDurationTime & voteEndTime;
		}

		// 投票持续时间
		UInt32 voteDurationTime;
		// 投票截止时间
		UInt32 voteEndTime;
	};

	/**
	*@brief client->teamcopy 投票
	*/
	class TeamCopyForceEndVoteReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FORCE_END_VOTE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & vote;
		}

		// 非0同意，0拒绝
		UInt32 vote;
	};
	
	/**
	*@brief teamcopy->client 有人投票
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
		// 非0同意，0拒绝
		UInt32 vote;
	};

	/**
	*@brief teamcopy->client 强制投票结果
	*/
	class TeamCopyForceEndVoteResult : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_FORCE_END_VOTE_RESULT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		// 返回结果(0成功，非0失败)
		UInt32 result;
	};
	
	/**
	*@brief client->teamcopy 修改队伍装备评分请求
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
	*@brief teamcopy->client 修改队伍装备评分返回
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
	*@brief teamcopy->client 格子据点数据下发
	*/
	class TeamCopyGridFieldNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_FIELD_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fieldVec;
		}

		// 据点列表
		std::vector<TCGridObjData> fieldVec;
	};

	/**
	*@brief teamcopy->client 格子怪物数据下发
	*/
	class TeamCopyGridMonsterNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_MONSTER_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & monsterVec & monsterPathVec;
		}

		// 怪物列表
		std::vector<TCGridObjData> monsterVec;
		// 怪物路径
		std::vector<TCMonsterPath> monsterPathVec;
	};

	/**
	*@brief teamcopy->client 格子小队数据下发
	*/
	class TeamCopyGridSquadNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_SQUAD_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & squadId & squadStatus & targetGridId & targetObjId & squadData & pathVec;
		}

		// 小队id
		UInt32 squadId;
		// 小队状态(TCGridSquadStatus)
		UInt32 squadStatus;
		// 目标格子id
		UInt32 targetGridId;
		// 目标对象id
		UInt32 targetObjId;
		// 小队数据
		TCGridObjData squadData;
		// 路径
		std::vector<UInt32> pathVec;
	};

	/**
	*@brief teamcopy->client 其他小队的数据
	*/
	class TeamCopyOtherSquadData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_OTHER_SQUAD_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & squadId & squadStatus & squadGridId & squadTargetId & squadMemberNum;
		}

		// 小队id
		UInt32 squadId;
		// 小队状态(TCGridSquadStatus)
		UInt32 squadStatus;
		// 其他小队位置
		UInt32 squadGridId;
		// 其他小队的目标
		UInt32 squadTargetId;
		// 成员数量
		UInt32 squadMemberNum;
	};

	/**
	*@brief client->teamcopy 团本格子移动
	*/
	class TeamCopyGridMoveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_MOVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & targetGridId & targetObjId;
		}

		// 目标格子id
		UInt32 targetGridId;
		// 目标对象id
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

		// 返回码
		UInt32 retCode;
		// 小队id
		UInt32 squadId;
		// 状态
		UInt32 squadStatus;
		// 目标格子id
		UInt32 targetGridId;
		// 目标对象id
		UInt32 targetObjId;
		// 路径(格子id)
		std::vector<UInt32> pathVec;
	};

	/**
	*@brief teamcopy->client 小队与据点战斗同步
	*/
	class TeamCopyGridSquadBattleNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_SQUAD_BATTLE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & squadId & squadGridId & fieldGridId & isBattle;
		}

		// 小队id
		UInt32 squadId;
		// 小队格子id
		UInt32 squadGridId;
		// 据点、怪物格子的id
		UInt32 fieldGridId;
		// 是否战斗(0战斗结束，非0战斗中)
		UInt32 isBattle;
	};

	/**
	*@brief teamcopy->client 怪物死亡
	*/
	class TeamCopyGridMonsterDead : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_GRID_MONSTER_DEAD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & monsterObjId;
		}

		// 怪物id
		UInt32 monsterObjId;
		// 是否被杀(1是，0不是)
		UInt32 isKill;
	};

	/**
	*@brief teamcopy->client 团本2队伍数据
	*/
	class TeamCopyTwoTeamData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_TWO_TEAM_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gameOverTime;
		}

		// 游戏结束时间
		UInt32 gameOverTime;
	};

	/**
	*@brief teamcopy->client 怪物传送门通知
	*/
	class TeamCopyMonsterTransfer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_MONSTER_TRANSFER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mTransferData;
		}

		// 怪物传送门
		TCGridObjData mTransferData;
	};

	/**
	*@brief teamcopy->client roll奖励通知
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

		// roll结束时间戳
		UInt32 rollEndStamp;
		// roll时间
		UInt32 rollTime;
		// roll选项
		UInt32 rollOption;

		// roll奖励
		UInt32 rollItemId;
		UInt32 rollItemNum;

		// 转换奖励
		std::vector<ItemRewardBase> convertItem;
	};
	
	/**
	*@brief client->teamcopy roll奖励选择
	*/
	class TeamCopyRollChoiceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_ROLL_CHOICE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isNeed;
		}

		// 是否需求(非0需求，0谦让)
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
	*@brief client->teamcopy roll结果下发
	*/
	class TeamCopyRollResult : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ST_TEAM_COPY_ROLL_RESULT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rewardPlayer & rollVec;
		}

		// 获奖玩家
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

		// 玩家列表
		std::vector<UInt64> playerIdVec;
		//团本类型
		UInt32 teamType;
		// 团本难度
		UInt32 teamGrade;
		//花费时间
		UInt32 costTime;
	};
};

#endif
