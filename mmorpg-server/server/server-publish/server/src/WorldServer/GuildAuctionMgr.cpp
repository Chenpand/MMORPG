#include <CLGuildProtocol.h>
#include <CLSystemValueDataEntry.h>

#include "GuildAuctionMgr.h"
#include "WSSelectInitDataCallback.h"
#include "CLRecordClient.h"
#include "WSPlayerMgr.h"
#include "WSItemTransaction.h"
#include "GuildMgr.h"
#include "GuildDungeonMgr.h"
#include "WSServiceSwitchMgr.h"


class SelectGuildAuctionItemCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return GuildAuctionMgr::Instance()->LoadAuctionItem(this);
	}
};

/**
*@brief 公会拍卖扣款
*/
class GuildAuctionTransaction : public ItemTransaction
{
public:
	GuildAuctionTransaction(WSPlayer *player, UInt64 guid, UInt32 source, UInt32 num)
		: ItemTransaction(player), m_AuctionItemGuid(guid), m_SourceType(source), m_RemoveNum(num) {}
	~GuildAuctionTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		GuildAuctionMgr::Instance()->AuctionRemoveItem(player, this);
		UnLock();
	}

	void OnCheckFailed(WSPlayer *player)
	{
		if (NULL == player) return;
		
		ErrorLogStream << "GuildAuctionTransaction Failed! PlayerId:"<<player->GetID()<< " Name:"<<player->GetName()<< LogStream::eos;

		if (SOURCE_TYPE_GUILD_AUCTION_BID== m_SourceType)
		{
			CLProtocol::WorldGuildAuctionBidRes res;
			res.retCode = ErrorCode::GUILD_NOT_ENOUGH_MONEY;
			player->SendProtocol(res);
		}
		else if (SOURCE_TYPE_GUILD_AUCTION_FIX == m_SourceType)
		{
			CLProtocol::WorldGuildAuctionFixRes res;
			res.retCode = ErrorCode::GUILD_NOT_ENOUGH_MONEY;
			player->SendProtocol(res);
		}

		UnLock();
	}

	UInt64 GetAuctionItemGuid() { return m_AuctionItemGuid; }
	UInt32 GetSourceType() { return m_SourceType; }
	UInt32 GetRemoveNum() {	return m_RemoveNum; }

private:

	UInt64 m_AuctionItemGuid;
	UInt32 m_SourceType;
	UInt32 m_RemoveNum;
};

GuildAuctionMgr::GuildAuctionMgr()
{
	m_IsDayChange = false;
}

GuildAuctionMgr::~GuildAuctionMgr()
{

}

void GuildAuctionMgr::Init()
{
	CLSelectCommand* cmd_1 = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_auction_item");
	CLRecordClient::Instance()->SendCommand(cmd_1, new SelectGuildAuctionItemCallback());
}

