#include "PKOccuRecord.h"

PkOccuRecordMgr::PkOccuRecordMgr()
{
    
}

PkOccuRecordMgr::~PkOccuRecordMgr()
{

}

void PkOccuRecordMgr::AddPkOccuRecord(Occupation opponentOccu, PkRaceResult result)
{
	if (opponentOccu == OCCU_NONE)
	{
		return;
	}

    PkOccuRecord* record = FindPkOccuRecord(opponentOccu);
    if (!record)
    {
        PkOccuRecord occuRecord;
        occuRecord.occu = opponentOccu;
        m_pkOccuRecords.push_back(occuRecord);
        record = FindPkOccuRecord(opponentOccu);
        if (!record)
        {
            ErrorLogStream << "add pk occu(" << opponentOccu << ") record failed, result(" << result << ")." << LogStream::eos;
            return;
        }
    }

    if (result == PKRR_WIN)
    {
        record->winNum++;
    }
    else if (result == PKRR_LOSE)
    {
        record->loseNum++;
    }
    record->totalNum++;

    record->isDirty = true;

    SetDirty();
}

PkOccuRecord* PkOccuRecordMgr::FindPkOccuRecord(Occupation occu)
{
    for (auto& record : m_pkOccuRecords)
    {
        if (record.occu == occu)
        {
            return &record;
        }
    }

    return NULL;
}

void PkOccuRecordMgr::DecodeString(std::istringstream& is)
{
    PkOccuRecord record;
    UInt8 split;
    UInt32 occu;
    while (is >> occu && occu > 0)
    {
        record.occu = (UInt8)occu;
        is >> split >> record.winNum >> split >> record.loseNum >> split >> record.totalNum >> split;
        m_pkOccuRecords.push_back(record);
    }
}

void PkOccuRecordMgr::EncodeString(std::ostringstream& os)
{
    for (UInt32 i = 0; i < m_pkOccuRecords.size(); i++)
    {
        if (i)
        {
            os << "|";
        }

        auto& record = m_pkOccuRecords[i];
        os << (UInt32)record.occu << ":" << record.winNum << ":" << record.loseNum << ":" << record.totalNum;
    }
}

void PkOccuRecordMgr::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
    for (auto& record : m_pkOccuRecords)
    {
        if (record.isDirty || !bDirty)
        {
            stream & (UInt8)record.occu;
            record.Output(stream);
        }
        record.isDirty = false;
    }
    stream & (UInt8)0;
}

void PkOccuRecordMgr::Decode(Avalon::NetInputStream& stream)
{
    UInt8 occu = 0;
    while (true)
    {
        stream & occu;
        if (!occu)
        {
            break;
        }

        PkOccuRecord* record = FindPkOccuRecord((Occupation)occu);
        if (!record)
        {
            PkOccuRecord occuRecord;
            occuRecord.occu = occu;
            m_pkOccuRecords.push_back(occuRecord);
            record = FindPkOccuRecord((Occupation)occu);
            if (!record)
            {
                return;
            }
        }

        record->Input(stream);
    }
}