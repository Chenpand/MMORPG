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
	
	//�������а�
	void UpdateSortList(TerritorySortListEntry& entry);

	//�·����а�
	void ListSortEntries(WSPlayer* player, SortListType sortType, UInt16 start, UInt16 count);

	//��ȡ����
	UInt32 GetRanking(SortListType sortType, WSPlayer* player);

	//��ȡǰn���ܷ�
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
	*@brief һ��ս������
	*/
	void OnBattleRaceEnd(GuildBattleMember& winnerMember, GuildBattleMember& loserMember);

	/**
	*@brief ������ʼ
	*/
	void OnEnrollBegin();

	/**
	*@brief �Զ�����
	*/
	void AutoEnroll();

public:

	//��ȡ���ID
	inline UInt8 GetTerritoryId() { return m_DataEntry->id; }

	//��ȡ�����
	inline std::string GetTerritoryName() { return m_DataEntry->name; }

	//��ȡ�������
	inline GuildTerritoryType GetTerritoryType() { return m_DataEntry->type; }

	//�Ƿ�������ս���
	inline bool IsCrossTerritory() { return m_DataEntry->type == GuildTerritoryType::GTT_CROSS; }

	//��ȡ�᳤�ƺ�
	inline UInt32 GetLeaderTitleId() { return m_DataEntry->leaderRewards.empty() ? 0 : m_DataEntry->leaderRewards[0].id; }

	//��ȡ�����������
	inline UInt32 GetMaxEnrollSize() { return m_DataEntry->enroll; }

	//��ȡ������Ҫռ������
	inline UInt8 GetNeedTerritoryId() { return m_DataEntry->needTerritoryId; }

	//��ȡ������Ҫ�Ĺ���ȼ�
	inline UInt8 GetNeedGuildLevel() { return m_DataEntry->needGuildLevel; }

	//��ȡ��صȼ�
	inline UInt8 GetLevel() { return m_DataEntry->level; }

	//��ȡ�ϼ����ID
	inline UInt8 GetPreTerritoryId() { return m_DataEntry->preTerritoryId; }

	//��ȡ�������ȼ�
	inline UInt32 GetInspireMax() { return 50; }

	//��ȡ��������
	inline std::vector<ConsumeItem> GetConsumeItem() { return m_DataEntry->consumeItem; }

	//��ȡ�᳤����
	inline std::vector<ItemReward> GetLearderReward() { return m_DataEntry->leaderRewards; }

	//��ȡ��Ա����
	inline std::vector<ItemReward> GetMemberReward() { return m_DataEntry->memberRewards; }

	//��ȡ�᳤ÿ�ս���
	inline std::vector<ItemReward> GetLearderItemReward() { return m_DataEntry->leaderDayRewards; }

	//��ȡ��Աÿ�ս���(�������ս�᳤��Ա��ʱһ��)
	inline std::vector<ItemReward> GetMemberItemReward() { return m_DataEntry->memberDayRewards; }

	//��ȡʧ�ܽ���
	inline std::vector<ItemReward> GetLoseItemReward() { return m_DataEntry->loseRewards; }

	//��ȡռ��Ĺ���
	inline Guild* GetGuild() { return m_Guild; };

	//����ռ��Ĺ���
	void SetGuild(Guild* guild);

	//��ȡ��¼����
	inline UInt32 GetRecordSize() { return m_Records.size(); }

	//��ñ�������
	inline UInt32 GetCrossEnrollSize() { return m_CrossEnrollSize; }
	inline void SetCrossEnrollSize(UInt32 size) { m_CrossEnrollSize = size; }

	//���ÿ����������
	inline std::string GetCrossGuildName() { return m_CrossGuildName; }
	inline void SetCrossGuildName(std::string name) { m_CrossGuildName = name; }

	//����ռ�칤��ķ���������
	inline std::string GetCrossOccupyServerName() { return m_CrossOccupyServerName; }
	inline void SetCrossOccupyServerName(std::string name) { m_CrossOccupyServerName = name; }

	//����ռ������صĹ���ID
	inline ObjID_t GetCrossGuildId() { return m_CrossGuildId; }
	inline void SetCrossGuildId(ObjID_t id) { m_CrossGuildId = id; }

	//����ռ�칤��Ļ᳤����
	inline std::string GetCrossGuildLeaderName() { return m_CrossGuildLeaderName; }
	inline void SetCrossGuildLeaderName(std::string name) { m_CrossGuildLeaderName = name; }

public:

	//��ӹ���ս��¼
	void AddGuildBattleRecord(GuildBattleRecord& record);
	void AddGuildBattleRecord(GuildBattleMember& winnerMember, Guild* winGuild, GuildBattleMember& loserMember, Guild* loseGuild);

	//��չ���ս��¼
	void ClearGuildBattleRecord();

	//��ȡ��¼
	void GetRecords(std::vector<GuildBattleRecord>& record, Int32 startIndex, UInt32 count);

	//��ȡ�����¼
	void GetSelfRecords(std::vector<GuildBattleRecord>& record, UInt64 playerId);

	//��������
	std::vector<ConsumeItem> GetMatchConsumeItem(UInt32 index) { return m_DataEntry->GetMatchConsume(index); }

	//����ս����
	void ListSortEntries(WSPlayer* player, SortListType sortType, UInt16 start, UInt16 count);

	//��ȡ��������
	UInt32 GetRanking(SortListType sortType, WSPlayer* player);

	//�Ƿ����ڴ�������
	bool IsEnrollTransaction() { return m_EnrollTransaction != NULL; }

	//���ñ�������
	void SetEnrollTransaction(GuildBattleEnrollTransaction* tran) { m_EnrollTransaction = tran; }

	//��ù�������
	std::string GetGuildName();

	//��ʼ���������ս����
	void InitCrossSortList(Guild* guild);

private:
	void _UpdateSortList(SortListType sortType, GuildBattleMember& winnerMember, Guild* winGuild);

private:
	// ����ս����¼
	std::vector<GuildBattleRecord>	m_Records;

	//ռ��Ĺ���
	Guild* m_Guild;

	//ռ��Ĺ���ID
	ObjID_t							m_CrossGuildId;

	//ռ�칤��ķ���������
	std::string						m_CrossOccupyServerName;

	//ռ��Ĺ�������
	std::string						m_CrossGuildName;

	//ռ�칤��᳤����
	std::string						m_CrossGuildLeaderName;

	//��������
	UInt32							m_CrossEnrollSize;

	// ����������
	GuildBattleEnrollTransaction*	m_EnrollTransaction;

	//���м�¼
	TerritorySortListMgr			m_SortListMgr;

	// ���������
	GuildTerritoryDataEntry*		m_DataEntry;
};

#endif