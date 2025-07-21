#ifndef _SCENE_SESSION_MGR_H_
#define _SCENE_SESSION_MGR_H_

#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include <AvalonProtocol.h>
#include "SceneSession.h"

/** 
 *@brief �������������ӹ�����
 */
class SceneSessionMgr : public Avalon::Singleton<SceneSessionMgr>
{
public:
	SceneSessionMgr();
	~SceneSessionMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief ��ӡ�ɾ����������Ϸ��
	 */
	bool AddSceneSession(SceneSession* session);
	void RemoveSceneSession(SceneSession* session);
	SceneSession* FindSceneSession(UInt32 id);

	/**
	 *@brief �������з�����
	 */
	void VisitSceneSessions(SceneSessionVisitor& visitor);

public://��Ϣ���
	/**
	*@brief ����Э��
	*/
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

public://�¼�
	/**
	 *@brief ���ӽ���
	 */
	void OnConnected(UInt32 id);

	/**
	 *@brief ���ӶϿ�
	 */
	void OnDisconnected(UInt32 id);

private:
	//�淨������
	CLUIntObjIndex<SceneSession>	m_Sessions;
};

#endif
