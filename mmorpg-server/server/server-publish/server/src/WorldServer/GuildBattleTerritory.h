#ifndef __GUILD_BATTLE_TERRITORY_H__
#define __GUILD_BATTLE_TERRITORY_H__

#include <CLDefine.h>
#include <CLGuildDefine.h>
#include <CLGuildBattleDataEntry.h>
#include <CLSortListDefine.h>

class Guild;
class WSPlayer;
class GuildBattleEnrollTransaction;

class TerritorySortListMgr
{
public:
	typedef std::list<TerritorySortListEntry> TerritorySortEntryList;

	TerritorySortListMgr(){}
	~TerritorySortListMgr(){}
	
	//更新排行榜
	void UpdateSortList(TerritorySortListEntry& entry);

	//下发排行榜
	void ListSortEntries(WSPlayer* player, SortListType sortType, UInt16 start, UInt16 count);

	//获取排名
	UInt32 GetRanking(SortListType sortType, WSPlayer* player);

	//获取前n名总分
	UInt32 GetNumRankScoreSum(SortListType sortType, UInt64 guildId, UInt32 num);

public:
	void AddToList(TerritorySortListEntry& entry);
	TerritorySortEntryList& GetSortEntryList(SortListType sortType, UInt64 guildId);
	void ClearSortList();

private:

	TerritorySortEntryList m_SortGuildMemberList;
	TerritorySortEntryList m_SortGuildList;
	std::map<UInt64, TerritorySortEntryList> m_SortGuildMemberMap;
};

class GuildBattleTerritory
{

public:
	GuildBattleTerritory() :m_Guild(NULL), m_CrossGuildId(0), m_CrossGuildName(""), m_CrossEnrollSize(0), m_EnrollTransaction(NULL) {};
	~GuildBattleTerritory(){};

	bool Init(GuildTerritoryDataEntry* dataEntry);

public:
	/**
	*@brief 一场战斗结束
	*/
	void OnBattleRaceEnd(GuildBattleMember& winnerMember, GuildBattleMember& loserMember);

	/**
	*@brief 报名开始
	*/
	void OnEnrollBegin();

	/**
	*@brief 自动报名
	*/
	void AutoEnroll();

public:

	//获取领地ID
	inline UInt8 GetTerritoryId() { return m_DataEntry->id; }

	//获取领地名
	inline std::string GetTerritoryName() { return m_DataEntry->name; }

	//获取领地类型
	inline GuildTerritoryType GetTerritoryType() { return m_DataEntry->type; }

	//是否跨服工会战领地
	inline bool IsCrossTerritory() { return m_DataEntry->type == GuildTerritoryType::GTT_CROSS; }

	//获取会长称号
	inline UInt32 GetLeaderTitleId() { return m_DataEntry->leaderRewards.empty() ? 0 : m_DataEntry->leaderRewards[0].id; }

	//获取报名最大数量
	inline UInt32 GetMaxEnrollSize() { return m_DataEntry->enroll; }

	//获取报名需要占领的领地
	inline UInt8 GetNeedTerritoryId() { return m_DataEntry->needTerritoryId; }

	//获取报名需要的公会等级
	inline UInt8 GetNeedGuildLevel() { return m_DataEntry->needGuildLevel; }

	//获取领地等级
	inline UInt8 GetLevel() { return m_DataEntry->level; }

	//获取上级领地ID
	inline UInt8 GetPreTerritoryId() { return m_DataEntry->preTerritoryId; }

	//获取鼓舞最大等级
	inline UInt32 GetInspireMax() { return 50; }

	//获取报名消耗
	inline std::vector<ConsumeItem> GetConsumeItem() { return m_DataEntry->consumeItem; }

	//获取会长奖励
	inline std::vector<ItemReward> GetLearderReward() { return m_DataEntry->leaderRewards; }

	//获取会员奖励
	inline std::vector<ItemReward> GetMemberReward() { return m_DataEntry->memberRewards; }

