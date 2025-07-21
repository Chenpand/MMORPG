#ifndef __CL_LEVEL_DATA_ENTRY_H__
#define __CL_LEVEL_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

struct AreaGroup
{
	bool HasArea(UInt32 areaId) const 
	{
		return std::find(areaIds.begin(), areaIds.end(), areaId) != areaIds.end();
	}

	UInt32 randArea(std::vector<UInt32>& exceptIds) const
	{
		std::vector<UInt32> ids;

		for (auto id : areaIds)
		{
			if (std::find(exceptIds.begin(), exceptIds.end(), id) == exceptIds.end())
			{
				ids.push_back(id);
			}
		}

		if (ids.empty())
		{
			return 0;
		}

		UInt32 index = Avalon::Random::RandBetween(0, ids.size() - 1);
		if (index >= ids.size())
		{
			return 0;
		}

		return ids[index];
	}

	std::vector<UInt32> areaIds;
};

// ���³�����
enum DungeonType
{
    LT_NORMAL,      // ��ͨ
    LT_STORY,       // ����
    LT_ACTIVITY,    // �
    LT_TOWER,       // ��
};

// ���³�������
enum DungeonSubType
{
	// ��ͨ�ؿ�
	DST_NORMAL,
	// Զ�Źؿ�
	DST_YUANGU,
	// ţͷ����԰
	DST_NIUTOUGUAI,
	// �ϲ�Ϫ��
	DST_NANBUXIGU,
	// ����֮��
	DST_SIWANGZHITA,
	// ����
	DST_GUILD,
	// PKս��
	DST_PK,
	// ���
	DST_GOLD,
	// ��Ԩ
	DST_HELL,
	// ����PK
	DST_GUILD_PK,
	// ��Ԩ���
	DST_HELL_ENTRY,
	// ���BOSS
	DST_TEAM_BOSS,
	// �����ؿ�
	DST_WUDAOHUI = 13,
	// ����֮��
	DST_JUEWANG_TOWER,
	// ���н�ѧ
	DST_COMBO,
	// ���﹥��
	DST_CITY_MONSTER,
	// ���
	DST_DEVILDDOM,
	// ������³�
	DST_GUILD_DUNGEON,
	// ��ʱ��Ԩ
	DST_LIMIT_TIME_HELL,
	// �ܳ���Ԩ
	DST_WEEK_HELL, 
	// PVEѵ����
	DST_TRAINING_PVE,
	// �ܳ���Ԩ���
	DST_WEEK_HELL_ENTRY,
	// �ܳ���Ԩǰ�ùؿ�
	DST_WEEK_HELL_PER,
	// �ռ���������
	DST_ZJSL_TOWER,
	// �ŶӸ���
	DST_RAID_DUNGEON,
	// ������ͨ
	DST_ANNIVERSARY_NORMAL,
	// ��������
	DST_ANNIVERSARY_HARD,
	// �����������
	DST_TREASUREMAP = 28,
	// �����ʱ��Ԩ
	DST_FREE_LIMIT_TIME_HELL = 31,
	// ����С��Ϸ�ؿ�
	DST_PLAYGAME = 32,

	// ����
	DST_NUM,
};

// ���³ǵ�������
enum DungeonThreeType
{
	DTT_NORMAL,			//  ��ͨ���³�
	DTT_TEAM_ELITE,		//  ��Ӿ�Ӣ���³�
	DTT_CHIJI_PK,		//  �Լ�PK����
	DTT_LOST_DUNGEON_MONSTER,		//  ����С�ֵ��³�
};

// ���³��Ѷ�
enum DungeonHardType
{
    LHT_NORMAL,     // ��ͨ
    LHT_RISK,       // ð��
    LHT_WARRIOR,    // ��ʿ
    LHT_KING,       // ����

    LHT_NUM,
};

// ��Ԩģʽ
enum DungeonHellMode
{
	// ��
	HELL_MODE_NULL,
	// ��ͨ
	HELL_MODE_NORMAL,
	// ����
	HELL_MODE_HARD,
	// �ռ�����
	HELL_MODE_HARD_ULTIMATE,

