#include "MallItemBuyRecord.h"

#include <CLRecordClient.h>


MallItemAccountBuyRecord::MallItemAccountBuyRecord()
{
	m_MallItemId = 0;
	m_Owner = 0;
	m_BuyedNum = 0;
	m_RefreshTime = 0;
	m_CreateTime = 0;
}

void MallItemAccountBuyRecord::IncBuyedNum(UInt32 incNum)
{
	if (incNum == 0) return;
	m_BuyedNum = m_BuyedNum + incNum; 
	SaveToDB();
}

bool MallItemAccountBuyRecord::TryRefresh(TimeUtil::RefreshType refreshType, TimeUtil::RefreshDate refreshDate)
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
	if (!TimeUtil::CalculateNextRefreshTimestamp(refreshType, refreshDate, nextRefreshTimestamp))
	{
		ErrorLogStream << "Mall item(" << GetMallItemId() << ") calculate next account refresh timestamp failed by refresh type("
			<< (UInt32)refreshType << ") and (" << refreshDate.LogStr() << ")!" << LogStream::eos;
		return false;
	}

	if (GetRefreshTime() != 0)
	{
		SetBuyedNum(0);
	}
	SetRefreshTime(nextRefreshTimestamp);
	SaveToDB();

	return true;
}

const std::string MallItemAccountBuyRecord::LogStr()
{
	std::ostringstream oss;
	oss << "Player(" << GetOwner() << ")'s buy record(" << GetID() << ","
		<< GetMallItemId() << "," << GetBuyedNum() << ")";
	return oss.str();
}

void MallItemAccountBuyRecord::SaveToDB()
{
	if (GetID() == 0)
	{
		SetID(CLRecordClient::Instance()->GenGuid());
		SetCreateTime(CURRENT_TIME.MSec());
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_mall_acc_buy_record", GetID());
		GetDBData(cmd, false);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mall_acc_buy_record", GetID(), true);
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