void GuildAuctionMgr::OnTick(const Avalon::Time &now)
{
	// 公会拍卖
	for (auto& iter : m_AuctionGuildMap)
	{
		GuildAuctionItem& auctionItem = iter.second;
		if (auctionItem.endTime > now.Sec() || !_IsCanAuction(auctionItem.state))
			continue;

		// 准备的物品现在开始卖
		if (auctionItem.state == GAI_STATE_PREPARE)
		{
			InfoLogStream << "Guild Auction Normal Info:" << auctionItem.SerializeToString() << LogStream::eos;

			auctionItem.state = GAI_STATE_NORMAL;
			auctionItem.endTime = UInt32(CURRENT_TIME.Sec()) + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_AUCTION_TIME);
			_UpdateAuctionItem(auctionItem);
			continue;
		}

		// 成交
		if (auctionItem.bidRoleId != 0)
		{
			InfoLogStream << "Guild Auction Deal Info:" << auctionItem.SerializeToString() << LogStream::eos;
			// 处理成交
			_AuctionDeal(auctionItem);

			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(auctionItem.bidRoleId);
			if (player != NULL)
			{
				player->SendNotify(10032);
			}

			// 修改状态,成交
			auctionItem.state = GAI_STATE_DEAL;
			_UpdateAuctionItem(auctionItem);

			// udp日志成交
			_UdpAuctionDeal(auctionItem);

			// 检查是否可以分成
			_CheckAuctionBonus(auctionItem.guildId);

			continue;
		}

		InfoLogStream << "Guild Auction Aboration Info:" << auctionItem.SerializeToString() << LogStream::eos;

		// 流拍到世界
		auctionItem.state = GAI_STATE_ABORATION;
		_UpdateAuctionItem(auctionItem);

		// udp日志公会流拍
		_UdpGuildAuction(UDP_GUILD_AUCTION_ABORATION, auctionItem);

		/******************流拍到世界******************/

		// 世界新创建一份拍卖品
		GuildAuctionItem newAuction = auctionItem;
		newAuction.guid = CLRecordClient::Instance()->GenGuid();
		newAuction.state = GAI_STATE_PREPARE;
		newAuction.auctionType = G_AUCTION_WORLD;
		newAuction.endTime = UInt32(CURRENT_TIME.Sec()) + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_AUCTION_PREPARE_TIME);
		m_AuctionWorldMap[newAuction.guid] = newAuction;
		_InsertAuctionItem(newAuction);

		InfoLogStream << "World Auction Obtain Info:" << newAuction.SerializeToString() << LogStream::eos;

		// 世界拍，有拍品，通知全服
		_NotifyAuctionEntrance(G_AUCTION_WORLD, true);

		// udp日志创建世界拍卖
		_UdpGuildAuction(UDP_GUILD_AUCTION_CREATE, newAuction);
	}

	// 世界拍卖
	for (auto& iter : m_AuctionWorldMap)
	{
		GuildAuctionItem& auctionItem = iter.second;
		if (auctionItem.endTime > now.Sec() || !_IsCanAuction(auctionItem.state))
			continue;

		// 准备的物品现在开始卖
		if (auctionItem.state == GAI_STATE_PREPARE)
		{
			InfoLogStream << "World Auction Normal Info:" << auctionItem.SerializeToString() << LogStream::eos;

			auctionItem.state = GAI_STATE_NORMAL;
			auctionItem.endTime = UInt32(CURRENT_TIME.Sec()) + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_WORLD_AUCTION_TIME);
			_UpdateAuctionItem(auctionItem);
			continue;
		}

		// 成交
		if (auctionItem.bidRoleId != 0)
		{
			InfoLogStream << "World Auction Deal Info:"<< auctionItem.SerializeToString() << LogStream::eos;

			// 处理成交
			_AuctionDeal(auctionItem);

			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(auctionItem.bidRoleId);
			if (player != NULL)
			{
				player->SendNotify(10032);
			}

			// 修改状态,成交
			auctionItem.state = GAI_STATE_DEAL;
			_UpdateAuctionItem(auctionItem);

			// udp日志成交
			_UdpAuctionDeal(auctionItem);

			// 检查是否可以分成
			_CheckAuctionBonus(auctionItem.guildId);

			continue;
		}

		InfoLogStream << "World Auction Aboration Info:" << auctionItem.SerializeToString() << LogStream::eos;

		// 流拍
		auctionItem.state = GAI_STATE_ABORATION;
		_UpdateAuctionItem(auctionItem);

		// udp日志世界流拍
		_UdpGuildAuction(UDP_GUILD_AUCTION_ABORATION, auctionItem);

		// 检查是否可以分成
		_CheckAuctionBonus(auctionItem.guildId);
	}

	if (!m_IsDayChange)
	{
		if (CURRENT_DATE.Hour() == 0 && CURRENT_DATE.Min() == 0 && CURRENT_DATE.Sec() < 3)
		{
			m_IsDayChange = true;
			// 关掉所有拍卖入口，删除所有拍卖数据
			for (auto& iter : m_GuildAuctionIndex)
			{
				_NotifyAuctionEntrance(G_AUCTION_GUILD, false, iter.first);
			}

			_NotifyAuctionEntrance(G_AUCTION_WORLD, false);
			_ClearAuctionData();

			InfoLogStream << "Clear All Auction Data!" << LogStream::eos;
		}
	}
	else
	{
		if (CURRENT_DATE.Hour() == 0 && CURRENT_DATE.Min() > 0)
			m_IsDayChange = false;
	}
}

void GuildAuctionMgr::HandleAuctionItemReq(WSPlayer* player, UInt32 type)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_AUCTION))
		return;

	if (NULL == player) return;
	if (G_AUCTION_GUILD != type && G_AUCTION_WORLD != type)
		return;

	CLProtocol::WorldGuildAuctionItemRes res;
	res.type = type;

	GuildAuctionItemMap* auctionMap = _GetGuildAuctionItemMap(type);
	if (NULL == auctionMap)
	{
		player->SendProtocol(res);
		return;
	}

	if (G_AUCTION_GUILD == type)
	{
		if (player->GetGuildID() == 0)
		{
			player->SendProtocol(res);
			return;
		}

		const std::set<UInt64>* guidSet = _GetGuildAuctionIndex(player->GetGuildID());
		if (guidSet != NULL)
		{
			for (auto& guid : *guidSet)
			{
				GuildAuctionItem* auctionItem = _GetGuildAuctionItem(guid);
				if (NULL == auctionItem) 
					continue;

				res.auctionItemVec.push_back(*auctionItem);
			}
		}
	}
	else if (G_AUCTION_WORLD == type)
	{
		for (auto& iter : *auctionMap)
			res.auctionItemVec.push_back(iter.second);
	}

	player->SendProtocol(res); 
}

