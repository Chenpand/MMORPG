#ifndef __CL_DUNGEON_CHEST_DATA_ENTRY_H__
#define __CL_DUNGEON_CHEST_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLDropItemDefine.h>

/**
*@brief ���㱦��������
*/
struct DungeonChestDataEntry : public Avalon::DataEntry
{
public:
    DungeonChestDataEntry();
    ~DungeonChestDataEntry();

public:
    UInt32 GetKey() const { return dungeonId; }

    bool Read(Avalon::DataRow& row);


public:
    // ���³�ID
    UInt32      dungeonId;
	// ����
	UInt32		score;
    // ��ҷ���ID
    UInt32      goldDropSetId;
    // �����䷽��ID
    UInt32      chestDropoSetIds[DCT_NUM];
	// ���ѱ������Ļ�������
	UInt32		payChestCostItemId;
    // ���ѱ������Ļ�������
    UInt32      payChestCost;
};




class DungeonChestDataEntryMgr : public Avalon::DataEntryMgrBase<DungeonChestDataEntry>
	, public Avalon::Singleton<DungeonChestDataEntryMgr>
{
public:
	DungeonChestDataEntry* FindData(UInt32 dungeonId, UInt32 score);
	virtual bool AddEntry(DungeonChestDataEntry* entry);
	virtual ~DungeonChestDataEntryMgr();
private:
	std::map<UInt64, DungeonChestDataEntry*> m_Datas;

};

#endif