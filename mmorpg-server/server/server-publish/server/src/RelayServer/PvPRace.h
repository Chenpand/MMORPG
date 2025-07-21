#ifndef __PVP_RACE_H__
#define __PVP_RACE_H__

#include "Race.h"
#include <CLMatchDefine.h>
#include <CLReplayDefine.h>

struct ReplayInfo;
class PvPRace : public Race
{
    AVALON_DEFINE_OBJECTPOOL(PvPRace, Avalon::NullMutex)
public:
	PvPRace(RaceType raceType, RaceInstanceType subType = RACE_PVP);
    virtual ~PvPRace();

	/**
	*@brief pk类型
	*/
	PkType GetPkType() const { return m_pkType; }
	void SetPkType(PkType type) { m_pkType = type; }

	/**
	*@brief pk场景
	*/
	UInt32 GetPkDungeonID() const { return m_pkDungeonId; }
	void SetPkDungeonID(UInt32 id) { m_pkDungeonId = id; }

	/**
	*@brief 战斗标志
	*/
	UInt64 GetBattleFlag() const { return m_battleFlag; }
	void SetBattleFlag(UInt64 flag) { m_battleFlag = flag; }

	/**
	*@brief 战斗玩家信息
	*/
	void SetRacePlayerInfo(std::vector<RacePlayerInfo>& infoes) { m_racePlayerInfoes = infoes; }
	const std::vector<RacePlayerInfo>& GetRacePlayerInfo() const { return m_racePlayerInfoes; }

    /**
    *@brief 注册状态定时器
    */
    virtual void RegisterStatusTimers();

	/**
	*@brief 比赛最多持续时间(s)
	*/
	virtual UInt32 GetRaceMaxTime() { return 10 * 60; }

	/**
	*@brief 录像评分
	*/
	UInt32 GetReplayScore() { return m_replayScore; }

	/**
	*@brief 生成录像文件
	*/
	ReplayFile GenReplayFile();

public:
    /**
    *@brief 加载完成
    */
    virtual void OnLoadFinish();

    /**
    *@brief 战斗结算
    */
    virtual void OnRaceEnd();

    /**
    *@brief 发送结算消息到MatchServer
    */
    virtual void SendRaceEndToMatchServer(EGameSessionDismissType reason);

    /**
    *@brief 发送结束消息给玩家
    */
    virtual void SendRaceEndToPlayer(EGameSessionDismissType reason);

	/**
	*@brief 检测结算合法性
	*/
	virtual void CheckRaceEnd(EGameSessionDismissType reason);

	/**
	*@brief 解散
	*/
	virtual bool Dismiss(EGameSessionDismissType reason);

public:
    /**
    *@brief 收到玩家的结算消息
    */
    void OnRecvRaceEndMsg(RLPlayer* player, CLProtocol::RelaySvrEndGameReq& req);

protected:
    // 各状态超时处理函数

	/**
	*@brief 是否需要验证
	*/
	virtual bool _IsNeedVerify();

    /**
    *@brief 登录超时
    */
    virtual void _OnLoginTimeout();

	/**
	*@brief 加载超时
	*/
	virtual void _OnLoadTimeout();

    /**
    *@brief 结算超时
    */
	virtual void _OnRaceEndTimeout();

	/**
	*@brief 等待玩家重连超时
	*/
	virtual void _OnWaitReconnectTimeout();

	/**
	*@brief 玩家断线重连超时
	*/
	virtual void OnGamerReconnectTimeout(ObjID_t id);

protected:
    /**
    *@brief 获取对手信息
    */
    RaceGamer* _GetOpponent(ObjID_t gamerId);

	/**
	*@brief 获取录像信息
	*/
	virtual bool _GetReplayInfo(ReplayInfo& info);

	/**
	*@brief 获取比赛结果
	*/
	PkRaceResult _GetPkRaceResultByFlag(UInt8 flag);

	/**
	*@brief 是否需要解散
	*/
	virtual bool _IsNeedDismiss();

protected:
	// PK类型
	PkType						m_pkType;
	// pk场景id
	UInt32						m_pkDungeonId;
	// 战斗标记
	UInt64						m_battleFlag;
	// 所有玩家的战斗信息
	std::vector<RacePlayerInfo>	m_racePlayerInfoes;
	// 录像评分
	UInt32						m_replayScore;
};

#endif