void GuildAuctionMgr::HandleAuctionBidReq(WSPlayer* player, UInt64 guid, UInt32 bidPrice)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_AUCTION))
		return;

	if (NULL == player) 
		return;

	CLProtocol::WorldGuildAuctionBidRes res;
	do
	{
		GuildAuctionItem* auctionItem = _GetGuildAuctionItem(guid);
		if (NULL == auctionItem)
		{
			res.retCode = ErrorCode::GUILD_AUCTION_ITEM_NOT_EXIST;
			break;
		}

		if (auctionItem->state != GAI_STATE_NORMAL)
		{
			res.retCode = _GetErrorCodeByAucState(auctionItem->state);
			break;
		}

		// 是否超过一口价
		if (bidPrice >= auctionItem->fixPrice)
		{
			res.retCode = ErrorCode::GUILD_AUCTION_OVER_FIX;
			break;
		}

		// 价格有变动，需要重新出价
		if (bidPrice <= auctionItem->curPrice)
		{
			res.retCode = ErrorCode::GUILD_AUCTION_BID_BE_OVER;
			break;
		}

		// 出价不正确
		if (bidPrice != auctionItem->bidPrice)
		{
			res.retCode = ErrorCode::GUILD_AUCTION_BID_PRICE_ERROR;
			break;
		}

		// 开始事务检查货币够不够
		auto trans = new GuildAuctionTransaction(player, guid, SOURCE_TYPE_GUILD_AUCTION_BID, bidPrice);
		trans->SetItem(GUILD_AUCTION_COIN, bidPrice);
		if (!player->BeginItemTransaction(trans))
		{
			ErrorLogStream << PLAYERINFO(player) << "Auction Bid Error Guid:" << guid <<" Price:"<< bidPrice << LogStream::eos;
			res.retCode = ErrorCode::GUILD_AUCTION_BEING_BID;
			break;
		}

		// 此处不给客户端回消息，等事务回来后再做处理
		return;

	} while (0);

	player->SendProtocol(res);
}

void GuildAuctionMgr::HandleAuctionFixReq(WSPlayer* player, UInt64 guid)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_AUCTION))
		return;

	if (NULL == player) return;

	CLProtocol::WorldGuildAuctionFixRes res;
	do 
	{
		GuildAuctionItem* auctionItem = _GetGuildAuctionItem(guid);
		if (NULL == auctionItem)
		{
			res.retCode = ErrorCode::GUILD_AUCTION_ITEM_NOT_EXIST;
			break;
		}

		if (auctionItem->state != GAI_STATE_NORMAL)
		{
			res.retCode = _GetErrorCodeByAucState(auctionItem->state);
			break;
		}

		// 开始事务检查货币够不够
		auto trans = new GuildAuctionTransaction(player, guid, SOURCE_TYPE_GUILD_AUCTION_FIX, auctionItem->fixPrice);
		trans->SetItem(GUILD_AUCTION_COIN, auctionItem->fixPrice);
		if (!player->BeginItemTransaction(trans))
		{
			ErrorLogStream << PLAYERINFO(player) << "Auction Fix Error Guid:" << guid << " Price:" << auctionItem->fixPrice << LogStream::eos;
			res.retCode = ErrorCode::GUILD_AUCTION_BEING_BID;
			break;
		}

		// 此处不给客户端回消息，等事务回来后再做处理
		return;

	} while (0);

	player->SendProtocol(res);
}

