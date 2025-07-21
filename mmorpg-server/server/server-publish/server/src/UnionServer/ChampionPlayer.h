#ifndef __CHAMPION_PLAYER_H__
#define __CHAMPION_PLAYER_H__

#include "USPlayer.h"
#include "CLRecordClient.h"
#include <AvalonSimpleTimer.h>
#include <CLChampionDefine.h>

class ChampionPlayer : public USPlayer
{

public:
	ChampionPlayer();
	void Init();
	virtual void OnTick(const Avalon::Time& now);
	virtual bool CanSendProtocol();
	void SaveToDB();
	void InsterToDB();
	void LoadDBData(CLRecordCallback* callback);
	/**
	*@brief ͬ��ʤ������
	*/
	void SyncBattleCount();
	/**
	*@brief ͬ���Լ�״̬
	*/
	void SyncStatus();
	/**
	*@brief ��ȡ�ý�ɫ������Ϣ
	*/
	RankRole GetRankRole() const;

public:
	/**
	*@brief ����Get-Set�ӿ�
	*/
	void SetStatus(ChampionStatus status);
	inline const ChampionStatus GetStatus() const { return static_cast<ChampionStatus>(static_cast<UInt32>(m_Status)); }
	void SetGroup(UInt32 groupNum);
	inline const UInt32 GetGroup() const { return m_Group; }
	inline const bool GetOnline()const { return m_BoOnline; };
	inline void SetOnline(bool bo) { m_BoOnline = bo; }
	inline const Int32 GetScore()const { return m_Score; };
	void SetScore(Int32 score);
	inline const UInt32 GetWinCount()const { return m_WinCount; };
	inline void SetWinCount(UInt32 score) { m_WinCount = score; }
	inline const UInt64 GetScoreChangeTime() const { return m_ScoreChangeTime; }
	inline void SetScoreChangeTime(UInt64 time) { m_ScoreChangeTime = time; }
	inline void SetRank(UInt32 rank) { m_Rank = rank; }
	inline const UInt32 GetRank() const { return m_Rank; }
	inline void SetIsWin(bool bo) { m_Rank = bo ? 1 : 0; }
	inline const UInt32 GetIsWin()const { return m_Rank ? 1 : 0; }
	void SetBattleCount(UInt32 count);
	inline const UInt32 GetBattleCount() const { return m_BattleCount; }
	inline void SetLoseCount(UInt32 count) { m_LoseCount = count; }
	inline const UInt32 GetLoseCount() const { return m_LoseCount; }
	inline void SetSceneID(UInt32 id) { m_SceneID = id; }
	inline const UInt32 GetSceneID() const { return m_SceneID; }
	inline const bool IsInRace()const { return m_IsInRace; };
	void SetIsInRace(bool bo);
	inline void SetPos(UInt32 pos) { m_Pos = pos; }
	inline const UInt32 GetPos() const { return m_Pos; }
	inline void SetAvatar(PlayerAvatar avatar) { m_Avatar = avatar; }
	inline const PlayerAvatar GetAvatar() const { return m_Avatar; }
	inline const bool IsNoTurn()const { return m_NoTurn; }

	/**
	*@brief �����Ƿ��ֿ�
	*/
	void SetIsNoTurn(bool bo);
	/**
	*@brief �ܷ����ս��
	*/
	bool CanStartRace(ChampionStatus status);

	/**
	*@brief ���pk���Ľ�ɫid
	*/
	void AddBattledRole(UInt64 roleID);

	/**
	*@brief �жϸ�id�ܷ���Լ�pk
	*/
	bool CanBattleWith(UInt64 roleID);

	/**
	*@brief �������pk���Ľ�ɫid
	*/
	void ClearBattledRole();

	/**
	*@brief ֪ͨ�����ֿ�
	*/
	void NotifyNullTurn();

	/**
	*@brief �������ı�
	*/
	void OnRankChanged();
public:

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("zoneid", m_ZoneId)
		CL_DBPROPERTY("server", m_Server)
		CL_DBPROPERTY("accid", m_AccId)
		CL_DBPROPERTY("name", m_Name)
		CL_DBPROPERTY("level", m_Level)
		CL_DBPROPERTY("occu", m_Occu)
		CL_DBPROPERTY("avatar", m_Avatar)
		CL_DBPROPERTY("supporter", m_Supporter)
		CL_DBPROPERTY("score", m_Score)
		CL_DBPROPERTY("score_change_time", m_ScoreChangeTime)
		CL_DBPROPERTY("group", m_Group)
		CL_DBPROPERTY("win_count", m_WinCount)
		CL_DBPROPERTY("state", m_Status)
		CL_DBPROPERTY("has_reward", m_HasReward)
		CL_DBPROPERTY("rank", m_Rank)
		//CL_DBPROPERTY("winner", m_IsWin)
		CL_DBPROPERTY("battle_count", m_BattleCount)
		CL_DBPROPERTY("lose_count", m_LoseCount)
		CL_DBPROPERTY("pos", m_Pos)
	CL_DBPROPERTY_END()

private:
	ObjPlayerAvatar m_Avatar;
	ObjUInt32 m_Supporter;
	ObjInt32 m_Score;
	ObjInt64 m_ScoreChangeTime;
	ObjUInt32 m_Group;
	ObjUInt32 m_WinCount;
	ObjUInt8 m_Status;
	ObjUInt8 m_HasReward;
	ObjUInt32 m_Rank;
	ObjUInt8 m_IsWin;
	ObjUInt32 m_BattleCount;
	ObjUInt32 m_LoseCount;
	ObjUInt32 m_Pos;
private:
	// �������ݿⶨʱ��
	Avalon::SimpleTimer	m_SaveTimer;
	// Union�Ƿ�����
	bool m_BoOnline;
	// ����id
	UInt32 m_SceneID;
	// �Ƿ��ڱ�����
	bool m_IsInRace;
	// �Ѿ�ƥ��������
	std::set<UInt64> m_BattledRoleID;
	// �����Ƿ��ֿ�
	bool m_NoTurn;
};





#endif