	HELL_MODE_MAX,
};

// ���³���Ʊ����
struct DungeonTicket
{
    DungeonTicket()
    {
        itemId = 0;
        num = 0;
    }

    UInt32      itemId;
    UInt32       num;
};

// �����������
struct RaceEndDropMultiCost
{
	RaceEndDropMultiCost() : multi(0), cost(0) {}
	UInt8	multi;
	UInt32	cost;
};
typedef std::vector<RaceEndDropMultiCost> VRaceEndDropMultiCost;

// �ؿ�����ȼ���������
enum DungeonMonsterLevelAdaptType
{
	// ���ﱾ��ȼ�
	DMLAT_MONSTER,
	// �ؿ��ȼ�
	DMLAT_DUNGEON,
	// ����ƽ���ȼ�
	DMLAT_TEAM,
	// ���﹥�Ǽ�Ȩƽ���ȼ�
	DMLAT_CITY_MONSTER
};

struct DungeonChapterKey
{
	DungeonChapterKey(UInt32 dungeonId_, UInt8 hard_)
		: dungeonId(dungeonId_), hard(hard_)
	{}
	bool operator<(const DungeonChapterKey& other) const
	{
		if (other.dungeonId != dungeonId)
		{
			return dungeonId < other.dungeonId;
		}

		return hard < other.hard;
	}

	UInt32	dungeonId;
	UInt8   hard;

};

/**
*@brief ���³�������
*/
class DungeonDataEntry : public Avalon::DataEntry
{
public:
    DungeonDataEntry();
    ~DungeonDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

	// �ǲ�����Ԩ���³����
	bool IsHellEntry() const;

	// �ǲ�����Ԩ���³�
	bool IsHellDungeon() const;

	int GetAreaIndex(UInt32 areaId) const;

	// �ǲ����ܳ���Ԩ���
	bool IsWeekHellEntry() const;
private:
    bool ReadDropIDs(std::string dropIDStrs, UInt32* dropIDs);

public:
    // id
    UInt32	                id;
	// ����
	std::string				name;
    // �ؿ��ȼ�
    UInt16                  level;
    // ����
    DungeonType             type;
	// ������
	DungeonSubType			subType;
	// ��������
	DungeonThreeType		threeType;
    // �Ѷ�
    DungeonHardType         hardType;
	// ��ǩ
	UInt32					tag;
	// �Ƿ��Ѷȸ��ݵȼ�
	DungeonMonsterLevelAdaptType monsterLevelAdaptType;
	// ����Ѷ�����ȼ�
	UInt32					maxHardAdaptLevel;
    // ��͵ȼ�
    UInt32                  minLevel;
    // ��������
    UInt32                  storyTaskId;
    // ǰ������
    UInt32                  preTaskId;
    // ǰ�õ��³�
    UInt32                  prevDungeonId;
    // ������³�
    std::vector<UInt32>     storyDungeonIds;
	// �����Ƿ�ȼ�����
	bool					isExpLevelAdapt;
    // ���齱��
    UInt32                  expReward;
    // ����ID
	std::vector<AreaGroup>     areaList;
    // ��ʼ����ID
    UInt32                  startAreaId;
	// BOSS����
	UInt32					bossAreaId;
    // ��Ԩ����ID
    std::vector<UInt32>     hellAreaList;
    // ÿ���������ĵ�ƣ��
    UInt16                  costFatiguePerArea;
    // С�ֵ���
    UInt32                  normalDropItem;
    // ��Ӣ����
    UInt32                  eliteDropItem;
    // BOSS����
    UInt32                  bossDropItem;
	// �ؿ�����
	UInt32                  dungeonDropItem;
	// �����(ֻ�ڽ���ʱ����)
	std::vector<UInt32>		activityDropItems;
	// �ؿ��״ε���
	UInt32					dungeonFirstDropItem;
	// ���ƻ������
	UInt32					destructDropItem;
	// �ʵ�����
	UInt32					easterEggDropItem;
    // �������
    std::vector<UInt32>     taskDropItems;
	// �������
	UInt32					rollDropId;
    // ��Ʊ
    DungeonTicket           ticket;
    // ��ԨƱ
    UInt32                  hellTicketNum;
    // ��Ԩ����
    UInt32                  hellTask;
    // ��Ԩ����
    std::vector<UInt32>     hellDrops[HELL_MODE_MAX];
	// ��Ԩ�����
	std::vector<UInt32>		hellActivityDropItems;
	// �����ɸ������
	Int32					maxReviveTimes;
	// �ű�����ɸ������
	UInt32					maxTeamCopyReviveTimes;
	// ��������������
	std::vector<UInt8>		raceEndDropBaseMulti;
	// ��ͬ���ʽ��������������
	VRaceEndDropMultiCost	raceEndDropMultiCost;
	// �ID
	UInt32					activityId;
	// ÿ�մ���
	UInt32					dailyMaxTime;
	// ͨ�����ٷ�����
	UInt32					clearMinAreaNum;
	// �ؿ��ڿ��õĵ���
	std::vector<UInt32>     quickUseItems;
	// ��Ԩ�ȼ���
	UInt32					hellSplitLevel;
	// ��Ԩ�������
	UInt32					hellProb;
	// �Ƿ�ֻ�н�������
	bool					onlyRaceEndProfit;
	// �Ƿ���ʦͽ����ӳ�
	bool					masterExpAddition;
	// ��������, keyΪ��������id, valueΪ��Ӧ�������³�id
	std::map<UInt32, UInt32> guideTasks;
	// �Ƿ��˸���
	bool					singleDungeon;
	// �Ƿ�ͨ�ص��³ǽ���
	bool					onlyRaceEndSettlement;
	// �������³����
	UInt32					ownerEntryId;
	// ���ѡ��Ȩ��
	UInt32					weightEntry;
	// ��Ӣ���³�ǰ���½�
	UInt32					eliteDungeonPrevChapter;
};

