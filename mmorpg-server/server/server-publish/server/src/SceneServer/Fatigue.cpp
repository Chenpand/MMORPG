#include "Fatigue.h"
#include "Player.h"
#include <CLSystemValueDataEntry.h>


//const UInt16 NORMAL_PLAYER = 156;
//const UInt16 VIP_PLAYER = 188;

Fatigue::Fatigue() :m_pOwner(NULL), m_UseFatigue(0)
{
	m_Fatigue = 0;
	m_MaxFatigue = (UInt16)(SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_FATIGUE_MAX));
}

void Fatigue::SetOwner(Player* owner)
{
	m_pOwner = owner;
}

void Fatigue::SetMaxFatigue(UInt16 num)
{
	m_MaxFatigue = num;
	SetDirty();
}

UInt32 Fatigue::GetLastFatigue()
{
	UInt32 result = GetOwner()->GetCounter(COUNTER_LAST_FATIGUE);
	if (result != 0)
	{
		GetOwner()->SetCounter(COUNTER_LAST_FATIGUE, 0);
		InfoLogStream << PLAYERINFO(GetOwner()) << " last fatigue : " << result << " GetLastFatigue " << LogStream::eos;
	}
	return result;
}

void Fatigue::AddFatigue(const char* reason, UInt16 fatigue, bool notify)
{
	if (fatigue == 0) return;
	UInt16 oldFatigue = m_Fatigue;
	m_Fatigue = m_Fatigue + fatigue;
	m_Fatigue = m_Fatigue > m_MaxFatigue ? m_MaxFatigue : m_Fatigue;

	UInt16 nowAddFatigue = (UInt16)m_Fatigue - oldFatigue;

	SetDirty();

	GameInfoLogStream("Property") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() 
		<< ") Add Fatigue:" << fatigue << ", Actual Add:" << m_Fatigue - oldFatigue << ", Fatigue = " << m_Fatigue 
		<< ", UseFatigue = " << m_UseFatigue << LogStream::eos;

	GetOwner()->SendAssetUdpLog(AssetType::ASSET_FATIGUE, reason, (UInt64)oldFatigue, (Int64)fatigue, (Int64)nowAddFatigue, (UInt64)m_Fatigue);

	if (notify)
	{
		GetOwner()->SendNotify(2217, fatigue);
	}
}

void Fatigue::RemoveFatigue(const char* reason, UInt16 fatigue)
{
	if (fatigue == 0) return;
	UInt16 oldFatigue = m_Fatigue;
	UInt16 oldUseFatigue = m_UseFatigue;
	if (m_Fatigue < fatigue)
	{
		m_UseFatigue += m_Fatigue;
		m_Fatigue = 0;
	}
	else
	{
		m_UseFatigue += fatigue;
		m_Fatigue = m_Fatigue - fatigue;
	}

	UInt16 removeFatigue = oldFatigue - m_Fatigue;
	GetOwner()->OnRemoveFatigue(removeFatigue);

	SetDirty();

	GameInfoLogStream("Property") << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() <<
		") Remove Fatigue:" << fatigue << ", Actual Remove:" << oldFatigue - m_Fatigue << ", Fatigue = " << m_Fatigue
		<< ", UseFatigue(" << oldUseFatigue << "|" << m_UseFatigue << ")" << LogStream::eos;

	GetOwner()->SendAssetUdpLog(AssetType::ASSET_FATIGUE, reason, (UInt64)oldFatigue, (Int64)(removeFatigue * -1), (Int64)(removeFatigue * -1), (UInt64)m_Fatigue);

}

void Fatigue::RefreshFatigue(const Avalon::Time& now)
{
	GetOwner()->SetCounter(COUNTER_LAST_FATIGUE, m_Fatigue);
	UInt32 oldFatigue = m_Fatigue;

	if (m_pOwner->HasMonthCard())
	{
		SetMaxFatigue((UInt16)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FATIGUE_MAX) + (UInt16)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MONTH_CARD_ADD_FATIGUE));
		GetOwner()->SetCounter(COUNTER_IS_INC_MONTH_CARD_FATIGUE, 1);
	}

	//Ë¢ÐÂ
	m_LastRefreshTime = now;
	m_UseFatigue = 0;
	m_Fatigue = m_MaxFatigue;

	UInt32 addFatigue = (UInt32)m_Fatigue - oldFatigue;

	SetDirty();

	GetOwner()->OnRefreshFatigue();

	InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") old(" << oldFatigue <<
		") Refresh Fatigue(" << (UInt32)m_Fatigue << "|" << (UInt32)m_MaxFatigue << "|" << m_LastRefreshTime.Sec() << ") succeed" << LogStream::eos;

	GetOwner()->SendAssetUdpLog(AssetType::ASSET_FATIGUE, GetReason(LogSourceType::SOURCE_TYPE_PLAYER_REFRESH).c_str(), (UInt64)oldFatigue, (Int64)addFatigue, (Int64)addFatigue, (UInt64)m_Fatigue);
}


void Fatigue::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & m_Fatigue & m_MaxFatigue;
}

void Fatigue::Decode(Avalon::NetInputStream& stream)
{
	stream & m_Fatigue & m_MaxFatigue;
}

void Fatigue::DecodeString(std::istringstream& is)
{
	char split = ',';
	UInt32 time = 0;
	if (!is.eof())
	{
		UInt16 tmpVal = 0;
		is >> tmpVal >> split >> m_UseFatigue >> split >> time;
		m_Fatigue = tmpVal;
		m_LastRefreshTime = Avalon::Time((UInt64)time * (UInt64)Avalon::Time::MSECS_OF_SEC);
	}
}

void Fatigue::EncodeString(std::ostringstream& os)
{
	os << m_Fatigue << "," << m_UseFatigue << "," << m_LastRefreshTime.Sec();
}

void Fatigue::OnBirth()
{
	RefreshFatigue(CURRENT_TIME);
}

void Fatigue::OnOnline(bool bDayChanged)
{
	if (bDayChanged)
	{
		RefreshFatigue(CURRENT_TIME);
	}
}

void Fatigue::OnHeartbeat(const Avalon::Time& now, int tickType)
{
}
