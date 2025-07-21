/** 
 @file  protocol.c
 @brief ENet protocol functions
*/
#include <stdio.h>
#include <string.h>
#define ENET_BUILDING_LIB 1
#include "enet/utility.h"
#include "enet/time.h"
#include "enet/enet.h"

static size_t commandSizes [ENET_PROTOCOL_COMMAND_COUNT] =
{
    0,
    sizeof (ENetProtocolAcknowledge),
    sizeof (ENetProtocolConnect),
    sizeof (ENetProtocolVerifyConnect),
    sizeof (ENetProtocolDisconnect),
    sizeof (ENetProtocolPing),
    sizeof (ENetProtocolSendReliable),
    sizeof (ENetProtocolSendUnreliable),
    sizeof (ENetProtocolSendFragment),
    sizeof (ENetProtocolSendUnsequenced),
    sizeof (ENetProtocolBandwidthLimit),
    sizeof (ENetProtocolThrottleConfigure),
    sizeof (ENetProtocolSendFragment)
};

size_t
enet_protocol_command_size (enet_uint8 commandNumber)
{
    return commandSizes [commandNumber & ENET_PROTOCOL_COMMAND_MASK];
}

static void
enet_protocol_change_state (ENetHost * host, ENetPeer * peer, ENetPeerState state)
{
    if (state == ENET_PEER_STATE_CONNECTED || state == ENET_PEER_STATE_DISCONNECT_LATER)
      enet_peer_on_connect (peer);
    else
      enet_peer_on_disconnect (peer);

    peer -> state = state;

    LOGD("peer(%p) new state(%d) userid(%u)\n",peer, peer->state, peer->eventData);
}

static void
enet_protocol_dispatch_state (ENetHost * host, ENetPeer * peer, ENetPeerState state)
{
    enet_protocol_change_state (host, peer, state);

    if (! peer -> needsDispatch)
    {
       enet_list_insert (enet_list_end (& host -> dispatchQueue), & peer -> dispatchList);

       peer -> needsDispatch = 1;
    }
}

static int  //在enet_host_service 被调用  ENetHost::dispatchQueue （ENetPeer队列）
enet_protocol_dispatch_incoming_commands (ENetHost * host, ENetEvent * event)
{
    while (! enet_list_empty (& host -> dispatchQueue))
    {
       ENetPeer * peer = (ENetPeer *) enet_list_remove (enet_list_begin (& host -> dispatchQueue));

       peer -> needsDispatch = 0;

       switch (peer -> state)
       {
       case ENET_PEER_STATE_CONNECTION_PENDING:
       case ENET_PEER_STATE_CONNECTION_SUCCEEDED:
           enet_protocol_change_state (host, peer, ENET_PEER_STATE_CONNECTED);

           event -> type = ENET_EVENT_TYPE_CONNECT;
           event -> peer = peer;
           event -> data = peer -> eventData;

           return 1;
           
       case ENET_PEER_STATE_ZOMBIE:
           host -> recalculateBandwidthLimits = 1;

           event -> type = ENET_EVENT_TYPE_DISCONNECT;
           event -> peer = peer;
           event -> data = peer -> eventData;

           enet_peer_reset (peer);

           return 1;

       case ENET_PEER_STATE_CONNECTED:
           if (enet_list_empty (& peer -> dispatchedCommands))
             continue;

           event -> packet = enet_peer_receive (peer, & event -> channelID);
           if (event -> packet == NULL)
             continue;
             
           event -> type = ENET_EVENT_TYPE_RECEIVE;
           event -> peer = peer;

           if (! enet_list_empty (& peer -> dispatchedCommands)) //如果还有收到的command，则继续派发
           {
              peer -> needsDispatch = 1;
         
              enet_list_insert (enet_list_end (& host -> dispatchQueue), & peer -> dispatchList);
           }

           return 1;

       default:
           break;
       }
    }

    return 0;
}

static void
enet_protocol_notify_connect (ENetHost * host, ENetPeer * peer, ENetEvent * event)
{
    host -> recalculateBandwidthLimits = 1;

    if (event != NULL) //填充ENetEvent结构
    {
        enet_protocol_change_state (host, peer, ENET_PEER_STATE_CONNECTED);

        event -> type = ENET_EVENT_TYPE_CONNECT;
        event -> peer = peer;
        event -> data = peer -> eventData; //
    }
    else 
        enet_protocol_dispatch_state (host, peer, peer -> state == ENET_PEER_STATE_CONNECTING ? ENET_PEER_STATE_CONNECTION_SUCCEEDED : ENET_PEER_STATE_CONNECTION_PENDING);
}

static void
enet_protocol_notify_disconnect (ENetHost * host, ENetPeer * peer, ENetEvent * event)
{
    if (peer -> state >= ENET_PEER_STATE_CONNECTION_PENDING)
       host -> recalculateBandwidthLimits = 1;

    if (peer -> state != ENET_PEER_STATE_CONNECTING && peer -> state < ENET_PEER_STATE_CONNECTION_SUCCEEDED)
        enet_peer_reset (peer);
    else
    if (event != NULL)
    {
        event -> type = ENET_EVENT_TYPE_DISCONNECT;
        event -> peer = peer;
        event -> data = peer->eventData;  //modified by aprilliu, 2016.2.18 这样对于ENET_EVENT_TYPE_DISCONNECT事件，ENetEvent::data字段就是有效的了，也就是client传过来的帐号accId

        enet_peer_reset (peer);
    }
    else 
    {
        peer -> eventData = 0;

        enet_protocol_dispatch_state (host, peer, ENET_PEER_STATE_ZOMBIE);
    }
}

static void
enet_protocol_remove_sent_unreliable_commands (ENetPeer * peer)
{
    ENetOutgoingCommand * outgoingCommand;

    while (! enet_list_empty (& peer -> sentUnreliableCommands))
    {
        outgoingCommand = (ENetOutgoingCommand *) enet_list_front (& peer -> sentUnreliableCommands);
        
        enet_list_remove (& outgoingCommand -> outgoingCommandList);

        if (outgoingCommand -> packet != NULL)
        {
           -- outgoingCommand -> packet -> referenceCount;

           if (outgoingCommand -> packet -> referenceCount == 0)
           {
              outgoingCommand -> packet -> flags |= ENET_PACKET_FLAG_SENT;
 
              enet_packet_destroy (outgoingCommand -> packet);
           }
        }

        enet_free (outgoingCommand);
    }
}

