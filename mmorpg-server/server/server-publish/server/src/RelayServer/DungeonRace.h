#ifndef __DUNGEON_RACE_H__
#define __DUNGEON_RACE_H__

#include "Race.h"

class DungeonRace : public Race
{
    AVALON_DEFINE_OBJECTPOOL(DungeonRace, Avalon::NullMutex)
public:
	DungeonRace(RaceType type);
    virtual ~DungeonRace();

    /**
    *@brief ע��״̬��ʱ��
    */
    virtual void RegisterStatusTimers();

	/**
	*@brief ����������ʱ��(s)
	*/
	virtual UInt32 GetRaceMaxTime() { return 60 * 60; }

	/**
	*@brief �Ƿ�ʹ����֤����������
	*/
	bool IsUseVerifyRaceEnd() const { return m_UseVerifyRaceEnd; }
	void UseVerifyRaceEnd() { m_UseVerifyRaceEnd = true; }

public:
    /**
    *@brief ս������
    */
    virtual void OnRaceEnd();

    /**
    *@brief ���ͽ�����Ϣ��MatchServer
    */
    virtual void SendRaceEndToMatchServer(EGameSessionDismissType reason);

    /**
    *@brief ���ͽ�����Ϣ�����
    */
    virtual void SendRaceEndToPlayer(EGameSessionDismissType reason);

	/**
	*@brief ��ɢ
	*/
	virtual bool Dismiss(EGameSessionDismissType reason);

	/**
	*@brief ������Ϸ���
	*/
	virtual void CheckRaceEnd(EGameSessionDismissType reason);

	/**
	*@brief �յ���ʼ����֮��Ĵ���
	*/
	virtual void OnRecvInitPacket();

public:
    /**
    *@brief �յ���ҵĽ�����Ϣ
    */
    void OnRecvRaceEndMsg(RLPlayer* player, CLProtocol::RelaySvrDungeonRaceEndReq& req);

private:
    // ��״̬��ʱ������

    /**
    *@brief ��¼��ʱ
    */
    void _OnLoginTimeout();

	/**
	*@brief �ȴ����������ʱ
	*/
	void _OnWaitReconnectTimeout();

	/**
	*@brief �ȴ�����˳�
	*/
	void _OnWiatGamerLeaveRaceTimeout();

	/**
	*@brief �ȴ�����˳�
	*/
	void _OnWiatRaceEnd();
private:
	/**
	*@brief �Ƿ���Ҫ��֤
	*/
	virtual bool _IsNeedVerify();

	/**
	*@brief �Ƿ�crossDungeon
	*/
	bool _IsCrossDungeon();

	/**
	*@brief �Ƿ��յ����еĽ���
	*/
	virtual bool _IsRecvAllRaceEnd();

protected:
	/**
	*@brief  ����һ��InputData
	*/
	bool _HandleOneInputData(RLPlayer* player, CLProtocol::InputData& input);

	/**
	*@brief  ����ؿ�boss�׶α仯
	*/
	void _OnBossPhaseChange(UInt32 bossPhase);

	/**
	*@brief ֪ͨ�ݵ㱻����
	*/
	void OnNotifyDestoryDungeon(ObjID_t destoryDungeonId);

	/**
	*@brief �߼���������
	*/
	void OnLogicEndDungeon(UInt32 reason);

	/**
	*@brief ��ʼУ��֡����
	*/
	virtual void _OnFrameChecksum(UInt32 frame);

	/**
	*@brief  ��������������һ��
	*/
	virtual void _OnPlayerChecksumUnsync(RaceGamer& gamer, UInt32 frame, UInt32 finalChecksum);

	/**
	*@brief  �ű�֪ͨ���³Ǳ�ϣĦ˹֮�������������
	*/
	virtual void OnTeamCopyNotifyBimsEnergyProgress(UInt32 progress, UInt32 dungeonId);

	/**
	*@brief  �ű����Ӹ������
	*/
	virtual void OnAddReviveCnt(UInt32 reviveCnt);

private:
	// ���³�ID
	UInt32				m_DungeonId;
	// boss�׶�
	UInt32				m_BossPhase; 
	// ��ʼ�ȴ�����ʱ��
	UInt32				m_StartWaitVerifyRaceEndTime;
	// �Ƿ�ʹ����֤�������Ľ���
	bool				m_UseVerifyRaceEnd;
};

#endif