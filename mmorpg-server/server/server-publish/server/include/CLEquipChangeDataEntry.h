#ifndef _CL_EQUIP_CHAGNGE_DATAENTRY_H_
#define _CL_EQUIP_CHAGNGE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief װ��ת����
*/
struct EquipChangeDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	// Ψһid
	UInt32	id;
	// Сְҵid
	std::vector<UInt32> jobIds;
	// ת������
	UInt8	convertType;
	// ��װװ��id
	std::vector<UInt32> suitEquips;
	// ��װ�ȼ�
	UInt32	suitLevel;
};

class EquipChangeDataEntryMgr : public Avalon::DataEntryMgrBase<EquipChangeDataEntry>
	, public Avalon::Singleton<EquipChangeDataEntryMgr>
{
public:
	virtual bool AddEntry(EquipChangeDataEntry* entry);
	UInt32  GetSuitIdByEqId(UInt32 eqId);
private:
	std::map<UInt32, UInt32> equipMapSuit;
};


#endif
