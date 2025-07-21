#ifndef _CL_MALL_PERSONAL_TAILOR_DATA_ENTRY_H_
#define _CL_MALL_PERSONAL_TAILOR_DATA_ENTRY_H_


#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLDefine.h>


// ˽�˶��Ʊ�����
struct MallPersonalTailorDataEntry : public Avalon::DataEntry
{
public:
	MallPersonalTailorDataEntry();
	~MallPersonalTailorDataEntry();

public:
	UInt32 GetKey() const { return activateCond; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 7; };

public:
	// ��������
	UInt8				activateCond;

	// ����������
	UInt8				activateSubCond;

	// �̳���Ʒid
	std::vector<UInt32>	mallGoodsIds;

	// ����������
	UInt8				activateSubType;

	// ��ҵȼ�����
	UInt16				playerLevelLimit;

	// ��ʱʱ��(Сʱ)	
	UInt16				limitInterval;

	// ��ɫ������������
	UInt16				roleActivateTimesLimit;
};


class MallPersonalTailorDataEntryMgr : public Avalon::DataEntryMgrBase<MallPersonalTailorDataEntry>
	, public Avalon::Singleton<MallPersonalTailorDataEntryMgr>
{
public:
	MallPersonalTailorDataEntryMgr();
	~MallPersonalTailorDataEntryMgr();

	bool AddEntry(MallPersonalTailorDataEntry* entry);
};

#endif