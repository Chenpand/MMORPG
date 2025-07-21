#ifndef _BS_BATTLE_H_
#define _BS_BATTLE_H_
#include <CLBattleProtocol.h>
#include "BSBaseBattle.h"
#include "BSTeam.h"

class BSBattle : public BSBaseBattle
{
public:
	typedef std::list<BSTeam*> BSBattleTeamList;

public:
	BSBattle();
	~BSBattle();

public:
	void AddTeam(BSTeam* team);

	UInt32 PkSomeOne(UInt64 roleId, UInt64 dstId, UInt32 dungeonID);

	UInt32 LockSomeOne(UInt64 roleId, UInt64 dstId);

	void NotifySwitchBattleScene();

	void NotifyCreateSceneBattle();

	void OnRaceEnd();

	void OnSomeoneDead(UInt64 playerID, UInt64 killerID, UInt32 reason);

	void OnBattleTimeOut();

	UInt32 GetPlayerNum();

	UInt32 GetSurvivalNum();

	BSTeamMember* GetPlayer(UInt64 playerID);

	void NotifyMsgToAllPlayer(Avalon::Protocol& protocol, bool includeDead=false);

	void ScoreSortList(BSTeamMember* member, UInt32 score);

	void UpdatePlayerState(UInt64 roleID, UInt32 playerState);

	bool CheckNoWarCD(UInt64 roleID, UInt64 dstID);

	void AddNoWarCD(UInt64 roleID, UInt64 dstID);

	UInt32 CheckIsCanPk(UInt64 roleID, UInt64 dstID);

public:
	void OnTick(const Avalon::Time& now);

protected:

	void SendBalanceLog(const CLProtocol::SceneBattleBalanceEnd& notify, UInt64 playerID, UInt32 reason);

private:
	//进入的队伍
	BSBattleTeamList m_BattleTeams;
	//第几波
	UInt32 m_waveID;
	//商店波次
	UInt32 m_shopWave;
	//是否可以PK
	bool m_canPK;
};


#endif //_BS_BATTLE_H_
