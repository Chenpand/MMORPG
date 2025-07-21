#ifndef __CL_MONSTER_GROUP_DATA_ENTRY_H__
#define __CL_MONSTER_GROUP_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// ��������������
enum MonsterGroupLossyType
{
    // ��
    MGLT_NULL,
    // ����������
    MGLT_AREA,
    // ȫ������
    MGLT_GLOBAL,
};

/**
*@brief ˢ�ֱ�
*/
class MonsterGroupDataEntry : public Avalon::DataEntry
{
public:
    MonsterGroupDataEntry() {}
    ~MonsterGroupDataEntry() {}

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
    // id�����Ǹ�������ID���Զ�����
    UInt32                  id;
    // ����ID
    UInt32                  projID;
    // ��ID
    UInt32                  groupID;
	// �����
	UInt32					groupProb;
    // ��������
    MonsterGroupLossyType   lossyType;
    // ��������
    UInt8                   lossyNum;
    // ����
    UInt32                  prob;
    // �����б�
    std::vector<UInt32>     monsterList;
    
};

typedef Avalon::DataEntryMgr<MonsterGroupDataEntry> MonsterGroupDataEntryMgr;

#endif