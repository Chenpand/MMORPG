#ifndef __PREMIUM_LEAGUE_H__
#define __PREMIUM_LEAGUE_H__

#include <functional>
#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLPremiumLeagueDefine.h>
#include <CLGameSessionProtocol.h>
#include <AvalonSimpleTimer.h>
#include <AvalonDailyTimer.h>

#include "PremiumLeagueGamer.h"
#include "BattleRecordMgr.h"
#include "PremiumLeagueBattle.h"

#define PARAM_NAME_PREMIUM_LEAGUE_STATUS "pl_status"

// 赏金联赛时间点
struct PremiumLeaguePoint
{
	// 状态
	PremiumLeagueStatus			status;
	// 开始时间
	UInt32						startTime;
	// 结束时间
	UInt32						endTime;
};

typedef std::function<void()> PremiumLeaguePointFunc;
// 赏金联赛回调
struct PremiumLeagueStatusCallback
{
	PremiumLeaguePointFunc		onStart;
	PremiumLeaguePointFunc		onEnd;
};
typedef std::map<PremiumLeagueStatus, PremiumLeagueStatusCallback> MapPremiumLeagueStatusCallback;

class PremiumLeagueMgr : public Avalon::Singleton<PremiumLeagueMgr>
{
public:
	// 淘汰赛玩家数量
	static const UInt32 FINAL_BATTLE_GAMER_NUM = 8;

	PremiumLeagueMgr();
	~PremiumLeagueMgr();

	/**
	*@brief 初始化
	*/
	bool OnInit();

	/**
	*@brief 加载数据完成
	*/
	void OnLoadFinish();

	/**
	*@brief 到了新的一天
	*/
	void OnDayChanged();

	/**
	*@brief 心跳
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 退出
	*/
	void OnFinal();

public:
	// 数据库返回

	/**
	*@brief 加载参赛成员信息返回
	*/
	bool OnSelectGamerRet(CLRecordCallback* callback);

	/**
	*@brief 加载淘汰赛信息返回
	*/
	bool OnSelectBattleRet(CLRecordCallback* callback);

public:
	/**
	*@brief 状态
	*/
	PremiumLeagueStatus GetStatus() const { return m_Status; }
	void SetStatus(PremiumLeagueStatus status) { m_Status = status; }
	PremiumLeagueStatusInfo GetStatusInfo() const;

	/**
	*@brief 战斗记录
	*/
	BattleRecordMgr& GetBattleRecordMgr() { return m_BattleRecord; }

	/**
	*@brief 获取奖金池
	*/
	UInt32 GetRewardPool();

	/**
	*@brief 同步奖金池，如果player是空就是广播给所有玩家
	*/
	void SyncRewardPool(WSPlayer* player);

	/**
	*@brief 获取的预选赛奖金总数
	*/
	UInt32 GetTotalPreliminayRewardNum() const { return m_TotalPreliminayRewardNum; }
	void SetTotalPreliminayRewardNum(UInt32 num) { m_TotalPreliminayRewardNum = num; }
	void AddTotalPreliminayRewardNum(UInt32 num) { m_TotalPreliminayRewardNum = m_TotalPreliminayRewardNum + num; }

	/**
	*@brief 获取8强玩家
	*/
	std::vector<PremiumLeagueBattleGamer> GetFinalBattleGamer() const;

	/**
	*@brief 广播给所有玩家
	*/
	// 这里需要支持下发送Packet
	// ToDo...
	void Broadcast(Avalon::Protocol& protocol);

public:
	// 玩家操作

	/**
	*@brief 玩家上线
	*/
	void OnPlayerOnline(WSPlayer* player);

	/**
	@brief 玩家上线
	*/
	void OnPlayerOffline(WSPlayer* player);

	/**
	*@brief 报名
	*/
	UInt32 OnPlayerEnroll(WSPlayer* player);

	/**
	*@brief 玩家重新报名
	*/
	UInt32 OnPlayerReEnroll(WSPlayer* player);

	/**
	*@brief 玩家请求开始预算赛
	*/
	UInt32 OnPlayerStartPreliminay(WSPlayer* player);

public:
	// 战斗结果

	/**
	*@brief 预选赛结算
	*/
	void OnPreliminayRaceEnd(CLProtocol::MatchServerPremiumLeagueRaceEnd raceEnd);

	/**
	*@brief 淘汰赛
	*/
	void OnBattleRaceEnd(CLProtocol::MatchServerPremiumLeagueRaceEnd raceEnd);

	/**
	 *@brief 发送荣誉到scene
	 */
	void SendHonorToScene(UInt64 roleId, UInt32 pvpType, UInt32 honor);

public:
	// 事务操作
	/**
	*@brief 报名
	*/
	UInt32 OnCheckPlayerEnrollRet(WSPlayer* player);

	/**
	*@brief 重新报名
	*/
	UInt32 OnCheckPlayerReEnrollRet(WSPlayer* player);

public:
	/**
	*@brief 切换状态
	*/
	void ChangeStatus(PremiumLeagueStatus status);

public:
	/**
	*@brief 参赛成员相关
	*/
	PremiumLeagueGamer* FindGamer(ObjID_t id);
	void AddGamer(PremiumLeagueGamer* gamer);
	void DelGamer(ObjID_t id);

private:
	// 各种状态的回调

