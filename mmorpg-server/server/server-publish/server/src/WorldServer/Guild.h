#ifndef __GUILD_H__
#define __GUILD_H__

#include <AvalonSimpleTimer.h>
#include <CLSyncObject.h>
#include <CLGuildDefine.h>
#include <CLSceneObject.h>
#include <CLObjectIndex.h>
#include <CLObjectProperty.h>
#include <CLChatDefine.h>
#include <CLItemDefine.h>
#include "WorldSysNotify.h"
#include "GuildMember.h"
#include "GuildBuilding.h"
#include "GuildDonate.h"
#include "GuildTable.h"
#include "GuildStorage.h"
#include "GuildEventMgr.h"
#include "GuildHonorMgr.h"

struct GuildDataEntry;
struct GuildEntry;
struct GuildSkillDataEntry;
struct RedPacketDataEntry;

class GuildBattleTerritory;
class GuildEmblemTransaction;

enum GUILD_INIT_TYPE
{
	GUILD_INIT_TYPE_CREATE = 0,
	GUILD_INIT_TYPE_LOAD = 1,
};


class GuildComparer
{
public:
	bool operator()(Guild* a, Guild* b);
};

class Guild : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(Guild, Avalon::NullMutex)
public:
	// 公会申请列表最大人数
	const UInt32 GUILD_REQUEST_QUEEU_MAX_SIZE = 50;

	Guild();
	~Guild();

	bool Init(GUILD_INIT_TYPE type);

	void OnTick(const Avalon::Time& now);

public:
	// 公会事件

	/**
	*@brief 解散帮会
	*/
	void OnDismiss(UInt8 dismissType);

	/**
	*@brief 更换了会长
	*/
	void OnChangeLeader(GuildMember* newLeader);

	/**
	*@brief 换了一天
	*/
	void OnDayChanged();

	/**
	*@brief 换了一周
	*/
	void OnWeekChanged();

	/**
	*@brief 检测会长自动转入
	*/
	void CheckAutoChangeLeader();