bool GuildAuctionMgr::LoadAuctionItem(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		UInt64 guid = callback->GetKey();

		GuildAuctionItem auctionItem;
		auctionItem.guid = guid;
		auctionItem.rewardGroup = callback->GetData("reward_group");
		auctionItem.bidNum = callback->GetData("bid_num");
		auctionItem.auctionType = callback->GetData("auction_type");
		auctionItem.guildId = callback->GetData("guild_id");
		auctionItem.bidRoleId = callback->GetData("bid_role_id");
		auctionItem.vipLevel = callback->GetData("vip_level");
		auctionItem.level = callback->GetData("level");
		auctionItem.occu = callback->GetData("occu");
		auctionItem.bidTime = callback->GetData("bid_time");
		auctionItem.state = callback->GetData("state");
		auctionItem.curPrice = callback->GetData("cur_price");
		auctionItem.bidPrice = callback->GetData("bid_price");
		auctionItem.fixPrice = callback->GetData("fix_price");
		auctionItem.addPrice = callback->GetData("add_price");
		auctionItem.endTime = callback->GetData("end_time");
		auctionItem.isBonus = callback->GetData("is_bonus");

		std::string rewardItem = callback->GetData("auction_item").GetString();
		std::vector<std::string> param_1;
		Avalon::StringUtil::Split(rewardItem, param_1, "|");
		for (auto& iter : param_1)
		{
			if (iter.empty())
				continue;

			std::vector<std::string> param_2;
			Avalon::StringUtil::Split(iter, param_2, ",");
			if (param_2.size() != 2)
			{
				ErrorLogStream << "reward item format error:" << rewardItem << LogStream::eos;
				continue;
			}

			ItemReward reward;
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);
			auctionItem.auctionRewardVec.push_back(reward);
		}

		_AddAuctionItem(auctionItem);

		if (auctionItem.auctionType == G_AUCTION_GUILD)
		{
			_AddGuildAuctionIndex(auctionItem.guildId, guid);
		}
	}

	return true;
}

void GuildAuctionMgr::ObtainAuctionReward(UInt64 guildId, UInt32 rewardGroup, UInt32 initPrice, UInt32 fixPrice, UInt32 addPrice, const ItemRewardVec& reward)
{
	// 公会获得拍卖品，通知公会成员
	_NotifyAuctionEntrance(G_AUCTION_GUILD, true, guildId);

	GuildAuctionItem auctionItem;
	auctionItem.guildId = guildId;
	auctionItem.rewardGroup = rewardGroup;
	auctionItem.guid = CLRecordClient::Instance()->GenGuid();
	auctionItem.curPrice = 0;
	auctionItem.bidPrice = initPrice;
	auctionItem.fixPrice = fixPrice;
	auctionItem.addPrice = addPrice;
	auctionItem.state = GAI_STATE_PREPARE;
	auctionItem.endTime = UInt32(CURRENT_TIME.Sec()) + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_AUCTION_PREPARE_TIME);
	auctionItem.auctionType = G_AUCTION_GUILD;
	auctionItem.bidRoleId = 0;
	auctionItem.bidTime = 0;
	auctionItem.auctionRewardVec = reward;

	_AddAuctionItem(auctionItem);
	_AddGuildAuctionIndex(auctionItem.guildId, auctionItem.guid);

	_InsertAuctionItem(auctionItem);

	InfoLogStream << "Guild Auction Obtain Info:"<< auctionItem.SerializeToString() << LogStream::eos;

	// udp日志创建公会拍卖
	_UdpGuildAuction(UDP_GUILD_AUCTION_CREATE, auctionItem);
}

void GuildAuctionMgr::AuctionRemoveItem(WSPlayer* player, GuildAuctionTransaction* auctionTran)
{
	if (NULL == player || NULL == auctionTran)
	{
		ErrorLogStream << "AuctionRemoveItem Failed!!!" << LogStream::eos;
		return;
	}

	GuildAuctionItem* auctionItem = _GetGuildAuctionItem(auctionTran->GetAuctionItemGuid());
	if (NULL == auctionItem)
	{
		_NotifyClientResult(player, auctionTran->GetSourceType(), ErrorCode::GUILD_AUCTION_ITEM_NOT_EXIST);
		return;
	}

	if (auctionItem->state != GAI_STATE_NORMAL)
	{
		_NotifyClientResult(player, auctionTran->GetSourceType(), _GetErrorCodeByAucState(auctionItem->state));
		return;
	}

	// 价格有变动，需要重新出价
	if (auctionTran->GetRemoveNum() <= auctionItem->curPrice)
	{
		_NotifyClientResult(player, auctionTran->GetSourceType(), ErrorCode::GUILD_AUCTION_BID_BE_OVER);
		return;
	}

	// 扣款
	std::string reason = GetReason(LogSourceType(auctionTran->GetSourceType()), auctionItem->guildId, auctionItem->auctionType);
	auctionTran->RemoveItem(reason);

	// 上一个出价玩家
	UInt64 prePlayerId = auctionItem->bidRoleId;
	UInt32 prePrice = auctionItem->curPrice;
	
	// 给上一个出价玩家，邮件退款, 并发个通知
	if (prePlayerId != 0)
	{
		_PreBidPlayerRefund(prePlayerId, auctionItem->guildId, prePrice, auctionItem->auctionType, _GetRewardStr(auctionItem->auctionRewardVec));
		InfoLogStream << "Auction Pre Player Refund Id:"<< prePlayerId <<" Price:"<< prePrice << LogStream::eos;
	}

	auctionItem->bidRoleId = player->GetID();
	auctionItem->vipLevel = player->GetVipLvl();
	auctionItem->level = player->GetLevel();
	auctionItem->occu = player->GetOccu();
	auctionItem->bidTime = UInt32(CURRENT_TIME.Sec());
	auctionItem->curPrice = auctionTran->GetRemoveNum();
	auctionItem->bidPrice += auctionItem->addPrice;
	auctionItem->bidNum++;

	if (SOURCE_TYPE_GUILD_AUCTION_BID == auctionTran->GetSourceType())
	{	
		// 是否在竞价时间期内
		UInt32 bidTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_AUCTION_BID_TIME);
		if (auctionItem->endTime < UInt32(CURRENT_TIME.Sec()) + bidTime)
		{
			auctionItem->endTime = UInt32(CURRENT_TIME.Sec()) + bidTime;
		}

		_UpdateAuctionItem(*auctionItem);

		CLProtocol::WorldGuildAuctionBidRes res;
		res.retCode = ErrorCode::SUCCESS;
		player->SendProtocol(res);
		player->SendNotify(10030);

		InfoLogStream << "Auction Bid Price:"<< auctionItem->SerializeToString() << LogStream::eos;
	}
	else if (SOURCE_TYPE_GUILD_AUCTION_FIX == auctionTran->GetSourceType())
	{
		// 一口价，直接卖出
		auctionItem->state = GAI_STATE_DEAL;
		_UpdateAuctionItem(*auctionItem);

		// 成交
		_AuctionDeal(*auctionItem);

		// udp日志成交,一口价
		_UdpAuctionDeal(*auctionItem, 1);

		// 检查是否可以分成
		_CheckAuctionBonus(auctionItem->guildId);

		CLProtocol::WorldGuildAuctionFixRes res;
		res.retCode = ErrorCode::SUCCESS;
		player->SendProtocol(res);
		player->SendNotify(10031);

		InfoLogStream << "Auction Fix Price Deal:" << auctionItem->SerializeToString() << LogStream::eos;
	}

	// udp日志竞价
	_UdpAuctionBid(player, *auctionItem);
}

