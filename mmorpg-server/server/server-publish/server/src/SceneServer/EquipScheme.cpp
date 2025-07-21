#include "EquipScheme.h"

#include <CLRecordClient.h>
#include <CLItemProtocol.h>
#include <CLErrorCode.h>
#include "Player.h"
#include "SSServiceSwitchMgr.h"

void EquipSchemeItemSet::EncodeString(std::ostringstream& os)
{
	char split = ',';
	for (auto it : equips)
	{
		EquipSchemeItem& equip = it.second;
		os << equip.itemId << split << UInt32(equip.type)
			<< split << UInt32(equip.subType) <<  '|';
	}
	if (title.itemId > 0)
	{
		os << title.itemId << split << UInt32(title.type)
			<< split << UInt32(title.subType) << '|';
	}
}

void EquipSchemeItemSet::DecodeString(std::istringstream& is)
{
	std::string str;
	is >> str;
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");
	for (auto s : strs)
	{
		EquipSchemeItem item;
		std::vector<std::string> params;
		Avalon::StringUtil::Split(s, params, ",");
		if (params.size() >= 3)
		{
			item.itemId = Avalon::StringUtil::ConvertToValue<UInt64>(params[0]);
			item.type = Avalon::StringUtil::ConvertToValue<UInt8>(params[1]);
			item.subType = Avalon::StringUtil::ConvertToValue<UInt8>(params[2]);
		}

		if (item.type == ITEM_TITLE)
		{
			title = item;
		}
		else if (item.type == ITEM_EQUIP)
		{
			equips[item.subType] = item;
		}
	}
}

EquipSchemeItem& EquipSchemeItemSet::FindEquipInEquipPack(UInt8 subtype)
{
	static EquipSchemeItem schemeItem;
	auto it = equips.find(subtype);
	if (it != equips.end())
	{
		return it->second;
	}
	return schemeItem;
}

void  EquipSchemeItemSet::GetItemIds(std::vector<UInt64>& ids)
{
	for (auto it : equips)
	{
		EquipSchemeItem& schemeItem = it.second;
		ids.push_back(schemeItem.itemId);
	}
	if (title.itemId > 0)
	{
		ids.push_back(title.itemId);
	}
}

void EquipSchemeItemSet::OnTakeUpEquip(Item *item)
{
	if (!item)
	{
		return;
	}
	
	EquipSchemeItem schemeItem;
	schemeItem.itemId = item->GetID();
	schemeItem.type = item->GetType();
	schemeItem.subType = item->GetSubType();
	equips[schemeItem.subType] = schemeItem;
	SetDirty();
}

void EquipSchemeItemSet::OnTakeOffEquip(Item *item)
{
	if (!item)
	{
		return;
	}

	auto it = equips.find(item->GetSubType());
	if (it == equips.end())
	{
		return;
	}

	equips.erase(it);
	SetDirty();
}

void EquipSchemeItemSet::OnEquipMiss(UInt64 equipId)
{
	for (auto it : equips)
	{
		if (it.second.itemId == equipId)
		{
			equips.erase(it.first);
			SetDirty();
			break;
		}
	}
}

void EquipSchemeItemSet::OnTakeUpTitle(Item *item)
{
	if (!item)
	{
		return;
	}
	title.itemId = item->GetID();
	title.type = item->GetType();
	title.subType = item->GetSubType();
	SetDirty();
}

void EquipSchemeItemSet::OnTitleMiss()
{
	if (title.itemId == 0)
	{
		return;
	}
	title.itemId = 0;
	title.type = 0;
	title.subType = 0;
	SetDirty();
}

void EquipSchemeItemSet::OnItemMiss(UInt64 itemId)
{
	if (itemId == title.itemId)
	{
		return OnTitleMiss();
	}
	else
	{
		return OnEquipMiss(itemId);
	}
}

void EquipSchemeItemSet::OnTakeUpItem(Item *item)
{
	if (!item)	return;
	if (item->GetType() == ITEM_EQUIP)
	{
		OnTakeUpEquip(item);
	}
	else if (item->GetType() == ITEM_TITLE)
	{
		OnTakeUpTitle(item);
	}
}

