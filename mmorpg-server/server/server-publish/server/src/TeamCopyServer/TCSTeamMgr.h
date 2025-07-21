#ifndef _TCS_TEAM_MGR_H_
#define _TCS_TEAM_MGR_H_

#include <set>
#include <CLDefine.h>
#include <AvalonSimpleTimer.h>

class TCSTeam;
class TCSTeamCheck;
class TCSTeamPlayerInfo;
class TCSPlayer;
struct TeamCopyBattlePlan;
struct DungeonRacePlayerInfo;
struct TCConsumeItem;


namespace CLProtocol{
	class TeamCopyCreateTeamReq;
	class DungenServerTeamStartRaceRes;
	class DungeonSvrDungeonRaceEnd;
}

namespace Avalon{
	class Protocol;
	class Packet;
}

/**
*@brief 团队管理器
*/
class TCSTeamMgr : public Avalon::Singleton<TCSTeamMgr>
{
public:
	TCSTeamMgr();
	~TCSTeamMgr();

	/**
	*@brief Tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief Final
	*/
	void Final();

	/**
	*@brief 队伍是否存在
	*/
	bool IsExistTeam(UInt32 teamId);

	/**
	*@brief 配置同步到scene
	*/
	void ValCfgSyncToScene(UInt32 connId);

	/**
	*@brief 创建队伍
	*/
	void HandleCreateTeamReq(TCSPlayer* player, CLProtocol::TeamCopyCreateTeamReq& req);

	/**
	*@brief 请求队伍数据
	*/
	void HandleTeamDataReq(TCSPlayer* player, UInt32 teamType);

	/**
	*@brief 请求队伍详情
	*/
	void HandleTeamDetailReq(TCSPlayer* player, UInt32 teamId);

	/**
	*@brief 请求队伍列表
	*/
	void HandleTeamListReq(TCSPlayer* player, UInt32 pageNum, UInt32 type, UInt32 model);

	/**
	*@brief 入队申请请求
	*/
	void HandleTeamApplyReq(TCSPlayer* player, UInt32 teamId, UInt32 isGold);

	/**
	*@brief 退出队伍请求
	*/
	void HandleTeamQuitReq(TCSPlayer* player);

	/**
	*@brief 申请列表请求
	*/
	void HandleTeamApplyListReq(TCSPlayer* player);

	/**
	*@brief 申请回复
	*/
	void HandleTeamApplyReplyReq(TCSPlayer* player, UInt32 isAgree, std::vector<UInt64>& playerIds);

	/**
	*@brief 开战请求
	*/
	void HandleStartBattleReq(TCSPlayer* player, UInt32 planModel, std::vector<TeamCopyBattlePlan>& battlePlanVec);

	/**
	*@brief 开战投票
	*/
	void HandleStartBattleVote(TCSPlayer* player, UInt32 vote);

	/**
	*@brief 客户端发起开始挑战
	*/
	void HandleStartChallengeReq(TCSPlayer* player, UInt32 fieldId);

	/**
	*@brief 查询玩家信息返回
	*/
	void QueryPlayerRaceRes(UInt32 teamSquadId, UInt64 roleId, UInt32 result, DungeonRacePlayerInfo& playerInfo);

	/**
	*@brief 创建玩家信息查询
	*/
	void CreateTeamPlayerInfo(UInt32 teamSquadId, UInt32 dungeonId, UInt32 fieldId, UInt32 tailDungeonId, std::set<UInt64> playerIds);

	/**
	*@brief 请求玩家列表
	*/
	void HandlePlayerListReq(TCSPlayer* player);

	/**
	*@brief 邀请玩家
	*/
	void HandleInvitePlayerReq(TCSPlayer* player, std::vector<UInt64>& inviteList);

	/**
	*@brief 请求翻牌
	*/
	void HandleStageFlopReq(TCSPlayer* player, UInt32 stage);

	/**
	*@brief 换座位
	*/
	void HandleChangeSeatReq(TCSPlayer* player, UInt32 srcSeat, UInt32 destSeat);
	
	/**
	*@brief 踢人
	*/
	void HandleKickReq(TCSPlayer* player, UInt64 playerId);

	/**
	*@brief 任命
	*/
	void HandleAppointmentReq(TCSPlayer* player, UInt64 playerId, UInt32 post);

	/**
	*@brief 自动同意金主入团
	*/
	void HandleAutoAgreeGoldReq(TCSPlayer* player, UInt32 isAutoAgree);

	/**
	*@brief 邀请选择
	*/
	void HandleInviteChoiceReq(TCSPlayer* player, UInt32 isAgree, std::vector<UInt32> teamIds);

	/**
	*@brief 招募
	*/
	void HandleRecruitReq(TCSPlayer* player);

	/**
	*@brief 链接加入
	*/
	void HandleLinkJoinReq(TCSPlayer* player, UInt32 teamId, UInt32 isGold);

	/**
	*@brief 检查返回
	*/
	void HandleCheckCondRes(UInt32 retCode, UInt32 teamId, UInt64 roldId, UInt64 commissionOrder, UInt32 connId,
		const TCConsumeItem& commission, const std::vector<TCConsumeItem>& ticket);