public:
	/**
	*@brief 名字
	*/
	inline std::string GetName() { return m_Name; }
	inline void SetName(std::string name) { m_Name = name; }

	/**
	*@brief 等级
	*/
	inline UInt8 GetLevel() { return m_Level; }
	void SetLevel(UInt8 level);

	/**
	*@brief 会长
	*/
	inline GuildMember* GetLeader() const { return m_Leader; }
	inline void SetLeader(GuildMember* leader) { m_Leader = leader; }
	inline void SetLeaderName(const std::string& name) { m_LeaderName = name; }

	/**
	*@brief 宣言
	*/
	inline const std::string& GetDeclaration() const { return m_Declaration; }
	inline void SetDeclaration(std::string declaration) { m_Declaration = declaration; }

	/**
	*@brief 成立时间
	*/
	inline void SetUpTime(UInt32 time) { m_SetupTime = time; }
	inline UInt32 GetSetUpTime() const { return m_SetupTime; }

	/**
	*@brief 解散时间
	*/
	inline UInt32 GetDismissTime() const { return m_DismissTime; }
	inline void SetDismissTime(UInt32 time) { m_DismissTime = time; }

	/**
	*@brief 资产
	*/
	inline UInt32 GetFund() const { return m_Fund; }
	inline void AddFund(UInt32 fund) { m_Fund = m_Fund + fund; m_WeekAddedFund = m_WeekAddedFund + fund; }

	/**
	*@brief 成员管理
	*/
	bool AddMember(GuildMember* member);
	GuildMember* FindMember(ObjID_t id);
	void DelMember(GuildMember* member);
	void GetGuildMemberList(std::vector<GuildMemberEntry>& memberList);
	void GetGuildMemberListByPost(std::vector<GuildMember*>& memberList, GuildPost post);
	void GetGuildMembers(std::vector<GuildMember*>& memberList);
	UInt32 GetMemberNum() { return m_Members.size(); }

	/**
	*@brief 获取公会信息
	*/
	void GetGuildEntry(WSPlayer* player, GuildEntry& entry);

	/**
	*@brief 公会是否已满
	*/
	bool IsGuildFull() const;

	/**
	*@brief 公会建筑
	*/
	inline UInt8 GetBuildingLevel(GuildBuildingType type) const { return m_BuildingMgr.GetLevel(type); };

	/**
	*@brief 报名的公会领地
	*/
	inline UInt8 GetEnrollTerrID() const { return m_EnrollTerrId; }
	inline void SetEnrollTerrID(UInt8 id) { m_EnrollTerrId = id; }

	/**
	*@brief 占领的公会领地
	*/
	inline UInt8 GetOccupyTerrID() const { return m_OccupyTerrId; }
	inline void SetOccupyTerrID(UInt8 id) { m_OccupyTerrId = id; }

	/**
	*@brief 占领的公会跨服领地
	*/
	inline UInt8 GetOccupyCrossTerrID() const { return m_OccupyCrossTerrId; }
	inline void SetOccupyCrossTerrID(UInt8 id) { m_OccupyCrossTerrId = id; }

	/**
	*@brief 公会战积分
	*/
	inline UInt32 GetGuildBattleScore() const { return m_GuildBattleScore; }
	inline void SetGuildBattleScore(UInt32 score) { m_GuildBattleScore = score; }

	/**
	*@brief 公会战鼓舞
	*/
	inline UInt8 GetInspire() const { return m_Inspire; }

	/**
	*@brief 公会战成员战斗次数
	*/
	inline UInt32 GetGuildMemberBattleCount() const { return m_GuildMemberBattleCount; }

	/**
	*@brief 公会战鼓舞技能ID
	*/
	inline UInt16 GetInspireSkillId() const { return m_InspireSkillId; }

	/**
	*@brief 历史占领领地
	*/
	inline UInt8 GetHistoryTerrId() const { return (UInt8)m_HistoryTerrId; }
	inline void SetHistoryTerrId(UInt8 terrId) { m_HistoryTerrId = (UInt32)terrId; };

	/**
	*@brief 公会战胜利抽奖几率
	*/
	inline UInt8 GetWinProbability() const { return m_WinProbability; }

	/**
	*@brief 公会战失败抽奖几率
	*/
	inline UInt8 GetLoseProbability() const { return m_LoseProbability; }

	/**
	*@brief 公会仓库放入权限
	*/
	inline UInt8 GetStorageAddPost() const { return m_StorageAddPost; }

	/**
	*@brief 公会仓库放入权限
	*/
	inline UInt8 GetStorageDelPost() const { return m_StorageDelPost; }

	/**
	*@brief 公会入会等级
	*/
	inline UInt32 GetJoinLevel() const { return m_JoinLevel; }
	inline void SetJoinLevel(UInt32 level) { m_JoinLevel = level; }

	/**
	*@brief 设置公会地下城boss难度
	*/
	inline UInt32 GetGuildDungeonType() const { return m_GuildDungeonType ? m_GuildDungeonType : 1; }
	inline void SetGuildDungeonType(UInt32 bossType) { m_GuildDungeonType = bossType; }

	/**
	 *@brief 工会建筑最后更改时间
	 */
	inline UInt32 GetLastUpdateGuildBuildingTime() {return m_LastUpdateGuildBuildingTime;}
	inline void SetLastUpdateGuildBuildingTime(UInt32 LastTime) { m_LastUpdateGuildBuildingTime = LastTime; }
public:
	/* 申请加入公会相关 */

	/**
	*@brief 申请列表是否已满
	*/
	bool IsRequestQueueFull() const;

	/**
	*@brief 是否已经申请了
	*/
	bool IsRequesting(ObjID_t id) const;

	/**
	*@brief 删除申请者
	*/
	void DelRequester(ObjID_t id);

	/**
	*@brief 通知玩家已经没有申请者了
	*/
	void NotifyRequesterIsEmpty();

	/**
	*@brief 请求加入公会
	*/
	UInt32 RequestJoinGuild(WSPlayer* player);

	/**
	*@brief 回复加入请求
	*/
	void ReplyJoinRequest(WSPlayer* player, ObjID_t targetId, bool agree);

	/**
	*@brief 清空请求列表
	*/
	void RequestClearRequestList(WSPlayer* player);

	/**
	*@brief 获取请求者列表
	*/
	std::vector<GuildRequesterInfo> GetRequesterList();