	//获取会长每日奖励
	inline std::vector<ItemReward> GetLearderItemReward() { return m_DataEntry->leaderDayRewards; }

	//获取会员每日奖励(跨服公会战会长会员暂时一样)
	inline std::vector<ItemReward> GetMemberItemReward() { return m_DataEntry->memberDayRewards; }

	//获取失败奖励
	inline std::vector<ItemReward> GetLoseItemReward() { return m_DataEntry->loseRewards; }

	//获取占领的公会
	inline Guild* GetGuild() { return m_Guild; };

	//设置占领的公会
	void SetGuild(Guild* guild);

	//获取记录条数
	inline UInt32 GetRecordSize() { return m_Records.size(); }

	//获得报名人数
	inline UInt32 GetCrossEnrollSize() { return m_CrossEnrollSize; }
	inline void SetCrossEnrollSize(UInt32 size) { m_CrossEnrollSize = size; }

	//设置跨服工会名称
	inline std::string GetCrossGuildName() { return m_CrossGuildName; }
	inline void SetCrossGuildName(std::string name) { m_CrossGuildName = name; }

	//设置占领工会的服务器名字
	inline std::string GetCrossOccupyServerName() { return m_CrossOccupyServerName; }
	inline void SetCrossOccupyServerName(std::string name) { m_CrossOccupyServerName = name; }

	//设置占领跨服领地的工会ID
	inline ObjID_t GetCrossGuildId() { return m_CrossGuildId; }
	inline void SetCrossGuildId(ObjID_t id) { m_CrossGuildId = id; }

	//设置占领工会的会长名字
	inline std::string GetCrossGuildLeaderName() { return m_CrossGuildLeaderName; }
	inline void SetCrossGuildLeaderName(std::string name) { m_CrossGuildLeaderName = name; }

public:

	//添加公会战记录
	void AddGuildBattleRecord(GuildBattleRecord& record);
	void AddGuildBattleRecord(GuildBattleMember& winnerMember, Guild* winGuild, GuildBattleMember& loserMember, Guild* loseGuild);

	//清空公会战记录
	void ClearGuildBattleRecord();

	//获取记录
	void GetRecords(std::vector<GuildBattleRecord>& record, Int32 startIndex, UInt32 count);

	//获取本身记录
	void GetSelfRecords(std::vector<GuildBattleRecord>& record, UInt64 playerId);

	//报名消耗
	std::vector<ConsumeItem> GetMatchConsumeItem(UInt32 index) { return m_DataEntry->GetMatchConsume(index); }

	//公会战排行
	void ListSortEntries(WSPlayer* player, SortListType sortType, UInt16 start, UInt16 count);

	//获取自身排行
	UInt32 GetRanking(SortListType sortType, WSPlayer* player);

	//是否正在处理事务
	bool IsEnrollTransaction() { return m_EnrollTransaction != NULL; }

	//设置报名事务
	void SetEnrollTransaction(GuildBattleEnrollTransaction* tran) { m_EnrollTransaction = tran; }

	//获得工会名称
	std::string GetGuildName();

	//初始化跨服工会战排行
	void InitCrossSortList(Guild* guild);

private:
	void _UpdateSortList(SortListType sortType, GuildBattleMember& winnerMember, Guild* winGuild);

private:
	// 公会战斗记录
	std::vector<GuildBattleRecord>	m_Records;

	//占领的公会
	Guild* m_Guild;

	//占领的工会ID
	ObjID_t							m_CrossGuildId;

	//占领工会的服务器名字
	std::string						m_CrossOccupyServerName;

	//占领的工会名称
	std::string						m_CrossGuildName;

	//占领工会会长名称
	std::string						m_CrossGuildLeaderName;

	//报名数量
	UInt32							m_CrossEnrollSize;

	// 报名的事务
	GuildBattleEnrollTransaction*	m_EnrollTransaction;

	//排行记录
	TerritorySortListMgr			m_SortListMgr;

	// 领地数据项
	GuildTerritoryDataEntry*		m_DataEntry;
};

#endif