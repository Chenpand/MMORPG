#ifndef _BET_HORSE_INFO_H_
#define _BET_HORSE_INFO_H_

#include <CLDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLBetHorseDefine.h>

struct BetHorseShooterEntry;

class HorseYard : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(HorseYard, Avalon::NullMutex)
public:

	HorseYard();

	void SetMapID(UInt32 _id) { m_id = _id; }
	UInt32 GetMapID() { return m_id; }

	void SetTerrain(UInt32 _terrain){ m_terrain = _terrain; }
	UInt32 GetTerrain(){ return m_terrain; }

	void SetWinShooter(UInt32 _winShooter){ m_winShooter = _winShooter; }
	UInt32 GetWinShooter(){ return m_winShooter; }

	void SetShooter_1(UInt32 _shooter){ m_shooter_1 = _shooter; }
	UInt32 GetShooter_1() { return m_shooter_1; }

	void SetShooter_2(UInt32 _shooter){ m_shooter_2 = _shooter; }
	UInt32 GetShooter_2() { return m_shooter_2; }

	void UpdateToDB();
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("map_id", m_id)
		CL_DBPROPERTY("terrain", m_terrain)
		CL_DBPROPERTY("win_shooter", m_winShooter)
		CL_DBPROPERTY("shooter_1", m_shooter_1)
		CL_DBPROPERTY("shooter_2", m_shooter_2)
		CL_DBPROPERTY_END()

private:

	// ����ͼid
	ObjUInt32 m_id;
	// ����
	ObjUInt32 m_terrain;
	// ʤ��ѡ��
	ObjUInt32 m_winShooter;
	// ����1
	ObjUInt32 m_shooter_1;
	// ����2
	ObjUInt32 m_shooter_2;
};

class ShooterData : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(ShooterData, Avalon::NullMutex)
public:
	ShooterData();
	~ShooterData();

	void SetShooterID(UInt32 _id) { m_id = _id; }
	UInt32 GetShooterID() { return m_id; }

	void SetStatus(UInt32 _status) { m_status = _status; }
	UInt32 GetStatus() { return m_status; }

	void AddStakeNum(UInt32 _addNum)  { m_stakeNum = m_stakeNum + _addNum; }
	void SetStakeNum(UInt32 _stakeNum) { m_stakeNum = _stakeNum; }
	UInt32 GetStakeNum() { return m_stakeNum; }
	
	// ��������
	void UpdateOdds(UInt32 _totalStake);
	UInt32 GetOdds() { return m_odds; }

	void SetCfg(const BetHorseShooterEntry* _cfg) { m_cfg = _cfg; }
	const BetHorseShooterEntry* GetCfg() { return m_cfg; }

//���ݿ����
public:

	void UpdateToDB();
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("shooter_id", m_id)
		CL_DBPROPERTY("stake_num", m_stakeNum)
		CL_DBPROPERTY("status", m_status)

		CL_DBPROPERTY_END()

private:
	// ����id
	ObjUInt32 m_id;
	// ״̬
	ObjUInt32 m_status;
	// Ѻע����
	ObjUInt32 m_stakeNum;
	// ����
	ObjUInt32 m_odds;
	// ����
	const BetHorseShooterEntry* m_cfg;
};

struct HistoryRecord
{
	AVALON_DEFINE_OBJECTPOOL(HistoryRecord, Avalon::NullMutex)

	HistoryRecord()
	{
		id = 0;
		odds = 0;
		result = 0;
		courtId = 0;
	}

	// ����id
	UInt32 id;
	// ����
	UInt32 courtId;
	// ��ս���
	UInt32 result;
	// �ҵĵ�����
	UInt32 odds;
};

typedef std::vector<HistoryRecord> HistoryRecordVec;

struct ShooterRecordData
{
	AVALON_DEFINE_OBJECTPOOL(ShooterRecordData, Avalon::NullMutex)

	ShooterRecordData()
	{
		champion = 0;
		winRate = 0;
	}

	// �Լ�����
	UInt32 champion;
	// ʤ��
	UInt32 winRate;
	// ��ʷ��¼�б�
	HistoryRecordVec historyVec;
};


// ***************************************

struct PlayerStakeCourt
{
	AVALON_DEFINE_OBJECTPOOL(PlayerStakeCourt, Avalon::NullMutex)

	PlayerStakeCourt()
	{
		courtId = 0;
		shooterId = 0;
	}

	bool operator < (const PlayerStakeCourt& court) const
	{
		if (court.courtId < courtId)
			return true;

		if (courtId < court.courtId)
			return false;

		if (court.shooterId < shooterId)
			return true;

		if (shooterId < court.shooterId)
			return false;

		return false;
	}

	// ����id
	UInt32 courtId;
	// ����id
	UInt32 shooterId;
};


struct PlayerStakeRecord
{
	AVALON_DEFINE_OBJECTPOOL(PlayerStakeRecord, Avalon::NullMutex)

	PlayerStakeRecord()
	{
		odds = 0;
		stakeNum = 0;
		profit = -1;
	}

	// ����
	UInt32 odds;
	// Ѻע����
	UInt32 stakeNum;
	// ӯ��
	Int32 profit;
};

// ******************************************

struct BattleRecordData
{
	AVALON_DEFINE_OBJECTPOOL(BattleRecordData, Avalon::NullMutex)

	BattleRecordData()
	{
		courtId = 0;
		champion = 0;
		odds = 0;
		maxProfit = 0;
		endTime = 0;
	}

	// ����
	UInt32 courtId;
	// �ھ�
	UInt32 champion;
	// ����
	UInt32 odds;
	// ���ӯ��
	UInt32 maxProfit;
	// ����ʱ��
	UInt64 endTime;
};

// *****************************************
struct PlayerData
{
	AVALON_DEFINE_OBJECTPOOL(PlayerData, Avalon::NullMutex)

	PlayerData()
	{
		adminNode = 0;
		centerNode = 0;
	}

	bool operator < (const PlayerData& data) const
	{
		return uId < data.uId;
	}

	std::string uId;
	std::string name;
	std::string serverName;
	UInt32 adminNode;
	UInt32 centerNode;
};

// ***********************************************

struct BetHorseReward
{
	AVALON_DEFINE_OBJECTPOOL(BetHorseReward, Avalon::NullMutex)

	BetHorseReward()
	{
		adminNode = 0;
		centerNode = 0;
		courtId = 0;
		stakeNum = 0;
		rewardNum = 0;
		isSuccess = 0;
	}

	UInt32 adminNode;
	UInt32 centerNode;
	UInt32 courtId;
	UInt32 stakeNum;
	UInt32 rewardNum;
	UInt32 isSuccess;
};

#endif