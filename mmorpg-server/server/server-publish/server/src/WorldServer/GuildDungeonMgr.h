#ifndef _GUILD_DUNGEON_MGR_H_
#define _GUILD_DUNGEON_MGR_H_

#include <CLDefine.h>
#include <CLGuildDefine.h>
#include <AvalonSingleton.h>

class WSPlayer;
class GuildDungeon;
class CLRecordCallback;
class Guild;
namespace CLProtocol{
	struct GuildDungeonInfo;
}

/**
 *@brief 公会的地下城管理
 */
class GuildDungeonMgr : public Avalon::Singleton<GuildDungeonMgr>
{
	typedef std::vector<ItemReward> ItemRewardVec;
	typedef std::map<UInt64, GuildDungeon*> GuildDungeonMap;

public:
	GuildDungeonMgr();
	~GuildDungeonMgr();

public:
	void Init();

	void OnTick(const Avalon::Time &now);

	void Final();

	bool IsGuildDungeon();

	bool IsStartDungeon();

	bool IsJoinOrLeaveGuild();

	bool SwitchIsOpen();

	void NotifyGuildDungeonActivity(WSPlayer* player);

	// 公会解散
	void GuildDismiss(UInt64 guildId);
	// 处理公会地下城信息请求
	void HandleGuildDungeonInfoReq(WSPlayer* player);
	// 处理公会地下城伤害排行请求
	void HandleGuildDungeonDamageRankReq(WSPlayer* player);
	// 处理公会地下城副本信息请求
	void HandleGuildDungeonCopyReq(WSPlayer* player);
	// 处理公会地下城抽奖请求
	void HandleGuildDungeonLotteryReq(WSPlayer* player);
	// 处理公会地下城雕像请求
	void HandleGuildDungeonStatueReq(WSPlayer* player);
	// 通知客户端地下城状态修改
	void SyncGuildDungeonStatus();

	// 加载DB数据
	void LoadData();
	// 加载地下城状态
	bool LoadGuildDungeonStatus(CLRecordCallback* callback);
	// 加载公会地下城数据
	bool LoadGuildDungeonData(CLRecordCallback* callback);
	// 加载公会地下城战斗数据
	bool LoadGuildDungeonBattle(CLRecordCallback* callback);
	// 加载公会地下城玩家伤害记录
	bool LoadGuildDungeonDamage(CLRecordCallback* callback);
	// 加载公会地下城玩家领奖回调
	bool LoadGuildDungeonReward(CLRecordCallback* callback);
	// 加载公会地下城玩家参与回调
	bool LoadGuildDungeonJoin(CLRecordCallback* callback);
	// 加载公会地下城随机
	bool LoadGuildDungeonRand(CLRecordCallback* callback);

	// 获取公会地下城信息
	UInt32 StartGuildDungeon(UInt64 guildId, UInt32 dungeonId, UInt64 dungeonRaceId, CLProtocol::GuildDungeonInfo& guildDungeonInfo);
	// 记录地下城伤害
	void RecordDungeonDamage(UInt64 guildId, UInt32 dungeonId, UInt64 playerId, UInt32 occu, UInt64 damageVal, UInt64 dungeonRaceId, const std::string& playerName, UInt32 fightTime = 0);
	// 增加地下城的战斗次数
	void AddGuildDungeonBattleCnt(UInt64 guildId, UInt32 dungeonId);
	// 增加参与玩家
	void AddJoinPlayer(UInt64 guildId, UInt64 playerId);
	// 公会升级检查活动是否开启
	void GuildLevelUp(Guild* guildPtr);
	// 获取boss的剩余血量
	UInt64 GetBossOddBlood(UInt64 guildId, UInt32 dungeonId);
	// 发放拍卖分成
	void GuildAuctionBonus(UInt64 guildId, UInt32 rewardNum);
	// 获取随机关卡
	void GetRandDungeon(UInt32 dungeonType, std::vector<UInt32>& randVec);
	// GM更新状态
	void GMUpdateStatus(UInt32 status);
	// GM清除小boss
	void ClearSmallBoss(UInt64 guildId, UInt64 playerId, UInt32 occu, const std::string& playerName);
	// GM清除大boss
	void ClearBigBoss(UInt64 guildId, UInt64 playerId, UInt32 occu, UInt64 damage, const std::string& playerName);
	// GM加buff和次数
	void GMAddBuffCnt(UInt64 guildId, UInt32 buffItemId, UInt32 cnt);


