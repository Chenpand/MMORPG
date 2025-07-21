#include "SceneGuildHandler.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "SceneSysNotify.h"
#include "SSServiceSwitchMgr.h"

void HandleProtocol(CLProtocol::WorldGuildSyncToScene& protocol)
{
	auto player = PlayerMgr::Instance()->FindPlayer(protocol.id);
	if (!player)
	{
		ErrorLogStream << "[Guild]player(" << protocol.id << ") offline." << LogStream::eos;
		return;
	}

	player->SetGuildId(protocol.guildId);
	player->SetGuildLvl(protocol.guildLvl);
	player->SetGuildName(protocol.name);
	player->SetGuildPost(protocol.post);
	player->SetGuildEnterTime(protocol.enterTime);
	player->SetGuildShopID(protocol.shopId);
	player->SetGuildBattleScore(protocol.guildBattleScore);
	player->SetGuildBattleNumber(protocol.guildBattleNumber);
	player->SetGuildBattleLotteryStatus(protocol.guildBattleLotteryStatus);
	player->SetGuildBattleReward(protocol.guildBattleRewardMask);
	player->SetGuildOccupyCrossTerrId(protocol.occupyCrossTerrId);
	player->SetGuildOccupyTerrId(protocol.occupyTerrId);

	for (auto building : protocol.buildings)
	{
		player->SetGuildBuildingLevel((GuildBuildingType)building.type, building.level);
	}

	// 退出公会
	if (player->GetGuildPost() == GUILD_POST_INVALID)
	{
		// 清除所有公会红点
		player->ClearRedPoint(RED_POINT_GUILD_SHOP);
		player->ClearRedPoint(RED_POINT_GUILD_REQUESTER);
		player->ClearRedPoint(RED_POINT_GUILD_ORZ);
		player->ClearRedPoint(RED_POINT_GUILD_TABLE);
		player->ClearRedPoint(RED_POINT_GUILD_MERGER);
	}
	else
	{
		if (player->GetCount(COUNTER_GUILD_ORZ) == 0)
		{
			player->AddRedPoint(RED_POINT_GUILD_ORZ);
		}
		if (player->GetCount(COUNTER_GUILD_TABLE) == 0)
		{
			player->AddRedPoint(RED_POINT_GUILD_TABLE);
		}
	}
}

void HandleProtocol(CLProtocol::WorldGuildSyncContribution& protocol)
{
	auto player = PlayerMgr::Instance()->FindPlayer(protocol.id);
	if (!player)
	{
		ErrorLogStream << "player(" << protocol.id << ") sync contribution, isAdd(" << protocol.isAdd 
						<< ") value(" << protocol.value << ") reason(" << protocol.reason << ")." << LogStream::eos;
		return; 
	}

	if (protocol.isAdd)
	{
		player->AddGuildContribution(protocol.reason.c_str(), protocol.value);
	}
	else
	{
		player->RemoveGuildContribution(protocol.reason.c_str(), protocol.value);
	}
}