void EquipSchemeItemSet::OnTakeOffTitle()
{
	if (title.itemId ==0)
	{
		return;
	}
	title.itemId = 0;
	title.type = 0;
	title.subType = 0;
	SetDirty();
}

bool EquipSchemeItemSet::IsEmpty()
{
	return title.itemId == 0 && equips.empty();
}

void EquipScheme::GetEquipIds(std::set<UInt64>& equipIds)
{
	if (this->GetItemSet().title.itemId > 0)
	{
		equipIds.insert(this->GetItemSet().title.itemId);
	}

	for (auto schemeItem : this->GetItemSet().equips)
	{
		UInt64 equipId = schemeItem.second.itemId;
		equipIds.insert(equipId);
	}
}

EquipSchemeInfo EquipScheme::ToClient()
{
	EquipSchemeInfo schemeInfo;
	schemeInfo.guid = GetID();
	schemeInfo.type = type;
	schemeInfo.id = schemeId;
	schemeInfo.weared = weared;
	GetItemSet().GetItemIds(schemeInfo.equips);
	return schemeInfo;
}

void EquipScheme::LoadFromDb(CLRecordCallback *pCallback)
{
	if (!pCallback)
	{
		return;
	}
	this->SetDBData(pCallback);
	this->SetID(pCallback->GetKey());
	this->ClearDirty();
}

void EquipScheme::Update()
{
	if (GetID() == 0)
	{
		Insert();
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_equip_scheme", GetID(), false);
		this->GetDBData(cmd, true);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void EquipScheme::Insert()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_equip_scheme", GetID());
	this->GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void EquipScheme::Delete()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_equip_scheme", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void EquipScheme::SyncToClient(Player* player)
{
	if (!player)
	{
		return;
	}
	CLProtocol::SceneEquipSchemeSync protocl;
	protocl.schemes.push_back(this->ToClient());
	player->SendProtocol(protocl);
}

void EquipScheme::SyncFromOtherScheme(const EquipScheme& otherScheme)
{
	itemSet.equips = otherScheme.itemSet.equips;
	itemSet.title = otherScheme.itemSet.title;
	itemSet.SetDirty();
}

RaceEquipScheme EquipScheme::GetRaceInfo()
{
	RaceEquipScheme raceEqScheme;
	raceEqScheme.type = type;
	raceEqScheme.id = schemeId;
	raceEqScheme.isWear = weared;
	raceEqScheme.title = itemSet.title.itemId;
	for (auto it : itemSet.equips)
	{
		raceEqScheme.equips.push_back(it.second.itemId);
	}
	return raceEqScheme;
}

bool EquipScheme::IsEmpty()
{
	return itemSet.IsEmpty();
}

EquipSchemeMgr::~EquipSchemeMgr()
{
	for (auto it : guidMapSchemes)
	{
		EquipScheme* scheme = it.second;
		if (!scheme)
		{
			continue;
		}
		CL_SAFE_DELETE(scheme);
	}

	guidMapSchemes.clear();
	schemes.clear();
}


void EquipSchemeMgr::LoadDataFromDb(CLRecordCallback* pCallback)
{
	if (!pCallback)
	{
		return;
	}

	while (pCallback->NextRow())
	{
		UInt8	type = pCallback->GetData("type");
		UInt64  schemeId = pCallback->GetData("schemeId");
	
		EquipSchemeKey key(type, schemeId);
		auto record = GetScheme(key);
		if (record)	continue;	//重复

		record = new EquipScheme();
		record->LoadFromDb(pCallback);
		this->AddOneScheme(record);
	}
	return;
}

EquipScheme*  EquipSchemeMgr::GetScheme(const EquipSchemeKey& key)
{
	auto iter = schemes.find(key);
	if (iter == schemes.end())
	{
		return NULL;
	}
	return iter->second;
}

EquipScheme*  EquipSchemeMgr::GetScheme(UInt8 type, UInt32 schemeId)
{
	EquipSchemeKey key(type, schemeId);
	return GetScheme(key);
}

EquipScheme*  EquipSchemeMgr::GetScheme(UInt64 schemeGuid)
{
	auto iter = guidMapSchemes.find(schemeGuid);
	if (iter == guidMapSchemes.end())
	{
		return NULL;
	}
	return iter->second;
}

EquipScheme*   EquipSchemeMgr::GetWearingScheme(UInt8 type)
{
	EquipScheme* scheme = NULL;
	for (auto it : guidMapSchemes)
	{
		scheme = it.second;
		if (!scheme)
		{
			continue;
		}
		if (scheme->type != type)
		{
			continue;
		}
		if (scheme->weared == 1)
		{
			return scheme;
		}
	}

	return scheme;
}

bool	EquipSchemeMgr::AddOneScheme(EquipScheme* scheme)
{
	if (!scheme)
	{
		return false;
	}
	EquipSchemeKey key(scheme->type, scheme->schemeId);

	auto it = schemes.find(key);
	if (it != schemes.end())
	{
		return false;
	}
	schemes.insert(std::pair<EquipSchemeKey, EquipScheme*>(key, scheme));

	auto iter = guidMapSchemes.find(scheme->GetID());
	if (iter != guidMapSchemes.end())
	{
		return false;
	}

	guidMapSchemes.insert(std::pair<UInt64, EquipScheme*>(scheme->GetID(), scheme));

	return true;
}

void    EquipSchemeMgr::OnOnline()
{
	//检查装备方案1是否生成
	auto scheme = GetScheme(EQST_EQUIP, 1);
	if (!scheme)
	{
		CreateSchemeOnOnlie();
	}
	else if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME))
	{
		FixSchemeForServiceOpen();
	}

	CLProtocol::SceneEquipSchemeSync protocl;
	for (auto it : guidMapSchemes)
	{
		EquipScheme* scheme = it.second;
		if (!scheme)
		{
			continue;
		}
		protocl.schemes.push_back(scheme->ToClient());
	}
	GetOwner()->SendProtocol(protocl);
}

