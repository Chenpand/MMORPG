#include "USPlayer.h"

#include <CLErrorCode.h>
#include <CLItemProtocol.h>
#include <CLSysProtocol.h>
#include <CLUnionServerProtocol.h>

#include "GameZoneMgr.h"
#include "USApplication.h"
#include "USItemTransaction.h"
#include "USItemThing.h"

USPlayer::USPlayer()
{
	m_ZoneId = 0;
	m_AccId = 0;
	m_Occu = 0;
	m_Level = 0;
	m_VipLvl = 0;
	m_EquipScore = 0;
	m_ConnId = 0;
	m_CurSceneType = 0;
	m_SceneNodeId = 0;
	m_WorldNodeId = 0;
	m_GuildId = 0;
	m_Awaken = 0;
	IsLoading = 0;
	m_pItemTrans = NULL;
	m_pItemThing = NULL;
}

USPlayer::~USPlayer()
{

}

void USPlayer::OnTick(const Avalon::Time& now)
{
	if (m_pItemTrans != NULL && m_pItemTrans->GetTimeOut() != 0)
	{
		if (m_pItemTrans->CheckTimeOut(now))
		{
			CL_SAFE_DELETE(m_pItemTrans);
		}
	}

	if (m_pItemThing != NULL && m_pItemThing->GetTimeOut() != 0)
	{
		if (m_pItemThing->CheckTimeOut(now))
		{
			m_pItemThing->OnTimeOut(this);
			CL_SAFE_DELETE(m_pItemThing);
		}
	}
}

void USPlayer::SendPacket(Avalon::Packet* packet)
{
	if (!CanSendProtocol())
	{
		return;
	}
	auto zone = GameZoneMgr::Instance()->FindGameZone(m_ConnId);
	if (zone != NULL)
	{
		zone->SendToPlayer(GetID(), packet);
	}
	else
	{
		ErrorLogStream << "[US_PROTOCOL] cant find zone(" << m_ConnId << ") when send protocol(" << packet->GetID() << ")." << LogStream::eos;
	}
}

void USPlayer::SendProtocol(Avalon::Protocol& protocol)
{
	if (!CanSendProtocol())
	{
		return;
	}
	auto zone = GameZoneMgr::Instance()->FindGameZone(m_ConnId);
	if (zone != NULL)
	{
		zone->SendToPlayer(GetID(), protocol);
	}
	else
	{
		ErrorLogStream << "[US_PROTOCOL] cant find zone(" << m_ConnId << ") when send protocol(" << protocol.GetProtocolID() << ")." << LogStream::eos;
	}
}

void USPlayer::SendToScene(Avalon::Packet* packet)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(GetConnId());
	if (NULL == zone)
	{
		ErrorLogStream << PLAYERINFO(this) << " send to scene failed connid:" << GetConnId() << LogStream::eos;
		return;
	}

	zone->TransmitToScene(packet);
}

void USPlayer::SendToScene(Avalon::Protocol& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(GetConnId());
	if (NULL == zone)
	{
		ErrorLogStream << PLAYERINFO(this) << "player send to scene failed connid:" << GetConnId() << LogStream::eos;
		return;
	}

	zone->TransmitToScene(protocol);
}

void USPlayer::SendToWorld(Avalon::Packet* packet)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(GetConnId());
	if (NULL == zone)
	{
		ErrorLogStream << PLAYERINFO(this) << " send to scene failed connid:" << GetConnId() << LogStream::eos;
		return;
	}

	zone->TransmitToWorld(packet);
}

void USPlayer::SendToWorld(Avalon::Protocol& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(GetConnId());
	if (NULL == zone)
	{
		ErrorLogStream << PLAYERINFO(this) << "player send to scene failed connid:" << GetConnId() << LogStream::eos;
		return;
	}

	zone->TransmitToWorld(protocol);
}

void USPlayer::SendToScenePlayer(Avalon::Packet * packet)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(GetConnId());
	if (NULL == zone)
	{
		ErrorLogStream << PLAYERINFO(this) << "player send to scene failed connid:" << GetConnId() << LogStream::eos;
		return;
	}

	zone->SendToScenePlayer(GetID(), GetSceneNodeId(), packet);
}

void USPlayer::SendToScenePlayer(Avalon::Protocol & protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(GetConnId());
	if (NULL == zone)
	{
		ErrorLogStream << PLAYERINFO(this) << "player send to scene failed connid:" << GetConnId() << LogStream::eos;
		return;
	}

	zone->SendToScenePlayer(GetID(), GetSceneNodeId(), protocol);
}


bool USPlayer::BeginItemTransaction(USItemTransaction* transaction)
{
	if (NULL == transaction) return false;
	if (transaction->GetStatus() != CLItemTransaction::STATUS_INIT)
	{
		CL_SAFE_DELETE(transaction);
		return false;
	}

	//当前事务没结束
	if (m_pItemTrans != NULL && m_pItemTrans->m_Status != CLItemTransaction::STATUS_FINAL)
	{
		CL_SAFE_DELETE(transaction);
		return false;
	}

	CL_SAFE_DELETE(m_pItemTrans);
	m_pItemTrans = transaction;
	m_pItemTrans->CheckItem();

	return true;
}

void USPlayer::OnCheckItemRet(UInt32 serial, UInt32 result, UInt32 counterTimes)
{
	if (m_pItemTrans == NULL ||
		m_pItemTrans->GetStatus() != CLItemTransaction::STATUS_CHECK ||
		m_pItemTrans->GetSerial() != serial)
	{
		return;
	}

	if (result == ErrorCode::SUCCESS)
	{
		m_pItemTrans->OnCheckSucced(this);
	}
	else
	{
		m_pItemTrans->SetErrorCode(result);
		m_pItemTrans->OnCheckFailed(this);
	}
}

bool USPlayer::BeginItemThing(USItemThing* itemThing)
{
	if (NULL == itemThing) return false;

	//当前事务没结束
	if (m_pItemThing != NULL)
	{
		return false;
	}

	m_pItemThing = itemThing;
	m_pItemThing->StartThing();

	return true;
}

void USPlayer::BeginItemThingRes(UInt32 result, UInt64 serialId, UInt64 thingId)
{
	if (NULL == m_pItemThing)
	{
		WarnLogStream << "item time out thingId:" << thingId << " serialId:" << serialId << " result:" << result << LogStream::eos;
		return;
	}

	if (m_pItemThing->GetSerialId() != serialId)
	{
		WarnLogStream << PLAYERINFO(this) << " cur serial id:" << m_pItemThing->GetSerialId() << " rcev serial id:" << serialId << LogStream::eos;
		return;
	}

	if (result != ErrorCode::SUCCESS)
	{
		m_pItemThing->OnCheckFailed(this, result);
		CL_SAFE_DELETE(m_pItemThing);
		return;
	}

	m_pItemThing->OnCheckSucced(this, thingId);
	CL_SAFE_DELETE(m_pItemThing);
}

void USPlayer::CommitItemThingRes(UInt32 result, UInt64 thingId)
{
	DebugLogStream << PLAYERINFO(this) << " thingId commit item thing failed:" 
			<< thingId << " result:" << result << LogStream::eos;
}

void USPlayer::AddRewards(const std::string& reason, const ItemRewardVec& itemVec, bool isNotify)
{
	CLProtocol::UnionPlayerGetItem reward;
	reward.roleId = GetID();
	reward.reason = reason;
	reward.rewards = itemVec;
	reward.notice = isNotify ? 1 : 0;
	SendToScenePlayer(reward);
}