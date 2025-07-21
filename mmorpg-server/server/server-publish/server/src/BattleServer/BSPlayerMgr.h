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
	*@brief 增加玩家
	*/
	void AddPlayer(BSPlayer* tcsPlayer);

	/**
	*@brief 查找玩家
	*/
	BSPlayer* FindPlayer(UInt64 playerId);

	/**
	*@brief 删除玩家
	*/
	void RemovePlayer(UInt64 playerId);

public:
	void RegisterProtocols();
	void ExecuteProtocol(CLProtocol::GateTransmitPacket& protocol);

private:
	BSPlayerMap	m_players;

	//玩家消息包执行器
	CLParamPacketExecutor<BSPlayer*>	m_Executor;

	//定时器
	//Avalon::SimpleTimer		m_Timer;
};