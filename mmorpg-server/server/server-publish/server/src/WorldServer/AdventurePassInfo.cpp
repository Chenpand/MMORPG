#include "AdventurePassInfo.h"
#include <CLAdventurePassDataEntry.h>
#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>
#include <CLItemProtocol.h>
#include <CLFunctionDataEntry.h>
#include <CLSettingProtocol.h>
#include <CLWorldAdventurePassProtocol.h>

#include "AccountMgr.h"
#include "WSPlayerMgr.h"
#include "AdventurePassMgr.h"
#include "WSPlayer.h"
#include "WSItemTransaction.h"

const UInt32 ADVENTURE_PASS_UNLOCK_LEVEL = 16;

class AeventurePassResetTransaction : public ItemTransaction
{
public:
	AeventurePassResetTransaction(WSPlayer *player)
		: ItemTransaction(player) {}
	~AeventurePassResetTransaction() {}

	void OnCheckSucced(WSPlayer *player)
	{
		
		if (!player)
		{
			return;
		}

		std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ADVENTURE_PASS);
		RemoveItem(reason);
	}

	void OnCheckFailed(WSPlayer *player)
	{
		UnLock();
		ErrorLogStream << PLAYERINFO(player) << " check failed!" << LogStream::eos;
	}

private:

};


AdventurePassInfo::AdventurePassInfo(UInt32 accid)
{
	//账号ID
	m_Accid = accid;
	//通行证等级
	m_Level = 1;
	//通行证经验
	m_Exp = 0;
	//通行证类型
	m_PassType = 0;
	//普通奖励领取标记
	m_NormalReward = std::string(ADVENTURE_PASS_MAX_LEVEL+1, '1');
	//高级奖励领取标记
	m_HighReward = std::string(ADVENTURE_PASS_MAX_LEVEL+1, '1');
	//功能是否解锁
	m_Unlock = 0;
	m_SeasonID = 0;
	m_ExtraExp = 0;
	m_SaveTimer.SetInterval(1000);
}


AdventurePassInfo::~AdventurePassInfo()
{
}

void AdventurePassInfo::OnTick(const AdventurePassSeasonDataEntry* season, const Avalon::Time &now)
{

	bool change = false;
	if (season != nullptr)
	{
		if (season->id != 0 && GetSeasonID() != season->id)
		{
			change = true;
		}
	}
	else if (m_Unlock)
	{
		change = true;
	}

	if (change)
	{
		auto account = AccountMgr::Instance()->FindAccount(GetAccid());
		if (account != nullptr)
		{
			auto player = WSPlayerMgr::Instance()->FindNormalPlayer(account->GetRoleID());
			if (player != nullptr)
			{
				//在线的直接调online调整通行证状态
				AdventurePassMgr::Instance()->OnOnline(player);
			}
		}
	}

	if (m_SaveTimer.IsTimeout(now))
	{
		SaveToDB();
	}
}

void AdventurePassInfo::OnWeekChange()
{
	if (GetPassType() == ADVENTURE_PASS_TYPE_KING)
	{
		m_ExtraExp = ADVENTURE_PASS_EXTRA_EXP_TYPE_UNUSE;
	}
	else
	{
		m_ExtraExp = ADVENTURE_PASS_EXTRA_EXP_TYPE_UNLOCK;
	}
}

void AdventurePassInfo::OnDayChanged()
{
	m_Activity = 0;
}

void AdventurePassInfo::Reset(WSPlayer *player, UInt32 seasonID)
{
	if (seasonID == GetSeasonID())
	{
		return;
	}

	//上赛季通行证重置后，下个赛季可以获得一定等级的继承，每10级继承1级，继承的等级上限7级；只继承上赛季的等级，无法跨赛季继承（上期重置，本期不登陆，下期登陆，不继承等级）
	if (seasonID == GetSeasonID() + 1)
	{
		m_Level = 1 + (m_Level / 10 > 7 ? 7 : m_Level / 10);
	}
	else if (seasonID != 0)
	{
		m_Level = 1;
	}

	//通行证经验
	m_Exp = 0;
	//通行证类型
	m_PassType = 0;
	//普通奖励领取标记
	m_NormalReward = std::string(ADVENTURE_PASS_MAX_LEVEL + 1, '1');
	//高级奖励领取标记
	m_HighReward = std::string(ADVENTURE_PASS_MAX_LEVEL + 1, '1');
	//功能是否解锁
	m_Unlock = 0;
	m_ExtraExp = 0;

	m_SeasonID = seasonID;

	auto account = AccountMgr::Instance()->FindAccount(GetAccid());
	if (account != nullptr)
	{
		if (player != nullptr)
		{
			auto trans = new AeventurePassResetTransaction(player);
			trans->SetItem(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ADVENTURE_PASS_COIN), account->GetAdventureCoin());
			if (!player->BeginItemTransaction(trans))
			{
				ErrorLogStream << PLAYERINFO(player) << " begin transaction failed!" << LogStream::eos;
			}
		}
	}
	else
	{
		InfoLogStream << "error when reset account adventure coin account id is" << GetAccid() << LogStream::eos;
	}

}

