#ifndef _AS_LOGIN_HANDLER_H_
#define _AS_LOGIN_HANDLER_H_

#include <CLLoginProtocol.h>

//noted by aprilliu. �����Э�鴦����ExecutePacket(Avalon::Packet* packet)�и���id�ҵ���Ӧ��Э�鴦��handler��ASPacketFunction::operator()�б�����
void HandleProtocol(CLProtocol::AdminLoginNewPlayer& req);

void HandleProtocol(CLProtocol::AdminPlayerGateInfoRet& ret);

void HandleProtocol(CLProtocol::GateNetAddressRet& protocol);

void HandleProtocol(CLProtocol::AdminSyncGatePlayerNum& protocol);

void HandleProtocol(CLProtocol::AdminSyncWorldPlayerNum& protocol);

void HandleProtocol(CLProtocol::AdminSyncLoginQueueInfo& protocol);

#endif