public:
	/**
	*@brief 请求修改公会名
	*/
	void RequestChangeGuildName(WSPlayer* player, const std::string& name, ObjID_t itemId, UInt32 itemTypeId);

	/**
	*@brief 请求修改公会公告
	*/
	void RequestChangeAnnouncement(WSPlayer* player, const std::string& content);

	/**
	*@brief 请求修改公会宣言
	*/
	void RequestChangeDeclaration(WSPlayer* player, const std::string& content);

	/**
	*@brief 发送公会邮件
	*/
	void SendGuildMail(WSPlayer* player, const std::string& content);

	/**
	*@brief 修改职务
	*/
	void RequestChangePost(WSPlayer* player, ObjID_t targetId, ObjID_t replaceId, GuildPost post);

	/**
	*@brief 踢人
	*/
	void RequestKickPlayer(WSPlayer* player, ObjID_t targetId);

	/**
	*@brief 离开公会
	*/
	void RequestLeave(WSPlayer* player);

	/**
	*@brief 升级建筑
	*/
	void RequestUpgradeBuilding(WSPlayer* player, GuildBuildingType type);

	/**
	*@brief 申请成为会长
	*/
	UInt32 RequestBecomeLeader(WSPlayer* player);

	/**
	*@brief 请求解散
	*/
	void RequestDismiss(WSPlayer* player);

	/**
	*@brief 请求取消
	*/
	void RequestCancelDismiss(WSPlayer* player);

	/**
	*@brief 取消解散通知
	*/
	void CancelDismissNotify();

	/**
	*@brief 请求发送自费红包
	*/
	UInt32 OnRequestPayRedPacket(WSPlayer* player, UInt16 reason, std::string name, UInt8 num);

	/**
	*@brief 检查公会自费红包是否可以发送
	*/
	UInt32 CheckGuildRedPacketCanBuy(WSPlayer* player, RedPacketDataEntry& dataEntry);

	/**
	*@brief 请求设置加入等级
	*/
	void RequestSetJoinLevel(WSPlayer* player, UInt32 joinLevel);

public:
	/**
	*@brief 修改公会名事务返回
	*/
	bool OnCheckChangeGuildNameRet(WSPlayer* player, const std::string& name);

	/**
	*brief
	*/

	/**
	*@brief 发送公会邮件事务返回
	*/
	bool OnCheckMailCounterRet(WSPlayer* player, const std::string& content);

	/**
	*@brief 解散工会事务返回
	*/
	bool OnCheckDismissRet(WSPlayer* player);

	/**
	*@brief 公会自费红包事务返回
	*/
	UInt32 OnCheckPayRedPacketRet(WSPlayer* player, UInt16 reason, std::string name, UInt8 num);

public:
	// 公会福利所

	/**
	*@brief 请求捐赠
	*/
	void RequestDonate(WSPlayer* player, GuildDonateType type, UInt8 num);

	/**
	*@brief 捐赠事务返回
	*/
	bool OnCheckDonateRet(WSPlayer* player, GuildDonateType type, UInt8 num);

	/**
	*@brief 获取捐赠日志
	*/
	std::vector<GuildDonateLog> GetDonateLogs();

public:
	// 公会战争所

	/**
	*@brief 请求升级技能
	*/
	void RequestUpgradeSkill(WSPlayer* player, UInt16 skillId);

	/**
	*@brief 请求升级技能
	*/
	bool OnCheckUpgradeSkillRet(WSPlayer* player, UInt16 skillId, UInt8 level);

public:
	// 公会商店

	/**
	*@brief 刷新商店
	*/
	void RefreshShop();

	/**
	*@brief 刷新商店事件
	*/
	void OnRefreshShop();

public:
	// 雕像

	/**
	*@brief 获取会长信息
	*/
	void RequestLeaderInfo(WSPlayer* player);

	/**
	*@brief 请求膜拜
	*/
	void RequestOrz(WSPlayer* player, GuildOrzType type);

	/**
	*@brief 检测膜拜返回
	*/
	bool CheckOrzRet(WSPlayer* player, GuildOrzType type);

public:
	// 圆桌会议

	/**
	*@brief 参加圆桌会议
	*/
	void RequestJoinTable(WSPlayer* player, UInt8 seat, UInt8 type);

	/**
	*@brief 参加圆桌会议
	*/
	bool CheckJoinTable(WSPlayer* player, UInt8 seat, UInt8 type);

	/**
	*@brief 圆桌会议完成
	*/
	void OnTableFull();