void AdventurePassInfo::LoadData(CLRecordCallback * callback)
{
	SetID(callback->GetKey());
	SetDBData(callback);
	if (static_cast<std::string>(m_NormalReward) == "")
	{
		m_NormalReward = std::string(ADVENTURE_PASS_MAX_LEVEL + 1, '0');
	}
	if (static_cast<std::string>(m_HighReward) == "")
	{
		m_HighReward = std::string(ADVENTURE_PASS_MAX_LEVEL + 1, '0');
	}

}

void AdventurePassInfo::InsertToDB()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_account_adventure", GetID());
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);
	ClearDirty();
}

void AdventurePassInfo::OnOnline(WSPlayer *player)
{

	if (player == nullptr)
	{
		return;
	}

	do 
	{
		auto season = AdventurePassSeasonDataEntryMgr::Instance()->GetSeasonData(CURRENT_TIME);
		if (season != nullptr)
		{
			//下面根据赛季id进行重置工作
			if (season->id != GetSeasonID())
			{
				if (IsUnLock())
				{
					SendResetMail(player);
				}
				Reset(player, season->id);
			}

			if (IsUnLock())
			{
				break;
			}

			//赛季结束前几天不解锁
			if (CURRENT_TIME.Sec() > season->endTime - SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ADVENTURE_PASS_UNLOCK) * Avalon::Time::SECS_OF_DAY)
			{
				break;
			}

			//下面进入解锁的逻辑
			if (player->GetLevel() >= ADVENTURE_PASS_UNLOCK_LEVEL || player->HasRoleLevelLimit(ADVENTURE_PASS_UNLOCK_LEVEL))
			{
				UnLock();
				break;
			}
		}
		else
		{
			if (IsUnLock())
			{
				SendResetMail(player);
			}
			m_Unlock = 0;
		}

	} while (false);
	player->SendAdventurePassStatus();
	player->SendAdventurePassUdpLog(GetLevel(), GetPassType());
}

void AdventurePassInfo::SendResetMail(WSPlayer *player)
{
	if (player == nullptr)
	{
		return;
	}

	//上赛季关闭后，首次登陆的角色会有赛季重置邮件
	if (GetSeasonID() != 0)
	{
		std::string title = SysNotifyMgr::Instance()->MakeString(13001);
		std::string sender = SysNotifyMgr::Instance()->MakeString(13002);
		std::string content = SysNotifyMgr::Instance()->MakeString(13003);
		std::string reason = GetReason(SOURCE_TYPE_ADVENTURE_PASS);
		std::vector<ItemReward> reward;
		MailBox::SendSysMail(player->GetID(), sender, title, content, reason, reward);
	}
}

void AdventurePassInfo::SetNormalReward(UInt32 lv)
{
	if (lv > GetMaxLevel() || lv < 1)
	{
		return;
	}

	std::string reward = m_NormalReward;
	reward[lv] = '0';
	m_NormalReward = reward;
}

UInt32 AdventurePassInfo::GetNormalRewardNum(UInt32 lv)
{
	if (lv > GetMaxLevel() || lv < 1)
	{
		return 0;
	}
	std::string reward = m_NormalReward;
	if (reward.size() < lv)
	{
		return 0;
	}
	if (reward[lv] <= '0')
	{
		return 0;
	}
	return reward[lv]-'0';
}

void AdventurePassInfo::SetHighReward(UInt32 lv)
{
	if (lv > GetMaxLevel() || lv < 1)
	{
		return;
	}
	std::string reward = m_HighReward;
	reward[lv] = '0';
	m_HighReward = reward;
	return;
}