void HandleProtocol(CLProtocol::WorldGuildUpdateSkill& protocol)
{
	auto player = PlayerMgr::Instance()->FindPlayer(protocol.id);
	if (!player)
	{
		return;
	}

	if (protocol.skillType == SKILL_CONFIG_PVP)
	{
		player->GetPvpSkillMgr().AddFreeSkill(protocol.skill.id, protocol.skill.level);
	}
	else
	{
		player->GetSkillMgr().AddFreeSkill(protocol.skill.id, protocol.skill.level);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneGuildExchangeReq& req)
{
	if (!player)
	{
		return;
	}

	player->RequestGuildExchange();
}

void HandleProtocol(CLProtocol::WorldGuildGiveItemReq& req)
{
	UInt32 result = ErrorCode::GUILD_BATTLE_GIVE_REWARD_NOT_MEMBER;
	Player* player = PlayerMgr::Instance()->FindPlayer(req.playerId);
	if (player != NULL)
	{
		RewardGroup* rewards = player->GetRewardGroup();
		rewards->AddRewards(req.itemReward);
		result = player->CheckAddRewards(rewards);
		if (result == ErrorCode::SUCCESS)
		{
			player->AddRewards(GetReason(LogSourceType::SOURCE_TYPE_GUILD_BATTLE_SCORE, req.boxId).c_str(), rewards, true);
			//player->SetGuildBattleReward(req.boxId);
		}
	}

	CLProtocol::WorldGuildGiveItemRes res;
	res.playerId = req.playerId;
	res.boxId = req.boxId;
	res.result = result;
	Router::SendToWorld(res);
	
}

void HandleProtocol(CLProtocol::WSAddGuildStorageItemReq& req)
{
	UInt32 code = ErrorCode::SUCCESS;

	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.playerUid);
	if (!player)
	{
		code = ErrorCode::GUILD_STORAGE_PLAYER_NOT_ONLINE;
		return;
	}

	std::vector<Item*> detailItems;
	std::vector<Item*> deleteItems;
	std::vector<Item*> splitItems;

	// 检查道具有效性
	for (size_t i = 0; i < req.items.size(); ++i)
	{
		Item* item = player->FindItem(req.items[i].uid);
		if (!item)
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item is null! uid:" << req.items[i].uid << LogStream::eos;
			code = ErrorCode::GUILD_STORAGE_DATA_INVALID;
			break;
		}

		if (item->IsItemLock(ILT_LEASE_LOCK))
		{
			code = ErrorCode::ITEM_IN_LEASING;
			break;
		}

		if (item->IsMountedPrecBead())
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item mounted prec bead error! dataid:" << req.items[i].dataId << LogStream::eos;
			code = ErrorCode::ITEM_PREABEAD_PUSHFAIL_MOUNTEDBEAD;
			break;
		}

		if (item->IsInAuctionCoolTimeStamp())
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item is in auction cool time error! dataid:" << req.items[i].dataId << LogStream::eos;
			code = ErrorCode::ITEM_IN_AUCTIONCOOL_OPFAIL;
			break;
		}

		if (item->GetDataID() != req.items[i].dataId)
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item dataid is error! dataid:" << req.items[i].dataId << LogStream::eos;
			code = ErrorCode::GUILD_STORAGE_DATA_INVALID;
			break;
		}

		if (item->GetNum() < req.items[i].num)
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item num is error! num:" << req.items[i].num << LogStream::eos;
			code = ErrorCode::GUILD_STORAGE_DATA_INVALID;
			break;
		}

		if (!ItemDataEntryMgr::Instance()->CheckCanAddGuildStorage(item->GetDataEntry(), item->GetSealState() == 0 ? false : true))
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item color is error! color:" << item->GetQuality() << LogStream::eos;
			code = ErrorCode::GUILD_STORAGE_CANNOT_ADD;
			break;
		}

		if (item->GetEquipType() == EQUIP_RED)
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item GetEquipType is error! GetEquipType:" << static_cast<UInt8>(item->GetEquipType()) << LogStream::eos;
			code = ErrorCode::GUILD_STORAGE_CANNOT_ADD;
			break;
		}

		if (item->GetStrengthen() > 10 && item->GetType() == ITEM_EQUIP)
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item itemId:" << item->GetDataID() << ", strengthen : " << item->GetStrengthen() << LogStream::eos;
			code = ErrorCode::GUILD_STORAGE_STRENGTHEN_LEV;
			break;
		}

		if (item->GetQuality() == ITEM_QUALITY_SILVERY)
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item itemId:" << item->GetDataID() << ", strengthen : " << item->GetStrengthen() << LogStream::eos;
			code = ErrorCode::GUILD_STORAGE_CANNOT_ADD;
			break;
		}

		if (item->GetStrengthen() > 0 && item->GetSubType() == ST_MAGICCARD)
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item is magic and strengthen is error! itemId:" << item->GetDataID()
				<< ", strengthen : " << item->GetStrengthen() << LogStream::eos;
			code = ErrorCode::GUILD_STORAGE_MAGIC_LEV;
			break;
		}

		if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME) && item->IsCanPushScheme() && player->GetEqSchemeMgr().IsEquipInScheme(item->GetID()))
		{
			ErrorLogStream << PLAYERINFO(player) << " guild storage item is in equip scheme! item id:" << item->GetID() << LogStream::eos;
			code = ErrorCode::ITEM_IILEG_OPER_IN_SCHEME;
			break;
		}

		// 拆分道具
		if (item->GetMaxNum() > 1)
		{
			Item* splitItem = Item::CreateItem(item->GetDataID());
			splitItem->SetID(CLRecordClient::Instance()->GenGuid());
			splitItem->SetNum(req.items[i].num);
			splitItem->SetStrengthen(item->GetStrengthen());
			splitItem->ChgEquipType(item->GetEquipType());
			detailItems.push_back(splitItem);
			splitItems.push_back(splitItem);
		}
		else
		{
			detailItems.push_back(item);
		}
		
		deleteItems.push_back(item);
	}

	Avalon::Packet* pPacket = Avalon::Packet::Create();
	pPacket->SetID(CLProtocol::SW_GUILD_ADDSTORAGEITEM_RES);
	Avalon::NetOutputStream stream(pPacket->GetBuffer(), pPacket->GetMaxSize());

	stream & code & player->GetID() & req.guildUid;

	if (code == ErrorCode::SUCCESS)
	{
		try
		{
			for (size_t i = 0; i < detailItems.size(); ++i)
			{
				stream & detailItems[i]->GetID() & detailItems[i]->GetDataID();
				detailItems[i]->Encode(stream, SST_WORLD, false);
			}
			stream & ObjID_t(0);
		}
		catch (Avalon::NetException& exception)
		{
			Avalon::Packet::Destroy(pPacket);
			ErrorLogStream << "add guild storage item encode error failed:" << exception.what() << "!" << LogStream::eos;

			return;
		}

		// 删除道具
		for (size_t i = 0; i < deleteItems.size(); ++i)
		{
			player->ReduceItemNum(GetReason(SOURCE_TYPE_ADD_GUILD_STORAGE).c_str(), deleteItems[i], req.items[i].num);

		}

		// 删除拆分出来的道具
		for (size_t i = 0; i < splitItems.size(); ++i)
		{
			Item::DestroyItem(splitItems[i]);
		}
	}
	
	pPacket->SetSize(stream.GetPos());
	Router::SendToWorld(pPacket);
	Avalon::Packet::Destroy(pPacket);
	
	
}

