#ifndef __RACE_GAMER_H__
#define __RACE_GAMER_H__

#include <AvalonDefine.h>
#include <CLDefine.h>
#include "RLPlayer.h"
#include <CLChatDefine.h>
#include <CLSysNotify.h>

enum RaceGamerStatus
{
    GAMER_STATUS_WAIT_LOGIN,
    GAMER_STATUS_LOGIN_SUCCESS,
    GAMER_STATUS_WAIT_LOAD,
    GAMER_STATUS_LOAD_FINISH,
    GAMER_STATUS_RACING,
    GAMER_STATUS_WAIT_RACE_END,
    GAMER_STATUS_RACE_END,
	GAMER_STATUS_OFFLINE,
};

class RLPlayer;
class RaceGamer
{
public:
    RaceGamer();
    ~RaceGamer();

    /**
    *@brief 角色ID
    */
    inline ObjID_t GetID() const { return m_roleId; }
    void SetID(ObjID_t id) { m_roleId = id; }

    /**
    *@brief WorldServer ID
    */
    inline UInt32 GetWsID() const { return m_wsId; }
    void SetWsID(UInt32 id) { m_wsId = id; }

    /**
    *@brief 状态
    */
    inline RaceGamerStatus GetStatus() const { return m_status; }
    void ChangeStatus(RaceGamerStatus status);

	/**
	*@brief 加载进度
	*/
	inline UInt8 GetLoadProgress() const { return m_loadProgress; }
	void SetLoadProgress(UInt8 progress) { m_loadProgress = progress; }

    /**
    *@brief 账号
    */
    inline UInt32 GetAccountID() const { return m_accId; }
    inline void SetAccountID(UInt32 accId) { m_accId = accId; }

    /**
    *@brief 位置
    */
    inline UInt8 GetPos() const { return m_pos; }
    inline void SetPos(UInt8 pos) { m_pos = pos; }

    /**
    *@brief 离线状态
    */
    inline bool IsOffline() const { return m_offline; }
	void Online(RLPlayer* player);
    void Offline(ObjID_t raceId, UInt32 waitReconnTime);

	/**
	*@brief 重连最后时刻
	*/
	inline UInt32 GetReconnEndTime() const { return m_reconnEndTime; }
	inline void SetReconnEndTime(UInt32 time) { m_reconnEndTime = time; }

    /**
    *@brief Player对象
    */
    inline RLPlayer* GetPlayer() const { return m_player; }
    inline void SetPlayer(RLPlayer* player) { m_player = player; }

    /**
    *@brief 比赛结果
    */
    inline UInt8 GetRaceResult() const { return m_raceResult; }
    inline void SetRaceResult(UInt8 result) { m_raceResult = result; }

	/**
	*@brief 剩余血量(百分比)
	*/
	inline UInt32 GetRemainHp() const { return m_remainHp; }
	inline void SetRemainHp(UInt32 hp) { m_remainHp = hp; }

	/**
	*@brief 剩余魔量(百分比)
	*/
	inline UInt32 GetRemainMp() const { return m_remainMp; }
	inline void SetRemainMp(UInt32 mp) { m_remainHp = mp; }
		 
	/**
	*@brief 是否在重连
	*/
	inline bool IsReconnecting() const { return m_reconnecting; }
	inline void SetReconnecting(bool reconnecting) { m_reconnecting = reconnecting; }

	/**
	*@brief 重连同步帧数
	*/
	inline UInt32 GetSyncReconnectFrame() const { return m_syncReconnectFrame; }
	inline void SetSyncReconnectFrame(UInt32 frame) { m_syncReconnectFrame = frame; }

    /**
    *@brief 关卡结算
    */
    inline const DungeonRaceEndInfo& GetDungeonRaceEndInfo() const { return m_dungeonRaceEndInfo; }
    void SetDungeonRaceEndInfo(const DungeonRaceEndInfo& info) { m_dungeonRaceEndInfo = info; }

