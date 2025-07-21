#include "CLMonsterPrefixDataEntry.h"

MonsterPrefixDataEntry::MonsterPrefixDataEntry()
{


}

MonsterPrefixDataEntry::~MonsterPrefixDataEntry()
{

}

bool MonsterPrefixDataEntry::Read(Avalon::DataRow& row)
{
    UInt32 tmpId = row.ReadUInt32();
    /*if (tmpId >= 256)
    {
        return false;
    }*/

    id = (UInt8)tmpId;
    type = row.ReadUInt8();
    bufferId = row.ReadUInt32();

	return true;
}