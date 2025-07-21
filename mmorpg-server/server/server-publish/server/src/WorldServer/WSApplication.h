#ifndef _WS_APPLICATION_H_
#define _WS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLConfigLoadMgr.h>

// 作弊惩罚类型
enum CheatPunnishType
{
	// 只记录日志
	CHEAT_PUNISH_LOG,	
	// 踢下线
	CHEAT_PUNISH_KICK,
	// 踢下线并封号
	CHECK_PUNISH_FORBID_LOGIN
};

/**
 *@brief 世界服应用程序类
 */
class WSApplication : public CLApplication, public CLConfigLoadMgr, public Avalon::Singleton<WSApplication>
{
public:
	WSApplication();
	~WSApplication();

	/**
	 *@brief 获取支付签名密钥
	 */
	const std::string& GetBillingSigKey() const{ return m_BillingSigKey; }

	/**
	*@brief 获取服务器可承载最大人数
	*/
	UInt32 GetMaxPlayerNum() const { return m_MaxOnlinePlayer; }

	/**
	*@brief 获取服务器最多同时加载人数
	*/
	UInt32 GetMaxLoadingPlayerNum() const { return m_MaxLoadingPlayer; }

	/**
	*@brief 是否检测重复进入地下城
	*/
	bool IsCheckRepeatStartDungeon() const { return m_CheckRepeatStartDungeon; }

	/**
	*@brief 是否关闭防止4人队伍功能
	*/
	bool IsCloseFixTeamMatch() const { return m_CloseFixTeamMatch; }	
	
	/**
	*@brief 是否组队自动踢队长功能
	*/
	bool IsTeamAutoKick() const { return m_TeamAutoKick; }	

	/**
	*@brief 是否打开招募功能
	*/
	bool IsHireTurn() const { return m_HireTurn; }

	/**
	*@brief 是否检测重复进入地下城
	*/
	bool IsOpenVerifyDungeon() const { return m_OpenVerifyDungeon; }

	/**
	*@brief 是否检测订单安全性
	*/
	bool IsCheckChargeOrder() const { return m_CheckChargeOrder; }

	/**
	*@brief 是否开启登录排队功能
	*/
	bool IsOpenLoginQueue() const { return m_OpenLoginQueue; }

	/**
	*@brief 作弊惩罚
	*/
	CheatPunnishType GetCheatPunish() const { return (CheatPunnishType)m_CheatPunish; }

	/**
	*@brief 验证作弊惩罚
	*/
	CheatPunnishType GetVerifyCheatPunish() const { return (CheatPunnishType)m_VerifyCheatPunishType; }

	/**
	*@brief 静态数值出错时是否踢人
	*/
	bool KickWhenStaticValueCheckError() const { return m_KickWhenStaticValueCheckError; }

	/**
	*@brief 死亡之塔作弊时立刻T人
	*/
	UInt32 TowerKickWhenCheat() const { return m_TowerKickWhenCheat; }

	/**
	*@brief 异界每日可作弊次数
	*/
	UInt32 DevilddomDailyCheatNum() const { return m_DevilddomDailyCheatNum; }

	/**
	*@brief 公会地下城每日可作弊次数
	*/
	UInt32 GuildDungeonDailyCheatNum() const { return m_GuildDungeonDailyCheatNum; }

	/**
	*@brief 团队副本每日可作弊次数
	*/
	UInt32 TeamCopyDailyCheatNum() const { return m_TeamCopyDailyCheatNum; }

	/**
	*@brief 团队作弊封号时间
	*/
	UInt32 GetTeamCopyCheatForbidTime(UInt32 num);

	/**
	*@brief 不同步多少次之后就T人
	*/
	UInt32 GetKickCheatNum() const { return m_KickCheatNum; }

	/**
	*@brief 是否记录地下城日志
	*/
	bool IsRecordDungeonLog() const { return m_IsRecordDungeonLog; }

	/**
	*@brief 是否记录地下城录像
	*/
	bool IsRecordDungeonReplay() const { return m_IsRecordDungeonReplay; }

	/**
	*@brief 是否记录pk录像
	*/
	bool IsRecordPkReplay() const { return m_IsRecordPkReplay; }

	/**
	*@brief 使用加速挂T人
	*/
	bool KickWhenSpeedCheat() const { return m_KickWhenSpeedCheat; }

	/**
	*@brief 过门检测上报的帧
	*/
	bool CheckEnterAreaFrame() const { return m_CheckEnterAreaFrame; }

	/**
	*@brief 第几个房间开始检测上报帧
	*/
	UInt32 GetCheckAreaStartIndex() const { return m_CheckAreaStartIndex; }

