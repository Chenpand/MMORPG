#ifndef _TCS_TEAM_H_
#define _TCS_TEAM_H_

#include <AvalonProtocol.h>
#include <CLTeamCopyDefine.h>
#include <CLItemDefine.h>

#include "TCSPlayerMgr.h"

struct TeamCopyTargetCfg;
struct TeamCopyFieldCfg;
class TeamCopyFieldDataEntry;
class TCSPlayer;
namespace CLProtocol {
	class DungenServerTeamStartRaceRes;
	class DungeonSvrDungeonRaceEnd;
};

#define TEAMINFO(T) "team(type:" << T->GetTeamType() << ",id:" << T->GetTeamId() \
	<< ",name:" << T->GetTeamName() << ",grade:" << T->GetTeamGrade() << ",model:" << T->GetModel()<< ") "

/**
*@brief 团本队伍
*/
class TCSTeam
{
	AVALON_DEFINE_OBJECTPOOL(TCSTeam, Avalon::NullMutex)

public:
	TCSTeam();
	virtual ~TCSTeam();

	/**
	*@brief 团本id
	*/
	inline void SetTeamType(UInt32 type) { m_TeamType = TeamCopyType(type); }
	inline UInt32 GetTeamType() { return m_TeamType; }
	
	/**
	*@brief 语音房间id
	*/
	inline void SetVoiceRoomId(const std::string& id) { m_VoiceRoomId = id; }
	inline const std::string& GetVoiceRoomId() { return m_VoiceRoomId; }

	/**
	*@brief 队伍id
	*/
	inline void SetTeamId(UInt32 id) { m_TeamId = id; }
	inline UInt32 GetTeamId() { return m_TeamId; }

	/**
	*@brief 名字
	*/
	inline void SetTeamName(const std::string& name) { m_TeamName = name; }
	inline const std::string& GetTeamName() { return m_TeamName; }

	/**
	*@brief 难度
	*/
	inline void SetTeamGrade(UInt32 grade) { m_TeamGrade = TeamCopyTeamGrade(grade); }
	inline UInt32 GetTeamGrade() { return m_TeamGrade; }

	/**
	*@brief 模式
	*/
	inline void SetModel(UInt32 model) { m_Model = TeamCopyTeamModel(model); }
	inline UInt32 GetModel() { return m_Model; }

	/**
	*@brief 装备评分
	*/
	inline void SetEquipScore(UInt32 score) { m_EquipScore = score; }
	inline UInt32 GetEquipScore() { return m_EquipScore; }

	/**
	*@brief 条件佣金
	*/
	inline void SetCondCommission(UInt32 num) { m_CondCommission = num; }
	inline UInt32 GetCondCommission() { return m_CondCommission; }

	/**
	*@brief 总佣金
	*/
	inline void SetTotalCommission(UInt32 num) { m_TotalCommission = num; }
	inline UInt32 GetTotalCommission() { return m_TotalCommission; }

	/**
	*@brief 状态
	*/
	inline void SetStatus(UInt32 status) { m_Status = status; }
	inline UInt32 GetStatus() { return m_Status; }

	/**
	*@brief 团长Id
	*/
	inline void SetChiefId(UInt64 id) { m_ChiefId = id; }
	inline UInt64 GetChiefId() { return m_ChiefId; }

	/**
	*@brief 自动同意金主入团
	*/
	inline void SetAutoAgreeGold(UInt32 isAgree) { m_AutoAgreeGold = isAgree; }
	inline bool GetAutoAgreeGold() { return m_AutoAgreeGold ? 1 : 0; }

	/**
	*@brief 阶段
	*/
	inline void SetCurStage(UInt32 stage) { m_CurStage = stage; }
	inline UInt32 GetCurStage() { return m_CurStage; }

	/**
	*@brief 配置模式
	*/
	inline void SetPlanModel(UInt32 model) { m_PlanModel = model; }
	inline UInt32 GetPlanModel() { return m_PlanModel; }

