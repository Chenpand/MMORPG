#ifndef _WS_ROUTER_H_
#define _WS_ROUTER_H_

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
    *@brief ���͵�match
    */
    void SendToMatch(Avalon::Protocol& protocol);
    void SendToMatch(Avalon::Packet* packet);

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
	 *@brief ���͵�cross
	 */
	void SendToCross(Avalon::Protocol& protocol);
	void SendToCross(Avalon::Packet* packet);

	/**
	 *@brief ���͵�sortlist
	 */
	void SendToSortList(Avalon::Protocol& protocol);
	void SendToSortList(Avalon::Packet* packet);

	/**
	*@brief ���͵�globalActivity
	*/
	void SendToGlobalActivity(Avalon::Protocol& protocol);
	void SendToGlobalActivity(Avalon::Packet* packet);

	/**
	*@brief ���͵�relay
	*/
	void SendToRelay(UInt32 id, Avalon::Protocol& protocol);

	/**
	 *@brief ת�����Э�鵽sortlist
	 */
	void TransmitPlayerProtocolToSortList(ObjID_t playerId, Avalon::Protocol& protocol);


	/**
	 *@brief �㲥������
	 */
	void BroadcastToGate(Avalon::Protocol& protocol);

	/**
	 *@brief �㲥������
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
	*@brief ���͵�ս��������
	*/
	void SendToBattle(Avalon::Protocol& protocol);
	void SendToBattle(Avalon::Packet* packet);

	/**
	*@brief ���͵��ű�������
	*/
	void SendToTeamCopy(Avalon::Protocol& protocol);
	void SendToTeamCopy(Avalon::Packet* packet);

	/**
	*@brief ���͵���ת������
	*/
	void SendToTransfer(TransferHeader& tHeader, Avalon::Protocol& protocol);
	void SendToTransfer(TransferHeader& tHeader, Avalon::Packet* packet);

	/**
	*@brief ���͵�Union������
	*/
	void SendToUnion(UnionServerFunctionType type, Avalon::Protocol& protocol);
	void SendToUnion(UnionServerFunctionType type, Avalon::Packet* packet);
}

#endif
