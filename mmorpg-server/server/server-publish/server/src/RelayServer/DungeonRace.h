#ifndef __DUNGEON_RACE_H__
#define __DUNGEON_RACE_H__

#include "Race.h"

class DungeonRace : public Race
{
    AVALON_DEFINE_OBJECTPOOL(DungeonRace, Avalon::NullMutex)
public:
	DungeonRace(RaceType type);
    virtual ~DungeonRace();

    /**
    *@brief 注册状态定时器
    */
    virtual void RegisterStatusTimers();

	/**
	*@brief 比赛最多持续时间(s)
	*/
	virtual UInt32 GetRaceMaxTime() { return 60 * 60; }

	/**
	*@brief 是否使用验证服务器结算
	*/
	bool IsUseVerifyRaceEnd() const { return m_UseVerifyRaceEnd; }
	void UseVerifyRaceEnd() { m_UseVerifyRaceEnd = true; }

public:
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
	*@brief 解散
	*/
	virtual bool Dismiss(EGameSessionDismissType reason);

	/**
	*@brief 检测结算合法性
	*/
	virtual void CheckRaceEnd(EGameSessionDismissType reason);

	/**
	*@brief 收到初始化包之后的处理
	*/
	virtual void OnRecvInitPacket();

public:
    /**
    *@brief 收到玩家的结算消息
    */
    void OnRecvRaceEndMsg(RLPlayer* player, CLProtocol::RelaySvrDungeonRaceEndReq& req);

private:
    // 各状态超时处理函数

    /**
    *@brief 登录超时
    */
    void _OnLoginTimeout();

	/**
	*@brief 等待玩家重连超时
	*/
	void _OnWaitReconnectTimeout();

	/**
	*@brief 等待玩家退出
	*/
	void _OnWiatGamerLeaveRaceTimeout();

	/**
	*@brief 等待玩家退出
	*/
	void _OnWiatRaceEnd();
private:
	/**
	*@brief 是否需要验证
	*/
	virtual bool _IsNeedVerify();

	/**
	*@brief 是否crossDungeon
	*/
	bool _IsCrossDungeon();

	/**
	*@brief 是否收到所有的结算
	*/
	virtual bool _IsRecvAllRaceEnd();

protected:
	/**
	*@brief  处理一个InputData
	*/
	bool _HandleOneInputData(RLPlayer* player, CLProtocol::InputData& input);

	/**
	*@brief  处理关卡boss阶段变化
	*/
	void _OnBossPhaseChange(UInt32 bossPhase);

	/**
	*@brief 通知据点被歼灭
	*/
	void OnNotifyDestoryDungeon(ObjID_t destoryDungeonId);

	/**
	*@brief 逻辑结束比赛
	*/
	void OnLogicEndDungeon(UInt32 reason);

	/**
	*@brief 开始校验帧数据
	*/
	virtual void _OnFrameChecksum(UInt32 frame);

	/**
	*@brief  处理玩家随机数不一致
	*/
	virtual void _OnPlayerChecksumUnsync(RaceGamer& gamer, UInt32 frame, UInt32 finalChecksum);

	/**
	*@brief  团本通知地下城贝希摩斯之心能量蓄积进度
	*/
	virtual void OnTeamCopyNotifyBimsEnergyProgress(UInt32 progress, UInt32 dungeonId);

	/**
	*@brief  团本增加复活次数
	*/
	virtual void OnAddReviveCnt(UInt32 reviveCnt);

private:
	// 地下城ID
	UInt32				m_DungeonId;
	// boss阶段
	UInt32				m_BossPhase; 
	// 开始等待结算时间
	UInt32				m_StartWaitVerifyRaceEndTime;
	// 是否使用验证服务器的结算
	bool				m_UseVerifyRaceEnd;
};

#endif