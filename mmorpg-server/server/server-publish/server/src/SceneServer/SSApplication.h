#ifndef _SS_APPLICATION_H_
#define _SS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLConfigLoadMgr.h>

/**
 *@brief 场景服应用程序类
 */
class SSApplication : public CLApplication, public CLConfigLoadMgr, public Avalon::Singleton<SSApplication>
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

	UInt32 GetMaxPlayerBuffCacheNum() const { return m_MaxPlayerBuffCacheNum; }

public:
	bool IsCounterLog() { return m_IsCounterLog; };

	bool IsCheckAntiAddict() { return m_CheckAntiAddict; }
	UInt32 GetTouLoginInterval() { return m_TourLoginInterval; }
	UInt32 GetTourOnlineTime() { return m_TourOnlineTime; }
	UInt32 GetAntiAddictTipTime() { return m_AntiAddictTipTime; }
	void LoadAntiAddictNotifyTime(const std::string& tourTime, const std::string& noAdultTime);
	UInt32 IsNotifyTourAntiAddict(UInt32 oddTime);
	UInt32 IsNotifyNoAdultAntiAddict(UInt32 oddTime);

	bool UseNewOfflineCharge() const { return m_NewOfflineCharge; }

	bool IsCloseAbnormalTransHandle() const { return !m_IsHandleAbnormalTrans; }

	bool IsCloseDevil() const { return m_IsCloseDevil; }

	const std::string& GetStrengthenTicketPlanRecordPath() const { return m_StrenTicketPlanRecordFilePath; }
	const std::string& GetArtifactJarRecordPath() const { return m_ArtifactJarRecordFilePath; }

	UInt64 GetStrengthenTicketFuseUseMaxTime() const { return m_StrenTicketFuseUseMaxTime; }

	bool OpenSaveDataAfterCharge() const { return m_IsOpenChargeSaveData; }

	bool IsUseFreezeAbnormalNew() const { return m_IsUseFreezeAbnormalNew; }
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
	*@brief 增幅统计文件路径
	*/
	std::string GetEnhanceCountPath() const { return m_EnhanceCountPath; }

	/*
	*@brief 宝珠转换统计文件路径
	*/
	std::string GetBeadConvertCountPath() const { return m_BeadConvertCountPath; }

	/*
	*@brief 种植统计文件路径
	*/
	std::string GetPlantCountPath() const { return m_PlantPath; }

	std::string GetComposeCardPath() const { return m_ComposeCardPath; }
	
	std::string GetGiftCountPath() const { return m_GiftCountPath; }

	/*
	*@brief 是否开放重新开始地下城
	*/
	bool IsOpenRestartDungeon() const { return m_OpenRestartDungeon; }

	/*
	*@brief 是否检查开始地下城频率
	*/
	bool IsCheckStartDungeonFrequence() const { return m_IsCheckStartDungeonFrequence; }

	/*
	*@brief 同一秒内开始地下城消息上限
	*/
	UInt32 GetMaxStartDungeonNumInSecond() const { return m_MaxStartDungeonNumInSecond; }

	/*
	*@brief 是否检测罐子类型
	*/
	bool IsCheckJarType() const { return m_CheckJarType; }

	/*
	*@brief 强制能开的罐子
	*/
	bool IsSpecialJar(UInt32 id) const { return std::find(m_CanOpenJarIds.begin(), m_CanOpenJarIds.end(), id) != m_CanOpenJarIds.end(); }

	/*
	*@brief 是否开启新的商店刷新流程
	*/
	bool IsUseNewShopRefresh() const { return m_NewShopRefesh; }

	/*
	*@brief 是否开启新的聊天屏蔽流程
	*/
	bool IsNewChatCheck() const { return m_NewChatCheck; }

	/*
	*@brief 是否开启新的聊天屏蔽流程
	*/
	bool IsOpenMoveCheck() const { return m_OpenMoveCheck; }

	/*
	*@brief 是否开启详细日志
	*/
	bool IsOpenDetailLog() const { return m_OpenDetailLog; }
	
	/**
	*@brief 获取独立攻击力pvp加成系数
	*/
	UInt32	GetIndpentPvpRatio() const{ return m_IndpentAtlPvpRatio; }
	void	SetIndpentPvpRatio(UInt32 value) { m_IndpentAtlPvpRatio = value; }

	/**
	*@brief 客户端日志最大长度
	*/
	UInt32 GetClientLogMaxLength() const { return m_ClientLogMaxLength; }

	/**
	*@brief 检查强化结果
	*/
	bool CheckStrengthenResult() const { return m_CheckStrengthenResult; }

	/**
	*@brief 是否开启性能分析
	*/
	bool OpenPerf() const { return m_OpenPerf; }

	/**
	*@brief 是否开启快速批量出售
	*/
	bool OpenFastSellBat() const { return m_OpenFastSellBat; }

	/**
	*@brief 是否开启PlayerOnOnline优化
	*/
	bool OpenPlayerOnOnline() const { return m_OpenPlayerOnOnline; }

	/**
	*@brief 新的玩家心跳
	*/
	bool NewHeartbeat() const { return m_NewHeartbeat; }


	/**
	*@brief 新属性同步开关
	*/
	bool OpenNewSync() { return m_IsOpenNewSync; }