public:
	//公会战

	/**
	*@brief 结束一场公会战
	*/
	void OnGuildBattleRaceEnd(UInt32 terrId, UInt32 score);

	/**
	*@brief 公会战结束
	*/
	void ClearEnroll();

	/**
	*@brief 鼓舞
	*/
	UInt32 Inspire();

	/**
	*@brief 获取鼓舞信息
	*/
	UInt32 GetGuildBattleInspireInfo(std::vector<GuildBattleInspireInfo>& infos);

	/**
	*@brief 设置公会仓库权限和几率
	*/
	UInt32 SetGuildStorageSetting(GuildStorageSetting type, UInt32 value);

	/**
	*@brief 发放奖励
	*/
	void SendGuildBattleReward(const std::vector<ItemReward>& leaderReward, const std::vector<ItemReward>& memberReward);

	/**
	*@brief 发放失败奖励
	*/
	void SendGuildBattleLoseReward(std::vector<ItemReward> itemReward);

	/**
	*@brief 发放会长奖励
	*/
	void SendGuildLeaderReward(std::vector<ItemReward> itemReward);

	/**
	*@brief 发放成员奖励
	*/
	void SendGuildMemberReward(std::vector<ItemReward> itemReward);

	/**
	*@brief 公会战雕像
	*/
	void SetGuildBattleFigureStatue();


	/**
	*@brief 获取仓库物品
	*/
	void GetStorageItems(std::vector<GuildStorageItemInfo>& items);

	/**
	*@brief 获取仓库记录
	*/
	void GetStorageRecords(std::vector<GuildStorageOpRecord>& record);

	/**
	*@brief 删除仓库物品
	*/
	UInt32 DelStorageItems(std::vector<GuildStorageDelItemInfo>& delItems, std::vector<GuildStorageItemInfo>& itemInfos);

	/**
	*@brief 插入仓库记录
	*/
	void InsertStorageItemRecords(std::string name, GuildStorageOpType type, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& records);

	/**
	*@brief 获取仓库最大数量
	*/
	UInt32 GetStorageMaxSize();

	/**
	*@brief 返还宣战费用
	*/
	void ResultChallengeEnroll(std::vector<ItemReward> itemReward, std::string guildName, UInt32 price);

	/**
	*@brief 设置报名玩家id
	*/
	void SetEnrollPlayer(UInt64 playerId);

	/**
	*@brief 获取报名玩家id
	*/
	UInt64 GetEnrollPlayer();

	/**
	*@brief 抽奖
	*/
	UInt32 Lottery(WSPlayer* player, bool isWin, bool isCheck = true);

	/**
	*@brief 发放公会贡献
	*/
	UInt32 GiveContribution(WSPlayer* player);

	/**
	*@brief 公会成员添加贡献
	*/
	void MemberAddContri(GuildMember* member, UInt32 contri, std::string reason);

	/**
	*@brief 公会占领领地
	*/
	void OnGuildOccupyTerritory(UInt8 terrId);

	/**
	*@brief 公会战报名通知
	*/
	void OnGuildBattleEnrollMail(UInt8 terrId, GuildMember* member = NULL);

	/**
	*@brief 公会战排名积分奖励
	*/
	void OnGuildBattleScoreRankReward(UInt32 titleId, UInt32 expireTime);

public:
	/* 公会仓库 */
	void AddStorageItem(WSPlayer* player, std::vector<GuildStorageItemInfo>& inItems, AddGuildStorageItemCb* cb);
	
	void AddStorageItemCb(UInt64 playerUid, UInt32 code, std::vector<WSItem*>& items);

	void GetStorageItemCb(UInt64 playerUid, WSItem* item, UInt64 oldUid);

	WSItem* QueryStorageItem(UInt64 uid);

	bool CheckStorageCapacity(WSItem* item);

	bool ShopBuyAddStorage(WSPlayer* player, WSItem* item);

public:
	/* 玩家事件 */

	/**
	*@brief 玩家上线
	*/
	void OnPlayerOnline(WSPlayer* player, bool dayChanged);

	/**
	*@brief 玩家下线
	*/
	void OnPlayerOffline(WSPlayer* player);

	/**
	*@brief 新加入公会
	*/
	void OnMemberEnter(WSPlayer* replyPlayer, GuildMember* member);

	/**
	*@brief 玩家在scene上获得贡献
	*/
	void OnScenePlayerAddContri(ObjID_t roleId, UInt32 num);

	/**
	*@brief 账号永久性删除
	*/
	void OnPlayerForeverDeleted(ObjID_t roleId);

