#include "SortListMgr.h"

#include <CLSortListProtocol.h>
#include <CLCenterProtocol.h>
#include "SSRouter.h"
#include "PlayerMgr.h"
#include "ActivityMgr.h"

/**
 *@brief 查询装备回调
 */
class SelectEquipCallback : public CLRecordCallback
{
public:
	SelectEquipCallback(UInt8 sortType, ObjID_t watcher, ObjID_t id, const std::string& ownerName)
		:m_SortType(sortType), m_Watcher(watcher),m_Id(id),m_OwnerName(ownerName){}

	void OnFinished()
	{
		if(!NextRow())
		{
			SortListMgr::Instance()->OnSortEntryDeleted(m_SortType, m_Id);
			return;
		}
		SortListMgr::Instance()->OnSelectEquipRet(m_Watcher, m_OwnerName, this);
	}

private:
	UInt8	m_SortType;
	ObjID_t	m_Watcher;
	ObjID_t	m_Id;
	std::string	m_OwnerName;
};

SortListMgr::SortListMgr()
{
	memset(m_MinValues, 0, sizeof(m_MinValues));
}

SortListMgr::~SortListMgr()
{
}

void SortListMgr::SetEntries(UInt8 type, const std::vector<ObjID_t>& ids)
{
	if(type >= SORTLIST_MAX) return;

	m_Ids[type].clear();
	m_Ids[type].insert(ids.begin(), ids.end());
}

void SortListMgr::SetMinValue(UInt8 type, UInt64 value)
{
	if(type >= SORTLIST_MAX) return;

	m_MinValues[type] = value;
}

void SortListMgr::AddEntry(UInt8 type, ObjID_t id)
{
	if(type >= SORTLIST_MAX) return;

	m_Ids[type].insert(id);
}


void SortListMgr::RemoveEntry(UInt8 type, ObjID_t id)
{
	if(type >= SORTLIST_MAX) return;

	m_Ids[type].erase(id);
}

void SortListMgr::WatchData(ObjID_t watcher, UInt8 type, ObjID_t id, UInt32 dataId, ObjID_t owner, const std::string& ownerName, UInt32 accid)
{
	Player* pOwner = PlayerMgr::Instance()->FindNormalPlayer(owner);
	
	if(IsPetSortList(type))
	{
		/*
		if(pOwner != NULL)
		{
			SleepingPet* pet = pOwner->GetPetMgr().FindPet(id);
			if(pet == NULL)
			{
				OnSortEntryDeleted(type, id);
				return;
			}

			SendPetDataToWorld(watcher, pOwner->GetName(), pet);
		}
		else
		{
			CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_pet");
			selectCmd->PutCond("guid", id);
			CLRecordClient::Instance()->SendCommand(selectCmd, new SelectSortListPetCallback(type, watcher, id, ownerName));
		}
		*/
	}
	else if(IsEquipSortList(type))
	{
		if(pOwner != NULL)
		{
			Item* item = pOwner->FindItem(id);
			if(item == NULL)
			{
				OnSortEntryDeleted(type, id);
				return;
			}

			SendEquipDataToWorld(watcher, pOwner->GetName(), item);
		}
		else //如果道具主人不在scene server，则去record server拉取道具信息
		{
			CLSelectCommand *selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_item");
			selectCmd->PutCond("guid", id);
			CLRecordClient::Instance()->SendCommand(selectCmd, new SelectEquipCallback(type, watcher, id, ownerName));
		}
	}
}

UInt32 SortListMgr::GetResetMatchScore(UInt32 oldScore)
{
	return 1000 + (UInt32)round((oldScore - 1000) * 0.35f);
}

void SortListMgr::OnSortValueChanged(UInt8 sortType, ObjID_t id, const std::string& name, UInt8 quality, UInt8 occu, ObjID_t ownerId, UInt16 level, UInt32 value1, UInt32 value2, UInt32 value3)
{
	if(sortType >= SORTLIST_MAX) return;

	IdSet& ids = m_Ids[sortType];
	if(ids.find(id) != ids.end()
		|| ids.size() < GetSortListSize(sortType) 
		|| SortListEntry::GetSortFactor(sortType, id, value1, value2, value3) >= m_MinValues[sortType])
	{
		CLProtocol::WorldSortListUpdateReq req;
		req.entry.sortType = sortType;
		req.entry.id = id;
		req.entry.name = name;
		req.entry.quality = quality;
		req.entry.occu = occu;
		req.entry.ownerId = ownerId;
		req.entry.level = level;			//modified by aprilliu, 2017.04.22 
		req.entry.value1 = value1;
		req.entry.value2 = value2;
		req.entry.value3 = value3;
		Router::SendToWorld(req);
	}
}

void SortListMgr::OnWorldSortValueChanged(UInt8 sortType, ObjID_t id, const std::string& name, UInt8 vip, UInt32 value1, UInt32 value2, UInt32 value3)
{
	if(!IsWorldSortList(sortType)) return;

	CLProtocol::WorldSortListUpdateReq req;
	req.entry.sortType = sortType;
	req.entry.id = id;
	req.entry.name = name;
	req.entry.vip = vip;
	req.entry.zoneId = ZONE_ID;
	req.entry.value1 = value1;
	req.entry.value2 = value2;
	req.entry.value3 = value3;
	Router::SendToSortList(req);
}

