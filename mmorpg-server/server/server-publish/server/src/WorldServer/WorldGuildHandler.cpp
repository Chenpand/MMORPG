#include "WorldGuildHandler.h"

#include "CLGuildHonorDataEntry.h"

#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "GuildMgr.h"
#include "GuildBattleMgr.h"
#include "ChatDataMgr.h"
#include "WSServiceSwitchMgr.h"
#include "GuildDungeonMgr.h"
#include "GuildAuctionMgr.h"

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildCreateReq& req)
{
	if (!player)
	{
		return;
	}

	GuildMgr::Instance()->OnPlayerRequestCreateGuild(player, req.name, req.declaration);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildLeaveReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestLeave(player);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildJoinReq& req)
{
	if (!player)
	{
		return;
	}

	GuildMgr::Instance()->OnPlayerRequestJoinGuild(player, req.id);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildListReq& req)
{
	if (!player)
	{
		return;
	}

	GuildMgr::Instance()->OnPlayerRequestGuildList(player, req.start, req.num);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildRequesterReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		CLProtocol::WorldGuildRequesterRes res;
		res.requesters = guild->GetRequesterList();
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildProcessRequester& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		if (req.id == 0)
		{
			guild->RequestClearRequestList(player);
		}
		else
		{
			guild->ReplyJoinRequest(player, req.id, req.agree != 0);
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildChangePostReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestChangePost(player, req.id, req.replacerId, (GuildPost)req.post);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildKickReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestKickPlayer(player, req.id);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildMemberListReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	Guild *guild = nullptr;
	if (req.guildID == 0)
	{
		guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	}
	else
	{
		guild = GuildMgr::Instance()->FindGuildByID(req.guildID);
		if (guild)
		{
			if (guild->GetProsperity() != GUILD_PROSPERITY_LOW && guild->GetProsperity() != GUILD_PROSPERITY_DISMISS)
			{
				return;
			}
		}
		else
		{
			return;
		}

		auto selfGuild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (selfGuild)
		{
			auto member = selfGuild->FindMember(player->GetID());
			if (!member)
			{
				return;
			}

			if (member->GetPost() != GUILD_POST_LEADER)
			{
				return;
			}

			if (selfGuild->GetProsperity() != GUILD_PROSPERITY_MID)
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
	
	if (guild)
	{
		CLProtocol::WorldGuildMemberListRes res;
		guild->GetGuildMemberList(res.members);
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildModifyDeclaration& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestChangeDeclaration(player, req.declaration);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildModifyName& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestChangeGuildName(player, req.name, req.itemId, req.itemTypeId);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildModifyAnnouncement& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestChangeAnnouncement(player, req.content);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildSendMail& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->SendGuildMail(player, req.content);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildUpgradeBuilding& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestUpgradeBuilding(player, (GuildBuildingType)req.type);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDonateReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestDonate(player, (GuildDonateType)req.type, req.num);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDonateLogReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		CLProtocol::WorldGuildDonateLogRes res;
		res.logs = guild->GetDonateLogs();
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildUpgradeSkill& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestUpgradeSkill(player, req.skillId);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDismissReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestDismiss(player);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildCancelDismissReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestCancelDismiss(player);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildLeaderInfoReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestLeaderInfo(player);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildOrzReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestOrz(player, (GuildOrzType)req.type);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildTableJoinReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestJoinTable(player, req.seat, req.help);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildPayRedPacketReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->OnRequestPayRedPacket(player, req.packetId, req.name, req.num);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_CROSS_BATTLE) && GuildBattleMgr::Instance()->IsCrossBattle())
	{
		return;
	}

	CLProtocol::WorldGuildBattleRes res;
	res.result = GuildBattleMgr::Instance()->OnRequestEnroll(player, req.terrId);

	if (res.result != ErrorCode::GUILD_BATTLE_ENROLL_TRANSACTION && res.result != ErrorCode::SECURITY_LOCK_FORBID_OP)
	{
		ErrorLogStream << "player(" << player->GetID() << ") enroll guild battle territoryId(" << req.terrId << "), error(" << res.result << ")." << LogStream::eos;

		res.terrId = req.terrId;
		res.enrollSize = GuildBattleMgr::Instance()->GetEnrollSize(req.terrId);

		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleInspireReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_CROSS_BATTLE) && GuildBattleMgr::Instance()->IsCrossBattle())
	{
		return;
	}
	
	CLProtocol::WorldGuildBattleInspireRes res;
	res.result = GuildBattleMgr::Instance()->OnRequestInspire(player, res.inspire);
	if (res.result != ErrorCode::GUILD_BATTLE_INSPIRE_TRANSACTION && res.result != ErrorCode::SECURITY_LOCK_FORBID_OP)
	{
		ErrorLogStream << "player(" << player->GetID() << ") inspire guild battle, error(" << res.result << ")." << LogStream::eos;
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleReceiveReq& req)
{
	UInt32 nRes = GuildBattleMgr::Instance()->OnRequestReceive(player, req.boxId);
	if (nRes != ErrorCode::SUCCESS)
	{
		CLProtocol::WorldGuildBattleReceiveRes res;
		res.result = nRes;
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleRecordReq& req)
{
	CLProtocol::WorldGuildBattleRecordRes res;
	if (req.isSelf)
	{
		res.result = GuildBattleMgr::Instance()->OnRequestBattleSelfRecord(player, res.records);
	}
	else
	{
		res.result = GuildBattleMgr::Instance()->OnRequestBattleRecord(player, res.records, req.startIndex, req.count);
	}

	//这里消息被转发到Cross
	if (res.result != ErrorCode::CROSS_GUILD_BATTLE_NOT_RESULT)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleTerritoryReq& req)
{
	CLProtocol::WorldGuildBattleTerritoryRes res;
	res.result = GuildBattleMgr::Instance()->GetTerritroyBaseInfo(req.terrId, res.info);
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleSelfSortListReq& req)
{
	if (NULL == player) return;

	UInt32 terrType = GuildBattleMgr::Instance()->GetGuildTerritoryType(player->GetGuildID());
	if (GuildTerritoryType::GTT_CROSS == terrType)
	{
		CLProtocol::CrossGuildBattleSelfSortListReq req;
		req.playerId = player->GetID();
		req.guildId = player->GetGuildID();
		req.terrId = GuildBattleMgr::Instance()->GetTerritoryId(player->GetGuildID());
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldGuildBattleSelfSortListRes res;
		res.result = ErrorCode::SUCCESS;
		res.memberRanking = GuildBattleMgr::Instance()->GetRanking(SortListType::SORTLIST_GUILD_BATTLE_MEMBER, player);
		res.guildRanking = GuildBattleMgr::Instance()->GetRanking(SortListType::SORTLIST_GUILD_BATTLE_SCORE, player);
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildChallengeReq req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	CLProtocol::WorldGuildChallengeRes res;
	res.result = GuildBattleMgr::Instance()->OnRequestEnroll(player, req.terrId, req.itemId, req.itemNum);

	if (res.result != ErrorCode::GUILD_BATTLE_ENROLL_TRANSACTION && res.result != ErrorCode::SECURITY_LOCK_FORBID_OP)
	{
		ErrorLogStream << "player(" << player->GetID() << ") enroll guild challenge territoryId(" << req.terrId << "), error(" << res.result << ")." << LogStream::eos;
		player->SendProtocol(res);
	}
}


void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildChallengeInfoReq req)
{
	GuildBattleMgr::Instance()->SelectGuildBattleChallengeInfo(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleInspireInfoReq req)
{
	CLProtocol::WorldGuildBattleInspireInfoRes res;

	res.result = GuildMgr::Instance()->GetGuildBattleInspireInfos(player->GetGuildID(), res.inspireInfos);

	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildStorageSettingReq req)
{
	InfoLogStream << "player(" << player->GetID() << ") HandleProtocol WorldGuildStorageSettingReq guild=" 
		<< player->GetGuildID() << " setting(" << req.type << "|" << req.value << ")" << LogStream::eos;
	CLProtocol::WorldGuildStorageSettingRes res;
	res.result =  GuildMgr::Instance()->SetGuildStorageSetting(player, (GuildStorageSetting)req.type, req.value);
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildBattleLotteryReq req)
{
	InfoLogStream << "player(" << player->GetID() << ") HandleProtocol WorldGuildBattleLotteryReq." << LogStream::eos;

	UInt32 result = GuildBattleMgr::Instance()->Lottery(player);
	if (result != ErrorCode::SUCCESS)
	{
		UInt32 contribution = 0;
		Avalon::Packet *packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::WORLD_GUILD_BATTLE_LOTTERY_RES);
		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		UInt64 uId = 0;
		try
		{
			stream & result & contribution & uId;
		}
		catch (const Avalon::NetException&)
		{
			Avalon::Packet::Destroy(packet);
			return;
		}
		packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
		player->SendPacket(packet);
		Avalon::Packet::Destroy(packet);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildStorageListReq req)
{
	CLProtocol::WorldGuildStorageListRes res;
	res.result = GuildMgr::Instance()->GetStorageList(player, res.maxSize, res.items, res.itemRecords);
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildAddStorageReq req)
{
	InfoLogStream << "player(" << player->GetID() << ") HandleProtocol WorldGuildAddStorageReq." << LogStream::eos;
	CLProtocol::WorldGuildAddStorageRes res;

	res.result = GuildMgr::Instance()->AddStorageItem(player, req.items);

	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDelStorageReq req)
{
	CLProtocol::WorldGuildDelStorageRes res;
	res.result = GuildMgr::Instance()->DelStorageItem(player, req.items);
	player->SendProtocol(res);
}


void HandleProtocol(CLProtocol::WorldGuildSyncSkill& sync)
{
	auto player = WSPlayerMgr::Instance()->FindPlayer(sync.id);
	if (!player)
	{
		return;
	}

	for (auto& skill : sync.skills)
	{
		player->AddGuildSkill(skill.id, skill.level);
	}
}

void HandleProtocol(CLProtocol::WorldGuildSyncContribution& sync)
{
	// 这里只处理获得贡献
	if (sync.isAdd == 0)
	{
		return;
	}

	GuildMgr::Instance()->OnScenePlayerAddContri(sync.id, sync.value);
}

void HandleProtocol(CLProtocol::WorldGuildGiveItemRes& protocol)
{
	CLProtocol::WorldGuildBattleReceiveRes res;
	res.result = ErrorCode::GUILD_BATTLE_GIVE_REWARD_NOT_MEMBER;

	GuildMember* member = GuildMgr::Instance()->FindGuildMember(protocol.playerId);
	if (member != NULL)
	{
		if (protocol.result == ErrorCode::SUCCESS)
		{
			member->SetRewardMask(protocol.boxId);
			Guild* guild = GuildMgr::Instance()->FindGuildByID(member->GetGuildID());
			if (guild != NULL)
			{
				guild->SyncMemberInfoToScene(member);
			}
		}
		res.result = protocol.result;
		res.boxId = protocol.boxId;
	}

	member->SendProtocol(res);
}

void WorldRecvAddGuildStorageItems::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	UInt32 code;
	UInt64 playerUid;
	UInt64 guildUid;
	std::vector<WSItem*>	items;

	stream & code & playerUid & guildUid;

	if (code == ErrorCode::SUCCESS)
	{
		try
		{
			ObjID_t itemId = 0;
			stream & itemId;
			while (itemId != 0)
			{
				UInt32 itemDataId = 0;
				stream & itemDataId;
				WSItem* item = new WSItem();
				if (item == NULL) return;

				item->SetID(itemId);
				item->m_DataId = itemDataId;
				item->m_pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemDataId);
				item->Decode(stream);
				items.push_back(item);

				stream & itemId;
			}
		}
		catch (Avalon::NetException& exception)
		{
			ErrorLogStream << "add guild storage item decode error failed:" << exception.what() << "!" << LogStream::eos;
			return;
		}
	}
	
	
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerUid);
	if (!player)
	{
		ErrorLogStream << "player offline uid:" << playerUid << LogStream::eos;
		return;
	}

	if (player->GetGuildID() != guildUid)
	{
		ErrorLogStream << "player not in guild:" << guildUid << LogStream::eos;
		return;
	}

	Guild* guild = GuildMgr::Instance()->FindGuildByID(guildUid);
	if (!guild)
	{
		ErrorLogStream << "add guild storage guild is null, guild uid:" << guildUid << LogStream::eos;
		return;
	}

	guild->AddStorageItemCb(playerUid, code, items);
}

void WorldRecvGetGuildStorageItem::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	//UInt32 code = ErrorCode::SUCCESS;
	UInt64 playerUid;
	UInt64 guildUid;
	UInt64 newUid;
	UInt64 oldUid;
	UInt32 itemDataId;

	WSItem* item = new WSItem();
	try
	{
		//stream & code & playerUid & guildUid & itemUid & itemDataId;
		stream & playerUid & guildUid & newUid & oldUid & itemDataId;

		item->SetID(newUid);
		item->m_DataId = itemDataId;
		item->Decode(stream);
	}
	catch (Avalon::NetException& exception)
	{
		ErrorLogStream << "get guild storage item decode error failed:" << exception.what() << "!" << LogStream::eos;
		return;
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerUid);
	if (!player)
	{
		ErrorLogStream << "player offline uid:" << playerUid << LogStream::eos;
		return;
	}

	if (player->GetGuildID() != guildUid)
	{
		ErrorLogStream << "player not in guild:" << guildUid << LogStream::eos;
		return;
	}

	Guild* guild = GuildMgr::Instance()->FindGuildByID(guildUid);
	if (!guild)
	{
		ErrorLogStream << "get guild storage guild is null, guild uid:" << guildUid << LogStream::eos;
		return;
	}

	guild->GetStorageItemCb(playerUid, item, oldUid);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldWatchGuildStorageItemReq& req)
{
	// 有缓存
	if (ChatDataMgr::Instance()->IsExistData(req.uid))
	{
		ChatDataMgr::Instance()->SendLinkData(player, 'I', req.uid);
		return;
	}

	// 没缓存查找
	Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (!guild)
	{
		return;
	}

	WSItem* item = guild->QueryStorageItem(req.uid);
	if (!item)
	{
		return;
	}

	Avalon::Packet* packet = Avalon::Packet::Create();
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & 'I';
		stream & item->GetID() & item->m_DataId;
		item->Encode(stream, SST_WATCH, false);
		stream & ObjID_t(0);
	}
	catch (Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(UInt16(stream.GetPos()));

	ChatDataMgr::Instance()->AddData(req.uid, (const char*)packet->GetBuffer() + sizeof(UInt8), packet->GetSize() - sizeof(UInt8));
	ChatDataMgr::Instance()->SendLinkData(player, 'I', req.uid);

	Avalon::Packet::Destroy(packet);
}

