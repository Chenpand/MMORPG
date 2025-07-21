#ifndef _RELAY_APPLICATION_H_
#define _RELAY_APPLICATION_H_

#include <set>

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

class RLApplication : public CLApplication, public  Avalon::Singleton<RLApplication>
{
public:
	RLApplication ();
	~RLApplication ();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

	void SetIsSendUnsyncNotify(bool isSend) { m_IsSendUnsyncNotify = isSend; }
	bool IsSendUnsyncNotify() { return m_IsSendUnsyncNotify; }
	bool IsOpenReplay() const { return m_IsOpenReplay; }

	UInt32 GetVersion() const { return m_Version; }

	/**
	*@brief 是否检测重复进入地下城
	*/
	bool IsOpenVerifyDungeon() const { return m_OpenVerifyDungeon; }

	/**
	*@brief 是否开启新的随机数检测
	*/
	bool IsOpenNewChecksumCheck() const { return m_OpenNewChecksumCheck; }

	/**
	*@brief 地下城随机数不同步时T人
	*/
	bool IsKickPlayerWhenDungeonUnsync() const { return m_KickPlayerWhenDungeonUnsync; }

	/**
	*@brief 是否验证PK
	*/
	bool IsVerifyPK() const { return m_OpenVerifyPK; }

	/**
	*@brief 验证返回时如果比赛不存在就删除
	*/
	bool NotifyGiveupVerifyWhenRaceUnexist() { return m_NotifyGiveupVerifyWhenRaceUnexist; }

	/**
	*@brief 是否开启心跳
	*/
	bool IsOpenHeartbeat() const { return m_OpenHeartbeat; }

	/**
	*@brief 心跳超时时间
	*/
	UInt32 GetHeartbeatTimeoutSec() const { return m_HeartbeatTimeoutSec; }

	/**
	*@brief 心跳包间隔时间
	*/
	UInt32 GetHeartbeatIntervalSec() const { return m_HeartbeatIntervalSec; }

	/**
	*@brief 观战Relay等待玩家连接定时时间(s)
	*/
	UInt32 GetObserveWaitSec() const { return m_ObserveWaitSec; }

	Avalon::SockAddr GetAddr();

	/**
	*@brief 是否需要等待验证结算
	*/
	bool IsDungeonNeedWaitVerifyRaceEnd(UInt32 dungeonid);

	/**
	*@brief 等待验证结算时间(s)
	*/
	bool GetDungeonWaitVerifyRaceEndSec() const { return m_WaitVerifyRaceEndSec; }

protected:
	/**
	*@brief 加载所有数据表
	*/
	bool LoadDataTables();

	void LoadDynamicConfig();
	void LoadVersion();
	bool ReadVersionFromStr(std::string str);
	UInt32 MakeVersion(UInt32 major, UInt32 minor, UInt32 develop);

private:
    Avalon::SimpleTimer         memCheckTimer;

	// 是否要重新加载配置文件
	bool						m_ReloadConfig;

	// 是否发送不同步提示
	bool						m_IsSendUnsyncNotify;

	// 版本号
	UInt32						m_Version;

	// 是否打开录像
	bool						m_IsOpenReplay;

	// 是否开启验证
	bool						m_OpenVerifyDungeon;

	// 是否开启新的随机数检测
	bool						m_OpenNewChecksumCheck;

	// 地下城随机数不同步时T人
	bool						m_KickPlayerWhenDungeonUnsync;

	// 是否开启验证1v1
	bool						m_OpenVerifyPK;
	// 验证返回时如果比赛不存在就删除
	bool						m_NotifyGiveupVerifyWhenRaceUnexist;

	// 是否开启心跳检测
	bool						m_OpenHeartbeat;
	// 心跳超时时间(s)
	UInt32						m_HeartbeatTimeoutSec;
	// 心跳包间隔时间(s)
	UInt32						m_HeartbeatIntervalSec;
	// 观战Relay等待玩家连接定时时间(s)
	UInt32						m_ObserveWaitSec;

	// 需要等待验证结算的地下城
	std::set<UInt32>			m_WaitVerifyRaceEndDungeons;
	// 等待验证结算时间(s)
	UInt32						m_WaitVerifyRaceEndSec;
};



#endif
