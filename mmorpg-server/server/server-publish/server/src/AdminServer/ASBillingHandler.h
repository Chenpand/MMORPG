#ifndef _AS_BILLING_HANDLER_H_
#define _AS_BILLING_HANDLER_H_

#include <CLBillingProtocol.h>

void HandleProtocol(CLProtocol::WorldBillingGiveItemRet& protocol);

void HandleProtocol(CLProtocol::WorldBillingRoleInfoRet& protocol);

#endif
