#ifndef __PREMIUM_LEAGUE_GAMER_H__
#define __PREMIUM_LEAGUE_GAMER_H__

/*
	赏金联赛参赛者
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
	*@brief 同步自身信息
	*/
	void SyncSelfInfo();

	/**
	*@brief 玩家对象
	*/
	void SetPlayer(WSPlayer* player) { m_Player = player; }
	WSPlayer* GetPlayer() const { return m_Player; }

	/**
	*@brief 是否已经输了
	*/
	bool IsLose() const { return m_RemainHP == 0; }

	/**
	*@brief 积分
	*/
	UInt32 GetScore() const { return m_Score; }
	void SetScore(UInt32 score);

	/**
	*@brief 剩余血量
	*/
	UInt32 GetRemainHp() const { return m_RemainHP; }
	void SetRemainHp(UInt32 hp);

	/**
	*@brief 剩余蓝量
	*/
	UInt32 GetRemainMp() const { return m_RemainMP; }
	void SetRemainMp(UInt32 mp) { m_RemainMP = mp; }

	/**
	*@brief 胜场数
	*/
	UInt32 GetWinNum() const { return m_WinNum; }
	void SetWinNum(UInt32 num);

	/**
	*@brief 报名次数
	*/
	UInt32 GetEnrollCount() const { return m_EnrollCount; }
	void SetEnrollCount(UInt32 count);

	/**
	*@brief 报名时间
	*/
	UInt32 GetEnrollTime() const { return m_EnrollTime; }
	void SetEnrollTime(UInt32 time) { m_EnrollTime = time; }

	/**
	*@brief 获取淘汰赛胜场数
	*/
	UInt32 GetFinalBattleWinNum() const { return m_FinalBattleWinNum; }
	void SetFinalBattleWinNum(UInt32 num) { m_FinalBattleWinNum = num; }

	/**
	*@brief 获取的预选赛奖金总数
	*/
	UInt32 GetPreliminayRewardNum() const { return m_PreliminayRewardNum; }
	void SetPreliminayRewardNumNum(UInt32 num) { m_PreliminayRewardNum = num; }
	void AddPreliminayRewardNumNum(UInt32 num) { m_PreliminayRewardNum = m_PreliminayRewardNum + num; }

	/**
	*@brief 胜利时间
	*/
	UInt32 GetWinTime() const { return m_WinTime; }
	void SetWinTime(UInt32 time) { m_WinTime = time; }

	/**
	*@brief 排名
	*/
	UInt32 GetRanking() const { return m_Ranking; }
	void SetRanking(UInt32 ranking);

	/**
	*@brief 下一轮的位置
	*/
	UInt32 GetNextRollPos() const { return m_NextRollPos; }
	void SetNextRollPos(UInt32 pos) { m_NextRollPos = pos; }

	/**
	*@brief 排名
	*/
	UInt8 GetBattleFlag() const { return m_BattleFlag; }
	void SetBattleFlag(UInt8 flag) { m_BattleFlag = flag; }

	/**
	*@brief 排名
	*/
	bool IsFinishRollBattle() const { return m_IsFinishRollBattle != 0; }
	void SetFinishRollBattle(bool finish) { m_IsFinishRollBattle = finish ? 1 : 0; }

	/**
	*@brief 是否脏
	*/
	bool IsDirty() const { return m_Dirty; }
	void SetDirty(bool dirty) { m_Dirty = dirty; }

	/**
	*@brief 职业
	*/
	UInt8 GetOccu() const { return m_Occu; }

	/**
	*@brief 等级
	*/
	UInt16 GetLevel() const { return m_Level; }

	/**
	*@brief 获取淘汰赛玩家信息
	*/
	PremiumLeagueBattleGamer GetBattleGamerInfo() const;

	/**
	*@brief 获取淘汰赛玩家信息
	*/
	PremiumLeagueBattleFighter GetBattleFighterInfo() const;

	/**
	*@brief 能否参加比赛
	*/
	bool CanStartBattle();

public:
	/**
	*@brief 报名完成
	*/
	void OnEnroll();

	/**
	*@brief 更新排行榜
	*/
	void UpdateSortList();

	/**
	*@brief 8强获胜
	*/
	void OnBattleWin();

	/**
	*@brief 8强获胜
	*/
	void OnBattleLose();

public:
	/**
	*@brief 报名完成
	*/
	void SendProtocol(Avalon::Protocol& protocol);

public:
	// 数据库相关
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
	// 是否脏
	bool		m_Dirty;
	// 玩家对象
	WSPlayer*	m_Player;
	// 职业
	ObjUInt8	m_Occu;
	// 等级
	ObjUInt16	m_Level;
	// 积分
	ObjUInt32	m_Score;
	// 获取的预选赛奖金总数
	ObjUInt32	m_PreliminayRewardNum;

	// 报名时间
	ObjUInt32	m_EnrollTime;
	// 报名次数
	ObjUInt8	m_EnrollCount;
	// 剩余HP
	ObjUInt32	m_RemainHP;
	// 剩余MP
	ObjUInt32	m_RemainMP;
	// 胜场数
	ObjUInt8	m_WinNum;
	// 胜利时间
	ObjUInt32	m_WinTime;
	// 淘汰赛胜场数
	ObjUInt8	m_FinalBattleWinNum;
	// 排名
	ObjUInt32	m_Ranking;
	// 下一轮座位号
	ObjUInt32	m_NextRollPos;
	// 淘汰赛标志(1代表8强，2代表4强，4代表决赛，8代表决赛)
	ObjUInt8	m_BattleFlag;
	// 是否完成这一轮的比赛
	ObjUInt8	m_IsFinishRollBattle;
};


#endif