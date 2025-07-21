#ifndef __RACE_H__
#define __RACE_H__

#include "RaceDefine.h"
#include "RaceGamer.h"
#include <AvalonObjectPool.h>
#include <CLMatchDefine.h>

class Race
{
	// �������ʱ�Ľ���
	const UInt8 FULL_LOAD_PROGRESS = 100;
public:
    Race(RaceType type, RaceInstanceType subType);
    virtual ~Race();

    virtual void Init();

    virtual void OnTick(const Avalon::Time& now);
    /**
    *@brief ע��״̬��ʱ��
    */
    virtual void RegisterStatusTimers();

public:
    /**
    *@brief �ȴ���½���
    */
    virtual void OnTickWaitLoginFinish(const Avalon::Time& now);

    /**
    *@brief �ȴ���½���
    */
    virtual void OnTickWaitLoadFinish(const Avalon::Time& now);

    /**
    *@brief ս��ʱ����
    */
    virtual void OnTickRacing(const Avalon::Time& now);

    /**
    *@brief ����֡����
    */
    virtual void OnTickFrames(const Avalon::Time& now);

    /**
    *@brief �ȴ�����
    */
    virtual void OnTickWaitRaceEnd(const Avalon::Time& now);

	/**
	*@brief ����
	*/
	virtual void OnTickReconnecting(const Avalon::Time& now);

	/**
	*@brief ��սѭ��
	*/
	virtual void OnTickObserving(const Avalon::Time& now);

    /**
    *@brief ��ͬ��
    */
    virtual void OnChecksumUnsync();

	/**
	*@brief ��Ҷ���������ʱ
	*/
	virtual void OnGamerReconnectTimeout(ObjID_t id);

	/**
	*@brief ������ʱ�������涨�����ʱ�䣩
	*/
	virtual void OnRaceTimeout();

	/**
	*@brief ������֤���񷵻�
	*/
	virtual void OnCreateVerifyReturn(UInt32 nodeid);

public:
	// ��ս���
	
	/**
	*@brief ��ҿ�ʼ��ս
	*/
	virtual UInt32 OnObserverLogin(RLPlayer* player, UInt32 startFrame);

	/**
	*@brief ��ҹ�ս����
	*/
	virtual UInt32 OnObserverReconnect(RLPlayer* player, UInt32 lastFrame);

	/**
	*@brief ��ս�߶Ͽ�����
	*/
	virtual void OnObserverDisconnect(ObjID_t roleId);

	/**
	*@brief �Ƿ���Կ�ʼ��ս
	*/
	virtual bool CanObserve() const;

	/**
	*@brief ��ȡ��ս�ӳ�(֡��)
	*/
	virtual UInt32 GetObserveDelay() const;

	/**
	*@brief �յ�observeͬ������֡
	*/
	virtual void OnReceiveObserveFrames(std::vector<CLProtocol::Frame>& frames);

	/**
	*@brief ��ȡ����������ս��ʱ����������
	*/
	virtual UInt32 GetObserveLoad();

public:
    // ����ʵ�ֵĽӿ�

    /**
    *@brief ��½���
    */
    virtual void OnLoginFinish();

    /**
    *@brief �������
    */
    virtual void OnLoadFinish();

    /**
    *@brief ս������
    */
    virtual void OnRaceEnd() = 0;

    /**
    *@brief ���ͽ�����Ϣ��MatchServer
    */
    virtual void SendRaceEndToMatchServer(EGameSessionDismissType reason) = 0;

    /**
    *@brief ���ͽ�����Ϣ�����
    */
    virtual void SendRaceEndToPlayer(EGameSessionDismissType reason);

	/**
	*@brief ������Ϸ���
	*/
	virtual void CheckRaceEnd(EGameSessionDismissType reason) = 0;

	/**
	*@brief ����������ʱ��
	*/
	virtual UInt32 GetRaceMaxTime() = 0;

public:
    // ����¼�

	/**
	*@brief ����ϱ����ؽ���
	*/
	virtual bool OnGamerReportLoadProgress(RLPlayer* player, UInt8 progress);

    /**
    *@brief ��ҵ�½
    */
    virtual bool OnGamerLogin(RLPlayer* player);

	/**
	*@brief �������
	*/
	virtual bool OnGamerReconnect(RLPlayer* player, UInt32 lastFrame);

	/**
	*@brief ����뿪����
	*/
	virtual void OnGamerLeaveRace(ObjID_t roleId, GamerLeaveReason reason);

	/**
	*@brief ����˳�����
	*/
	virtual void OnGamerQuiteRace(ObjID_t roleId);

