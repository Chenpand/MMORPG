/** 
 @file  callbacks.h
 @brief ENet callbacks
*/
#ifndef __ENET_CALLBACKS_H__
#define __ENET_CALLBACKS_H__

#include <stdlib.h>

/*
 Noted by aprilliu, 内存分配待优化的地方：
 经常需要malloc以及free的地方，涉及的数据结构如下：
 1. 发送数据时，生成一个 ENetOutgoingCommand时    数据结构：ENetOutgoingCommand
 enet_peer_queue_outgoing_command (ENetPeer * peer, const ENetProtocol * command, ENetPacket * packet, enet_uint32 offset, enet_uint16 length)
 ->  ENetOutgoingCommand * outgoingCommand = (ENetOutgoingCommand *) enet_malloc (sizeof (ENetOutgoingCommand));

 2. 生成数据包结构时   数据结构：ENetPacket
 ENetPacket *
 enet_packet_create (const void * data, size_t dataLength, enet_uint32 flags)
 -> ENetPacket * packet = (ENetPacket *) enet_malloc (sizeof (ENetPacket));

 3. 生成确认消息  数据结构：ENetAcknowledgement
 ENetAcknowledgement * //生成对command协议的确认消息，并插入到确认消息队列ENetPeer::acknowledgements中
 enet_peer_queue_acknowledgement (ENetPeer * peer, const ENetProtocol * command, enet_uint16 sentTime)

 --> acknowledgement = (ENetAcknowledgement *) enet_malloc (sizeof (ENetAcknowledgement));

 4. 接收数据，生成ENetIncomingCommand      数据结构：ENetIncomingCommand
 incomingCommand = (ENetIncomingCommand *) enet_malloc (sizeof (ENetIncomingCommand));  //生成incomingCommand

 具体优化方法 参见AVALON::Packet 对该结构的管理， 就是利用多级MemPool<>  优化定长内存的管理，以牺牲空间利用率来提高运行效率（性能）

*/

typedef struct _ENetCallbacks
{
    void * (ENET_CALLBACK * malloc) (size_t size);
    void (ENET_CALLBACK * free) (void * memory);
    void (ENET_CALLBACK * no_memory) (void);
} ENetCallbacks;

/** @defgroup callbacks ENet internal callbacks
    @{
    @ingroup private
*/
extern void * enet_malloc (size_t);
extern void   enet_free (void *);

/** @} */

#endif /* __ENET_CALLBACKS_H__ */

