#ifndef _BS_BATTLE_MGR_H_
#define _BS_BATTLE_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLGameSessionDefine.h>
#include "GameZone.h"
#include "BSBattle.h"
#include "BSTeam.h"
#include "BSRace.h"
#include "BSBaseBattleMgr.h"
#include <CLBattleProtocol.h>


class BSBattleMgr : public BSBaseBattleMgr, public Avalon::Singleton<BSBattleMgr>
{
public:
	typedef std::map<UInt32, BSTeam*> BSTeamMap;

public:
	BSBattleMgr();
	~BSBattleMgr();

	/**
	*@brief �������ID
	*/
	UInt64 GenRaceID();

	/**
	*@brief ����
	*/
	void OnRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const PkRaceEndInfo& raceEndInfo);

	void OnSomeoneDead(UInt32 battleID, UInt64 playerID, UInt64 killerID, UInt32 reason);

public:
	void AddTeam(BSTeam* team);
	void DelTeam(UInt32 teamID);

	void DelBattle(BSBattle* battle);

	BSBattle* GetBattle(UInt32 battleID);

	void DelBattleRace(BattleRace* race);

	BattleRace* GetBattleRace(UInt64 raceID);

public://�¼�

	/**
	*@brief ��ʼ������
	*/
	void Init(UInt32 preBattleCount, UInt32 battleTimeOut);

	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time& now);

	bool OnMatchSuccess(UInt32 battleID, UInt32 dungeonID, VBSTeam teams);

	void OnQueryPlayerRaceInfoRes(UInt64 gamesessionID, UInt64 playerID, UInt8 result, RacePlayerInfo& info);

	void OnRelayServerStartResponse(UInt32 result, UInt64 gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

	UInt32 PkSomeOne(UInt32 battleID, UInt32 dungeonID, UInt64 roleID, UInt64 dstID);

	UInt32 LockSomeOne(UInt32 battleID, UInt64 roleID, UInt64 dstID);

	UInt32 GetPreBattleCount() { return m_PreBattleCount; }

	UInt32 GetBattleTimeOut() { return m_BattleTimeOut; }

	void OnPlayerLeaveBattleScene(CLProtocol::BattleExitSceneRes& protocol);

	void OnPlayerEnterBattleScene(UInt64 playerID, UInt32 sceneNodeID);

	bool PlayerOnline(UInt64 playerID);

	void SetActivityTime(UInt32 startTime, UInt32 endTime);

private:

	void RegistBattleRace(BattleRace* race);

	bool Match(void);

	bool MatchChiji();

	void ClearWaitTeams();

private:
	// ��ʱ����500ms
	Avalon::SimpleTimer	m_TickTimer;

	//����ս��
	std::map<UInt32, BSBattle*> m_Battles;

	//���б���
	std::map<UInt64, BattleRace*> m_BattleRaces;

	//�ȴ�����ս���Ķ���
	BSTeamMap m_WaitTeams;

	// ��������
	UInt32 m_PreBattleCount;

	// ��ʱʱ��
	UInt32 m_BattleTimeOut;

	//������ڳԼ�������
	std::map<UInt64, std::vector<UInt32>> m_PlayerBattleVecMap;

	// ��ʼʱ��
	UInt32 m_StartTime;
	// ����ʱ��
	UInt32 m_EndTime;
	// �ɽ�ʱ��
	UInt32 m_AwardTime;
};


#endif //_BS_BATTLE_MGR_H_
