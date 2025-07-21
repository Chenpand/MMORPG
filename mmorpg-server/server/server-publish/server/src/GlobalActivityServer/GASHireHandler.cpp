#include "GASHireHandler.h"
#include "GASApplication.h"
#include "GASHireMgr.h"
#include <CLErrorCode.h>

void HandleProtocol(CLProtocol::GASCheckHireBindReq& protocol)
{
	CLProtocol::GASCheckHireBindRet res;
	res.errorCode = ErrorCode::SUCCESS;
	res.accid = protocol.accid;
	res.zoneId = protocol.zoneId;
	res.code = protocol.code;
	if (GASHireMgr::Instance()->IsAlreadyBind(protocol.accid, protocol.platform))
	{
		res.errorCode = ErrorCode::HIRE_BINDED_OTHER;
		protocol.SendBack(res);
		return;
	}

	GASHireMgr::Instance()->AddToBindedSet(protocol.accid, protocol.platform);
	protocol.SendBack(res);
}

void HandleProtocol(CLProtocol::WorldQueryHireAlreadyBindReq & protocol)
{
	CLProtocol::WorldQueryHireAlreadyBindRes res;
	res.zoneId = protocol.zoneId;
	res.accid = protocol.accid;
	if (GASHireMgr::Instance()->IsAlreadyBind(protocol.accid, protocol.platform))
	{
		res.errorCode = ErrorCode::HIRE_BINDED_OTHER;
		protocol.SendBack(res);
		return;
	}
	res.errorCode = ErrorCode::SUCCESS;
	protocol.SendBack(res);
}
