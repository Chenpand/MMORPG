#ifndef __CL_DUNGEON_AREA_DATA_ENTRY_H__
#define __CL_DUNGEON_AREA_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum DungeonAreaPointType
{
    DAPT_MONSTER,						// ����
    DAPT_MONSTER_GROUP,					// ������
    DAPT_DESTRUCT,						// ���ƻ���
	DAPT_ACTIVITY_MONSTER,				// �С��
	DAPT_ACTIVITY_MONSTER_SMALL_BOSS,	// �СBOSS
	DAPT_ACTIVITY_MONSTER_BIG_BOSS,		// ���BOSS
	DAPT_NUM,
};

/**
*@brief ���³�����������
*/
class DungeonAreaDataEntry : public Avalon::DataEntry
{
public:
    DungeonAreaDataEntry();
    ~DungeonAreaDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
    // id
    UInt32	                id;
    // ����ID
    UInt32                  areaId;
    // ��ID
    UInt32                  groupId;
    // ˢ������
    DungeonAreaPointType    type;
    // ����ID��ID�������type������
    UInt32                  dataId;
    // ���ٻ����Ĺ���(��ӦMonsterGroup���е�ID)
    UInt32                  summonMonsterGroupId;
    // ����ȼ�
    UInt16                  level;
};

typedef Avalon::DataEntryMgr<DungeonAreaDataEntry> DungeonAreaDataEntryMgr;
typedef Avalon::DataEntryVisitor<DungeonAreaDataEntry> DungeonAreaDataEntryVisitor;

#endif