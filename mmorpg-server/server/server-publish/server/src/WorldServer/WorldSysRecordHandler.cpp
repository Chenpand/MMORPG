#include "WorldSysRecordHandler.h"

#include <CLRareItemDataEntry.h>
#include "SysRecordMgr.h"


void HandleProtocol(CLProtocol::SceneSysRecordDataReq& req)
{
	SysRecordMgr::Instance()->OnSysRecordDataReq(req);
}

void HandleProtocol(CLProtocol::SceneSysRecordDataSync& req)
{
	SysRecordMgr::Instance()->RecordGotRareItem((RareControlType)req.type, req.ownerId, req.param1, req.addNum);
}