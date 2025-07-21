/** 
 @file  protocol.h
 @brief ENet protocol
*/
#ifndef __ENET_PROTOCOL_H__
#define __ENET_PROTOCOL_H__

#include "enet/types.h"

enum
{
   ENET_PROTOCOL_MINIMUM_MTU             = 548,   //原值：576
   ENET_PROTOCOL_MAXIMUM_MTU             = 548,  //原值：4096 MTU的默认值设置为576-20-8 = 548Byte（除去IP+UDP头，留给应用层的空间大小）
   ENET_PROTOCOL_MAXIMUM_PACKET_COMMANDS = 32,
   ENET_PROTOCOL_MINIMUM_WINDOW_SIZE     = 4096,
   ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE     = 65536,
   ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT   = 1,
   ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT   = 255,
   ENET_PROTOCOL_MAXIMUM_PEER_ID         = 0xFFF,  //最多2^12 -1个链接(4095)    peer id 就是ENetHost::peers数组下标  只占用ushort的 低12bit
   ENET_PROTOCOL_MAXIMUM_FRAGMENT_COUNT  = 1024 * 1024
};

typedef enum _ENetProtocolCommand //总共12个command，所以用4bit就可以标识
{
   ENET_PROTOCOL_COMMAND_NONE               = 0,
   ENET_PROTOCOL_COMMAND_ACKNOWLEDGE        = 1,
   ENET_PROTOCOL_COMMAND_CONNECT            = 2,  //连接请求 (client -> server)
   ENET_PROTOCOL_COMMAND_VERIFY_CONNECT     = 3,  //确认连接请求 (server -> client)  server收到ENET_PROTOCOL_COMMAND_CONNECT后的响应信息
   ENET_PROTOCOL_COMMAND_DISCONNECT         = 4,  //断开连接请求(主动断开方->对端)
   ENET_PROTOCOL_COMMAND_PING               = 5,
   ENET_PROTOCOL_COMMAND_SEND_RELIABLE      = 6,  //无分片的可靠的发送数据
   ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE    = 7,  //无分片的无邪可靠的发送数据
   ENET_PROTOCOL_COMMAND_SEND_FRAGMENT      = 8,  //以分片形式可靠的发送数据
   ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED   = 9,
   ENET_PROTOCOL_COMMAND_BANDWIDTH_LIMIT    = 10,
   ENET_PROTOCOL_COMMAND_THROTTLE_CONFIGURE = 11,
   ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE_FRAGMENT = 12,  //以分片形式无需可靠的发送数据
   ENET_PROTOCOL_COMMAND_COUNT              = 13,

   ENET_PROTOCOL_COMMAND_MASK               = 0x0F    //用于获取4bit的command的mask
} ENetProtocolCommand;

typedef enum _ENetProtocolFlag
{
   ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE = (1 << 7),
   ENET_PROTOCOL_COMMAND_FLAG_UNSEQUENCED = (1 << 6),

   ENET_PROTOCOL_HEADER_FLAG_COMPRESSED = (1 << 14),  //peer id的第14bit 是压缩标识位
   ENET_PROTOCOL_HEADER_FLAG_SENT_TIME  = (1 << 15),  //在可靠发送模式下，协议里才需要发送sent time（见enet_protocol_send_reliable_outgoing_commands()）
   ENET_PROTOCOL_HEADER_FLAG_MASK       = ENET_PROTOCOL_HEADER_FLAG_COMPRESSED | ENET_PROTOCOL_HEADER_FLAG_SENT_TIME,

   ENET_PROTOCOL_HEADER_SESSION_MASK    = (3 << 12),  //peer id的第12,13bit是 session id.
   ENET_PROTOCOL_HEADER_SESSION_SHIFT   = 12
} ENetProtocolFlag;

#ifdef _MSC_VER
#pragma pack(push, 1)
#define ENET_PACKED
#elif defined(__GNUC__) || defined(__clang__)
#define ENET_PACKED __attribute__ ((packed))
#else
#define ENET_PACKED
#endif

/************************************************************************/
/* ENet协议格式：
   ENetProtocolHeader   +  [0 - N] 个ENetProtocol(ENetProtocolCommandHeader + 具体Commander协议) + [可能存在的具体数据]
   ENetProtocol结构中只是包含了固定长度的信息，对于数据传输协议（比如：ENET_PROTOCOL_COMMAND_SEND_RELIABLE, ENET_PROTOCOL_COMMAND_SEND_FRAGMENT等)还有具体的数据
*/
/************************************************************************/


typedef struct _ENetProtocolHeader
{
   enet_uint16 peerID;		//通讯对端的peer id, 理论上对另外一方是透明的，只是透传
   enet_uint16 sentTime;
} ENET_PACKED ENetProtocolHeader;

