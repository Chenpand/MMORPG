#ifndef _WORLD_SYS_RECORD_HANDLER_H_
#define _WORLD_SYS_RECORD_HANDLER_H_

#include <CLSysRecordProtocol.h>

void HandleProtocol(CLProtocol::SceneSysRecordDataReq& req);

void HandleProtocol(CLProtocol::SceneSysRecordDataSync& req);

#endif