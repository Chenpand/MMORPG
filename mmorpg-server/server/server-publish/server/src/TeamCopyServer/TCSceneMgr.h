#ifndef _TC_SCENE_MGR_H_
#define _TC_SCENE_MGR_H_

#include <set>
#include <AvalonSingleton.h>

#include "TCScene.h"

class TCSPlayer;

/**
 *@brief �ű�����������                                                                     
 */
class TCSceneMgr : public Avalon::Singleton<TCSceneMgr>
{
public:
	TCSceneMgr();
	~TCSceneMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief �¼�                                                                     
	 */
	void OnTick(const Avalon::Time& now);

	/**
	*@brief ׼������
	*/
	void AddToPrepareScene(UInt32 copyId, UInt64 playerId);
	void RemoveToPrepareScene(UInt32 copyId, UInt64 playerId);

	/**
	*@brief ͬ����Ϣ
	*/
	void AddSyncPacket(UInt64 playerId, Avalon::Packet* packet);
	void RemoveSyncPacket(UInt64 playerId);

	/**
	*@brief ͬ����Ϣ
	*/
	void SyncAllObjectToPlayer(TCSPlayer* player, UInt32 copyId);

	/**
	*@brief �㲥��Ϣ
	*/
	Avalon::Packet* GetPlayerSyncPacket(UInt64 playerId);

	/**
	*@brief �㲥��Ϣ
	*/
	void BroadcastPacket(Avalon::Packet* packet, UInt32 copyId);
	void BroadcastProtocol(Avalon::Protocol& protocol, UInt32 copyId);

private:

	/**
	*@brief ����׼������
	*/
	TCScene* _FindTCScene(UInt32 copyId);

private:

	// ׼�������б�(key->�ű�id)
	std::map<UInt32, TCScene> m_PrepareSceneMap;

	// ���ͬ����Ϣ(key->playerId)
	std::map<UInt64, Avalon::Packet*> m_SyncPacketMap;
};

#endif