void EquipSchemeMgr::CreateSchemeOnOnlie()
{
	EquipScheme* scheme = new EquipScheme;
	scheme->ownerId = GetOwner()->GetID();
	scheme->type = EQST_EQUIP;
	scheme->schemeId = 1;
	scheme->weared = 1;

	class WearEquipVisitor : public ItemVisitor
	{
	public:
		WearEquipVisitor(EquipScheme* scheme_) : m_scheme(scheme_) {}
		~WearEquipVisitor() {}

		bool operator()(Item *item)
		{
			if (item->GetType() == ITEM_EQUIP && item->GetSubType() != ST_EQUIP_WEAPON)
			{
				m_scheme->GetItemSet().OnTakeUpEquip(item);
			}
			else if (item->GetType() == ITEM_TITLE)
			{
				m_scheme->GetItemSet().OnTakeUpTitle(item);
			}
			
			return true;
		}

	public:
		EquipScheme* m_scheme;

	};

	WearEquipVisitor visitor(scheme);
	GetOwner()->GetItemMgr().VisitItems(PACK_WEAR, visitor);

	scheme->Update();
	AddOneScheme(scheme);

	return;
}

UInt32  EquipSchemeMgr::OnPlayerWearReq(UInt8 type, UInt8 schemeId, UInt8 isSync, std::vector<UInt64>& overdueIds)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME))
	{
		return	ErrorCode::ITEM_DATA_INVALID;
	}
	if (lsatOpTime > 0)
	{
		UInt64 curTime = CURRENT_TIME.MSec();
		if (curTime < lsatOpTime + 2000)
		{
			return ErrorCode::ITEM_CHANGE_SCHEME_FAIL_FREQ;
		}
		lsatOpTime = curTime;
	}
	else
	{
		lsatOpTime = CURRENT_TIME.MSec();
	}
	EquipScheme* curr_scheme = GetWearingScheme(type);
	if (!curr_scheme)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " change equip scheme fail not find wearing scheme type : " << type << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (curr_scheme->schemeId == schemeId)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " change equip scheme fail scheme->schemeId == schemeId,  type : " << type <<", schemeId : " << schemeId << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	EquipScheme* new_scheme = GetScheme(type, schemeId);
	if (new_scheme)
	{
		return OnPlayerWearScheme(curr_scheme, new_scheme, overdueIds);
	}
	else
	{
		return OnPlayerFristWearScheme(curr_scheme, type, schemeId, isSync);
	}
}

