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
	//装备唯一id
	UInt64	itemId;
	//类型
	UInt8	type;
	//子类型
	UInt8	subType;
};

class EquipSchemeItemSet : public CLComplexProperty
{
public:

public://编解码相关
/**
*@brief 编解码字符串
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
	//装备<subtype, EquipSchemeItem>
	std::map<UInt8, EquipSchemeItem> equips;
	//称号
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

	//是否空
	bool IsEmpty();

	UInt32 GetSchemeId() { return schemeId; }

	void GetEquipIds(std::set<UInt64>& equipIds);
public:
	//编解码
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("ownerId", ownerId)
	CL_DBPROPERTY("type", type)
	CL_DBPROPERTY("schemeId", schemeId)
	CL_DBPROPERTY("weared", weared)
	CL_DBPROPERTY("items", itemSet)
	CL_DBPROPERTY_END()

public:
	//玩家id
	ObjUInt64	ownerId;
	//方案类型
	ObjUInt8	type;
	//方案id
	ObjUInt32	schemeId;
	//是否选择穿上
	ObjUInt8	weared;
	//方案内装备数据
	EquipSchemeItemSet itemSet;
};

//装备方案索引
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

	//类型
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
	//设置、获得所有者
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
	//首次启用某装备方案
	UInt32	OnPlayerFristWearScheme(EquipScheme* curr_scheme, UInt8 type, UInt8 schemeId, UInt8 isSync);
	//非首次启用某装备方案
	UInt32	OnPlayerWearScheme(EquipScheme* curr_scheme, EquipScheme* new_scheme, std::vector<UInt64>& overdueIds);
	
	//战斗数据
	void GetRaceInfo(std::vector<RaceEquipScheme>& raceEqScheme);

	//获取所有方案的装备
	void GetAllEquipIds(std::set<UInt64>& equipIds);

	//处理穿上装备
	void OnTakeUpEquip(Item* item);
	//处理卸下装备
	void OnTakeOffEquip(Item* item, bool sync = false);

	//判断装备是否在方案中
	bool IsEquipInScheme(UInt64 ItemId);

	bool IsUnWearedSchemeEmpty();

	//获取装备所在装备方案
	UInt32 GetSchemeOfEquip(UInt64 ItemId);

	//修正装备方案，当系统开关开的时候
	void FixSchemeForServiceOpen(bool notify = false);
private:
	// 所有者
	Player		*m_pOwner;
	std::map<UInt64, EquipScheme*>	guidMapSchemes;
	std::map<EquipSchemeKey, EquipScheme*>	schemes;
	//最近换方案时间戳
	UInt64		lsatOpTime;
};

#endif