public:
	/* 消息相关 */

	/**
	*@brief 广播消息
	*/
	void Broadcast(Avalon::Protocol& protocol);
	void Broadcast(Avalon::Packet* packet);
	void Broadcast(Avalon::Protocol& protocol, UInt32 playerLevelLimit);

	void Broadcast(Avalon::Protocol& protocol, const std::function<void(WSPlayer*, Avalon::Protocol*)>& callback);
	
	/**
	*@brief 广播给有这个权限的人消息
	*/
	void BroadcastByPower(Avalon::Protocol& protocol, GuildPowerFlag power);
	void BroadcastByPower(Avalon::Packet* packet, GuildPowerFlag power);

	/**
	*@brief 同步公会基本信息
	*/
	void SyncBaseInfo(GuildMember* member);

	/**
	*@brief 同步公会基本信息
	*/
	void BroadcastInfo();

	/**
	*@brief 同步成员信息到scene
	*/
	void SyncMemberInfoToScene(GuildMember* member, bool leaveGuild = false);

	/**
	*@brief 同步徽记到scene
	*/
	void SyncEmblemToScene(GuildMember* member);

	/**
	*@brief 同步跨服信息到Scene
	*/
	void SyncCrossOccupyToScene();

	/**
	*@brief 同步领地信息到Scene
	*/
	void SyncOccupyToScene();

	/**
	*@brief 广播提示
	*/
	template <typename... Args>
	void SendNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.channel = 0;
		notify.color = color;
		notify.word = buffer;
		Broadcast(notify);
	}

	/**
	*@brief 发送公会聊天
	*/
	void Chat(const std::string& content, bool bLink = false);

	/**
	*@brief 更新公会等级排行榜
	*/
	void UpdateGuildLevelSortList();

	/**
	*@brief 升级徽记
	*/
	void HandleEmblemUpgrade(WSPlayer* player);

	/**
	*@brief 徽记事务返回
	*/
	void EmblemUpgradeTransaction(WSPlayer* player, UInt32 emblemLvl, GuildEmblemTransaction* tran);

	/**
	*@brief 公会副本难度设置
	*/
	void HandleGuildDungeonType(WSPlayer* player, UInt32 dType);

	/**
	*@brief 公会领地每日奖励领取
	*/
	UInt32 HandleGuildTerrDayReward(WSPlayer* player);

	/**
	*@brief 每日清除公会成员领地每日奖励领取标记
	*/
	void ClearMemberTerrDayReward(bool isClear);

	/**
	*@brief 更新公会战每周积分排行榜
	*/
	void UpdateGuildBattleWeekScoreRank(UInt32 addScore);

public:
	/* 数据库相关 */
	void SaveToDB();
	void DelFromDB();
	void InsertToDB();


	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("name", m_Name)
	CL_DBPROPERTY("level", m_Level)
	CL_DBPROPERTY("setuptime", m_SetupTime);
	CL_DBPROPERTY("declaration", m_Declaration)
	CL_DBPROPERTY("fund", m_Fund)
	CL_DBPROPERTY("announcement", m_Announcement)
	CL_DBPROPERTY("buildings", m_BuildingMgr)
	CL_DBPROPERTY("donate_logs", m_DonateLogMgr)
	CL_DBPROPERTY("shopid", m_ShopId)
	CL_DBPROPERTY("dismiss_time", m_DismissTime)
	CL_DBPROPERTY("popularoty", m_LeaderPopularoty)
	CL_DBPROPERTY("table", m_Table)
	CL_DBPROPERTY("enroll_terrid", m_EnrollTerrId)
	CL_DBPROPERTY("battle_score", m_GuildBattleScore)
	CL_DBPROPERTY("occupy_terrid", m_OccupyTerrId)
	CL_DBPROPERTY("inspire", m_Inspire)
	CL_DBPROPERTY("win_probability", m_WinProbability)
	CL_DBPROPERTY("lose_probability", m_LoseProbability)
	CL_DBPROPERTY("storage_add_post", m_StorageAddPost)
	CL_DBPROPERTY("storage_del_post", m_StorageDelPost)
	CL_DBPROPERTY("enroll_player", m_EnrollPlayerId)
	CL_DBPROPERTY("occupy_cross_terrid", m_OccupyCrossTerrId)
	CL_DBPROPERTY("history_terrid", m_HistoryTerrId)
	CL_DBPROPERTY("join_level", m_JoinLevel)
	CL_DBPROPERTY("dungeon_type", m_GuildDungeonType);
	CL_DBPROPERTY("prosperity_status", m_ProsperityStatus);
	CL_DBPROPERTY("week_added_fund", m_WeekAddedFund);
	CL_DBPROPERTY("merger_request_status", m_MergerRequsetStatus);
	CL_DBPROPERTY("goal_guild_id", m_MergerGoalGuildID);
	CL_DBPROPERTY("merger_request_time", m_MergerRequestTime);
	CL_DBPROPERTY("merger_accept_time", m_MergerRequestAccpetTime);
	CL_DBPROPERTY("merger_other_time", m_MergerOtherGuildTime);
	CL_DBPROPERTY("last_week_added_fund", m_LastWeekAddedFund);
	CL_DBPROPERTY("last_update_guild_building_time", m_LastUpdateGuildBuildingTime);
	CL_DBPROPERTY("honor_log", m_GuildHonorMgr);
	CL_DBPROPERTY_END()

