#ifndef _CL_BET_HORSE_DATA_ENTRY_H_
#define _CL_BET_HORSE_DATA_ENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <set>


struct BetHorseCfgEntry : public Avalon::DataEntry
{
	BetHorseCfgEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	UInt32 id;
	// �������Ѻע
	UInt32 stakeMax;
	// ÿ����Ҫ����������
	UInt32 needShooterNum;
	// ˢ������ʱ����
	UInt32 refreshOddsInterval;
	// Ѻע��ֹʱ��
	UInt32 stakeEndTime;
	// ��������ʱ��
	UInt32 adjustEndTime;
	// ÿ�쿪��ʱ��
	std::set<UInt32> startTimeSet;
	// �ӵ���ID
	UInt32 bulletID;
	// Ѻע��ʼ���ѹ���ID
	UInt32 startStakePublic;
	// Ѻע�������ѹ���
	UInt32 endStakePublic;
	// ������ʼ����
	UInt32 battleStartPublic;
	// ��������
	UInt32 openRewardPublic;
	// Ѻע��������ʱ��
	std::set<UInt32> endWarnTimeSet;
	// �������ֳ��ֵĸ���
	UInt32 mysteryRate;
	// ���ֻ���ʤ��
	UInt32 baseWinRate;
	// ��ʼ������Сֵ
	UInt32 initOddsMin;
	// ��ʼ�������ֵ
	UInt32 initOddsMax;
};

typedef Avalon::DataEntryMgr<BetHorseCfgEntry> BetHorseCfgEntryMgr;

// ***********************************************************

struct BetHorseShooterEntry : public Avalon::DataEntry
{
	BetHorseShooterEntry();

	UInt32 GetKey() const { return shooterId; }

	bool Read(Avalon::DataRow& row);

	// ����ID
	UInt32 shooterId;
	// ���ظ���
	UInt32 mysteryRate;
	// ��������
	UInt32 appearRate;
	// ����
	std::string shooterName;
	// ְҵ
	UInt32 occupation;
};

class BetHorseShooterEntryMgr : public Avalon::DataEntryMgrBase<BetHorseShooterEntry>
	, public Avalon::Singleton<BetHorseShooterEntryMgr>
{
public:
	BetHorseShooterEntryMgr();

	// ��������
	virtual bool AddEntry(BetHorseShooterEntry* entry);

	// ���������������
	UInt32 GetRandMysteryShooter(UInt32 _appearMysteryRate);

	// ����������ּ���
	void RandShooterVec(UInt32 _randNum, std::vector<UInt32>& _shooterVec);

private:

	// �����ܸ���
	UInt32 mysteryTotalRate;
	// �����ܸ���
	UInt32 appearTotalRate;
	// ���ظ����б�
	std::map<UInt32, UInt32> mysteryRateMap;
	// ���������б�
	std::map<UInt32, UInt32> appearRateMap;
	// �����б�
	std::map<UInt32, BetHorseShooterEntry*> shooterEntryMap;
};

//**********************************************************

struct BetHorseMapEntry : public Avalon::DataEntry
{
	BetHorseMapEntry();

	UInt32 GetKey() const { return mapId; }

	bool Read(Avalon::DataRow& row);

	// ��ͼid
	UInt32 mapId;
	// ��ͼʹ�õĽ׶�
	UInt32 mapPhase;
	// ��һ�ֱ���ʹ�õĵ�ͼ
	UInt32 nextMap;
};

typedef Avalon::DataEntryMgr<BetHorseMapEntry> BetHorseMapEntryMgr;

// ******************************

struct BetHorseRandEntry : public Avalon::DataEntry
{
	BetHorseRandEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	// id
	UInt32 id;
	// ��������
	UInt32 weatherType;
	// ��������
	UInt32 weatherRate;
	// ��������
	UInt32 terrainType;
	// ���θ���
	UInt32 terrainRate;
	// ״̬����
	UInt32 statusType;
	// ״̬����
	UInt32 statusRate;
};

class BetHorseRandEntryMgr : public Avalon::DataEntryMgrBase<BetHorseRandEntry>
	, public Avalon::Singleton<BetHorseRandEntryMgr>
{
public:
	BetHorseRandEntryMgr();

	// ��������
	virtual bool AddEntry(BetHorseRandEntry* entry);

	// ����������
	UInt32 GetRandWeather();

	// �����õ���
	UInt32 GetRandTerrain();

	// ������״̬
	UInt32 GetRandStatus();

private:

	// ������Ȩ��
	UInt32 weatherTotalRate;
	// ������Ȩ��
	UInt32 terrainTotalRate;
	// ״̬��Ȩ��
	UInt32 statusTotalRate;
	// ���������б�
	std::map<UInt32, UInt32> weatherRateMap;
	// ���θ����б�
	std::map<UInt32, UInt32> terrainRateMap;
	// ״̬�����б�
	std::map<UInt32, UInt32> statusRateMap;
};