static ENetProtocolCommand
enet_protocol_remove_sent_reliable_command (ENetPeer * peer, enet_uint16 reliableSequenceNumber, enet_uint8 channelID)
{
    ENetOutgoingCommand * outgoingCommand = NULL;
    ENetListIterator currentCommand;
    ENetProtocolCommand commandNumber;
    int wasSent = 1;

    for (currentCommand = enet_list_begin (& peer -> sentReliableCommands);
         currentCommand != enet_list_end (& peer -> sentReliableCommands);
         currentCommand = enet_list_next (currentCommand))
    {
       outgoingCommand = (ENetOutgoingCommand *) currentCommand;
        
       if (outgoingCommand -> reliableSequenceNumber == reliableSequenceNumber &&
           outgoingCommand -> command.header.channelID == channelID)
         break;
    }

    if (currentCommand == enet_list_end (& peer -> sentReliableCommands))
    {
       for (currentCommand = enet_list_begin (& peer -> outgoingReliableCommands);
            currentCommand != enet_list_end (& peer -> outgoingReliableCommands);
            currentCommand = enet_list_next (currentCommand))
       {
          outgoingCommand = (ENetOutgoingCommand *) currentCommand;

          if (outgoingCommand -> sendAttempts < 1) return ENET_PROTOCOL_COMMAND_NONE;

          if (outgoingCommand -> reliableSequenceNumber == reliableSequenceNumber &&
              outgoingCommand -> command.header.channelID == channelID)
            break;
       }

       if (currentCommand == enet_list_end (& peer -> outgoingReliableCommands))
         return ENET_PROTOCOL_COMMAND_NONE;

       wasSent = 0;
    }//end of if

    if (outgoingCommand == NULL)
      return ENET_PROTOCOL_COMMAND_NONE;

    if (channelID < peer -> channelCount)
    {
       ENetChannel * channel = & peer -> channels [channelID];
       enet_uint16 reliableWindow = reliableSequenceNumber / ENET_PEER_RELIABLE_WINDOW_SIZE;
       if (channel -> reliableWindows [reliableWindow] > 0)
       {
          -- channel -> reliableWindows [reliableWindow];
          if (! channel -> reliableWindows [reliableWindow])
            channel -> usedReliableWindows &= ~ (1 << reliableWindow);
       }
    }

    commandNumber = (ENetProtocolCommand) (outgoingCommand -> command.header.command & ENET_PROTOCOL_COMMAND_MASK);
    
    enet_list_remove (& outgoingCommand -> outgoingCommandList);

    if (outgoingCommand -> packet != NULL)
    {
       if (wasSent)
         peer -> reliableDataInTransit -= outgoingCommand -> fragmentLength;

       -- outgoingCommand -> packet -> referenceCount;

       if (outgoingCommand -> packet -> referenceCount == 0)
       {
          outgoingCommand -> packet -> flags |= ENET_PACKET_FLAG_SENT;

          enet_packet_destroy (outgoingCommand -> packet);
       }
    }

    enet_free (outgoingCommand);

    if (enet_list_empty (& peer -> sentReliableCommands))
      return commandNumber;
    
    outgoingCommand = (ENetOutgoingCommand *) enet_list_front (& peer -> sentReliableCommands);
    
    peer -> nextTimeout = outgoingCommand -> sentTime + outgoingCommand -> roundTripTimeout;

    return commandNumber;
} 
//处理ENET_PROTOCOL_COMMAND_CONNECT协议  新连接请求(from client)
static ENetPeer *
enet_protocol_handle_connect (ENetHost * host, ENetProtocolHeader * header, ENetProtocol * command)
{
    enet_uint8 incomingSessionID, outgoingSessionID;
    enet_uint32 mtu, windowSize;
    ENetChannel * channel;
    size_t channelCount, duplicatePeers = 0;
    ENetPeer * currentPeer, * peer = NULL;
    ENetProtocol verifyCommand;

    channelCount = ENET_NET_TO_HOST_32 (command -> connect.channelCount);

    if (channelCount < ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT ||
        channelCount > ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT)
      return NULL;
	
	//获取一个空闲CNetPeer来标识通讯的对端(client)  TODO:有改善空间，可以做到 O(1)的时间复杂度
    for (currentPeer = host -> peers;
         currentPeer < & host -> peers [host -> peerCount];
         ++ currentPeer)
    {
        if (currentPeer -> state == ENET_PEER_STATE_DISCONNECTED)
        {
            if (peer == NULL)
              peer = currentPeer;
        }
        else 
        if (currentPeer -> state != ENET_PEER_STATE_CONNECTING &&
            currentPeer -> address.host == host -> receivedAddress.host)
        {
            if (currentPeer -> address.port == host -> receivedAddress.port &&  //IP，PORT以及connectID都一致的 说明是重复链接
                currentPeer -> connectID == command -> connect.connectID)
              return NULL;

            ++ duplicatePeers;
        }
    }//end for 

    if (peer == NULL || duplicatePeers >= host -> duplicatePeers)
      return NULL;
	//设置client的相关信息（对于server来说一个 CNetPeer对应一个client）
    if (channelCount > host -> channelLimit)
      channelCount = host -> channelLimit;
    peer -> channels = (ENetChannel *) enet_malloc (channelCount * sizeof (ENetChannel));
    if (peer -> channels == NULL)
      return NULL;
    peer -> channelCount = channelCount;
    peer -> state = ENET_PEER_STATE_ACKNOWLEDGING_CONNECT;   //连接确认中（已收到连接请求)
    peer -> connectID = command -> connect.connectID;
    peer -> address = host -> receivedAddress;    //记录对端的地址信息
    peer -> outgoingPeerID = ENET_NET_TO_HOST_16 (command -> connect.outgoingPeerID); //记录对端(client)跟server通讯所使用的CNetPeer在client的CNetHost中peer数组的下标
    peer -> incomingBandwidth = ENET_NET_TO_HOST_32 (command -> connect.incomingBandwidth);
    peer -> outgoingBandwidth = ENET_NET_TO_HOST_32 (command -> connect.outgoingBandwidth);
    peer -> packetThrottleInterval = ENET_NET_TO_HOST_32 (command -> connect.packetThrottleInterval);
    peer -> packetThrottleAcceleration = ENET_NET_TO_HOST_32 (command -> connect.packetThrottleAcceleration);
    peer -> packetThrottleDeceleration = ENET_NET_TO_HOST_32 (command -> connect.packetThrottleDeceleration);
    peer -> eventData = ENET_NET_TO_HOST_32 (command -> connect.data);

    LOGD("peer(%p) new state(%d) userid(%u)\n",peer, peer->state, peer->eventData);

    incomingSessionID = command -> connect.incomingSessionID == 0xFF ? peer -> outgoingSessionID : command -> connect.incomingSessionID;
    incomingSessionID = (incomingSessionID + 1) & (ENET_PROTOCOL_HEADER_SESSION_MASK >> ENET_PROTOCOL_HEADER_SESSION_SHIFT);
    if (incomingSessionID == peer -> outgoingSessionID)
      incomingSessionID = (incomingSessionID + 1) & (ENET_PROTOCOL_HEADER_SESSION_MASK >> ENET_PROTOCOL_HEADER_SESSION_SHIFT);
    peer -> outgoingSessionID = incomingSessionID;

    outgoingSessionID = command -> connect.outgoingSessionID == 0xFF ? peer -> incomingSessionID : command -> connect.outgoingSessionID;
    outgoingSessionID = (outgoingSessionID + 1) & (ENET_PROTOCOL_HEADER_SESSION_MASK >> ENET_PROTOCOL_HEADER_SESSION_SHIFT);
    if (outgoingSessionID == peer -> incomingSessionID)
      outgoingSessionID = (outgoingSessionID + 1) & (ENET_PROTOCOL_HEADER_SESSION_MASK >> ENET_PROTOCOL_HEADER_SESSION_SHIFT);
    peer -> incomingSessionID = outgoingSessionID;

    for (channel = peer -> channels;
         channel < & peer -> channels [channelCount];
         ++ channel)
    {
        channel -> outgoingReliableSequenceNumber = 0;
        channel -> outgoingUnreliableSequenceNumber = 0;
        channel -> incomingReliableSequenceNumber = 0;
        channel -> incomingUnreliableSequenceNumber = 0;

        enet_list_clear (& channel -> incomingReliableCommands);
        enet_list_clear (& channel -> incomingUnreliableCommands);

        channel -> usedReliableWindows = 0;
        memset (channel -> reliableWindows, 0, sizeof (channel -> reliableWindows));
    }

    mtu = ENET_NET_TO_HOST_32 (command -> connect.mtu);

    if (mtu < ENET_PROTOCOL_MINIMUM_MTU)
      mtu = ENET_PROTOCOL_MINIMUM_MTU;
    else
    if (mtu > ENET_PROTOCOL_MAXIMUM_MTU)
      mtu = ENET_PROTOCOL_MAXIMUM_MTU;

    peer -> mtu = mtu;

    if (host -> outgoingBandwidth == 0 &&
        peer -> incomingBandwidth == 0)
      peer -> windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;
    else
    if (host -> outgoingBandwidth == 0 ||
        peer -> incomingBandwidth == 0)
      peer -> windowSize = (ENET_MAX (host -> outgoingBandwidth, peer -> incomingBandwidth) /
                                    ENET_PEER_WINDOW_SIZE_SCALE) *
                                      ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;
    else
      peer -> windowSize = (ENET_MIN (host -> outgoingBandwidth, peer -> incomingBandwidth) /
                                    ENET_PEER_WINDOW_SIZE_SCALE) * 
                                      ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;

    if (peer -> windowSize < ENET_PROTOCOL_MINIMUM_WINDOW_SIZE)
      peer -> windowSize = ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;
    else
    if (peer -> windowSize > ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE)
      peer -> windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;

    if (host -> incomingBandwidth == 0)
      windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;
    else
      windowSize = (host -> incomingBandwidth / ENET_PEER_WINDOW_SIZE_SCALE) *
                     ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;

    if (windowSize > ENET_NET_TO_HOST_32 (command -> connect.windowSize))
      windowSize = ENET_NET_TO_HOST_32 (command -> connect.windowSize);

    if (windowSize < ENET_PROTOCOL_MINIMUM_WINDOW_SIZE)
      windowSize = ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;
    else
    if (windowSize > ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE)
      windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;

	//发送连接请求接受确认包
    verifyCommand.header.command = ENET_PROTOCOL_COMMAND_VERIFY_CONNECT | ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE;
    verifyCommand.header.channelID = 0xFF;
    verifyCommand.verifyConnect.outgoingPeerID = ENET_HOST_TO_NET_16 (peer -> incomingPeerID); //把server端用于跟client通讯的CNetPeer的下标发送给client,
    verifyCommand.verifyConnect.incomingSessionID = incomingSessionID;
    verifyCommand.verifyConnect.outgoingSessionID = outgoingSessionID;
    verifyCommand.verifyConnect.mtu = ENET_HOST_TO_NET_32 (peer -> mtu);
    verifyCommand.verifyConnect.windowSize = ENET_HOST_TO_NET_32 (windowSize);
    verifyCommand.verifyConnect.channelCount = ENET_HOST_TO_NET_32 (channelCount);
    verifyCommand.verifyConnect.incomingBandwidth = ENET_HOST_TO_NET_32 (host -> incomingBandwidth);
    verifyCommand.verifyConnect.outgoingBandwidth = ENET_HOST_TO_NET_32 (host -> outgoingBandwidth);
    verifyCommand.verifyConnect.packetThrottleInterval = ENET_HOST_TO_NET_32 (peer -> packetThrottleInterval);
    verifyCommand.verifyConnect.packetThrottleAcceleration = ENET_HOST_TO_NET_32 (peer -> packetThrottleAcceleration);
    verifyCommand.verifyConnect.packetThrottleDeceleration = ENET_HOST_TO_NET_32 (peer -> packetThrottleDeceleration);
    verifyCommand.verifyConnect.connectID = peer -> connectID; //把请求连接端(client）生成的connectID再传回给client

    enet_peer_queue_outgoing_command (peer, & verifyCommand, NULL, 0, 0);

    return peer;
}

static int //处理ENET_PROTOCOL_COMMAND_SEND_RELIABLE 协议
enet_protocol_handle_send_reliable (ENetHost * host, ENetPeer * peer, const ENetProtocol * command, enet_uint8 ** currentData)
{
    size_t dataLength;

    if (command -> header.channelID >= peer -> channelCount ||
        (peer -> state != ENET_PEER_STATE_CONNECTED && peer -> state != ENET_PEER_STATE_DISCONNECT_LATER))
      return -1;

    dataLength = ENET_NET_TO_HOST_16 (command -> sendReliable.dataLength);
    * currentData += dataLength;
    if (dataLength > host -> maximumPacketSize ||
        * currentData < host -> receivedData ||
        * currentData > & host -> receivedData [host -> receivedDataLength])
      return -1;

    if (enet_peer_queue_incoming_command (peer, command, (const enet_uint8 *) command + sizeof (ENetProtocolSendReliable), dataLength, ENET_PACKET_FLAG_RELIABLE, 0) == NULL)
      return -1;

    return 0;
}

static int
enet_protocol_handle_send_unsequenced (ENetHost * host, ENetPeer * peer, const ENetProtocol * command, enet_uint8 ** currentData)
{
    enet_uint32 unsequencedGroup, index;
    size_t dataLength;

    if (command -> header.channelID >= peer -> channelCount ||
        (peer -> state != ENET_PEER_STATE_CONNECTED && peer -> state != ENET_PEER_STATE_DISCONNECT_LATER))
      return -1;

    dataLength = ENET_NET_TO_HOST_16 (command -> sendUnsequenced.dataLength);
    * currentData += dataLength;
    if (dataLength > host -> maximumPacketSize ||
        * currentData < host -> receivedData ||
        * currentData > & host -> receivedData [host -> receivedDataLength])
      return -1; 

    unsequencedGroup = ENET_NET_TO_HOST_16 (command -> sendUnsequenced.unsequencedGroup);
    index = unsequencedGroup % ENET_PEER_UNSEQUENCED_WINDOW_SIZE;
   
    if (unsequencedGroup < peer -> incomingUnsequencedGroup)
      unsequencedGroup += 0x10000;

    if (unsequencedGroup >= (enet_uint32) peer -> incomingUnsequencedGroup + ENET_PEER_FREE_UNSEQUENCED_WINDOWS * ENET_PEER_UNSEQUENCED_WINDOW_SIZE)
      return 0;

    unsequencedGroup &= 0xFFFF;

    if (unsequencedGroup - index != peer -> incomingUnsequencedGroup)
    {
        peer -> incomingUnsequencedGroup = unsequencedGroup - index;

        memset (peer -> unsequencedWindow, 0, sizeof (peer -> unsequencedWindow));
    }
    else
    if (peer -> unsequencedWindow [index / 32] & (1 << (index % 32)))
      return 0;
      
    if (enet_peer_queue_incoming_command (peer, command, (const enet_uint8 *) command + sizeof (ENetProtocolSendUnsequenced), dataLength, ENET_PACKET_FLAG_UNSEQUENCED, 0) == NULL)
      return -1;
   
    peer -> unsequencedWindow [index / 32] |= 1 << (index % 32);
 
    return 0;
}

