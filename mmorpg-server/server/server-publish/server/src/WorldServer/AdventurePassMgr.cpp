#include "AdventurePassMgr.h"

#include <CLAdventurePassDataEntry.h>
#include "WSSelectInitDataCallback.h"
#include "WSPlayer.h"
#include "CLFunctionDataEntry.h"
#include "CLSettingProtocol.h"
#include "AccountMgr.h"




/**
*@brief 加载冒险通行证信息
*/
class SelectAdeveturePassCallback : public SelectInitDataCallback
{
public:
	SelectAdeveturePassCallback() {

	}

	bool OnLoadFinished()
	{
		AdventurePassMgr::Instance()->LoadDBData(this);
		return true;
	}

};





AdventurePassMgr::AdventurePassMgr()
{
}


AdventurePassMgr::~AdventurePassMgr()
{
}

void AdventurePassMgr::OnTick(const Avalon::Time &now)
{
	auto season = AdventurePassSeasonDataEntryMgr::Instance()->GetSeasonData(now);
	for (auto &i : m_AdventurePassInfos)
	{
		i.second.OnTick(season, now);
	}
}

void AdventurePassMgr::OnWeekChange()
{
	for (auto &i : m_AdventurePassInfos)
	{
		i.second.OnWeekChange();
	}
}

void AdventurePassMgr::OnDayChanged()
{
	for (auto &i : m_AdventurePassInfos)
	{
		i.second.OnDayChanged();
	}
}

void AdventurePassMgr::Init()
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_adventure");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectAdeveturePassCallback());
}

bool AdventurePassMgr::LoadDBData(CLRecordCallback* callback)
{
	while (callback->NextRow())
	{
		AddAdventurePass(callback->GetData("accid"));
		auto pass = GetAdventurePass(callback->GetData("accid"));
		if (pass == nullptr)
		{
			return false;
		}
		pass->LoadData(callback);
	}
	return true;
}

void AdventurePassMgr::AddAdventurePass(UInt32 accid)
{
	auto pass = m_AdventurePassInfos.find(accid);
	if (pass == m_AdventurePassInfos.end())
	{
		m_AdventurePassInfos.insert(std::make_pair(accid,AdventurePassInfo{ accid }));
	}
	else
	{
		InfoLogStream << "PassInfo already have" << LogStream::eos;
	}
}

AdventurePassInfo* AdventurePassMgr::GetAdventurePass(UInt32 accid)
{
	auto pass = m_AdventurePassInfos.find(accid);
	if (pass == m_AdventurePassInfos.end())
	{
		return nullptr;
	}
	else
	{
		return &pass->second;
	}
}

void AdventurePassMgr::OnOnline(WSPlayer *player)
{

	auto pass = GetAdventurePass(player->GetAccId());
	if (pass == nullptr)
	{
		AddAdventurePassAndInsertDB(player->GetAccId());
		pass = GetAdventurePass(player->GetAccId());
		if (pass == nullptr)
		{
			ErrorLogStream<< player->GetAccId() << " pass is null" << LogStream::eos;
			return;
		}
	}

	pass->OnOnline(player);
}

void AdventurePassMgr::AddAdventurePassAndInsertDB(UInt32 accid)
{
	auto pass = GetAdventurePass(accid);
	if (pass != nullptr)
	{
		return;
	}
	AddAdventurePass(accid);
	pass = GetAdventurePass(accid);
	if (pass != nullptr)
	{
		pass->InsertToDB();
	}
}

bool AdventurePassMgr::OnBuyPassRet(UInt32 accid, AdventurePassType type)
{
	auto pass = AdventurePassMgr::Instance()->GetAdventurePass(accid);
	if (pass == nullptr)
	{
		return false;
	}
	if (!pass->IsUnLock())
	{
		return false;
	}

	if (pass->GetPassType() != ADVENTURE_PASS_TYPE_NORMAL)
	{
		return false;
	}

	if (type != ADVENTURE_PASS_TYPE_KING) //&& type != ADVENTURE_PASS_TYPE_TOPKING)
	{
		return false;
	}

	pass->SetLevel(pass->GetLevel() + AdventurePassBuyRewardDataEntryMgr::Instance()->GetLevel(pass->GetSeasonID(), type));
	pass->SetPassType(type);
	pass->SetExtraExp(ADVENTURE_PASS_EXTRA_EXP_TYPE_UNUSE);

	return true;
}

void AdventurePassMgr::OnSyncActivity(UInt32 accid, UInt32 activity)
{
	auto pass = AdventurePassMgr::Instance()->GetAdventurePass(accid);
	if (pass == nullptr)
	{
		return;
	}

	if (activity <= pass->GetActivity())
	{
		return;
	}

	if (pass->IsUnLock())
	{
		auto exp = AdventurePassActivityDataEntryMgr::Instance()->GetAddExp(pass->GetActivity(), activity);
		pass->IncExp(exp);
	}
	pass->SetActivity(activity);
}

UInt32 AdventurePassMgr::CanPlayerBuyKingPassByRMB(UInt32 accid)
{
	auto pass = GetAdventurePass(accid);
	if (pass == nullptr)
	{
		return ErrorCode::BILLING_PLAYER_OFFLINE;
	}

	if (!pass->IsUnLock())
	{
		return ErrorCode::BILLING_GOODS_ADVENTURE_KING_ALREADY;
	}

	if (pass->GetPassType() != ADVENTURE_PASS_TYPE_NORMAL)
	{
		return ErrorCode::BILLING_GOODS_ADVENTURE_KING_ALREADY;
	}

	if (AdventurePassBuyRewardDataEntryMgr::Instance()->GetBuyType(pass->GetSeasonID()) != APBRT_RMB)
	{
		return ErrorCode::BILLING_GOODS_ADVENTURE_KING_CANT_USE_RMB;;
	}

	return ErrorCode::SUCCESS;
}

//void AdventurePassMgr::OnSyncCoin(UInt32 accid, UInt32 num)
//{
//	auto pass = AdventurePassMgr::Instance()->GetAdventurePass(accid);
//	if (pass == nullptr)
//	{
//		return;
//	}
//
//	pass->SetCoin(num);
//}
