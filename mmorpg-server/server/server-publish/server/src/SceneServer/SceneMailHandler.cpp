#include "SceneMailHandler.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "CLRelationDefine.h"
#include "CLSystemValueDataEntry.h"
#include "SSServiceSwitchMgr.h"

void HandleProtocol(CLProtocol::SceneMailCheckPackageReq& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.playerId);
	if(player == NULL) return;

	CLProtocol::SceneMailCheckPackageRet ret;
	ret.playerId = req.playerId;

	RewardGroup* rewardGroup = player->GetRewardGroup();
	for(std::map<ObjID_t, std::vector<ItemReward> >::reverse_iterator iter = req.items.rbegin(); iter != req.items.rend(); ++iter)
	{
		const ItemRewardVec& items = iter->second;

		ItemRewardVec getItems;
		bool isContinue = false;
		for (size_t i = 0; i < items.size(); ++i)
		{
			ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(items[i].id);
			if (!data)
			{
				continue;
			}

			if (data->getLimitNum > 0)
			{
				if (player->GetItemNum(data->id) + items[i].num > data->getLimitNum)
				{
					player->SendNotify(9102, data->name.c_str());
					isContinue = true;
					break;
				}
			}

			ItemReward item(items[i].id, items[i].num, items[i].qualityLv, items[i].strenth, items[i].auctionCoolTimeStamp, items[i].equipType, items[i].auctionTransTimes);
			getItems.push_back(item);
		}

		if (getItems.size() == 0 || isContinue == true)
		{
			continue;
		}

		rewardGroup->AddRewards(getItems);
		UInt32 nRes = player->CheckAddRewards(rewardGroup);
		if (ErrorCode::SUCCESS != nRes)
		{
			if (ret.mailids.empty())
			{
				player->SendNotify(nRes);
			}
			break;
		}
		else
		{
			ret.mailids.push_back(iter->first);
		}
	}

	Router::SendToWorld(ret);
}

