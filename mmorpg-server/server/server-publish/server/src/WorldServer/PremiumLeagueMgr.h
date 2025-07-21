#ifndef __PREMIUM_LEAGUE_H__
#define __PREMIUM_LEAGUE_H__

#include <functional>
#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLPremiumLeagueDefine.h>
#include <CLGameSessionProtocol.h>
#include <AvalonSimpleTimer.h>
#include <AvalonDailyTimer.h>

#include "PremiumLeagueGamer.h"
#include "BattleRecordMgr.h"
#include "PremiumLeagueBattle.h"

#define PARAM_NAME_PREMIUM_LEAGUE_STATUS "pl_status"

// �ͽ�����ʱ���
struct PremiumLeaguePoint
{
	// ״̬
	PremiumLeagueStatus			status;
	// ��ʼʱ��
	UInt32						startTime;
	// ����ʱ��
	UInt32						endTime;
};

typedef std::function<void()> PremiumLeaguePointFunc;
// �ͽ������ص�
struct PremiumLeagueStatusCallback
{
	PremiumLeaguePointFunc		onStart;
	PremiumLeaguePointFunc		onEnd;
};
typedef std::map<PremiumLeagueStatus, PremiumLeagueStatusCallback> MapPremiumLeagueStatusCallback;

class PremiumLeagueMgr : public Avalon::Singleton<PremiumLeagueMgr>
{
public:
	// ��̭���������
	static const UInt32 FINAL_BATTLE_GAMER_NUM = 8;

	PremiumLeagueMgr();
	~PremiumLeagueMgr();

	/**
	*@brief ��ʼ��
	*/
	bool OnInit();

	/**
	*@brief �����������
	*/
	void OnLoadFinish();

	/**
	*@brief �����µ�һ��
	*/
	void OnDayChanged();

	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief �˳�
	*/
	void OnFinal();

public:
	// ���ݿⷵ��

	/**
	*@brief ���ز�����Ա��Ϣ����
	*/
	bool OnSelectGamerRet(CLRecordCallback* callback);

	/**
	*@brief ������̭����Ϣ����
	*/
	bool OnSelectBattleRet(CLRecordCallback* callback);

public:
	/**
	*@brief ״̬
	*/
	PremiumLeagueStatus GetStatus() const { return m_Status; }
	void SetStatus(PremiumLeagueStatus status) { m_Status = status; }
	PremiumLeagueStatusInfo GetStatusInfo() const;

	/**
	*@brief ս����¼
	*/
	BattleRecordMgr& GetBattleRecordMgr() { return m_BattleRecord; }

	/**
	*@brief ��ȡ�����
	*/
	UInt32 GetRewardPool();

	/**
	*@brief ͬ������أ����player�ǿվ��ǹ㲥���������
	*/
	void SyncRewardPool(WSPlayer* player);

	/**
	*@brief ��ȡ��Ԥѡ����������
	*/
	UInt32 GetTotalPreliminayRewardNum() const { return m_TotalPreliminayRewardNum; }
	void SetTotalPreliminayRewardNum(UInt32 num) { m_TotalPreliminayRewardNum = num; }
	void AddTotalPreliminayRewardNum(UInt32 num) { m_TotalPreliminayRewardNum = m_TotalPreliminayRewardNum + num; }

	/**
	*@brief ��ȡ8ǿ���
	*/
	std::vector<PremiumLeagueBattleGamer> GetFinalBattleGamer() const;

	/**
	*@brief �㲥���������
	*/
	// ������Ҫ֧���·���Packet
	// ToDo...
	void Broadcast(Avalon::Protocol& protocol);

public:
	// ��Ҳ���

	/**
	*@brief �������
	*/
	void OnPlayerOnline(WSPlayer* player);

	/**
	@brief �������
	*/
	void OnPlayerOffline(WSPlayer* player);

	/**
	*@brief ����
	*/
	UInt32 OnPlayerEnroll(WSPlayer* player);

	/**
	*@brief ������±���
	*/
	UInt32 OnPlayerReEnroll(WSPlayer* player);

	/**
	*@brief �������ʼԤ����
	*/
	UInt32 OnPlayerStartPreliminay(WSPlayer* player);

public:
	// ս�����

	/**
	*@brief Ԥѡ������
	*/
	void OnPreliminayRaceEnd(CLProtocol::MatchServerPremiumLeagueRaceEnd raceEnd);

	/**
	*@brief ��̭��
	*/
	void OnBattleRaceEnd(CLProtocol::MatchServerPremiumLeagueRaceEnd raceEnd);

	/**
	 *@brief ����������scene
	 */
	void SendHonorToScene(UInt64 roleId, UInt32 pvpType, UInt32 honor);

public:
	// �������
	/**
	*@brief ����
	*/
	UInt32 OnCheckPlayerEnrollRet(WSPlayer* player);

	/**
	*@brief ���±���
	*/
	UInt32 OnCheckPlayerReEnrollRet(WSPlayer* player);

public:
	/**
	*@brief �л�״̬
	*/
	void ChangeStatus(PremiumLeagueStatus status);

public:
	/**
	*@brief ������Ա���
	*/
	PremiumLeagueGamer* FindGamer(ObjID_t id);
	void AddGamer(PremiumLeagueGamer* gamer);
	void DelGamer(ObjID_t id);

private:
	// ����״̬�Ļص�

	/**
	*@brief ע��״̬�ص�
	*/
	void _RegisterStatusCallback(PremiumLeagueStatus status, PremiumLeaguePointFunc onStartFunc, PremiumLeaguePointFunc onEndFunc);

