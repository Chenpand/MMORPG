#ifndef _SCORE_WAR_2V2_MGR_H_
#define _SCORE_WAR_2V2_MGR_H_

#include <AvalonSingleton.h>
#include <CL2V2ScoreWarDefine.h>

class Player;

class ScoreWar2V2Mgr : public Avalon::Singleton<ScoreWar2V2Mgr>
{
public:
	ScoreWar2V2Mgr() :m_Status(ScoreWar2V2Status::SWS_2V2_INVALID), m_StatusEndTime(0), m_StartTime(0), m_IsSync(false), m_IsInit(false), m_LastInitTime(0){}
	~ScoreWar2V2Mgr(){}

	bool Init();

	inline ScoreWar2V2Status GetScoreWarStatus() { return m_Status; }
	inline void SetScoreWarStatus(ScoreWar2V2Status status) { m_Status = status; }

	inline UInt32 GetStatusEndTime() { return m_StatusEndTime; }
	inline void SetStatusEndTime(UInt32 statusEndTime) { m_StatusEndTime = statusEndTime; }

	inline UInt32 GetStartTime() { return m_StartTime; }
	inline void SetStartTime(UInt32 startTime) { m_StartTime = startTime; }

public:

	void OnTick(Avalon::Time now);

	void OnCrossSyncScoreWarInfo(ScoreWar2V2Status status, UInt32 statusEndTime, UInt32 startTime);

	void SyncScoreWarInfo(Player* player);

	void SyncScoreWarBattleInfo(Player* player);

	//�Ƿ���ս���ڼ�
	bool IsBattle();

	bool IsMatch();

private:
	//����������״̬
	ScoreWar2V2Status		m_Status;

	//���׶ν���ʱ��
	UInt32				m_StatusEndTime;

	//��ʼʱ��
	UInt32				m_StartTime;

	//�Ƿ����ͬ��
	bool				m_IsSync;

	//�Ƿ���ɳ�ʼ��
	bool				m_IsInit;

	//�Ƿ���ɳ�ʼ��
	UInt32				m_LastInitTime;
};

#endif