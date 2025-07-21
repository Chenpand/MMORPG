/** 
 @file  callbacks.h
 @brief ENet callbacks
*/
#ifndef __ENET_CALLBACKS_H__
#define __ENET_CALLBACKS_H__

#include <stdlib.h>

/*
 Noted by aprilliu, �ڴ������Ż��ĵط���
 ������Ҫmalloc�Լ�free�ĵط����漰�����ݽṹ���£�
 1. ��������ʱ������һ�� ENetOutgoingCommandʱ    ���ݽṹ��ENetOutgoingCommand
 enet_peer_queue_outgoing_command (ENetPeer * peer, const ENetProtocol * command, ENetPacket * packet, enet_uint32 offset, enet_uint16 length)
 ->  ENetOutgoingCommand * outgoingCommand = (ENetOutgoingCommand *) enet_malloc (sizeof (ENetOutgoingCommand));

 2. �������ݰ��ṹʱ   ���ݽṹ��ENetPacket
 ENetPacket *
 enet_packet_create (const void * data, size_t dataLength, enet_uint32 flags)
 -> ENetPacket * packet = (ENetPacket *) enet_malloc (sizeof (ENetPacket));

 3. ����ȷ����Ϣ  ���ݽṹ��ENetAcknowledgement
 ENetAcknowledgement * //���ɶ�commandЭ���ȷ����Ϣ�������뵽ȷ����Ϣ����ENetPeer::acknowledgements��
 enet_peer_queue_acknowledgement (ENetPeer * peer, const ENetProtocol * command, enet_uint16 sentTime)

 --> acknowledgement = (ENetAcknowledgement *) enet_malloc (sizeof (ENetAcknowledgement));

 4. �������ݣ�����ENetIncomingCommand      ���ݽṹ��ENetIncomingCommand
 incomingCommand = (ENetIncomingCommand *) enet_malloc (sizeof (ENetIncomingCommand));  //����incomingCommand

 �����Ż����� �μ�AVALON::Packet �Ըýṹ�Ĺ��� �������ö༶MemPool<>  �Ż������ڴ�Ĺ����������ռ����������������Ч�ʣ����ܣ�

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

