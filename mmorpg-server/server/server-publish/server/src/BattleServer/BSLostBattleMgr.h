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
	*@brief ������ʼ
	*/
	void OnActOpen();

	bool  IsOpen() { return m_isOpen; }
	/** 
	*@brief ��������
	*/
	void OnActClose();

	virtual void OnTickImp(const Avalon::Time& now);

	BSLostBattle*	SelectOneBattle(UInt32 floor);
	BSLostBattle*   GetPlayerLeastBattle(UInt32 floor);
	void CreateOneBattleDynamic(UInt32 floor);
	virtual void SetActivityTime(UInt32 preTime, UInt32 startTime, UInt32 endTime);

private:
	//�������ɵ�ս��<floor, ÿ��ս������>
	std::map<UInt32, std::vector<BSLostBattle*>>	m_lostBattles;
	
	//��Ƿ���
	bool			m_isOpen;
	
	// ��ʱ����1s
	Avalon::SimpleTimer	m_CheckTimer;
};

#endif