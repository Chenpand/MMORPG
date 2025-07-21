#include "SceneTransferHandler.h"
#include "SSRouter.h"
#include "PlayerLoadCallback.h"

class PlayerLoadingForQueryInfo : public PlayerLoadingCallback
{
public:
	PlayerLoadingForQueryInfo(UInt32 procotolId, TransferHeader* header) : m_ProtocolId(procotolId), m_Header(header) {}
	virtual void OnFinish(Player* player)
	{
		DebugLogStream << PLAYERINFO(player) << "loading for query finish!" << LogStream::eos;

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}

		if (m_ProtocolId == CLProtocol::TRANSFER_QUERY_PLAYER_ONE)
		{
			CLProtocol::TransferQueryPlayerTwo req;
			player->OnOhterPlayerWatch(req.watchInfo);

			Avalon::Packet* packet = Avalon::Packet::Create();
			if (req.Encode(packet))
			{
				CLProtocol::SysTransferProtocol transfer(packet);
				transfer.tHeader = *m_Header;
				Router::SendToWorld(transfer);
			}
			else
			{
				Avalon::Packet::Destroy(packet);
			}
		}
		else if (m_ProtocolId == CLProtocol::TRANSFER_QUERY_PLAYER_DETAIL_ONE)
		{
			CLProtocol::TransferQueryPlayerDetailTwo req;
			req.info = player->GetMatchRacePlayerInfo(false);
			m_Header->SwapHeader();
			Router::SendToTransfer(*m_Header, req);
		}

		CL_SAFE_DELETE(m_Header);
	}

	virtual void OnFailure(Player* player)
	{
		ErrorLogStream << PLAYERINFO(player) << "loading failed for query!" << LogStream::eos;

		if (player->GetGameStatus() == PS_INIT)
		{
			ErrorLogStream << PLAYERINFO(player) << "loading failed wait to destory..." << LogStream::eos;
			player->SetGameStatus(PS_DESTORY);
		}
		CL_SAFE_DELETE(m_Header);
	}

	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
		CL_SAFE_DELETE(m_Header);
	}

private:
	UInt32 m_ProtocolId;
	TransferHeader* m_Header;
};

void HandleProtocol(TransferHeader& tHeader, CLProtocol::TransferQueryPlayerOne& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(tHeader.destObj);
	if (NULL == player)
	{
		TransferHeader* header = new TransferHeader;
		*header = tHeader;
		PlayerMgr::Instance()->LoadOfflinePlayerCb(tHeader.destObj, 0, new PlayerLoadingForQueryInfo(protocol.GetProtocolID(), header));
		return;
	}

	CLProtocol::TransferQueryPlayerTwo req;
	player->OnOhterPlayerWatch(req.watchInfo);

	Avalon::Packet* packet = Avalon::Packet::Create();
	if (req.Encode(packet))
	{
		CLProtocol::SysTransferProtocol transfer(packet);
		transfer.tHeader = tHeader;
		Router::SendToWorld(transfer);
	}
	else
	{
		Avalon::Packet::Destroy(packet);
	}
}

void HandleProtocol(Player* player, TransferHeader& tHeader, CLProtocol::TransferQueryPlayerTwo& protocol)
{
	if (NULL == player) return;

	CLProtocol::WorldQueryPlayerRet ret;
	ret.info = protocol.watchInfo;
	ret.zoneId = tHeader.srcZone;
	player->SendProtocol(ret);
}

void HandleProtocol(TransferHeader& tHeader, CLProtocol::TransferQueryPlayerDetailOne& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(tHeader.destObj);
	if (NULL == player)
	{
		TransferHeader* header = new TransferHeader;
		*header = tHeader;
		PlayerMgr::Instance()->LoadOfflinePlayerCb(tHeader.destObj, 0, new PlayerLoadingForQueryInfo(protocol.GetProtocolID(), header));
		return;
	}

	CLProtocol::TransferQueryPlayerDetailTwo req;
	req.info = player->GetMatchRacePlayerInfo(false);
	tHeader.SwapHeader();
	Router::SendToTransfer(tHeader, req);
}