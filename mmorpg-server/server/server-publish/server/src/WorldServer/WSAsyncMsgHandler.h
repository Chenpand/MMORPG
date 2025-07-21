#ifndef __WS_ASYNC_MSG_HANDLER_H__
#define __WS_ASYNC_MSG_HANDLER_H__

#include <CLSysProtocol.h>

void HandleProtocol(CLProtocol::SysAsyncWaitMsg& protocol);

void RegisterAsyncMsgHandlers();

#endif
