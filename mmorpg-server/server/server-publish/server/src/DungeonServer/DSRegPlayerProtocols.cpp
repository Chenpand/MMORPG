#include "DSPlayer.h"

#include "DungeonHandler.h"
#include "DungeonRaceMgr.h"

CLParamPacketExecutor<DSPlayer*>	DSPlayer::m_Executor;

template<typename ProtocolT>
class DSPlayerPacketFunction : public CLParamPacketFunction<DSPlayer*>
{
public:
	void operator()(Avalon::Packet* packet, DSPlayer* player) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			HandleProtocol(player, protocol); //noted by aprilliu, HandleProtocol是全局函数，很多重载形式
		}
	}
};

#define DS_REGISTER_PROTOCOL(ProtocolT) \
	m_Executor.Bind(ProtocolT::__ID,new DSPlayerPacketFunction<ProtocolT>);

void DSPlayer::RegisterProtocols()
{
	DS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonEnterNextAreaReq);
	DS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonRewardReq);
	DS_REGISTER_PROTOCOL(CLProtocol::SceneDungeonKillMonsterReq);
}

bool DSPlayer::InitPlayerProtocols()
{
	RegisterProtocols();
	return true;
}

void DSPlayer::ExecutePacket(ObjID_t playerId, Avalon::Packet* packet)
{
	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(playerId);
	if (!dungeonRace)
	{
		ErrorLogStream << "execute packet err not find race player id = " << playerId << ", packet id = " << packet->GetID() << LogStream::eos;
		return;
	}

	DungeonPlayer * player = dungeonRace->FindDungeonPlayer(playerId);
	if (!player)
	{
		ErrorLogStream << "execute packet err not find player : " << playerId << "race id : " << dungeonRace->GetID() << ", raceSessionId : " 
			<< dungeonRace->GetSessionId() <<  ", packet id = " << packet->GetID() << LogStream::eos;
		return;
	}

	m_Executor.ExecutePacket(packet, player);
}