    /**
    *@brief ��Ҽ������
    */
    virtual bool OnGamerLoadFinish(RLPlayer* player);

    /**
    *@brief �յ���ҵ�֡����
    */
    virtual void OnRecvGamerInputData(RLPlayer* player, CLProtocol::InputData& input);

    /**
    *@brief �յ���ҵ�У��֡
    */
    virtual void OnRecvGamerFrameChecksum(RLPlayer* player, CLProtocol::FrameChecksum& checksum);

	/**
	*@brief �յ���ҵ�pingֵ
	*/
	virtual void OnRecvGamerPing(RLPlayer* player, UInt32 ping);

    /**
    *@brief ����ֻ�����յ�������Ϣ֮��������
    */
    virtual void OnGamerRaceEnd(RLPlayer* player);

	/**
	*@brief ����
	*/
	virtual void OnGamerLevelUp(ObjID_t roleId, UInt16 level);

	/**
	*@brief ����
	*/
	virtual void OnGamerReviveOther(ObjID_t roleId, ObjID_t targetId, UInt64 hpMp);

    /**
    *@brief ��ɢ
    */
    virtual bool Dismiss(EGameSessionDismissType reason);

	/*
	*@brief �ݵ㱻����
	*/
	virtual void OnNotifyDestoryDungeon(ObjID_t destoryDungeonId) {}

	/**
	*@brief �߼���������
	*/
	virtual void OnLogicEndDungeon(UInt32 reason) {}

	/**
	*@brief �ű�֪ͨ���³Ǳ�ϣĦ˹֮�������������
	*/
	virtual void OnTeamCopyNotifyBimsEnergyProgress(UInt32 progress, UInt32 dungeonId) {}
	
	/**
	*@brief ʹ�õ���
	*/
	virtual void OnUseItem(ObjID_t roleId, UInt32 itemId, UInt32 num);

	/**
	*@brief �ű����Ӹ������
	*/
	virtual void OnAddReviveCnt(UInt32 reviveCnt) {}

public:
    /**
    *@brief ID
    */
    inline ObjID_t GetID() const { return m_id; };
    inline void SetID(ObjID_t id) { m_id = id; }

	/**
	*@brief ��������
	*/
	RaceType GetRaceType() const { return m_raceType; }

	/**
	*@brief ս������
	*/
	RaceInstanceType GetType() const { return m_type; }

    /**
    *@brief ״̬
    */
    inline RaceStatus GetStatus() const { return m_status; }
    void ChangeStatus(RaceStatus status);
	inline RaceStatus GetLastStatus() const { return m_lastStatus; }

    /**
    *@brief ��ʼʱ��
    */
    inline void SetStartTime(Avalon::Time now) { m_startTime = now; }
	inline UInt64 GetStartTime() const { return m_startTime.Sec(); }

    /**
    *@brief ���
    */
    RaceGamer* GetGamerByPos(UInt8 pos);
    RaceGamer* GetGamerByRoleID(ObjID_t roleId);
    void SetGamerByPos(UInt8 pos, RaceGamer& gamer);
    bool AddGamer(UInt32 accId, ObjID_t roleId, UInt8 pos, UInt32 wsId);

	/**
	*@brief ��ȡ��ɢʱ��
	*/
	UInt32 GetDestoryTime() const { return m_destoryTime; }

	/**
	*@brief PK����
	*/
	const PkRaceEndInfo& GetPkRaceEndInfo() const { return m_pkRaceEndInfo; }
	void SetPkRaceEndInfo(const PkRaceEndInfo& info) { m_pkRaceEndInfo = info; }

	/**
	*@brief ���³ǽ���
	*/
	const DungeonRaceEndInfo& GetDungeonRaceEndInfo() const { return m_dungeonEndInfo; }
	void SetDungeonRaceEndInfo(const DungeonRaceEndInfo& info) { m_dungeonEndInfo = info; }

	/**
	*@brief ��ʼ����
	*/
	void SetInitPacket(Avalon::Packet* packet) { m_InitPacket = packet; }
	Avalon::Packet* GetInitPacket() const { return m_InitPacket; }
	virtual void OnRecvInitPacket() {}


	/**
	*@brief ��������֡
	*/
	void SetAllFrames(const std::vector<CLProtocol::Frame>& frames) { m_allFrames = frames; }
	std::vector<CLProtocol::Frame>& GetAllFrames() { return m_allFrames; }
	

    /**
    *@brief �Ƿ�����Ч�����
    */
    virtual bool IsValidGamer(RLPlayer* player);

