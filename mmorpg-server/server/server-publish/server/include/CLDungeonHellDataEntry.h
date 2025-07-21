#ifndef __CL_DUNGEON_HELL_DATA_ENTRY_H__
#define __CL_DUNGEON_HELL_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLDungeonDefine.h"

class DungeonHellDataEntry : public Avalon::DataEntry
{
public:
    DungeonHellDataEntry();
    virtual ~DungeonHellDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

private:

public:
    // id
    UInt32	                id;
	// �ؿ��ȼ�
	UInt16					dungeonLevel;
    // ģʽ
    DungeonHellMode         mode;
    // ˢ����
    std::vector<UInt32>     monsterGroups;
	// ����
	UInt32					prob;
	// Ҫ����ĵ��³�ID�б�
	std::vector<UInt32>		dungeonIDs;
	// �Ƿ���������³�ID
	UInt32					choiceType;
};

class DungeonHellDataEntryMgr : public Avalon::DataEntryMgrBase<DungeonHellDataEntry>
	, public Avalon::Singleton<DungeonHellDataEntryMgr>
{
public:
	// ��������
	virtual bool AddEntry(DungeonHellDataEntry* entry);

	// ��������³�ID
	UInt32 RandDungeonID(UInt16 dungeonLevel, DungeonHardType dungeonHard);
};

#endif