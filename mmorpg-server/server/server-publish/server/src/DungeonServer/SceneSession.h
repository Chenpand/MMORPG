#ifndef _SCENE_SESSION_H_
#define _SCENE_SESSION_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>


/**
 *@brief ��������������
 */
class SceneSession
{
public:
	SceneSession();
	~SceneSession();

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

private:
	//id
	UInt32	m_Id;

	//��������������
	Avalon::NetConnectionPtr m_Conn;
};

typedef CLVisitor<SceneSession>	SceneSessionVisitor;

#endif
