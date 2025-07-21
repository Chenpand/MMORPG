#ifndef _US_PLAYER_MGR_H_
#define _US_PLAYER_MGR_H_

#include <AvalonSingleton.h>
#include <CLPacketExecutor.h>
#include <functional>

#include <CLDefine.h>
#include "USPlayer.h"

namespace CLProtocol
{
	class SysTransmitPlayerToUnion;
}

class UnionService;

using USPlayerMap = HashMap<UInt64, USPlayer*>;
using USPlayerVec = std::vector<USPlayer*>;

struct RoleIDConnID
{
	UInt32 accID;
	UInt64 roleID;
	UInt32 connId;
	std::map<UInt32, UInt32> sourceNode;
};

/**
*@brief �����ҹ�����
*/
class USPlayerMgr
{


public:
	USPlayerMgr(UnionService& service);
	~USPlayerMgr();

	/**
	*@brief ��ʼ��
	*/
	void Init();

	/**
	*@brief tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief �������
	*/
	USPlayer* FindUSPlayer(UInt64 id);

	/**
	*@brief �������
	*/
	USPlayer* FindUSPlayerByAccid(UInt32 accid);


	/**
	*@brief �������
	*/
	USPlayer* AddUSPlayer(UInt64 id);

	/**
	*@brief ɾ�����
	*/
	void RemoveUSPlayer(UInt64 id);

	/**
	*@brief ������
	*/
	void ClearUSPlayer();

	/**
	*@brief �����������
	*/
	USPlayerVec& GetAllUSPlayer();

	/**
	*@brief �����������
	*/
	USPlayerVec GetSuitUSPlayer(std::function<bool (USPlayer* )> suit);

	/**
	*@brief �㲥��Ϣ
	*/
	void BroadcastToPlayer(Avalon::Packet* packet);
	void BroadcastToPlayer(Avalon::Protocol& protocol);

public:
	void RegisterProtocols();
	void ExecuteProtocol(const CLProtocol::SysTransmitPlayerToUnion& protocol);

private:
	//�����Ϣ��ִ����
	CLParamPacketExecutor<USPlayer*>	m_Executor;

	//����Ҫ�������������Ϣ��ִ����
	CLParamPacketExecutor<RoleIDConnID>	m_IDExecutor;
	
	// ��ҹ���
	USPlayerMap m_PlayerMap;
	USPlayerVec m_PlayerVec;
	
	//�����еĿ������
	UnionService& m_UnionService;
};

#endif
