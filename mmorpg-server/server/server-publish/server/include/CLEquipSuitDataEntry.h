#ifndef _CL_EQUIP_SUIT_DATAENTRY_H_
#define _CL_EQUIP_SUIT_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//��װ����
class EquipSuitDataEntry : public Avalon::DataEntry
{
public:
	EquipSuitDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	// ��װ�ȼ�
	UInt32 suitLev;
	// ��ͬ������Ӧ����װ����
	UInt32 attrIdByEquipNum[6];
	// ��װ��������
	UInt32 eqScoreAdjust;
	// ��ͬ������Ӧ����װ����
	UInt32 attrIdByEquipNumNew[9];
};
typedef Avalon::DataEntryMgr<EquipSuitDataEntry> EquipSuitDataEntryMgr;
#endif
