#ifndef _BS_LOST_BATTLE_MGR_H_
#define _BS_LOST_BATTLE_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLGameSessionDefine.h>
#include "GameZone.h"
#include "BSLostBattle.h"
#include "BSLostPKRace.h"
#include "BSBaseBattleMgr.h"

#include <CLBattleProtocol.h>

class BSLostBattleMgr : public BSBaseBattleMgr, public Avalon::Singleton<BSLostBattleMgr>
{
public:
	BSLostBattleMgr();
	~BSLostBattleMgr();

public:
	virtual bool AddBattle(BSBaseBattle* battle);

	BSLostBattle* FindLostBattle(UInt32 battleId);

	virtual void DestoryBattleImp(BSBaseBattle* battle);

	/**
	*@brief 处理活动开始
	*/
	void OnActOpen();

	bool  IsOpen() { return m_isOpen; }
	/** 
	*@brief 处理活动结束
	*/
	void OnActClose();

	virtual void OnTickImp(const Avalon::Time& now);

	BSLostBattle*	SelectOneBattle(UInt32 floor);
	BSLostBattle*   GetPlayerLeastBattle(UInt32 floor);
	void CreateOneBattleDynamic(UInt32 floor);
	virtual void SetActivityTime(UInt32 preTime, UInt32 startTime, UInt32 endTime);

private:
	//各层生成的战场<floor, 每层战场集合>
	std::map<UInt32, std::vector<BSLostBattle*>>	m_lostBattles;
	
	//活动是否开启
	bool			m_isOpen;
	
	// 定时器，1s
	Avalon::SimpleTimer	m_CheckTimer;
};

#endif