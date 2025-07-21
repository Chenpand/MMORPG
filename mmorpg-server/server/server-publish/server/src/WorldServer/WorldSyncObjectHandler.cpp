#include "WorldSyncObjectHandler.h"

#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "GuildMgr.h"
#include "AccountMgr.h"
#include "GameParamMgr.h"
#include "AdventurePassMgr.h"

void HandleProtocol(CLProtocol::WorldSyncPlayerBaseInfo& sync)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(sync.playerId);
	if(player == NULL) return;

	if(player->GetLevel() != sync.level)
	{
		player->SetLevel(sync.level);
		player->OnLevelChanged(sync.oldLevel);
	}

	if (player->GetVipExp() != sync.vipExp)
	{
		player->SetVipExp(sync.vipExp);
	}

	if(player->GetVipLvl() != sync.vip)
	{
		player->SetVipLvl(sync.vip);
		player->OnVipLvlChanged();
	}

	if (player->GetTotalChargeNum() != sync.totalChargeNum)
	{
		player->SetTotalChargeNum(sync.totalChargeNum);
	}

	if (player->GetMonthCardExpireTime() != sync.monthCardExpireTime)
	{
		player->SetMonthCardExpireTime(sync.monthCardExpireTime);
	}

	if(sync.power > player->GetPower())
	{
		UInt32 oldPower = player->GetPower();
		player->SetPower(sync.power);
		if(oldPower != 0)
		{
			player->OnPowerChanged();
		}
	}

	if (sync.point != player->GetPoint())
	{
		player->SetPoint(sync.point);
	}

	if (sync.creditPoint != player->GetCreditPoint())
	{
		player->SetCreditPoint(sync.creditPoint);
	}

	if (sync.occu != player->GetOccu())
	{
		player->SetOccu(sync.occu);
		player->OnOccuChanged();
	}

	if (sync.seasonLv != player->GetSeasonLv())
	{
		player->SetSeasonLv(sync.seasonLv);
		player->OnSeasonLvChanged();
	}

	if (sync.auctionRefreshTime > player->GetAuctionRefreshTime())
	{
		player->SetAuctionRefreshTime(sync.auctionRefreshTime);
	}

	if (sync.auctionAdditionBooth > player->GetAuctionAdditionBooth())
	{
		player->SetAuctionAdditionBooth(sync.auctionAdditionBooth);
	}

	auto guildMember = GuildMgr::Instance()->FindGuildMember(player->GetID());
	if (guildMember)
	{
		guildMember->SetActiveDegree(sync.activeDegree);
	}

	if (player->GetTotalPlayerChargeNum() != sync.totalPlayerChargeNum)
	{
		player->SetTotalPlayerChargeNum(sync.totalPlayerChargeNum);
	}

	if (player->GetMoneyManageStatus() != sync.moneyManageStatus)
	{
		player->SetMoneyManageStatus(sync.moneyManageStatus);
	}

	if (player->GetHeadFrame() != sync.headFrame)
	{
		player->SetHeadFrame(sync.headFrame);
		player->OnHeadFrameChanged(sync.headFrame);
	}

	if (player->GetReturnYearTitle() != sync.returnYearTitle)
	{
		player->SetReturnYearTitle(sync.returnYearTitle);
		player->GetRelationMgr().OnReturnYearTitle();
	}

	if (player->GetSuit() != sync.haveSuit)
	{
		player->SetSuit(sync.haveSuit);
	}

	player->OnExpIncrease(sync.expInfo.incExp, sync.expInfo.reason);
}

void HandleProtocol(CLProtocol::WorldNotifyKillPlayer& notify)
{
	WSPlayer* attacker = WSPlayerMgr::Instance()->FindNormalPlayer(notify.killer);
	if(attacker == NULL) return;

	WSPlayer* dieder = WSPlayerMgr::Instance()->FindNormalPlayer(notify.dieder);
	if(dieder == NULL) return;
	if(attacker == dieder) return;

	dieder->OnKilled(attacker);
}

void HandleProtocol(CLProtocol::SceneSyncEvent& sync)
{
	auto player = WSPlayerMgr::Instance()->FindPlayer(sync.playerId);
	if (!player)
	{
		return;
	}

	player->OnSyncSceneEvent((SyncEventType)sync.type, sync.param1, sync.param2, sync.param3);
}

void HandleProtocol(CLProtocol::SceneSyncOfflineEvent& sync)
{
	OfflineEvent* event = NULL;
	switch (sync.type)
	{
	case OFFLINE_GOLD_CONSIGNMENT_POINT:
	{
		event = new GoldConsignmentPointEvent(sync.param1);
		GlobalOfflineEventMgr::Instance()->AddOfflineEvent(sync.playerId, event);
	}
	break;
	default:return;
	}

	if (event != NULL)
		event->InsertToDB(sync.playerId);
}

