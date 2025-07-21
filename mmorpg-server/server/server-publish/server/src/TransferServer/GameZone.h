#ifndef _GAME_ZONE_H_
#define _GAME_ZONE_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>

/**
 *@brief ��Ϸ��
 */
class GameZone
{
public:
	GameZone();
	~GameZone();

public:
	/**
	 *@brief ���û�ȡid
	 */
	bool Init(UInt32 id);
	UInt32 GetID() const{ return m_Id; }
	UInt32 GetZoneID() const{ return CL_GET_ZONEID_FROM_NODEID(m_Id); }

public: //��Ϣ���
	/**
	 *@brief ������Ϣ
	 */
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	 *@brief ת����Ϣ��word
	 */
	void TransmitToWorld(Avalon::Packet* packet);
	void TransmitToWorld(Avalon::Protocol& protocol);

	/**
	*@brief ת����Ϣ��scene
	*/
	void TransmitToScene(Avalon::Packet* packet);
	void TransmitToScene(Avalon::Protocol& protocol);

private:
	//id
	UInt32	m_Id;
	//��Ϸ���������������
	Avalon::NetConnectionPtr m_Conn;
};

typedef CLVisitor<GameZone>	GameZoneVisitor;

#endif
