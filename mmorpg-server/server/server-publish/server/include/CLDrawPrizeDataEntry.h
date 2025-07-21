#ifndef _CL_DRAW_PRIZE_DATA_ENTRY_H_
#define _CL_DRAW_PRIZE_DATA_ENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLItemDefine.h>
#include <CLActivityDefine.h>


/**
*@brief �齱��
*/
class DrawPrizeDataEntry : public Avalon::DataEntry
{
public:
	DrawPrizeDataEntry() {};
	~DrawPrizeDataEntry() {};

public:
	UInt32 GetKey() const { return dataId; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 19; };

public:
	// ������Ψһid
	UInt32		dataId;
	// ����
	std::string	name;
	// �id
	ActiveID    activityId;
	// ��Ӫ�����
	OpActivityType opActivityType;
	// ����
	UInt32		drawPrizeType;
	// ������
	std::string channelName;
	// �Ƿ���
	UInt8		isOpen;
	// �齱����
	UInt32		baseCount;
	// �����齱����
	UInt32		continuousDrawDays;
	// �����齱����
	UInt32		rewardCount;
	// ˢ�¼�����죩
	UInt32		refreshInterval;
	// ˢ��ʱ��
	UInt8		refreshTime;
	// �ѳ齱����key
	std::string drawPrizeCountKey;
	// �齱�ܴ���key
	std::string allCountKey;
	// �����齱����key
	std::string continuousDrawKey;
	// ʣ��齱����key
	std::string restDrawCountKey;
	// ��ƷID key
	std::string rewardIdKey;
	// �齱ȯ����
	ItemSubType raffleTicketType;
	// ��չ����
	std::vector<UInt32> parms;
};

/**
*@brief �齱�������
*/
class DrawPrizeDataEntryMgr : public Avalon::DataEntryMgrBase<DrawPrizeDataEntry>
	, public Avalon::Singleton<DrawPrizeDataEntryMgr>
{
public:
	DrawPrizeDataEntryMgr() {};
	~DrawPrizeDataEntryMgr() {};

public:
	bool AddEntry(DrawPrizeDataEntry* dataEntry);
	DrawPrizeDataEntry* GetDrawPrizeDataByType(UInt32 type);

private:
	std::vector<DrawPrizeDataEntry*> m_Datas;
};


/**
*@brief ����
*/
class RewardPoolDataEntry : public Avalon::DataEntry
{
public:
	RewardPoolDataEntry() {};
	~RewardPoolDataEntry() {};

public:
	UInt32 GetKey() const { return dataId; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 10; };

public:
	// Ψһid
	UInt32		dataId;
	// �齱��id
	UInt32		drawPrizeTableId;
	// ����id
	UInt32		itemId;
	// ����
	UInt32		itemNum;
	// ǿ���ȼ�
	UInt8		strengthenLevel;
	// Ȩ��
	UInt32		itemWeight;
	// �齱��������
	UInt32		drawPrizeCountLimit;
	// ��ֵ����
	UInt32		chargeCond;
	// ����ID
	UInt32		announceNum;
	// �������ܲ���
	UInt32		serverTotalNum;
};

class IRewardPoolRareItemControl
{
public:
	/**
	*@brief ϡ�п�����Ʒʣ������
	*/
	virtual UInt32 RareRewardRemainNum(UInt32 rewardId, UInt32 num) = 0;
};

class RewardPoolDataEntryMgr : public Avalon::DataEntryMgrBase<RewardPoolDataEntry>
	, public Avalon::Singleton<RewardPoolDataEntryMgr>
{
public:
	RewardPoolDataEntryMgr() {};
	~RewardPoolDataEntryMgr() {};

	bool Init(IRewardPoolRareItemControl* control = NULL);

	RewardPoolDataEntry* RandReward(UInt32 drawPrizeId, UInt32 usedDrawCnt = 0, UInt32 totalChargeOnRole = 0);

private:
	IRewardPoolRareItemControl*	m_rareItemControl;
};


/**
*@brief �ᱦ��Ʒ��
*/
class GambingItemDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return gambingItemId; }

	bool Read(Avalon::DataRow& row);
	bool ReadCustomer(UInt32 _actId, UInt32 _gambingItemId, UInt32 _sortId, const ItemRewardVec& soldItems, const std::vector<std::string>& strVec);

	static UInt32 GetColNum() { return 14; };

public:
	// �ᱦ��Ʒid
	UInt32 gambingItemId;
	// �id
	UInt32 actId;
	// �ᱦ��Ʒ����
	UInt32 gambingItemNum;
	// ����
	UInt16 sortId;
	// ���߱�id
	UInt32 itemDataId;
	// ������
	UInt16 totalGroups;
	// ÿ�����
	UInt32 copiesOfEachGroup;
	// ���Ļ���id
	UInt32 costCurrencyId;
	// ���ۣ�һ�ݣ�
	UInt32 unitPrice;
	// ���cd
	UInt32 coolDownIntervalBetweenGroups;
	// ÿ�ݽ���
	std::vector<ItemReward> rewardsPerCopy;
	// ��Ȩ����
	UInt32 weightingStepSize;
	// ��Ȩֵ
	UInt32 weightingValue;
	// �Ƿ񹫸�
	UInt8  isAnnounce;
};

class GambingItemDataEntryMgr : public Avalon::DataEntryMgrBase<GambingItemDataEntry>
	, public Avalon::Singleton<GambingItemDataEntryMgr>
{
public:
	bool AddDataEntryCustomer(UInt32 _actId, UInt32 _gambingItemId, UInt32 _sortId, const ItemRewardVec& soldItems, const std::vector<std::string>& strVec);
};


#endif