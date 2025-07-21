#include "SSNetwork.h"
#include "SceneTransferHandler.h"

template<typename ProtocolT>
class SSTransferPacketFunction : public CLParamPacketFunction<TransferHeader&>
{
public:
	void operator()(Avalon::Packet* packet, TransferHeader& tHeader) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			HandleProtocol(tHeader, protocol);
		}
	}
};

#define SS_REGISTER_TRANSFER_PROTOCOL(ProtocolT) \
	RegisterTransferProtocol(ProtocolT::__ID, new SSTransferPacketFunction<ProtocolT>);

template<typename ProtocolT>
class SSTransferPlayerPacketFunction : public CLParamPacketFunction<TransferHeader&>
{
public:
	void operator()(Avalon::Packet* packet, TransferHeader& tHeader) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			Player* player = PlayerMgr::Instance()->FindPlayer(tHeader.destObj);
			if (NULL == player)
			{
				ErrorLogStream << "transfer not find player " << tHeader.LogStr() << LogStream::eos;
				return;
			}
		
			HandleProtocol(player, tHeader, protocol);
		}
	}
};

#define SS_REGISTER_TRANSFER_PLAYER_PROTOCOL(ProtocolT) \
	RegisterTransferProtocol(ProtocolT::__ID, new SSTransferPlayerPacketFunction<ProtocolT>);

void SSNetwork::RegisterTransferProtocols()
{
	SS_REGISTER_TRANSFER_PROTOCOL(CLProtocol::TransferQueryPlayerOne)
	SS_REGISTER_TRANSFER_PLAYER_PROTOCOL(CLProtocol::TransferQueryPlayerTwo)
	SS_REGISTER_TRANSFER_PROTOCOL(CLProtocol::TransferQueryPlayerDetailOne)
}