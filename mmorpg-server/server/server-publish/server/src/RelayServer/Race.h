#ifndef __RACE_H__
#define __RACE_H__

#include "RaceDefine.h"
#include "RaceGamer.h"
#include <AvalonObjectPool.h>
#include <CLMatchDefine.h>

class Race
{
	// 加载完成时的进度
	const UInt8 FULL_LOAD_PROGRESS = 100;
public:
    Race(RaceType type, RaceInstanceType subType);
    virtual ~Race();

    virtual void Init();

    virtual void OnTick(const Avalon::Time& now);
    /**
    *@brief 注册状态定时器
    */
    virtual void RegisterStatusTimers();

public:
    /**
    *@brief 等待登陆完成
    */
    virtual void OnTickWaitLoginFinish(const Avalon::Time& now);

    /**
    *@brief 等待登陆完成
    */
    virtual void OnTickWaitLoadFinish(const Avalon::Time& now);

    /**
    *@brief 战斗时更新
    */
    virtual void OnTickRacing(const Avalon::Time& now);

    /**
    *@brief 更新帧数据
    */
    virtual void OnTickFrames(const Avalon::Time& now);

    /**
    *@brief 等待结算
    */
    virtual void OnTickWaitRaceEnd(const Avalon::Time& now);

	/**
	*@brief 重连
	*/
	virtual void OnTickReconnecting(const Avalon::Time& now);

	/**
	*@brief 观战循环
	*/
	virtual void OnTickObserving(const Avalon::Time& now);

    /**
    *@brief 不同步
    */
    virtual void OnChecksumUnsync();

	/**
	*@brief 玩家断线重连超时
	*/
	virtual void OnGamerReconnectTimeout(ObjID_t id);

	/**
	*@brief 比赛超时（超过规定的最大时间）
	*/
	virtual void OnRaceTimeout();

	/**
	*@brief 创建验证任务返回
	*/
	virtual void OnCreateVerifyReturn(UInt32 nodeid);

public:
	// 观战相关
	
	/**
	*@brief 玩家开始观战
	*/
	virtual UInt32 OnObserverLogin(RLPlayer* player, UInt32 startFrame);

	/**
	*@brief 玩家观战重连
	*/
	virtual UInt32 OnObserverReconnect(RLPlayer* player, UInt32 lastFrame);

	/**
	*@brief 观战者断开连接
	*/
	virtual void OnObserverDisconnect(ObjID_t roleId);

	/**
	*@brief 是否可以开始观战
	*/
	virtual bool CanObserve() const;

	/**
	*@brief 获取观战延迟(帧数)
	*/
	virtual UInt32 GetObserveDelay() const;

	/**
	*@brief 收到observe同步数据帧
	*/
	virtual void OnReceiveObserveFrames(std::vector<CLProtocol::Frame>& frames);

	/**
	*@brief 获取本场比赛观战定时器负载数量
	*/
	virtual UInt32 GetObserveLoad();

public:
    // 必须实现的接口

    /**
    *@brief 登陆完成
    */
    virtual void OnLoginFinish();

    /**
    *@brief 加载完成
    */
    virtual void OnLoadFinish();

    /**
    *@brief 战斗结算
    */
    virtual void OnRaceEnd() = 0;

    /**
    *@brief 发送结算消息到MatchServer
    */
    virtual void SendRaceEndToMatchServer(EGameSessionDismissType reason) = 0;

    /**
    *@brief 发送结束消息给玩家
    */
    virtual void SendRaceEndToPlayer(EGameSessionDismissType reason);

	/**
	*@brief 检测结算合法性
	*/
	virtual void CheckRaceEnd(EGameSessionDismissType reason) = 0;

	/**
	*@brief 比赛最多持续时间
	*/
	virtual UInt32 GetRaceMaxTime() = 0;

public:
    // 玩家事件

	/**
	*@brief 玩家上报加载进度
	*/
	virtual bool OnGamerReportLoadProgress(RLPlayer* player, UInt8 progress);

    /**
    *@brief 玩家登陆
    */
    virtual bool OnGamerLogin(RLPlayer* player);

	/**
	*@brief 玩家重连
	*/
	virtual bool OnGamerReconnect(RLPlayer* player, UInt32 lastFrame);

	/**
	*@brief 玩家离开比赛
	*/
	virtual void OnGamerLeaveRace(ObjID_t roleId, GamerLeaveReason reason);

	/**
	*@brief 玩家退出比赛
	*/
	virtual void OnGamerQuiteRace(ObjID_t roleId);

