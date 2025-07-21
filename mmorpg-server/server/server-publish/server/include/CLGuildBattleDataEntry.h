#ifndef _CL_GUILD_TERRITORY_DATAENTRY_H_
#define _CL_GUILD_TERRITORY_DATAENTRY_H_

#include "CLDefine.h"
#include "CLGameDefine.h"
#include "CLItemDefine.h"
#include "CLGuildDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

struct GuildBattleScore
{
public:
	UInt32 winScore;
	UInt32 endScore;
	UInt32 loserScore;
};

/**
*@brief ������ñ�
*/
struct GuildTerritoryDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	std::vector<ConsumeItem> GetMatchConsume(UInt32 index);

	bool IsChestDoubleDungeon(UInt32 dungeonId);

	bool IsDropAdditionDungeon(UInt32 dungeonId);

	void ParseReward(ItemRewardVec& rewardVec, std::string& strReward);

public:
	//���ID
	UInt8 id;

	//����
	std::string name;
	
	//�������
	GuildTerritoryType type;

	//��صȼ�
	UInt8 level;

	//��Ҫռ������
	UInt8 needTerritoryId;

	//��Ҫ�Ĺ���ȼ�
	UInt8 needGuildLevel;

	//�ϼ����
	UInt8 preTerritoryId;

	//��������
	UInt32 enroll;

	//���ĵ���
	std::vector<ConsumeItem> consumeItem;

	//���ּ���
	std::vector<GuildBattleScore> guildBattleScore;

	// ռ�����
	UInt32 occupyScore;

	// ս������ϵ��(�ٷֱ�)
	UInt32 battleScoreFactor;

	//�᳤����
	ItemRewardVec leaderRewards;

	//��Ա����
	ItemRewardVec memberRewards;

	//�᳤ÿ�ս���
	ItemRewardVec leaderDayRewards;

	//��Աÿ�ս���
	ItemRewardVec memberDayRewards;

	//ʧ�ܽ���
	ItemRewardVec loseRewards;

	//ƥ������
	std::vector<std::vector<ConsumeItem>> matchConsumeItems;

	//���³�ID
	std::vector<UInt32> chestDoubleDungeons;

	// ÿ�շ��Ʒ�������
	UInt32		dailyChestDoubleTimes;

	// ���Ƽӳ�����(��Ӧö��DungeonChestType��λ���)
	UInt8		chestDoubleFlag;

	//���³�ID
	std::vector<UInt32> dropAdditionDungeons;

	//���ʼӳ�
	UInt32		dropAddition;

	// �᳤PVE�ӳɼ���
	UInt32				leaderPveAddSkill;
	// �᳤PVP�ӳɼ���
	UInt32				leaderPvpAddSkill;
};

class GuildTerritoryDataEntryMgr :public  Avalon::DataEntryMgrBase<GuildTerritoryDataEntry>, public Avalon::Singleton<GuildTerritoryDataEntryMgr>
{
public:
	GuildTerritoryDataEntryMgr();
	~GuildTerritoryDataEntryMgr();

public:
	bool AddEntry(GuildTerritoryDataEntry* entry);
	
	const UInt32 GetMaxLevel() const { return m_MaxLevel; }

private:
	UInt32 m_MaxLevel;
};



/**
*@brief ����ս�����
*/
struct GuildInspireDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return level; }

	bool Read(Avalon::DataRow& row);

public:
	//�ȼ�
	UInt8 level;

	//��Ҫ�Ĺ���ȼ�
	UInt8 needGuildLevel;

	//���ĵ���
	std::vector<ConsumeItem> consumeItem;

	//������ĵ���
	std::vector<ConsumeItem> crossConsumeItem;
};

class GuildInspireDataEntryMgr :public  Avalon::DataEntryMgrBase<GuildInspireDataEntry>, public Avalon::Singleton<GuildInspireDataEntryMgr>
{
public:
	GuildInspireDataEntryMgr();
	~GuildInspireDataEntryMgr();

	UInt8 GetMaxLevel() { return m_MaxLevel; }

public:
	bool AddEntry(GuildInspireDataEntry* entry);

private:
	UInt8 m_MaxLevel;
};

struct GuildRewardDataEntry : public Avalon::DataEntry
{
public:
	GuildRewardDataEntry();
	~GuildRewardDataEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);
	
public:
	//ID
	UInt32 id;

	//���ַ�Χ
	UInt32 beginScore;

	//������Ʒ
	std::vector<ItemReward> itemReward;

};

typedef Avalon::DataEntryMgr<GuildRewardDataEntry> GuildRewardDataEntryMgr;

struct GuildBattleTimeDataEntry : public Avalon::DataEntry
{
public:
	GuildBattleTimeDataEntry() {};
	~GuildBattleTimeDataEntry() {};

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt32 group;
	GuildBattleType type;
	GuildBattleStatus status;
	UInt32 week;
	UInt32 hour;
	UInt32 min;
	UInt32 sec;
	bool isOpen;
};

class GuildBattleTimeDataEntryMgr : public  Avalon::DataEntryMgrBase<GuildBattleTimeDataEntry>, public Avalon::Singleton<GuildBattleTimeDataEntryMgr>
{
public:
	typedef std::map<GuildBattleStatus, GuildBattleTimeDataEntry*> GuildBattleStatusMap;
	typedef std::map<UInt32, GuildBattleStatusMap> GuildBattleTimeMap;
public:
	GuildBattleTimeDataEntryMgr(){}
	~GuildBattleTimeDataEntryMgr(){}

	//GuildBattleStatus GetGuildBattleStatus(Avalon::Time now);
	GuildBattleTimeInfo GetGuildBattleTimeInfo(Avalon::Time now, GuildBattleType type = GuildBattleType::GBT_INVALID);

	UInt32 GetGuildCrossBattleEndTime(Avalon::Time now);

	/**
	*@breif ��ȡ��һ�λʱ���
	*/
	UInt32 GetLastGuildBattleTim(Avalon::Time now, GuildBattleType type);

public:
	bool AddEntry(GuildBattleTimeDataEntry* entry);

private:
	GuildBattleTimeMap m_GuildBattleTimeMap;
};

struct GuildBattleLotteryNumDataEntry : public Avalon::DataEntry
{
public:
	GuildBattleLotteryNumDataEntry(){}
	~GuildBattleLotteryNumDataEntry(){}

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt16 num;

};

typedef Avalon::DataEntryMgr<GuildBattleLotteryNumDataEntry> GuildBattleLotteryNumDataEntryMgr;

#endif