	/**
	*@brief 注册状态回调
	*/
	void _RegisterStatusCallback(PremiumLeagueStatus status, PremiumLeaguePointFunc onStartFunc, PremiumLeaguePointFunc onEndFunc);

	/**
	*@brief 无效的回调
	*/
	void _OnNousefulStatusCallback();

	/**
	*@brief 报名开始
	*/
	void _OnEnrollStart();

	/**
	*@brief 报名结束
	*/
	void _OnEnrollEnd();

	/**
	*@brief 预选赛开始
	*/
	void _OnPreliminayStart();

	/**
	*@brief 预选赛结束
	*/
	void _OnPreliminayEnd();

	/**
	*@brief 8强开始
	*/
	void _OnFinalEigthStart();

	/**
	*@brief 8强结束
	*/
	void _OnFinalEigthEnd();

	/**
	*@brief 4强开始
	*/
	void _OnFinalFourStart();

	/**
	*@brief 4强开始
	*/
	void _OnFinalFourEnd();

	/**
	*@brief 决赛开始
	*/
	void _OnFinalBattleStart();

	/**
	*@brief 决赛结束
	*/
	void _OnFinalBattleEnd();

	/**
	*@brief 所有状态都结束了
	*/
	void _OnAllOver();

public:
	// 淘汰赛战斗相关
	ObjID_t AddNewBattle(PremiumLeagueGamer* gamer1, PremiumLeagueGamer* gamer2);
	void AddBattle(const PremiumLeagueBattle& battle);
	PremiumLeagueBattle* FindBattleByGuid(ObjID_t guid);
	PremiumLeagueBattle* FindBattleByRoleAndStatus(ObjID_t roleId, PremiumLeagueStatus status);
	void ClearBattle();

private:
	/**
	*@brief 计算下一轮的参赛人员
	*/
	void _CountNextRollGamer(UInt32 maxNum);

	/**
	*@brief 开始当前轮的战斗
	*/
	void _StartBattle(UInt32 maxNum);

	/**
	*@brief 是否已经完成当前轮的所有战斗
	*/
	bool _IsFinishCurrentRollBattle();

	/**
	*@brief 发送下一轮准备的公告
	*/
	void _SendNextRollPrepareAnnouncement();

private:
	/**
	*@brief 计算各个时间点
	*/
	void _CountStatusPoint();

	/**
	*@brief 计算当前时间点
	*/
	PremiumLeagueStatus _CountCurrentPoint();

	/**
	*@brief 玩家能否报名
	*/
	UInt32 _CheckPlayerCanEnroll(WSPlayer* player);

	/**
	*@brief 玩家能否重新报名
	*/
	UInt32 _CheckPlayerCanReEnroll(WSPlayer* player);

	/**
	*@brief 清除所有记录
	*/
	void _Clear();

	/**
	*@brief 清除所有参赛者
	*/
	void _ClearGamers();

	/**
	*@brief 根据排名，获取8强位置
	*/
	UInt32 _GetFinalEightPosByRanking(UInt32 ranking);

	/**
	*@brief 玩家能否参加比赛
	*/
	bool _CanPlayerStartBattle(ObjID_t roleId);

	/**
	*@brief 根据排名发放奖励
	*/
	void _SendReward(PremiumLeagueGamer* gamer, PremiumLeagueRewardType type);

	/**
	*@brief 根据排名发放奖励
	*/
	UInt32 _GetReward(PremiumLeagueRewardType type);

	/**
	*@brief 计算获取的积分
	*/
	UInt32 _CountWinGotScore(UInt32 winnerWinNum, UInt32 loserWinNum);

	/**
	*@brief 计算获取的积分
	*/
	UInt32 _CountLoseGotScore();

	/**
	 *@brief 荣誉奖励结算
	 */
	void _SendHonorReward(UInt32 pvpType, UInt64 roleId, UInt32 rank);

private:
	// 是否加载完成
	bool										m_LoadFinish;
	// 当前状态
	PremiumLeagueStatus							m_Status;
	// 各个时间点
	PremiumLeaguePoint							m_TimePoints[PLS_NUM];
	// 状态回调
	MapPremiumLeagueStatusCallback				m_StatusCallbacks;
	// 所有报名的人
	std::map<ObjID_t, PremiumLeagueGamer*>		m_Gamers;
	// 8强玩家
	PremiumLeagueGamer*							m_FinalBattleGamer[FINAL_BATTLE_GAMER_NUM];
	// 保存定时器
	Avalon::SimpleTimer							m_SaveTimer;
	// 战斗记录
	BattleRecordMgr								m_BattleRecord;
	// 淘汰赛战斗
	std::vector<PremiumLeagueBattle>			m_PremiumBattles;
	// 每日定时器
	Avalon::DailyTimer							m_DailyTimer;
	// 已经分配出去的预选赛奖金
	UInt32										m_TotalPreliminayRewardNum;
}; 

#define REGISTER_STATUS_CALLBACK(status, startFunc, endFunc)		\
		do {														\
			PremiumLeaguePointFunc _startFunc;						\
			if (startFunc)											\
			{														\
				_startFunc = std::bind(startFunc, this);            \
			}														\
			PremiumLeaguePointFunc _endFunc;						\
if (endFunc)											\
{														\
	_endFunc = std::bind(endFunc, this);            \
}\
			_RegisterStatusCallback(status, _startFunc, _endFunc);  \
		} while (0)

#endif