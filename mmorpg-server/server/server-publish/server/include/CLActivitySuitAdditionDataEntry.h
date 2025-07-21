#ifndef __ACTIVITY_SUIT_ADDITION_DATA_ENTRY_H__
#define __ACTIVITY_SUIT_ADDITION_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <set>

enum ActivityMonsterAdditionType
{
	// ʱװ
	AMAT_FASHION,
	// �¿�
	AMAT_MONTH_CARD,
};

/**
*@brief �ʱװ�ӳɱ�
*/
class ActivitySuitAdditionDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32					id;
	// �ӳ�����
	ActivityMonsterAdditionType		type;
	// װ���б�
	std::vector<std::vector<UInt32>> equipList;
	// ������б�
	std::vector<UInt32>		activityMonsterList;
	// ����������
	std::set<UInt32>		doubleDropItems;
	// �����������
	UInt32					doubleDropRate;
	// ����ˢ�¸��ʼӳ�
	UInt32					monsterRateAddition[3];
};

typedef Avalon::DataEntryVisitor<ActivitySuitAdditionDataEntry> ActivitySuitAdditionDataEntryVisitor;
typedef Avalon::DataEntryMgr<ActivitySuitAdditionDataEntry> ActivitySuitAdditionDataEntryMgr;

#endif