UInt32 AdventurePassInfo::GetHighRewardNum(UInt32 lv)
{

	if (lv > GetMaxLevel() || lv < 1)
	{
		return 0;
	}
	std::string reward = m_HighReward;
	if (reward.size() < lv)
	{
		return 0;
	}
	if (reward[lv] <= '0')
	{
		return 0;
	}
	return reward[lv] - '0';
}

void AdventurePassInfo::SetLevel(UInt32 lv)
{
	if (lv < 1 || lv > GetMaxLevel())
	{
		return;
	}
	m_Level = lv;
}

void AdventurePassInfo::SetPassType(UInt32 type)
{
	if (type != ADVENTURE_PASS_TYPE_KING && type != ADVENTURE_PASS_TYPE_NORMAL)
	{
		return;
	}
	m_PassType = type;
}

bool AdventurePassInfo::IncExp(UInt32 exp)
{
	if (exp <= 0)
	{
		return false;
	}

	auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(GetAccid());
	ItemRewardVec items;
	if (GetLevel() >= GetMaxLevel())
	{
		//满级之后经验转化为冒险币
		UInt32 bons = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ADVENTURE_PASS_EXP_COIN) ? SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ADVENTURE_PASS_EXP_COIN) : 1;
		if (player != nullptr)
		{
			player->AddMoneyItem(GetReason(SOURCE_TYPE_ADVENTURE_PASS).c_str(), ST_ADVENTURE_COIN, exp / bons);
			items.emplace_back(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ADVENTURE_PASS_COIN), exp / bons ,0 ,0);
			player->NotifyClientAddItem(GetReason(SOURCE_TYPE_ADVENTURE_PASS).c_str(), items, 0);
		}
		else
		{
			ErrorLogStream << "error when IncExp turn coin account id is" << GetAccid() << LogStream::eos;
		}
		return false;
	}

	m_Exp = m_Exp + exp;

	if (player != nullptr)
	{
		items.emplace_back(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ADVENTURE_PASS_EXP_ITME), exp, 0, 0);
		player->NotifyClientAddItem(GetReason(SOURCE_TYPE_ADVENTURE_PASS).c_str(), items, 0);
	}

	auto data = AdventurePassRewardDataEntryMgr::Instance()->FindEntry(GetSeasonID(), GetLevel());
	if (data == nullptr)
	{
		return false;
	}
	do 
	{
		if (m_Exp >= data->exp)
		{
			SetLevel(GetLevel() + 1);
			m_Exp = m_Exp - data->exp;
		}
		data = AdventurePassRewardDataEntryMgr::Instance()->FindEntry(GetSeasonID(), GetLevel());
		if (data == nullptr)
		{
			break;
		}
	} while (m_Exp >= data->exp);
	
	if (player != nullptr)
	{
		player->SendAdventurePassStatus();
	}

	return true;
}

void AdventurePassInfo::UnLock()
{
	auto season = AdventurePassSeasonDataEntryMgr::Instance()->GetSeasonData(CURRENT_TIME);
	if (season != nullptr)
	{
		m_SeasonID = season->id;
	}
	else
	{
		return;
	}

	m_Unlock = 1;
	//先获得活跃值后解锁也要给经验
	auto exp = AdventurePassActivityDataEntryMgr::Instance()->GetAddExp(0, GetActivity());
	IncExp(exp);

	auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(GetAccid());
	if (player != nullptr)
	{
		auto funcData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_ADVENTUREPASSSEASON);
		if (funcData != nullptr)
		{
			CLProtocol::WorldSyncFuncUnlock protocol;
			protocol.funcId = FUNC_ADVENTUREPASSSEASON;
			player->SendProtocol(protocol);
		}
	}
}

UInt32 AdventurePassInfo::GetMaxLevel()
{
	auto data = AdventurePassSeasonDataEntryMgr::Instance()->FindEntry(GetSeasonID());
	if (data == nullptr)
	{
		ErrorLogStream << "cant find season config,season id is" << GetSeasonID() << LogStream::eos;
		return 0;
	}

	return data->maxLevel;
}

void AdventurePassInfo::SaveToDB()
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account_adventure", GetID(), false);
	GetDBData(updateCmd, true);
	if (updateCmd->GetDataNum() > 0)
	{
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
	else
	{
		CL_SAFE_DELETE(updateCmd);
	}
}