UInt32	EquipSchemeMgr::OnPlayerFristWearScheme(EquipScheme* cur_scheme, UInt8 type, UInt8 schemeId, UInt8 isSync)
{
	if (!cur_scheme)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	CLProtocol::SceneEquipSchemeSync protocl;
	std::vector<Item*> syncItems;

	//同步当前装备方案
	if (isSync)
	{
		EquipScheme* new_scheme = new EquipScheme;
		new_scheme->ownerId = GetOwner()->GetID();
		new_scheme->type = EQST_EQUIP;
		new_scheme->schemeId = schemeId;
		new_scheme->SyncFromOtherScheme(*cur_scheme);
		new_scheme->weared = 1;
		new_scheme->Update();
		AddOneScheme(new_scheme);
		cur_scheme->weared = 0;
		cur_scheme->Update();
		protocl.schemes.push_back(cur_scheme->ToClient());
		protocl.schemes.push_back(new_scheme->ToClient());
	}
	else //不同步当前方案
	{
		//检查背包格子是否足够
		if (cur_scheme->GetItemSet().title.itemId > 0 && !GetOwner()->GetItemMgr().CheckPackageCapacityEnough(PACK_TITLE, 1))
		{
			return ErrorCode::ITEM_CHANGE_SCHEME_FAIL_PACK_INSUFF;
		}

		if (!GetOwner()->GetItemMgr().CheckPackageCapacityEnough(PACK_EQUIP, cur_scheme->GetItemSet().equips.size()))
		{
			return ErrorCode::ITEM_CHANGE_SCHEME_FAIL_PACK_INSUFF;
		}

		EquipScheme* new_scheme = new EquipScheme;
		new_scheme->ownerId = GetOwner()->GetID();
		new_scheme->type = EQST_EQUIP;
		new_scheme->schemeId = schemeId;
		new_scheme->weared = 1;
		new_scheme->Update();
		AddOneScheme(new_scheme);
		cur_scheme->weared = 0;
		cur_scheme->Update();
		protocl.schemes.push_back(cur_scheme->ToClient());
		protocl.schemes.push_back(new_scheme->ToClient());

		//脱掉当前方案的装备
		for (auto it : cur_scheme->GetItemSet().equips)
		{
			EquipSchemeItem& scheme = it.second;
			Item* item = GetOwner()->GetItemMgr().FindItem(scheme.itemId);
			if (!item)
			{
				continue;
			}
			GetOwner()->GetItemMgr().ChangeEquip(item, syncItems, 0, true);
		}
		//脱掉称号
		if (cur_scheme->GetItemSet().title.itemId > 0)
		{
			Item* item = GetOwner()->GetItemMgr().FindItem(cur_scheme->GetItemSet().title.itemId);
			if (item)
			{
				GetOwner()->GetItemMgr().ChangeEquip(item, syncItems, 0, true);
			}
		}
	}

	if (protocl.schemes.size() > 0)
	{
		GetOwner()->SendProtocol(protocl);
	}
	if (syncItems.size() > 0)
	{
		GetOwner()->GetItemMgr().SyncItemBatch(syncItems);
	}
	
	return ErrorCode::SUCCESS;
}