    /**
    *@brief 玩家加载完成
    */
    virtual bool OnGamerLoadFinish(RLPlayer* player);

    /**
    *@brief 收到玩家的帧数据
    */
    virtual void OnRecvGamerInputData(RLPlayer* player, CLProtocol::InputData& input);

    /**
    *@brief 收到玩家的校验帧
    */
    virtual void OnRecvGamerFrameChecksum(RLPlayer* player, CLProtocol::FrameChecksum& checksum);

	/**
	*@brief 收到玩家的ping值
	*/
	virtual void OnRecvGamerPing(RLPlayer* player, UInt32 ping);

    /**
    *@brief 这里只处理收到结算消息之后做的事
    */
    virtual void OnGamerRaceEnd(RLPlayer* player);

	/**
	*@brief 升级
	*/
	virtual void OnGamerLevelUp(ObjID_t roleId, UInt16 level);

	/**
	*@brief 复活
	*/
	virtual void OnGamerReviveOther(ObjID_t roleId, ObjID_t targetId, UInt64 hpMp);

    /**
    *@brief 解散
    */
    virtual bool Dismiss(EGameSessionDismissType reason);

	/*
	*@brief 据点被歼灭
	*/
	virtual void OnNotifyDestoryDungeon(ObjID_t destoryDungeonId) {}

	/**
	*@brief 逻辑结束比赛
	*/
	virtual void OnLogicEndDungeon(UInt32 reason) {}

	/**
	*@brief 团本通知地下城贝希摩斯之心能量蓄积进度
	*/
	virtual void OnTeamCopyNotifyBimsEnergyProgress(UInt32 progress, UInt32 dungeonId) {}
	
	/**
	*@brief 使用道具
	*/
	virtual void OnUseItem(ObjID_t roleId, UInt32 itemId, UInt32 num);

	/**
	*@brief 团本增加复活次数
	*/
	virtual void OnAddReviveCnt(UInt32 reviveCnt) {}

public:
    /**
    *@brief ID
    */
    inline ObjID_t GetID() const { return m_id; };
    inline void SetID(ObjID_t id) { m_id = id; }

	/**
	*@brief 比赛类型
	*/
	RaceType GetRaceType() const { return m_raceType; }

	/**
	*@brief 战斗类型
	*/
	RaceInstanceType GetType() const { return m_type; }

    /**
    *@brief 状态
    */
    inline RaceStatus GetStatus() const { return m_status; }
    void ChangeStatus(RaceStatus status);
	inline RaceStatus GetLastStatus() const { return m_lastStatus; }

    /**
    *@brief 开始时间
    */
    inline void SetStartTime(Avalon::Time now) { m_startTime = now; }
	inline UInt64 GetStartTime() const { return m_startTime.Sec(); }

    /**
    *@brief 玩家
    */
    RaceGamer* GetGamerByPos(UInt8 pos);
    RaceGamer* GetGamerByRoleID(ObjID_t roleId);
    void SetGamerByPos(UInt8 pos, RaceGamer& gamer);
    bool AddGamer(UInt32 accId, ObjID_t roleId, UInt8 pos, UInt32 wsId);

	/**
	*@brief 获取解散时间
	*/
	UInt32 GetDestoryTime() const { return m_destoryTime; }

	/**
	*@brief PK结算
	*/
	const PkRaceEndInfo& GetPkRaceEndInfo() const { return m_pkRaceEndInfo; }
	void SetPkRaceEndInfo(const PkRaceEndInfo& info) { m_pkRaceEndInfo = info; }

	/**
	*@brief 地下城结算
	*/
	const DungeonRaceEndInfo& GetDungeonRaceEndInfo() const { return m_dungeonEndInfo; }
	void SetDungeonRaceEndInfo(const DungeonRaceEndInfo& info) { m_dungeonEndInfo = info; }

	/**
	*@brief 初始化包
	*/
	void SetInitPacket(Avalon::Packet* packet) { m_InitPacket = packet; }
	Avalon::Packet* GetInitPacket() const { return m_InitPacket; }
	virtual void OnRecvInitPacket() {}


	/**
	*@brief 设置所有帧
	*/
	void SetAllFrames(const std::vector<CLProtocol::Frame>& frames) { m_allFrames = frames; }
	std::vector<CLProtocol::Frame>& GetAllFrames() { return m_allFrames; }
	

    /**
    *@brief 是否是有效的玩家
    */
    virtual bool IsValidGamer(RLPlayer* player);

