#ifndef _TCS_PLAYER_MGR_H_
#define _TCS_PLAYER_MGR_H_

#include <AvalonSingleton.h>
#include <CLPacketExecutor.h>
#include <AvalonSimpleTimer.h>

#include <CLDefine.h>
#include "TCSPlayer.h"

namespace CLProtocol
{
	class SysTransmitPlayerToTeamCopy;
}
/**
*@brief �ű���ҹ�����
*/
class TCSPlayerMgr : public Avalon::Singleton<TCSPlayerMgr>
{
	typedef HashMap<UInt64, TCSPlayer*> TCSPlayerMap;
	typedef std::vector<TCSPlayer*> TCSPlayerVec;

public:
	TCSPlayerMgr();
	~TCSPlayerMgr();

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
	TCSPlayer* FindTCSPlayer(UInt64 id);

	/**
	*@brief ������һ�����
	*/
	TCSPlayer* RandTCSPlayer();
	void RandTCSPlayer(UInt32 num, std::vector<TCSPlayer*>& playerList, UInt32 param, bool isTeam = false);

	/**
	*@brief �������
	*/
	void AddTCSPlayer(TCSPlayer* tcsPlayer);

	/**
	*@brief ɾ�����
	*/
	void RemoveTCSPlayer(UInt64 playerId);

	/**
	*@brief ��������б�
	*/
	void AddExpire(UInt64 playerId, UInt64 leaveTime);

	/**
	*@brief �ӹ����б�ɾ��
	*/
	void DelExpire(UInt64 playerId);

	/**
	*@brief ����Ƿ��뿪
	*/
	bool PlayerIsExpire(UInt64 playerId);

	/**
	*@brief ��ҹ���ʱ��
	*/
	UInt64 PlayerExpireTime(UInt64 playerId);

	/**
	*@brief �������ʱ��
	*/
	UInt64 PlayerLeaveTime(UInt64 playerId);

	/**
	*@brief �㲥��Ϣ
	*/
	void BroadcastToPlayer(Avalon::Packet* packet);
	void BroadcastToPlayer(Avalon::Protocol& protocol);

public:
	void RegisterProtocols();
	void ExecuteProtocol(const CLProtocol::SysTransmitPlayerToTeamCopy& protocol);

private:
	//�����Ϣ��ִ����
	CLParamPacketExecutor<TCSPlayer*>	m_Executor;
	
	// ��ҹ���
	TCSPlayerMap m_PlayerMap;
	TCSPlayerVec m_PlayerVec;
	
	// ��ҹ���ʱ��
	std::map<UInt64, UInt64> m_PlayerExpireMap;

	Avalon::SimpleTimer m_60sTimer;
};

#endif