    /**
    *@brief �Ƿ�������Ҷ��뿪����
    */
    bool IsAllGamerLeave();

	/**
	*@brief �㲥��ʾ
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
	*@brief ��ȡ��ս����Ϣ
	*/
	RaceGamer* GetObserverByRoleID(ObjID_t id);
	UInt32 GetObserverNum() const { return m_observers.size(); }

	/**
	*@brief �����ȡһ����ҵ�WSID
	*/
	UInt32 RandGetWSID() const;

	/**
	*@brief ������֤������������
	*/
	void SetVerifyRaceEndPacket(Avalon::Packet* packet) { m_VerifyRaceEndPacket = packet; }

	/**
	*@brief �յ�Observe����ʼͬ��֡
	*/
	virtual void OnReceiveObserveStartSync();

	/**
	*@brief �յ�Observe����ʼͬ��֡
	*/
	virtual void OnObserveRealyAddObservePlayer(UInt64 roleID);
	

protected:
	// ��״̬��ʱ������

	/**
	*@brief ���س�ʱ
	*/
	virtual void _OnLoadTimeout();

protected:
	// ��֤���
	
	/**
	*@brief �Ƿ�������֤
	*/
	bool _IsVerifying() const { return m_VSId > 0; };

	/**
	*@brief ���󴴽���֤����
	*/
	void _CreateVerifyTask();

	/**
	*@brief �Ƿ���Ҫ��֤
	*/
	virtual bool _IsNeedVerify() { return false; }

	/**
	*@brief ��ʼ��֤
	*/
	void _StartVerify();

	/**
	*@brief ������Ϣ����֤������
	*/
	void _SendToVerifyServer(Avalon::Protocol& protocol);

	/**
	*@brief ��ȡ��֤������������
	*/
	Avalon::Packet* GetVerifyRaceEndPacket() { return m_VerifyRaceEndPacket; }

protected:
    /**
    *@brief ����״̬��ʱ��
    */
    void _OnTickStatusTimer(const Avalon::Time& now);

    /**
    *@brief ע��״̬��ʱ��
    */
    void _RegisterStatusTimer(RaceStatus status, UInt32 waitSec, RaceStatusTimer::RaceStatusTimeOutFunc func);

    /**
    *@brief ���ǰ״̬�Ķ�ʱ��
    */
    void _ActiveStatusTimer(RaceStatus oldStatus, RaceStatus newStatus);

    /**
    *@brief ���ǰ״̬�Ķ�ʱ��
    */
    RaceStatusTimer* _GetStatusTimer(RaceStatus status);

    /**
    *@brief �Ƿ�����Ч��λ��
    */
    bool _IsValidPos(UInt8 pos);

    /**
    *@brief �Ƿ�������Ҷ���½�ɹ�
    */
    bool _IsAllGamerLogin();

    /**
    *@brief �Ƿ�������Ҷ����سɹ�
    */
    bool _IsAllGamerLoadFinish();

    /**
    *@brief �Ƿ��յ�������ҵĽ���
    */
    virtual bool _IsRecvAllRaceEnd();

    /**
    *@brief �����������ǲ�����ͬһ״̬
    */
    bool _IsAllGamerInSameStatus(RaceGamerStatus status, bool isGamerMustInRace = true);

    /**
    *@brief �յ�������ҵ�У��֡
    */
    bool _IsRecvAllGamerFrameChecksum(UInt32 frame);

    /**
    *@brief ��ʼУ��֡����
    */
    virtual void _OnFrameChecksum(UInt32 frame);

    /**
    *@brief �޸��������״̬
    */
    void _ChangeAllGamerStatus(RaceGamerStatus status, bool isGamerMustInRace);

    /**
    *@brief �㲥�����¼����������
    */
    void _BroadcastEvent(const GameSessionEvent& event);

    /**
    *@brief ��ӵ�ǰ֡�Ĳ���
    */
    void _PushInputDataToCurrentFrame(UInt8 pos, const CLProtocol::InputData& data, bool push_front = false);

    /**
    *@brief ��ǰ֡�������
    */
    void _FinishCurrentFrame(const Avalon::Time& now);

    /**
    *@brief ����֡���ݸ��������
    */
    void _SyncFrames();

	/**
	*@brief ������֡���ݸ��������ʱ
	*/
	virtual void _OnSyncFrames(std::vector<CLProtocol::Frame>& unSyncFrame);

