#ifndef _HONOR_MGR_H_
#define _HONOR_MGR_H_

#include <CLDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLHonor.h>

/**
 *@brief ��������
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
	 *@brief ���ø��������ȼ�
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
	// ����
	ObjUInt32 m_Honor;
	// �������ʱ��
	ObjUInt64 m_HonorTime;
	// �����ȼ�
	ObjUInt32 m_HonorLvl;
	// ��������
	ObjUInt32 m_HonorExp;
	// ��������
	ObjUInt32 m_LastWeekRank;
	// ��ʷ����
	ObjUInt32 m_HistoryRank;
	// ��������ȼ�
	ObjUInt32 m_HighestHonorLvl;
	// �Ƿ�ʹ�ñ��Ͽ�
	ObjUInt32 m_IsUseGuardCard;
};

/**
 *@brief ����ϵͳ
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
	 *@brief pvp����
	 */
	void PvpEnd(UInt64 roldId, UInt32 getHonor);

	/**
	 *@brief ���ӽ�ɫ����
	 */
	void AddRoleHonor(UInt64 roldId, RoleHonor* honor);

	/**
	 *@brief ���ҽ�ɫ����
	 */
	RoleHonor* FindRoleHonor(UInt64 roldId, bool isCreate = false);

	/**
	 *@brief ����ȫ������
     */
	void ChangeWeekRank();

	/**
	 *@brief���������ȼ�
	 */
	void UpdateHonorLevel(UInt64 roleId, UInt32 level);

	/**
	 *@brief ����С���
	 */
	void UpdateRoleHonorInfo(UInt64 playerId, UInt32 honorLvl);

	/**
	 *@brief �´�����ʱ��
	 */
	UInt32 NextRankTime();

private:

	// ����ʱ���Ƿ�ͬ��
	bool m_RankTimeSync;

	// �����б�(key->roleid)
	std::map<UInt64, RoleHonor*> m_HonorMap;
};

#endif
