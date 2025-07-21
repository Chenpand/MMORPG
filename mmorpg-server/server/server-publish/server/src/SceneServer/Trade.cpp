#include "Trade.h"

#include <CLTradeProtocol.h>
#include "Player.h"

Trade::Trade(Player* player1, Player* player2)
{
	m_TradeInfo[0].player = player1;
	m_TradeInfo[1].player = player2;

	player1->SetTrade(this);
	player2->SetTrade(this);
}

Trade::~Trade()
{
	m_TradeInfo[0].player->SetTrade(NULL);
	m_TradeInfo[1].player->SetTrade(NULL);
}

void Trade::Open()
{
	CLProtocol::SceneTradeNotifyBegin notify;
	notify.otherName = m_TradeInfo[1].player->GetName();
	m_TradeInfo[0].player->SendProtocol(notify);

	notify.otherName = m_TradeInfo[0].player->GetName();
	m_TradeInfo[1].player->SendProtocol(notify);
}

void Trade::Cancel()
{
	m_TradeInfo[0].status = TRADE_STATUS_CANCEL;
	m_TradeInfo[1].status = TRADE_STATUS_CANCEL;
	SyncStatus();
	delete this;
}

void Trade::Close()
{
// 	if(m_TradeInfo[0].status != TRADE_STATUS_CONFIRM
// 		|| m_TradeInfo[1].status != TRADE_STATUS_CONFIRM) return;
// 
// 	for(int i = 0; i < 2; ++i)
// 	{
// 		TradeInfo* info = &m_TradeInfo[i];
// 		TradeInfo* otherInfo = &(m_TradeInfo[(i+1)%2]);
// 
// 		RewardGroup* rewardGroup = info->player->GetRewardGroup();
// 		for(std::map<ObjID_t, UInt16>::iterator iter = otherInfo->items.begin();
// 			iter != otherInfo->items.end(); ++iter)
// 		{
// 			Item* item = otherInfo->player->FindCarryOnItem(iter->first);
// 			if(item == NULL || iter->second > item->GetNum())
// 			{
// 				Cancel();
// 				return;
// 			}
// 			rewardGroup->AddReward(item->GetDataID(), iter->second);
// 		}
// 		if (ErrorCode::SUCCESS != info->player->CheckAddRewards(rewardGroup))
// 		{
// 			info->player->SendNotify(302);
// 			otherInfo->player->SendNotify(303);
// 			Cancel();
// 			return;
// 		}
// 		
// 		if(info->silver > info->player->GetGold())
// 		{
// 			Cancel();
// 			return;
// 		}
// 	}
// 
// 	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_INVALID);
// 
// 	for(int i = 0; i < 2; ++i)
// 	{
// 		TradeInfo* info = &m_TradeInfo[i];
// 		TradeInfo* otherInfo = &(m_TradeInfo[(i+1)%2]);
// 
// 		for(std::map<ObjID_t, UInt16>::iterator iter = otherInfo->items.begin();
// 			iter != otherInfo->items.end(); ++iter)
// 		{
// 			Item* item = otherInfo->player->FindCarryOnItem(iter->first);
// 
// 			if(iter->second == item->GetNum())
// 			{
// 				
// 				otherInfo->player->RemoveItemDirect(reason.c_str(), item);
// 				if (info->player->AddItemDirect(reason.c_str(), item) == 0)
// 				{
// 					Item::DestroyItem(item);
// 				}
// 			}
// 			else
// 			{
// 				item->SetNum(item->GetNum() - iter->second);
// 				item->SyncProperty(otherInfo->player);
// 				info->player->AddItem(reason.c_str(), item->GetDataID(), iter->second, item->GetBind(), item->GetQuality());
// 			}
// 		}
// 
// 		otherInfo->player->RemoveGold(reason.c_str(), otherInfo->silver);
// 		info->player->AddGold(reason.c_str(), otherInfo->silver);
// 	}
// 
// 	m_TradeInfo[0].player->SendNotify(304);
// 	m_TradeInfo[1].player->SendNotify(304);
// 	delete this;
}

void Trade::SetSilver(Player* player, UInt32 silver)
{
	TradeInfo* info = GetInfo(player);
	if(info == NULL) return;
	if(info->status != TRADE_STATUS_UNLOCK) return;

	if(silver > player->GetGold()) return;
	info->silver = silver;
	SyncSilver();
}

