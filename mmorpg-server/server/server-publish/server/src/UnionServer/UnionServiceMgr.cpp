#include "UnionServiceMgr.h"
#include "ChampionService.h"
#include "GoldConsignmentService.h"
#include "USGameParamMgr.h"
#include "USApplication.h"


bool UnionServiceMgr::Init(std::set<UnionServerFunctionType> &openFun)
{
	for (auto &type : openFun)
	{
		auto service = _CreateUnionService(type);
		if (service == nullptr)
		{
			return false;
		}
		m_UnionServieces.insert(std::make_pair(type, service));
	}
	return true;
}

void UnionServiceMgr::OnTick(const Avalon::Time & now)
{
	CheckTime(now);

	for (auto &iter : m_UnionServieces)
	{
		auto service = iter.second;
		service->OnTick(now);
	}
}

void UnionServiceMgr::OnQuit()
{
	for (auto &iter : m_UnionServieces)
	{
		auto service = iter.second;
		CL_SAFE_DELETE(service);
	}
	m_UnionServieces.clear();
}

void UnionServiceMgr::OnConnected(GameZone* zone)
{
	for (auto &iter : m_UnionServieces)
	{
		auto service = iter.second;
		service->OnConnected(zone);
	}
}

void UnionServiceMgr::CheckTime(const Avalon::Time& now)
{
	Avalon::Date date(now);
	date.Hour(6);
	date.Min(0);
	date.Sec(0);
	UInt32 dayChangeTime = USGameParamMgr::Instance()->GetValue("day_changed_time");
	if (now.Sec() >= dayChangeTime + DAY_TO_SEC)
	{
		dayChangeTime = UInt32(date.ToTime().Sec());
		USGameParamMgr::Instance()->SetValue("day_changed_time", dayChangeTime);

		for (auto &iter : m_UnionServieces)
		{
			iter.second->OnDayChanged();
		}
	}
}

void UnionServiceMgr::OnReceivePlayerProtocol(const CLProtocol::SysTransmitPlayerToUnion& protocol)
{
	auto service = _GetUnionService(static_cast<UnionServerFunctionType>(protocol.funType));
	if (service == nullptr)
	{
		return;
	}
	service->OnReceivePlayerProtocol(protocol);
}

void UnionServiceMgr::OnPlayerDisconnect(UInt64 roleId)
{
	for (auto &iter : m_UnionServieces)
	{
		auto service = iter.second;
		service->OnPlayerDisconnect(roleId);
	}
}

void UnionServiceMgr::OnPlayerSyncSceneObject(UInt64 roleID, Avalon::Packet * packet)
{
	for (auto &iter : m_UnionServieces)
	{
		auto service = iter.second;
		service->OnPlayerSyncSceneObject(roleID, packet);
	}
}

void UnionServiceMgr::OnPlayerDeleteSceneObject(UInt64 roleID, Avalon::Packet * packet)
{
	for (auto &iter : m_UnionServieces)
	{
		auto service = iter.second;
		service->OnPlayerDeleteSceneObject(roleID, packet);
	}
}

void UnionServiceMgr::OnPlayerSyncObjectProperty(UInt64 roleID, Avalon::Packet * packet)
{
	for (auto &iter : m_UnionServieces)
	{
		auto service = iter.second;
		service->OnPlayerSyncObjectProperty(roleID, packet);
	}
}

void UnionServiceMgr::OnPlayerMove(UInt64 roleID, Avalon::Packet* packet)
{
	for (auto &iter : m_UnionServieces)
	{
		auto service = iter.second;
		for (UInt32 i = 0; i < USApplication::Instance()->GetMoveCnt(); ++i)
		{
			service->OnPlayerMove(roleID, packet);
		}
	}
}

void UnionServiceMgr::OnSceneReady(CLProtocol::UnionSceneIsReady & protocol)
{
	for (auto &iter : m_UnionServieces)
	{
		auto service = iter.second;
		service->OnSceneReady(protocol);
	}
}

void UnionServiceMgr::OnWorldChgNewZoneID(UInt32 oldZoneID, UInt32 newZoneID)
{
	for (auto &iter : m_UnionServieces)
	{
		auto service = iter.second;
		service->OnWorldChgNewZoneID(oldZoneID, newZoneID);
	}
}

void UnionServiceMgr::OnActivityNotify(CLProtocol::UnionNotifyActivityInfo& protocol)
{
	auto service = m_UnionServieces.find(UnionServerFunctionType(protocol.funType));
	if (service != m_UnionServieces.end())
	{
		if (service->second != NULL)
		{
			service->second->OnActivityNotify(protocol);
		}
	}
}

UnionService* UnionServiceMgr::_GetUnionService(UnionServerFunctionType type)
{
	auto iter = m_UnionServieces.find(type);
	if (iter == m_UnionServieces.end())
	{
		return nullptr;
	}
	return iter->second;
}

UnionService* UnionServiceMgr::_CreateUnionService(UnionServerFunctionType type)
{
	UnionService* service = nullptr;
	switch (type)
	{
	case USFT_NONE:
		break;
	case USFT_ALL:
		break;
	case USFT_CHAMPION:
		service = new ChampionService;
		break;
	case USFT_GOLD_CONSIGNMENT:
		service = new GoldConsignmentService;
		break;
	default:
		break;
	}

	if (service == nullptr)
	{
		ErrorLogStream << "service(" << static_cast<UInt32>(type) << ") not exists." << LogStream::eos;
		return nullptr;
	}

	if (!service->Init())
	{
		ErrorLogStream << "service(" << static_cast<UInt32>(type) << ") init failed." << LogStream::eos;
		delete service;
		return nullptr;
	}

	return service;
}
