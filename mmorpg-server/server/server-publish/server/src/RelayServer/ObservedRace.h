#ifndef __OBSERVED_RACE_H__
#define __OBSERVED_RACE_H__

#include "Race.h"
#include <CLMatchDefine.h>
#include <CLReplayDefine.h>
#include <AvalonSimpleTimer.h>

struct ReplayInfo;


class ObservedRace : public Race
{
	AVALON_DEFINE_OBJECTPOOL(ObservedRace, Avalon::NullMutex)
public:
	ObservedRace(RaceType raceType, RaceInstanceType subType = RACE_PVP);
	virtual ~ObservedRace();


	virtual void Init();

	virtual void OnTick(const Avalon::Time& now);

	/**
	*@brief pk����
	*/
	PkType GetPkType() const { return m_pkType; }
	void SetPkType(PkType type) { m_pkType = type; }

	/**
	*@brief pk����
	*/
	UInt32 GetPkDungeonID() const { return m_pkDungeonId; }
	void SetPkDungeonID(UInt32 id) { m_pkDungeonId = id; }

	/**
	*@brief ս������
	*/
	virtual void OnRaceEnd();

	/**
	*@brief ����������ʱ��
	*/
	virtual UInt32 GetRaceMaxTime();

	/**
	*@brief ���ͽ�����Ϣ��MatchServer
	*/
	virtual void SendRaceEndToMatchServer(EGameSessionDismissType reason) {};

	/**
	*@brief ������Ϸ���
	*/
	virtual void CheckRaceEnd(EGameSessionDismissType reason) {};

	/**
	*@brief ����Observeһ������
	*/
	void CancelObserveOverload();

	/**
	*@brief �յ�Observe����ʼͬ��֡
	*/
	virtual void OnObserveRealyAddObservePlayer(UInt64 roleID);

	/**
	*@brief ��ҿ�ʼ��ս
	*/
	virtual UInt32 OnObserverLogin(RLPlayer* player, UInt32 startFrame);

	/**
	*@brief �յ�observeͬ������֡
	*/
	virtual void OnReceiveObserveFrames(std::vector<CLProtocol::Frame>& frames);

	/**
	*@brief ��ȡ����������ս��ʱ����������
	*/
	virtual UInt32 GetObserveLoad();

protected:
	// PK����
	PkType						m_pkType;
	// pk����id
	UInt32						m_pkDungeonId;
	// ������ҵ�ս����Ϣ
	std::vector<RacePlayerInfo>	m_racePlayerInfoes;
	// �ȴ���ս������ӵĶ�ʱ��
	std::map<UInt64, Avalon::SimpleTimer> m_ObserveTimerMap;

};

#endif