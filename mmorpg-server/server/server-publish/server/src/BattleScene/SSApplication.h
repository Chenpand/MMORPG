#ifndef _SS_APPLICATION_H_
#define _SS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
 *@brief 场景服应用程序类
 */
class SSApplication : public CLApplication, public Avalon::Singleton<SSApplication>
{
public:
	SSApplication();
	~SSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

	UInt32 GetSceneMgrTickIntervalMs() const { return m_SceneMgrTickIntervalMSec; }

	UInt32 GetPlayerBuffCacheTime() const { return m_PlayerBuffCacheTime; }

public:
	bool IsCounterLog() { return m_IsCounterLog; };

	bool IsCheckAntiAddict() { return m_CheckAntiAddict; }

	bool UseNewOfflineCharge() const { return m_NewOfflineCharge; }

	bool IsCloseAbnormalTransHandle() const { return !m_IsHandleAbnormalTrans; }

	bool IsCloseDevil() const { return m_IsCloseDevil; }

	const std::string& GetStrengthenTicketPlanRecordPath() const { return m_StrenTicketPlanRecordFilePath; }
	const std::string& GetArtifactJarRecordPath() const { return m_ArtifactJarRecordFilePath; }

	UInt64 GetStrengthenTicketFuseUseMaxTime() const { return m_StrenTicketFuseUseMaxTime; }

	bool OpenSaveDataAfterCharge() const { return m_IsOpenChargeSaveData; }


	/**
	*@brief 移动包同步频率(ms)
	*/
	UInt32 GetMovePacketIntervalMSec() const { return m_MovePacketIntervalMSec; }

	/**
	*@brief 场景关注列表更新频率(ms)
	*/
	UInt32 GeWatchListUpdateIntervalMSec() const { return m_WatchListUpdateIntervalMSec; }

	/*
	*@brief 统计文件路径
	*/
	std::string GetJarCountPath() const { return m_JarCountPath; }

	/*
	*@brief 吃鸡可捡技能数量
	*/
	UInt32 GetChiJiChoiceSkillNum() { return m_ChiJiChoiceSkillNum; }

	/*
	*@brief 吃鸡客户端移动包发送间隔(ms)
	*/
	UInt32 GetChiJiMovePacketIntervalMs() { return m_ChiJiMovePacketIntervalMs; }

	/*
	*@brief 是不是调试账号
	*/
	bool IsDebugAccount(UInt32 accid) { return std::find(m_DebugAccounts.begin(), m_DebugAccounts.end(), accid) != m_DebugAccounts.end(); }

private:
	/**
	 *@brief 加载所有数据表
	 */
	bool LoadDataTables();

	/**
	*@brief 重新加载数据表
	*/
	void ReloadDataTables();

	// 加载各种动态配置
	void LoadDynamicConfig();

private:
	//tick时间
	UInt64	m_TickTime;

	//重新加载配置
	bool	m_ReloadConfig;

	// 是否Counter日志
	bool	m_IsCounterLog;

	// SceneMgr的Tick间隔时间(ms)
	UInt32	m_SceneMgrTickIntervalMSec;

	// 是否检查防沉迷
	bool	m_CheckAntiAddict;

	// 玩家对象缓存时间(Sec)
	UInt32	m_PlayerBuffCacheTime;

	// 新的离线补单
	bool	m_NewOfflineCharge;

	// 是否处理异常交易
	bool	m_IsHandleAbnormalTrans;

	//重新加载表格
	bool	m_ReloadTable;

	// 关闭异界关卡
	bool	m_IsCloseDevil;

	// 强化券方案记录文件路径
	std::string m_StrenTicketPlanRecordFilePath;
	// 神器罐录文件路径
	std::string m_ArtifactJarRecordFilePath;

	// 强化券融合耗时最大时间
	UInt64 m_StrenTicketFuseUseMaxTime;

	// 是否开启充值后保存数据
	bool	m_IsOpenChargeSaveData;

	// 移动包同步频率(ms)
	UInt32	m_MovePacketIntervalMSec;

	// 场景关注列表更新频率(ms)
	UInt32	m_WatchListUpdateIntervalMSec;

	// 罐子统计文件路径
	std::string		m_JarCountPath;

	// 吃鸡可捡技能数量
	UInt32	m_ChiJiChoiceSkillNum;

	// 吃鸡客户端移动包发送间隔(ms)
	UInt32	m_ChiJiMovePacketIntervalMs;

	// Tick定时器，200ms
	Avalon::SimpleTimer				m_TickTimer200ms;
	// Tick定时器，500ms
	Avalon::SimpleTimer				m_TickTimer500ms;
	// Tick定时器，1000ms
	Avalon::SimpleTimer				m_TickTimer1000ms;

	// 测试玩家
	std::vector<UInt32>				m_DebugAccounts;
};


#endif
