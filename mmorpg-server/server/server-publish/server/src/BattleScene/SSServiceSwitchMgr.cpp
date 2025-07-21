#include "SSServiceSwitchMgr.h"
#include <CLSettingProtocol.h>
#include "Player.h"
#include "SSRouter.h"


void SSServiceSwitchMgr::OnOpenService(ServiceType type)
{
	InfoLogStream << "service(" << type << ") opened..." << LogStream::eos;

	// 广播给所有玩家
	CLProtocol::SceneUpdateServiceSwitch update;
	update.type = type;
	update.isOpen = 1;
	Router::BroadcastToPlayer(update);
}

void SSServiceSwitchMgr::OnCloseService(ServiceType type)
{
	InfoLogStream << "service(" << type << ") closed..." << LogStream::eos;

	// 广播给所有玩家
	CLProtocol::SceneUpdateServiceSwitch update;
	update.type = type;
	update.isOpen = 0;
	Router::BroadcastToPlayer(update);
}

void SSServiceSwitchMgr::OnPlayerOnline(Player* player)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneSyncServiceSwitch sync;

	for (auto config : m_serviceConfigs)
	{
		if (!config.open)
		{
			sync.closedServices.push_back(config.type);
		}
	}

	player->SendProtocol(sync);
}