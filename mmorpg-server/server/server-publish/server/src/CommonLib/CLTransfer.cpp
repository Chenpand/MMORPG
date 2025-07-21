#include <CLTransfer.h>

CLTransfer::CLTransfer()
{

}

CLTransfer::~CLTransfer()
{

}

void CLTransfer::RegisterTransferProtocol(UInt32 id, CLParamPacketFunction<TransferHeader&>* func)
{
	m_TransferExecutor.Bind(id, func);
}

bool CLTransfer::ExecuteTransferPacket(TransferHeader& tHeader, Avalon::Packet* packet)
{
	return m_TransferExecutor.ExecutePacket(packet, tHeader);
}

