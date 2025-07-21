#ifndef _AS_LOGGER_HANDLER_H_
#define _AS_LOGGER_HANDLER_H_

#include <CLSysProtocol.h>
#include <CLPacketExecutor.h>

void HandleProtocol(CLProtocol::SysNetLogger& protocol);

class DBLoggerFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

#endif