	/**
	*@brief 地下城开始挑战返回
	*/
	void HandlerStartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res);

	/**
	*@brief 比赛结算
	*/
	void HandlerRaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end);

	/**
	*@brief 上报boss阶段
	*/
	void HandlerPhaseBoss(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate);

	/**
	*@brief 强制结束
	*/
	void HandleForceEndReq(TCSPlayer* player);

	/**
	*@brief 强制结束投票
	*/
	void HandleForceEndVoteReq(TCSPlayer* player, UInt32 vote);

	/**
	*@brief 修改装备评分
	*/
	void HandleModifyEquipScore(TCSPlayer* player, UInt32 equipScore);

	/**
	*@brief 移动格子
	*/
	void HandleSquadMoveGrid(TCSPlayer* player, UInt32 targetGridId, UInt32 targetObjId);

	/**
	*@brief roll奖励
	*/
	void HandleRollReward(TCSPlayer* player, UInt32 isNeed);

	/**
	*@brief boss阶段据点
	*/
	void BossPhaseAddField(UInt32 teamSquadId, UInt32 fieldId);
	void BossPhaseDelField(UInt32 teamSquadId, UInt32 fieldId);

	/**
	*@brief 玩家退出
	*/
	void TicketUpdate(UInt64 playerId, UInt32 isEnough);

	/**
	*@brief 玩家退出
	*/
	void OnPlayerQuit(UInt64 playerId);
	
	/**
	*@brief 玩家队伍Id
	*/
	UInt32 GetPlayerTeamId(UInt64 playerId);
	void SetPlayerTeamId(UInt64 playerId, UInt32 teamId);

	/**
	*@brief 玩家队伍小队Id
	*/
	UInt32 GetPlayerTeamSquadId(UInt64 playerId);

	/**
	*@brief 是否重连
	*/
	bool IsReconnect(UInt32 teamId);

	/**
	*@brief 同步队伍玩家
	*/
	void SyncAllObjectToPlayer(TCSPlayer* player);

	/**
	*@brief GM通关当前阶段所有据点
	*/
	void GMClearAllField(UInt64 playerId);

	/**
	*@brief GM通关据点
	*/
	void GMClearField(UInt64 playerId, UInt32 fieldId);

	/**
	*@brief GM清除小队状态
	*/
	void GMClearSquadStatus(UInt64 playerId);

	/**
	*@brief GM团本失败
	*/
	void GMTeamCopyFailed(UInt64 playerId);

	/**
	*@brief GM能量恢复据点比例
	*/
	void GMEnergyReviveRate(UInt64 playerId, UInt32 rate);

	/**
	*@brief GM设置小队属性
	*/
	void GMSetSquadPro(UInt64 playerId, UInt32 grid, UInt32 cd);

	/**
	*@brief GM清理格子据点
	*/
	void GMClearGridObj(UInt64 playerId, UInt32 gridType);

	/**
	*@brief 队伍开战检查
	*/
	TCSTeamCheck* GetTeamCheck(UInt32 teamId);
	void DelTeamCheck(UInt32 teamId);
	void AddTeamCheck(UInt32 teamId, UInt64 playerId);

	/**
	*@brief 玩家离线
	*/
	void PlayerExpire(UInt64 playerId, UInt64 expireTime);

	/**
	*@brief 获取攻打据点id
	*/
	UInt32 GetAttackFieldId(UInt32 teamSquadId);

	/**
	*@brief 广播队伍消息
	*/
	void BroadcastToTeam(Avalon::Protocol& protocol, UInt32 teamId, UInt64 exceptPlayer = 0);
	void BroadcastToTeam(Avalon::Packet* packet, UInt32 teamId, UInt64 exceptPlayer = 0);

	/**
	*@brief 广播小队消息
	*/
	void BroadcastToSquad(Avalon::Protocol& protocol, UInt32 teamId, UInt64 playerId);
	void BroadcastToSquad(Avalon::Packet* packet, UInt32 teamId, UInt64 playerId);

private:

	/**
	*@brief 加入队伍列表
	*/
	TCSTeam* _FindTeam(UInt32 teamId);
	TCSTeam* _FindTeam(UInt64 playerId);

	/**
	*@brief 加入队伍列表
	*/
	void _AddTeamMap(TCSTeam* team);

	/**
	*@brief 退出队伍
	*/
	UInt32 _QuitTeam(UInt64 playerId, TCSPlayer* player);

	/**
	*@brief 玩家队伍Id
	*/
	UInt32 _GetPlayerTeamId(UInt64 playerId);
	void _SetPlayerTeamId(UInt64 playerId, UInt32 teamId);

	/**
	*@brief 检查战斗配置
	*/
	bool _CheckBattlePlan(const std::vector<TeamCopyBattlePlan>& battlePlanVec);

	/**
	*@brief 查找玩家数据
	*/
	TCSTeamPlayerInfo* _FindTeamPlayerInfo(UInt32 teamSquadId);

	/**
	*@brief playerInfo 结束
	*/
	void _TeamPlayerInfoEnd(UInt32 teamSquadId);

	/**
	*@brief 创建队伍
	*/
	TCSTeam* _CreateTeam(UInt32 tcType);

private:

	typedef std::map<UInt32, TCSTeam*> TCSTeamMap;
	typedef std::map<UInt32, TCSTeamCheck> TCSTeamCheckMap;
	typedef std::map<UInt32, TCSTeamPlayerInfo> TCSTeamPlayerInfoMap;

	// 队伍ID种子
	UInt32 m_TeamIdSeed;

	// 玩家所在队伍(key->playerId, val->teamId)
	std::map<UInt64, UInt32> m_PlayerTeamIdMap;

	// 队伍列表(key->队伍Id)
	TCSTeamMap	 m_TeamMap;

	// 检查
	TCSTeamCheckMap		m_TeamCheck;

	// 查询玩家数据(key->teamSquadId)
	TCSTeamPlayerInfoMap	 m_TeamPlayerInfoMap;

	Avalon::SimpleTimer m_60sTimer;

};

#endif
