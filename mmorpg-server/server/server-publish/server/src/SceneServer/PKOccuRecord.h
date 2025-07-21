#ifndef __PK_OCCU_RECORD_INFO_H__
#define __PK_OCCU_RECORD_INFO_H__

#include <CLObjectProperty.h>
#include <CLMatchProtocol.h>
#include <CLJobDataEntry.h>

/**
*对不同职业的战绩
*/
struct PkOccuRecord
{
    PkOccuRecord()
    {
        memset(this, 0, sizeof(*this));
    }

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & occu & winNum & loseNum & totalNum;
    }

    // 职业类型
    UInt8           occu;

    // 胜场数
    UInt32          winNum;

    // 负场数
    UInt32          loseNum;

    // 总场数
    UInt32          totalNum;

    bool            isDirty;
};

class PkOccuRecordMgr : public CLComplexProperty
{
public:
    PkOccuRecordMgr();
    ~PkOccuRecordMgr();

    virtual void DecodeString(std::istringstream& is);
    virtual void EncodeString(std::ostringstream& os);

    virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
    virtual void Decode(Avalon::NetInputStream& stream);

    void AddPkOccuRecord(Occupation opponentOccu, PkRaceResult result);
    PkOccuRecord* FindPkOccuRecord(Occupation occu);

private:
    std::vector<PkOccuRecord>       m_pkOccuRecords;
};

#endif