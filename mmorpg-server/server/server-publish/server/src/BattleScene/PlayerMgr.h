#ifndef _PLAYER_MGR_H_
#define _PLAYER_MGR_H_

#include <AvalonPropertySet.h>
#include <AvalonSingleton.h>
#include <CLObjectIndex.h>
#include <CLPacketExecutor.h>
#include "Player.h"

/**
 *@brief ��ҹ�����
 */
class PlayerMgr : public Avalon::Singleton<PlayerMgr>
{
public:
	PlayerMgr();
	~PlayerMgr();

public:
	bool Init();
	void Final();

	/**
	 *@brief ���һ�����
	 */
	bool AddPlayer(Player* player);

	/**
	*@brief ɾ��һ�����
	*/
	void DelPlayer(Player* player);

	/**
	 *@brief ����һ�����
	 */
	Player* FindPlayer(ObjID_t id);
	Player* FindNormalPlayer(ObjID_t id);
	Player* FindPlayer(const std::string &strName);
	Player* FindNormalPlayer(const std::string &strName);
	Player* FindLoadingFinishPlayer(ObjID_t id);
	// ���߼�����һص������߼�
	void LoadOfflinePlayerCb(ObjID_t roleid, UInt32 accid, PlayerLoadingCallback* cb);

	/**
	 *@brief �Ƴ�һ�����
	 */
	void RemovePlayer(Player* player, bool removeFromMap = false);

public:
	/**
	 *@brief ִ��һ����Ϣ��
	 */
	void ExecutePacket(ObjID_t id,Avalon::Packet* packet);

	/**
	*@brief չʾһ֡���յ��İ�
	*/
	void ShowPacketInOneTick();

public:
	/**
	 *@brief �����������
	 */
	void VisitPlayers(PlayerVisitor& visitor);
	void VisitNormalPlayers(PlayerVisitor& visitor);

public:
	/**
	 *@brief ���ضϿ�����
	 */
	void OnGateDisconnected(UInt32 id);

	/**
	 *@brief ����
	 */
	void OnTick(const Avalon::Time& now);

	/**
	*@brief �˺�������ɾ��
	*/
	void OnPlayerForeverDeleted(ObjID_t roleId);

	/**
	*@brief ��ý�ɫ��ʼֵ����
	*/
	Avalon::PropertySet* GetPlayerAttr() { return &m_InitAttrs; }

	/**
	*@brief ��ȡ��ҳ�ʼ����
	*/
	UInt32 GetInitSceneID() { return GetPlayerAttr()->GetProperty("PlayerInitInfo", "scene"); }

private:
	/**
	 *@brief ע�����Э��
	 */
	void RegisterProtocols();

	/**
	*@brief �жϻ��������������������һ��ֵ���ͳ���ɾ���������
	*/
	void TryRelaseCachePlayerData();

private:
	//id->player����
	CLIDObjIndex<Player>	m_Players;
	//�����Ϣ��ִ����
	CLParamPacketExecutor<Player*>	m_Executor;

	//�ֶ�ʱ��
	Avalon::SimpleTimer		m_MinTimer;

	//��ɫ��ʼ����
	Avalon::PropertySet		m_InitAttrs;

	// һ��tick�ڸ�����Ϣ������
	std::map<UInt32, UInt32>	m_PacketNumInOneTick;
};


#endif
