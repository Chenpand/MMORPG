#ifndef __AVENTURE_PASS_MGR_H__
#define __AVENTURE_PASS_MGR_H__

#include <AvalonSingleton.h>
#include <CLWorldAdventurePassProtocol.h>
#include <CLAdventurePassDataEntry.h>
#include "AdventurePassInfo.h"
#include <CLDefine.h>
#include "WSItemTransaction.h"
#include "WSPlayer.h"
#include "udplog_typedef.h"


class WSPlayer;




class AdventurePassMgr : public Avalon::Singleton<AdventurePassMgr>
{
public:
	AdventurePassMgr();
	~AdventurePassMgr();
	void OnTick(const Avalon::Time &now);
	void OnWeekChange();
	void OnDayChanged();
	void Init();
	bool LoadDBData(CLRecordCallback* callback);
	void AddAdventurePass(UInt32 accid);
	AdventurePassInfo* GetAdventurePass(UInt32 accid);
	void OnOnline(WSPlayer *player);
	void OnLevelChg(WSPlayer *player) { OnOnline(player); }
	void AddAdventurePassAndInsertDB(UInt32 accid);
	bool OnBuyPassRet(UInt32 accid, AdventurePassType type);
	void OnSyncActivity(UInt32 accid, UInt32 activity);
	//void OnSyncCoin(UInt32 accid, UInt32 num);
	UInt32 CanPlayerBuyKingPassByRMB(UInt32 accid);
private:


private:
	HashMap<UInt32, AdventurePassInfo>		m_AdventurePassInfos;
};



class AdventurePassBuyTransaction : public ItemTransaction
{
public:
	AdventurePassBuyTransaction(WSPlayer *player, AdventurePassType type, UInt32 season)
		: ItemTransaction(player), m_Type(type), m_Season(season) {}
	~AdventurePassBuyTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		CLProtocol::WorldAventurePassBuyRet ret;
		if (AdventurePassMgr::Instance()->OnBuyPassRet(player->GetAccId(), m_Type))
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_ADVENTURE_PASS_BUY));
			player->AddItem(GetReason(LogSourceType::SOURCE_TYPE_ADVENTURE_PASS_BUY), AdventurePassBuyRewardDataEntryMgr::Instance()->GetReward(m_Season,m_Type));
			ret.type = m_Type;
			player->SendProtocol(ret);
		}
		else
		{
			ret.type = 0;
			player->SendProtocol(ret);
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		CLProtocol::WorldAventurePassBuyRet ret;
		ret.type = 0;
		player->SendProtocol(ret);

		UnLock();
	}

private:
	AdventurePassType m_Type;
	UInt32 m_Season;
};


class AdventurePassBuyLevelTransaction : public ItemTransaction
{
public:
	AdventurePassBuyLevelTransaction(WSPlayer *player, UInt32 lv)
		: ItemTransaction(player), m_Lv(lv) {}
	~AdventurePassBuyLevelTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		CLProtocol::WorldAventurePassBuyLvRet ret;
		auto pass = AdventurePassMgr::Instance()->GetAdventurePass(player->GetAccId());
		if (pass != nullptr)
		{
			RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_ADVENTURE_PASS_BUY_LEVEL));
			pass->SetLevel(pass->GetLevel() + m_Lv);
			ret.lv = m_Lv;
			player->SendProtocol(ret);
		}
		else
		{
			ret.lv = 0;
			player->SendProtocol(ret);
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer *player)
	{
		CLProtocol::WorldAventurePassBuyLvRet ret;
		ret.lv = 0;
		player->SendProtocol(ret);

		UnLock();
	}

private:
	UInt32  m_Lv;
};



#endif