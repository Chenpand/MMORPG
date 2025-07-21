#include "PkStatistic.h"

void PkRecentRecord::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
    if (IsDirty(type) || !bDirty)
    {
        for (int i = 0; i < MAX_RECENT_RECORD_NUM; i++)
        {
            stream & m_record[i];
        }
    }
}

void PkRecentRecord::Decode(Avalon::NetInputStream& stream)
{
    for (int i = 0; i < MAX_RECENT_RECORD_NUM; i++)
    {
        stream & m_record[i];
    }
}

void PkRecentRecord::PushRecentRecord(PkRaceResult result)
{
    for (int i = MAX_RECENT_RECORD_NUM - 1; i > 0; i--)
    {
        m_record[i] = m_record[i - 1];
    }
    m_record[0] = (UInt8)result;
    SetDirty();
}

void PkRecentRecord::DecodeString(std::istringstream& is)
{
    UInt8 index = 0;
    char result;

    while (is >> result && result != 0)
    {
        m_record[index++] = (UInt8)result - '0';
    }
}

void PkRecentRecord::EncodeString(std::ostringstream& os)
{
    for (int i = 0; i < MAX_RECENT_RECORD_NUM && m_record[i]; i++)
    {
        os << (UInt32)m_record[i];
    }
}

PkStatistic::PkStatistic()
{
	Clear();
}

PkStatistic::~PkStatistic()
{

}

void PkStatistic::Clear()
{
	m_totalWinNum = 0;
	m_totalLoseNum = 0;
	m_totalNum = 0;
	m_maxWinStreak = 0;
	m_curWinStreak = 0;
	m_curLoseStreak = 0;
}

void PkStatistic::OnUpdatePkStatistic(Occupation opponentOccu, PkRaceResult result)
{
    if (result == PKRR_WIN)
    {
        m_totalWinNum = m_totalWinNum + 1;
        m_curWinStreak = m_curWinStreak + 1;
        if (m_curWinStreak > m_maxWinStreak)
        {
            m_maxWinStreak = m_curWinStreak;
        }
		m_curLoseStreak = 0;
    }
    else if (result == PKRR_LOSE)
    {
		m_curLoseStreak = m_curLoseStreak + 1;
        m_totalLoseNum = m_totalLoseNum + 1;
        m_curWinStreak = 0;
    }
    else if (result == PKRR_DOGFALL)
    {
        m_curWinStreak = 0;
		m_curLoseStreak = 0;
    }
    else
    {
        return;
    }

    m_totalNum = m_totalNum + 1;
    m_recentRecord.PushRecentRecord(result);
    m_occuRecordMgr.AddPkOccuRecord(opponentOccu, result);
}

PkStatistic* PkStatistic::CreatePkStatistic(PkType type)
{
    PkStatistic* pkStatistic = new PkStatistic();
    if (!pkStatistic)
    {
        return NULL;
    }

    pkStatistic->SetType(type);
    return pkStatistic;
}