    /**
    *@brief �㲥��Ϣ�����ͻ���
    */
    void _BroadcastProtocol(Avalon::Protocol& protocol, bool onlyRacingGamer = true, bool disconnect = false);
	void _BroadcastPacket(Avalon::Packet* packet, bool onlyRacingGamer = true, bool disconnect = false);

	/**
	*@brief ��ȡ״̬�ַ���
	*/
    const char* _ShowStatus(RaceStatus status);

	/**
	*@brief ��ȡ״̬�ַ���
	*/
	_gameSessionDismissType _GetDismissTypeByLeaveReason(GamerLeaveReason reason);

	/**
	*@brief ������ʼ֡������֡����
	*@return ����true������滹����Ҫ����֡��false�����Ѿ��������µ���
	*/
	bool _SendFrames(RaceGamer* gamer, UInt32 startFrame);

	/**
	*@brief �ǲ����ڿ�����״̬��
	*/
	bool _IsStatusCanReconnect();

	/**
	*@brief ����pingֵ��ȡ�ź�ǿ��
	*/
	UInt8 _GetNetworkLevelByPing(UInt32 ping);

	/**
	*@brief ǿ�й㲥һ������֡
	*/
	void _BroadcastFrame(UInt8 pos, FrameCommandType type, UInt32 param);

	/**
	*@brief ��⵽��ͬ��ʱ�Ƿ����ս��
	*/
	virtual bool _IsDismissWhenUnsync() { return false; }

	/**
	*@brief  ����һ��InputData
	*/
	virtual bool _HandleOneInputData(RLPlayer* player, CLProtocol::InputData& input);

	/**
	*@brief  ��������������һ��
	*/
	virtual void _OnPlayerChecksumUnsync(RaceGamer& gamer, UInt32 frame, UInt32 finalChecksum);
protected:
	// ��������
	RaceType							m_raceType;

    // ս��ʵ������
    RaceInstanceType                    m_type;

    // ID
    ObjID_t                             m_id;

    // ֮ǰ��״̬
	RaceStatus							m_lastStatus;

	// ��ǰ״̬
    RaceStatus                          m_status;

	// ɾ��ʱ��
	UInt32								m_destoryTime;

    // ��״̬��ʱ��
    RaceStatusTimer                     m_statusTimer[RACE_STATUS_NUM];

	// ��������ʱ��
	UInt32								m_createTime;

    // ��ս��ʼʱ��
    Avalon::Time						m_startTime;

	// ����ʱ��
	Avalon::Time						m_raceEndTime;

    // ���в������
    std::vector<RaceGamer>              m_gamers;

	// ��ս���
	std::vector<RaceGamer>				m_observers;

    // ֡���
    //��ǰ֡��������֡�У�
    CLProtocol::Frame						m_currentFrame;	

    //δͬ����֡���߼�֡Ƶ������֡Ƶ��һ��ʱ��
    std::vector<CLProtocol::Frame>		m_unsyncFrames;

    //֡���ݣ��Ѿ���ɵ�֡������ͬ������ս˫����
    std::vector<CLProtocol::Frame>		m_framesData;
	//���е�֡
	std::vector<CLProtocol::Frame>		m_allFrames;

    UInt32								m_nextRealFrame;
    UInt32								m_nextLogicFrameSequence;

    //֡ͬ��У�����
    //�Ѿ�У����˵�֡
    UInt32                              m_checkedSyncFrame;

	// ����У������
	std::vector<RaceChecksumInfo>		m_checksumInfoes;

	// ���ֽ���
	// pk����
	PkRaceEndInfo						m_pkRaceEndInfo;
	// ���³ǽ���
	DungeonRaceEndInfo					m_dungeonEndInfo;

	// �Ƿ��Ѿ���ͬ��
	bool								m_isUnsync;

	// ������������
	UInt32								m_reconnNum;

	// �������
	UInt32								m_reviveNum;

	// ��������
	UInt32								m_levelUpNum;

	// ��ͬ��������֡
	UInt32								m_firstUnsyncFrame;

	// ��֤���
	// ս����ʼ����
	Avalon::Packet*						m_InitPacket;
	// ������֤����ʼʱ��
	UInt32								m_StartVerifyTime;
	// ��֤������ID
	UInt32								m_VSId;
	// ��֤�������ϴ��Ľ���
	Avalon::Packet*						m_VerifyRaceEndPacket;
};

#define REGISTER_STATUS_TIMER(status, timeout, func)    \
    do {                                                \
        RaceStatusTimer::RaceStatusTimeOutFunc _func;   \
        _func = std::bind(func, this);                  \
        _RegisterStatusTimer(status, timeout, _func);   \
    } while(0)

#endif