typedef struct _ENetProtocolCommandHeader
{
   enet_uint8 command;
   enet_uint8 channelID;
   enet_uint16 reliableSequenceNumber;
} ENET_PACKED ENetProtocolCommandHeader;

typedef struct _ENetProtocolAcknowledge
{
   ENetProtocolCommandHeader header;
   enet_uint16 receivedReliableSequenceNumber;
   enet_uint16 receivedSentTime;
} ENET_PACKED ENetProtocolAcknowledge;

typedef struct _ENetProtocolConnect
{
   ENetProtocolCommandHeader header;
   enet_uint16 outgoingPeerID;  // ENetPeer::incomingPeerID也就是peer在host->peers中的数组下标
   enet_uint8  incomingSessionID;
   enet_uint8  outgoingSessionID;
   enet_uint32 mtu;
   enet_uint32 windowSize;
   enet_uint32 channelCount;
   enet_uint32 incomingBandwidth;
   enet_uint32 outgoingBandwidth;
   enet_uint32 packetThrottleInterval;
   enet_uint32 packetThrottleAcceleration;
   enet_uint32 packetThrottleDeceleration;
   enet_uint32 connectID;
   enet_uint32 data;
} ENET_PACKED ENetProtocolConnect;

typedef struct _ENetProtocolVerifyConnect
{
   ENetProtocolCommandHeader header;
   enet_uint16 outgoingPeerID;
   enet_uint8  incomingSessionID;
   enet_uint8  outgoingSessionID;
   enet_uint32 mtu;
   enet_uint32 windowSize;
   enet_uint32 channelCount;
   enet_uint32 incomingBandwidth;
   enet_uint32 outgoingBandwidth;
   enet_uint32 packetThrottleInterval;
   enet_uint32 packetThrottleAcceleration;
   enet_uint32 packetThrottleDeceleration;
   enet_uint32 connectID;
} ENET_PACKED ENetProtocolVerifyConnect;

typedef struct _ENetProtocolBandwidthLimit
{
   ENetProtocolCommandHeader header;
   enet_uint32 incomingBandwidth;
   enet_uint32 outgoingBandwidth;
} ENET_PACKED ENetProtocolBandwidthLimit;

typedef struct _ENetProtocolThrottleConfigure
{
   ENetProtocolCommandHeader header;
   enet_uint32 packetThrottleInterval;
   enet_uint32 packetThrottleAcceleration;
   enet_uint32 packetThrottleDeceleration;
} ENET_PACKED ENetProtocolThrottleConfigure;

typedef struct _ENetProtocolDisconnect
{
   ENetProtocolCommandHeader header;
   enet_uint32 data;
} ENET_PACKED ENetProtocolDisconnect;

typedef struct _ENetProtocolPing
{
   ENetProtocolCommandHeader header;
} ENET_PACKED ENetProtocolPing;

typedef struct _ENetProtocolSendReliable
{
   ENetProtocolCommandHeader header;
   enet_uint16 dataLength;
} ENET_PACKED ENetProtocolSendReliable;

typedef struct _ENetProtocolSendUnreliable
{
   ENetProtocolCommandHeader header;
   enet_uint16 unreliableSequenceNumber;
   enet_uint16 dataLength;
} ENET_PACKED ENetProtocolSendUnreliable;

typedef struct _ENetProtocolSendUnsequenced
{
   ENetProtocolCommandHeader header;
   enet_uint16 unsequencedGroup;
   enet_uint16 dataLength;
} ENET_PACKED ENetProtocolSendUnsequenced;

typedef struct _ENetProtocolSendFragment
{
   ENetProtocolCommandHeader header;
   enet_uint16 startSequenceNumber;
   enet_uint16 dataLength;
   enet_uint32 fragmentCount;
   enet_uint32 fragmentNumber;
   enet_uint32 totalLength;
   enet_uint32 fragmentOffset;
} ENET_PACKED ENetProtocolSendFragment;

typedef union _ENetProtocol
{
   ENetProtocolCommandHeader header;
   ENetProtocolAcknowledge acknowledge;
   ENetProtocolConnect connect;
   ENetProtocolVerifyConnect verifyConnect;
   ENetProtocolDisconnect disconnect;
   ENetProtocolPing ping;
   ENetProtocolSendReliable sendReliable;
   ENetProtocolSendUnreliable sendUnreliable;
   ENetProtocolSendUnsequenced sendUnsequenced;
   ENetProtocolSendFragment sendFragment;
   ENetProtocolBandwidthLimit bandwidthLimit;
   ENetProtocolThrottleConfigure throttleConfigure;
} ENET_PACKED ENetProtocol;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif /* __ENET_PROTOCOL_H__ */

