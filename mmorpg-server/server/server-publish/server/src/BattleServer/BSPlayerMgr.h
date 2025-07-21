#pragma once
#include <AvalonSingleton.h>
#include <CLPacketExecutor.h>
#include <CLDefine.h>
#include "BSPlayer.h"

namespace CLProtocol
{
	class GateTransmitPacket;
}

class BSPlayerMgr : public Avalon::Singleton<BSPlayerMgr>
{
	typedef HashMap<UInt64, BSPlayer*> BSPlayerMap;
	typedef std::vector<BSPlayer*> BSPlayerVec;

public:
	BSPlayerMgr();
	~BSPlayerMgr();

	/**
	*@brief tick
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief �������
	*/
	void AddPlayer(BSPlayer* tcsPlayer);

	/**
	*@brief �������
	*/
	BSPlayer* FindPlayer(UInt64 playerId);

	/**
	*@brief ɾ�����
	*/
	void RemovePlayer(UInt64 playerId);

public:
	void RegisterProtocols();
	void ExecuteProtocol(CLProtocol::GateTransmitPacket& protocol);

private:
	BSPlayerMap	m_players;

	//�����Ϣ��ִ����
	CLParamPacketExecutor<BSPlayer*>	m_Executor;

	//��ʱ��
	//Avalon::SimpleTimer		m_Timer;
};