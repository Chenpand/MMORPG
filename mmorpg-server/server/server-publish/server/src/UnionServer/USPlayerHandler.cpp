#include "USPlayerHandler.h"
#include "USPlayerMgr.h"
#include "UnionServiceMgr.h"



void HandleProtocol(CLProtocol::UnionServerAddPlayerReq& protocol)
{
	InfoLogStream << "receive UnionServerAddPlayerReq:" << protocol.accId << LogStream::eos;

	auto service = UnionServiceMgr::Instance()->_GetUnionService(static_cast<UnionServerFunctionType>(protocol.funType));
	if (service == nullptr)
	{
		return;
	}
	service->AddNewPlayer(protocol);
}

void HandleProtocol(CLProtocol::UnionServerDelPlayerReq& protocol)
{
	InfoLogStream << "receive UnionServerDelPlayerReq:" << protocol.roleId << LogStream::eos;

	auto service = UnionServiceMgr::Instance()->_GetUnionService(static_cast<UnionServerFunctionType>(protocol.funType));
	if (service == nullptr)
	{
		return;
	}
	service->RemovePlayer(protocol);
}