UInt32	EquipSchemeMgr::OnPlayerWearScheme(EquipScheme* curr_scheme, EquipScheme* new_scheme, std::vector<UInt64>& overdueIds)
{
	if (!curr_scheme || !new_scheme)
	{
		return  ErrorCode::ITEM_DATA_INVALID;
	}

	std::set<UInt64> curSchemeMissEquip;
	std::set<UInt64> newSchenmeMissEquip;
	//先穿的装备
	std::vector<Item*> firstTakeEquip; 
	//次先穿的装备
	std::vector<Item*> secondTakeEquip;
	//最后脱下的装备
	std::vector<Item*> lastTakeoffEquip;
	//计算切换需要的格子数量
	UInt32 equipPackNeedGrid = 0;
	UInt32 titlePackNeddGrid = 0;
	
	std::vector<Item*> syncItems;

	//计算称号背包需要格子数量
	if (curr_scheme->GetItemSet().title.itemId > 0)
	{
		Item* title = GetOwner()->FindItem(new_scheme->GetItemSet().title.itemId);
		if (new_scheme->GetItemSet().title.itemId == 0 || (!title || title->CheckInvalid()))
		{
			titlePackNeddGrid = 1;
		}
	}

	//计算装备背包需要格子数量
	//计算旧方案里装备脱下需要的格子
	for (auto it : curr_scheme->GetItemSet().equips)
	{
		Item* item = GetOwner()->FindItem(it.second.itemId);
		if (!item ||item->CheckInvalid())
		{
			curSchemeMissEquip.insert(it.second.itemId);
			if (!item)
			{
				continue;
			}
		}
		UInt8 subtype = it.first;
		EquipSchemeItem& schemeItem = new_scheme->GetItemSet().FindEquipInEquipPack(subtype);
		Item* new_scheme_item = GetOwner()->FindItem(schemeItem.itemId);
		if (schemeItem.itemId == 0 || (new_scheme_item == NULL || new_scheme_item->CheckInvalid()))
		{
			++equipPackNeedGrid;
			lastTakeoffEquip.push_back(item);
		}
	}
	//计算新方案里装备穿上空出的格子
	for (auto it : new_scheme->GetItemSet().equips)
	{
		Item* item = GetOwner()->FindItem(it.second.itemId);
		if (item == NULL || item->CheckInvalid())
		{
			newSchenmeMissEquip.insert(it.second.itemId);
			if (item)
			{
				syncItems.push_back(item);
			}
			continue;
		}
		
		UInt8 subtype = it.first;
		EquipSchemeItem& schemeItem = curr_scheme->GetItemSet().FindEquipInEquipPack(subtype);
		if (schemeItem.itemId == 0)
		{
			firstTakeEquip.push_back(item);
			if (equipPackNeedGrid > 0)
			{
				--equipPackNeedGrid;
			}
		}
		else if(schemeItem.itemId != it.second.itemId)
		{
			secondTakeEquip.push_back(item);
		}
	}

	if (titlePackNeddGrid > 0 && !GetOwner()->GetItemMgr().CheckPackageCapacityEnough(PACK_TITLE, titlePackNeddGrid))
	{
		return ErrorCode::ITEM_CHANGE_SCHEME_FAIL_PACK_INSUFF;
	}
	if (equipPackNeedGrid > 0 && !GetOwner()->GetItemMgr().CheckPackageCapacityEnough(PACK_EQUIP, equipPackNeedGrid))
	{
		return ErrorCode::ITEM_CHANGE_SCHEME_FAIL_PACK_INSUFF;
	}

	//换称号
	if (new_scheme->GetItemSet().title.itemId > 0 && new_scheme->GetItemSet().title.itemId != curr_scheme->GetItemSet().title.itemId)
	{
		Item* curr_title = GetOwner()->GetItemMgr().FindItem(curr_scheme->GetItemSet().title.itemId);
		if (!curr_title || curr_title->CheckInvalid())
		{
			curr_scheme->GetItemSet().OnTitleMiss();
			if (curr_title)
			{
				syncItems.push_back(curr_title);
			}
		}

		Item* item = GetOwner()->GetItemMgr().FindItem(new_scheme->GetItemSet().title.itemId);
		if (item && !item->CheckInvalid())
		{
			GetOwner()->GetItemMgr().ChangeEquip(item, syncItems, 0, true);
		}
		else
		{
			new_scheme->GetItemSet().OnTitleMiss();
			if (item)
			{
				syncItems.push_back(item);
			}
			if (curr_title)
			{
				GetOwner()->GetItemMgr().ChangeEquip(curr_title, syncItems, 0, true);
			}
		}
	}
	else if (curr_scheme->GetItemSet().title.itemId > 0 && new_scheme->GetItemSet().title.itemId == 0)
	{
		Item* item = GetOwner()->GetItemMgr().FindItem(curr_scheme->GetItemSet().title.itemId);
		if (item)
		{
			GetOwner()->GetItemMgr().ChangeEquip(item, syncItems, 0, true);
		}
		if (!item || item->CheckInvalid())
		{
			curr_scheme->GetItemSet().OnTitleMiss();
		}
	}

	//换装备
	for (auto item : firstTakeEquip)
	{
		GetOwner()->GetItemMgr().ChangeEquip(item, syncItems, 0, true);
	}

	for (auto item : secondTakeEquip)
	{
		GetOwner()->GetItemMgr().ChangeEquip(item, syncItems, 0, true);
	}

	for (auto item : lastTakeoffEquip)
	{
		GetOwner()->GetItemMgr().ChangeEquip(item, syncItems, 0, true);
	}

	for (auto equip : newSchenmeMissEquip)
	{
		new_scheme->GetItemSet().OnEquipMiss(equip);
		overdueIds.push_back(equip);
	}

	for (auto equip : curSchemeMissEquip)
	{
		curr_scheme->GetItemSet().OnEquipMiss(equip);
		overdueIds.push_back(equip);
	}

	CLProtocol::SceneEquipSchemeSync protocl;
	new_scheme->weared = 1;
	new_scheme->Update();
	curr_scheme->weared = 0;
	curr_scheme->Update();
	protocl.schemes.push_back(curr_scheme->ToClient());
	protocl.schemes.push_back(new_scheme->ToClient());
	GetOwner()->SendProtocol(protocl);

	GetOwner()->GetItemMgr().SyncItemBatch(syncItems);

	return ErrorCode::SUCCESS;
}