void WorldRecvShopBuyAddGuildStorage::operator()(Avalon::Packet* packet) const
{
	CLProtocol::WSShopBuyAddGuildStorageRes res;
	//UInt32 retCode = ErrorCode::GUILD_STORAGE_CANNOT_ADD;

	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	UInt64 playerUid = 0;
	
	UInt8 shopId = 0;
	UInt32 shopItemId = 0;
	UInt16 num = 0;
	UInt32 costItemId = 0;
	UInt32 costNum = 0;

	stream & playerUid;

	//玩家离线
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerUid);
	if (!player)
	{
		ErrorLogStream << " player is offline uid = " << playerUid << LogStream::eos;
		return;
	}

	//没有公会
	Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (!guild)
	{
		ErrorLogStream << PLAYERINFO(player) " no guild!" << LogStream::eos;
		res.code = ErrorCode::GUILD_STORAGE_CANNOT_ADD;
		player->SendProtocol(res);
		return;
	}
		
	WSItem* item = new WSItem();

	try
	{
		ObjID_t itemId = 0;
		stream & itemId;
		if (itemId != 0)
		{
			UInt32 itemDataId = 0;
			stream & itemDataId;
			if (item == NULL) return;

			item->SetID(itemId);
			item->m_DataId = itemDataId;
			item->m_pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemDataId);
			if (!item->m_pDataEntry)
			{
				ErrorLogStream << PLAYERINFO(player) << " shop buy itemdata is null! itemid = " << itemDataId << LogStream::eos;
				res.code = ErrorCode::GUILD_STORAGE_CANNOT_ADD;
				player->SendProtocol(res);

				CL_SAFE_DELETE(item);
				return;
			}
			item->Decode(stream);
		}
	}
	catch (Avalon::NetException& exception)
	{
		CL_SAFE_DELETE(item);
		ErrorLogStream << "shop buy add guild storage item decode error failed:" << exception.what() << "!" << LogStream::eos;
		return;
	}

	stream & shopId & shopItemId & num & costItemId & costNum;
	
	res.playerUid = playerUid;

	//仓库没容量
	if (!guild->CheckStorageCapacity(item))
	{
		CL_SAFE_DELETE(item);
		res.code = ErrorCode::GUILD_STORAGE_FULL;
		player->SendToScene(res);
		return;
	}
	
	if (!guild->ShopBuyAddStorage(player, item))
	{
		CL_SAFE_DELETE(item);
		res.code = ErrorCode::GUILD_STORAGE_CANNOT_ADD;
		player->SendToScene(res);
		return;
	}
		
	res.shopId = shopId;
	res.shopItemId = shopItemId;
	res.num = num;
	res.costItemId = costItemId;
	res.costNum = costNum;
	res.code = ErrorCode::SUCCESS;
	player->SendToScene(res);
}

