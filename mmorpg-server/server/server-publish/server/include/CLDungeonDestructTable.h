#ifndef __CL_DESTRUCT_DATA_ENTRY_H__
#define __CL_DESTRUCT_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ø…∆∆ªµŒÔ
*/
class DungeonDestructDataEntry : public Avalon::DataEntry
{
public:
    DungeonDestructDataEntry() {}
    ~DungeonDestructDataEntry() {}

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
    // id
    UInt32	                id;
    // µÙ¬‰ID
    std::vector<UInt32>     dropSetIds;
};

typedef Avalon::DataEntryMgr<DungeonDestructDataEntry> DungeonDestructDataEntryMgr;

#endif