#ifndef _WS_CHAT_HANDLER_H_
#define _WS_CHAT_HANDLER_H_

#include <CLChatProtocol.h>
#include <CLPacketExecutor.h>

class TransChatFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

/**
 *@brief 处理缓存聊天链接数据
 */
class CacheChatDataFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldChatLinkDataReq& req);

void HandleProtocol(CLProtocol::WorldTransmitRequest& req);

void HandleProtocol(CLProtocol::WorldTransmitReply& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldOfflineNotifyReq& req);

void HandleProtocol(CLProtocol::WorldSendOfflineNotify& req);

void HandleProtocol(CLProtocol::WorldSendAnnouncement& req);

void HandleProtocol(CLProtocol::WorldChatHorn& req);

#endif