static int
enet_protocol_handle_send_unreliable (ENetHost * host, ENetPeer * peer, const ENetProtocol * command, enet_uint8 ** currentData)
{
    size_t dataLength;

    if (command -> header.channelID >= peer -> channelCount ||
        (peer -> state != ENET_PEER_STATE_CONNECTED && peer -> state != ENET_PEER_STATE_DISCONNECT_LATER))
      return -1;

    dataLength = ENET_NET_TO_HOST_16 (command -> sendUnreliable.dataLength);
    * currentData += dataLength;
    if (dataLength > host -> maximumPacketSize ||
        * currentData < host -> receivedData ||
        * currentData > & host -> receivedData [host -> receivedDataLength])
      return -1;

    if (enet_peer_queue_incoming_command (peer, command, (const enet_uint8 *) command + sizeof (ENetProtocolSendUnreliable), dataLength, 0, 0) == NULL)
      return -1;

    return 0;
}

static int //处理收到的协议ENET_PROTOCOL_COMMAND_SEND_FRAGMENT 分片数据包. command :收到的fragment协议包， currentData：主要是根据本次协议的具体长度，指向下个协议，并传出给上层调用（做循环处理）
enet_protocol_handle_send_fragment (ENetHost * host, ENetPeer * peer, const ENetProtocol * command, enet_uint8 ** currentData)
{
    enet_uint32 fragmentNumber,
           fragmentCount,
           fragmentOffset,
           fragmentLength,
           startSequenceNumber,
           totalLength;
    ENetChannel * channel;
    enet_uint16 startWindow, currentWindow;
    ENetListIterator currentCommand;
    ENetIncomingCommand * startCommand = NULL;

    if (command -> header.channelID >= peer -> channelCount ||
        (peer -> state != ENET_PEER_STATE_CONNECTED && peer -> state != ENET_PEER_STATE_DISCONNECT_LATER))
      return -1;

    fragmentLength = ENET_NET_TO_HOST_16 (command -> sendFragment.dataLength);
    * currentData += fragmentLength; //根据具体数据分片的长度 步进到下个协议，传出后供上层调用使用
    if (fragmentLength > host -> maximumPacketSize ||
        * currentData < host -> receivedData ||
        * currentData > & host -> receivedData [host -> receivedDataLength])
      return -1;

    channel = & peer -> channels [command -> header.channelID];
    startSequenceNumber = ENET_NET_TO_HOST_16 (command -> sendFragment.startSequenceNumber);
    startWindow = startSequenceNumber / ENET_PEER_RELIABLE_WINDOW_SIZE;
    currentWindow = channel -> incomingReliableSequenceNumber / ENET_PEER_RELIABLE_WINDOW_SIZE;

    if (startSequenceNumber < channel -> incomingReliableSequenceNumber)
      startWindow += ENET_PEER_RELIABLE_WINDOWS;

    if (startWindow < currentWindow || startWindow >= currentWindow + ENET_PEER_FREE_RELIABLE_WINDOWS - 1)
      return 0;

    fragmentNumber = ENET_NET_TO_HOST_32 (command -> sendFragment.fragmentNumber);
    fragmentCount = ENET_NET_TO_HOST_32 (command -> sendFragment.fragmentCount);
    fragmentOffset = ENET_NET_TO_HOST_32 (command -> sendFragment.fragmentOffset);
    totalLength = ENET_NET_TO_HOST_32 (command -> sendFragment.totalLength);
    
    if (fragmentCount > ENET_PROTOCOL_MAXIMUM_FRAGMENT_COUNT ||
        fragmentNumber >= fragmentCount ||
        totalLength > host -> maximumPacketSize ||
        fragmentOffset >= totalLength ||
        fragmentLength > totalLength - fragmentOffset)
      return -1;
 
    for (currentCommand = enet_list_previous (enet_list_end (& channel -> incomingReliableCommands));
         currentCommand != enet_list_end (& channel -> incomingReliableCommands);
         currentCommand = enet_list_previous (currentCommand))
    {
       ENetIncomingCommand * incomingCommand = (ENetIncomingCommand *) currentCommand;

       if (startSequenceNumber >= channel -> incomingReliableSequenceNumber)
       {
          if (incomingCommand -> reliableSequenceNumber < channel -> incomingReliableSequenceNumber)
            continue;
       }
       else
       if (incomingCommand -> reliableSequenceNumber >= channel -> incomingReliableSequenceNumber)
         break;

       if (incomingCommand -> reliableSequenceNumber <= startSequenceNumber)
       {
          if (incomingCommand -> reliableSequenceNumber < startSequenceNumber)
            break;
        
          if ((incomingCommand -> command.header.command & ENET_PROTOCOL_COMMAND_MASK) != ENET_PROTOCOL_COMMAND_SEND_FRAGMENT ||
              totalLength != incomingCommand -> packet -> dataLength ||
              fragmentCount != incomingCommand -> fragmentCount)
            return -1;

          startCommand = incomingCommand;
          break;
       }
    }//end of for
 
    if (startCommand == NULL) //如果是首次收到该协议包的分片（也就是还没有为该协议创建一个ENetIncomingCommand），则需要首先创建一个 插入到channel的incomingReliableCommands或incomingUnReliableCommands队列的ENetIncomingCommand
    {
       ENetProtocol hostCommand = * command;

       hostCommand.header.reliableSequenceNumber = startSequenceNumber;

       startCommand = enet_peer_queue_incoming_command (peer, & hostCommand, NULL, totalLength, ENET_PACKET_FLAG_RELIABLE, fragmentCount);
       if (startCommand == NULL)
         return -1;
    }
    
    if ((startCommand -> fragments [fragmentNumber / 32] & (1 << (fragmentNumber % 32))) == 0) //如果以前没有收到过该分片
    {
       -- startCommand -> fragmentsRemaining;  //已收到一个分片，则更新待接收分片数量

       startCommand -> fragments [fragmentNumber / 32] |= (1 << (fragmentNumber % 32)); //把相应分片标识位置位

       if (fragmentOffset + fragmentLength > startCommand -> packet -> dataLength)
         fragmentLength = startCommand -> packet -> dataLength - fragmentOffset;

	   //把分片数据拷贝到ENetIncomingCommand::packet::data区域中，组装分片数据
       memcpy (startCommand -> packet -> data + fragmentOffset,
               (enet_uint8 *) command + sizeof (ENetProtocolSendFragment),
               fragmentLength);

        if (startCommand -> fragmentsRemaining <= 0) //若已经收到所有分片数据
          enet_peer_dispatch_incoming_reliable_commands (peer, channel);
    }

    return 0;
}

static int
enet_protocol_handle_send_unreliable_fragment (ENetHost * host, ENetPeer * peer, const ENetProtocol * command, enet_uint8 ** currentData)
{
    enet_uint32 fragmentNumber,
           fragmentCount,
           fragmentOffset,
           fragmentLength,
           reliableSequenceNumber,
           startSequenceNumber,
           totalLength;
    enet_uint16 reliableWindow, currentWindow;
    ENetChannel * channel;
    ENetListIterator currentCommand;
    ENetIncomingCommand * startCommand = NULL;

    if (command -> header.channelID >= peer -> channelCount ||
        (peer -> state != ENET_PEER_STATE_CONNECTED && peer -> state != ENET_PEER_STATE_DISCONNECT_LATER))
      return -1;

    fragmentLength = ENET_NET_TO_HOST_16 (command -> sendFragment.dataLength);
    * currentData += fragmentLength;
    if (fragmentLength > host -> maximumPacketSize ||
        * currentData < host -> receivedData ||
        * currentData > & host -> receivedData [host -> receivedDataLength])
      return -1;

    channel = & peer -> channels [command -> header.channelID];
    reliableSequenceNumber = command -> header.reliableSequenceNumber;
    startSequenceNumber = ENET_NET_TO_HOST_16 (command -> sendFragment.startSequenceNumber);

    reliableWindow = reliableSequenceNumber / ENET_PEER_RELIABLE_WINDOW_SIZE;
    currentWindow = channel -> incomingReliableSequenceNumber / ENET_PEER_RELIABLE_WINDOW_SIZE;

    if (reliableSequenceNumber < channel -> incomingReliableSequenceNumber)
      reliableWindow += ENET_PEER_RELIABLE_WINDOWS;

    if (reliableWindow < currentWindow || reliableWindow >= currentWindow + ENET_PEER_FREE_RELIABLE_WINDOWS - 1)
      return 0;

    if (reliableSequenceNumber == channel -> incomingReliableSequenceNumber &&
        startSequenceNumber <= channel -> incomingUnreliableSequenceNumber)
      return 0;

    fragmentNumber = ENET_NET_TO_HOST_32 (command -> sendFragment.fragmentNumber);
    fragmentCount = ENET_NET_TO_HOST_32 (command -> sendFragment.fragmentCount);
    fragmentOffset = ENET_NET_TO_HOST_32 (command -> sendFragment.fragmentOffset);
    totalLength = ENET_NET_TO_HOST_32 (command -> sendFragment.totalLength);

    if (fragmentCount > ENET_PROTOCOL_MAXIMUM_FRAGMENT_COUNT ||
        fragmentNumber >= fragmentCount ||
        totalLength > host -> maximumPacketSize ||
        fragmentOffset >= totalLength ||
        fragmentLength > totalLength - fragmentOffset)
      return -1;

    for (currentCommand = enet_list_previous (enet_list_end (& channel -> incomingUnreliableCommands));
         currentCommand != enet_list_end (& channel -> incomingUnreliableCommands);
         currentCommand = enet_list_previous (currentCommand))
    {
       ENetIncomingCommand * incomingCommand = (ENetIncomingCommand *) currentCommand;

       if (reliableSequenceNumber >= channel -> incomingReliableSequenceNumber)
       {
          if (incomingCommand -> reliableSequenceNumber < channel -> incomingReliableSequenceNumber)
            continue;
       }
       else
       if (incomingCommand -> reliableSequenceNumber >= channel -> incomingReliableSequenceNumber)
         break;

       if (incomingCommand -> reliableSequenceNumber < reliableSequenceNumber)
         break;

       if (incomingCommand -> reliableSequenceNumber > reliableSequenceNumber)
         continue;

       if (incomingCommand -> unreliableSequenceNumber <= startSequenceNumber)
       {
          if (incomingCommand -> unreliableSequenceNumber < startSequenceNumber)
            break;

          if ((incomingCommand -> command.header.command & ENET_PROTOCOL_COMMAND_MASK) != ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE_FRAGMENT ||
              totalLength != incomingCommand -> packet -> dataLength ||
              fragmentCount != incomingCommand -> fragmentCount)
            return -1;

          startCommand = incomingCommand;
          break;
       }
    }

    if (startCommand == NULL)
    {
       startCommand = enet_peer_queue_incoming_command (peer, command, NULL, totalLength, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT, fragmentCount);
       if (startCommand == NULL)
         return -1;
    }

    if ((startCommand -> fragments [fragmentNumber / 32] & (1 << (fragmentNumber % 32))) == 0)
    {
       -- startCommand -> fragmentsRemaining;

       startCommand -> fragments [fragmentNumber / 32] |= (1 << (fragmentNumber % 32));

       if (fragmentOffset + fragmentLength > startCommand -> packet -> dataLength)
         fragmentLength = startCommand -> packet -> dataLength - fragmentOffset;

       memcpy (startCommand -> packet -> data + fragmentOffset,
               (enet_uint8 *) command + sizeof (ENetProtocolSendFragment),
               fragmentLength);

        if (startCommand -> fragmentsRemaining <= 0)
          enet_peer_dispatch_incoming_unreliable_commands (peer, channel);
    }

    return 0;
}

