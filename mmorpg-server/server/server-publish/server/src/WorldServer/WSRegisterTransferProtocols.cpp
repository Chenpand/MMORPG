#include "WSNetwork.h"
#include "WorldTransferHandler.h"

template<typename ProtocolT>
class WSTransferPacketFunction : public CLParamPacketFunction<TransferHeader&>
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

#define WS_REGISTER_TRANSFER_PROTOCOL(ProtocolT) \
	RegisterTransferProtocol(ProtocolT::__ID, new WSTransferPacketFunction<ProtocolT>);

template<typename ProtocolT>
class WSTransferPlayerPacketFunction : public CLParamPacketFunction<TransferHeader&>
{
public:
	void operator()(Avalon::Packet* packet, TransferHeader& tHeader) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(tHeader.destObj);
			if (NULL == player)
			{
				ErrorLogStream << "transfer not find player " << tHeader.LogStr() << LogStream::eos;
				return;
			}

			HandleProtocol(player, tHeader, protocol);
		}
	}
};

#define WS_REGISTER_TRANSFER_PLAYER_PROTOCOL(ProtocolT) \
	RegisterTransferProtocol(ProtocolT::__ID, new WSTransferPlayerPacketFunction<ProtocolT>);

void WSNetwork::RegisterTransferProtocols()
{
	WS_REGISTER_TRANSFER_PROTOCOL(CLProtocol::TransferQueryPlayerTipsOne)
	WS_REGISTER_TRANSFER_PROTOCOL(CLProtocol::TransferQueryPlayerTwo)

	WS_REGISTER_TRANSFER_PLAYER_PROTOCOL(CLProtocol::TransferQueryPlayerDetailTwo)
	WS_REGISTER_TRANSFER_PLAYER_PROTOCOL(CLProtocol::TransferQueryPlayerTipsTwo)
}


