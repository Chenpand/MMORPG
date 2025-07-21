#ifndef _GAMEZONE_MGR_H_
#define _GAMEZONE_MGR_H_

#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include <AvalonProtocol.h>
#include "GameZone.h"

#include <set>
/** 
 *@brief ��Ϸ��������
 */
class GameZoneMgr : public Avalon::Singleton<GameZoneMgr>
{
	typedef std::map<UInt32, GameZone*>	GameZoneMap;

public:
	GameZoneMgr();
	~GameZoneMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief ��ӡ�ɾ����������Ϸ��
	 */
	bool AddGameZone(GameZone* zone);
	void RemoveGameZone(GameZone* zone);
	GameZone* FindGameZone(UInt32 id);

	GameZone* FindGameZoneByID(UInt32 id);

	/**
	 *@brief ����������Ϸ��
	 */
	void VisitGameZones(GameZoneVisitor& visitor);

public://��Ϣ���
	/** 
	 *@brief �㲥��Ϣ
	 */
	void BroadcastToGameZone(Avalon::Packet* packet);
	void BroadcastToGameZone(Avalon::Protocol& protocol);

	void BroadcastToGameZoneWorldByZoneId(Avalon::Packet* packet,std::set<UInt32>& st);
	void BroadcastToGameZoneWorldByZoneId(Avalon::Protocol& protocol,std::set<UInt32>& st);

public://�¼�
	/**
	 *@brief ����
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ���ӽ���
	 */
	void OnConnected(UInt32 id);

	/**
	 *@brief ���ӶϿ�
	 */
	void OnDisconnected(UInt32 id);

private:
	//��Ϸ������
	CLUIntObjIndex<GameZone>	m_Zones;
	//��id
	GameZoneMap					m_IdZones;
};

#endif
