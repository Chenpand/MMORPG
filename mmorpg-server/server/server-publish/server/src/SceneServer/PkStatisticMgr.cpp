#include "PkStatisticMgr.h"
#include <CLRecordClient.h>
#include "Player.h"

PkStatisticMgr::PkStatisticMgr()
{
    m_pOwner = NULL;
    memset(m_pkStatistices, 0, sizeof(m_pkStatistices));
}

PkStatisticMgr::~PkStatisticMgr()
{
    for (UInt32 i = 0; i < sizeof(m_pkStatistices) / sizeof(m_pkStatistices[0]); i++)
    {
        if (m_pkStatistices[i])
        {
            delete m_pkStatistices[i];
        }
    }
}

void PkStatisticMgr::OnOnline()
{
    //SCENE_SYNC_PK_STATISITC_INFO;
    Avalon::Packet* packet = Avalon::Packet::Create();
    packet->SetID(CLProtocol::SCENE_SYNC_PK_STATISITC_INFO);

    Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
    for (UInt32 i = 0; i < sizeof(m_pkStatistices) / sizeof(m_pkStatistices[0]); i++)
    {
        if (!m_pkStatistices[i])
        {
            continue;
        }
        
        stream & m_pkStatistices[i]->GetID();
        m_pkStatistices[i]->Encode(stream, SST_SELF);
    }
    if (stream.GetPos() > 0)
    {
        stream & (UInt64)0;
        packet->SetSize(UInt16(stream.GetPos()));
        GetOwner()->SendPacket(packet);
    }

    Avalon::Packet::Destroy(packet);
}

void PkStatisticMgr::OnUpdatePkStatistic(PkType type, Occupation opponentOccu, PkRaceResult result)
{
    if (type >= PK_NUM)
    {
        ErrorLogStream << "player(" << GetOwner()->GetID() << ") wants to update invalid pk type(" << type << ")." << LogStream::eos;
        return;
    }

    bool isNew = false;
    PkStatistic* pkInfo = FindPkStatistic(type);
    if (!pkInfo)
    {
        isNew = true;
        pkInfo = PkStatistic::CreatePkStatistic(type);
        if (!pkInfo)
        {
            ErrorLogStream << "player(" << GetOwner()->GetID() << ") create pk instance failed, type(" << type << ")." << LogStream::eos;
            return;
        }

        pkInfo->SetID(CLRecordClient::Instance()->GenGuid());
        m_pkStatistices[type] = pkInfo;
    }

    pkInfo->OnUpdatePkStatistic(opponentOccu, result);
    _RecordPkStatistic(pkInfo, isNew, true);
}

void PkStatisticMgr::OnClearPkStatistic()
{
	for (UInt32 i = 0; i < PK_NUM; i++)
	{
		auto statistic = m_pkStatistices[i];
		if (!statistic)
		{
			continue;
		}
		
		statistic->Clear();
		_RecordPkStatistic(statistic, false, true);
	}
}

PkStatistic* PkStatisticMgr::FindPkStatistic(PkType type)
{
    if (type >= sizeof(m_pkStatistices) / sizeof(m_pkStatistices[0]))
    {
        return NULL;
    }

    return m_pkStatistices[type];
}

bool PkStatisticMgr::LoadPkStatistic(PkStatistic* pkStatistic)
{
    if (!pkStatistic)
    {
        return false;
    }

    if (pkStatistic->GetType() >= PK_NUM)
    {
        delete pkStatistic;
        return false;
    }

    if (FindPkStatistic(pkStatistic->GetType()))
    {
        ErrorLogStream << "player(" << GetOwner()->GetID() << ") has repeat pk statistic(" << pkStatistic->GetType() << ") data." << LogStream::eos;
        delete pkStatistic;
        return false;
    }

    m_pkStatistices[pkStatistic->GetType()] = pkStatistic;
    return true;
}

void PkStatisticMgr::_RecordPkStatistic(PkStatistic* pkInfo, bool isNew, bool isSyncToPlayer)
{
    if (!pkInfo)
    {
        return;
    }

    CLSaveCommand* cmd = NULL;
    if (isNew)
    {
        cmd = CLRecordClient::Instance()->PrepareInsertCommand(GetOwner()->GetSrcZoneId(), "t_pk_statistic", pkInfo->GetID());
    }
    else
    {
        cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_pk_statistic", pkInfo->GetID(), false);
    }

    pkInfo->GetDBData(cmd, false);
    cmd->PutData("owner", GetOwner()->GetID());
    CLRecordClient::Instance()->SendCommand(cmd);

    if (isSyncToPlayer)
    {
        // 同步到客户端
        Avalon::Packet* packet = Avalon::Packet::Create();
        packet->SetID(CLProtocol::SCENE_SYNC_PK_STATISITC_PROPERTY);

        Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
        stream & (UInt8)pkInfo->GetType();
        pkInfo->Encode(stream, SST_SELF, true);
        if (stream.GetPos() > 0)
        {
            packet->SetSize(UInt16(stream.GetPos()));
            GetOwner()->SendPacket(packet);
        }

        Avalon::Packet::Destroy(packet);
    }
}

void PkStatisticMgr::Input(Avalon::NetInputStream &stream)
{
    while (true)
    {
        UInt64 id = 0;
        stream & id;
        if (id == 0)
        {
            break;
        }

        PkStatistic* pkInfo = PkStatistic::CreatePkStatistic(PK_NORMAL_1V1);
        if (!pkInfo)
        {
            break;
        }
        pkInfo->Decode(stream);
        LoadPkStatistic(pkInfo);
    }
}

void PkStatisticMgr::Output(Avalon::NetOutputStream &stream)
{
    for (UInt32 i = 0; i < sizeof(m_pkStatistices) / sizeof(m_pkStatistices[0]); i++)
    {
        if (!m_pkStatistices[i])
        {
            continue;
        }

        stream & m_pkStatistices[i]->GetID();
        m_pkStatistices[i]->Encode(stream, SST_SELF);
    }
    stream & (UInt64)0;
}