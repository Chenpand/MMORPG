#ifndef _AS_LOGIN_HANDLER_H_
#define _AS_LOGIN_HANDLER_H_

#include <CLLoginProtocol.h>

//noted by aprilliu. 具体的协议处理，在ExecutePacket(Avalon::Packet* packet)中根据id找到相应的协议处理handler，ASPacketFunction::operator()中被调用
void HandleProtocol(CLProtocol::AdminLoginNewPlayer& req);

void HandleProtocol(CLProtocol::AdminPlayerGateInfoRet& ret);

void HandleProtocol(CLProtocol::GateNetAddressRet& protocol);

void HandleProtocol(CLProtocol::AdminSyncGatePlayerNum& protocol);

void HandleProtocol(CLProtocol::AdminSyncWorldPlayerNum& protocol);

void HandleProtocol(CLProtocol::AdminSyncLoginQueueInfo& protocol);

#endif
