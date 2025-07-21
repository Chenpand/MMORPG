#ifndef __GUILD_BATTLE_MGR_H__
#define __GUILD_BATTLE_MGR_H__

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include <CLGameSessionProtocol.h>
#include <CLGuildProtocol.h>
#include "GuildBattleTerritory.h"

class WSPlayer;
class Guild;
class GuildMember;
class MatchServerGuildBattleRaceEnd;

class GuildBattleMgr : public Avalon::Singleton<GuildBattleMgr>
{
public:

	GuildBattleMgr() :m_Status(GuildBattleStatus::GBS_INVALID), m_TargetStatus(GuildBattleStatus::GBS_INVALID), m_IsGuildLoad(false), m_IsByTime(true){}
	~GuildBattleMgr(){}

	bool Init();

	/**
	*@brief 添加报名信息
	*/
	UInt32 AddEnroll(Guild* guild, UInt8 terrId, UInt64 playerId);

	/**
	*@brief 添加宣战信息
	*/
	UInt32 AddChallengeEnroll(Guild* guild, UInt8 terrId, UInt64 playerId);

	/**
	*@brief 初始化报名信息
	*/
	UInt32 LoadEnroll(Guild* guild);

	/**
	*@brief 获取报名数量
	*/
	UInt32 GetEnrollSize(UInt8 terrId);
	UInt32 GetEnrollSize(GuildBattleTerritory* terr);

	/**
	*@brief 不根据自然时间变化状态
	*/
	void SetIsByTime(bool isByTime) { m_IsByTime = isByTime; }

	/**
	*@brief 设置公会战时间信息
	*/
	void SetGuildBattleTimeInfo(GuildBattleTimeInfo info);

public:
	/**
	*@brief 报名
	*/
	UInt32 OnRequestEnroll(WSPlayer* player, UInt8 terrId, UInt32 itemId = 0, UInt32 itemNum = 0);

	/**
	*@brief 鼓舞
	*/
	UInt32 OnRequestInspire(WSPlayer* player, UInt8& inspire);

	/**
	*@brief 匹配
	*/
	UInt32 OnMatchBattleStart(WSPlayer* player);

	/**
	*@brief 一场公会战开始
	*/
	UInt32 OnBattleRaceStart(WSPlayer* player);

	/**
	*@brief 一场公会战结果
	*/
	UInt32 OnBattleRaceEnd(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd);

	/**
	*@brief 请求领取奖励
	*/
	UInt32 OnRequestReceive(WSPlayer* player, UInt32 boxId);

	/**
	*@brief 请求公会战记录
	*/
	UInt32 OnRequestBattleRecord(WSPlayer* player, std::vector<GuildBattleRecord>& records, Int32 startIndex, UInt32 count);
	UInt32 OnRequestBattleSelfRecord(WSPlayer* player, std::vector<GuildBattleRecord>& records);

	/**
	*@brief 获取领地信息
	*/
	UInt32 GetTerritroyBaseInfo(UInt8 terrId, GuildTerritoryBaseInfo& info);

	/**
	*@brief 获取所有领地信息
	*/
	UInt32 GetTerritroyBaseInfos(std::vector<GuildTerritoryBaseInfo>& infos);

	/**
	*@brief 获取公会战类型
	*/
	GuildBattleType GetGuildBattleType() { return m_Type; }

	/**
	*@brief 获取公会战状态
	*/
	GuildBattleStatus GetGuildBattleStatus() { return m_Status; }

	/**
	*@brief 获取公会战状态结束时间
	*/
	UInt32 GetGuildBattleStatusEndTime(UInt32 status);

	/**
	*@brief 公会战排行
	*/
	void ListSortEntries(WSPlayer* player, SortListType sortType, UInt16 start, UInt16 count);

	/**
	*@brief 获取本身排名
	*/
	UInt32 GetRanking(SortListType sortType, WSPlayer* player);

	/**
	*@brief 查询公会宣战信息
	*/
	UInt32 SelectGuildBattleChallengeInfo(WSPlayer* player);

	/**
	*@brief 抽奖
	*/
	UInt32 Lottery(WSPlayer* player);

	/**
	*@brief 增加记录
	*/
	UInt32 AddTestRecord(WSPlayer* player);

	/**
	*@brief 解散帮会
	*/
	bool Dismiss(UInt64 guildId);