	/**
	*@brief 游戏结束时间
	*/
	inline void SetGameOverTime(UInt32 time) { m_GameOverTime = time; }
	inline UInt32 GetGameOverTime() { return m_GameOverTime; }

	/**
	*@brief 招募CD
	*/
	inline void SetRecruitCD(UInt32 time) { m_RecruitCD = time; }
	inline UInt32 GetRecruitCD() { return m_RecruitCD; }

	/**
	*@brief 扣除消耗标记
	*/
	inline void SetConsumeFlag(bool flag) { m_ConsumeFlag = flag; }
	inline bool GetConsumeFlag() { return m_ConsumeFlag; }

	/**
	*@brief 投票结束时间
	*/
	inline void SetVoteEndTime(UInt32 time) { m_VoteEndTime = time; }
	inline UInt32 GetVoteEndTime() { return m_VoteEndTime; }
	
	/**
	*@brief 检查玩家条件结束时间
	*/
	inline void SetCheckCondEndTime(UInt32 time) { m_CheckCondEndTime = time; }
	inline UInt32 GetCheckCondEndTime() { return m_CheckCondEndTime; }
	
	/**
	*@brief 强制结束标记
	*/
	inline void SetForceEndFlag(bool flag) { m_ForceEndFlag = flag; }
	inline bool GetForceEndFlag() { return m_ForceEndFlag; }

	/**
	*@brief 强制结束投票时间
	*/
	inline void SetForceEndVoteTime(UInt32 time) { m_ForceEndVoteTime = time; }
	inline UInt32 GetForceEndVoteTime() { return m_ForceEndVoteTime; }
	
	/**
	*@brief 强制结束冷却时间
	*/
	inline void SetForceEndCoolTime(UInt32 time) { m_ForceEndCoolTime = time; }
	inline UInt32 GetForceEndCoolTime() { return m_ForceEndCoolTime; }

	/**
	*@brief 小队
	*/
	const std::vector<TeamCopySquad>& GetSquadList() { return m_SquadVec; }
	
	/**
	*@brief 是否已经开战
	*/
	bool IsYetStartBattle();

	/**
	*@brief 是否战斗中
	*/
	bool IsInBattle();

	/**
	*@brief 是否结束
	*/
	bool IsGameOver();

	/**
	*@brief 佣金分成额度
	*/
	UInt32 CommissionBonusNum();

	/**
	*@brief Tick
	*/
	virtual bool OnTick(const Avalon::Time& now);

	/**
	*@brief 加队伍检查
	*/
	UInt32 CheckJoinTeam(TCSPlayer* joinPlayer, UInt32 isGold);

	/**
	*@brief 增加队员
	*/
	UInt32 JoinTeam(TCSPlayer* joinPlayer, UInt32 isGold = 0);
	bool QuitTeam(UInt64 playerId);

	/**
	*@brief 获得队伍人数
	*/
	UInt32 GetTeamMemberNum();

	/**
	*@brief 金主数量
	*/
	UInt32 GoldPlayerNum();

	/**
	*@brief 检查开战
	*/
	UInt32 CheckStartBattle();

	/**
	*@brief 申请列表
	*/
	UInt32 JoinApply(UInt64 playerId, UInt32 isGold);
	void RemoveApply(UInt64 playerId);
	void ClearApply();
	void ApplyListRes(TCSPlayer* player);
	UInt32 AgreeApply(UInt64 playerId);
	UInt32 ApplyNum();

	/**
	*@brief 设置小队状态
	*/
	void SetSquadStatus(UInt32 squadId, UInt32 status);

	/**
	*@brief 队伍数据广播
	*/
	void TeamDataBroadcast();
	void TeamDataRes(TCSPlayer* player);

	/**
	*@brief 获得所有队伍成员列表
	*/
	const std::map<UInt64, UInt32>& GetMemberMap() { return m_PlayerSquad;  }

	/**
	*@brief 开战权限
	*/
	bool StartChallengePower(UInt64 playerId);

	/**
	*@brief 获得玩家小队id
	*/
	UInt32 GetPlayerSquadId(UInt64 playerId);