void EquipSchemeMgr::GetRaceInfo(std::vector<RaceEquipScheme>& raceEqScheme)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME))
	{
		return;
	}

	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME)
		&& IsUnWearedSchemeEmpty())
	{
		return;
	}

	for (auto it : guidMapSchemes)
	{
		EquipScheme* scheme = it.second;
		if (!scheme)
		{
			continue;
		}
		if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME)
			&& scheme->weared == 0)
		{
			continue;
		}
		raceEqScheme.push_back(scheme->GetRaceInfo());
	}
}

void EquipSchemeMgr::GetAllEquipIds(std::set<UInt64>& equipIds)
{
	for (auto it : schemes)
	{
		EquipScheme* scheme = it.second;
		if (!scheme)
		{
			continue;
		}

		if (scheme->GetItemSet().title.itemId > 0)
		{
			equipIds.insert(scheme->GetItemSet().title.itemId);
		}

		for (auto schemeItem : scheme->GetItemSet().equips)
		{
			UInt64 equipId = schemeItem.second.itemId;
			if (equipIds.find(equipId) != equipIds.end())
			{
				continue;
			}
			equipIds.insert(equipId);
		}
	}
}

//处理穿上装备
void EquipSchemeMgr::OnTakeUpEquip(Item* item)
{
	if (!item)
	{
		return;
	}
	if (item->GetSubType() == ST_EQUIP_WEAPON)
	{
		return;
	}
	EquipScheme* scheme = GetWearingScheme(EQST_EQUIP);
	if (!scheme)
	{
		return;
	}

	if (item->GetType() == ITEM_EQUIP)
	{
		scheme->GetItemSet().OnTakeUpEquip(item);
	}
	else if (item->GetType() == ITEM_TITLE)
	{
		scheme->GetItemSet().OnTakeUpTitle(item);
	}
	scheme->Update();
	scheme->SyncToClient(GetOwner());
	return;
}