static int
enet_protocol_handle_ping (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
    if (peer -> state != ENET_PEER_STATE_CONNECTED && peer -> state != ENET_PEER_STATE_DISCONNECT_LATER)
      return -1;

    return 0;
}

static int
enet_protocol_handle_bandwidth_limit (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
    if (peer -> state != ENET_PEER_STATE_CONNECTED && peer -> state != ENET_PEER_STATE_DISCONNECT_LATER)
      return -1;

    if (peer -> incomingBandwidth != 0)
      -- host -> bandwidthLimitedPeers;

    peer -> incomingBandwidth = ENET_NET_TO_HOST_32 (command -> bandwidthLimit.incomingBandwidth);
    peer -> outgoingBandwidth = ENET_NET_TO_HOST_32 (command -> bandwidthLimit.outgoingBandwidth);

    if (peer -> incomingBandwidth != 0)
      ++ host -> bandwidthLimitedPeers;

    if (peer -> incomingBandwidth == 0 && host -> outgoingBandwidth == 0)
      peer -> windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;
    else
    if (peer -> incomingBandwidth == 0 || host -> outgoingBandwidth == 0)
      peer -> windowSize = (ENET_MAX (peer -> incomingBandwidth, host -> outgoingBandwidth) /
                             ENET_PEER_WINDOW_SIZE_SCALE) * ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;
    else
      peer -> windowSize = (ENET_MIN (peer -> incomingBandwidth, host -> outgoingBandwidth) /
                             ENET_PEER_WINDOW_SIZE_SCALE) * ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;

    if (peer -> windowSize < ENET_PROTOCOL_MINIMUM_WINDOW_SIZE)
      peer -> windowSize = ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;
    else
    if (peer -> windowSize > ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE)
      peer -> windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;

    return 0;
}

static int
enet_protocol_handle_throttle_configure (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
    if (peer -> state != ENET_PEER_STATE_CONNECTED && peer -> state != ENET_PEER_STATE_DISCONNECT_LATER)
      return -1;

    peer -> packetThrottleInterval = ENET_NET_TO_HOST_32 (command -> throttleConfigure.packetThrottleInterval);
    peer -> packetThrottleAcceleration = ENET_NET_TO_HOST_32 (command -> throttleConfigure.packetThrottleAcceleration);
    peer -> packetThrottleDeceleration = ENET_NET_TO_HOST_32 (command -> throttleConfigure.packetThrottleDeceleration);

    return 0;
}

static int //处理ENET_PROTOCOL_COMMAND_DISCONNECT消息 
enet_protocol_handle_disconnect (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
    if (peer -> state == ENET_PEER_STATE_DISCONNECTED || peer -> state == ENET_PEER_STATE_ZOMBIE || peer -> state == ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT)
      return 0;

    enet_peer_reset_queues (peer);

    if (peer -> state == ENET_PEER_STATE_CONNECTION_SUCCEEDED || peer -> state == ENET_PEER_STATE_DISCONNECTING || peer -> state == ENET_PEER_STATE_CONNECTING)
        enet_protocol_dispatch_state (host, peer, ENET_PEER_STATE_ZOMBIE);
    else
    if (peer -> state != ENET_PEER_STATE_CONNECTED && peer -> state != ENET_PEER_STATE_DISCONNECT_LATER)
    {
        if (peer -> state == ENET_PEER_STATE_CONNECTION_PENDING) host -> recalculateBandwidthLimits = 1;

        enet_peer_reset (peer);
    }
    else
    if (command -> header.command & ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE) //收到需要确认的 断开连接请求
      enet_protocol_change_state (host, peer, ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT);
    else
      enet_protocol_dispatch_state (host, peer, ENET_PEER_STATE_ZOMBIE);

    if (peer -> state != ENET_PEER_STATE_DISCONNECTED)
      peer -> eventData = ENET_NET_TO_HOST_32 (command -> disconnect.data);

    return 0;
}

static int
enet_protocol_handle_acknowledge (ENetHost * host, ENetEvent * event, ENetPeer * peer, const ENetProtocol * command)
{
    enet_uint32 roundTripTime,
           receivedSentTime,
           receivedReliableSequenceNumber;
    ENetProtocolCommand commandNumber;

    if (peer -> state == ENET_PEER_STATE_DISCONNECTED || peer -> state == ENET_PEER_STATE_ZOMBIE)
      return 0;

    receivedSentTime = ENET_NET_TO_HOST_16 (command -> acknowledge.receivedSentTime);
    receivedSentTime |= host -> serviceTime & 0xFFFF0000;                                      //取当前时间的高2Byte 拼接成4Byte的时间（网络通讯时只使用了系统时间的低2Byte，节省流量)
    if ((receivedSentTime & 0x8000) > (host -> serviceTime & 0x8000))
        receivedSentTime -= 0x10000;

    if (ENET_TIME_LESS (host -> serviceTime, receivedSentTime))
      return 0;

    peer -> lastReceiveTime = host -> serviceTime;
    peer -> earliestTimeout = 0;

    roundTripTime = ENET_TIME_DIFFERENCE (host -> serviceTime, receivedSentTime);   //此包经历的网络延迟RTT

    enet_peer_throttle (peer, roundTripTime);

    peer -> roundTripTimeVariance -= peer -> roundTripTimeVariance / 4;

    if (roundTripTime >= peer -> roundTripTime) //根据此包的RTT修正 该对端的RTT
    {
       peer -> roundTripTime += (roundTripTime - peer -> roundTripTime) / 8;
       peer -> roundTripTimeVariance += (roundTripTime - peer -> roundTripTime) / 4;
    }
    else
    {
       peer -> roundTripTime -= (peer -> roundTripTime - roundTripTime) / 8;
       peer -> roundTripTimeVariance += (peer -> roundTripTime - roundTripTime) / 4;
    }

    if (peer -> roundTripTime < peer -> lowestRoundTripTime)
      peer -> lowestRoundTripTime = peer -> roundTripTime;

    if (peer -> roundTripTimeVariance > peer -> highestRoundTripTimeVariance) 
      peer -> highestRoundTripTimeVariance = peer -> roundTripTimeVariance;

    if (peer -> packetThrottleEpoch == 0 ||
        ENET_TIME_DIFFERENCE (host -> serviceTime, peer -> packetThrottleEpoch) >= peer -> packetThrottleInterval)
    {
        peer -> lastRoundTripTime = peer -> lowestRoundTripTime;
        peer -> lastRoundTripTimeVariance = peer -> highestRoundTripTimeVariance;
        peer -> lowestRoundTripTime = peer -> roundTripTime;
        peer -> highestRoundTripTimeVariance = peer -> roundTripTimeVariance;
        peer -> packetThrottleEpoch = host -> serviceTime;
    }

    receivedReliableSequenceNumber = ENET_NET_TO_HOST_16 (command -> acknowledge.receivedReliableSequenceNumber);

    commandNumber = enet_protocol_remove_sent_reliable_command (peer, receivedReliableSequenceNumber, command -> header.channelID);

    switch (peer -> state)
    {
    case ENET_PEER_STATE_ACKNOWLEDGING_CONNECT:
       if (commandNumber != ENET_PROTOCOL_COMMAND_VERIFY_CONNECT)
         return -1;

       enet_protocol_notify_connect (host, peer, event);
       break;

    case ENET_PEER_STATE_DISCONNECTING:
       if (commandNumber != ENET_PROTOCOL_COMMAND_DISCONNECT)
         return -1;

       enet_protocol_notify_disconnect (host, peer, event);
       break;

    case ENET_PEER_STATE_DISCONNECT_LATER:
       if (enet_list_empty (& peer -> outgoingReliableCommands) &&
           enet_list_empty (& peer -> outgoingUnreliableCommands) &&   
           enet_list_empty (& peer -> sentReliableCommands))
         enet_peer_disconnect (peer, peer -> eventData);
       break;

    default:
       break;
    }
   
    return 0;
}