void GuildAuctionMgr::OnPlayerOnline(WSPlayer* player)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_AUCTION))
		return;

	CLProtocol::WorldGuildAuctionNotify notify;
	notify.isOpen = 1;

	// 是否有公会拍卖
	if (_CheckGuildAuctionIsHasItem(player->GetGuildID()))
	{
		notify.type = G_AUCTION_GUILD;
		player->SendProtocol(notify);
	}

	// 是否有世界拍卖
	UInt32 lvl = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_WORLD_AUCTION_LVL);
	if (player->GetLevel() >= lvl && !m_AuctionWorldMap.empty())
	{
		notify.type = G_AUCTION_WORLD;
		player->SendProtocol(notify);
	}
}

void GuildAuctionMgr::OnLevelUp(WSPlayer* player)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_AUCTION))
		return;

	UInt32 lvl = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_WORLD_AUCTION_LVL);
	if (player->GetLevel() < lvl)
		return;

	if (m_AuctionWorldMap.empty())
		return;

	CLProtocol::WorldGuildAuctionNotify notify;
	notify.isOpen = 1;
	notify.type = G_AUCTION_WORLD;
	player->SendProtocol(notify);
}

bool GuildAuctionMgr::CheckGuildIsAuction(UInt64 guildId)
{
	auto iter = m_GuildAuctionIndex.find(guildId);
	if (iter != m_GuildAuctionIndex.end())
	{
		for (auto& id : iter->second)
		{
			auto item = m_AuctionGuildMap.find(id);
			if (item == m_AuctionGuildMap.end())
				continue;

			if (_IsCanAuction(item->second.state))
				return true;
		}
	}

	for (auto& it : m_AuctionWorldMap)
	{
		if (it.second.guildId != guildId)
			continue;

		if (_IsCanAuction(it.second.state))
			return true;
	}

	return false;
}

