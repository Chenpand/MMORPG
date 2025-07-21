#ifndef __CL_DUNGEON_PREFIX_LIMIT_ENTRY_H__
#define __CL_DUNGEON_PREFIX_LIMIT_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ���³Ǵ�׺����������
*/
class DungeonPrefixLimitDataEntry : public Avalon::DataEntry
{
public:
    DungeonPrefixLimitDataEntry() {}
    ~DungeonPrefixLimitDataEntry() {}

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
    // id
    UInt32	                id;
    // ��͵ȼ�
    UInt16                  minLevel;
    // ��͵ȼ�
    UInt16                  maxLevel;
    // �Ѷ�
    UInt8                   hardType;
    // ��׺������
    UInt32                  prefixMonsterTotalNum;
    // �߼���׺����
    UInt32                  seniorPrefixNum;
    // �ͼ���׺����
    UInt32                  lowPrefixNum;
};

typedef Avalon::DataEntryMgr<DungeonPrefixLimitDataEntry> DungeonPrefixLimitDataMgr;

#endif