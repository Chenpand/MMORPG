#ifndef __LOST_DUNGEON_DATAENTRY_H__
#define __LOST_DUNGEON_DATAENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//¥������ս��Ȩ��
struct FloorBattleWeight
{
	UInt32	battleDataId;	//ս��dataid
	UInt32	weight;		//Ȩ��
};

//���ѹؿ�������Ϣ
struct HardBattleGenInfo
{
	UInt8	num;			//����
	UInt32	pro;			//����
	UInt8	groupId;		//��id
	UInt8	groupMaxNum;	//����������
};

//����������Ϣ
struct TreasChestGenInfo
{
	UInt8   num;			//����
	UInt32	pro;			//����
	UInt8	groupId;		//��id
	UInt8	groupMaxNum;	//����������
};

//�������Ȩ��
struct TreasChestWeight
{
	UInt32	dataId;		//����id
	UInt32	weight;		//Ȩ��
};

/**
*@breif	 ����¥��������
*/
class LostDungeonFloorDataEntry : public Avalon::DataEntry
{
public:
	LostDungeonFloorDataEntry();
	~LostDungeonFloorDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);
public:
	//Ψһid
	UInt32		id;
	//¥��
	UInt32		floor;
	//¥������ս��
	std::vector<FloorBattleWeight>	battlesWt;
	//¥����սʧ�ܴ���
	UInt8		failHandlerId;
	//ʧ��ʱ����۳�����
	UInt32		failDecScore;
	//�Ƿ���ܴ��������̵�
	UInt8		existMysticShop;
	//�ڵ�����
	UInt8		nodeNum;
	//���ѹؿ�������Ϣ
	HardBattleGenInfo	hardBattleGenInfo;
	//����������Ϣ
	TreasChestGenInfo	treasChestGenInfo;
	//�����������
	std::vector<TreasChestWeight>	treasChestWt;
	//�������Ϳ۳����ֵĵ���
	UInt32	 notFallItemId;
	//��������սһ�α���ĵ���
	UInt32	againItemId;
};

class LostDungeonFloorDataEntryMgr : public Avalon::DataEntryMgrBase<LostDungeonFloorDataEntry>
	, public Avalon::Singleton<LostDungeonFloorDataEntryMgr>
{
public:
	LostDungeonFloorDataEntryMgr();
	~LostDungeonFloorDataEntryMgr();

	bool AddEntry(LostDungeonFloorDataEntry* dataEntry);
	UInt32  GetMaxFloor();
	LostDungeonFloorDataEntry* GetDataEntry(UInt32 floor);
public:
	UInt32 m_maxFloor;
	std::map<UInt32, LostDungeonFloorDataEntry*> m_datas;
};


/**
*@breif	 ��������Ԫ��������
*/
class LostDungeonElementDataEntry : public Avalon::DataEntry
{
public:
	LostDungeonElementDataEntry();
	~LostDungeonElementDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);
public:
	//Ψһid
	UInt32		id;
	//����
	UInt8		type;
	//����
	std::string param;
};

class LostDungeonElementDataEntryMgr : public Avalon::DataEntryMgrBase<LostDungeonElementDataEntry>
	, public Avalon::Singleton<LostDungeonElementDataEntryMgr>
{
public:
	LostDungeonElementDataEntryMgr();
	~LostDungeonElementDataEntryMgr();
public:
	bool AddEntry(LostDungeonElementDataEntry* dataEntry);
	UInt32 GetElementId(UInt8 elemetType);
public:
	std::map<UInt32, UInt32> m_typeMapId;
};

#endif