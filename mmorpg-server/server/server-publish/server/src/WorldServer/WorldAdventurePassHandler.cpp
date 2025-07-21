#include "CLWorldAdventurePassProtocol.h"
#include "CLAdventurePassDataEntry.h"

#include "WSPlayerMgr.h"
#include "AdventurePassMgr.h"


void HandleProtocol(WSPlayer* player, CLProtocol::WorldAventurePassStatusReq &req)
{
	player->SendAdventurePassStatus();
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAventurePassBuyReq &req)
{
	CLProtocol::WorldAventurePassBuyRet ret;
	ret.type = 0;
	auto pass = AdventurePassMgr::Instance()->GetAdventurePass(player->GetAccId());
	if (pass == nullptr)
	{
		player->SendProtocol(ret);
		return;
	}
	if (!pass->IsUnLock())
	{
		player->SendProtocol(ret);
		return;
	}

	if (pass->GetPassType() != ADVENTURE_PASS_TYPE_NORMAL)
	{
		player->SendProtocol(ret);
		return;
	}

	if (req.type != ADVENTURE_PASS_TYPE_KING) //&& req.type != ADVENTURE_PASS_TYPE_TOPKING)
	{
		player->SendProtocol(ret);
		return;
	}

	if (AdventurePassBuyRewardDataEntryMgr::Instance()->GetPrice(pass->GetSeasonID(), req.type) == 0)
	{
		player->SendProtocol(ret);
		return;
	}

	if (AdventurePassBuyRewardDataEntryMgr::Instance()->GetBuyType(pass->GetSeasonID()) != APBRT_POINT)
	{
		player->SendProtocol(ret);
		return;
	}

	auto trans = new AdventurePassBuyTransaction(player, static_cast<AdventurePassType>(req.type), pass->GetSeasonID());
	//trans->SetItem(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ADVENTURE_PASS_KING_ITEM), 1);
	trans->SetGold(AdventurePassBuyRewardDataEntryMgr::Instance()->GetPrice(pass->GetSeasonID(), req.type));
	if (!player->BeginItemTransaction(trans))
	{
		ErrorLogStream << PLAYERINFO(player) << " failed begin AdventurePassBuyTransaction!" << LogStream::eos;
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAventurePassBuyLvReq &req)
{
	CLProtocol::WorldAventurePassBuyLvRet ret;
	ret.lv = 0;
	auto pass = AdventurePassMgr::Instance()->GetAdventurePass(player->GetAccId());
	if (pass == nullptr)
	{
		player->SendProtocol(ret);
		return;
	}
	if (!pass->IsUnLock())
	{
		player->SendProtocol(ret);
		return;
	}
	if (pass->GetPassType() < ADVENTURE_PASS_TYPE_KING)
	{
		player->SendProtocol(ret);
		return;
	}

	auto data = AdventurePassBuyLevelDataEntryMgr::Instance()->FindEntry(req.lv);
	if (data == nullptr)
	{
		player->SendProtocol(ret);
		return;
	}

	if (pass->GetLevel() + data->buyLv > pass->GetMaxLevel())
	{
		player->SendProtocol(ret);
		return;
	}

	auto trans = new AdventurePassBuyLevelTransaction(player, req.lv);
	trans->SetGold(data->pointCost);
	if (!player->BeginItemTransaction(trans))
	{
		ErrorLogStream << PLAYERINFO(player) << " failed begin AdventurePassBuyLevelTransaction!" << LogStream::eos;
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAventurePassExpPackReq &req)
{
	CLProtocol::WorldAventurePassExpPackRet ret;
	ret.type = 0;
	auto pass = AdventurePassMgr::Instance()->GetAdventurePass(player->GetAccId());
	if (pass == nullptr)
	{
		player->SendProtocol(ret);
		return;
	}
	if (!pass->IsUnLock())
	{
		player->SendProtocol(ret);
		return;
	}

	if (req.op == 0)
	{
		ret.type = pass->GetExtraExp();
		player->SendProtocol(ret);
		return;
	}

	if (pass->GetPassType() != ADVENTURE_PASS_TYPE_KING) //&& pass->GetPassType() != ADVENTURE_PASS_TYPE_TOPKING)
	{
		player->SendProtocol(ret);
		return;
	}

	if (pass->GetExtraExp() != ADVENTURE_PASS_EXTRA_EXP_TYPE_UNUSE)
	{
		player->SendProtocol(ret);
		return;
	}

	pass->SetExtraExp(ADVENTURE_PASS_EXTRA_EXP_TYPE_USE);
	pass->IncExp(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ADVENTURE_PASS_EXP));
	ret.type = ADVENTURE_PASS_EXTRA_EXP_TYPE_USE;
	player->SendProtocol(ret);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAventurePassRewardReq &req)
{
	CLProtocol::WorldAventurePassRewardRet ret;
	ret.lv = req.lv;
	ret.errorCode = ErrorCode::ITEM_DATA_INVALID;
	auto pass = AdventurePassMgr::Instance()->GetAdventurePass(player->GetAccId());
	if (pass == nullptr)
	{
		player->SendProtocol(ret);
		return;
	}
	if (!pass->IsUnLock())
	{
		player->SendProtocol(ret);
		return;
	}

	if (pass->GetLevel() < req.lv)
	{
		player->SendProtocol(ret);
		return;
	}

	if (req.lv == 0)
	{
		for (UInt32 i = 1; i <= pass->GetLevel(); ++i)
		{
			auto data = AdventurePassRewardDataEntryMgr::Instance()->FindEntry(pass->GetSeasonID(), i);
			if (data == nullptr)
			{
				ErrorLogStream << "AdventurePassReward one key faild,req lv is " << i << LogStream::eos;
				continue;
			}

			if (pass->GetPassType() == ADVENTURE_PASS_TYPE_NORMAL)
			{
				if (pass->GetNormalRewardNum(i) <= 0)
				{
					continue;
				}
			}
			else
			{
				if (pass->GetHighRewardNum(i) <= 0 && pass->GetNormalRewardNum(i) <= 0)
				{
					continue;
				}
			}

			ItemRewardVec items;
			if (pass->GetNormalRewardNum(i) > 0)
			{
				auto tempitems = data->GetNormalReward();
				for (auto &j : tempitems)
				{
					j.num *= pass->GetNormalRewardNum(i);
				}
				pass->SetNormalReward(i);
				items.insert(items.end(), tempitems.begin(), tempitems.end());
			}
			if (pass->GetPassType() == ADVENTURE_PASS_TYPE_KING) //|| pass->GetPassType() == ADVENTURE_PASS_TYPE_TOPKING)
			{
				if (pass->GetHighRewardNum(i) > 0)
				{
					auto tempitems = data->GetHighReward();
					for (auto &j : tempitems)
					{
						j.num *= pass->GetHighRewardNum(i);
					}
					pass->SetHighReward(i);
					items.insert(items.end(), tempitems.begin(), tempitems.end());
				}
			}
			player->AddItem(GetReason(LogSourceType::SOURCE_TYPE_ADVENTURE_PASS_REWARD), items);
		}
	}
	else
	{
		auto data = AdventurePassRewardDataEntryMgr::Instance()->FindEntry(pass->GetSeasonID(), req.lv);
		if (data == nullptr)
		{
			player->SendProtocol(ret);
			return;
		}

		if (pass->GetPassType() == ADVENTURE_PASS_TYPE_NORMAL)
		{
			if (pass->GetNormalRewardNum(req.lv) <= 0)
			{
				player->SendProtocol(ret);
				return;
			}
		}
		else
		{
			if (pass->GetHighRewardNum(req.lv) <= 0 && pass->GetNormalRewardNum(req.lv) <= 0)
			{
				player->SendProtocol(ret);
				return;
			}
		}

		if (pass->GetNormalRewardNum(req.lv) > 0)
		{
			auto items = data->GetNormalReward();
			for (auto &i : items)
			{
				i.num *= pass->GetNormalRewardNum(req.lv);
			}
			pass->SetNormalReward(req.lv);
			player->AddItem(GetReason(LogSourceType::SOURCE_TYPE_ADVENTURE_PASS_REWARD), items);

		}

		if (pass->GetPassType() == ADVENTURE_PASS_TYPE_KING) //|| pass->GetPassType() == ADVENTURE_PASS_TYPE_TOPKING)
		{
			if (pass->GetHighRewardNum(req.lv) > 0)
			{
				auto items = data->GetHighReward();
				for (auto &i : items)
				{
					i.num *= pass->GetHighRewardNum(req.lv);
				}
				pass->SetHighReward(req.lv);
				player->AddItem(GetReason(LogSourceType::SOURCE_TYPE_ADVENTURE_PASS_REWARD), items);
			}
		}
	}


	ret.errorCode = ErrorCode::SUCCESS;
	player->SendProtocol(ret);
}