static int   //Client端 处理ENET_PROTOCOL_COMMAND_VERIFY_CONNECT消息
enet_protocol_handle_verify_connect (ENetHost * host, ENetEvent * event, ENetPeer * peer, const ENetProtocol * command)
{
    enet_uint32 mtu, windowSize;
    size_t channelCount;

    if (peer -> state != ENET_PEER_STATE_CONNECTING)
      return 0;

    channelCount = ENET_NET_TO_HOST_32 (command -> verifyConnect.channelCount);

    if (channelCount < ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT || channelCount > ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT ||
        ENET_NET_TO_HOST_32 (command -> verifyConnect.packetThrottleInterval) != peer -> packetThrottleInterval ||
        ENET_NET_TO_HOST_32 (command -> verifyConnect.packetThrottleAcceleration) != peer -> packetThrottleAcceleration ||
        ENET_NET_TO_HOST_32 (command -> verifyConnect.packetThrottleDeceleration) != peer -> packetThrottleDeceleration ||
        command -> verifyConnect.connectID != peer -> connectID)
    {
        peer -> eventData = 0;

        enet_protocol_dispatch_state (host, peer, ENET_PEER_STATE_ZOMBIE);

        return -1;
    }

    enet_protocol_remove_sent_reliable_command (peer, 1, 0xFF);
    
    if (channelCount < peer -> channelCount)
      peer -> channelCount = channelCount;

    peer -> outgoingPeerID = ENET_NET_TO_HOST_16 (command -> verifyConnect.outgoingPeerID);
    peer -> incomingSessionID = command -> verifyConnect.incomingSessionID;
    peer -> outgoingSessionID = command -> verifyConnect.outgoingSessionID;

    mtu = ENET_NET_TO_HOST_32 (command -> verifyConnect.mtu);

    if (mtu < ENET_PROTOCOL_MINIMUM_MTU)
      mtu = ENET_PROTOCOL_MINIMUM_MTU;
    else 
    if (mtu > ENET_PROTOCOL_MAXIMUM_MTU)
      mtu = ENET_PROTOCOL_MAXIMUM_MTU;

    if (mtu < peer -> mtu)
      peer -> mtu = mtu;

    windowSize = ENET_NET_TO_HOST_32 (command -> verifyConnect.windowSize);

    if (windowSize < ENET_PROTOCOL_MINIMUM_WINDOW_SIZE)
      windowSize = ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;

    if (windowSize > ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE)
      windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;

    if (windowSize < peer -> windowSize)
      peer -> windowSize = windowSize;

    peer -> incomingBandwidth = ENET_NET_TO_HOST_32 (command -> verifyConnect.incomingBandwidth);
    peer -> outgoingBandwidth = ENET_NET_TO_HOST_32 (command -> verifyConnect.outgoingBandwidth);

    enet_protocol_notify_connect (host, peer, event);
    return 0;
}

static int
enet_protocol_handle_incoming_commands (ENetHost * host, ENetEvent * event)
{
    ENetProtocolHeader * header;
    ENetProtocol * command;
    ENetPeer * peer;
    enet_uint8 * currentData;
    size_t headerSize;
    enet_uint16 peerID, flags;
    enet_uint8 sessionID;

    if (host -> receivedDataLength < (size_t) & ((ENetProtocolHeader *) 0) -> sentTime)
      return 0;

    header = (ENetProtocolHeader *) host -> receivedData;   //ENetHost::receivedData实际指向ENetHost::packetData [0]  见enet_protocol_receive_incoming_commands

	//解包ENetProtocolHeader
    peerID = ENET_NET_TO_HOST_16 (header -> peerID);  //先解码（此函数把解码和处理逻辑揉合在一起了，不提倡）
    sessionID = (peerID & ENET_PROTOCOL_HEADER_SESSION_MASK) >> ENET_PROTOCOL_HEADER_SESSION_SHIFT;  //取peer id的第12-13bit  = sessionID
    flags = peerID & ENET_PROTOCOL_HEADER_FLAG_MASK; //获取第14，15bit的标识位（压缩标识以及senttime标识）
    peerID &= ~ (ENET_PROTOCOL_HEADER_FLAG_MASK | ENET_PROTOCOL_HEADER_SESSION_MASK);  //取低位12bit  就是真正的peer id

    headerSize = (flags & ENET_PROTOCOL_HEADER_FLAG_SENT_TIME ? sizeof (ENetProtocolHeader) : (size_t) & ((ENetProtocolHeader *) 0) -> sentTime);
    if (host -> checksum != NULL)
      headerSize += sizeof (enet_uint32);

    if (peerID == ENET_PROTOCOL_MAXIMUM_PEER_ID)  //对应（client的）新连接请求(ENET_PROTOCOL_COMMAND_CONNECT,协议包中的peer id传过来的是ENET_PROTOCOL_MAXIMUM_PEER_ID)
      peer = NULL;
    else
    if (peerID >= host -> peerCount)
      return 0;
    else
    {
       peer = & host -> peers [peerID];

       if (peer -> state == ENET_PEER_STATE_DISCONNECTED ||
           peer -> state == ENET_PEER_STATE_ZOMBIE ||
           ((host -> receivedAddress.host != peer -> address.host ||
             host -> receivedAddress.port != peer -> address.port) &&
             peer -> address.host != ENET_HOST_BROADCAST) ||
           (peer -> outgoingPeerID < ENET_PROTOCOL_MAXIMUM_PEER_ID &&
            sessionID != peer -> incomingSessionID))
         return 0;
    }
 
    if (flags & ENET_PROTOCOL_HEADER_FLAG_COMPRESSED) //若已压缩，则解压
    {
        size_t originalSize;
        if (host -> compressor.context == NULL || host -> compressor.decompress == NULL)
          return 0;

        originalSize = host -> compressor.decompress (host -> compressor.context,
                                    host -> receivedData + headerSize, 
                                    host -> receivedDataLength - headerSize, 
                                    host -> packetData [1] + headerSize, 
                                    sizeof (host -> packetData [1]) - headerSize);
        if (originalSize <= 0 || originalSize > sizeof (host -> packetData [1]) - headerSize)
          return 0;

        memcpy (host -> packetData [1], header, headerSize);
        host -> receivedData = host -> packetData [1];
        host -> receivedDataLength = headerSize + originalSize;
    }

    if (host -> checksum != NULL)
    {
        enet_uint32 * checksum = (enet_uint32 *) & host -> receivedData [headerSize - sizeof (enet_uint32)],
                    desiredChecksum = * checksum;
        ENetBuffer buffer;

        * checksum = peer != NULL ? peer -> connectID : 0;

        buffer.data = host -> receivedData;
        buffer.dataLength = host -> receivedDataLength;

        if (host -> checksum (& buffer, 1) != desiredChecksum)
          return 0;
    }
       
    if (peer != NULL)
    {
       peer -> address.host = host -> receivedAddress.host;
       peer -> address.port = host -> receivedAddress.port;
       peer -> incomingDataTotal += host -> receivedDataLength;
    }
    
    currentData = host -> receivedData + headerSize; //指向除了ENetProtocolHeader后的数据
  
	//收到的数据可能包含多个command消息（同一个对端发来的, 但都含同一个ENetProtocolHeader)
    while (currentData < & host -> receivedData [host -> receivedDataLength]) 
    {
       enet_uint8 commandNumber;
       size_t commandSize;

       command = (ENetProtocol *) currentData;

       if (currentData + sizeof (ENetProtocolCommandHeader) > & host -> receivedData [host -> receivedDataLength])
         break;

       commandNumber = command -> header.command & ENET_PROTOCOL_COMMAND_MASK;  //取command id , 具体就是低4位 。 command id只有一个字节 无需解码
       if (commandNumber >= ENET_PROTOCOL_COMMAND_COUNT) 
         break;
       
       commandSize = commandSizes [commandNumber];  //得到command包的大小，由于每个command结构是定长的（没有变长字段)所以可以根据command id就可以知道相应的包长度。
													//需要特别注意的是：有些command除了固定长度的结构外，还有变长的后续数据，例如：ENET_PROTOCOL_COMMAND_SEND_RELIABLE等command
       if (commandSize == 0 || currentData + commandSize > & host -> receivedData [host -> receivedDataLength])
         break;

       currentData += commandSize; //跳过已解码的command的固定长度信息（注意：command还可能有变长的后续数据信息）

       if (peer == NULL && commandNumber != ENET_PROTOCOL_COMMAND_CONNECT) //如果不是新到来的连接请求，并且peer没有分配，说明有错误（只有新的连接请求，对端对应的peer才会是未指定的)
         break;
         
       command -> header.reliableSequenceNumber = ENET_NET_TO_HOST_16 (command -> header.reliableSequenceNumber); //先解码

       switch (commandNumber)
       {
       case ENET_PROTOCOL_COMMAND_ACKNOWLEDGE:
          if (enet_protocol_handle_acknowledge (host, event, peer, command))
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_CONNECT:
          if (peer != NULL)  //新连接请求，peer必须为未指定的
            goto commandError;
          peer = enet_protocol_handle_connect (host, header, command);
          if (peer == NULL)
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_VERIFY_CONNECT:
          if (enet_protocol_handle_verify_connect (host, event, peer, command))
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_DISCONNECT:
          if (enet_protocol_handle_disconnect (host, peer, command))
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_PING:
          if (enet_protocol_handle_ping (host, peer, command))
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_SEND_RELIABLE:
          if (enet_protocol_handle_send_reliable (host, peer, command, & currentData))
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE:
          if (enet_protocol_handle_send_unreliable (host, peer, command, & currentData))
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED:
          if (enet_protocol_handle_send_unsequenced (host, peer, command, & currentData))
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_SEND_FRAGMENT:
          if (enet_protocol_handle_send_fragment (host, peer, command, & currentData))
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_BANDWIDTH_LIMIT:
          if (enet_protocol_handle_bandwidth_limit (host, peer, command))
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_THROTTLE_CONFIGURE:
          if (enet_protocol_handle_throttle_configure (host, peer, command))
            goto commandError;
          break;

       case ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE_FRAGMENT:
          if (enet_protocol_handle_send_unreliable_fragment (host, peer, command, & currentData))
            goto commandError;
          break;

       default:
          goto commandError;
       }//end of switch

       if (peer != NULL &&
           (command -> header.command & ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE) != 0)  //如果该command需要发送确认消息给对端，则调用enet_peer_queue_acknowledgement生成确认消息，插入到ENetPeer::acknowledgements中
       {
           enet_uint16 sentTime;

           if (! (flags & ENET_PROTOCOL_HEADER_FLAG_SENT_TIME))
             break;

           sentTime = ENET_NET_TO_HOST_16 (header -> sentTime);

           switch (peer -> state)
           {
           case ENET_PEER_STATE_DISCONNECTING:
           case ENET_PEER_STATE_ACKNOWLEDGING_CONNECT:
           case ENET_PEER_STATE_DISCONNECTED:
           case ENET_PEER_STATE_ZOMBIE:
              break;

           case ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT:
              if ((command -> header.command & ENET_PROTOCOL_COMMAND_MASK) == ENET_PROTOCOL_COMMAND_DISCONNECT)
                enet_peer_queue_acknowledgement (peer, command, sentTime);
              break;

           default:   
              enet_peer_queue_acknowledgement (peer, command, sentTime);        
              break;
           }
       }
    }//end of  while

commandError:
    if (event != NULL && event -> type != ENET_EVENT_TYPE_NONE)
      return 1;

    return 0;
}
 
