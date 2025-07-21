#ifndef _GS_SCENE_H_
#define _GS_SCENE_H_

#include <AvalonProtocol.h>
#include <CLScene.h>

#include "GSPlayer.h"

/**
 *@brief ���س���
 */
class GSScene : public CLScene
{
public:
	GSScene();
	~GSScene();

public:
	bool Init(UInt32 id, const std::string& name, UInt32 length,
		UInt32 width, UInt32 syncRange, UInt32 nodeid,
		UInt32 sceneDataId, UInt8 sceneType);

	/**
	 *@brief ���һ�����
	 */
	bool AddPlayer(UInt32 gridIndex,GSPlayer* player);

	/**
	 *@brief �Ƴ�һ�����
	 */
	void RemovePlayer(GSPlayer* player);

	/**
	 *@brief ��ȡ���ڽڵ�id
	 */
	UInt32 GetNodeID() const;

	void SetSceneDataId(UInt32 sceneDataId);
	UInt32 GetSceneDataId();

	void SetSceneType(UInt8 sceneType);
	UInt8 GetSceneType();
public://visit�ӿ�
	/**
	 *@brief �����������
	 */
	void VisitPlayers(GSPlayerVisitor& visitor);

	/**
	 *@brief ������Χ�������
	 */
	void VisitAround(UInt32 grid,GSPlayerVisitor& visitor);

	/**
	 *@brief ������ͬ�������
	 */
	void VisitExcept(UInt32 grid1,UInt32 grid2,GSPlayerVisitor& visitor);

public://������Ϣ
	/**
	 *@brief �㲥Э�鵽��Χ
	 */
	void BroadcastAround(UInt32 grid,Avalon::Protocol& protocol);

	/**
	 *@brief �㲥��Ϣ����Χ
	 */
	void BroadcastAround(UInt32 grid,Avalon::Packet* packet,ObjID_t except = 0);

	/**
	 *@brief �㲥��Ϣ����ͬ����
	 */
	void BroadcastExcept(UInt32 grid1, UInt32 grid2,Avalon::Packet* packet,ObjID_t except);

	/**
	 *@brief ������Ϣ������
	 */
	void SendToSceneServer(Avalon::Protocol& protocol);
private:
	//scene����
	Avalon::NetConnectionPtr m_Conn;
	//������id
	UInt32 m_sceneDataId;
	//��������
	UInt8 m_sceneType;
};

typedef CLVisitor<GSScene> GSSceneVisitor;

#endif
