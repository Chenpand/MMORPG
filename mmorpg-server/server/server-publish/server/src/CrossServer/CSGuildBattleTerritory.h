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

	//�������а�
	void UpdateSortList(TerritorySortListEntry& entry);

	//�·����а�
	void ListSortEntries(GameZone* zone, ObjID_t playerId, ObjID_t guildId, SortListType sortType, UInt16 start, UInt16 count);

	//��ȡ����
	UInt32 GetRanking(SortListType sortType, ObjID_t playerId, ObjID_t guildId);

	//��ȡ��һ����
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
	//��ȡ���ID
	inline UInt8 GetTerritoryId() { return m_DataEntry->id; }

	//��ȡ�����
	inline std::string GetTerritoryName() { return m_DataEntry->name; }

	//��ȡ�������
	inline GuildTerritoryType GetTerritoryType() { return m_DataEntry->type; }

	//�Ƿ�������ս���
	inline bool IsCrossTerritory() { return m_DataEntry->type == GuildTerritoryType::GTT_CROSS; }

	//��ȡ���ÿ������ID
	inline void SetCrossGuildId(ObjID_t id) { m_CrossGuildId = id; }
	inline ObjID_t GetCrossGuildId() const { return m_CrossGuildId; }

	//��ȡ���ÿ����������
	inline void SetCrossGuildName(std::string name) { m_CrossGuildName = name; }
	inline std::string GetCrossGuildName() { return m_CrossGuildName; }

	//��ȡ���ÿ�������������
	inline void SetCrossOccupyServerName(std::string name) { m_CrossOccupyServerName = name; }
	inline std::string GetCrossOccupyServerName() { return m_CrossOccupyServerName; }

public:

	//��ӹ���ս��¼
	UInt32 AddGuildCrossBattleRecord(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd);
	void AddGuildBattleRecord(GuildBattleRecord& record);

	//��չ���ս��¼
	void ClearGuildBattleRecord();

	//��ȡ��¼
	void GetRecords(std::vector<GuildBattleRecord>& record, Int32 startIndex, UInt32 count);

	//��ȡ�����¼
	void GetSelfRecords(std::vector<GuildBattleRecord>& record, UInt64 playerId);

	//�������а���Ϣ
	void UpdateSortList(TerritorySortListEntry& entry);

	//�·�������Ϣ
	void ListSortEntries(GameZone* zone, ObjID_t playerId, ObjID_t guildId, SortListType sortType, UInt16 start, UInt16 count);

	//��ȡ�ŵ�һ�Ĺ���
	UInt32 GetSortFirstGuild(GuildBattleEndInfo& info);

	//��ȡ����
	UInt32 GetRanking(SortListType sortType, ObjID_t playerId, ObjID_t guildId);

private:

	// ����ս����¼
	std::vector<GuildBattleRecord>	m_Records;

	//ռ��Ĺ���ID
	ObjID_t							m_CrossGuildId;

	//ռ��Ĺ�������
	std::string						m_CrossGuildName;

	//ռ��ķ�������
	std::string						m_CrossOccupyServerName;

	//�Ѿ���¼����ID
	std::vector<ObjID_t>			m_AlreadyRecordRaceIds;

	//���м�¼
	CSTerritorySortListMgr			m_SortListMgr;

	// ���������
	GuildTerritoryDataEntry*		m_DataEntry;

};

#endif