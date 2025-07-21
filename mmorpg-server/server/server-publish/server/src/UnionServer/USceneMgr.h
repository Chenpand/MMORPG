#ifndef _TC_SCENE_MGR_H_
#define _TC_SCENE_MGR_H_

#include <set>
#include <AvalonSingleton.h>

#include "UScene.h"

class USPlayer;
class UnionService;

/**
 *@brief �������������                                                                     
 */
class USceneMgr
{
public:
	USceneMgr(UnionService& service);
	~USceneMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief �¼�                                                                     
	 */
	void OnTick(const Avalon::Time& now);

	/**
	*@brief ����
	*/
	UScene* AddScene(UInt32 id);
	void RemoveScene(UInt32 Id);
	void ClearScene();

	std::map<UInt32, UScene*>& GetAllScene();
	/**
	*@brief ���ҳ���
	*/
	UScene* FindUScene(UInt32 Id);

	/**
	*@brief ͬ����Ϣ
	*/
	void AddSyncPacket(UInt64 playerId, Avalon::Packet* packet);
	void RemoveSyncPacket(UInt64 playerId);

	/**
	*@brief ͬ����Ϣ
	*/
	void SyncAllObjectToPlayer(USPlayer* player, UInt32 Id);

	/**
	*@brief �㲥��Ϣ
	*/
	Avalon::Packet* GetPlayerSyncPacket(UInt64 playerId);

	/**
	*@brief �㲥��Ϣ
	*/
	void BroadcastPacket(Avalon::Packet* packet);
	void BroadcastProtocol(Avalon::Protocol& protocol);


private:

	// �����б�
	std::map<UInt32, UScene*> m_SceneMap;

	// ���ͬ����Ϣ(key->playerId)
	std::map<UInt64, Avalon::Packet*> m_SyncPacketMap;

	//�����еĿ������
	UnionService& m_UnionService;
};

#endif