public://同步相关	
	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
		CL_SYNC_PROPERTY(m_Name, GA_NAME, SST_WORLD)
		CL_SYNC_PROPERTY(m_Level, GA_LEVEL, SST_WORLD)
		CL_SYNC_PROPERTY(m_Declaration, GA_DECLARATION, SST_WORLD)
		CL_SYNC_PROPERTY(m_Fund, GA_FUND, SST_WORLD)
		CL_SYNC_PROPERTY(m_Announcement, GA_ANNOUNCEMENT, SST_WORLD)
		CL_SYNC_PROPERTY(m_BuildingMgr, GA_BUILDING, SST_WORLD)
		CL_SYNC_PROPERTY(m_DismissTime, GA_DISMISS_TIME, SST_WORLD)
		CL_SYNC_PROPERTY(m_MemberNum, GA_MEMBER_NUM, SST_WORLD)
		CL_SYNC_PROPERTY(m_LeaderName, GA_LEADER_NAME, SST_WORLD)
		CL_SYNC_PROPERTY(m_EnrollTerrId, GA_ENROLL_TERRID, SST_WORLD)
		CL_SYNC_PROPERTY(m_GuildBattleScore, GA_BATTLE_SCORE, SST_WORLD)
		CL_SYNC_PROPERTY(m_OccupyTerrId, GA_OCCUPY_TERRID, SST_WORLD)
		CL_SYNC_PROPERTY(m_Inspire, GA_INSPIRE, SST_WORLD)
		CL_SYNC_PROPERTY(m_WinProbability, GA_WIN_PROBABILITY, SST_WORLD)
		CL_SYNC_PROPERTY(m_LoseProbability, GA_LOSE_PROBABILITY, SST_WORLD)
		CL_SYNC_PROPERTY(m_StorageAddPost, GA_STORAGE_ADD_POST, SST_WORLD)
		CL_SYNC_PROPERTY(m_StorageDelPost, GA_STORAGE_DEL_POST, SST_WORLD)
		CL_SYNC_PROPERTY(m_OccupyCrossTerrId, GA_OCCUPY_CROSS_TERRID, SST_WORLD)
		CL_SYNC_PROPERTY(m_HistoryTerrId, GA_HISTORY_TERRID, SST_WORLD)
		CL_SYNC_PROPERTY(m_JoinLevel, GA_JOIN_LEVEL, SST_WORLD)
		CL_SYNC_PROPERTY(m_GuildDungeonType, GA_DUNGEON_TYPE, SST_WORLD)
		CL_SYNC_PROPERTY(m_LastWeekAddedFund, GA_LAST_WEEK_ADD_FUND, SST_WORLD)
		CL_SYNC_PROPERTY(m_WeekAddedFund, GA_THIS_WEEK_ADD_FUND, SST_WORLD)

	CL_SYNC_PROPERTY_END()