void GetGuildStorageItemFunction::operator()(Avalon::Packet* packet) const
{
	UInt64 playerUid = 0;
	UInt64 guildUid = 0;
	UInt32	itemid = 0;
	UInt64 oldUid = 0;

	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	stream & playerUid & guildUid & oldUid & itemid;

	Player* player = PlayerMgr::Instance()->FindNormalPlayer(playerUid);
	if (!player)
	{
		return;
	}

	Item* item = Item::CreateItem(itemid);
	if (item == NULL) return;

	item->SetID(CLRecordClient::Instance()->GenGuid());

	try
	{
		item->Decode(stream);
	}
	catch (const Avalon::NetException& e)
	{
		Item::DestroyItem(item);
		ErrorLogStream << "decode item data failed:" << e.what() << "!" << LogStream::eos;
		return;
	}

	item->SetAttribute();


	Avalon::Packet* outPacket = Avalon::Packet::Create();
	outPacket->SetID(CLProtocol::SW_GUILD_GETSTORAGEITEM_RES);
	Avalon::NetOutputStream outStream(outPacket->GetBuffer(), outPacket->GetMaxSize());

	try
	{
		outStream & player->GetID() & guildUid & item->GetID() & oldUid & item->GetDataID();
		item->Encode(outStream, SST_WORLD, false);
	}
	catch (const Avalon::NetException&)
	{
		ErrorLogStream << "encode item:" << item->GetID() << " failed!" << LogStream::eos;
		Avalon::Packet::Destroy(outPacket);
		Item::DestroyItem(item);
		return;
	}
	outPacket->SetSize(outStream.GetPos());

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_GUILD_LOTTRY);
	if (!player->GetItemMgr().CheckAddItem(item->GetDataID(), item->GetNum(), (ItemType)item->GetDataEntry()->type, 
		item->GetAuctionCoolTimeStamp(), item->GetStrengthen(), item->GetAuctionTransNum()))
	{
		std::vector<Item*> mailGroup;
		mailGroup.push_back(item);

		std::string strSenderName;
		std::string	strTitle;
		std::string	strContent;
		player->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);

		Sys::SendSysMail(player->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), MAIL_VALID_DAY, NULL, mailGroup);
	}
	else
	{
		UInt64 itemId = item->GetID();
		UInt32 itemDataId = item->GetDataID();
		ItemType itemType = (ItemType)item->GetType();
		ItemSubType itemSubType = (ItemSubType)item->GetSubType();
		std::string itemName = item->GetName();
		UInt32 itemQuality = (UInt32)item->GetQuality();
		if (player->AddItemDirect(reason.c_str(), item, false) > 0)
		{
			player->SendItemUdpLog(reason.c_str(), itemId, itemDataId, itemType, itemSubType, itemName, itemQuality, 1, 0, item->GetStrengthen(),item->GetEquipType(),item->GetEnhanceType());
		}
	}

	Router::SendToWorld(outPacket);
	Avalon::Packet::Destroy(outPacket);
}

void HandleProtocol(CLProtocol::WSShopBuyAddGuildStorageRes& res)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(res.playerUid);
	// 玩家离线了
	if (!player)
	{
		return;
	}

	player->GetShopMgr().BuyGuildStorageCb(res.code, res.shopId, res.shopItemId, res.num, res.costItemId, res.costNum);
}

void HandleProtocol(CLProtocol::WorldGuildEmblemSync& sync)
{
	auto player = PlayerMgr::Instance()->FindPlayer(sync.roleId);
	if (!player)
	{
		ErrorLogStream << "[Guild] emblem player(" << sync.roleId << ") offline." << LogStream::eos;
		return;
	}

	player->SetGuildEmblem(sync.emblemLevel);
}