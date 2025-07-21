#ifndef _TCS_TEAM_ONE_H_
#define _TCS_TEAM_ONE_H_

#include <AvalonSimpleTimer.h>
#include "TCSTeam.h"

class TCSTeamOne : public TCSTeam
{
public:
	TCSTeamOne();
	~TCSTeamOne();

	virtual bool OnTick(const Avalon::Time& now);

	/**
	*@brief 设置团队目标
	*/
	virtual bool SetTeamTarget(UInt32 stage);

	/**
	*@brief 小队攻坚数据通知
	*/
	virtual void SquadAttackDataNotify();

	/**
	*@brief 获取门票
	*/
	virtual void GetTicket(std::vector<TCConsumeItem>& itemVec);

	/**
	*@brief 金主限制数量
	*/
	virtual UInt32 GetGoldMaxNum();
	virtual UInt32 GetGoldNumErrorCode();

	/**
	*@brief 攻坚数据通知
	*/
	virtual void AttackDataNotify(UInt32 stage);
	virtual void AttackDataNotify(TCSPlayer* player);

	/**
	*@brief 小队通关据点
	*/
	virtual void ClearField(UInt32 squadId, UInt32 fieldId);

	/**
	*@brief 设置小队难度
	*/
	virtual void SetSquadGrade(UInt32 squadId, UInt32 grade);

	/**
	*@brief 设置小队目标
	*/
	virtual void SetSquadTarget(UInt32 squadId, const TeamCopyTargetCfg* targetCfg);

	/**
	*@brief 添加据点
	*/
	virtual bool AddField(const TeamCopyFieldDataEntry* fieldEntry);

	/**
	*@brief 据点比赛结束
	*/
	virtual void FieldRaceEnd(UInt32 fieldId);

	/**
	*@brief boss阶段通知
	*/
	virtual void BossPhaseNotify(TCSPlayer* player);

	/**
	*@brief 据点通知
	*/
	virtual void FieldNotify(const std::vector<UInt32>& filedVec);
	virtual void FieldNotify(UInt32 fieldId);

	/**
	*@brief 通知战斗能量恢复进度
	*/
	virtual void NotifyRaceEnergyRevive(UInt64 raceId);

	/**
	*@brief 上报阶段boss信息
	*/
	virtual void PhaseBossInfo(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate);

	/**
	*@brief 条件检查完成
	*/
	virtual void CheckCondFinish();

	/**
	*@brief 删掉boss衍生据点
	*/
	virtual void DelBossAttachField();

	/**
	*@brief 通知所有小队退出地下城
	*/
	virtual void NotifyAllSquadLeaveDungeon();

	/**
	*@brief 通知所有小队地下城被歼灭
	*/
	virtual void NotifyAllSquadDungeonDestory(UInt32 dungeonId);

	/**
	*@brief 查找据点
	*/
	virtual TeamCopyField* FindField(UInt32 fieldId);

	/**
	*@brief 队伍是否满
	*/
	virtual bool TeamIsFull();

	/**
	*@brief 创建比赛返回
	*/
	virtual void StartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res);

	/**
	*@brief 比赛结束
	*/
	virtual void RaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime);

	/**
	*@brief Gm通关
	*/
	virtual void GMClearAllField(UInt32 squadId);

	/**
	*@brief 游戏结束
	*/
	virtual void GameOver(UInt32 overReason);

	/**
	*@brief 阶段Udp日志
	*/
	virtual void UdpStageEnd(UInt32 costTime, UInt32 result);

	/**
	*@brief 阶段boss结束
	*/
	void PhaseBossEnd();

private:

	/**
	*@brief 翻牌奖励
	*/
	void _FlopReward(UInt32 stage);

private:

	// 团队目标
	TeamCopyTarget m_TeamTarget;

	// 小队难度(key->小队id, val->难度)
	std::map<UInt32, UInt32> m_SquadGrade;

	// 据点列表
	std::map<UInt32, TeamCopyField> m_FieldMap;

	// boss阶段
	TeamCopyBossPhase m_BossPhase;

	// 1秒，能量恢复定时器
	Avalon::SimpleTimer m_TickTimers;
};


#endif // !_TCS_TEAM_ONE_H_