private:
	/* 内部接口 */

	/**
	*@brief 获取不同职务的最大人数，-1代表无限
	*/
	UInt32 _GetMaxNumByPost(GuildPost post);

	/**
	*@brief 获取不同职务的人数
	*/
	UInt32 _GetMemberNumByPost(GuildPost post);

	/**
	*@brief 当前职务能否任命目标职务
	*/
	bool _CanSetPost(GuildPost post, GuildPost targetPost);

	/**
	*@brief 获取捐赠消耗
	*/
	UInt32 _GetDonateCost(GuildDonateType type);

	/**
	*@brief 获取捐赠消耗
	*/
	UInt8 _GetDonateMaxCount(WSPlayer* player, GuildDonateType type);

	/**
	*@brief 获取counter名字
	*/
	const char* _GetDonateCounter(GuildDonateType type);

	/**
	*@brief 获取捐献获得的公会贡献
	*/
	UInt32 _GetDonateContribution(GuildDonateType type, UInt8 num);

	/**
	*@brief 获取商店ID
	*/
	UInt8 _GetShopIDByLevel();

	/**
	*@brief 发送公会邮件
	*/
	void _BroadcastMail(const std::string& title, const std::string& sender, const std::string& content, const std::string& reason);

	/**
	*@brief 发送公会奖励邮件
	*/
	UInt32 _GetOrzMaxTime();

	/**
	*@brief 获取圆桌会议计数名字
	*/
	std::string _GetTableCounterName(UInt8 help);

	/**
	*@brief 获取圆桌会议最大次数
	*/
	UInt32 _GetTableMaxTime(UInt8 help);

	/**
	*@brief 计算一个新的公会会长
	*/
	GuildMember* _GenNewLeader();

	/**
	*@brief 更新公会成员数
	*/
	void UpdateMemberNum();

public:
	static GuildSkillDataEntry* FindSkillEntry(UInt16 skillId, UInt8 level);

public: //公会事件相关

	/**
	 *@brief  初始化公会事件管理器的公会ID
	 */
	inline void SetGuildEventGuildID() { m_GuildEventMgr.SetGuildID(GetID()); };

	/**
	 *@brief  加载公会事件
	 */
	inline void LoadGuildEvent(GuildEvent* guildEvent) { m_GuildEventMgr.LoadGuildEvent(guildEvent); }

	/**
	 *@brief  添加一条公会事件
	 */
	inline bool AddGuildEvent(std::string data) { Chat(data); return m_GuildEventMgr.addGuildEvent(data); }

	/**
	 *@brief  删除所有公会事件
	 */
	inline void DelGuildEvent() { m_GuildEventMgr.DelDBData(); }

	/**
	 *@brief  获取公会事件的总数量
	 */
	inline UInt16 GetGuildEventSize() { return m_GuildEventMgr.GetGuildEventSize(); }

	/**
	 *@brief  获取公会事件
	 */
	inline void GetGuildEventPage(std::vector<GuildEvent>& guildEvents, UInt32 upTime) { m_GuildEventMgr.GetGuildEvent(guildEvents, upTime); };

private:
	// 公会表
	GuildDataEntry*				m_DataEntry;

	// 名字
	ObjString					m_Name;

	// 公会等级
	ObjUInt8					m_Level;

	// 会长
	GuildMember*				m_Leader;

	// 会长名字
	ObjString					m_LeaderName;

	// 成员列表
	HashMap<ObjID_t, GuildMember*>	m_Members;

	// 成员数量
	ObjUInt16					m_MemberNum;

	//成立时间
	ObjUInt32					m_SetupTime;

	// 宣言
	ObjString					m_Declaration;

	// 公告
	ObjString					m_Announcement;

	// 资产
	ObjUInt32					m_Fund;

	// 公会建筑
	GuildBuildingMgr			m_BuildingMgr;

	// 捐赠日志
	GuildDonateLogMgr			m_DonateLogMgr;

	// 公会商店ID
	ObjUInt8					m_ShopId;

	// 公会商店刷新时间
	UInt32						m_ShopRefreshTime;

	// 请求加入公会的人
	MapGuildRequesterInfo		m_Requesters;

	// 解散时间
	ObjUInt32					m_DismissTime;

	// 会长人气
	ObjUInt32					m_LeaderPopularoty;

	//保存定时器
	Avalon::SimpleTimer			m_SaveTimer;

	//同步信息定时器
	Avalon::SimpleTimer			m_SyncTimer;

	// 圆桌会议
	GuildTable					m_Table;

	// 工会战相关
	// 报名的领地ID
	ObjUInt8					m_EnrollTerrId;
	// 工会战积分
	ObjUInt32					m_GuildBattleScore;
	// 已经占领的领地ID
	ObjUInt8					m_OccupyTerrId;
	// 已经占领的跨服领地ID
	ObjUInt8					m_OccupyCrossTerrId;
	//鼓舞次数
	ObjUInt8					m_Inspire;
	//公会战成员战斗次数
	UInt32						m_GuildMemberBattleCount;
	//公会战鼓舞技能ID
	UInt16						m_InspireSkillId;

	//历史领地
	ObjUInt32					m_HistoryTerrId;

	//公会战胜利抽奖几率
	ObjUInt8					m_WinProbability;
	//公会战失败抽奖几率
	ObjUInt8					m_LoseProbability;

	//仓库放入物品权限
	ObjUInt8					m_StorageAddPost;
	//仓库删除物品权限
	ObjUInt8					m_StorageDelPost;

	//公会战报名玩家id
	ObjUInt64					m_EnrollPlayerId;

	// 公会仓库
	GuildStorage				m_guildStorage;
	// 入会等级
	ObjUInt32					m_JoinLevel;
	//公会boss难度
	ObjUInt32					m_GuildDungeonType;
	//公会繁荣度状态
	ObjUInt8                    m_ProsperityStatus;
	//公会本周增加的繁荣度
	ObjUInt32                   m_WeekAddedFund;
	//公会上周增加的繁荣度
	ObjUInt32                   m_LastWeekAddedFund;
	//兼并别的公会的请求状态
	ObjUInt8                    m_MergerRequsetStatus;
	//兼并目标
	ObjUInt64                   m_MergerGoalGuildID;
	//兼并请求时间
	ObjUInt64                   m_MergerRequestTime;
	//兼并请求被接受的
	ObjUInt64                   m_MergerRequestAccpetTime;
	//上次兼并别的公会的时间
	ObjUInt64                   m_MergerOtherGuildTime;
	//收到的别的公会的兼并请求列表
	std::set<ObjID_t>           m_HavedMergerRequest;
	//公会事件管理器
	GuildEventMgr				m_GuildEventMgr;

	// 公会荣誉
	GuildHonorMgr              m_GuildHonorMgr;

	//工会建筑最后修改时间
	ObjUInt32					m_LastUpdateGuildBuildingTime;
