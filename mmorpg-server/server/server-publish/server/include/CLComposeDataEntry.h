#ifndef _CL_COMPOSE_DATAENTRY_H_
#define _CL_COMPOSE_DATAENTRY_H_

#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <map>

#define MAKE_COMPOSE_KEY(ItemId, Quality) ((ItemId << 4) + Quality)

/**
 *@brief �ϳ�������
 */
struct ComposeDataEntry  : public Avalon::DataEntry
{
	typedef std::map<UInt32, UInt32> RateMap;

public:
	ComposeDataEntry();
	~ComposeDataEntry();

public:
	UInt32 GetKey() const { return MAKE_COMPOSE_KEY(itemid, quality); }

	bool Read(Avalon::DataRow& row);

public:
	//����id
	UInt32	itemid;
	//Ʒ��
	UInt8	quality;
	//���ϳ�����
	UInt32	maxNum;
	//����
	UInt32	materials[MAX_ITEM_COMPOSE_MATERIAL_NUM];
	//������
	UInt32	materialNum[MAX_ITEM_COMPOSE_MATERIAL_NUM];
	//�ֽ׶θ�������
	RateMap	rates;
};

typedef Avalon::DataEntryMgr<ComposeDataEntry> ComposeDataEntryMgr;

#endif
