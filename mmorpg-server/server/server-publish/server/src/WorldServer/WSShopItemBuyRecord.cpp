#include "WSShopItemBuyRecord.h"

#include <CLRecordClient.h>
#include <CLAdventurePassDataEntry.h>


WSShopItemBuyBaseRecord::WSShopItemBuyBaseRecord()
{
	m_ShopItemId = 0;
	m_BuyedNum = 0;
	m_RefreshTime = 0;
	m_CreateTime = 0;
	m_UpdateTime = 0;
}

bool WSShopItemBuyBaseRecord::TryRefresh(TimeUtil::RefreshType refreshType, TimeUtil::RefreshDate refreshDate)
{
	if (refreshType == TimeUtil::REFRESH_TYPE_NONE)
	{
		return false;
	}

	UInt64 curTimeMsec = CURRENT_TIME.MSec();

	if (GetRefreshTime() > curTimeMsec)
	{
		return false;
	}

	UInt64 nextRefreshTimestamp = 0;

	//赛季刷新由当前赛季配置结束时间决定
	if (refreshType == TimeUtil::RefreshType::REFRESH_TYPE_SEASON)
	{
		auto data = AdventurePassSeasonDataEntryMgr::Instance()->GetSeasonData(curTimeMsec);
		if (data == nullptr)
		{
			return false;
		}
		nextRefreshTimestamp = static_cast<UInt64>(data->endTime) * Avalon::Time::MSECS_OF_SEC;
	}
	else
	{
		if (!TimeUtil::CalculateNextRefreshTimestamp(refreshType, refreshDate, nextRefreshTimestamp))
		{
			ErrorLogStream << "Shop item(" << GetShopItemId() << ") calculate next account refresh timestamp failed by refresh type("
				<< (UInt32)refreshType << ") and (" << refreshDate.LogStr() << ")!" << LogStream::eos;
			return false;
		}
	}

	if (GetRefreshTime() != 0)
	{
		SetBuyedNum(0);
	}
	SetRefreshTime(nextRefreshTimestamp);
	return true;
}


WSShopItemAccountBuyRecord::WSShopItemAccountBuyRecord(UInt32 accid)
{
	m_Owner = accid;
}

const std::string WSShopItemAccountBuyRecord::LogStr()
{
	std::ostringstream oss;
	oss << "Player(" << GetOwner() << ")'s buy record(" << GetID() << ","
		<< GetShopItemId() << "," << GetBuyedNum() << ")";
	return oss.str();
}

void WSShopItemAccountBuyRecord::SaveToDB()
{
	if (GetID() == 0)
	{
		SetID(CLRecordClient::Instance()->GenGuid());
		SetCreateTime(CURRENT_TIME.MSec());
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_account_shop_acc_buy_record", GetID());
		GetDBData(cmd, false);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account_shop_acc_buy_record", GetID(), true);
		GetDBData(cmd, true);
		if (cmd->GetDataNum() > 0)
		{
			CLRecordClient::Instance()->SendCommand(cmd);
		}
		else
		{
			CL_SAFE_DELETE(cmd);
		}
	}
}


WSShopItemRoleBuyRecord::WSShopItemRoleBuyRecord(ObjID_t roleid)
{
	m_Owner = roleid;
}

const std::string WSShopItemRoleBuyRecord::LogStr()
{
	std::ostringstream oss;
	oss << "Player(" << GetOwner() << ")'s buy record(" << GetID() << ","
		<< GetShopItemId() << "," << GetBuyedNum() << ")";
	return oss.str();
}

void WSShopItemRoleBuyRecord::SaveToDB()
{
	if (GetID() == 0)
	{
		SetID(CLRecordClient::Instance()->GenGuid());
		SetCreateTime(CURRENT_TIME.MSec());
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_account_shop_role_buy_record", GetID());
		GetDBData(cmd, false);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account_shop_role_buy_record", GetID(), true);
		GetDBData(cmd, true);
		if (cmd->GetDataNum() > 0)
		{
			CLRecordClient::Instance()->SendCommand(cmd);
		}
		else
		{
			CL_SAFE_DELETE(cmd);
		}
	}
}