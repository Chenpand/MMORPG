#ifndef __CL_MONSTER_ATTRIBUTE_TABLE_H__
#define __CL_MONSTER_ATTRIBUTE_TABLE_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLMonsterDataEntry.h"
#include "CLDungeonDataEntry.h"

/**
*@brief ����������
*/
class MonsterAttributeDataEntry : public Avalon::DataEntry
{
public:
    MonsterAttributeDataEntry() {}
    ~MonsterAttributeDataEntry() {}

public:
	UInt32 GetKey() const { return MakeKey(difficulty, type, mode, level); }

	static UInt32 MakeKey(UInt32 diff, UInt32 type, UInt32 mode, UInt32 level)
	{
		return diff * 10000000 + type * 100000 + mode * 1000 + level;
	}

    bool Read(Avalon::DataRow& row);

public:
    // id
    UInt32	                id;
    // �����Ѷ�(��DungeonHardType�Ͳ�1)
    UInt8                   difficulty;
    // ��������
    MonsterType				type;
	// ����ģʽ
	UInt8					mode;
    // �ȼ�
    UInt16                  level;
    // ����
    UInt32                  exp;
    // ����ID
    std::vector<UInt32>     dropSetIds;
};

typedef Avalon::DataEntryMgr<MonsterAttributeDataEntry> MonsterAttributeDataEntryMgr;

#endif