static int
enet_protocol_receive_incoming_commands (ENetHost * host, ENetEvent * event)
{
    int packets;

    for (packets = 0; packets < 256; ++ packets)
    {
       int receivedLength;
       ENetBuffer buffer;

       buffer.data = host -> packetData [0];
       buffer.dataLength = sizeof (host -> packetData [0]);

       receivedLength = enet_socket_receive (host -> socket,
                                             & host -> receivedAddress,
                                             & buffer,
                                             1);

       if (receivedLength < 0) //如果接收数据错误
         return -1;

       if (receivedLength == 0)
         return 0;

       host -> receivedData = host -> packetData [0];
       host -> receivedDataLength = receivedLength;
      
       host -> totalReceivedData += receivedLength;
       host -> totalReceivedPackets ++;

       if (host -> intercept != NULL)
       {
          switch (host -> intercept (host, event))
          {
          case 1:
             if (event != NULL && event -> type != ENET_EVENT_TYPE_NONE)
               return 1;

             continue;
          
          case -1:
            LOGV("in enet_protocol_receive_incoming_commands,  intercept process error \n");
            return -1;
        
          default:
             break;
          }
       }
        
       switch (enet_protocol_handle_incoming_commands (host, event))
       {
       case 1:
          return 1;
       
       case -1:
            LOGV("in enet_protocol_receive_incoming_commands,  enet_protocol_handle_incoming_commands error \n");
            return -1;
       default:
          break;
       }
    } //end for

    return -1;
}

static void  //发送peer的待发送确认消息（实际处理就是把消息放入ENetHost::commands, buffers中）
enet_protocol_send_acknowledgements (ENetHost * host, ENetPeer * peer)
{
    ENetProtocol * command = & host -> commands [host -> commandCount];
    ENetBuffer * buffer = & host -> buffers [host -> bufferCount];
    ENetAcknowledgement * acknowledgement;
    ENetListIterator currentAcknowledgement;
    enet_uint16 reliableSequenceNumber;
 
    currentAcknowledgement = enet_list_begin (& peer -> acknowledgements);
         
    while (currentAcknowledgement != enet_list_end (& peer -> acknowledgements))
    {
       if (command >= & host -> commands [sizeof (host -> commands) / sizeof (ENetProtocol)] ||
           buffer >= & host -> buffers [sizeof (host -> buffers) / sizeof (ENetBuffer)] ||
           peer -> mtu - host -> packetSize < sizeof (ENetProtocolAcknowledge))
       {
          host -> continueSending = 1;

          break;
       }

       acknowledgement = (ENetAcknowledgement *) currentAcknowledgement;
 
       currentAcknowledgement = enet_list_next (currentAcknowledgement);

       buffer -> data = command;
       buffer -> dataLength = sizeof (ENetProtocolAcknowledge);

       host -> packetSize += buffer -> dataLength;

       reliableSequenceNumber = ENET_HOST_TO_NET_16 (acknowledgement -> command.header.reliableSequenceNumber);
  
       command -> header.command = ENET_PROTOCOL_COMMAND_ACKNOWLEDGE;
       command -> header.channelID = acknowledgement -> command.header.channelID;
       command -> header.reliableSequenceNumber = reliableSequenceNumber;
       command -> acknowledge.receivedReliableSequenceNumber = reliableSequenceNumber;
       command -> acknowledge.receivedSentTime = ENET_HOST_TO_NET_16 (acknowledgement -> sentTime);
  
	   //若确认消息是对断开连接的消息进行确认的话，则更新相应peer的状态为ENET_PEER_STATE_ZOMBIE
       if ((acknowledgement -> command.header.command & ENET_PROTOCOL_COMMAND_MASK) == ENET_PROTOCOL_COMMAND_DISCONNECT)
         enet_protocol_dispatch_state (host, peer, ENET_PEER_STATE_ZOMBIE);

       enet_list_remove (& acknowledgement -> acknowledgementList);
       enet_free (acknowledgement);

       ++ command;
       ++ buffer;
    }

    host -> commandCount = command - host -> commands;
    host -> bufferCount = buffer - host -> buffers;
}

static void
enet_protocol_send_unreliable_outgoing_commands (ENetHost * host, ENetPeer * peer)
{
    ENetProtocol * command = & host -> commands [host -> commandCount];
    ENetBuffer * buffer = & host -> buffers [host -> bufferCount];
    ENetOutgoingCommand * outgoingCommand;
    ENetListIterator currentCommand;

    currentCommand = enet_list_begin (& peer -> outgoingUnreliableCommands);
    
    while (currentCommand != enet_list_end (& peer -> outgoingUnreliableCommands))
    {
       size_t commandSize;

       outgoingCommand = (ENetOutgoingCommand *) currentCommand;
       commandSize = commandSizes [outgoingCommand -> command.header.command & ENET_PROTOCOL_COMMAND_MASK];

       if (command >= & host -> commands [sizeof (host -> commands) / sizeof (ENetProtocol)] ||
           buffer + 1 >= & host -> buffers [sizeof (host -> buffers) / sizeof (ENetBuffer)] ||
           peer -> mtu - host -> packetSize < commandSize ||
           (outgoingCommand -> packet != NULL &&
             peer -> mtu - host -> packetSize < commandSize + outgoingCommand -> fragmentLength))
       {
          host -> continueSending = 1;

          break;
       }

       currentCommand = enet_list_next (currentCommand);

       if (outgoingCommand -> packet != NULL && outgoingCommand -> fragmentOffset == 0)
       {
          peer -> packetThrottleCounter += ENET_PEER_PACKET_THROTTLE_COUNTER;
          peer -> packetThrottleCounter %= ENET_PEER_PACKET_THROTTLE_SCALE;
          
          if (peer -> packetThrottleCounter > peer -> packetThrottle)
          {
             enet_uint16 reliableSequenceNumber = outgoingCommand -> reliableSequenceNumber,
                         unreliableSequenceNumber = outgoingCommand -> unreliableSequenceNumber;
             for (;;)
             {
                -- outgoingCommand -> packet -> referenceCount;

                if (outgoingCommand -> packet -> referenceCount == 0)
                  enet_packet_destroy (outgoingCommand -> packet);
         
                enet_list_remove (& outgoingCommand -> outgoingCommandList);
                enet_free (outgoingCommand);

                if (currentCommand == enet_list_end (& peer -> outgoingUnreliableCommands))
                  break;

                outgoingCommand = (ENetOutgoingCommand *) currentCommand;
                if (outgoingCommand -> reliableSequenceNumber != reliableSequenceNumber ||
                    outgoingCommand -> unreliableSequenceNumber != unreliableSequenceNumber)
                  break;

                currentCommand = enet_list_next (currentCommand);
             }
           
             continue;
          }
       }

       buffer -> data = command;
       buffer -> dataLength = commandSize;
      
       host -> packetSize += buffer -> dataLength;

       * command = outgoingCommand -> command;
       
       enet_list_remove (& outgoingCommand -> outgoingCommandList);

       if (outgoingCommand -> packet != NULL)
       {
          ++ buffer;
          
          buffer -> data = outgoingCommand -> packet -> data + outgoingCommand -> fragmentOffset;
          buffer -> dataLength = outgoingCommand -> fragmentLength;

          host -> packetSize += buffer -> dataLength;

          enet_list_insert (enet_list_end (& peer -> sentUnreliableCommands), outgoingCommand);
       }
       else
         enet_free (outgoingCommand);

       ++ command;
       ++ buffer;
    } 

    host -> commandCount = command - host -> commands;
    host -> bufferCount = buffer - host -> buffers;

    if (peer -> state == ENET_PEER_STATE_DISCONNECT_LATER && 
        enet_list_empty (& peer -> outgoingReliableCommands) &&
        enet_list_empty (& peer -> outgoingUnreliableCommands) && 
        enet_list_empty (& peer -> sentReliableCommands))
      enet_peer_disconnect (peer, peer -> eventData);
}

static int
enet_protocol_check_timeouts (ENetHost * host, ENetPeer * peer, ENetEvent * event)
{
    ENetOutgoingCommand * outgoingCommand;
    ENetListIterator currentCommand, insertPosition;

    currentCommand = enet_list_begin (& peer -> sentReliableCommands);
    insertPosition = enet_list_begin (& peer -> outgoingReliableCommands);

    while (currentCommand != enet_list_end (& peer -> sentReliableCommands))
    {
       outgoingCommand = (ENetOutgoingCommand *) currentCommand;

       currentCommand = enet_list_next (currentCommand);

       if (ENET_TIME_DIFFERENCE (host -> serviceTime, outgoingCommand -> sentTime) < outgoingCommand -> roundTripTimeout)
         continue;

       if (peer -> earliestTimeout == 0 ||
           ENET_TIME_LESS (outgoingCommand -> sentTime, peer -> earliestTimeout))
         peer -> earliestTimeout = outgoingCommand -> sentTime;

       if (peer -> earliestTimeout != 0 &&
             (ENET_TIME_DIFFERENCE (host -> serviceTime, peer -> earliestTimeout) >= peer -> timeoutMaximum ||
               (outgoingCommand -> roundTripTimeout >= outgoingCommand -> roundTripTimeoutLimit &&
                 ENET_TIME_DIFFERENCE (host -> serviceTime, peer -> earliestTimeout) >= peer -> timeoutMinimum)))
       {
          enet_protocol_notify_disconnect (host, peer, event);

          return 1;
       }

       if (outgoingCommand -> packet != NULL)
         peer -> reliableDataInTransit -= outgoingCommand -> fragmentLength;
          
       ++ peer -> packetsLost;

       outgoingCommand -> roundTripTimeout *= 2;

       enet_list_insert (insertPosition, enet_list_remove (& outgoingCommand -> outgoingCommandList));

       if (currentCommand == enet_list_begin (& peer -> sentReliableCommands) &&
           ! enet_list_empty (& peer -> sentReliableCommands))
       {
          outgoingCommand = (ENetOutgoingCommand *) currentCommand;

          peer -> nextTimeout = outgoingCommand -> sentTime + outgoingCommand -> roundTripTimeout;
       }
    }
    
    return 0;
}

