#ifndef __CL_SEASON_DATA_ENTRY_H__
#define __CL_SEASON_DATA_ENTRY_H__

#include "CLDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLSeasonDefine.h>

/**
*@brief ������
*/
struct SeasonDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;

	SeasonEventType eventType;

	//ѭ������
	SeasonCyclesType cyclesType;

	//��ʼ��
	UInt32 day;

	//��ʼʱ��
	std::string startTime;
};

/**
*@brief ���������
*/
class SeasonDataEntryMgr : public Avalon::DataEntryMgrBase<SeasonDataEntry>, public Avalon::Singleton<SeasonDataEntryMgr>
{
public:
	SeasonDataEntryMgr();
	~SeasonDataEntryMgr();

public:
	bool AddEntry(SeasonDataEntry* entry);

public:
	SeasonDataEntry* GetSeason(UInt32 now);

	UInt64 GetNextSeasonTime(SeasonEventType eventType, Avalon::Date nowDate);

	UInt64 GetNowSeasonTime(SeasonEventType eventType, Avalon::Date nowDate);

private:
	SeasonCyclesType m_SeasonCyclesType;

	std::map<UInt32, SeasonDataEntry*> m_SeasonDayMap;
};

/**
*@brief ������λ��
*/
struct SeasonLevelDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	bool IsUplevelBattle();

public:
	//��λid
	UInt32 id;

	//�ϼ���λ
	UInt32 preId;

	//�¼���λ
	UInt32 afterId;

	//���λ
	UInt32 mainLevel;

	//С��λ
	UInt32 smallLevel;

	//�Ǽ�
	UInt32 star;

	//�����
	UInt32 maxExp;

	//���ط���
	UInt32 hideScore;

	//�Ƿ�ƥ�������
	bool isRobot;

	//�Ƿ����
	bool isDownLevel;

	//�Ƿ����ģʽ
	bool isPromotion;

	//������ʤ������
	UInt32 uplevelBattleWinCount;

	//������ս������
	UInt32 uplevelBattleCount;

	//������ʤ����λ
	UInt32 uplevelBattleWinExp;

	//������ʧ�ܶ�λ
	UInt32 uplevelBattleLoseExp;

	//��������
	UInt32 battleDayNumber;
	//���侭��
	UInt32 battleCount;

	//�嵵�ع��λ�ȼ�
	UInt32 downLevel;

	//ÿ�ս���
	std::vector<ItemReward>	dailyRewards;

	//��������
	std::vector<ItemReward> seasonRewards;

	UInt32 attrId;
};

/**
*@brief ������λ�����
*/
class SeasonLevelDataEntryMgr : public Avalon::DataEntryMgrBase<SeasonLevelDataEntry>, public Avalon::Singleton<SeasonLevelDataEntryMgr>
{

public:
	SeasonLevelDataEntryMgr();
	~SeasonLevelDataEntryMgr();

public:
	bool AddEntry(SeasonLevelDataEntry* entry);

	SeasonLevelDataEntry* GetSeasonLevel(UInt32 score);

	SeasonLevelDataEntry* GetMinSeasonLevel();

	SeasonLevelDataEntry* GetMaxSeasonLevel();

	UInt32 GetMinSeasonLevelValue();

	UInt32 GetMaxSeasonLevelValue();

	//SeasonLevelDataEntry* GetPreSeason(UInt32 id);

	//SeasonLevelDataEntry* GetAfterSeason(UInt32 id);

	ItemRewardVec GetDailyItemRewardVec(UInt32 id);

	UInt32 GetSeasonExp(UInt32 winMainLevel, UInt32 loseMainLevel);

	bool GetSeasonLevelByExp(UInt32& seasonLevel, UInt32& seasonStar, UInt32& seasonExp, Int32 incExp);

	UInt32 GetSeasonLevelSumExp(UInt32 seasonLevel);

	SeasonMainLevel GetSeasonMainLevelBySumExp(UInt32 exp);

	UInt32 GetSeasonLevelBySumExp(UInt32 exp);

private:
	SeasonLevelDataEntry* m_MinSeasonLevel;

	SeasonLevelDataEntry* m_MaxSeasonLevel;

	//std::map<UInt32, SeasonLevelDataEntry*> m_PreSeasonMap;

	//std::map<UInt32, SeasonLevelDataEntry*> m_AfterSeasonMap;

	std::vector<SeasonLevelDataEntry*> m_MatchScoreVec;

	std::vector<SeasonExpEntry> m_SeasonExpVec;

	UInt32 m_SeasonExpVecSize;

	std::map<UInt32, UInt32> m_SeasonLevelExpMap;

	std::map<UInt32, UInt32> m_SeasonExpLevelMap;

};

/*
struct SeasonAttrDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32 id;

	//buff
	std::vector<UInt32> buffIds;

};

class SeasonAttrDataEntryMgr : public Avalon::DataEntryMgrBase<SeasonAttrDataEntry>, public Avalon::Singleton<SeasonAttrDataEntryMgr>
{
public:
	SeasonAttrDataEntryMgr(){}
	~SeasonAttrDataEntryMgr(){}
public:
	bool AddEntry(SeasonAttrDataEntry* entry);

	SeasonAttrDataEntry* GetSeasonSortListAttr();

};
*/

#endif  //__CL_SEASON_DATA_ENTRY_H__