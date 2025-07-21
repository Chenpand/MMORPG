#ifndef _GAS_GAME_ZONE_H_
#define _GAS_GAME_ZONE_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>

/**
 *@brief ��Ϸ��
 */
class GASGameZone
{
public:
	GASGameZone();
	~GASGameZone();

public:
	/**
	 *@brief ���û�ȡid
	 */
	bool Init(UInt32 id);
	UInt32 GetID() const{ return m_Id; }

public: //��Ϣ���
	/**
	 *@brief ������Ϣ
	 */
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	*@brief ת�������Ϣ
	*/
	void TransmitPlayerMsg(const std::string uid, UInt32 adminNode, Avalon::Packet* packet);
	void TransmitPlayerMsg(const std::string uid, UInt32 adminNode, Avalon::Protocol& protocol);

	/**
	*@brief ת����Ϣ��admin
	*/
	void TransmitToAdmin(UInt32 adminNode, Avalon::Protocol& protocol);

	/**
	*@brief ת����Ϣ��gate
	*/
	void BroadcastToGate(UInt32 adminNode, Avalon::Protocol& protocol);
	void BroadcastToGate(Avalon::Protocol& protocol);

	/**
	 *@brief ת����Ϣ��word
	 */
	void BroadcastToWorld(Avalon::Protocol& protocol);
	void TransmitToWorld(UInt32 adminNode, Avalon::Protocol& protocol);

private:
	//id
	UInt32	m_Id;
	//��Ϸ���������������
	Avalon::NetConnectionPtr m_Conn;
};

typedef CLVisitor<GASGameZone>	GASGameZoneVisitor;

#endif