	/**
	*@brief 
	*/
	bool IsCrossGuildBattleDungeon(Guild* guild, UInt32 dungeonId);

	/**
	*@brief 公会战是否有效期间
	*/
	bool IsValidGuildBattle();

	/**
	*@brief 获取公会参与领地类型
	*/
	UInt32 GetGuildTerritoryType(UInt64 guildId);

public:
	//跨服工会战

	/**
	*@brief 获取跨服工会战报名信息
	*/
	UInt32 GetCrossEnrollInfo(std::map<UInt8, std::vector<CSEnrollInfo>>& enrollInfo, std::vector<GuildTerritoryBaseInfo>& baseInfos);

	/**
	*@brief 初始化跨服工会战信息
	*/
	void InitCrossSortListInfo();

	/**
	*@brief 获取跨服工会战领地信息
	*/
	UInt32 SetCrossTerritoryInfo(GuildTerritoryBaseInfo& info);

	/**
	*@brief 获取跨服工会战领地信息
	*/
	UInt32 SetCrossTerritoryInfos(std::vector<GuildTerritoryBaseInfo>& infos);

	/**
	*@brief 公会战是否加载完成
	*/
	bool IsGuildLoad() { return m_IsGuildLoad; }

	/**
	*@brief 公会解散处理
	*/
	void GuildDismiss(ObjID_t id);

public:
	// 工会战事件

	void OnTick(Avalon::Time now);

	/**
	*@brief 报名开始
	*/
	void OnEnrollBegin();

	/**
	*@brief 报名结束
	*/
	void OnEnrollEnd();

	/**
	*@brief 战斗准备
	*/
	void OnBattlePrepare();

	/**
	*@brief 战斗开始
	*/
	void OnBattleBegin();

	/**
	*@brief 战斗结束
	*/
	void OnBattleEnd();

	/**
	*@brief 宣战战斗结束
	*/
	void OnChallengeBattleEnd();

	/**
	*@brief 跨服工会战结束
	*/
	void OnCrossBattleEnd();

	/**
	*@brief 领奖
	*/
	void OnBattleReward();

	/**
	*@brief Tick
	*/
	void OnDayChanged();

	/**
	*@brief 公会信息加载完成
	*/
	void OnGuildLoadFinish();

	/**
	*@brief 同步新记录
	*/
	void SyncBattleRecord(UInt8 terrId, CLProtocol::WorldGuildBattleRecordSync sync);

	/**
	*@brief 获取领地ID
	*/
	UInt8 GetTerritoryId(UInt64 guildId);

	/**
	*@brief 设置领地公会
	*/
	void SetTerrGuild(Guild* guild);

	/**
	*@brief 设置占领领地公会
	*/
	void SetCrossTerrGuild(Guild* guild);

	/**
	*@brief 公会战是否处于开启状态
	*/
	bool IsGuildBattle();

	/**
	*@brief 公会是否处于报名中
	*/
	bool IsEnroll(Guild* guild);

	/**
	*@brief 公会战报名是否需要通知
	*/
	bool IsNotifyGuildEnroll(Guild* guild);

	/**
	*@brief 转换到目标公会战状态
	*/
	void ChangeTargetStatus(GuildBattleStatus targetStatus);

	/**
	*@brief 设置公会战类型
	*/
	void SetGuildBattleType(GuildBattleType type) { m_Type = type; };

	/**
	*@brief 是否普通工会战
	*/
	bool IsNormalBattle();

	/**
	*@brief 是否公会宣战
	*/
	bool IsChallengeBattle();

	/**
	*@brief 是否跨服工会战
	*/
	bool IsCrossBattle();

	/**
	*@brief 获取宣战领地ID
	*/
	UInt8 GetGuildBattleChallengeTerrId();

	/**
	*@brief 获取宣战物品ID
	*/
	UInt32 GetGuildBattleChallengeItemId();

	/**
	*@brief 获取宣战物品数量
	*/
	UInt32 GetGuildBattleChallengeItemNum();

	/**
	*@brief 清理领地报名事务
	*/
	void ClearTerritoryTransaction(UInt8 terrId);

	/**
	*@brief 同步公会战宣战信息
	*/
	void SyncGuildBattleChallengeInfoToPlayers();

