#ifndef _SS_ROUTER_H_
#define _SS_ROUTER_H_

#include <CLDefine.h>
#include <AvalonProtocol.h>

namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	 *@brief ���͵�admin
	 */
	void SendToAdmin(Avalon::Protocol& protocol);
	void SendToAdmin(Avalon::Packet* packet);

	/**
	 *@brief ���͵�World
	 */
	void SendToWorld(Avalon::Protocol& protocol);
	void SendToWorld(Avalon::Packet* packet);

	/**
	 *@brief ���͵�social
	 */
	void SendToSocial(Avalon::Protocol& protocol);
	void SendToSocial(Avalon::Packet* packet);

	/**
	 *@brief ���͵�center
	 */
	void SendToCenter(Avalon::Protocol& protocol);
	void SendToCenter(Avalon::Packet* packet);

	/**
	 *@brief ���͵���Ӧ��Ϸ��                                                                     
	 */
	void SendToZone(UInt32 zoneId, Avalon::Protocol& protocol);
	void SendToZone(UInt32 zoneId, Avalon::Packet* packet);

	/**
	 *@brief �㲥��������Ϸ��                                                       
	 */
	void BroadcastToZones(Avalon::Protocol& protocol);
	void BroadcastToZones(Avalon::Packet* packet);

	/**
	 *@brief ���͵����������
	 */
	void SendToCross(Avalon::Protocol& protocol);
	void SendToCross(Avalon::Packet* packet);

	/**
	*@brief ���͵�¼�������
	*/
	void SendToReplay(Avalon::Protocol& protocol);
	void SendToReplay(Avalon::Packet* packet);

	/**
	 *@brief ���͵�sortlist                                                                     
	 */
	void SendToSortList(Avalon::Protocol& protocol);
	void SendToSortList(Avalon::Packet* packet);

	/**
	 *@brief �㲥������
	 */
	void BroadcastToGate(Avalon::Protocol& protocol);

	/**
	 *@brief ���͵���һ������������
	 */
	void SendToScene(UInt32 nodeid,Avalon::Protocol& protocol);
	void SendToScene(UInt32 nodeid,Avalon::Packet* packet);

	/**
	 *@brief �㲥�����г���
	 */
	void BroadcastToScene(Avalon::Protocol& protocol);

	/**
	 *@brief �㲥���������
	 */
	void BroadcastToPlayer(Avalon::Protocol& protocol, UInt8 bZone = 1);
	void BroadcastToPlayer(Avalon::Packet* packet, UInt8 bZone = 1);

	/**
	 *@brief �㲥��ָ���ȼ����ϵ����
	 */
	void BroadcastToPlayerByLvl(UInt16 level, Avalon::Protocol& protocol, UInt8 bZone = 1);
	void BroadcastToPlayerByLvl(UInt16 level, Avalon::Packet* packet, UInt8 bZone = 1);

	/**
	 *@brief ת�������
	 */
	void TransmitToPlayer(ObjID_t id, Avalon::Protocol& protocol);
	void TransmitToPlayer(ObjID_t id, Avalon::Packet* packet);

    /**
    *@brief ����MatchServer
    */
    void SendToMatch(Avalon::Protocol& protocol);
    
    /**
    *@brief ����RelayServer
    */
    void SendToRelay(UInt32 nodeId, Avalon::Protocol& protocol);

	/**
	*@brief ����WSceneServer
	*/
	void SendToWScene(UInt32 nodeId, Avalon::Protocol& protocol);
	void SendToBScene(UInt32 nodeId, Avalon::Protocol& protocol);

	/**
	*@brief WScene�����Լ����ڵ�Scene
	*/
	void SendToSelfScene(UInt32 nodeId, Avalon::Protocol& protocol);
	void SendToSelfScenePlayer(UInt32 nodeId, ObjID_t id, Avalon::Protocol& protocol);

	/**
	*@brief ���͵�ս��������
	*/
	void SendToBattle(Avalon::Protocol& protocol);
	void SendToBattle(Avalon::Packet* packet);

	/**
	*@brief ���͵����³Ƿ�����
	*/
	void SendToDungeon(Avalon::Protocol& protocol);
	void SendToDungeon(Avalon::Packet* packet);
}

#endif
