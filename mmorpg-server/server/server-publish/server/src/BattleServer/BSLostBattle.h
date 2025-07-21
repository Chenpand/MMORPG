#pragma once

#include <CLDefine.h>
#include <CLLostDungeonDefine.h>
#include <AvalonSimpleTimer.h>
#include "BSBaseBattle.h"

struct RacePlayerInfo;
class BSPlayer;
class LostDungeonBattleDataEntry;

class BSLostBattle : public BSBaseBattle
{
public:
	BSLostBattle();
	~BSLostBattle();

	void	SetFloor(UInt32 floor) { m_floorNum = floor; }
	UInt32	GetFloor() { return m_floorNum; }
	
	void	InitBattle(LostDungeonBattleDataEntry* battleData);

	void    SetBattleDataId(UInt32 battleDataId) { m_battleDataId = battleDataId; }
	UInt32  GetBattleDataId() { return m_battleDataId; }

	UInt32	GetPlayerNum();
	inline UInt32  GetPlayerLimit() { return m_playerLimit; }
	bool   IsFull();

	virtual bool IsTimeToDestory(const Avalon::Time& now);

	virtual void NotifyCreateSceneBattle();
	virtual void NotifyCloseSceneBattle(UInt8 reason);

	void  OnPlayerBeginMatchBattle(ObjID_t roleId);

	virtual bool OnPlayerBeginEnterBattle(UInt32 accId, ObjID_t roleId, UInt32 sourceSceneNodeId,
		UInt32 dstSceneNodeId, RacePlayerInfo& racePlayerInfo);

	virtual void OnPlayerEnterBattle(BSPlayer* player);

	virtual void OnPlayerLeaveBattle(BSPlayer* player);

	virtual UInt32 PkPlayer(UInt64 roleId, BSPlayer* attacker, BSPlayer* attackeder, UInt32 dungeonID);

	virtual void OnTick(const Avalon::Time& now);

	void OnSceneBattleDestoryed();
private:
	//����¥��
	UInt32			m_floorNum;
	//ս����id
	UInt32			m_battleDataId;

	//��������
	UInt32			m_playerLimit;

	//ս�������
	std::set<UInt64> playerSet;

	//���ڽ������� 
	std::map<UInt64, WaitEnterBatPlayer> waitPlayers;
	
	// ս���رձ��׶�ʱ��
	Avalon::SimpleTimer m_DestroyBattleTimer;
};