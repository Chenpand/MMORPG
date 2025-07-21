#ifndef _GAS_PLAYER_MGR_H_
#define _GAS_PLAYER_MGR_H_

#include <AvalonSingleton.h>
#include <CLPacketExecutor.h>
#include <CLSysProtocol.h>
#include <CLObjectIndex.h>

#include "GASPlayer.h"


class GASPlayerMgr : public Avalon::Singleton<GASPlayerMgr>
{
public:
	GASPlayerMgr();
	~GASPlayerMgr();

public:
	bool Init();
	void Final();

public:
	/**
	*@brief ��ӡ�ɾ�����������
	*/
	bool AddPlayer(GASPlayer* player);
	GASPlayer* FindPlayer(const std::string& name);
	void RemovePlayer(GASPlayer* player);

	void VisitPlayers(GASPlayerVisitor& visitor);

public:
	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time& now);

public:
	void RegisterProtocols();
	void ExecuteProtocol(const CLProtocol::SysTransmitPlayerFormNodesToGlobal& protocol);

private:
	//�����Ϣ��ִ����
	CLParamPacketExecutor<GASPlayer*>	m_Executor;

	// ���Ψһid����
	CLNameObjectIndex<GASPlayer> m_Players;
};

#endif