struct HellDungeonInfo
{
	UInt32 dungeonId;
	UInt32 prob;
};

class DungeonDataEntryMgr : public Avalon::DataEntryMgrBase<DungeonDataEntry>
	, public Avalon::Singleton<DungeonDataEntryMgr>
{
public:
	/**
	*@brief ������������б���
	*/
	virtual bool AddEntry(DungeonDataEntry* entry);

	/**
	*@brief ������Ԩ�ȼ������Ԩ���³�ID
	*/
	UInt32 GenHellDungeonByLevel(UInt32 level, DungeonHardType hard);

	/**
	*@brief ������Ԩ�ȼ������Ԩ���³�ID
	*/
	UInt32 GetHellEntry(UInt32 level, DungeonHardType hard);

	/**
	*@brief ���ݵ��³����ID������³�ID
	*/
	UInt32 GetDungeonIdByEntryId(UInt32 entryId);

	/**
	*@brief ���ݵ��³�id�ҵ��³����ID
	*/
	UInt32 GetDungeonEntryIdByDungeonId(UInt32 dungeonId);

	/**
	*@brief �����½�id���ѶȻ�ȡ���³�id
	*/
	bool  GetDungeonIdsByChapterHard(UInt32 chapter, UInt8 hard, std::vector<UInt32>& ids);

	std::map<DungeonChapterKey, std::vector<UInt32>>& GetAllChapterIds() {
		return  m_ChapterDungeonIds;
	}
private:
	// ��Ԩ���³ǣ����ݵȼ����Ѷȣ�
	std::map<UInt32, std::vector<HellDungeonInfo>>	m_HellDungeonMap;
	// ��Ԩ���³���ڣ����ݵȼ����Ѷȣ�
	std::map<UInt32, UInt32>						m_HellEntryMap;
	// ���³���ڼ���
	std::map<UInt32, std::vector<HellDungeonInfo>>  m_DungeonMap;
	// ���³�idӳ�����id
	std::map<UInt32, UInt32>						m_DungeonMapEntry;
	// �½ڵ��³�id
	std::map<DungeonChapterKey, std::vector<UInt32>>  m_ChapterDungeonIds;
};

#endif