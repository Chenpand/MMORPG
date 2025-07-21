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

	// ����һ�£�����Ѿ����ˣ����滻
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

	// �����������������˺���Ϣָ��
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

	// �����˺���Ϣ
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountCallback(account));
	}
	
	// ���س�ֵ��Ϣ
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_charge_record");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadChargeRecordCallback(account));
	}

	// ���ص��³��Ѷ���Ϣ
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_g_dungeon_hard");
		cmd->PutCond("account", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadDungeonHardCallback(account));
	}

	// �����˺�����
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_activity_acc_task");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountActivityTaskCallback(account));
	}

	// �����˺��̵���Ʒ
	{
		//auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_shop_item");
		//cmd->PutCond("owner", accid);
		//CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountShopItemCallback(account));
	}

	// �����˺��̵��˺Ź����¼
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_shop_acc_buy_record");
		cmd->PutCond("owner", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountShopItemAccountBuyRecordCallback(account));
	}

	// �����˺ż���
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_counter");
		cmd->PutCond("owner", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountCounterCallback(account));
	}

	// �����̳��˺Ź����¼
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_mall_acc_buy_record");
		cmd->PutCond("owner", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountMallItemAccountBuyRecordCallback(account));
	}

	// ���ؽ�ɫ����
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

	// ����Զ����ͼ
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_expedition_map");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountExpeditionMapCallback(account));
	}

	// ����Զ����Ա
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_expedition_member");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountExpeditionMemberCallback(account));
	}

	// �����˺�����
	{
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account_task");
		cmd->PutCond("accid", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new LoadAccountTaskCallback(account));
	}

	
	// �����˺�ͷ��
	{
		account->GetTitleMgr().LoadTitles();
	}	
	
	// ���ش���
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