void HandleProtocol(CLProtocol::SceneSyncPlayerAvatar& sync)
{
	auto player = WSPlayerMgr::Instance()->FindPlayer(sync.id);
	if (!player)
	{
		return;
	}

	player->SetAvatar(sync.avatar);
}

void HandleProtocol(CLProtocol::SceneSyncAccountInfo& sync)
{
	auto accountInfo = AccountMgr::Instance()->FindAccount(sync.accid);
	if (NULL == accountInfo)
	{
		class SelectAccountSceneInfoLock : public AccountCallback
		{
		public:
			SelectAccountSceneInfoLock(UInt32 accid, UInt32 type, UInt32 value) : AccountCallback(accid), m_Type(type), m_Value(value) {}
			void OnFinish(AccountInfo* info)
			{
				InfoLogStream << "offline set accid success :" << m_AccId << " type:" << m_Type << " value:" << m_Value << LogStream::eos;
				if (m_Type == ACCOUNT_POINT)
				{
					info->SetPoint(m_Value);
				}
				else if (m_Type == ACCOUNT_CREDIT_POINT) 
				{
					info->SetCreditPoint(m_Value);
				}
			}

			void OnFailure()
			{
				ErrorLogStream << "offline set accid error :" << m_AccId << " type:" << m_Type << " value:" << m_Value << LogStream::eos;
			}

		private:
			UInt32 m_Type;
			UInt32 m_Value;
		};

		AccountMgr::Instance()->LoadAccountInfo(sync.accid, new SelectAccountSceneInfoLock(sync.accid, sync.type, sync.value));
		return;
	}

	auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(sync.accid);
	if (sync.type == ACCOUNT_POINT)
	{
		accountInfo->SetPoint(sync.value);
	}
	else if (sync.type == ACCOUNT_CREDIT_POINT)
	{
		accountInfo->SetCreditPoint(sync.value);
	}
	else if (sync.type == ACCOUNT_VIP_LEVEL)
	{
		accountInfo->SetVipLevel((UInt8)sync.value);

		InfoLogStream << "account(" << sync.accid << ") new vip level(" << accountInfo->GetVipLevel() << ")." << LogStream::eos;
		accountInfo->SaveToDB();

		if (player)
		{
			player->OnVipLvlChanged();
		}
	}
	else if (sync.type == ACCOUNT_VIP_EXP)
	{
		accountInfo->SetVipExp(sync.value);

		InfoLogStream << "account(" << sync.accid << ") new vip exp(" << accountInfo->GetVipExp() << ")." << LogStream::eos;
		accountInfo->SaveToDB();
	}
	else if (sync.type == ACCOUNT_TOTAL_CHARGE_NUM)
	{
		accountInfo->SetTotalChargeNum(sync.value);
		if (player)
		{
			player->RequestGlobalServerInfo();
		}
	}
	else if (sync.type == ACCOUNT_MONEY_MANAGE_STATUS)
	{
		accountInfo->SetMoneyManageStatus((UInt8)sync.value);
	}
	else if (sync.type == ACCOUNT_WEAPON_LEASE_TICKETS)
	{
		accountInfo->SetWeaponLeaseTickets(sync.value);
	}
	else if (sync.type == ACCOUNT_MALL_POINT)
	{
		accountInfo->SetMallPoint(sync.value);
	}
	else if (sync.type == ACCOUNT_ACTIVITY)
	{
		AdventurePassMgr::Instance()->OnSyncActivity(sync.accid,sync.value);
	}
	else if (sync.type == ACCOUNT_ADVENTURE)
	{
		accountInfo->SetAdventureCoin(sync.value);
	}
	else if (sync.type == ACCOUNT_MONOPOLY_COIN)
	{
		if (player == nullptr)
		{
			ErrorLogStream << "receive ACCOUNT_MONOPOLY_COIN,but can't find player" << LogStream::eos;
			return;
		}
		player->GetMonopolyInfo().SetCoin(player->GetMonopolyInfo().GetCoin() + sync.value, GetReason(SOURCE_MONOPOLY_ITEM));
	}
	else if (sync.type == ACCOUNT_MONOPOLY_CARD)
	{
		if (player == nullptr)
		{
			ErrorLogStream << "receive ACCOUNT_MONOPOLY_COIN,but can't find player" << LogStream::eos;
			return;
		}
		player->GetMonopolyInfo().AddCard(sync.value, 1);
	}	
	else if (sync.type == ACCOUNT_MONOPOLY_ROLL)
	{
		if (player == nullptr)
		{
			ErrorLogStream << "receive ACCOUNT_MONOPOLY_COIN,but can't find player" << LogStream::eos;
			return;
		}
		player->GetMonopolyInfo().SetRemainRoll(player->GetMonopolyInfo().GetRemainRoll() + sync.value, GetReason(SOURCE_MONOPOLY_ITEM));
	}
	else if (sync.type == ACCOUNT_GIFT_RIGHT)
	{
		if (player == nullptr)
		{
			ErrorLogStream << "receive ACCOUNT_GIFT_RIGHT,but can't find player" << LogStream::eos;
			return;
		}
		std::bitset<32> tempBitSet(sync.value);
		player->SetHaveGiftRightCard(tempBitSet[0]);
		player->SetCanBuyGiftRightCard(tempBitSet[1]);
	}
	else if (sync.type == ACCOUNT_DUNGEON_RIGHT)
	{
		if (player == nullptr)
		{
			ErrorLogStream << "receive ACCOUNT_DUNGEON_RIGHT,but can't find player" << LogStream::eos;
			return;
		}
		player->SetCanBuyDungeonRightCard(sync.value > 0 ? true : false);
	}
	else if (sync.type == ACCOUNT_ADVENTURE_EXP)
	{
		auto pass = AdventurePassMgr::Instance()->GetAdventurePass(accountInfo->GetAccID());
		if (pass != nullptr)
		{
			pass->IncExp(sync.value);
		}
	}
	else if (sync.type == ACCOUNT_ADVENTURE_KING)
	{
		InfoLogStream << "receive ACCOUNT_ADVENTURE_KING accid is " << sync.accid << "sync value is" << sync.value << LogStream::eos;
		if (!AdventurePassMgr::Instance()->OnBuyPassRet(sync.accid, ADVENTURE_PASS_TYPE_KING))
		{
			InfoLogStream << "open king faild" << LogStream::eos;
			if (sync.value == APOKT_RMB)
			{
				auto season = AdventurePassSeasonDataEntryMgr::Instance()->GetSeasonData(CURRENT_TIME);
				if (season == nullptr)
				{
					ErrorLogStream << "season is null " << LogStream::eos;
					return;
				}
				UInt32 price = AdventurePassBuyRewardDataEntryMgr::Instance()->GetPrice(season->id, ADVENTURE_PASS_TYPE_KING);

				if (price <= 0)
				{
					ErrorLogStream << "price is 0 season id is" << season->id << LogStream::eos;
					return;
				}

				InfoLogStream << "rpirce is "<< price << LogStream::eos;

				//ÓÃrmbµÄ½âËøÊ§°Ü·µµãÈ¯
				if (player)
				{
					player->AddMoneyItem(GetReason(SOURCE_TYPE_ADVENTURE_PASS_BUY).c_str(), ST_ITEM_POINT, price);
					InfoLogStream << "player online,AddMoneyItem " << LogStream::eos;
				}
				else
				{
					accountInfo->SetPoint(accountInfo->GetPoint() + price);
					InfoLogStream << "player not online,now point is "<< accountInfo->GetPoint() << LogStream::eos;
				}
			}
		}
	}
	else
	{
		ErrorLogStream << "account(" << sync.accid << ") sync(" << sync.type << ") value(" << sync.value << "), invalid type." << LogStream::eos;
	}
}