	/**
	*@brief 通知未投票玩家投票
	*/
	void NotifyPlayerVote();

	/**
	*@brief 开战投票
	*/
	void HandlerVote(UInt64 playerId, UInt32 vote);

	/**
	*@brief 删除投票玩家
	*/
	void RemoveVote(UInt64 playerId);

	/**
	*@brief 据点攻打小队
	*/
	void FieldAddAttackSquad(UInt32 squadId, UInt32 fieldId, UInt64 raceId);
	void FieldDelAttackSquad(UInt32 squadId, UInt32 fieldId, UInt64 raceId);

	/**
	*@brief 通知小队状态
	*/
	void SquadNotify(UInt32 squadId, UInt32 squadStatus);

	/**
	*@brief 佣金订单号
	*/
	void AddCommissionOrder(UInt64 playerId, UInt32 connid, UInt64 commissionOrder);

	/**
	*@brief 换位置
	*/
	UInt32 ChangeSeat(UInt32 srcSeat, UInt32 destSeat);

	/**
	*@brief 任命成员
	*/
	UInt32 AppointmentPost(UInt64 handlerPlayer, UInt64 playerId, UInt32 post);

	/**
	*@brief 更新门票
	*/
	void TicketUpdate(UInt64 playerId, UInt32 isEnough);

	/**
	*@brief 踢人
	*/
	UInt32 KickMember(UInt64 playerId);

	/**
	*@brief 加入时间限制
	*/
	void JoinTimeLimit(UInt64 playerId, UInt64 expireTime);

	/**
	*@brief 是否加入时间限制
	*/
	bool IsJoinTimeLimit(UInt64 playerId);

	/**
	*@brief 获取加入时间限制
	*/
	UInt64 GetJoinTimeLimit(UInt64 playerId);

	/**
	*@brief 消耗存库
	*/
	void SaveConsume(UInt64 playerId, UInt32 itemId, UInt32 itemNum, UInt32 consumeType);
	void RemoveDBConsume(UInt64 guid);
	void RemoveAllMemberConsume();

	/**
	*@brief 队员消耗返还
	*/
	void MemberReturnConsume(UInt64 playerId = 0);

	/**
	*@brief 强制结束投票
	*/
	void ForceEndVote(TCSPlayer* player, UInt32 vote);

	/**
	*@brief 强制结束投票超时
	*/
	void ForceEndVoteTimeOut();

	/**
	*@brief 返还消耗根据类型
	*/
	void ReturnConsumeByType(UInt32 consumeType);

	/**
	*@brief 失败返还挑战次数
	*/
	void FailedReturnChallengeNum();

	/**
	*@brief 翻牌请求
	*/
	void FlopDataRes(TCSPlayer* player, UInt32 stage);

	/**
	*@brief 据点结算Udp日志
	*/
	void UdpFieldSettle(UInt32 squadId, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime, UInt32 result);

	/**
	*@brief 清除装备评分低的玩家
	*/
	void ClearEquipScoreLowPlayer();

	/**
	*@brief GM修改能量据点进度
	*/
	void GMUpdateEnergyField(UInt32 rate);

	/**
	*@brief roll奖励
	*/
	void RollRewardPoint(TCSPlayer* player, UInt32 isNeed);

	/**
	*@brief 广播通知
	*/
	template <typename... Args>
	void BroadcastToMember(UInt32 squadId, UInt32 id, const Args & ... args)
	{
		for (auto& iter : m_PlayerSquad)
		{
			if (squadId != 0 && iter.second != squadId)
			{
				continue;
			}

			TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
			if (NULL == player)
			{
				continue;
			}

			player->SendNotify(id, args...);
		}
	}

	/**
	*@brief 广播队伍消息,squadId为0表示全队广播，否则表示小队ID
	*/
	void BroadcastToMember(Avalon::Protocol& protocol, UInt32 squadId = 0);
	void BroadcastToMember(Avalon::Packet& packet, UInt32 squadId = 0);