    /**
    *@brief 是否所有玩家都离开比赛
    */
    bool IsAllGamerLeave();

	/**
	*@brief 广播提示
	*/
	template <typename... Args>
	void BroadcastNotify(UInt32 id, bool disconnect, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);
		if (!ret) return;

		SendNotify(type, color, buffer, disconnect);
	}

	void SendNotify(UInt16 type, UInt8 color, const char* word, bool disconnect);

	/**
	*@brief 获取观战者信息
	*/
	RaceGamer* GetObserverByRoleID(ObjID_t id);
	UInt32 GetObserverNum() const { return m_observers.size(); }

	/**
	*@brief 随机获取一个玩家的WSID
	*/
	UInt32 RandGetWSID() const;

	/**
	*@brief 设置验证服务器结算结果
	*/
	void SetVerifyRaceEndPacket(Avalon::Packet* packet) { m_VerifyRaceEndPacket = packet; }

	/**
	*@brief 收到Observe请求开始同步帧
	*/
	virtual void OnReceiveObserveStartSync();

	/**
	*@brief 收到Observe请求开始同步帧
	*/
	virtual void OnObserveRealyAddObservePlayer(UInt64 roleID);
	

protected:
	// 各状态超时处理函数

	/**
	*@brief 加载超时
	*/
	virtual void _OnLoadTimeout();

protected:
	// 验证相关
	
	/**
	*@brief 是否正在验证
	*/
	bool _IsVerifying() const { return m_VSId > 0; };

	/**
	*@brief 请求创建验证任务
	*/
	void _CreateVerifyTask();

	/**
	*@brief 是否需要验证
	*/
	virtual bool _IsNeedVerify() { return false; }

	/**
	*@brief 开始验证
	*/
	void _StartVerify();

	/**
	*@brief 发送消息到验证服务器
	*/
	void _SendToVerifyServer(Avalon::Protocol& protocol);

	/**
	*@brief 获取验证服务器结算结果
	*/
	Avalon::Packet* GetVerifyRaceEndPacket() { return m_VerifyRaceEndPacket; }

protected:
    /**
    *@brief 更新状态定时器
    */
    void _OnTickStatusTimer(const Avalon::Time& now);

    /**
    *@brief 注册状态定时器
    */
    void _RegisterStatusTimer(RaceStatus status, UInt32 waitSec, RaceStatusTimer::RaceStatusTimeOutFunc func);

    /**
    *@brief 激活当前状态的定时器
    */
    void _ActiveStatusTimer(RaceStatus oldStatus, RaceStatus newStatus);

    /**
    *@brief 激活当前状态的定时器
    */
    RaceStatusTimer* _GetStatusTimer(RaceStatus status);

    /**
    *@brief 是否是有效的位置
    */
    bool _IsValidPos(UInt8 pos);

    /**
    *@brief 是否所有玩家都登陆成功
    */
    bool _IsAllGamerLogin();

    /**
    *@brief 是否所有玩家都加载成功
    */
    bool _IsAllGamerLoadFinish();

    /**
    *@brief 是否收到所有玩家的结算
    */
    virtual bool _IsRecvAllRaceEnd();

    /**
    *@brief 检测所有玩家是不是在同一状态
    */
    bool _IsAllGamerInSameStatus(RaceGamerStatus status, bool isGamerMustInRace = true);

    /**
    *@brief 收到所有玩家的校验帧
    */
    bool _IsRecvAllGamerFrameChecksum(UInt32 frame);

    /**
    *@brief 开始校验帧数据
    */
    virtual void _OnFrameChecksum(UInt32 frame);

    /**
    *@brief 修改所有玩家状态
    */
    void _ChangeAllGamerStatus(RaceGamerStatus status, bool isGamerMustInRace);

    /**
    *@brief 广播比赛事件给所有玩家
    */
    void _BroadcastEvent(const GameSessionEvent& event);

    /**
    *@brief 添加当前帧的操作
    */
    void _PushInputDataToCurrentFrame(UInt8 pos, const CLProtocol::InputData& data, bool push_front = false);

    /**
    *@brief 当前帧构建完成
    */
    void _FinishCurrentFrame(const Avalon::Time& now);

    /**
    *@brief 发送帧数据给所有玩家
    */
    void _SyncFrames();

	/**
	*@brief 当发送帧数据给所有玩家时
	*/
	virtual void _OnSyncFrames(std::vector<CLProtocol::Frame>& unSyncFrame);

    /**
    *@brief 广播消息包给客户端
    */
    void _BroadcastProtocol(Avalon::Protocol& protocol, bool onlyRacingGamer = true, bool disconnect = false);
	void _BroadcastPacket(Avalon::Packet* packet, bool onlyRacingGamer = true, bool disconnect = false);

	/**
	*@brief 获取状态字符串
	*/
    const char* _ShowStatus(RaceStatus status);

	/**
	*@brief 获取状态字符串
	*/
	_gameSessionDismissType _GetDismissTypeByLeaveReason(GamerLeaveReason reason);

	/**
	*@brief 根据起始帧数发送帧数据
	*@return 返回true代表后面还有需要发的帧，false代表已经发到最新的了
	*/
	bool _SendFrames(RaceGamer* gamer, UInt32 startFrame);

	/**
	*@brief 是不是在可重连状态下
	*/
	bool _IsStatusCanReconnect();

	/**
	*@brief 根据ping值获取信号强度
	*/
	UInt8 _GetNetworkLevelByPing(UInt32 ping);

	/**
	*@brief 强行广播一个命令帧
	*/
	void _BroadcastFrame(UInt8 pos, FrameCommandType type, UInt32 param);

	/**
	*@brief 检测到不同步时是否结束战斗
	*/
	virtual bool _IsDismissWhenUnsync() { return false; }

	/**
	*@brief  处理一个InputData
	*/
	virtual bool _HandleOneInputData(RLPlayer* player, CLProtocol::InputData& input);

	/**
	*@brief  处理玩家随机数不一致
	*/
	virtual void _OnPlayerChecksumUnsync(RaceGamer& gamer, UInt32 frame, UInt32 finalChecksum);