	/**
	*@brief 拾取掉落时检测上报的帧
	*/
	bool CheckPickDropFrame() const { return m_CheckPickDropFrame; }

	/**
	*@brief 是否验证md5
	*/
	bool IsCheckMD5() const { return m_CheckMd5; }

	/**
	*@brief 是否离开战斗还呆着队伍中
	*/
	bool StayTeamWhenLeaveRace() const { return m_StayTeamWhenLeaveRace; }

	/**
	*@brief 是否开启红包功能
	*/
	bool IsOpenRedPacket() const { return m_OpenRedPacket; }

	/**
	*@brief 是否需要转移账号
	*/
	bool IsNeedConvertAccount(const std::string& pf) const { return std::find(m_ConvertPfs.begin(), m_ConvertPfs.end(), pf) != m_ConvertPfs.end(); }

	/**
	*@brief 新包下载地址
	*/
	const std::string& GetDownloadUrl() const { return m_DownloadUrl; }

	/**
	*@brief 需要记录的掉落最低品质
	*/
	UInt8 GetNeedRecordDropQuality() const { return m_NeedRecordDropQuality; }

	/**
	*@brief 需要记录的掉落最低品质
	*/
	UInt32 GetGMQueryPlayerMaxNum() const { return m_GMQueryPlayerMaxNum; }
	
	/**
	*@brief 是否超级新服
	*/
	bool IsSuperNewService() const { return m_SuperNewServiceFlag; }

	/**
	*@brief 获取超级新服活动列表
	*/
	std::vector<UInt32>& GetSuperNewServiceOpActs() { return m_SuperNewServiceOpActs; }

	/**
	*@brief 是否检测gm数据加载情况
	*/
	bool IsCheckGmDataLoadSituation() { return m_CheckGmDataLoadSituation; }

	/*
	*@brief 是否检查好友UpdateRelation函数调用
	*/
	bool IsCheckUpDateRelation() { return m_CheckUpdateRelation; }

	/*
	*@brief 是否优化好友UpdateRelation函数调用
	*/
	bool IsOptimUpdateRelation() { return m_OptimUpdateRelation; }

	/*
	*@brief 掉落统计文件路径
	*/
	std::string GetDropCountPath() const { return m_DropCountPath; }

	/*
	*@brief 宝藏统计文件路径
	*/
	std::string GetDigCountPath() const { return m_DigCountPath; }

	UInt32	GetAuctionQueryPriceListInterval() { return m_AuctionQueryPriceListInterval; }

	bool GetAuctionSqlJointModify() { return m_AuctionSqlJointModify; }

	bool GetAuctionSqlInModify() { return m_AuctionSqlInModify; }

	UInt32 GetAuctionPriceListCacheTime() { return m_AuctionPriceListCacheTime; }

	bool IsAccountDataFlush() { return m_AccountDataFlush; }

	UInt64 GetBattleFlag() const { return m_BattleFlag; }

	UInt32 GetAuctionDueOffSaleTime() { return m_AuctionDueOffSaleTime; }

	bool NotifyGiveupVerifyWhenRaceUnexist() { return m_NotifyGiveupVerifyWhenRaceUnexist; }

	bool CheckDevilDropBug() const { return m_DevilDropBug; }
	
	/**
	*@brief 是否延迟刷新账号周常数据等等
	*/
	bool IsAccountDelayedRefresh() const { return m_AccountDelayedRefreshFlag; }

	bool IsNewDailyDrop() const { return m_NewDailyDrop; }

	bool IsCheckChargeAccount() const { return m_CheckChargeAccount; }

	bool IsAdjustOnOnline() const { return m_AdjustOnOnline; }
	
	/*
	*@brief 统计掉落文件路径
	*/
	std::string GetDropItemCountPath() const { return m_DropItemCountPath; }

	/**
	*@brief 是否记录团本日志
	*/
	bool IsLogTeamCopyProtocol() { return m_LogTeamCopyProtocol; }

	/**
	*@brief 是否加载其他角色信息
	*/
	bool IsLoadOtherRoleInfo() { return m_LoadOtherRoleInfo; }

	/**
	*@brief 获取飞升礼包时是否检测角色等级
	*/
	bool IsCheckLevelWhenFlyupGift() { return m_CheckLevelWhenFlyupGift; }

	/**
	*@brief 公会战积分排行名次
	*/
	UInt32 GetGuildBattleScoreRank() { return m_GuildBattleScoreRank; }

	UInt32 GetTableCount() { return m_databaseTableCount; }