	/**
	*@brief ��Ч�Ļص�
	*/
	void _OnNousefulStatusCallback();

	/**
	*@brief ������ʼ
	*/
	void _OnEnrollStart();

	/**
	*@brief ��������
	*/
	void _OnEnrollEnd();

	/**
	*@brief Ԥѡ����ʼ
	*/
	void _OnPreliminayStart();

	/**
	*@brief Ԥѡ������
	*/
	void _OnPreliminayEnd();

	/**
	*@brief 8ǿ��ʼ
	*/
	void _OnFinalEigthStart();

	/**
	*@brief 8ǿ����
	*/
	void _OnFinalEigthEnd();

	/**
	*@brief 4ǿ��ʼ
	*/
	void _OnFinalFourStart();

	/**
	*@brief 4ǿ��ʼ
	*/
	void _OnFinalFourEnd();

	/**
	*@brief ������ʼ
	*/
	void _OnFinalBattleStart();

	/**
	*@brief ��������
	*/
	void _OnFinalBattleEnd();

	/**
	*@brief ����״̬��������
	*/
	void _OnAllOver();

public:
	// ��̭��ս�����
	ObjID_t AddNewBattle(PremiumLeagueGamer* gamer1, PremiumLeagueGamer* gamer2);
	void AddBattle(const PremiumLeagueBattle& battle);
	PremiumLeagueBattle* FindBattleByGuid(ObjID_t guid);
	PremiumLeagueBattle* FindBattleByRoleAndStatus(ObjID_t roleId, PremiumLeagueStatus status);
	void ClearBattle();

private:
	/**
	*@brief ������һ�ֵĲ�����Ա
	*/
	void _CountNextRollGamer(UInt32 maxNum);

	/**
	*@brief ��ʼ��ǰ�ֵ�ս��
	*/
	void _StartBattle(UInt32 maxNum);

	/**
	*@brief �Ƿ��Ѿ���ɵ�ǰ�ֵ�����ս��
	*/
	bool _IsFinishCurrentRollBattle();

	/**
	*@brief ������һ��׼���Ĺ���
	*/
	void _SendNextRollPrepareAnnouncement();

private:
	/**
	*@brief �������ʱ���
	*/
	void _CountStatusPoint();

	/**
	*@brief ���㵱ǰʱ���
	*/
	PremiumLeagueStatus _CountCurrentPoint();

	/**
	*@brief ����ܷ���
	*/
	UInt32 _CheckPlayerCanEnroll(WSPlayer* player);

	/**
	*@brief ����ܷ����±���
	*/
	UInt32 _CheckPlayerCanReEnroll(WSPlayer* player);

	/**
	*@brief ������м�¼
	*/
	void _Clear();

	/**
	*@brief ������в�����
	*/
	void _ClearGamers();

	/**
	*@brief ������������ȡ8ǿλ��
	*/
	UInt32 _GetFinalEightPosByRanking(UInt32 ranking);

	/**
	*@brief ����ܷ�μӱ���
	*/
	bool _CanPlayerStartBattle(ObjID_t roleId);

	/**
	*@brief �����������Ž���
	*/
	void _SendReward(PremiumLeagueGamer* gamer, PremiumLeagueRewardType type);

	/**
	*@brief �����������Ž���
	*/
	UInt32 _GetReward(PremiumLeagueRewardType type);

	/**
	*@brief �����ȡ�Ļ���
	*/
	UInt32 _CountWinGotScore(UInt32 winnerWinNum, UInt32 loserWinNum);

	/**
	*@brief �����ȡ�Ļ���
	*/
	UInt32 _CountLoseGotScore();

	/**
	 *@brief ������������
	 */
	void _SendHonorReward(UInt32 pvpType, UInt64 roleId, UInt32 rank);

private:
	// �Ƿ�������
	bool										m_LoadFinish;
	// ��ǰ״̬
	PremiumLeagueStatus							m_Status;
	// ����ʱ���
	PremiumLeaguePoint							m_TimePoints[PLS_NUM];
	// ״̬�ص�
	MapPremiumLeagueStatusCallback				m_StatusCallbacks;
	// ���б�������
	std::map<ObjID_t, PremiumLeagueGamer*>		m_Gamers;
	// 8ǿ���
	PremiumLeagueGamer*							m_FinalBattleGamer[FINAL_BATTLE_GAMER_NUM];
	// ���涨ʱ��
	Avalon::SimpleTimer							m_SaveTimer;
	// ս����¼
	BattleRecordMgr								m_BattleRecord;
	// ��̭��ս��
	std::vector<PremiumLeagueBattle>			m_PremiumBattles;
	// ÿ�ն�ʱ��
	Avalon::DailyTimer							m_DailyTimer;
	// �Ѿ������ȥ��Ԥѡ������
	UInt32										m_TotalPreliminayRewardNum;
}; 

#define REGISTER_STATUS_CALLBACK(status, startFunc, endFunc)		\
		do {														\
			PremiumLeaguePointFunc _startFunc;						\
			if (startFunc)											\
			{														\
				_startFunc = std::bind(startFunc, this);            \
			}														\
			PremiumLeaguePointFunc _endFunc;						\
if (endFunc)											\
{														\
	_endFunc = std::bind(endFunc, this);            \
}\
			_RegisterStatusCallback(status, _startFunc, _endFunc);  \
		} while (0)

#endif