private:
	/**
	 *@brief 加载所有数据表
	 */
	bool LoadDataTables();

	/**
	*@brief 重新加载数据表
	*/
	void ReloadDataTables();

	/**
	*@brief 加载各种动态配置
	*/
	void LoadDynamicConfig();

	/**
	*@brief 注册需要重载的表
	*/
	virtual void RegisterReloadDataTables();

private:
	//新属性同步开关
	bool  m_IsOpenNewSync = false;

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
	// 防沉迷游客登录间隔
	UInt32	m_TourLoginInterval;
	// 防沉迷游客在线时间
	UInt32	m_TourOnlineTime;
	// 防沉迷提前提示时间
	UInt32  m_AntiAddictTipTime;
	// 防沉迷游客提示时间
	std::vector<UInt32> m_AddictTourNotifyTimeVec;
	// 防沉迷未成年提示时间
	std::vector<UInt32> m_AddictNoAdultNotifyTimeVec;

	// 玩家对象缓存时间(Sec)
	UInt32	m_PlayerBuffCacheTime;

	// 玩家对象最大缓存个数
	UInt32	m_MaxPlayerBuffCacheNum;

	// 新的离线补单
	bool	m_NewOfflineCharge;

	// 是否处理异常交易
	bool	m_IsHandleAbnormalTrans;

	// 是否利用新的金币冻结定时
	bool	m_IsUseFreezeAbnormalNew;

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

	// 增幅统计文件路径
	std::string		m_EnhanceCountPath;

	// 宝珠转换统计文件路径
	std::string		m_BeadConvertCountPath;

	// 植树统计文件路径
	std::string     m_PlantPath;

	std::string     m_ComposeCardPath;

	std::string		m_GiftCountPath;
	
	// Tick定时器，200ms
	Avalon::SimpleTimer				m_TickTimer200ms;
	// Tick定时器，500ms
	Avalon::SimpleTimer				m_TickTimer500ms;
	// Tick定时器，1000ms
	Avalon::SimpleTimer				m_TickTimer1000ms;
	// Tick定时器，1分钟
	Avalon::SimpleTimer				m_TickTimerMin;
	// Tick定时器，1月
	Avalon::SimpleTimer				m_TickTimerMonth;

	// 是否开启重新开始地下城
	bool							m_OpenRestartDungeon;
	// 是否检查开始地下城频率
	bool							m_IsCheckStartDungeonFrequence;
	// 同一秒内开始地下城消息上限
	UInt32							m_MaxStartDungeonNumInSecond;

	// 检测罐子类型
	bool							m_CheckJarType;
	// 强制能开的罐子ID
	std::vector<UInt32>				m_CanOpenJarIds;

	// 新的商店刷新流程
	bool							m_NewShopRefesh;
	// 是否开启新的聊天屏蔽流程
	bool							m_NewChatCheck;
	// 是否开启移动监测
	bool							m_OpenMoveCheck;
	// 是否开启详细日志
	bool							m_OpenDetailLog;
	// 独立攻击力PVP系数
	UInt32							m_IndpentAtlPvpRatio;

	// 客户端日志长度
	UInt32							m_ClientLogMaxLength;
	// 检查强化结果
	bool							m_CheckStrengthenResult;

	// 性能分析
	bool							m_OpenPerf;

	//快速批量出售
	bool							m_OpenFastSellBat;

	//PlayerOnOnline优化
	bool							m_OpenPlayerOnOnline;

	// 新的玩家心跳
	bool							m_NewHeartbeat;

	// 新的重载表格机制
	bool							m_NewReloadTable;
};


#endif
