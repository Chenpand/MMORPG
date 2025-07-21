#ifndef __PREMIUM_LEAGUE_GAMER_H__
#define __PREMIUM_LEAGUE_GAMER_H__

/*
	�ͽ�����������
*/

#include <CLDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLPremiumLeagueDefine.h>
#include "PremiumLeagueBattle.h"

class WSPlayer;
class PremiumLeagueGamer : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(PremiumLeagueGamer, Avalon::NullMutex)
public:
	PremiumLeagueGamer();
	~PremiumLeagueGamer();

	bool Init(WSPlayer* player);

	void Clear();

	/**
	*@brief ͬ��������Ϣ
	*/
	void SyncSelfInfo();

	/**
	*@brief ��Ҷ���
	*/
	void SetPlayer(WSPlayer* player) { m_Player = player; }
	WSPlayer* GetPlayer() const { return m_Player; }

	/**
	*@brief �Ƿ��Ѿ�����
	*/
	bool IsLose() const { return m_RemainHP == 0; }

	/**
	*@brief ����
	*/
	UInt32 GetScore() const { return m_Score; }
	void SetScore(UInt32 score);

	/**
	*@brief ʣ��Ѫ��
	*/
	UInt32 GetRemainHp() const { return m_RemainHP; }
	void SetRemainHp(UInt32 hp);

	/**
	*@brief ʣ������
	*/
	UInt32 GetRemainMp() const { return m_RemainMP; }
	void SetRemainMp(UInt32 mp) { m_RemainMP = mp; }

	/**
	*@brief ʤ����
	*/
	UInt32 GetWinNum() const { return m_WinNum; }
	void SetWinNum(UInt32 num);

	/**
	*@brief ��������
	*/
	UInt32 GetEnrollCount() const { return m_EnrollCount; }
	void SetEnrollCount(UInt32 count);

	/**
	*@brief ����ʱ��
	*/
	UInt32 GetEnrollTime() const { return m_EnrollTime; }
	void SetEnrollTime(UInt32 time) { m_EnrollTime = time; }

	/**
	*@brief ��ȡ��̭��ʤ����
	*/
	UInt32 GetFinalBattleWinNum() const { return m_FinalBattleWinNum; }
	void SetFinalBattleWinNum(UInt32 num) { m_FinalBattleWinNum = num; }

	/**
	*@brief ��ȡ��Ԥѡ����������
	*/
	UInt32 GetPreliminayRewardNum() const { return m_PreliminayRewardNum; }
	void SetPreliminayRewardNumNum(UInt32 num) { m_PreliminayRewardNum = num; }
	void AddPreliminayRewardNumNum(UInt32 num) { m_PreliminayRewardNum = m_PreliminayRewardNum + num; }

	/**
	*@brief ʤ��ʱ��
	*/
	UInt32 GetWinTime() const { return m_WinTime; }
	void SetWinTime(UInt32 time) { m_WinTime = time; }

	/**
	*@brief ����
	*/
	UInt32 GetRanking() const { return m_Ranking; }
	void SetRanking(UInt32 ranking);

	/**
	*@brief ��һ�ֵ�λ��
	*/
	UInt32 GetNextRollPos() const { return m_NextRollPos; }
	void SetNextRollPos(UInt32 pos) { m_NextRollPos = pos; }

	/**
	*@brief ����
	*/
	UInt8 GetBattleFlag() const { return m_BattleFlag; }
	void SetBattleFlag(UInt8 flag) { m_BattleFlag = flag; }

	/**
	*@brief ����
	*/
	bool IsFinishRollBattle() const { return m_IsFinishRollBattle != 0; }
	void SetFinishRollBattle(bool finish) { m_IsFinishRollBattle = finish ? 1 : 0; }

	/**
	*@brief �Ƿ���
	*/
	bool IsDirty() const { return m_Dirty; }
	void SetDirty(bool dirty) { m_Dirty = dirty; }

	/**
	*@brief ְҵ
	*/
	UInt8 GetOccu() const { return m_Occu; }

	/**
	*@brief �ȼ�
	*/
	UInt16 GetLevel() const { return m_Level; }

	/**
	*@brief ��ȡ��̭�������Ϣ
	*/
	PremiumLeagueBattleGamer GetBattleGamerInfo() const;

	/**
	*@brief ��ȡ��̭�������Ϣ
	*/
	PremiumLeagueBattleFighter GetBattleFighterInfo() const;

	/**
	*@brief �ܷ�μӱ���
	*/
	bool CanStartBattle();

public:
	/**
	*@brief �������
	*/
	void OnEnroll();

	/**
	*@brief �������а�
	*/
	void UpdateSortList();

	/**
	*@brief 8ǿ��ʤ
	*/
	void OnBattleWin();

	/**
	*@brief 8ǿ��ʤ
	*/
	void OnBattleLose();

public:
	/**
	*@brief �������
	*/
	void SendProtocol(Avalon::Protocol& protocol);

public:
	// ���ݿ����
	void SaveToDB();
	void InsertToDB();
	void DelFromDB();

public:
	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("occu", m_Occu)
		CL_DBPROPERTY("level", m_Level)
		CL_DBPROPERTY("enroll_time", m_EnrollTime)
		CL_DBPROPERTY("enroll_count", m_EnrollCount)
		CL_DBPROPERTY("remain_hp", m_RemainHP)
		CL_DBPROPERTY("remain_mp", m_RemainMP)
		CL_DBPROPERTY("win_num", m_WinNum)
		CL_DBPROPERTY("win_time", m_WinTime)
		CL_DBPROPERTY("final_battle_win_num", m_FinalBattleWinNum)
		CL_DBPROPERTY("ranking", m_Ranking)
		CL_DBPROPERTY("next_roll_pos", m_NextRollPos)
		CL_DBPROPERTY("battle_flag", m_BattleFlag)
		CL_DBPROPERTY("is_finish_roll_battle", m_IsFinishRollBattle)
		CL_DBPROPERTY("score", m_Score)
		CL_DBPROPERTY("preliminay_reward", m_PreliminayRewardNum)
	CL_DBPROPERTY_END()

private:
	// �Ƿ���
	bool		m_Dirty;
	// ��Ҷ���
	WSPlayer*	m_Player;
	// ְҵ
	ObjUInt8	m_Occu;
	// �ȼ�
	ObjUInt16	m_Level;
	// ����
	ObjUInt32	m_Score;
	// ��ȡ��Ԥѡ����������
	ObjUInt32	m_PreliminayRewardNum;

	// ����ʱ��
	ObjUInt32	m_EnrollTime;
	// ��������
	ObjUInt8	m_EnrollCount;
	// ʣ��HP
	ObjUInt32	m_RemainHP;
	// ʣ��MP
	ObjUInt32	m_RemainMP;
	// ʤ����
	ObjUInt8	m_WinNum;
	// ʤ��ʱ��
	ObjUInt32	m_WinTime;
	// ��̭��ʤ����
	ObjUInt8	m_FinalBattleWinNum;
	// ����
	ObjUInt32	m_Ranking;
	// ��һ����λ��
	ObjUInt32	m_NextRollPos;
	// ��̭����־(1����8ǿ��2����4ǿ��4���������8�������)
	ObjUInt8	m_BattleFlag;
	// �Ƿ������һ�ֵı���
	ObjUInt8	m_IsFinishRollBattle;
};


#endif