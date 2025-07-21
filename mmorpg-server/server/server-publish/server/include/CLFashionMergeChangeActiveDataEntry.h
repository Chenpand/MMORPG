#ifndef _CL_FASHION_MERGE_CHANGE_ACTIVE_DATAENTRY_H_ 
#define _CL_FASHION_MERGE_CHANGE_ACTIVE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLItemDefine.h"

/**
*@brief �ٱ�ʱװ��ʱװ�ϳ�ģ���
*/
class FashionMergeChangeActiveDataEntry : public Avalon::DataEntry
{
public:
	FashionMergeChangeActiveDataEntry(){};
	~FashionMergeChangeActiveDataEntry(){};

public:
	virtual UInt32 GetKey() const { return id; };

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 5; };
public:
	//ʱװid
	UInt32 id;
	//�߼�ʱװid
	UInt32 advanceId;
	//�߼�ʱװ�ϳɸ���
	UInt32 pro;
	//��������
	ItemReward goldConsume;
	//��װ������
	ItemReward ticketConsume;
};

typedef Avalon::DataEntryVisitor<FashionMergeChangeActiveDataEntry> FashionMergeChangeActiveDataEntryVisitor;

class FashionMergeChangeActiveDataEntryMgr : public Avalon::DataEntryMgrBase<FashionMergeChangeActiveDataEntry>
	, public Avalon::Singleton<FashionMergeChangeActiveDataEntryMgr>
{

};

#endif