	UInt32 GetGuildDungeonStatus() { return m_DungeonStatus; }
private:

	// 获得公会地下城
	GuildDungeon* FindGuildDungeon(UInt64 guildId);
	// 创建公会的地下城
	GuildDungeon* CreateGuildDungeon(Guild* guildPtr);

	// 切换状态
	void SwitchDungeonStatus(UInt32 status, UInt64 startStamp);
	// 准备
	void DungeonPrepare();
	// 开始
	void DungeonStart();
	// 领奖
	void DungeonReward();
	// 结束
	void DungeonEnd();
	// 初始化TickMap
	void InitTickMap();
	// 执行tick操作
	void ExecuteTick(const Avalon::Time &now);
	// 状态是否有效
	bool IsVaildStatus(UInt32 status);

	// 增加领奖玩家
	void AddObtainRewardPlayer(UInt64 playerId, bool isSave = false);
	// 玩家是否已经领取奖励
	bool PlayerIsObtainReward(UInt64 playerId);
	// 获取玩家打出的伤害比例(百分比向上取整)
	UInt32 GetPlayerDamageRate(UInt64 guildId, UInt64 playerId);
	// 发送参与奖励邮件
	void SendAttendRewardMail(UInt64 guildId, GuildDungeon* dungeon);
	// 雕像
	void GuildDungeonStatue(UInt64 guildId, const GuildDungeonDamageVec& damageVec);
	// 加入通关排行
	void AddClearGateRank(UInt64 guildId);
	// 通知boss死亡
	void BroadcastBossDead(GuildDungeon* guildDungeon, UInt32 dungeonId);
	// 根据类型获取奖励列表
	void GetRewardByType(UInt32 rewardType, UInt32 dungeonType, ItemRewardVec& rewardVec);
	// 获得拍卖奖励
	void GuildAuctionReward(UInt64 guildId, UInt32 dungeonType, UInt32 guildRank);
	// 清除数据表
	void ClearDBTable(const std::string& _tableName);
	// 清除地下城数据
	void ClearGuildDungeon();
	// 增加随机地下城
	void AddRandDungeon(UInt32 dungeonType, UInt32 dungeonId);
	// 生成随机地下城
	void RandDungeon(UInt32 guildLvl);
	// 获取公会通关时间排名
	UInt32 GetPassGateTimeRank(UInt64 guildId);

private:

	// 公会地下城状态
	UInt32 m_DungeonStatus;
	// 状态开始时间戳
	UInt64 m_StatusStartStamp;
	// 地下城开始的时间
	UInt64 m_startStamp;
	// 数据加载
	UInt32 m_LoadFlag;
	// 公会地下城,key->公会ID
	GuildDungeonMap m_GuildDungeonMap;
	// 领取奖励的玩家
	std::set<UInt64> m_ObtainRewardPlayer;
	// 恢复重启现场
	bool m_IsRecvCheck;
	// 周几
	UInt32 m_Week;
	// 恢复现场的tickmap
	std::map<UInt64, UInt32> m_TickMap;
	// 通关用时排行
	std::vector<GuildDungeonGateTime> m_GateTimeVec;
	// 难度随机(key->难度)
	std::map<UInt32, std::vector<UInt32>> m_RandDungeonMap;

	// 难度随机地下城数量(key->难度，val->随机数量)
	std::map<UInt32, UInt32> m_RandDungenNumMap;
};

#endif // !_GUILD_DUNGEON_MGR_H_