	/**
	*@brief 广播队伍消息, 除某玩家之外
	*/
	void BroadcastToExceptMember(Avalon::Protocol& protocol, UInt64 exceptPlayer = 0);
	void BroadcastToExceptMember(Avalon::Packet& packet, UInt64 exceptPlayer = 0);

protected:

	/**
	*@brief 查找队员
	*/
	TeamCopyMember* _FindMember(UInt64 playerId);
	TeamCopyMember* _FindMember(UInt32 seatId);

	/**
	*@brief 查找队长
	*/
	TeamCopyMember* _FindCaptain(UInt64 playerId);

	/**
	*@brief 查找团长
	*/
	TeamCopyMember* _FindChief();

	/**
	*@brief 查找小队
	*/
	TeamCopySquad* _FindSquad(UInt32 squadId);

	/**
	*@brief 小队是否有队员
	*/
	UInt32 _SquadMemberNum(UInt32 squadId);

	/**
	*@brief 获得玩家小队id
	*/
	UInt32 _GetPlayerSquadId(UInt64 playerId);

	/**
	*@brief 更新据点
	*/
	UInt32 _UpdateField(UInt32 fieldId);

	/**
	*@brief 阶段结束
	*/
	void _StageEnd(UInt32 stage, UInt32 param = 0);

	/**
	*@brief 增加翻牌信息
	*/
	void _AddFlop(UInt32 stage, const TeamCopyFlop& flop);

	/**
	*@brief 普通玩家数量
	*/
	UInt32 _CommonPlayerNum();

	/**
	*@brief 投票通知
	*/
	void _VoteNotify(UInt64 playerId, UInt32 vote);

	/**
	*@brief 投票完成，开战
	*/
	void _VoteFinish();

	/**
	*@brief 投票超时
	*/
	void _VoteTimeOut();

	/**
	*@brief 条件检查超时
	*/
	void _CheckCondTimeOut();

	/**
	*@brief 结束处理佣金 参数：是否返还
	*/
	void _GameOverCommission(bool isReturn);

	/**
	*@brief 团队状态通知
	*/
	void _TeamStatusNotify(UInt32 status);

	/**
	*@brief 分成佣金
	*/
	void _CommissionBonus(UInt32 senderId, UInt32 titleId, UInt32 contentId);

	/**
	*@brief 增加通关次数
	*/
	void _AddModelPassNum(UInt32 model, UInt32 costTime);

	/**
	*@brief 队员通知
	*/
	void _MemberNotify(const std::string& name, UInt32 flag);

	/**
	*@brief bos据点通知衍生据点被歼灭
	*/
	bool _BossFieldAttachDestroyNotify(UInt32 dungeonId);

	/**
	*@brief 据点状态通知
	*/
	void _FieldStatusNotify(UInt32 fieldId, UInt32 status);

	/**
	*@brief 增加强制结束的比赛
	*/
	void _AddForceEndRace(UInt64 raceId);

	/**
	*@brief 是否是强制结束比赛
	*/
	bool _IsForceEndRace(UInt64 raceId);

	/**
	*@brief 开战Udp日志
	*/
	void _UdpStartBattle();

	/**
	*@brief 通知强制结束
	*/
	void _NotifyForceEndFlag();

	/**
	*@brief 处理能量恢复据点
	*/
	void _HandleEnergyReviveField(UInt32 progress, UInt32 energyDungeonId);

	/**
	*@brief 通知战斗能量恢复进度
	*/
	void _NotifyRaceEnergyRevive(UInt32 progress, UInt32 energyDungeonId, UInt32 needNotifyFieldType);

	/**
	*@brief 获取本局开始时间
	*/
	UInt32 _GetGameStartTime();

	/**
	*@brief 本局比赛是否跨天或周
	*/
	UInt32 _GameIsCrossDate(UInt32 time);

	/**
	*@brief 是否参与阶段结算
	*/
	bool _StageIsSettle(UInt32 stage, UInt64 playerId);

	/**
	*@brief 小队通关据点
	*/
	void _RaceSettlement(UInt32 squadId, UInt64 raceId);