	/**
	*@brief pk结算
	*/
	inline const PkRaceEndInfo& GetPkRaceEndInfo() const { return m_pkRaceEndInfo; }
	void SetPkRaceEndInfo(const PkRaceEndInfo& info) { m_pkRaceEndInfo = info; }

	/**
	*@brief 网络质量
	*/
	inline UInt8 GetNetQuality() const { return m_netQuality; }
	void SetNetQuality(UInt8 quality) { m_netQuality = quality; }

    /**
    *@brief 事件
    */
    void SendEventToPlayer(const GameSessionEvent& event);

    /**
    *@brief 发送消息
    */
    void SendProtocol(Avalon::Protocol& protocol, bool disconnect = false);
    void SendPacket(Avalon::Packet* packet, bool disconnect);

	/**
	*@brief 发给这个玩家的包的数量
	*/
	UInt32 GetSentPacketNum() const { return m_sentPacketNum; }

	/**
	*@brief 收包统计
	*/
	void IncRecvPacketNum() { m_recvPacketNum++; }
	UInt32 GetRecvPacketNum() const { return m_recvPacketNum; }

	/**
	*@brief 最后一次收到心跳包时间
	*/
	void SetLastRecvHeartbeatTime(UInt32 t) { m_lastRecvHeartbeatTime = t; }
	UInt32 GetLastRecvHeartbeatTime() const { return m_lastRecvHeartbeatTime; }

	/**
	*@brief 最后一次收到心跳包时间
	*/
	void SetLastSendHeartbeatTime(UInt32 t) { m_lastSendHeartbeatTime = t; }
	UInt32 GetLastSendHeartbeatTime() const { return m_lastSendHeartbeatTime; }

    /**
    *@brief 帧同步相关
    */
    bool PushFrameChecksum(UInt32 frame, UInt32 checksum);
    bool HaveFrameChecksum(UInt32 frame);
    UInt32 GetFrameChecksum(UInt32 frame);
	void SetLastRecvChecksumTime();
	UInt32 GetLastRecvChecksumTime() const { return m_lastRecvChecksumTime; }

	UInt32 GetLastObserveFrame() const { return m_lastObserveFrame; }
	void SetLastObserveFrame(UInt32 frame) { m_lastObserveFrame = frame; }

	/**
	*@brief 发送提示
	*/
	template <typename... Args>
	void SendNotify(UInt32 id, bool disconnect, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		Notify(type, color, buffer, disconnect);
	}

	void Notify(UInt16 type, UInt8 color, const char* word, bool disconnect);

private:
    ObjID_t				    m_roleId;
    UInt32				    m_accId;
    UInt8				    m_pos;
    UInt32                  m_wsId;

	// 加载进度
	UInt8					m_loadProgress;

    RaceGamerStatus		    m_status;
    bool                    m_offline;
    RLPlayer*			    m_player;

    // 战斗结果
    UInt8				    m_raceResult;

    DungeonRaceEndInfo      m_dungeonRaceEndInfo; // 关卡结算
	PkRaceEndInfo			m_pkRaceEndInfo;

    // 帧同步校验数据
    std::vector<UInt32>     m_frameChecksums;
	// 最后一次收到校验帧的时间
	UInt32					m_lastRecvChecksumTime;

	// 断线重连相关
	// 可以重连的最后时间
	UInt32					m_reconnEndTime;
	// 是否正在重连
	bool					m_reconnecting;
	// 同步到第几帧
	UInt32					m_syncReconnectFrame;

	// 剩余血量(百分比)
	UInt32					m_remainHp;
	// 剩余魔量(百分比)
	UInt32					m_remainMp;

	// 网络质量
	UInt8					m_netQuality;

	// 发包数量
	UInt32					m_sentPacketNum;

	// 收包数量
	UInt32					m_recvPacketNum;

	// 观战相关
	// 最后一个观战的帧
	UInt32					m_lastObserveFrame;

	// 心跳相关
	// 最后一次发送心跳包时间
	UInt32					m_lastSendHeartbeatTime;
	// 最后一次收到心跳包时间
	UInt32					m_lastRecvHeartbeatTime;
};

#endif