#include "AccountMgr.h"
#include <CLRecordClient.h>
#include "WSPlayerMgr.h"
#include "AccountLoadDataCallback.h"
#include "WSServiceSwitchMgr.h"

AccountMgr::AccountMgr()
{

}

AccountMgr::~AccountMgr()
{
	for (auto itr : m_Accounts)
	{
		auto account = itr.second;
		if (account)
		{
			delete account;
		}
	}
	m_Accounts.clear();
}

void AccountMgr::Final()
{
	for (auto itr : m_Accounts)
	{
		auto account = itr.second;
		if (account)
		{
			account->SaveToDB();
			delete account;
		}
	}
	m_Accounts.clear();
}

void AccountMgr::OnTick(const Avalon::Time& now)
{
	for (auto itr : m_Accounts)
	{
		auto account = itr.second;
		if (account)
		{
			account->OnTick(now);
		}
	}
}

AccountInfo* AccountMgr::FindAccount(UInt32 accid)
{
	auto itr = m_Accounts.find(accid);
	return itr == m_Accounts.end() ? NULL : itr->second;
}

void AccountMgr::AddAccount(AccountInfo* info)
{
	if (!info)
	{
		return;
	}

	// 先找一下，如果已经有了，就替换
	auto tmp = FindAccount(info->GetAccID());
	if (tmp)
	{
		*tmp = *info;
		delete info;
		info = tmp;
	}
	else
	{
		m_Accounts[info->GetAccID()] = info;
	}

	// 重新设置玩家里面的账号信息指针
	auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(info->GetAccID());
	if (player)
	{
		player->SetAccountInfo(info);
	}
}

void AccountMgr::DelAccount(UInt32 accid)
{
	auto itr = m_Accounts.find(accid);
	if (itr == m_Accounts.end())
	{
		return;
	}

	InfoLogStream << "delete account(" << accid << ")'s info." << LogStream::eos;

	auto info = itr->second;
	if (info)
	{
		delete info;
	}

	m_Accounts.erase(itr);
}

void AccountMgr::LoadAccountInfo(UInt32 accid, AccountCallback* cb)
{
	auto info = FindAccount(accid);
	if (info)
	{
		if (cb)
		{
			cb->OnFinish(info);
			delete cb;
		}
		return;
	}

	AccountInfo* account = new AccountInfo();
	account->SetAccID(accid);
	account->SetLoadFinishCallback(cb);

	// 加载账号信息
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountCallback(account));
	}
	
	// 加载充值信息
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_charge_record");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadChargeRecordCallback(account));
	}

	// 加载地下城难度信息
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_g_dungeon_hard");
		cmd->PutCond("account", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadDungeonHardCallback(account));
	}

	// 加载账号任务
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_activity_acc_task");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountActivityTaskCallback(account));
	}

	// 加载账号商店商品
	{
		//auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_shop_item");
		//cmd->PutCond("owner", accid);
		//CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountShopItemCallback(account));
	}

	// 加载账号商店账号购买记录
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_shop_acc_buy_record");
		cmd->PutCond("owner", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountShopItemAccountBuyRecordCallback(account));
	}

	// 加载账号计数
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_counter");
		cmd->PutCond("owner", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountCounterCallback(account));
	}

	// 加载商城账号购买记录
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_mall_acc_buy_record");
		cmd->PutCond("owner", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountMallItemAccountBuyRecordCallback(account));
	}

	// 加载角色评分
	{
		if (WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ADVENTURE_TEAM_SORTLIST))
		{
			auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
			cmd->PutCond("accid", accid);
			cmd->PutColumn("role_value_score");
			std::ostringstream limitoss;
			limitoss << " AND (`deletetime` = 0 OR `deletetime` > " << UInt32(CURRENT_TIME.Sec()) - DELETE_PLAYER_SAVE_TIME << ")";
			cmd->SetLimit(limitoss.str());
			CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountRoleValueScoreCallback(account));
		}
	}

	// 加载远征地图
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_expedition_map");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountExpeditionMapCallback(account));
	}

	// 加载远征成员
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_expedition_member");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountExpeditionMemberCallback(account));
	}

	// 加载账号任务
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_task");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountTaskCallback(account));
	}

	
	// 加载账号头衔
	{
		account->GetTitleMgr().LoadTitles();
	}	
	
	// 加载大富翁
	{
		account->GetMonopolyInfo().LoadData();
	}
}

void AccountMgr::SetRoleID2AccID(ObjID_t roleid, UInt32 accid)
{
	m_RoleId2Accid[roleid] = accid;
}

UInt32 AccountMgr::GetAccIDByRoleID(ObjID_t roleid)
{
	auto itr = m_RoleId2Accid.find(roleid);
	return itr == m_RoleId2Accid.end() ? 0 : itr->second;
}