	/**
	*@brief 是否使用拍卖行拍卖物品数量缓存
	*/
	bool IsUseAuctionNumCache() const { return m_IsUseAuctionNumCache; }

	/**
	*@brief 拍卖行拍卖物品数量缓存时间
	*/
	UInt32	GetAuctionNumCacheTime() const{ return m_AuctionNumCacheTime; }
	void SetAuctionNumCacheTime(UInt32 time) { m_AuctionNumCacheTime = time; }

	/**
	*@brief 是否好友检测玩家
	*/
	bool GetPrintRelationsUid() { return m_PrintRelationsUid; }

	/**
	*@brief 拍卖行操作走内存
	*/
	bool IsAuctionOpUseMem() { return m_AuctionOpUseMem; }

	/**
	*@brief 是否关闭师徒离线发布师门任务
	*/
	bool IsCloseMasterOffPubTask() const { return m_CloseMasterOffPubTask; }

	/**
	*@brief 是否关闭离线好友赠送
	*/
	bool IsCloseOffPresentGive() const { return m_CloseOffPresentGive; }
public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	/**
	*@brief 信号处理
	*/
	virtual void OnSignal(int signal);

	/**
	 *@brief 跨周，启动时也会判断是否跨周
	 */
	void OnWeekChanged();

	/**
	 *@brief 跨月6点，启动时也会判断是否跨月
	 */
	void OnMonthChanged();
	

	/**
	*@brief 执行跨天周条件是否满足
	*/
	bool IsOnChangedConditionReady();

	/**
	 *@brief 跨天，启动时也会判断是否跨天
	 */
	void OnDayChanged();

	/**
	*@brief 重新加载数据表
	*/
	void ReloadDataTables();

	std::string GetHellMonsterCountPath() const { return m_HellMonsterCountPath; }

	/**
	*@brief 开启地下城结算补随机数校验
	*/
	bool IsOpenDungeonRaceEndChecksum() const {	return m_OpenDungeonRaceEndChecksum; }

public:
	void IncInitDataLoadCount(){ ++m_InitDataLoadCount; }
	void DecInitDataLoadCount(){ if(m_InitDataLoadCount > 0) --m_InitDataLoadCount; }

private:
	/**
	*@brief 加载动态的配置
	*/
	void LoadDynamicConfig();

	/**
	 *@brief 等待数据库加载完成
	 */
	bool WaitDataLoadFinished();

	/**
	 *@brief 加载所有数据表
	 */
	bool LoadDataTables();

	/**
	*@brief 注册需要重载的表
	*/
	virtual void RegisterReloadDataTables();

	/**
	 *@brief 跨周和跨天
	 */
	void CheckTime(const Avalon::Time& now);	
	
	/**
	 *@brief 通知Union区id改变
	 */
	void NotifyUnionZoneIDChanged();


private:
	//初始数据加载数
	UInt32			m_InitDataLoadCount;
	//支付签名key
	std::string		m_BillingSigKey;
	//上次tick时间
	UInt64			m_TickTime;
	//最大人数
	UInt32			m_MaxOnlinePlayer;
	//最大人数
	UInt32			m_MaxLoadingPlayer;
	//重新加载配置
	bool			m_ReloadConfig;
	//是否关闭防止4人队伍功能
	bool            m_CloseFixTeamMatch;
	//是否检测重复进入地下城
	bool			m_CheckRepeatStartDungeon;
	//是否检测订单安全性
	bool			m_CheckChargeOrder;
	// 是否开启排队功能
	bool			m_OpenLoginQueue;
	// 作弊惩罚
	UInt8			m_CheatPunish;
	// 是否打开md5检测
	bool			m_CheckMd5;
	// 是否重新表格
	bool			m_ReloabTable;
	// 是否离开战斗还呆着队伍中
	bool			m_StayTeamWhenLeaveRace;
	// 静态数值出错时是否踢人
	bool			m_KickWhenStaticValueCheckError;

	// 是否开启验证
	bool			m_OpenVerifyDungeon;
	// 验证作弊后的处理
	UInt8			m_VerifyCheatPunishType;
	// 死亡之塔作弊时立刻T人
	bool			m_TowerKickWhenCheat;
	// 异界每天可作弊次数
	UInt32			m_DevilddomDailyCheatNum;
	// 公会地下城每天可作弊次数
	UInt32			m_GuildDungeonDailyCheatNum;
	// 团队副本每天可作弊次数
	UInt32			m_TeamCopyDailyCheatNum;
	// 团本作弊封号时间
	std::vector<UInt32>	m_TeamCopyCheatForbidTime;

