#ifndef __VERIFY_TASK_H__
#define __VERIFY_TASK_H__

#include <CLDefine.h>
#include <CLRelayServerProtocol.h>
#include <CLMatchDefine.h>
#include <CLVerifyDefine.h>
#include <CLCheatDefine.h>

// 验证任务的状态
enum VerifyTaskStatus
{
	// 初始化
	VERIFY_TASK_STATUS_INIT,
	// 正在进行
	VERIFY_TASK_STATUS_DOING,
	// 等待逻辑结束
	VERIFY_TASK_WAIT_LOGIC_FINISH,
	// 完成
	VERIFY_TASK_STATUS_FINISH,
};

struct LogicChecksum
{
	UInt32 frame;
	UInt32 checksum;
};

struct VerifyGamer
{
	VerifyGamer()
	{
		roleId = 0;
		pos = 0;
		cheat = false;
	}
	ObjID_t						roleId;
	UInt8						pos;
	std::vector<ChecksumInfo>	checksums;
	bool						cheat;
};

// 最多参战人数 0~4 是一方，5-9 是另外一方
#define MAX_GAMER_COUNT	(10)
//帧校验的间隔（单位：帧）
#define FRAME_CHECKSUM_INTERVAL (50)

struct RaceChecksumInfo
{
	RaceChecksumInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	// 帧数
	UInt32 frame;
	// 所有玩家的校验（按座位号）
	UInt32 checksums[MAX_GAMER_COUNT];
};

class ILogic;
class VerifyTask
{
public:
	VerifyTask(RaceType type, ObjID_t raceId, UInt32 connId);
	virtual ~VerifyTask();

public:
	/**
	*@brief 根据战斗初始化消息创建任务
				pk:			MatchSvrNotifyRacePlayerInfo
				Dungeon:	WorldDungeonStartRaceReq
	*/
	bool OnInit(Avalon::Packet* raceInitPacket);

	/**
	*@brief Tick
	*/
	void OnTick(const Avalon::Time& now);

public:
	/**
	*@brief ID
	*/
	ObjID_t GetID() const { return m_RaceId; }

	/**
	*@brief Type
	*/
	RaceType GetType() const { return m_Type; }

	/**
	*@brief 验证任务状态
	*/
	void SetStatus(VerifyTaskStatus status) { m_Status = status; }
	VerifyTaskStatus GetStatus() const { return m_Status; }

	/**
	*@brief 游戏内玩家
	*/
	void AddGamer(ObjID_t roleId, UInt8 pos);
	VerifyGamer* FindGamerByRoleID(ObjID_t raceId);
	VerifyGamer* FindGamerByPos(UInt8 pos);

	/**
	*@brief 获取玩家上报结算的时间(最早的那个)
	*/
	UInt64 GetPlayerReportRaceEndTime() const { return m_PlayerReportRaceEndTime; }

public:
	/**
	*@brief 添加帧数据
		fillFrame代表是否填充时间帧
	*/
	void PushFrame(const std::vector<CLProtocol::Frame>& frames);

	/**
	*@brief 设置玩家上报的结算
	*/
	void SetPlayerRaceEndInfo(ObjID_t roleId, Avalon::Packet* raceEndInfo);

	/**
	*@brief 设置玩家上报的随机数
	*/
	void SetPlayerChecksum(ObjID_t roleId, UInt32 frame, UInt32 checksum);

public:
	// 逻辑相关
	/**
	*@brief 设置逻辑上报的结算
	*/
	void SetLogicRaceEndInfo(Avalon::Packet* raceEndInfo);
	Avalon::Packet* GetLogicRaceEndInfo() { return m_LogicRaceEnd; }

	/**
	*@brief 设置逻辑上报的随机数
	*/
	void SetLogicChecksum(UInt32 frame, UInt32 checksum);

	/**
	*@brief 放弃验证
	*/
	void GiveupVerify();

protected:
	// 验证相关

	/**
	*@brief 验证随机数
	*/
	void _OnFrameChecksum();

	/**
	*@brief 验证结算
	*/
	void _OnCheckRaceEnd();

	/**
	*@brief 验证成功
	*/
	virtual void _OnVerifyFinish();

	/**
	*@brief 验证超时
	*/
	virtual void _OnVerifyTimeout();

	/**
	*@brief 玩家作弊
	*/
	virtual void _OnPlayerCheat(ObjID_t roleid, PlayerCheatReason reason, UInt32 param = 0);

	/**
	*@brief 获得一局最大帧数
	*/
	UInt32 _GetMaxFrameNum() const;

	/**
	*@brief 一局比赛最多时间(ms)
	*/
	UInt64 _GetRaceMaxTime() const ;

	/**
	*@brief 打印结算
	*/
	void ShowDungeonRaceEnd(ObjID_t roleid, Avalon::Packet* packet);

	/**
	*@brief 打印结算
	*/
	void ShowPkRaceEnd(ObjID_t roleid, Avalon::Packet* packet);

protected:
	/**
	*@brief 发送协议到服务器
	*/
	void _SendbackProtocol(Avalon::Protocol& protocol);

protected:
	// 比赛类型
	RaceType							m_Type;
	// 逻辑对象
	ILogic*								m_Logic;
	// 比赛ID
	ObjID_t								m_RaceId;
	// 服务器ID
	UInt32								m_ConnId;
	// 状态
	VerifyTaskStatus					m_Status;
	// 开始时间
	UInt64								m_StartTime;
	// 收到的最后一帧
	UInt32								m_LastFrame;
	// 所有的帧
	std::vector<CLProtocol::Frame>		m_Frames;
	// 玩家信息
	std::vector<VerifyGamer>			m_Gamers;
	
	// 验证相关
	// 所有玩家上报的结算消息
	std::map<ObjID_t, Avalon::Packet*>	m_PlayerRaceEnd;
	// 逻辑生成的待校验数据
	std::map<UInt32, UInt32>			m_logicChecksums;
	// 逻辑生成的结算消息
	Avalon::Packet*						m_LogicRaceEnd;
	// 收到玩家结算时间
	UInt64								m_PlayerReportRaceEndTime;
};

#endif