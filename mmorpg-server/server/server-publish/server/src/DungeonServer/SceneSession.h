#ifndef _SCENE_SESSION_H_
#define _SCENE_SESSION_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>


/**
 *@brief 场景服务器连接
 */
class SceneSession
{
public:
	SceneSession();
	~SceneSession();

public:
	/**
	 *@brief 设置获取id
	 */
	bool Init(UInt32 id);
	UInt32 GetID() const{ return m_Id; }

public: //消息相关
	/**
	 *@brief 发送消息
	 */
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

private:
	//id
	UInt32	m_Id;

	//场景服务器连接
	Avalon::NetConnectionPtr m_Conn;
};

typedef CLVisitor<SceneSession>	SceneSessionVisitor;

#endif
