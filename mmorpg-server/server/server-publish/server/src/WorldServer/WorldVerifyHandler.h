#ifndef __WORLD_VERIFY_HANDLER_H__
#define __WORLD_VERIFY_HANDLER_H__

#include <CLVerifyProtocol.h>

void HandleProtocol(CLProtocol::VerifyCenterNotifyVSInfo& protocol);
void HandleProtocol(CLProtocol::VerifyCenterNotifyVSOnline& protocol);
void HandleProtocol(CLProtocol::VerifyCenterNotifyVSOffline& protocol);
void HandleProtocol(CLProtocol::VerifyCenterNotifyAbility& protocol);

void HandleProtocol(CLProtocol::VerifyCreateTaskRes& res);

void HandleProtocol(CLProtocol::VerifyReportTaskResult& protocol);

void HandleProtocol(CLProtocol::VerifyReportTaskRaceEnd& protocol);

#endif