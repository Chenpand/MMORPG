#ifndef _CL_DIG_DATAENTRY_H_
#define _CL_DIG_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

#include <CLDigDefine.h>

/**
*@brief 宝藏地图数据项
*/
struct DigMapDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	bool IsGoldRefreshHour(UInt32 hour);
	bool IsSilverRefreshHour(UInt32 hour);

public:
	UInt32 id;
	UInt32 goldDigMinNum;
	UInt32 goldDigMaxNum;
	std::vector<UInt32> goldRefreshHours;
	UInt32 silverDigMinNum;
	UInt32 silverDigMaxNum;
	std::vector<UInt32> silverRefreshHours;
	UInt32 digMaxNum;
};

class DigMapDataEntryMgr :public Avalon::DataEntryMgrBase<DigMapDataEntry>, public Avalon::Singleton<DigMapDataEntryMgr>
{
public:
	DigMapDataEntryMgr() { };
	~DigMapDataEntryMgr() { };

public:
	bool AddEntry(DigMapDataEntry* entry);

};

/**
*@brief 宝藏奖池数据项
*/
struct DigItemPoolDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	DigType digType;
	DigItemType digItemType;
	UInt32 group;
	UInt32 priority;
	UInt32 itemId;
	UInt32 itemNum;
	UInt32 boxWeight;
	UInt32 digWeight;
	UInt32 singleDigWeight;
	UInt32 rewardId;
};

class DigItemPoolDataEntryMgr : public Avalon::DataEntryMgrBase<DigItemPoolDataEntry>, public Avalon::Singleton<DigItemPoolDataEntryMgr>
{
public:

	struct DigItemPoolGroup
	{
		DigItemPoolGroup() :weight(0){}
		~DigItemPoolGroup(){}

		std::vector<DigItemPoolDataEntry*> digItemPools;
		UInt32 weight;
	};

	typedef std::map<UInt32, DigItemPoolGroup> DigItemPoolGroupMap;
	typedef std::map<DigType, DigItemPoolGroupMap> DigItemPoolTypeMap;

public:
	DigItemPoolDataEntryMgr() { };
	~DigItemPoolDataEntryMgr() { };

public:
	bool AddEntry(DigItemPoolDataEntry* entry);

	bool GetDigItemPoolType(DigType digType, DigItemPoolGroupMap& digItemPoolGroup);

private:
	DigItemPoolTypeMap m_DigItemPoolType;

};

struct DigItemRandomDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt32 group;
	UInt32 itemId;
	UInt32 weight;
	UInt32 itemNum;
	bool isAnnounce;

};

class DigItemRandomDataEntryMgr : public Avalon::DataEntryMgrBase<DigItemRandomDataEntry>, public Avalon::Singleton<DigItemRandomDataEntryMgr>
{
public:
	struct DigItemRandomGroup
	{
		DigItemRandomGroup() : weight(0) {}

		std::vector<DigItemRandomDataEntry*> digItemRandoms;
		UInt32 weight;
	};

public:
	DigItemRandomDataEntryMgr() {};
	~DigItemRandomDataEntryMgr() {};

public:
	bool AddEntry(DigItemRandomDataEntry* entry);

	bool GetRandomItem(UInt32 group, UInt32& itemId, UInt32& itemNum, bool& isAnn);

public:
	std::map<UInt32, DigItemRandomGroup> m_DigItemRandomGroups;

};

#endif //_CL_DIG_DATAENTRY_H_