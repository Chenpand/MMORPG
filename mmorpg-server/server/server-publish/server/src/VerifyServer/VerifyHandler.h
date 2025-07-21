#ifndef __VERIFY_HANDLER_H__
#define __VERIFY_HANDLER_H__

#include <CLVerifyProtocol.h>

void HandleProtocol(CLProtocol::VerifyCreateTaskReq& req);

void HandleProtocol(CLProtocol::VerifyReportFrame& report);

void HandleProtocol(CLProtocol::VerifyReportChecksum& report);

void HandleProtocol(CLProtocol::VerifyReportRaceEnd& report);

void HandleProtocol(CLProtocol::VerifyReportRaceDismiss& report);

void HandleProtocol(CLProtocol::VerifyGiveupTask& protocol);

#endif