//处理卸下装备
void EquipSchemeMgr::OnTakeOffEquip(Item* item, bool sync)
{
	if (!item)
	{
		return;
	}

	if (item->GetSubType() == ST_EQUIP_WEAPON)
	{
		return;
	}

	EquipScheme* scheme = GetWearingScheme(EQST_EQUIP);
	if (!scheme)
	{
		return;
	}

	if (item->GetType() == ITEM_EQUIP)
	{
		scheme->GetItemSet().OnTakeOffEquip(item);
	}
	else if (item->GetType() == ITEM_TITLE)
	{
		scheme->GetItemSet().OnTakeOffTitle();
	}
	scheme->Update();
	if (sync)
	{
		scheme->SyncToClient(GetOwner());
	}
	return;
}

bool EquipSchemeMgr::IsEquipInScheme(UInt64 ItemId)
{
	for (auto it : schemes)
	{
		EquipScheme* scheme = it.second;
		if (!scheme)
		{
			continue;
		}
		if (scheme->GetItemSet().title.itemId == ItemId)
		{
			return true;
		}
		for (auto iter : scheme->GetItemSet().equips)
		{
			if (iter.second.itemId == ItemId)
			{
				return true;
			}
		}
	}

	return false;
}

bool EquipSchemeMgr::IsUnWearedSchemeEmpty()
{
	for (auto it : schemes)
	{
		EquipScheme* scheme = it.second;
		if (!scheme)
		{
			continue;
		}
		if (scheme->weared == 0 && scheme->IsEmpty())
		{
			return true;
		}
	}
	return false;
}

UInt32 EquipSchemeMgr::GetSchemeOfEquip(UInt64 ItemId)
{
	UInt32 schemeIds = 0;
	for (auto it : schemes)
	{
		EquipScheme* scheme = it.second;
		if (!scheme)
		{
			continue;
		}
		if (scheme->GetItemSet().title.itemId == ItemId)
		{
			schemeIds |= 1 << (scheme->GetSchemeId() - 1);
			continue;
		}
		for (auto iter : scheme->GetItemSet().equips)
		{
			if (iter.second.itemId == ItemId)
			{
				schemeIds |= 1 << (scheme->GetSchemeId() - 1);
				continue;
			}
		}
	}
	return schemeIds;
}

void EquipSchemeMgr::FixSchemeForServiceOpen(bool notify)
{
	EquipScheme* scheme = GetWearingScheme(EQST_EQUIP);
	if (!scheme) return;

	std::set<UInt64> schemeEqs;
	scheme->GetEquipIds(schemeEqs);

	class WearEquipVisitor : public ItemVisitor
	{
	public:
		WearEquipVisitor(std::set<UInt64>& schemeEqs) : m_schemeEqs(schemeEqs) {}
		~WearEquipVisitor() {}

		bool operator()(Item *item)
		{
			if (item->GetSubType() == ST_EQUIP_WEAPON)
			{
				return true;
			}
			if (m_schemeEqs.find(item->GetID()) == m_schemeEqs.end())
			{
				m_addItems.push_back(item);
			}

			return true;
		}

	public:
		std::set<UInt64>& m_schemeEqs;
		std::vector<Item *> m_addItems;
	};
	WearEquipVisitor visitor(schemeEqs);
	GetOwner()->GetItemMgr().VisitItems(PACK_WEAR, visitor);

	std::vector<UInt64> delItems;
	for (auto eqId : schemeEqs)
	{
		auto item = GetOwner()->FindItem(eqId);
		if (!item || item->GetPos().pack != PACK_WEAR)
		{
			delItems.push_back(eqId);
		}
	}

	for (auto item : visitor.m_addItems)
	{
		scheme->GetItemSet().OnTakeUpItem(item);
	}
	for (auto itemId : delItems)
	{
		scheme->GetItemSet().OnItemMiss(itemId);
	}

	if (visitor.m_addItems.size() > 0 || delItems.size() > 0)
	{
		scheme->Update();
		if (notify)
		{
			CLProtocol::SceneEquipSchemeSync protocl;
			protocl.schemes.push_back(scheme->ToClient());
			GetOwner()->SendProtocol(protocl);
		}

	}
}