void GuildAuctionMgr::_InsertAuctionItem(const GuildAuctionItem& auctionItem)
{
	std::string item;
	item.reserve(32);
	for (auto& iter : auctionItem.auctionRewardVec)
	{
		item.append(std::to_string(iter.id)).append(",").
			append(std::to_string(iter.num)).append("|");
	}

	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_auction_item", auctionItem.guid);
	cmd->PutData("guild_id", auctionItem.guildId);
	cmd->PutData("reward_group", auctionItem.rewardGroup);
	cmd->PutData("bid_num", auctionItem.bidNum);
	cmd->PutData("auction_type", auctionItem.auctionType);
	cmd->PutData("bid_role_id", auctionItem.bidRoleId);
	cmd->PutData("vip_level", auctionItem.vipLevel);
	cmd->PutData("level", auctionItem.level);
	cmd->PutData("occu", auctionItem.occu);
	cmd->PutData("bid_time", auctionItem.bidTime);
	cmd->PutData("state", auctionItem.state);
	cmd->PutData("cur_price", auctionItem.curPrice);
	cmd->PutData("bid_price", auctionItem.bidPrice);
	cmd->PutData("fix_price", auctionItem.fixPrice);
	cmd->PutData("add_price", auctionItem.addPrice);
	cmd->PutData("end_time", auctionItem.endTime);
	cmd->PutData("is_bonus", auctionItem.isBonus);
	cmd->PutData("auction_item", item);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GuildAuctionMgr::_UpdateAuctionItem(const GuildAuctionItem& auctionItem)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_auction_item", auctionItem.guid, true);
	cmd->PutData("bid_num", auctionItem.bidNum);
	cmd->PutData("auction_type", auctionItem.auctionType);
	cmd->PutData("bid_role_id", auctionItem.bidRoleId);
	cmd->PutData("vip_level", auctionItem.vipLevel);
	cmd->PutData("level", auctionItem.level);
	cmd->PutData("occu", auctionItem.occu);
	cmd->PutData("bid_time", auctionItem.bidTime);
	cmd->PutData("state", auctionItem.state);
	cmd->PutData("cur_price", auctionItem.curPrice);
	cmd->PutData("bid_price", auctionItem.bidPrice);
	cmd->PutData("end_time", auctionItem.endTime);
	cmd->PutData("is_bonus", auctionItem.isBonus);
	CLRecordClient::Instance()->SendCommand(cmd);
}

GuildAuctionItemMap* GuildAuctionMgr::_GetGuildAuctionItemMap(UInt32 type)
{
	if (G_AUCTION_GUILD == type)
	{
		return &m_AuctionGuildMap;
	}
	else if (G_AUCTION_WORLD == type)
	{
		return &m_AuctionWorldMap;
	}

	return NULL;
}

void GuildAuctionMgr::_AddAuctionItem(const GuildAuctionItem& auctionItem)
{
	GuildAuctionItemMap* auctionMap = _GetGuildAuctionItemMap(auctionItem.auctionType);
	if (NULL == auctionMap)
		return;

	(*auctionMap)[auctionItem.guid] = auctionItem;
}

GuildAuctionItem* GuildAuctionMgr::_GetGuildAuctionItem(UInt64 guid)
{
	auto iterGuild = m_AuctionGuildMap.find(guid);
	if (iterGuild != m_AuctionGuildMap.end())
	{
		return &(iterGuild->second);
	}

	auto iterWorld = m_AuctionWorldMap.find(guid);
	if (iterWorld != m_AuctionWorldMap.end())
	{
		return &(iterWorld->second);
	}

	return NULL;
}

void GuildAuctionMgr::_PreBidPlayerRefund(UInt64 playerId, UInt64 guildId, UInt32 num, UInt32 auctionType, const std::string& rewardName)
{
	// 先发个通知提醒，出价被超过
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
	if (player != NULL)
	{
		player->SendNotify(10017);
	}

	std::string sender = SysNotifyMgr::Instance()->MakeString(10009);
	std::string title = SysNotifyMgr::Instance()->MakeString(10010);
	std::string content = SysNotifyMgr::Instance()->MakeString(10011, rewardName);
	std::string reason = GetReason(SOURCE_TYPE_GUILD_AUCTION_REFUND, guildId, auctionType);

	ItemReward reward(GUILD_AUCTION_COIN, num, 0, 0, 0);
	ItemRewardVec rewardVec;
	rewardVec.push_back(reward);
	if (MailBox::SendSysMail(playerId, sender, title, content, reason, rewardVec) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "Guild Auction Refund Faild PlayerId:"<<playerId<<" num:"<< num << LogStream::eos;
	}
}

void GuildAuctionMgr::_AuctionDeal(const GuildAuctionItem& auctionItem)
{
	// 给玩家加奖励邮件
	std::string sender = SysNotifyMgr::Instance()->MakeString(10009);
	std::string title = SysNotifyMgr::Instance()->MakeString(10012);
	std::string content = SysNotifyMgr::Instance()->MakeString(10013);
	std::string reason = GetReason(SOURCE_TYPE_GUILD_AUCTION_REWARD, auctionItem.guildId, auctionItem.auctionType);

	if (MailBox::SendSysMail(auctionItem.bidRoleId, sender, title, content, reason, auctionItem.auctionRewardVec) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "Guild Auction Reward Faild PlayerId:" << auctionItem.bidRoleId << LogStream::eos;
	}
}

