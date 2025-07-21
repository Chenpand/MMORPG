#ifndef __VERIFY_CENTER_HANDLER_H__
#define __VERIFY_CENTER_HANDLER_H__

#include <CLVerifyProtocol.h>

void HandleProtocol(CLProtocol::VerifyCenterRegisterReq& req);

void HandleProtocol(CLProtocol::VerifyCenterReportAbility& protocol);

#endif