//·ÏÆú
void HandleProtocol(WSPlayer* player, CLProtocol::WorldWatchPlayerReq& req)
{
	WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayer(req.playerId);
	if(target == NULL) target = WSPlayerMgr::Instance()->FindNormalPlayerByName(req.name);
	if(target == NULL)
	{
		player->SendNotify(1301);
		return;
	}
	if(target == player) return;

	CLProtocol::SceneWatchPlayerReq sceneReq;
	sceneReq.watcher = player->GetID();
	sceneReq.target = target->GetID();
	target->SendToScene(sceneReq);

	target->SendNotify(1302, player->GetName().c_str());
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldPlayerPowerReq& req)
{
	WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayerByName(req.name);
	if(target == NULL)
	{
		player->SendNotify(1301);
		return;
	}
	if(target == player) return;

	CLProtocol::ScenePlayerPowerReq	sceneReq;
	sceneReq.watcher = player->GetID();
	sceneReq.target = target->GetID();
	target->SendToScene(sceneReq);
}

void HandleProtocol(CLProtocol::WorldAccountCounterSync& sync)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(sync.roleId);
	if (player == NULL)
		return;

	player->GetAccountInfo()->SetAccountCounterNum(player, AccountCounterType(sync.accCounter.type), sync.accCounter.num);
}

void HandleProtocol(CLProtocol::SceneSyncRoleValueScore& req)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(req.roleid);
	if (!player) return;

	player->OnRoleValueScoreSync(req.roleValueScore);
	DebugLogStream << PLAYERINFO(player) << ", req.isCreateAdvent : " << req.isCreateAdvent << LogStream::eos;
	if (req.isCreateAdvent == 1 && player->GetAccountInfo())
	{
		InfoLogStream << "refresh task" << LogStream::eos;
		player->GetAccountInfo()->GetWeeklyTaskMgr().RefreshTasks();
		player->GetAccountInfo()->GetAccountAchievementTaskMgr().OnCreateAdventTeam();
	}
}

void HandleProtocol(CLProtocol::SceneGameParamSync& sync)
{
	for (auto& iter : sync.gameParamMap)
	{
		GameParamMgr::Instance()->SetValue(iter.first.c_str(), iter.second);
	}
}