void GuildAuctionMgr::_AddGuildAuctionIndex(UInt64 guildId, UInt64 guid)
{
	auto iter = m_GuildAuctionIndex.find(guildId);
	if (iter == m_GuildAuctionIndex.end())
	{
		std::set<UInt64> tempSet;
		tempSet.insert(guid);
		m_GuildAuctionIndex[guildId] = tempSet;
	}
	else
	{
		iter->second.insert(guid);
	}
}

const std::set<UInt64>* GuildAuctionMgr::_GetGuildAuctionIndex(UInt64 guildId)
{
	auto iter = m_GuildAuctionIndex.find(guildId);
	if (iter != m_GuildAuctionIndex.end())
	{
		return &(iter->second);
	}

	return NULL;
}

bool GuildAuctionMgr::_CheckGuildAuctionIsHasItem(UInt64 guildId)
{
	auto iter = m_GuildAuctionIndex.find(guildId);
	if (iter == m_GuildAuctionIndex.end())
	{
		return false;
	}

	return !iter->second.empty();
}

void GuildAuctionMgr::_NotifyAuctionEntrance(UInt32 type, bool _isOpen, UInt64 guildId)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_GUILD_AUCTION))
		return;

	CLProtocol::WorldGuildAuctionNotify notify;
	notify.type = type;
	notify.isOpen = _isOpen ? 1 : 0;
	if (G_AUCTION_GUILD == type)
	{
		Guild* guild = GuildMgr::Instance()->FindGuildByID(guildId);
		if (NULL != guild)
		{
			guild->Broadcast(notify);
		}
	}
	else if (G_AUCTION_WORLD == type)
	{
		UInt32 lvl = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_WORLD_AUCTION_LVL);
		Router::BroadcastToPlayerByLvl(lvl, notify);
	}
}

