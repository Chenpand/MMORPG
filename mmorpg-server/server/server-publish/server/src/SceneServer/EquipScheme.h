#ifndef __EQUIP_SCHEME_H__
#define __EQUIP_SCHEME_H__

#include <CLDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLEquipSchemeDefine.h>

class Player;
class Item;

class EquipSchemeItem 
{
public:
	EquipSchemeItem()
		:itemId(0), type(0), subType(0)
	{};
	EquipSchemeItem(UInt64 itemId_, UInt8 isAssist_, UInt8 type_, UInt8 subType_, UInt32 param_)
		:itemId(itemId_), type(type_), subType(subType_)
	{};
	~EquipSchemeItem() {};


public:
	//װ��Ψһid
	UInt64	itemId;
	//����
	UInt8	type;
	//������
	UInt8	subType;
};

class EquipSchemeItemSet : public CLComplexProperty
{
public:

public://��������
/**
*@brief ������ַ���
*/
	void EncodeString(std::ostringstream& os);
	void DecodeString(std::istringstream& is);

	EquipSchemeItem& FindEquipInEquipPack(UInt8 subtype);
	void GetItemIds(std::vector<UInt64>& ids);

	void OnTakeUpEquip(Item *item);
	void OnTakeOffEquip(Item *item);
	void OnEquipMiss(UInt64 equipId);

	void OnTakeUpTitle(Item *item);
	void OnTakeOffTitle();
	void OnTitleMiss();

	void OnTakeUpItem(Item *item);
	void OnItemMiss(UInt64 itemId);
	bool IsEmpty();
public:
	//װ��<subtype, EquipSchemeItem>
	std::map<UInt8, EquipSchemeItem> equips;
	//�ƺ�
	EquipSchemeItem	title;
};

class EquipScheme : public CLSyncObject
{
public:
	EquipScheme()
	{
		ownerId = 0;
		type = 0;
		schemeId = 0;
		weared = 0;
	};
	~EquipScheme() {};

public:
	EquipSchemeInfo ToClient();
	void LoadFromDb(CLRecordCallback *pCallback);
	void Update();
	void Insert();
	void Delete();

	void SyncToClient(Player* player);

	void SyncFromOtherScheme(const EquipScheme& otherScheme);
	EquipSchemeItemSet& GetItemSet() { return itemSet; }

	RaceEquipScheme GetRaceInfo();

	//�Ƿ��
	bool IsEmpty();

	UInt32 GetSchemeId() { return schemeId; }

	void GetEquipIds(std::set<UInt64>& equipIds);
public:
	//�����
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("ownerId", ownerId)
	CL_DBPROPERTY("type", type)
	CL_DBPROPERTY("schemeId", schemeId)
	CL_DBPROPERTY("weared", weared)
	CL_DBPROPERTY("items", itemSet)
	CL_DBPROPERTY_END()

public:
	//���id
	ObjUInt64	ownerId;
	//��������
	ObjUInt8	type;
	//����id
	ObjUInt32	schemeId;
	//�Ƿ�ѡ����
	ObjUInt8	weared;
	//������װ������
	EquipSchemeItemSet itemSet;
};

//װ����������
struct  EquipSchemeKey
{
	EquipSchemeKey(UInt8 type_, UInt32 schemeId_)
		:type(type_), schemeId(schemeId_)
	{}

	bool operator<(const EquipSchemeKey& other) const
	{
		if (other.type != type)
		{
			return type < other.type;
		}
		return schemeId < other.schemeId;
	}

	//����
	UInt8 type;
	//id
	UInt32 schemeId;
};

class EquipSchemeMgr
{
public:
	EquipSchemeMgr() 
		: lsatOpTime(0)
	{};
	~EquipSchemeMgr();
public:
	//���á����������
	void SetOwner(Player *pPlayer) { m_pOwner = pPlayer; }
	Player* GetOwner() const { return m_pOwner; }

	void	LoadDataFromDb(CLRecordCallback* pCallback);
	EquipScheme*  GetScheme(const EquipSchemeKey& key);
	EquipScheme*  GetScheme(UInt8 type, UInt32 schemeId);
	EquipScheme*  GetScheme(UInt64 schemeGuid);
	EquipScheme*  GetWearingScheme(UInt8 type);

	bool	AddOneScheme(EquipScheme* scheme);

	void    OnOnline();
	void    CreateSchemeOnOnlie();

	UInt32  OnPlayerWearReq(UInt8 type, UInt8 schemeId, UInt8 isSync, std::vector<UInt64>& overdueIds);
	//�״�����ĳװ������
	UInt32	OnPlayerFristWearScheme(EquipScheme* curr_scheme, UInt8 type, UInt8 schemeId, UInt8 isSync);
	//���״�����ĳװ������
	UInt32	OnPlayerWearScheme(EquipScheme* curr_scheme, EquipScheme* new_scheme, std::vector<UInt64>& overdueIds);
	
	//ս������
	void GetRaceInfo(std::vector<RaceEquipScheme>& raceEqScheme);

	//��ȡ���з�����װ��
	void GetAllEquipIds(std::set<UInt64>& equipIds);

	//������װ��
	void OnTakeUpEquip(Item* item);
	//����ж��װ��
	void OnTakeOffEquip(Item* item, bool sync = false);

	//�ж�װ���Ƿ��ڷ�����
	bool IsEquipInScheme(UInt64 ItemId);

	bool IsUnWearedSchemeEmpty();

	//��ȡװ������װ������
	UInt32 GetSchemeOfEquip(UInt64 ItemId);

	//����װ����������ϵͳ���ؿ���ʱ��
	void FixSchemeForServiceOpen(bool notify = false);
private:
	// ������
	Player		*m_pOwner;
	std::map<UInt64, EquipScheme*>	guidMapSchemes;
	std::map<EquipSchemeKey, EquipScheme*>	schemes;
	//���������ʱ���
	UInt64		lsatOpTime;
};

#endif