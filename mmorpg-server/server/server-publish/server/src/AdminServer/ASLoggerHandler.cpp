#include "ASLoggerHandler.h"
#include <AvalonLogger.h>
#include "DBLoggerMgr.h"

void HandleProtocol(CLProtocol::SysNetLogger& protocol)
{
	Avalon::Logger* logger = Avalon::Logger::getInstance(protocol.name.c_str());
	if(logger == NULL) return;

	Avalon::LogEvent logEvent;
	logEvent.time = Avalon::Time(protocol.now);
	logEvent.priority = Avalon::LogPriority(protocol.priority);
	memmove(logEvent.content, protocol.content.c_str(), protocol.content.length());
	logEvent.content[protocol.content.length()] = 0;
	logger->Log(logEvent.priority, logEvent);
}

void DBLoggerFunction::operator()(Avalon::Packet* packet) const
{
	Avalon::Packet* logpacket = Avalon::Packet::Create(packet->GetSize());
	packet->CopyTo(logpacket);
	DBLoggerMgr::Instance()->ProcessLogPacket(logpacket);
}