	/**
	*@brief roll奖励
	*/
	void _RollReward();

	/**
	*@brief 获取roll点
	*/
	UInt32 _GetRollPoint();

	/**
	*@brief roll tick
	*/
	void _OnRollTick(const Avalon::Time& now);

	/**
	*@brief 发送roll邮件奖励
	*/
	void _SendRollMail(UInt64 playerId, UInt32 playerConnId);

public:

	/**
	*@brief 小队攻坚数据通知
	*/
	virtual void SquadAttackDataNotify() {}

	/**
	*@brief 获取门票
	*/
	virtual void GetTicket(std::vector<TCConsumeItem>& itemVec) {}

	/**
	*@brief 金主限制数量
	*/
	virtual UInt32 GetGoldMaxNum() { return 0; }
	virtual UInt32 GetGoldNumErrorCode() { return 0; }

	/**
	*@brief 设置团队目标
	*/
	virtual bool SetTeamTarget(UInt32 stage) { return true; }

	/**
	*@brief 攻坚数据通知
	*/
	virtual void AttackDataNotify(UInt32 stage) {}
	virtual void AttackDataNotify(TCSPlayer* player) {}

	/**
	*@brief 小队通关据点
	*/
	virtual void ClearField(UInt32 squadId, UInt32 fieldId) {}

	/**
	*@brief 设置小队难度
	*/
	virtual void SetSquadGrade(UInt32 squadId, UInt32 grade) {}
	
	/**
	*@brief 设置小队目标
	*/
	virtual void SetSquadTarget(UInt32 squadId, const TeamCopyTargetCfg* targetCfg) {}

	/**
	*@brief 添加据点
	*/
	virtual bool AddField(const TeamCopyFieldDataEntry* fieldEntry) { return true; }

	/**
	*@brief 据点比赛结束
	*/
	virtual void FieldRaceEnd(UInt32 fieldId) {}

	/**
	*@brief boss阶段通知
	*/
	virtual void BossPhaseNotify(TCSPlayer* player) {}

	/**
	*@brief 据点通知
	*/
	virtual void FieldNotify(const std::vector<UInt32>& filedVec) {}
	virtual void FieldNotify(UInt32 fieldId) {}

	/**
	*@brief 上报阶段boss信息
	*/
	virtual void PhaseBossInfo(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate){}

	/**
	*@brief 通知战斗能量恢复进度
	*/
	virtual void NotifyRaceEnergyRevive(UInt64 raceId) {}

	/**
	*@brief 条件检查完成
	*/
	virtual void CheckCondFinish();

	/**
	*@brief 删掉boss衍生据点
	*/
	virtual void DelBossAttachField() {}

	/**
	*@brief 通知所有小队退出地下城
	*/
	virtual void NotifyAllSquadLeaveDungeon() {}

	/**
	*@brief 通知所有小队地下城被歼灭
	*/
	virtual void NotifyAllSquadDungeonDestory(UInt32 dungeonId) {}

	/**
	*@brief 查找据点
	*/
	virtual TeamCopyField* FindField(UInt32 fieldId) { return NULL; }

	/**
	*@brief 队伍是否满
	*/
	virtual bool TeamIsFull() { return true; }

	/**
	*@brief 移动格子
	*/
	virtual void MoveGrid(TCSPlayer* player, UInt32 targetGridId, UInt32 targetObjId) {}

	/**
	*@brief 查询玩家信息超时
	*/
	virtual void QueryRacePlayerInfoTimeOut(UInt32 squadId);

	/**
	*@brief 开始发起比赛
	*/
	virtual UInt32 StartRace(UInt32 squadId, UInt32 fieldId);