void HandleProtocol(CLProtocol::WorldCrossGuildBattleEnrollInfoReq& req)
{
	if (!GuildBattleMgr::Instance()->IsGuildLoad()) return;
	InfoLogStream << "HandleProtocol WorldCrossGuildBattleEnrollInfoReq" << LogStream::eos;
	CLProtocol::WorldCrossGuildBattleEnrollInfoRes res;
	res.result = GuildBattleMgr::Instance()->GetCrossEnrollInfo(res.terrEnrollInfo, res.terrBaseInfo);
	GuildBattleMgr::Instance()->InitCrossSortListInfo();
	Router::SendToCross(res);
}

void HandleProtocol(CLProtocol::CrossGuildBattleEnrollRes& req)
{
	if (req.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "Handle Protocol WorldCrossGuildBattleEnrollRes result = " << req.result << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::CrossGuildBattleRecordRes& res)
{
	InfoLogStream << "HandleProtocol CrossGuildBattleRecordRes(playerId=" << res.playerId << ", result=" << res.result << ")" << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(res.playerId);
	if (player == NULL) return;
	CLProtocol::WorldGuildBattleRecordRes playerResult;
	playerResult.records = res.records;
	playerResult.result = res.result;
	player->SendProtocol(playerResult);
}

void HandleProtocol(CLProtocol::WorldCrossGuildBattleTerritoryListInfoSync& sync)
{
	InfoLogStream << "HandleProtocol WorldCrossGuildBattleTerritoryListInfoSync" << LogStream::eos;
	GuildBattleMgr::Instance()->SetCrossTerritoryInfos(sync.infos);
}

void HandleProtocol(CLProtocol::WorldCrossGuildBattleTerritoryInfoSync& sync)
{
	InfoLogStream << "HandleProtocol WorldCrossGuildBattleTerritoryInfoSync" << LogStream::eos;
	GuildBattleMgr::Instance()->SetCrossTerritoryInfo(sync.info);
}

void HandleProtocol(CLProtocol::WorldCrossGuildBattleRecordInfoSync& sync)
{
	InfoLogStream << "HandleProtocol WorldCrossGuildBattleRecordInfoSync(terrId = " << sync.terrId << ")" << LogStream::eos;
	CLProtocol::WorldGuildBattleRecordSync worldSync;
	worldSync.record = sync.info;
	GuildBattleMgr::Instance()->SyncBattleRecord(sync.terrId, worldSync);
}

void HandleProtocol(CLProtocol::WorldCrossGuildBattleEndInfoSync& sync)
{
	InfoLogStream << "HandleProtocol WorldCrossGuildBattleEndInfoSync" << LogStream::eos;
	GuildBattleMgr::Instance()->SyncCrossGuildBattleEndInfo(sync.info);
}

void HandleProtocol(CLProtocol::CrossGuildBattleSortListRes& res)
{
	InfoLogStream << "HandleProtocol CrossGuildBattleSortListRes(playerId=" << res.playerId << ")" << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(res.playerId);
	if (player == NULL) return;
	player->SendPacket(res.packet);
}

void HandleProtocol(CLProtocol::CrossGuildBattleSelfSortListRes& res)
{
	InfoLogStream << "HandleProtocol CrossGuildBattleSelfSortListRes(playerId=" << res.playerId << ")" << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(res.playerId);
	if (player == NULL) return;
	player->SendPacket(res.packet);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDungeonInfoReq req)
{
	GuildDungeonMgr::Instance()->HandleGuildDungeonInfoReq(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDungeonDamageRankReq req)
{
	GuildDungeonMgr::Instance()->HandleGuildDungeonDamageRankReq(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDungeonCopyReq req)
{
	GuildDungeonMgr::Instance()->HandleGuildDungeonCopyReq(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDungeonLotteryReq& req)
{
	GuildDungeonMgr::Instance()->HandleGuildDungeonLotteryReq(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildDungeonStatueReq& req)
{
	GuildDungeonMgr::Instance()->HandleGuildDungeonStatueReq(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildSetJoinLevelReq& req)
{
	if (NULL == player || player->GetGuildID() == 0)
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		guild->RequestSetJoinLevel(player, req.joinLevel);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildEmblemUpReq& req)
{
	if (NULL == player || player->GetGuildID() == 0)
	{
		return;
	}

	CLProtocol::WorldGuildEmblemUpRes res;
	auto* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (NULL == guild)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return;
	}

	guild->HandleEmblemUpgrade(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildSetDungeonTypeReq &req)
{
	if (NULL == player || player->GetGuildID() == 0)
	{
		return;
	}

	CLProtocol::WorldGuildSetDungeonTypeRes res;
	auto* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (NULL == guild)
	{
		res.result = ErrorCode::GUILD_NOT_IN_GUILD;
		player->SendProtocol(res);
		return;
	}

	guild->HandleGuildDungeonType(player, req.dungeonType);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildGetTerrDayRewardReq &req)
{
	if (NULL == player || player->GetGuildID() == 0)
	{
		return;
	}

	CLProtocol::WorldGuildGetTerrDayRewardRes res;

	do 
	{
		auto* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (NULL == guild)
		{
			res.result = ErrorCode::GUILD_NOT_IN_GUILD;
			break;
		}

		res.result = guild->HandleGuildTerrDayReward(player);

	} while (0);

	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildAuctionItemReq& req)
{
	if (NULL == player) return;
	GuildAuctionMgr::Instance()->HandleAuctionItemReq(player, req.type);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildAuctionBidReq& req)
{
	if (NULL == player) return;
	GuildAuctionMgr::Instance()->HandleAuctionBidReq(player, req.guid, req.bidPrice);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildAuctionFixReq& req)
{
	if (NULL == player) return;
	GuildAuctionMgr::Instance()->HandleAuctionFixReq(player, req.guid);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildWatchCanMergerReq& req)
{
	if (player == nullptr)
	{
		return;
	}
	GuildMgr::Instance()->OnPlayerRequestCanMergerGuildList(player, req.start, req.num);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildMergerRequestOperatorReq& req)
{
	if (player == nullptr)
	{
		return;
	}
	GuildMgr::Instance()->OnPlayerMergerRequestOperator(player, req.guildId, req.opType);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildReceiveMergerRequestReq& req)
{
	if (player == nullptr)
	{
		return;
	}
	GuildMgr::Instance()->OnPlayerQueryIsHaveMergerRequest(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildWatchHavedMergerRequestReq& req)
{
	if (player == nullptr)
	{
		return;
	}
	GuildMgr::Instance()->OnPlayerQueryReceiveMergerRequestList(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildAcceptOrRefuseOrCancleMergerRequestReq& req)
{
	if (player == nullptr)
	{
		return;
	}
	GuildMgr::Instance()->OnPlayerOperaMergerRequest(player, req.guildId, req.opType);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGuildEventListReq& req)
{
	if (!player || !player->GetGuildID())
	{
		return;
	}

	auto guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
	if (guild)
	{
		DevLogStream << "player(" << player->GetID() << ") get guild(" << player->GetGuildID() << ") event uptime=" << req.uptime << LogStream::eos;
		CLProtocol::WorldGuildEventListRes res;
		guild->GetGuildEventPage(res.guildEvents, req.uptime);
		res.uptime = (UInt32)CURRENT_TIME.Sec();
		player->SendProtocol(res);
	}
	else
	{
		ErrorLogStream << "player(" << player->GetID() << ") request GetGuildEventPage not find guild(" << player->GetGuildID() << ")" << LogStream::eos;
	}
}

void HandleProtocol(WSPlayer * player, CLProtocol::WorldGuildHonorInfoReq & req)
{
	CLProtocol::WorldGuildHonorInfoRes res;
	Guild* guild = GuildMgr::Instance()->FindGuildByID(req.guildId);

	res.guildId = req.guildId;

	if (guild != NULL)
	{
		res.guildName = guild->GetName();
		guild->GetGuildHonor(res,req.groupIds);
		player->SendProtocol(res);
	}
	
}

void HandleProtocol(CLProtocol::SyncGuildEventToWorld& sync)
{
	if (sync.content.empty())
	{
		return;
	}

	Guild* guild = GuildMgr::Instance()->FindGuildByID(sync.id);
	if (guild != NULL)
	{
		guild->AddGuildEvent(sync.content);
	}
}


void HandleProtocol(CLProtocol::TeamCopyTeamPlayerInfo & info)
{
	std::vector<UInt32> conds(GuildHonorConditionType::GUILD_COND_MAX,0);
	conds[GuildHonorConditionType::GUILD_COND_TYPE] = info.teamType;
	conds[GuildHonorConditionType::GUILD_COND_DIFF] = info.teamGrade;
	conds[GuildHonorConditionType::GUILD_COND_MEMBER] = 1;
	//conds[GuildHonorConditionType::GUILD_COND_TIME] = info.costTime;
	UInt64 guildId = 0;
	bool flag = false;
	for (auto playerId : info.playerIdVec)
	{
		GuildMember* member = GuildMgr::Instance()->FindGuildMember(playerId);
		if (!member)
		{
			flag = true;
			guildId = 0;
			continue;
		}
		
		if (!flag)
		{
			flag = true;
			guildId = member->GetGuildID();
		}
		else if (guildId != member->GetGuildID())
		{
			guildId = 0;
		}

		Guild* guild= GuildMgr::Instance()->FindGuildByID(member->GetGuildID());
		if (!guild)
		{
			ErrorLogStream << "guildId (" << member->GetGuildID() << ") ,not find guild" << LogStream::eos;
			continue;
		}
		guild->AddGuildHonorCnt(GuildHonorChallenge::GUILD_CHALLENGE_TEMACOPY, conds);
	}
	//团本全是公会成员,团本通关时间荣誉
	if (guildId != 0)
	{
		conds[GuildHonorConditionType::GUILD_COND_MEMBER] = info.playerIdVec.size();
		conds[GuildHonorConditionType::GUILD_COND_TIME] = info.costTime;
		conds[GuildHonorConditionType::GUILD_COND_TEAM] = 1;
		Guild* guild = GuildMgr::Instance()->FindGuildByID(guildId);
		if (!guild)
		{
			ErrorLogStream << "guildId (" << guildId << ") ,not find guild" << LogStream::eos;
			return;
		}
		guild->AddGuildHonorCnt(GuildHonorChallenge::GUILD_CHALLENGE_TEMACOPY, conds);
	}

}
