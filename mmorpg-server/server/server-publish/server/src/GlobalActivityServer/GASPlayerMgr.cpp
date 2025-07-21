#include "GASPlayerMgr.h"

#include <CLGlobalActivityServerDefine.h>


GASPlayerMgr::GASPlayerMgr()
{

}

GASPlayerMgr::~GASPlayerMgr()
{

}

bool GASPlayerMgr::Init()
{
	RegisterProtocols();

	return true;
}

void GASPlayerMgr::Final()
{

}

bool GASPlayerMgr::AddPlayer(GASPlayer* player)
{
	return m_Players.AddObject(player);
}

GASPlayer* GASPlayerMgr::FindPlayer(const std::string& name)
{
	return m_Players.FindObject(name);
}

void GASPlayerMgr::RemovePlayer(GASPlayer* player)
{
	m_Players.RemoveObject(player);
}

void GASPlayerMgr::VisitPlayers(GASPlayerVisitor& visitor)
{
	m_Players.VisitObjects(visitor);
}

void GASPlayerMgr::OnTick(const Avalon::Time& now)
{
	class HeartbeatVisitor : public GASPlayerVisitor
	{
	public:
		HeartbeatVisitor(const Avalon::Time& now) :m_Now(now) {}

		bool operator()(GASPlayer* player)
		{
			if (player != NULL && m_Now.Sec() > player->GetRemoveTime())
			{
				GASPlayerMgr::Instance()->RemovePlayer(player);
				CL_SAFE_DELETE(player);
			}

			if (player != NULL)
			{
				player->OnTick(m_Now);
			}

			return true;
		}

	private:
		Avalon::Time m_Now;
	};
	HeartbeatVisitor visitor(now);
	VisitPlayers(visitor);
}

void GASPlayerMgr::ExecuteProtocol(const CLProtocol::SysTransmitPlayerFormNodesToGlobal& protocol)
{
	std::string uid = EncodeGASPlayerUId(protocol.playerInfo.platform, protocol.playerInfo.serverZoneId, protocol.playerInfo.accId, protocol.playerInfo.roleId);

	GASPlayer* player = FindPlayer(uid);
	if (!player)
	{
		player = new GASPlayer();
		player->SetID(protocol.playerInfo.roleId);
		player->SetName(uid);
		player->SetPlayerName(protocol.playerInfo.name);
		player->SetAccId(protocol.playerInfo.accId);
		player->SetPlatform(protocol.playerInfo.platform);
		player->SetServerZoneId(protocol.playerInfo.serverZoneId);
		player->SetServerName(protocol.playerInfo.serverName);

		for (UInt32 serverType = ST_INVALID; serverType != SERVER_TYPE_MAX; ++serverType)
		{
			player->SetSourceNode((ServerType)serverType, protocol.playerInfo.sourceNode[serverType]);
		}

		AddPlayer(player);
	}

	player->SetRemoveTime(CURRENT_TIME.Sec() + DAY_TO_SEC);

	m_Executor.ExecutePacket(protocol.packet, player);
}