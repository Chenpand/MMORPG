#ifndef _HONOR_MGR_H_
#define _HONOR_MGR_H_

#include <CLDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLHonor.h>

/**
 *@brief 个人荣誉
 */
class RoleHonor : public CLSyncObject
{
public:
	RoleHonor();
	~RoleHonor();

public:

	void SetID(UInt64 id) { m_RoleId = id; }
	UInt64 GetID() const { return m_RoleId; }

	void AddHonor(UInt32 honor) { m_Honor = m_Honor + honor; }
	void SetHonor(UInt32 honor) { m_Honor = honor; }
	UInt32 GetHonor() const { return m_Honor; }

	void SetHonorTime(UInt64 time) { m_HonorTime = time; }
	UInt64 GetHonorTime() const { return m_HonorTime; }

	void SetHonorLvl(UInt32 lvl) { m_HonorLvl = lvl; }
	UInt32 GetHonorLvl() const { return m_HonorLvl; }

	void SetHonorExp(UInt32 exp) { m_HonorExp = exp; }
	UInt32 GetHonorExp() const { return m_HonorExp; }

	void SetLastWeekRank(UInt32 rank) { m_LastWeekRank = rank; }
	UInt32 GetLastWeekRank() const { return m_LastWeekRank; }

	void SetHistoryRank(UInt32 rank) { m_HistoryRank = rank; }
	UInt32 GetHistoryRank() const { return m_HistoryRank; }

	void SetHighestHonorLvl(UInt32 lvl) { m_HighestHonorLvl = lvl; }
	UInt32 GetHighestHonorLvl() const { return m_HighestHonorLvl; }

	void ResetGuildCard() { m_IsUseGuardCard = 0; }
	bool IsUseGuardCard() { return m_IsUseGuardCard == 1; }
	void UseGuildCard() { m_IsUseGuardCard = 1; UpdateToDB(true); }
	UInt32 GetUseGuildCard() const { return m_IsUseGuardCard; }

	/**
	 *@brief 重置个人荣誉等级
	 */
	void ResetHonorLvl();

	std::string GetHonorStr();

public:

	void UpdateToDB(bool bFlush = true);
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("honor", m_Honor)
		CL_DBPROPERTY("honor_time", m_HonorTime)
		CL_DBPROPERTY("honor_lvl", m_HonorLvl)
		CL_DBPROPERTY("honor_exp", m_HonorExp)
		CL_DBPROPERTY("last_week_rank", m_LastWeekRank)
		CL_DBPROPERTY("history_rank", m_HistoryRank)
		CL_DBPROPERTY("highest_honor_lvl", m_HighestHonorLvl)
		CL_DBPROPERTY("is_use_guard_card", m_IsUseGuardCard)
		CL_DBPROPERTY_END()

private:

	ObjUInt64 m_RoleId;
	// 荣誉
	ObjUInt32 m_Honor;
	// 获得荣誉时间
	ObjUInt64 m_HonorTime;
	// 荣誉等级
	ObjUInt32 m_HonorLvl;
	// 荣誉经验
	ObjUInt32 m_HonorExp;
	// 上周排名
	ObjUInt32 m_LastWeekRank;
	// 历史排名
	ObjUInt32 m_HistoryRank;
	// 最高荣誉等级
	ObjUInt32 m_HighestHonorLvl;
	// 是否使用保障卡
	ObjUInt32 m_IsUseGuardCard;
};

/**
 *@brief 荣誉系统
 */
class HonorMgr : public Avalon::Singleton<HonorMgr> , public CLHonor
{
public:
	HonorMgr();
	~HonorMgr();

	bool IsSyncRankTime() { return m_RankTimeSync; }
	void SetSyncRankTime() { m_RankTimeSync = true; }

	void Init();

	void OnTick(const Avalon::Time& now);

	/**
	 *@brief pvp结算
	 */
	void PvpEnd(UInt64 roldId, UInt32 getHonor);

	/**
	 *@brief 增加角色荣誉
	 */
	void AddRoleHonor(UInt64 roldId, RoleHonor* honor);

	/**
	 *@brief 查找角色荣誉
	 */
	RoleHonor* FindRoleHonor(UInt64 roldId, bool isCreate = false);

	/**
	 *@brief 跨周全服排名
     */
	void ChangeWeekRank();

	/**
	 *@brief更新荣誉等级
	 */
	void UpdateHonorLevel(UInt64 roleId, UInt32 level);

	/**
	 *@brief 荣誉小红点
	 */
	void UpdateRoleHonorInfo(UInt64 playerId, UInt32 honorLvl);

	/**
	 *@brief 下次排名时间
	 */
	UInt32 NextRankTime();

private:

	// 排名时间是否同步
	bool m_RankTimeSync;

	// 荣誉列表(key->roleid)
	std::map<UInt64, RoleHonor*> m_HonorMap;
};

#endif
