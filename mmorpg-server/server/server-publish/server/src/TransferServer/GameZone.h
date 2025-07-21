#ifndef _GAME_ZONE_H_
#define _GAME_ZONE_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>

/**
 *@brief 游戏区
 */
class GameZone
{
public:
	GameZone();
	~GameZone();

public:
	/**
	 *@brief 设置获取id
	 */
	bool Init(UInt32 id);
	UInt32 GetID() const{ return m_Id; }
	UInt32 GetZoneID() const{ return CL_GET_ZONEID_FROM_NODEID(m_Id); }

public: //消息相关
	/**
	 *@brief 发送消息
	 */
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	 *@brief 转发消息到word
	 */
	void TransmitToWorld(Avalon::Packet* packet);
	void TransmitToWorld(Avalon::Protocol& protocol);

	/**
	*@brief 转发消息到scene
	*/
	void TransmitToScene(Avalon::Packet* packet);
	void TransmitToScene(Avalon::Protocol& protocol);

private:
	//id
	UInt32	m_Id;
	//游戏区管理服务器连接
	Avalon::NetConnectionPtr m_Conn;
};

typedef CLVisitor<GameZone>	GameZoneVisitor;

#endif