private:
	//同步兼并信息，包括繁荣度状态和自己的请求状态
	void SyncMergerInfo();
	void SendMergerUDPLog(UInt32 result);
public:
	//获取繁荣度状态
	inline UInt8 GetProsperity(){ return m_ProsperityStatus; }
	//设置繁荣度状态
	inline void SetProsperity(UInt8 properity){ m_ProsperityStatus = properity; }
	//获取本周增加的资金数额
	inline UInt32 GetWeekAddedFund(){ return m_WeekAddedFund; }
	//设置本周增加的资金数额
	inline void SetWeekAddedFund(UInt32 fund){ m_WeekAddedFund = fund; }
	//从兼并自己的列表里移除参数公会
	void RemoveMergerRequestGuild(Guild * guild);
	//请求兼并参数公会
	UInt32 RequestMerger(Guild * guild);
	//取消自己的请求
	UInt32 CancelMerger(Guild * guild);
	//获取向自己请求兼并的列表
	std::set<ObjID_t>& GetMergerRequestedGuildSet();
	//接受参数公会的兼并请求
	UInt32 AcceptMergerRequest(Guild * guild);
	//拒绝参数公会的兼并请求
	UInt32 RefuseMergerRequest(Guild * guild);
	//取消已接受的参数公会的兼并请求
	UInt32 CancelMergerRequest(Guild * guild);
	//清除所有兼并自己的请求，除了已经接受的
	UInt32 CleanAllMergerRequest(bool isSendMail = true);
	//获取兼并目标公会id
	inline UInt64 GetMergerGoalGuildID(){ return m_MergerGoalGuildID; }
	//获取自己的请求状态
	inline UInt8 GetMergerRequestStatus(){ return m_MergerRequsetStatus; }
	//判断自己是否可以兼并
	bool CanMerger();
	//判断自己有没有已接受的兼并请求
	bool ByAcceptedMerger();
	//兼并
	void OnMerger(Guild *byGuild);
	//清除自己发出兼并请求
	void CleanSelfMergerRequest();
	//把参数公会添加到请求兼并自己的列表里
	void AddMergerRequestGuild(Guild * guild);
	//获取上周增加的繁荣度
	inline UInt32 GetLastWeekAddFund(){ return m_LastWeekAddedFund; }
	
	//公会荣誉计数
	void AddGuildHonorCnt(GuildHonorChallenge type,std::vector<UInt32>& conds){m_GuildHonorMgr.AddGuildHonorCnt(type,conds);}
	void GetGuildHonor(CLProtocol::WorldGuildHonorInfoRes& res, std::vector<UInt32>& groupIds){m_GuildHonorMgr.GetGuildHonor(res,groupIds); }
};


#endif //__GUILD_MEMBER_H__