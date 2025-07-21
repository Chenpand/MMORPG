#include "RSNetwork.h"
#include <CLProtocolDefine.h>
#include "RSRecordHandler.h"

template<typename ProtocolT>
class RSPacketFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const
	{
		ProtocolT protocol;
		if(protocol.Decode(packet))
		{
			HandleProtocol(protocol);
		}
	}
};

#define RS_REGISTER_PROTOCOL(ProtocolT) \
	RegisterProtocol(ProtocolT::__ID,new RSPacketFunction<ProtocolT>());


void RSNetwork::RegisterProtocols()
{
	RegisterProtocol(CLProtocol::RECORD_INSERT_REQ,new InsertFunction());
	RegisterProtocol(CLProtocol::RECORD_UPDATE_REQ,new UpdateFunction());
	RegisterProtocol(CLProtocol::RECORD_DELETE_REQ,new DeleteFunction());
	RegisterProtocol(CLProtocol::RECORD_SELECT_REQ,new SelectFunction());
	RegisterProtocol(CLProtocol::RECORD_FLUSH_REQ, new FlushFunction());
	RS_REGISTER_PROTOCOL(CLProtocol::RecordGuidseedReq)
	RegisterProtocol(CLProtocol::RECORD_CHANGENAME_REQ, new ChangeNameFunction());
	RegisterProtocol(CLProtocol::RECORD_AUTO_GUID_INSERT_REQ, new AutoGuidInsertFunction());

	RS_REGISTER_PROTOCOL(CLProtocol::RecordTransmitRequest)
    RS_REGISTER_PROTOCOL(CLProtocol::RecordTransmitResponse)
	RS_REGISTER_PROTOCOL(CLProtocol::RecordNotifyOtherDisconnected)
}