static int //其实就是把所有需要可靠发送的消息保存到ENetHost::buffers[1]以后的元素中，统一使用
enet_protocol_send_reliable_outgoing_commands (ENetHost * host, ENetPeer * peer)
{
    ENetProtocol * command = & host -> commands [host -> commandCount];
    ENetBuffer * buffer = & host -> buffers [host -> bufferCount];
    ENetOutgoingCommand * outgoingCommand;
    ENetListIterator currentCommand;
    ENetChannel *channel;
    enet_uint16 reliableWindow;
    size_t commandSize;
    int windowExceeded = 0, windowWrap = 0, canPing = 1;

    currentCommand = enet_list_begin (& peer -> outgoingReliableCommands);
    
    while (currentCommand != enet_list_end (& peer -> outgoingReliableCommands))
    {
       outgoingCommand = (ENetOutgoingCommand *) currentCommand;

       channel = outgoingCommand -> command.header.channelID < peer -> channelCount ? & peer -> channels [outgoingCommand -> command.header.channelID] : NULL;
       reliableWindow = outgoingCommand -> reliableSequenceNumber / ENET_PEER_RELIABLE_WINDOW_SIZE;
       if (channel != NULL)
       {
           if (! windowWrap &&      
               outgoingCommand -> sendAttempts < 1 && 
               ! (outgoingCommand -> reliableSequenceNumber % ENET_PEER_RELIABLE_WINDOW_SIZE) &&
               (channel -> reliableWindows [(reliableWindow + ENET_PEER_RELIABLE_WINDOWS - 1) % ENET_PEER_RELIABLE_WINDOWS] >= ENET_PEER_RELIABLE_WINDOW_SIZE ||
                 channel -> usedReliableWindows & ((((1 << ENET_PEER_FREE_RELIABLE_WINDOWS) - 1) << reliableWindow) | 
                   (((1 << ENET_PEER_FREE_RELIABLE_WINDOWS) - 1) >> (ENET_PEER_RELIABLE_WINDOWS - reliableWindow)))))
             windowWrap = 1;
          if (windowWrap)
          {
             currentCommand = enet_list_next (currentCommand);
 
             continue;
          }
       }
 
       if (outgoingCommand -> packet != NULL)
       {
          if (! windowExceeded)
          {
             enet_uint32 windowSize = (peer -> packetThrottle * peer -> windowSize) / ENET_PEER_PACKET_THROTTLE_SCALE;
             
             if (peer -> reliableDataInTransit + outgoingCommand -> fragmentLength > ENET_MAX (windowSize, peer -> mtu))
               windowExceeded = 1;
          }
          if (windowExceeded)
          {
             currentCommand = enet_list_next (currentCommand);

             continue;
          }
       }

       canPing = 0;

       commandSize = commandSizes [outgoingCommand -> command.header.command & ENET_PROTOCOL_COMMAND_MASK];
       if (command >= & host -> commands [sizeof (host -> commands) / sizeof (ENetProtocol)] ||
           buffer + 1 >= & host -> buffers [sizeof (host -> buffers) / sizeof (ENetBuffer)] ||  
           peer -> mtu - host -> packetSize < commandSize ||
           (outgoingCommand -> packet != NULL && 
             (enet_uint16) (peer -> mtu - host -> packetSize) < (enet_uint16) (commandSize + outgoingCommand -> fragmentLength)))
       {
          host -> continueSending = 1;
          
          break;
       }

       currentCommand = enet_list_next (currentCommand);

       if (channel != NULL && outgoingCommand -> sendAttempts < 1)
       {
          channel -> usedReliableWindows |= 1 << reliableWindow;
          ++ channel -> reliableWindows [reliableWindow];
       }

       ++ outgoingCommand -> sendAttempts;
 
       if (outgoingCommand -> roundTripTimeout == 0)
       {
          outgoingCommand -> roundTripTimeout = peer -> roundTripTime + 4 * peer -> roundTripTimeVariance;
          outgoingCommand -> roundTripTimeoutLimit = peer -> timeoutLimit * outgoingCommand -> roundTripTimeout;
       }

       if (enet_list_empty (& peer -> sentReliableCommands))
         peer -> nextTimeout = host -> serviceTime + outgoingCommand -> roundTripTimeout;

       enet_list_insert (enet_list_end (& peer -> sentReliableCommands),                              //插入正在发送的并且需要可靠发送的command队列
                         enet_list_remove (& outgoingCommand -> outgoingCommandList));

       outgoingCommand -> sentTime = host -> serviceTime;

       buffer -> data = command;
       buffer -> dataLength = commandSize;

       host -> packetSize += buffer -> dataLength;
       host -> headerFlags |= ENET_PROTOCOL_HEADER_FLAG_SENT_TIME;

       * command = outgoingCommand -> command;

       if (outgoingCommand -> packet != NULL) //若ENetOutgoingCommand::packet非空，则表示该command有数据（除了command本身的固定格式的数据以外，比如ENetProtocolSendFragment协议就还有具体的fragment数据）
       {
          ++ buffer;  //把command的具体数据放入下一个ENetBuffer中， 也就是说一个含有具体数据的ENetOutgoingCommand，会把command的固定协议数据和具体数据放入到相邻的2个ENetBuffer中
          
          buffer -> data = outgoingCommand -> packet -> data + outgoingCommand -> fragmentOffset;
          buffer -> dataLength = outgoingCommand -> fragmentLength;

          host -> packetSize += outgoingCommand -> fragmentLength;

          peer -> reliableDataInTransit += outgoingCommand -> fragmentLength;
       }

       ++ peer -> packetsSent;
        
       ++ command;
       ++ buffer;
    }//end of while

    host -> commandCount = command - host -> commands;
    host -> bufferCount = buffer - host -> buffers;

    return canPing;
}

