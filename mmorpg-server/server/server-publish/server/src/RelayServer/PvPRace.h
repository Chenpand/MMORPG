#ifndef __PVP_RACE_H__
#define __PVP_RACE_H__

#include "Race.h"
#include <CLMatchDefine.h>
#include <CLReplayDefine.h>

struct ReplayInfo;
class PvPRace : public Race
{
    AVALON_DEFINE_OBJECTPOOL(PvPRace, Avalon::NullMutex)
public:
	PvPRace(RaceType raceType, RaceInstanceType subType = RACE_PVP);
    virtual ~PvPRace();

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
	*@brief ս����־
	*/
	UInt64 GetBattleFlag() const { return m_battleFlag; }
	void SetBattleFlag(UInt64 flag) { m_battleFlag = flag; }

	/**
	*@brief ս�������Ϣ
	*/
	void SetRacePlayerInfo(std::vector<RacePlayerInfo>& infoes) { m_racePlayerInfoes = infoes; }
	const std::vector<RacePlayerInfo>& GetRacePlayerInfo() const { return m_racePlayerInfoes; }

    /**
    *@brief ע��״̬��ʱ��
    */
    virtual void RegisterStatusTimers();

	/**
	*@brief ����������ʱ��(s)
	*/
	virtual UInt32 GetRaceMaxTime() { return 10 * 60; }

	/**
	*@brief ¼������
	*/
	UInt32 GetReplayScore() { return m_replayScore; }

	/**
	*@brief ����¼���ļ�
	*/
	ReplayFile GenReplayFile();

public:
    /**
    *@brief �������
    */
    virtual void OnLoadFinish();

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
	*@brief ������Ϸ���
	*/
	virtual void CheckRaceEnd(EGameSessionDismissType reason);

	/**
	*@brief ��ɢ
	*/
	virtual bool Dismiss(EGameSessionDismissType reason);

public:
    /**
    *@brief �յ���ҵĽ�����Ϣ
    */
    void OnRecvRaceEndMsg(RLPlayer* player, CLProtocol::RelaySvrEndGameReq& req);

protected:
    // ��״̬��ʱ������

	/**
	*@brief �Ƿ���Ҫ��֤
	*/
	virtual bool _IsNeedVerify();

    /**
    *@brief ��¼��ʱ
    */
    virtual void _OnLoginTimeout();

	/**
	*@brief ���س�ʱ
	*/
	virtual void _OnLoadTimeout();

    /**
    *@brief ���㳬ʱ
    */
	virtual void _OnRaceEndTimeout();

	/**
	*@brief �ȴ����������ʱ
	*/
	virtual void _OnWaitReconnectTimeout();

	/**
	*@brief ��Ҷ���������ʱ
	*/
	virtual void OnGamerReconnectTimeout(ObjID_t id);

protected:
    /**
    *@brief ��ȡ������Ϣ
    */
    RaceGamer* _GetOpponent(ObjID_t gamerId);

	/**
	*@brief ��ȡ¼����Ϣ
	*/
	virtual bool _GetReplayInfo(ReplayInfo& info);

	/**
	*@brief ��ȡ�������
	*/
	PkRaceResult _GetPkRaceResultByFlag(UInt8 flag);

	/**
	*@brief �Ƿ���Ҫ��ɢ
	*/
	virtual bool _IsNeedDismiss();

protected:
	// PK����
	PkType						m_pkType;
	// pk����id
	UInt32						m_pkDungeonId;
	// ս�����
	UInt64						m_battleFlag;
	// ������ҵ�ս����Ϣ
	std::vector<RacePlayerInfo>	m_racePlayerInfoes;
	// ¼������
	UInt32						m_replayScore;
};

#endif