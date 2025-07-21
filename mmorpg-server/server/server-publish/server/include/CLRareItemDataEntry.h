#ifndef _CL_RARE_ITEM_DATAENTRY_H_
#define _CL_RARE_ITEM_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLSysRecordDefine.h"

// һ���ڵ�ʱ��,���㼸��
struct DayTime
{
	DayTime()
	{
		memset(this, 0, sizeof(*this));
	}

	UInt32 hour;
	UInt32 minute;
	UInt32 second;
};

// ����ʱ��
struct DayOpenTime
{
	DayTime		startTime;
	DayTime		endTime;
};

/**
*@brief ϡ�п�����Ʒ������
*/
class RareItemDataEntry : public Avalon::DataEntry
{
public:
	RareItemDataEntry() {}
	~RareItemDataEntry() {}

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

	bool IsInOpenTime(const Avalon::Date& now);

public:
	// ID
	UInt32						id;
	// ϡ�п�������
	RareControlType				type;
	// �����ID���������;���
	UInt32						valueId;
	// �����޶�
	UInt32						minPayCost;
	// ʱ������
	std::vector<DayOpenTime>	timeLimit;
	// ʱ��������ʱ��(min)
	UInt32						timeLimitTotalMin;
	// ����ÿ�ղ���
	UInt32						persionalDailyNum;
	// ����ÿ�ܲ���
	UInt32						persionalWeeklyNum;
	// ����ÿ�²���
	UInt32						persionalMonthlyNum;
	// �����ܲ���
	UInt32						persionalTotalNum;
	// ������ÿ�ղ���
	UInt32						serverDailyNum;
	// ������ÿ�ܲ���
	UInt32						serverWeeklyNum;
	// ������ÿ�²���
	UInt32						serverMonthlyNum;
	// �������ܲ���
	UInt32						serverTotalNum;
};

class RareItemDataEntryMgr : public Avalon::DataEntryMgrBase<RareItemDataEntry>, public Avalon::Singleton<RareItemDataEntryMgr>
{
public:
	virtual bool AddEntry(RareItemDataEntry* dataEntry);

	RareItemDataEntry* FindEntryByType(RareControlType type, UInt32 id);

private:
	std::map<std::pair<RareControlType, UInt32>, RareItemDataEntry*> m_rareItems;
};

#endif