void SortListMgr::OnOwnerChanged(UInt8 sortType, ObjID_t entryId, ObjID_t ownerId, std::string ownerName)
{
	if (sortType >= SORTLIST_MAX) return;
	CLProtocol::WorldSortListOwnerChangedReq req;
	req.type = sortType;
	req.entryId = entryId;
	req.ownerId = ownerId;
	req.ownerName = ownerName;
	Router::SendToWorld(req);
}

void SortListMgr::OnSortEquipValueChanged(UInt8 sortType, ObjID_t id, const std::string& name, UInt8 quality, UInt8 occu, ObjID_t ownerId, UInt16 level, UInt32 value1, UInt32 value2, UInt32 value3, UInt8 equipType, UInt8 enhanceType)
{
	if (sortType >= SORTLIST_MAX) return;

	IdSet& ids = m_Ids[sortType];
	if (ids.find(id) != ids.end()
		|| ids.size() < GetSortListSize(sortType)
		|| SortListEntry::GetSortFactor(sortType, id, value1, value2, value3) >= m_MinValues[sortType])
	{
		CLProtocol::WorldSortListUpdateReq req;
		req.entry.sortType = sortType;
		req.entry.id = id;
		req.entry.name = name;
		req.entry.quality = quality;
		req.entry.occu = enhanceType;
		req.entry.ownerId = ownerId;
		req.entry.level = level;			//modified by aprilliu, 2017.04.22 
		req.entry.value1 = value1;
		req.entry.value2 = value2;
		req.entry.value3 = value3;
		req.entry.sex = equipType;

		Router::SendToWorld(req);
	}
}

void SortListMgr::OnSortEntryDeleted(UInt8 sortType, ObjID_t id)
{
	if(sortType >= SORTLIST_MAX) return;

	IdSet& ids = m_Ids[sortType];
	if(ids.find(id) != ids.end())
	{
		CLProtocol::WorldSortListDeleteEntryReq req;
		req.sortType = sortType;
		req.id = id;
		Router::SendToWorld(req);
	}
}

void SortListMgr::OnSelectEquipRet(ObjID_t watcher, const std::string& owner, CLRecordCallback* callback)
{
	Item* item = Item::CreateItem(callback->GetData("itemid"));
	if(item == NULL) return;

	item->SetID(callback->GetKey());
	item->SetDBData(callback);
	//设置装备和时装的属性
	item->SetAttribute();
	SendEquipDataToWorld(watcher, owner, item);

	Item::DestroyItem(item);
}

void SortListMgr::OnActivitySortValueChanged(UInt8 sortType, const std::string& platform, const std::string& openId, UInt32 zoneId, const std::string& zoneName, ObjID_t id, UInt32 accId, const std::string& name, UInt8 occu, UInt16 level, UInt32 value1, UInt32 value2, UInt32 value3)
{
	//if (value1 < SORTLIST_ACTIVITY_MIN_SEASON_LEVEL) return;

	if (!IsInActivitySort(platform)) return;

	CLProtocol::CenterActivityYearSortListSync sync;
	sync.sortType = sortType;
	sync.platform = platform;
	sync.openId = openId;
	sync.zoneName = zoneName;
	sync.zoneId = zoneId;
	sync.id = id;
	sync.name = name;
	sync.accId = accId;
	sync.occu = occu;
	sync.level = level;
	sync.value1 = value1;
	sync.value2 = value2;
	sync.value3 = value3;

	Router::SendToCenter(sync);
}

bool SortListMgr::IsInActivitySort(const std::string& platform)
{
	if (platform == "hw" && ActivityMgr::Instance()->IsInActivity(ActiveID::AID_HUAWEI_YEAR_SORTLIST))
		return true;
	else if (platform == "oppo" && ActivityMgr::Instance()->IsInActivity(ActiveID::AID_OPPO_YEAR_SORTLIST))
		return true;
	else if (platform == "vivo" && ActivityMgr::Instance()->IsInActivity(ActiveID::AID_VIVO_YEAR_SORTLIST))
		return true;
	else if (platform == "xiaomi" && ActivityMgr::Instance()->IsInActivity(ActiveID::AID_XIAOMI_YEAR_SORTLIST))
		return true;
	else if (platform == "meizu" && ActivityMgr::Instance()->IsInActivity(ActiveID::AID_MEIZU_YEAR_SORTLIST))
		return true;
	return false;
}

void SortListMgr::SendEquipDataToWorld(ObjID_t watcher, const std::string& owner, Item* item)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SORTLIST_WATCH_DATA_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & UInt8(CHAT_LINKDATA_ITEM) & watcher & item->GetID() & item->GetDataID()/* & owner*/;	// 不需要owner,owner存在榜单数据里的
		item->PackWatchData(stream);
	}
	catch(Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());
	Router::SendToWorld(packet);
	Avalon::Packet::Destroy(packet);
}