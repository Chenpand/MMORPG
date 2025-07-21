#include "ScenePremiumLeagueHandler.h"
#include "PlayerMgr.h"
#include "PlayerLoadCallback.h"

class PlayerLoadingForAddHonor : public PlayerLoadingCallback
{
public:
	PlayerLoadingForAddHonor(const std::string& _reason, UInt32 _pvpType, UInt32 _honor) : reason(_reason), pvpType(_pvpType), honor(_honor) {}
	virtual void OnFinish(Player* player)
	{
		InfoLogStream << "player(" << player->GetID() << ") loading for add honor finish." << LogStream::eos;

		player->AddHonor(reason.c_str(), honor);
		player->GetHistoryHonor().OnPvP(pvpType, honor);

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}
	}

	virtual void OnFailure(Player* player)
	{
		ErrorLogStream << "player(" << player->GetID() << "offline add honor failed! reason:" << reason
			<< " pvpType:" << pvpType << " honor:" << honor << LogStream::eos;

		if (player->GetGameStatus() == PS_INIT)
		{
			ErrorLogStream << PLAYERINFO(player) << "loading failed wait to destory..." << LogStream::eos;
			player->SetGameStatus(PS_DESTORY);
		}
	}

	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

private:
	std::string reason;
	UInt32 pvpType;
	UInt32 honor;
};

void HandleProtocol(CLProtocol::WorldPremiumLeagueEndToScene& end)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(end.roleId);
	if (player != NULL)
	{
		player->AddHonor(end.reason.c_str(), end.honor);
		player->GetHistoryHonor().OnPvP(end.pvpType, end.honor);
	}
	else
	{
		PlayerMgr::Instance()->LoadOfflinePlayerCb(end.roleId, 0,
			new PlayerLoadingForAddHonor(end.reason, end.pvpType, end.honor));
	}

}