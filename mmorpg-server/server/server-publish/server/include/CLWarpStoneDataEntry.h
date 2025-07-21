#ifndef _CL_WARP_STONE_DATAENTRY_H_
#define _CL_WARP_STONE_DATAENTRY_H_

#include "CLDefine.h"
#include "CLWarpStoneDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ����ʯ���ñ������
*/
struct EnergyStoneDataEntry : public Avalon::DataEntry
{
public:
	EnergyStoneDataEntry();
	~EnergyStoneDataEntry();
public:
	UInt32 GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

public:
	//����ʯID
	UInt32	id;
	//��ƷID
	UInt32	itemId;
	//����ֵ
	UInt32	energyNum;
	//����ʯ����
	WarpStoneType	type;
};

/**
*@brief ����ʯ���ñ������
*/
typedef Avalon::DataEntryMgr<EnergyStoneDataEntry> EnergyStoneDataEntryMgr;

/**
*@brief ��Ԫʯ�ɳ����ñ�
*/
struct WarpStoneLevelDataEntry : public Avalon::DataEntry
{
public:
	WarpStoneLevelDataEntry();
	~WarpStoneLevelDataEntry();
public:
	UInt32 GetKey() const { return level; }
	bool Read(Avalon::DataRow& row);

public:
	//�ȼ�
	UInt32	level;
	//����
	UInt32	energy;
};

/**
*@brief ��Ԫʯ�ɳ����ñ������
*/
class WarpStoneLevelDataEntryMgr : public Avalon::DataEntryMgrBase<WarpStoneLevelDataEntry>, public Avalon::Singleton<WarpStoneLevelDataEntryMgr>
{
public:
	typedef std::map<UInt32, UInt32> SumEnergyMap;

	WarpStoneLevelDataEntryMgr();
	~WarpStoneLevelDataEntryMgr();

public:
	bool AddEntry(WarpStoneLevelDataEntry* entry);
	
	UInt32 GetSumEnergy(UInt32 level);

private:
	SumEnergyMap	m_SumEnergyMap;
};


/**
*@brief ��Ԫʯ���ñ�
*/
struct WarpStoneDataEntry : public Avalon::DataEntry
{
public:
	WarpStoneDataEntry();
	~WarpStoneDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32	id;

	//����
	std::string name;

	//��Ԫʯ����
	WarpStoneType	type;
	//�����ȼ�
	UInt16	level;
	//��������
	UInt32	gold;
	//�Ƿ��Զ�����
	bool	isUnlock;

};


/**
*@brief ��Ԫʯ���ñ������
*/
class WarpStoneDataEntryMgr : public Avalon::DataEntryMgrBase<WarpStoneDataEntry>, public Avalon::Singleton<WarpStoneDataEntryMgr>
{
public:

	typedef std::vector<WarpStoneDataEntry*> WarpStoneDataVec;
	typedef std::map<UInt16, WarpStoneDataVec> WarpStoneDataMap;

	WarpStoneDataEntryMgr();
	~WarpStoneDataEntryMgr();

public:
	bool AddEntry(WarpStoneDataEntry* entry);

	/**
	*@brief ���ݵȼ���ȡ��Ԫʯ
	*/
	const WarpStoneDataVec& GetLvlWarpStones(UInt16 level) const;

private:
	WarpStoneDataMap m_LvlWarpStones;

};


#endif

