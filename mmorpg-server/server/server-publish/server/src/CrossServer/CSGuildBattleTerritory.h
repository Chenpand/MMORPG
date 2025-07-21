#ifndef _CS_GUILD_BATTLE_TERRITORY_H_
#define _CS_GUILD_BATTLE_TERRITORY_H_

#include <CLGuildBattleDataEntry.h>
#include <CLGameSessionProtocol.h>

#include "GameZoneMgr.h"

class CSTerritorySortListMgr
{
public:
	typedef std::list<TerritorySortListEntry> TerritorySortEntryList;

	CSTerritorySortListMgr(){}
	~CSTerritorySortListMgr(){}

	//更新排行榜
	void UpdateSortList(TerritorySortListEntry& entry);

	//下发排行榜
	void ListSortEntries(GameZone* zone, ObjID_t playerId, ObjID_t guildId, SortListType sortType, UInt16 start, UInt16 count);

	//获取排名
	UInt32 GetRanking(SortListType sortType, ObjID_t playerId, ObjID_t guildId);

	//获取第一工会
	UInt32 GetSortFirstGuild(GuildBattleEndInfo& info);

public:
	void AddToList(TerritorySortListEntry& entry);
	TerritorySortEntryList& GetSortEntryList(SortListType sortType, UInt64 guildId);
	void ClearSortList();

private:

	TerritorySortEntryList m_SortGuildMemberList;
	TerritorySortEntryList m_SortGuildList;
	std::map<UInt64, TerritorySortEntryList> m_SortGuildMemberMap;
};

class CSGuildBattleTerritory
{
public:
	CSGuildBattleTerritory() :m_DataEntry(NULL){}
	~CSGuildBattleTerritory(){}

	bool Init(GuildTerritoryDataEntry* dataEntry);

public:
	//获取领地ID
	inline UInt8 GetTerritoryId() { return m_DataEntry->id; }

	//获取领地名
	inline std::string GetTerritoryName() { return m_DataEntry->name; }

	//获取领地类型
	inline GuildTerritoryType GetTerritoryType() { return m_DataEntry->type; }

	//是否跨服工会战领地
	inline bool IsCrossTerritory() { return m_DataEntry->type == GuildTerritoryType::GTT_CROSS; }

	//获取设置跨服工会ID
	inline void SetCrossGuildId(ObjID_t id) { m_CrossGuildId = id; }
	inline ObjID_t GetCrossGuildId() const { return m_CrossGuildId; }

	//获取设置跨服工会名称
	inline void SetCrossGuildName(std::string name) { m_CrossGuildName = name; }
	inline std::string GetCrossGuildName() { return m_CrossGuildName; }

	//获取设置跨服工会服务器名
	inline void SetCrossOccupyServerName(std::string name) { m_CrossOccupyServerName = name; }
	inline std::string GetCrossOccupyServerName() { return m_CrossOccupyServerName; }

public:

	//添加公会战记录
	UInt32 AddGuildCrossBattleRecord(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd);
	void AddGuildBattleRecord(GuildBattleRecord& record);

	//清空公会战记录
	void ClearGuildBattleRecord();

	//获取记录
	void GetRecords(std::vector<GuildBattleRecord>& record, Int32 startIndex, UInt32 count);

	//获取本身记录
	void GetSelfRecords(std::vector<GuildBattleRecord>& record, UInt64 playerId);

	//更新排行榜信息
	void UpdateSortList(TerritorySortListEntry& entry);

	//下发排行信息
	void ListSortEntries(GameZone* zone, ObjID_t playerId, ObjID_t guildId, SortListType sortType, UInt16 start, UInt16 count);

	//获取排第一的工会
	UInt32 GetSortFirstGuild(GuildBattleEndInfo& info);

	//获取排名
	UInt32 GetRanking(SortListType sortType, ObjID_t playerId, ObjID_t guildId);

private:

	// 公会战斗记录
	std::vector<GuildBattleRecord>	m_Records;

	//占领的工会ID
	ObjID_t							m_CrossGuildId;

	//占领的工会名称
	std::string						m_CrossGuildName;

	//占领的服务器名
	std::string						m_CrossOccupyServerName;

	//已经记录单局ID
	std::vector<ObjID_t>			m_AlreadyRecordRaceIds;

	//排行记录
	CSTerritorySortListMgr			m_SortListMgr;

	// 领地数据项
	GuildTerritoryDataEntry*		m_DataEntry;

};

#endif