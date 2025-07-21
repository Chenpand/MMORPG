#ifndef _TCS_TEAM_ONE_H_
#define _TCS_TEAM_ONE_H_

#include <AvalonSimpleTimer.h>
#include "TCSTeam.h"

class TCSTeamOne : public TCSTeam
{
public:
	TCSTeamOne();
	~TCSTeamOne();

	virtual bool OnTick(const Avalon::Time& now);

	/**
	*@brief �����Ŷ�Ŀ��
	*/
	virtual bool SetTeamTarget(UInt32 stage);

	/**
	*@brief С�ӹ�������֪ͨ
	*/
	virtual void SquadAttackDataNotify();

	/**
	*@brief ��ȡ��Ʊ
	*/
	virtual void GetTicket(std::vector<TCConsumeItem>& itemVec);

	/**
	*@brief ������������
	*/
	virtual UInt32 GetGoldMaxNum();
	virtual UInt32 GetGoldNumErrorCode();

	/**
	*@brief ��������֪ͨ
	*/
	virtual void AttackDataNotify(UInt32 stage);
	virtual void AttackDataNotify(TCSPlayer* player);

	/**
	*@brief С��ͨ�ؾݵ�
	*/
	virtual void ClearField(UInt32 squadId, UInt32 fieldId);

	/**
	*@brief ����С���Ѷ�
	*/
	virtual void SetSquadGrade(UInt32 squadId, UInt32 grade);

	/**
	*@brief ����С��Ŀ��
	*/
	virtual void SetSquadTarget(UInt32 squadId, const TeamCopyTargetCfg* targetCfg);

	/**
	*@brief ��Ӿݵ�
	*/
	virtual bool AddField(const TeamCopyFieldDataEntry* fieldEntry);

	/**
	*@brief �ݵ��������
	*/
	virtual void FieldRaceEnd(UInt32 fieldId);

	/**
	*@brief boss�׶�֪ͨ
	*/
	virtual void BossPhaseNotify(TCSPlayer* player);

	/**
	*@brief �ݵ�֪ͨ
	*/
	virtual void FieldNotify(const std::vector<UInt32>& filedVec);
	virtual void FieldNotify(UInt32 fieldId);

	/**
	*@brief ֪ͨս�������ָ�����
	*/
	virtual void NotifyRaceEnergyRevive(UInt64 raceId);

	/**
	*@brief �ϱ��׶�boss��Ϣ
	*/
	virtual void PhaseBossInfo(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate);

	/**
	*@brief ����������
	*/
	virtual void CheckCondFinish();

	/**
	*@brief ɾ��boss�����ݵ�
	*/
	virtual void DelBossAttachField();

	/**
	*@brief ֪ͨ����С���˳����³�
	*/
	virtual void NotifyAllSquadLeaveDungeon();

	/**
	*@brief ֪ͨ����С�ӵ��³Ǳ�����
	*/
	virtual void NotifyAllSquadDungeonDestory(UInt32 dungeonId);

	/**
	*@brief ���Ҿݵ�
	*/
	virtual TeamCopyField* FindField(UInt32 fieldId);

	/**
	*@brief �����Ƿ���
	*/
	virtual bool TeamIsFull();

	/**
	*@brief ������������
	*/
	virtual void StartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res);

	/**
	*@brief ��������
	*/
	virtual void RaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime);

	/**
	*@brief Gmͨ��
	*/
	virtual void GMClearAllField(UInt32 squadId);

	/**
	*@brief ��Ϸ����
	*/
	virtual void GameOver(UInt32 overReason);

	/**
	*@brief �׶�Udp��־
	*/
	virtual void UdpStageEnd(UInt32 costTime, UInt32 result);

	/**
	*@brief �׶�boss����
	*/
	void PhaseBossEnd();

private:

	/**
	*@brief ���ƽ���
	*/
	void _FlopReward(UInt32 stage);

private:

	// �Ŷ�Ŀ��
	TeamCopyTarget m_TeamTarget;

	// С���Ѷ�(key->С��id, val->�Ѷ�)
	std::map<UInt32, UInt32> m_SquadGrade;

	// �ݵ��б�
	std::map<UInt32, TeamCopyField> m_FieldMap;

	// boss�׶�
	TeamCopyBossPhase m_BossPhase;

	// 1�룬�����ָ���ʱ��
	Avalon::SimpleTimer m_TickTimers;
};


#endif // !_TCS_TEAM_ONE_H_