void MailGiveItemsFunction::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	ObjID_t playerId = 0;
	UInt32 silver = 0;
	std::vector<MailItemReward>	items;
	try
	{
		stream & playerId & silver & items;
	}
	catch(Avalon::NetException&)
	{
		ErrorLogStream << "playerId(" << playerId << ")" << " decode mail data failed:" << "!" << LogStream::eos;
		return;
	}
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(playerId);
	if (player == NULL)
	{
		ErrorLogStream << "playerId(" << playerId << ")" << " not normal player" << "!" << LogStream::eos;
		return;
	}

	//若是好友赠送邮件，判断收取次数
	for (size_t i = 0; i < items.size(); ++i)
	{
		if (items[i].id == FRIEND_GIVE_GIFT)
		{
			UInt32 num = player->GetCounter(COUNTER_GET_FRIEND_GIVE);
			if (num >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FRIEND_GIVE_TAKE_NUM))
			{
				player->SendNotify(1099);	//领取好友上限到了
				continue;
			}
			else
			{
				player->IncCounter(COUNTER_GET_FRIEND_GIVE, 1);
			}
		}
		std::string reason = items[i].reason;
		if (reason.empty())
		{
			reason = GetReason(SOURCE_TYPE_MAIL_COLLECTION_ATTACHMENT);
		}
		player->AddGold(reason.c_str(), silver);
		RewardGroup group;
		ReasonInfo reasonInfo;
		if (ParseReason(reason.c_str(), reasonInfo) && reasonInfo.type == SOURCE_TYPE_GM)
		{
			group.AddReward(items[i].id, items[i].num, 0, items[i].strenth, items[i].auctionCoolTimeStamp, EQUIP_NORMAL, items[i].auctionTransTimes);
		}
		else
		{
			group.AddReward(items[i].id, items[i].num, 0, items[i].strenth, items[i].auctionCoolTimeStamp, EQUIP_NORMAL, items[i].auctionTransTimes);
		}
		
		player->AddRewardsByMailGive(reason.c_str(), &group, true, MAIL_VALID_DAY, false);
	}

	std::string reason;
	ObjID_t id = 0;
	stream & id;
	while(id != 0)
	{
		UInt32	itemid = 0;
		UInt8   sealState = 0;
		UInt32  sealCount = 0;
		stream & itemid & reason & sealState & sealCount;
		Item* item = Item::CreateItem(itemid);
		if (item == NULL)
		{
			ErrorLogStream << PLAYERINFO(player) << " create item(" << itemid << ") error " << "!" << LogStream::eos;
			return;
		}

		item->SetID(id);

		try
		{
			item->Decode(stream);
		}
		catch (const Avalon::NetException& e)
		{
			ErrorLogStream << PLAYERINFO(player) << " decode item(" << itemid << ") data failed:" << e.what() << "!" << LogStream::eos;
			Item::DestroyItem(item);
			return;
		}

		if (item->CheckInvalid())
		{
			player->SendNotify(10041);
			Item::DestroyItem(item);
			stream & id;
			continue;
		}
		item->SetAttribute();
		item->SetOwnerId(player->GetID());
		item->SetAccId(player->GetAccID());
		item->SetSealState(sealState);
		item->SetSealCount(sealCount);

		if (reason.empty())
		{
			reason = GetReason(SOURCE_TYPE_MAIL_COLLECTION_ATTACHMENT);
		}
		//没有这个方法了  huchenhui
		//item->SetupEquipBase();
		UInt64 itemId = item->GetID();
		UInt32 itemDataId = item->GetDataID();
		ItemType itemType = (ItemType)item->GetType();
		ItemSubType itemSubType = (ItemSubType)item->GetSubType();
		std::string itemName = item->GetName();
		UInt32 itemQuality = (UInt32)item->GetQuality();

		//UInt32 oldNumber = player->GetItemNum(itemDataId);
		Int32  number = item->GetNum();

		if (player->AddItemDirect(reason.c_str(), item, true, true) == 0)
		{
			ErrorLogStream << PLAYERINFO(player) << " decode item(" << item->GetID() << "|" << itemId << "|" << item->GetNum() 
				<< "|" << item->GetAuctionCoolTimeStamp() << ") add item direct failed!" << LogStream::eos;
			Item::DestroyItem(item);
			return;
		}
		else
		{
			ReasonInfo reasonInfo;
			if (ParseReason(reason.c_str(), reasonInfo) && reasonInfo.type == SOURCE_TYPE_ACTION_BUY
				&& item->IsTreas() && SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
			{
				item->SetItemLock(ILT_AUCTION_RETURN_LOCK);
				item->SyncProperty(player);
			}
			//player->SendItemUdpLog(reason.c_str(), itemId, itemDataId, itemType, itemSubType, itemName, itemQuality, number, 0, item->GetStrengthen(), item->GetEquipType(), item->GetEnhanceType());
		}
		stream & id;
	}
}

void HandleProtocol(CLProtocol::SceneMailRemoveCostReq& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.playerId);
	if(player == NULL) return;

	if(player->GetGold() < (req.silver + req.cost))
	{
		player->SendNotify(404);
		return;
	}

	for(std::map<ObjID_t,UInt16>::iterator iter = req.items.begin();
		iter != req.items.end(); ++iter)
	{
		Item* item = player->FindCarryOnItem(iter->first);
		if(item == NULL/* || item->GetPos().pack != PACK_NORMAL*/
			||item->GetBind() != 0 || iter->second > item->GetNum())
		{
			return;
		};
	}

	player->RemoveGoldBoth(GetReason(SOURCE_TYPE_MAIL).c_str(), req.silver + req.cost);

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_MAIL_REMOVECOST_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & req.playerId & req.receiveId & req.accid & req.receiveName & req.replyTimes & req.title & req.content & req.silver;
		for(std::map<ObjID_t,UInt16>::iterator iter = req.items.begin();
			iter != req.items.end(); ++iter)
		{
			Item* item = player->FindCarryOnItem(iter->first);

			UInt32 oldNum = item->GetNum();
			item->SetNum(iter->second);
			stream & item->GetDataID();
			item->Encode(stream, SST_WORLD, false);
			item->SetNum(oldNum);

			player->ReduceItemNum(GetReason(SOURCE_TYPE_MAIL).c_str(), item, iter->second);
		}
		stream & UInt32(0);
	}
	catch(const Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());
	Router::SendToWorld(packet);
	Avalon::Packet::Destroy(packet);
}