void Trade::AddItem(Player* player, ObjID_t itemid, UInt16 num)
{
	TradeInfo* info = GetInfo(player);
	if(info == NULL) return;
	if(info->status != TRADE_STATUS_UNLOCK) return;

	Item* item = player->FindCarryOnItem(itemid);
	if(item == NULL /*|| item->GetPos().pack != PACK_NORMAL*/) return;
	if(num == 0 || num > item->GetNum()) return;

	//ÒÑ¾­´æÔÚ
	if(info->items.find(itemid) != info->items.end()) return;
	if(info->items.size() >= TRADE_GRID_NUM) return;

	info->items.insert(std::make_pair(itemid, num));

	CLProtocol::SceneTradeSyncSelfItems syncSelf;
	syncSelf.items = info->items;
	player->SendProtocol(syncSelf);

	Avalon::Packet* syncOther = Avalon::Packet::Create();
	syncOther->SetID(CLProtocol::SCENE_TRADE_NOTIFY_OTHER_ADDITEM);
	Avalon::NetOutputStream stream(syncOther->GetBuffer(), syncOther->GetMaxSize());
	try
	{
		stream & item->GetID() & item->GetDataID() & num;
		item->Encode(stream, SST_WATCH, false);
	}
	catch(const Avalon::NetException&)
	{
		Avalon::Packet::Destroy(syncOther);
		return;
	}
	syncOther->SetSize(UInt16(stream.GetPos()));
	TradeInfo* otherInfo = GetOtherInfo(player);
	otherInfo->player->SendPacket(syncOther);
	Avalon::Packet::Destroy(syncOther);
}

void Trade::RemoveItem(Player* player, ObjID_t itemid)
{
	TradeInfo* info = GetInfo(player);
	if(info == NULL) return;
	if(info->status != TRADE_STATUS_UNLOCK) return;

	if(info->items.find(itemid) == info->items.end()) return;

	info->items.erase(itemid);

	CLProtocol::SceneTradeSyncSelfItems syncSelf;
	syncSelf.items = info->items;
	player->SendProtocol(syncSelf);

	CLProtocol::SceneTradeNotifyOtherRemoveItem notify;
	notify.itemid = itemid;
	TradeInfo* otherInfo = GetOtherInfo(player);
	otherInfo->player->SendProtocol(notify);
}

void Trade::SetStatus(Player* player, UInt8 status)
{
	TradeInfo* info = GetInfo(player);
	if(info == NULL) return;
	if(info->status == status) return;
	
	TradeInfo* otherInfo = GetOtherInfo(player);
	switch(status)
	{
	case TRADE_STATUS_UNLOCK:
		{
			if(info->status != TRADE_STATUS_LOCK) return;
			info->status = TRADE_STATUS_UNLOCK;
			otherInfo->status = TRADE_STATUS_UNLOCK;
			SyncStatus();
		}
		break;
	case TRADE_STATUS_LOCK:
		{
			if(info->status != TRADE_STATUS_UNLOCK) return;
			info->status = TRADE_STATUS_LOCK;
			SyncStatus();
		}
		break;
	case TRADE_STATUS_CONFIRM:
		{
			if(info->status != TRADE_STATUS_LOCK
				||otherInfo->status < TRADE_STATUS_LOCK) return;
			info->status = TRADE_STATUS_CONFIRM;
			SyncStatus();
			Close();
		}
		break;
	case TRADE_STATUS_CANCEL:
		{
			Cancel();
		}
		break;
	}
}

void Trade::SyncStatus()
{
	CLProtocol::SceneTradeSyncStatus sync;
	sync.selfStatus = m_TradeInfo[0].status;
	sync.otherStatus = m_TradeInfo[1].status;
	m_TradeInfo[0].player->SendProtocol(sync);

	sync.selfStatus = m_TradeInfo[1].status;
	sync.otherStatus = m_TradeInfo[0].status;
	m_TradeInfo[1].player->SendProtocol(sync);
}

void Trade::SyncSilver()
{
	CLProtocol::SceneTradeSyncSilver sync;
	sync.selfSilver = m_TradeInfo[0].silver;
	sync.otherSilver = m_TradeInfo[1].silver;
	m_TradeInfo[0].player->SendProtocol(sync);

	sync.selfSilver = m_TradeInfo[1].silver;
	sync.otherSilver = m_TradeInfo[0].silver;
	m_TradeInfo[1].player->SendProtocol(sync);
}

TradeInfo* Trade::GetInfo(Player* player)
{
	for(int i = 0; i < 2; ++i)
	{
		if(m_TradeInfo[i].player == player) return &(m_TradeInfo[i]);
	}
	return NULL;
}

TradeInfo* Trade::GetOtherInfo(Player* player)
{
	for(int i = 0; i < 2; ++i)
	{
		if(m_TradeInfo[i].player == player) return &(m_TradeInfo[(i+1)%2]);
	}
	return NULL;
}