	// 不同步多少次之后就T人
	UInt32			m_KickCheatNum;
	// 是否记录地下城日志
	bool			m_IsRecordDungeonLog;
	// 是否记录地下城录像
	bool			m_IsRecordDungeonReplay;
	// 是否记录pk录像
	bool			m_IsRecordPkReplay;
	// 使用加速挂是否踢人
	bool			m_KickWhenSpeedCheat;
	// 过门检测上报的帧
	bool			m_CheckEnterAreaFrame;
	// 第几个房间开始检测上报帧
	UInt32			m_CheckAreaStartIndex;
	// 拾取掉落时检测上报的帧
	bool			m_CheckPickDropFrame;
	// 是否开放新年红包
	bool			m_OpenRedPacket;
	// 需要账号转移的渠道
	std::vector<std::string>	m_ConvertPfs;
	// 新报下载地址
	std::string					m_DownloadUrl;
	// 需要记录的掉落最低品质
	UInt8						m_NeedRecordDropQuality;
	// gm单次最多查询玩家数
	UInt32			m_GMQueryPlayerMaxNum;
	// 超级新服标记
	bool			m_SuperNewServiceFlag;
	// 超级新服活动
	std::vector<UInt32> m_SuperNewServiceOpActs;
	// 是否检测gm数据加载情况
	bool			m_CheckGmDataLoadSituation;
	// 是否检查好友UpDateRelation函数调用
	bool			m_CheckUpdateRelation;

	//是否优化UpDateRelation函数调用
	bool			m_OptimUpdateRelation;

	// 掉落统计文件路径
	std::string		m_DropCountPath;

	// 开宝箱文件路径
	std::string		m_DigCountPath;

	//拍卖行查询价格列表时间间隔
	UInt32			m_AuctionQueryPriceListInterval;

	//拍卖行查询语句修改
	bool			m_AuctionSqlJointModify;

	//拍卖行查询语句修改
	bool			m_AuctionSqlInModify;

	//拍卖行价格列表缓存时间
	UInt32			m_AuctionPriceListCacheTime;

	//拍卖行到期物品自动下架时间
	UInt32			m_AuctionDueOffSaleTime;

	// Tick定时器，200ms
	Avalon::SimpleTimer				m_TickTimer200ms;
	// Tick定时器，500ms
	Avalon::SimpleTimer				m_TickTimer500ms;
	// Tick定时器，1000ms
	Avalon::SimpleTimer				m_TickTimer1000ms;
	// Tick定时器，1分钟
	Avalon::SimpleTimer				m_TickTimerMin;

	// 账号表数据不立马刷新
	bool							m_AccountDataFlush;
	// 战斗标记
	UInt64							m_BattleFlag;
	// 验证返回时如果比赛不存在就删除
	bool							m_NotifyGiveupVerifyWhenRaceUnexist;
	// 处理异界掉落BUG
	bool							m_DevilDropBug;
	// 账号延迟刷新开关
	bool							m_AccountDelayedRefreshFlag;
	// 每日掉落新的记录方式
	bool							m_NewDailyDrop;
	// 充值时检测账号与角色是否一致
	bool							m_CheckChargeAccount;
	// 调整ononlie顺序
	bool							m_AdjustOnOnline;
	// 掉落统计文件路径
	std::string					    m_DropItemCountPath;
	// 是否记录团本消息
	bool							m_LogTeamCopyProtocol;
	// 是否加载其他角色信息
	bool							m_LoadOtherRoleInfo;
	// 获取飞升礼包时是否检测角色等级
	bool							m_CheckLevelWhenFlyupGift;
	// 公会战积分排行名次
	UInt32							m_GuildBattleScoreRank;

	//数据库分表数量
	UInt32							m_databaseTableCount;

	//组队自动踢队长开关
	bool							m_TeamAutoKick;

	//招募开关
	bool							m_HireTurn;

	//拍卖行拍卖物品缓存开关
	bool							m_IsUseAuctionNumCache;
	UInt32							m_AuctionNumCacheTime;

	// 新的重载表格机制
	bool							m_NewReloadTable;

	//需要打印好友数据的玩家id
	bool							m_PrintRelationsUid;

	//拍卖行操作走内存
	bool							m_AuctionOpUseMem;

	// 罐子统计文件路径
	std::string						m_HellMonsterCountPath;

	// 开启地下城结算补随机数校验
	bool							m_OpenDungeonRaceEndChecksum;

	//师徒离线发布任务是否关闭
	bool							m_CloseMasterOffPubTask;

	//好友离线赠送师傅关闭
	bool							m_CloseOffPresentGive;
};

#endif