void GuildAuctionMgr::_ClearAuctionData()
{
	m_GuildAuctionIndex.clear();
	m_AuctionGuildMap.clear();
	m_AuctionWorldMap.clear();

	std::ostringstream cond;
	cond << "1 = 1";
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_guild_auction_item", cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

std::string GuildAuctionMgr::_GetRewardStr(const ItemRewardVec& rewardVec)
{
	std::string ss;
	ss.reserve(256);
	for (auto& it : rewardVec)
	{
		ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(it.id);
		if (NULL == itemEntry)
		{
			ErrorLogStream << "_GetRewardStr not find itemEntry Failed! itemId:" << it.id << LogStream::eos;
			continue;
		}

		ss.append(itemEntry->name).append(",");
	}

	if (!ss.empty())
		ss.pop_back();

	return ss;
}

void GuildAuctionMgr::_NotifyClientResult(WSPlayer* player, UInt32 sourceType, UInt32 retCode)
{
	if (SOURCE_TYPE_GUILD_AUCTION_BID == sourceType)
	{
		CLProtocol::WorldGuildAuctionBidRes res;
		res.retCode = retCode;
		player->SendProtocol(res);
	}
	else if (SOURCE_TYPE_GUILD_AUCTION_FIX == sourceType)
	{
		CLProtocol::WorldGuildAuctionFixRes res;
		res.retCode = retCode;
		player->SendProtocol(res);
	}
}

UInt32 GuildAuctionMgr::_GetErrorCodeByAucState(UInt32 aucState)
{
	switch (aucState)
	{
	case GAI_STATE_PREPARE:return ErrorCode::GUILD_AUCTION_PREPARE;
	case GAI_STATE_DEAL:return ErrorCode::GUILD_AUCTION_DEAL;
	case GAI_STATE_ABORATION:return ErrorCode::GUILD_AUCTION_ABORATION;
	case GAI_STATE_NORMAL:return ErrorCode::SUCCESS;
	default:
		break;
	}

	return ErrorCode::GUILD_AUCTION_NOT_SEAL_STATE;
}

bool GuildAuctionMgr::_IsCanAuction(UInt32 state)
{
	switch (state)
	{
	case GAI_STATE_PREPARE:break;
	case GAI_STATE_NORMAL:break;
	default: return false;
	}

	return true;
}

void GuildAuctionMgr::_CheckAuctionBonus(UInt64 guildId)
{
	// 检查如果公会所有的物品都拍卖完毕，一起发放分成奖励
	if (CheckGuildIsAuction(guildId))
		return;

	 UInt32 bonusReward = _GetBonusUpdateBonusState(guildId);
	 if (bonusReward == 0)
		 return;

	InfoLogStream << "Guild Auction Bonus GuildId:" << guildId << "bonusReward:" << bonusReward << LogStream::eos;

	GuildDungeonMgr::Instance()->GuildAuctionBonus(guildId, bonusReward);
}

UInt32 GuildAuctionMgr::_GetBonusUpdateBonusState(UInt64 guildId)
{
	UInt32 reward = 0;
	auto iter = m_GuildAuctionIndex.find(guildId);
	if (iter != m_GuildAuctionIndex.end())
	{
		for (auto& id : iter->second)
		{
			auto item = m_AuctionGuildMap.find(id);
			if (item == m_AuctionGuildMap.end())
				continue;

			if (item->second.isBonus != 0)
				continue;

			if (item->second.state == GAI_STATE_DEAL)
			{
				reward += item->second.curPrice;
				item->second.isBonus = 1;
				_UpdateAuctionItem(item->second);
			}
		}
	}

	for (auto& it : m_AuctionWorldMap)
	{
		if (it.second.guildId != guildId)
			continue;

		if (it.second.isBonus != 0)
			continue;

		if (it.second.state == GAI_STATE_DEAL)
		{
			reward += it.second.curPrice;
			it.second.isBonus = 1;
			_UpdateAuctionItem(it.second);
		}
	}

	return reward;
}

void GuildAuctionMgr::_UdpGuildAuction(UInt32 opType, const GuildAuctionItem& auctionItem)
{
	Guild* guild = GuildMgr::Instance()->FindGuildByID(auctionItem.guildId);
	if (NULL == guild)
	{
		ErrorLogStream << "Not Find Guild Id:" << auctionItem.guildId << LogStream::eos;
		return;
	}

	for (auto& iter : auctionItem.auctionRewardVec)
	{
		Sys::SendUdpLog("guild_auction", LOG_GUILD_AUCTION, GAME_VERSION.c_str(), ZONE_ID, (UInt32)CURRENT_TIME.Sec(), PLATFORM.c_str(), opType, auctionItem.auctionType, auctionItem.guildId, guild->GetLevel(), auctionItem.guid, auctionItem.rewardGroup, iter.id, iter.num);
	}
}

void GuildAuctionMgr::_UdpAuctionDeal(const GuildAuctionItem& auctionItem, UInt32 param)
{
	Guild* guild = GuildMgr::Instance()->FindGuildByID(auctionItem.guildId);
	if (NULL == guild)
	{
		ErrorLogStream << "Not Find Guild Id:"<< auctionItem.guildId << LogStream::eos;
		return;
	}

	for (auto& iter : auctionItem.auctionRewardVec)
	{
		Sys::SendUdpLog("guild_auction_deal", LOG_GUILD_AUCTION_DEAL, GAME_VERSION.c_str(), ZONE_ID, (UInt32)CURRENT_TIME.Sec(), PLATFORM.c_str(), auctionItem.auctionType, auctionItem.guildId, guild->GetLevel(), auctionItem.guid, auctionItem.rewardGroup, iter.id, iter.num, auctionItem.curPrice, auctionItem.bidNum, param, auctionItem.bidRoleId, auctionItem.vipLevel, auctionItem.level, auctionItem.occu);
	}
}

void GuildAuctionMgr::_UdpAuctionBid(WSPlayer* player, const GuildAuctionItem& auctionItem)
{
	if (NULL == player) return;
	
	Guild* guild = GuildMgr::Instance()->FindGuildByID(auctionItem.guildId);
	if (NULL == guild)
	{
		ErrorLogStream << "Not Find Guild Id:" << auctionItem.guildId << LogStream::eos;
		return;
	}

	for (auto& iter : auctionItem.auctionRewardVec)
	{
		player->SendUdpLog("guild_auction_bid", LOG_GUILD_AUCTION_BID, auctionItem.auctionType, auctionItem.guildId, guild->GetLevel(), auctionItem.guid, auctionItem.rewardGroup, iter.id, iter.num, auctionItem.curPrice, auctionItem.bidNum);
	}
}

void GuildAuctionMgr::StopAuction(UInt64 guildId)
{
	auto iter = m_GuildAuctionIndex.find(guildId);
	if (iter != m_GuildAuctionIndex.end())
	{
		for (auto& id : iter->second)
		{
			auto item = m_AuctionGuildMap.find(id);
			if (item == m_AuctionGuildMap.end())
				continue;

			if (_IsCanAuction(item->second.state))
			{
				item->second.state = GAI_STATE_ABORATION;
				_UpdateAuctionItem(item->second);
			}
		}
	}

	for (auto& it : m_AuctionWorldMap)
	{
		if (it.second.guildId != guildId)
			continue;

		if (_IsCanAuction(it.second.state))
		{
			it.second.state = GAI_STATE_ABORATION;
			_UpdateAuctionItem(it.second);
		}
	}

	// 检查是否可以分成
	_CheckAuctionBonus(guildId);
}