	/**
	*@brief 创建比赛返回
	*/
	virtual void StartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res) {}

	/**
	*@brief 比赛结束
	*/
	virtual void RaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime) {}

	/**
	*@brief Gm通关
	*/
	virtual void GMClearAllField(UInt32 squadId) {}

	/**
	*@brief 设置小队属性
	*/
	virtual void SetSquadPro(UInt64 playerId, UInt32 grid, UInt32 cd) {}

	/**
	*@brief 清除格子对象
	*/
	virtual void GMClearGridObj(UInt64 playerId, UInt32 gridType) {}

	/**
	*@brief 游戏结束
	*/
	virtual void GameOver(UInt32 overReason);

	/**
	*@brief 是否允许换座位
	*/
	virtual UInt32 IsAllowChangeSeat(UInt32 srcSquadId, UInt32 destSquadId) { return 0; }

	/**
	*@brief 换位完成
	*/
	virtual void ChangeSeatFinish(UInt32 srcSquadId, bool srcIsDec, UInt32 destSquadId, bool destIsInc) {}

	/**
	*@brief 成员离线
	*/
	virtual void MemberExpire(UInt64 playerId, UInt64 expireTime);

	/**
	*@brief 成员数量空
	*/
	virtual void SquadMemberNull(UInt32 squadId) {}

	/**
	*@brief 阶段Udp日志
	*/
	virtual void UdpStageEnd(UInt32 costTime, UInt32 result) {}

protected:

	// 团本id
	TeamCopyType m_TeamType;
	// 语音房间ID
	std::string m_VoiceRoomId;
	// id
	UInt32 m_TeamId;
	// 队伍名字
	std::string m_TeamName;
	// 难度
	TeamCopyTeamGrade m_TeamGrade;
	// 模式
	TeamCopyTeamModel m_Model;
	// 装备评分
	UInt32 m_EquipScore;
	// 条件佣金
	UInt32 m_CondCommission;
	// 总佣金
	UInt32 m_TotalCommission;
	// 状态
	UInt32 m_Status;
	// 团长ID
	UInt64 m_ChiefId;
	// 自动同意金主入团
	UInt32 m_AutoAgreeGold;
	// 当前阶段
	UInt32 m_CurStage;
	// 配置模式
	UInt32 m_PlanModel;
	// 游戏结束时间
	UInt32 m_GameOverTime;
	// 招募CD时间
	UInt32 m_RecruitCD;
	// 扣除消耗标记
	bool m_ConsumeFlag;

	// 申请列表(key->玩家id，val->是否金主)
	std::map<UInt64, UInt32> m_ApplyMap;

	// 小队列表
	std::vector<TeamCopySquad> m_SquadVec;

	// 玩家所在的小队(key->玩家id，小队id)
	std::map<UInt64, UInt32> m_PlayerSquad;

	// 投票结束时间
	UInt32 m_VoteEndTime;
	// 投票列表
	std::set<UInt64> m_VoteSet;

	// 翻牌信息(key->阶段)
	std::map<UInt32, std::vector<TeamCopyFlop>> m_FlopMap;

	// 检查玩家条件结束时间
	UInt32 m_CheckCondEndTime;

	// 佣金记录(key->playerId, val->佣金订单号)
	std::map<UInt64, TeamCopyCommission> m_CommissiomMap;

	// 阶段通关(key->阶段，val->playerId)
	std::map<UInt32, std::set<UInt64>> m_StageSettlePlayerMap;

	// 强制结束的比赛(比赛Id)
	std::set<UInt64> m_ForceEndRaceSet;

	// 玩家入队时间限制(key->playerId)
	std::map<UInt64, UInt64>	m_PlayerJoinTimeLimitMap;

	// 强制结束标记
	bool m_ForceEndFlag;
	// 开战后退队人数
	UInt32 m_BattleQuitTeamNum;
	// 强制投票结束时间
	UInt32 m_ForceEndVoteTime;
	// 强制结束冷却时间
	UInt32 m_ForceEndCoolTime;
	// 强制投票(key->playerId, val->是否同意)
	std::map<UInt64, bool> m_ForceEndVoteMap;

	// roll结束时间戳
	UInt32 m_RollEndStamp;
	// roll奖励
	ItemRewardVec m_RollRewards;
	std::vector<UInt32> m_RollPointVec;
	std::map<UInt64, TCPlayerRoll> m_RollMap;
};

#endif