// *************************************

struct BetHorseShooterAttrEntry : public Avalon::DataEntry
{
	BetHorseShooterAttrEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	// id
	UInt32 id;
	// ����1����
	UInt32 shooter_1;
	// ����2״̬
	UInt32 shooter_2;
	// ����1��ȡʤ����
	Int32 winRate;
};

struct BetHorseShooterAttrGroup
{
	BetHorseShooterAttrGroup()
	{
		shooterAttr_1 = 0;
		shooterAttr_2 = 0;
	}
	
	bool operator<(const BetHorseShooterAttrGroup& status) const
	{
		if (status.shooterAttr_1 < shooterAttr_1)
			return true;

		if (shooterAttr_1 < status.shooterAttr_1)
			return false;

		if (status.shooterAttr_2 < shooterAttr_2)
			return true;

		if (shooterAttr_2 < status.shooterAttr_2)
			return false;

		return false;
	}

	// ����1״̬
	UInt32 shooterAttr_1;
	// ����2״̬
	UInt32 shooterAttr_2;
};

class BetHorseShooterStatusEntryMgr : public Avalon::DataEntryMgrBase<BetHorseShooterAttrEntry>
	, public Avalon::Singleton<BetHorseShooterStatusEntryMgr>
{
public:
	BetHorseShooterStatusEntryMgr(){}

	// ��������
	virtual bool AddEntry(BetHorseShooterAttrEntry* entry);

	// ��ȡ��״̬�µ�ʤ��
	Int32 GetShooterStatusWinRate(UInt32 _shooterStatus_1, UInt32 _shooterStatus_2);

private:

	std::map<BetHorseShooterAttrGroup, Int32> shooterRateMap;
};

// ****************************************************

struct BetHorseRateEntry : public Avalon::DataEntry
{
	BetHorseRateEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	// id
	UInt32 id;
	// ��������
	UInt32 weatherType;
	// ������ְҵ1
	UInt32 weatherOccupation_1;
	// ������ְҵ2
	UInt32 weatherOccupation_2;
	// ����ȡʤ����
	Int32 weatherWinRate;

	// ��������
	UInt32 terrainType;
	// ������ְҵ1
	UInt32 terrainOccupation_1;
	// ������ְҵ2
	UInt32 terrainOccupation_2;
	// ������ȡʤ����
	Int32 terrainWinRate;
};

struct BetHorseAmbient
{
	BetHorseAmbient()
	{
		ambientType = 0;
		occupation_1 = 0;
		occupation_2 = 0;
	}

	bool operator<(const BetHorseAmbient& rhs) const
	{
		if (rhs.ambientType < ambientType)
			return true;

		if (ambientType < rhs.ambientType)
			return false;

		if (rhs.occupation_1 < occupation_1)
			return true;

		if (occupation_1 < rhs.occupation_1)
			return false;

		if (rhs.occupation_2 < occupation_2)
			return true;

		if (occupation_2 < rhs.occupation_2)
			return false;

		return false;
	}

	// ��������(����������)
	UInt32 ambientType;
	// ְҵ1
	UInt32 occupation_1;
	// ְҵ2
	UInt32 occupation_2;
};

class BetHorseRateEntryMgr : public Avalon::DataEntryMgrBase<BetHorseRateEntry>
	, public Avalon::Singleton<BetHorseRateEntryMgr>
{
public:
	BetHorseRateEntryMgr(){}

	// ��������
	virtual bool AddEntry(BetHorseRateEntry* entry);

	// ��ȡĳ��������ְҵ��ʤ��
	Int32 GetWeatherOccpationWinRate(UInt32 _weather, UInt32 _occupation_1, UInt32 _occupation_2);

	// ��ȡĳ��������ְҵ��ʤ��
	Int32 GetTerrainOccpationWinRate(UInt32 _terrain, UInt32 _occupation_1, UInt32 _occupation_2);

private:

	std::map<BetHorseAmbient, Int32> weatherOccupationRateMap;
	std::map<BetHorseAmbient, Int32> terrainOccupationRateMap;
};

//*************************************

class BetHorseOccuSuppressEntryMgr : public Avalon::DataEntryMgrBase<BetHorseShooterAttrEntry>
	, public Avalon::Singleton<BetHorseOccuSuppressEntryMgr>
{
public:
	BetHorseOccuSuppressEntryMgr(){}

	// ��������
	virtual bool AddEntry(BetHorseShooterAttrEntry* entry);

	// ��ȡ��ְҵ�µ�ʤ��
	Int32 GetOccuSuppressWinRate(UInt32 _occu_1, UInt32 _occu_2);

private:

	std::map<BetHorseShooterAttrGroup, Int32> occuSuppressMap;
};
#endif