protected:
	// 比赛类型
	RaceType							m_raceType;

    // 战斗实例类型
    RaceInstanceType                    m_type;

    // ID
    ObjID_t                             m_id;

    // 之前的状态
	RaceStatus							m_lastStatus;

	// 当前状态
    RaceStatus                          m_status;

	// 删除时间
	UInt32								m_destoryTime;

    // 各状态定时器
    RaceStatusTimer                     m_statusTimer[RACE_STATUS_NUM];

	// 比赛创建时间
	UInt32								m_createTime;

    // 对战开始时间
    Avalon::Time						m_startTime;

	// 结算时间
	Avalon::Time						m_raceEndTime;

    // 所有参赛玩家
    std::vector<RaceGamer>              m_gamers;

	// 观战玩家
	std::vector<RaceGamer>				m_observers;

    // 帧相关
    //当前帧（正在组帧中）
    CLProtocol::Frame						m_currentFrame;	

    //未同步的帧（逻辑帧频和物理帧频不一样时）
    std::vector<CLProtocol::Frame>		m_unsyncFrames;

    //帧数据（已经完成的帧，并已同步给参战双方）
    std::vector<CLProtocol::Frame>		m_framesData;
	//所有的帧
	std::vector<CLProtocol::Frame>		m_allFrames;

    UInt32								m_nextRealFrame;
    UInt32								m_nextLogicFrameSequence;

    //帧同步校验相关
    //已经校验过了的帧
    UInt32                              m_checkedSyncFrame;

	// 所有校验数据
	std::vector<RaceChecksumInfo>		m_checksumInfoes;

	// 各种结算
	// pk结算
	PkRaceEndInfo						m_pkRaceEndInfo;
	// 地下城结算
	DungeonRaceEndInfo					m_dungeonEndInfo;

	// 是否已经不同步
	bool								m_isUnsync;

	// 断线重连次数
	UInt32								m_reconnNum;

	// 复活次数
	UInt32								m_reviveNum;

	// 升级次数
	UInt32								m_levelUpNum;

	// 不同步发生的帧
	UInt32								m_firstUnsyncFrame;

	// 验证相关
	// 战斗初始化包
	Avalon::Packet*						m_InitPacket;
	// 创建验证任务开始时间
	UInt32								m_StartVerifyTime;
	// 验证服务器ID
	UInt32								m_VSId;
	// 验证服务器上传的结算
	Avalon::Packet*						m_VerifyRaceEndPacket;
};

#define REGISTER_STATUS_TIMER(status, timeout, func)    \
    do {                                                \
        RaceStatusTimer::RaceStatusTimeOutFunc _func;   \
        _func = std::bind(func, this);                  \
        _RegisterStatusTimer(status, timeout, _func);   \
    } while(0)

#endif