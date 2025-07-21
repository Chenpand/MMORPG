#pragma once

#include "BSBaseRace.h"

class BSPlayer;
class BSLostBattle;
//迷失地牢PK比赛对象
class BSLostPKRace : public BSBaseRace
{
public:
	BSLostPKRace();
	BSLostPKRace(UInt8 raceType);
	~BSLostPKRace();

	void OnCreate(UInt64 attacker,  UInt64 attackeder);

	inline void SetLostBattle(BSLostBattle*  battle) { m_ownerBattle = battle; }
	BSLostBattle* GetLostBattle();

	UInt32 GetAttackerHpBack();
	void SetAttackerHpBack(UInt32 hp);
	UInt32 GetAttackerMpBack();
	void SetAttackerMpBack(UInt32 mp);

	UInt32 GetAttackederHpBack();
	void SetAttackederHpBack(UInt32 hp);
	UInt32 GetAttackederMpBack();
	void SetAttackederMpBack(UInt32 mp);

	void GetAllPlayers(std::vector<BSPlayer*>& players);
	BSPlayer* FindPlayer(UInt64 playerid);
	
	virtual UInt32 StartRace();
	virtual void OnRelayStartRaceRes(UInt32 result, UInt64 gamesessionID, UInt32 abality, const Avalon::SockAddr& address);
	virtual void OnStartRaceFail();
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const PkRaceEndInfo& raceEndInfo);
	virtual BSBaseBattle* GetBattle();

	
private:
	UInt64  m_attacker; //攻击者id
	UInt64	m_attackeder;	//被攻击者id
	BSLostBattle*  m_ownerBattle; //所在战场
	UInt32  m_attackerHpBack;
	UInt32  m_attackerMpBack;
	UInt32  m_attackederHpBack;
	UInt32  m_attackederMpBack;
};