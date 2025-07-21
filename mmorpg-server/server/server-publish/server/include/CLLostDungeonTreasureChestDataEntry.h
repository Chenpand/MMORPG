#pragma once
#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief	 ���α����
*/

//������Ȩ��
struct GiftWeight
{
	UInt32	dataId;		//���id
	UInt32	weight;		//Ȩ��
};

class LostDungeonTreasureDataEntry : public Avalon::DataEntry
{
public:
	LostDungeonTreasureDataEntry();
	~LostDungeonTreasureDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	//Ψһid
	UInt32		id;
	//����
	UInt8		type;
	//����
	std::vector<GiftWeight>  gifts;
};

class LostDungeonTreasureDataEntryMgr : public Avalon::DataEntryMgrBase<LostDungeonTreasureDataEntry>
	, public Avalon::Singleton<LostDungeonTreasureDataEntryMgr>
{
public:
	LostDungeonTreasureDataEntryMgr();
	~LostDungeonTreasureDataEntryMgr();
};