	/**
	*@brief 是否抽奖状态
	*/
	bool IsGuildBattleLottery();

	/**
	*@brief 下发跨服工会战结束消息
	*/
	void SyncCrossGuildBattleEndInfo(std::vector<GuildBattleEndInfo> info);

private:
	/**
	*@brief 公会战是否可以报名
	*/
	bool _IsGuildBattleEnroll();

	/**
	*@brief 是否已经报名
	*/
	bool _IsEnroll(UInt64 guildId);

	/**
	*@brief 清理报名信息
	*/
	void _ClearEnroll();

	/**
	*@brief 清理领地信息
	*/
	void _ClearTerritory();

	/**
	*@brief 清理跨服领地报名信息
	*/
	void _ClearTerritoryCrossEnrollInfo();

	/**
	*@brief 清理领地信息
	*/
	void _ClearTerritory(UInt8 terrId);

	/**
	*@brief 清理跨服领地报名信息
	*/
	void _ClearTerritoryCrossEnrollInfo(UInt8 terrId);

	/**
	*@brief 获取领地信息
	*/
	GuildBattleTerritory* _GetGuildBattleTerritory(UInt8 terrId);

	/**
	*@brief 公会战结束播放公告
	*/
	void _BattleEndAnnouncement(GuildTerritoryType type = GuildTerritoryType::GTT_NORMAL);

	/**
	*@brief 公会战一场局
	*/
	void _BattleInvalid(UInt64 playerA, UInt64 playerB, UInt8 result);

	/**
	*@brief 给所有参加公会战的成员发送消息
	*/
	void _SendBattleMember(Avalon::Protocol &protocol);

	/**
	*@brief 记录数据统计
	*/
	void _SendGuildPvpUdpLog(GuildMember* winGuildMember, UInt32 winOldGuildScore, GuildMember* loseGuildMember, UInt32 loseOldGuildScore, UInt8 result);

	/**
	*@brief 初始化公会战状态
	*/
	void _InitGuildBattleStatus();

	/**
	*@brief 保存公会战状态
	*/
	void _SaveGuildBattleStatus();

	/**
	*@brief 初始化公会战宣战竞价
	*/
	void _InitGuildBattleChallengePrice();

	/**
	*@brief 保存公会战宣战竞价
	*/
	void _SaveGuildBattleChallengePrice();

	/**
	*@brief 同步公会宣战信息
	*/
	UInt32 _SyncGuildBattleChallengeInfo(WSPlayer* player);

	/**
	*@brief 同步公会战状态
	*/
	void _SyncGuildBattleStatus();

	/**
	*@brief 设置公会战状态
	*/
	void _SetGuildBattleStatus(GuildBattleStatus status);

	/**
	*@brief 执行状态转换
	*/
	void _ChangeToTargetStatus();

	/**
	*@brief 获取宣战的公会
	*/
	Guild* _GetEnrollChallengeGuild();

	/**
	*@brief 重置宣战的价格
	*/
	void _ResetGuildBattleChallengePrice();

	/**
	*@brief 跳过这次公会战
	*/
	void _SkipGuildBattle();

	/**
	*@brief 每日玩家领奖状态刷新
	*/
	void _DayMemberTerrRewardRefresh(bool isClear);

private:
	// 报名信息
	HashMap<UInt8, std::vector<ObjID_t> >		m_Enroll;

	// 各个领地信息
	HashMap<UInt8, GuildBattleTerritory>		m_Territory;

	//当前状态
	GuildBattleStatus							m_Status;

	//目标状态
	GuildBattleStatus							m_TargetStatus;

	//当前公会战类型
	GuildBattleType								m_Type;

	//公会战时间
	GuildBattleTimeInfo							m_GuildBattleTimeInfo;

	//公会战结果
	std::vector<GuildBattleEndInfo>				m_GuildBattleEndInfo;

	//公会宣战竞价价格
	UInt32										m_GuildBattleChallengePrice;

	//公会宣战竞价玩家
	ObjID_t										m_GuildBattleChallengePlayer;

	//公会宣战预报名列表,关注价格变化的玩家
	std::vector<ObjID_t>						m_GuildBattlePreEnrollPlayers;

	//数据是否加载完成
	bool										m_IsGuildLoad;

	//是否根据自然时间
	bool										m_IsByTime;
};

#endif