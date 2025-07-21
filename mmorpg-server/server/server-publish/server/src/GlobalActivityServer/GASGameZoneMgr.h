#ifndef _GAS_GAMEZONE_MGR_H_
#define _GAS_GAMEZONE_MGR_H_

#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include <AvalonProtocol.h>
#include "GASGameZone.h"

/** 
 *@brief ��Ϸ��������
 */
class GASGameZoneMgr : public Avalon::Singleton<GASGameZoneMgr>
{
	typedef std::map<UInt32, GASGameZone*>	GASGameZoneMap;

public:
	GASGameZoneMgr();
	~GASGameZoneMgr();

public:
	void Final();

public:
	/**
	 *@brief ��ӡ�ɾ����������Ϸ��
	 */
	bool AddGASGameZone(GASGameZone* zone);
	void RemoveGASGameZone(GASGameZone* zone);
	GASGameZone* FindGASGameZone(UInt32 id);

	/**
	 *@brief ����������Ϸ��
	 */
	void VisitGASGameZones(GASGameZoneVisitor& visitor);

public://��Ϣ���
	/** 
	 *@brief �㲥��Ϣ
	 */
	void BroadcastToGASGameZone(Avalon::Packet* packet);
	void BroadcastToGASGameZone(Avalon::Protocol& protocol);

	/**
	*@brief �㲥��Ϣ��word
	*/
	void BroadcastToWorld(Avalon::Protocol& protocol);

	/**
	*@brief �㲥��Ϣ������gate
	*/
	void BroadcastToGate(Avalon::Protocol& protocol);


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
	//��Ϸ������
	CLUIntObjIndex<GASGameZone>	m_Zones;
};

#endif