static int //把所有需要发送的消息组包（包括压缩)，然后实际进行发送（使用enet_socket_send）
enet_protocol_send_outgoing_commands (ENetHost * host, ENetEvent * event, int checkForTimeouts)
{
    enet_uint8 headerData [sizeof (ENetProtocolHeader) + sizeof (enet_uint32)];
    ENetProtocolHeader * header = (ENetProtocolHeader *) headerData;
    ENetPeer * currentPeer;
    int sentLength;
    size_t shouldCompress = 0;
 
    host -> continueSending = 1;

    while (host -> continueSending)
    for (host -> continueSending = 0,
           currentPeer = host -> peers;
         currentPeer < & host -> peers [host -> peerCount];
         ++ currentPeer)
    {
        if (currentPeer -> state == ENET_PEER_STATE_DISCONNECTED ||
            currentPeer -> state == ENET_PEER_STATE_ZOMBIE)
          continue;

        host -> headerFlags = 0;
        host -> commandCount = 0;
        host -> bufferCount = 1;       //！！！！ 预留ENetHost::buffers[0] 用来保存CNetProtocolHeader（以及可能存在的checksum）,从ENetHost::buffers[1]以后才用来保存所有需要发送的数据
									   //比如：enet_protocol_send_acknowledgements函数中就是把所有需要发送的确认消息保存到ENetHost::buffers[1]以后的元素中
									   //此处的赋值也意味这先前发送的数据(在buffers[]中的老数据)被清除了，buffers[0] 也会在下面的逻辑中被重新赋值
        host -> packetSize = sizeof (ENetProtocolHeader);

        if (! enet_list_empty (& currentPeer -> acknowledgements))
          enet_protocol_send_acknowledgements (host, currentPeer);    //发送每个CNetPeer待发送的确认消息，其实就是把所有需要发送的确认消息保存到ENetHost::buffers[1]以后的元素中

        if (checkForTimeouts != 0 &&
            ! enet_list_empty (& currentPeer -> sentReliableCommands) &&
            ENET_TIME_GREATER_EQUAL (host -> serviceTime, currentPeer -> nextTimeout) &&
            enet_protocol_check_timeouts (host, currentPeer, event) == 1)
        {
            if (event != NULL && event -> type != ENET_EVENT_TYPE_NONE)
              return 1;
            else
              continue;
        }

        if ((enet_list_empty (& currentPeer -> outgoingReliableCommands) ||
              enet_protocol_send_reliable_outgoing_commands (host, currentPeer)) &&
            enet_list_empty (& currentPeer -> sentReliableCommands) &&
            ENET_TIME_DIFFERENCE (host -> serviceTime, currentPeer -> lastReceiveTime) >= currentPeer -> pingInterval &&
            currentPeer -> mtu - host -> packetSize >= sizeof (ENetProtocolPing))
        { 
            enet_peer_ping (currentPeer);  //向对端发送ping包
            enet_protocol_send_reliable_outgoing_commands (host, currentPeer);
        }
                      
        if (! enet_list_empty (& currentPeer -> outgoingUnreliableCommands))
          enet_protocol_send_unreliable_outgoing_commands (host, currentPeer);

        if (host -> commandCount == 0)
          continue;

        if (currentPeer -> packetLossEpoch == 0)
          currentPeer -> packetLossEpoch = host -> serviceTime;
        else
        if (ENET_TIME_DIFFERENCE (host -> serviceTime, currentPeer -> packetLossEpoch) >= ENET_PEER_PACKET_LOSS_INTERVAL &&
            currentPeer -> packetsSent > 0) //检查丢包率 每ENET_PEER_PACKET_LOSS_INTERVAL ms检测一次
        {
           enet_uint32 packetLoss = currentPeer -> packetsLost * ENET_PEER_PACKET_LOSS_SCALE / currentPeer -> packetsSent;

#ifdef ENET_DEBUG
           printf ("peer %u: %f%%+-%f%% packet loss, %u+-%u ms round trip time, %f%% throttle, %u/%u outgoing, %u/%u incoming\n", currentPeer -> incomingPeerID, currentPeer -> packetLoss / (float) ENET_PEER_PACKET_LOSS_SCALE, currentPeer -> packetLossVariance / (float) ENET_PEER_PACKET_LOSS_SCALE, currentPeer -> roundTripTime, currentPeer -> roundTripTimeVariance, currentPeer -> packetThrottle / (float) ENET_PEER_PACKET_THROTTLE_SCALE, enet_list_size (& currentPeer -> outgoingReliableCommands), enet_list_size (& currentPeer -> outgoingUnreliableCommands), currentPeer -> channels != NULL ? enet_list_size (& currentPeer -> channels -> incomingReliableCommands) : 0, currentPeer -> channels != NULL ? enet_list_size (& currentPeer -> channels -> incomingUnreliableCommands) : 0);
#endif
          
           currentPeer -> packetLossVariance -= currentPeer -> packetLossVariance / 4;

           if (packetLoss >= currentPeer -> packetLoss)
           {
              currentPeer -> packetLoss += (packetLoss - currentPeer -> packetLoss) / 8;
              currentPeer -> packetLossVariance += (packetLoss - currentPeer -> packetLoss) / 4;
           }
           else
           {
              currentPeer -> packetLoss -= (currentPeer -> packetLoss - packetLoss) / 8;
              currentPeer -> packetLossVariance += (currentPeer -> packetLoss - packetLoss) / 4;
           }

           currentPeer -> packetLossEpoch = host -> serviceTime;
           currentPeer -> packetsSent = 0;
           currentPeer -> packetsLost = 0;
        }

        host -> buffers -> data = headerData;  //也就是buffers[0] 就是headerData
        if (host -> headerFlags & ENET_PROTOCOL_HEADER_FLAG_SENT_TIME)
        {
            header -> sentTime = ENET_HOST_TO_NET_16 (host -> serviceTime & 0xFFFF);   //取低2Byte就可以了，节省通讯流量

            host -> buffers -> dataLength = sizeof (ENetProtocolHeader);
        }
        else
          host -> buffers -> dataLength = (size_t) & ((ENetProtocolHeader *) 0) -> sentTime;

        shouldCompress = 0;
        if (host -> compressor.context != NULL && host -> compressor.compress != NULL)
        {
            size_t originalSize = host -> packetSize - sizeof(ENetProtocolHeader),
                   compressedSize = host -> compressor.compress (host -> compressor.context,
                                        & host -> buffers [1], host -> bufferCount - 1,         //把从ENetHost::buffers[1]开始的所有数据 进行压缩. buffers[0] 应该是用来保存共同的CNetProtocolHeader的     
                                        originalSize,
                                        host -> packetData [1],                 //用ENetHost::packetData[1] 来输出压缩后的数据
                                        originalSize);
            if (compressedSize > 0 && compressedSize < originalSize)
            {
                host -> headerFlags |= ENET_PROTOCOL_HEADER_FLAG_COMPRESSED;
                shouldCompress = compressedSize;
#ifdef ENET_DEBUG_COMPRESS
                printf ("peer %u: compressed %u -> %u (%u%%)\n", currentPeer -> incomingPeerID, originalSize, compressedSize, (compressedSize * 100) / originalSize);
#endif
            }
        }

        if (currentPeer -> outgoingPeerID < ENET_PROTOCOL_MAXIMUM_PEER_ID)
          host -> headerFlags |= currentPeer -> outgoingSessionID << ENET_PROTOCOL_HEADER_SESSION_SHIFT;
        header -> peerID = ENET_HOST_TO_NET_16 (currentPeer -> outgoingPeerID | host -> headerFlags);        //组装peer id (包括低12bit的真实peer id，以及第12-13bit的session id, 第14bit的压缩标识, 第15bit 是否包含发送时间的标识
																											 //对于新连接请求协议 由于还没有连接到对端(server)所以此时的currentPeer->outgoingPeerID = 0xFFF(初始化时赋予的初始值)
		if (host -> checksum != NULL)
        {
            enet_uint32 * checksum = (enet_uint32 *) & headerData [host -> buffers -> dataLength];
            * checksum = currentPeer -> outgoingPeerID < ENET_PROTOCOL_MAXIMUM_PEER_ID ? currentPeer -> connectID : 0;
            host -> buffers -> dataLength += sizeof (enet_uint32);
            * checksum = host -> checksum (host -> buffers, host -> bufferCount);
        }

        if (shouldCompress > 0) //如果已经把ENetHost::buffers[1]开始的数据进行了压缩， 那要发送的buffer就只有buffers[0] -- 就是ENetProtocolHeader（以及可能含checksum), buffers[1]
        {
            host -> buffers [1].data = host -> packetData [1];
            host -> buffers [1].dataLength = shouldCompress;
            host -> bufferCount = 2;   //压缩后只剩下buffers[0](ENetProtocolHeader）以及压缩后数据buffers[1]了
        }

        currentPeer -> lastSendTime = host -> serviceTime;

        sentLength = enet_socket_send (host -> socket, & currentPeer -> address, host -> buffers, host -> bufferCount);

        enet_protocol_remove_sent_unreliable_commands (currentPeer);

		if (sentLength < 0)
		{
			LOGV("in enet_protocol_send_outgoing_commands,  enet_socket_send error \n");
			return -1;
		}
          

        host -> totalSentData += sentLength;
        host -> totalSentPackets ++;
    } //end of  for loop
   
    return 0;
}

/** Sends any queued packets on the host specified to its designated peers.

    @param host   host to flush
    @remarks this function need only be used in circumstances where one wishes to send queued packets earlier than in a call to enet_host_service().
    @ingroup host
*/
void
enet_host_flush (ENetHost * host)
{
    host -> serviceTime = enet_time_get ();

    enet_protocol_send_outgoing_commands (host, NULL, 0);
}

/** Checks for any queued events on the host and dispatches one if available.

    @param host    host to check for events
    @param event   an event structure where event details will be placed if available
    @retval > 0 if an event was dispatched
    @retval 0 if no events are available
    @retval < 0 on failure
    @ingroup host
*/
int
enet_host_check_events (ENetHost * host, ENetEvent * event)
{
    if (event == NULL) return -1;

    event -> type = ENET_EVENT_TYPE_NONE;
    event -> peer = NULL;
    event -> packet = NULL;

    return enet_protocol_dispatch_incoming_commands (host, event);
}

/** Waits for events on the host specified and shuttles packets between
    the host and its peers.

    @param host    host to service
    @param event   an event structure where event details will be placed if one occurs   //event是单个元素不是ENetEvent数组
                   if event == NULL then no events will be delivered
    @param timeout number of milliseconds that ENet should wait for events
    @retval > 0 if an event occurred within the specified time limit
    @retval 0 if no event occurred
    @retval < 0 on failure
    @remarks enet_host_service should be called fairly regularly for adequate performance
    @ingroup host
*/
int
enet_host_service (ENetHost * host, ENetEvent * event, enet_uint32 timeout)
{
    enet_uint32 waitCondition;
	enet_uint32 realTimeout = timeout;
    if (event != NULL)
    {
        event -> type = ENET_EVENT_TYPE_NONE;
        event -> peer = NULL;
        event -> packet = NULL;

        switch (enet_protocol_dispatch_incoming_commands (host, event))
        {
        case 1:
            return 1;

        case -1:
// #ifdef ENET_DEBUG
//             perror ("Error dispatching incoming packets");
// #endif
			LOGV("in enet_host_service, enet_protocol_dispatch_incoming_commands fail\n");
            return -1;

        default:
            break;
        }
    }

    host -> serviceTime = enet_time_get ();
    
    timeout += host -> serviceTime;

    do
    {
       if (ENET_TIME_DIFFERENCE (host -> serviceTime, host -> bandwidthThrottleEpoch) >= ENET_HOST_BANDWIDTH_THROTTLE_INTERVAL)
         enet_host_bandwidth_throttle (host);

       switch (enet_protocol_send_outgoing_commands (host, event, 1))
       {
       case 1:
          return 1;

       case -1:
// #ifdef ENET_DEBUG
//           perror ("Error sending outgoing packets");
// #endif
		  LOGV("in enet_host_service, enet_protocol_send_outgoing_commands error \n");
          return -1;

       default:
          break;
       }

       switch (enet_protocol_receive_incoming_commands (host, event)) //调用enet_socket_receive接收数据并调用enet_protocol_handle_incoming_commands处理数据
       {
       case 1:
          return 1;

       case -1:
// #ifdef ENET_DEBUG
//           perror ("Error receiving incoming packets");
// #endif
		    LOGV("in enet_host_service, enet_protocol_receive_incoming_commands error \n");
            return -1;

       default:
          break;
       }

       switch (enet_protocol_send_outgoing_commands (host, event, 1))
       {
       case 1:
          return 1;

       case -1:
// #ifdef ENET_DEBUG
//           perror ("Error sending outgoing packets");
// #endif
#if defined(ENET_DEBUG_LOG)
		   LOGV("in enet_host_service, enet_protocol_send_outgoing_commands error \n");
#endif
          return -1;

       default:
          break;
       }

       if (event != NULL)
       {
          switch (enet_protocol_dispatch_incoming_commands (host, event)) 
          {
          case 1:
             return 1;

          case -1:
			 LOGV("in enet_host_service, enet_protocol_dispatch_incoming_commands error \n");
             return -1;

          default:
             break;
          }
       }

       if (ENET_TIME_GREATER_EQUAL (host -> serviceTime, timeout))
         return 0;

       do //此处循环检测host -> socket是否有数据可读，如果可读 则继续外层的while循环 进行读取并处理
       {
          host -> serviceTime = enet_time_get ();

          if (ENET_TIME_GREATER_EQUAL (host -> serviceTime, timeout))
            return 0;

          waitCondition = ENET_SOCKET_WAIT_RECEIVE | ENET_SOCKET_WAIT_INTERRUPT;

//#if defined(__linux__) || defined(__gnu_linux__)  //modified by aprilliu
//		  if (enet_reactor_wait(host, &waitCondition, ENET_TIME_DIFFERENCE(timeout, host -> serviceTime)) != 0)
//#else
		  int ret = enet_socket_wait(host->socket, &waitCondition, ENET_TIME_DIFFERENCE(timeout, host->serviceTime));
		  if (ret  != 0)
//#endif
		  {
			  LOGV("in enet_host_service, enet_socket_wait error, timeout: %u, return value(%d)\n", realTimeout, ret);
			  return -1;
		  }
            
       }
       while (waitCondition & ENET_SOCKET_WAIT_INTERRUPT);

       host -> serviceTime = enet_time_get ();
    } while (waitCondition & ENET_SOCKET_WAIT_RECEIVE); // do ... while

    return 0; 
}

