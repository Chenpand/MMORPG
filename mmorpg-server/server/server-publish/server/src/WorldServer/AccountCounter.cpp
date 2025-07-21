#include "AccountCounter.h"

#include <CLRecordClient.h>
#include <CLAccountCounterDataEntry.h>


AccountCounter::AccountCounter()
{
	m_Owner = 0;
	m_CounterType = 0;
	m_CounterNum = 0;
	m_CreateTime = 0;
	m_RefreshTime = 0;
}

void AccountCounter::SetCounterNum(UInt64 num)
{
	m_CounterNum = num;
	SaveToDB();
}

UInt64 AccountCounter::GetCounterNum()
{ 
	TryRefresh(); 
	return m_CounterNum;
}

UInt64 AccountCounter::IncCounterNum(UInt64 incNum)
{
	if (incNum == 0) return GetCounterNum();
	SetCounterNum(GetCounterNum() + incNum);
	return GetCounterNum();
}

bool AccountCounter::TryRefresh()
{
	auto counterData = AccountCounterDataEntryMgr::Instance()->FindEntry(GetCounterType());
	if (!counterData)
	{
		ErrorLogStream << LogStr() << " can't find account counter data by type(" << (UInt32)GetCounterType() << ")!" << LogStream::eos;
		return false;
	}

	return TryRefresh((TimeUtil::RefreshType)counterData->refreshType, counterData->refreshDate);
}

bool AccountCounter::TryRefresh(TimeUtil::RefreshType refreshType, TimeUtil::RefreshDate refreshDate)
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
		ErrorLogStream << LogStr() << " calculate next account refresh timestamp failed by refresh type("
			<< (UInt32)refreshType << ") and (" << refreshDate.LogStr() << ")!" << LogStream::eos;
		return false;
	}

	if (GetRefreshTime() != 0)
	{
		SetCounterNum(0);
		InfoLogStream << LogStr() << " refreshed, pre time(" << GetRefreshTime() << ") next time(" << nextRefreshTimestamp << ")." << LogStream::eos;
	}

	m_RefreshTime = nextRefreshTimestamp;
	SaveToDB();

	return true;
}

const std::string AccountCounter::LogStr()
{
	std::ostringstream oss;
	oss << "Account(" << GetOwner() << ") counter(" << GetID() << ")";
	return oss.str();
}

void AccountCounter::SaveToDB()
{
	if (GetID() == 0)
	{
		SetID(CLRecordClient::Instance()->GenGuid());
		m_CreateTime = (UInt32)CURRENT_TIME.Sec();
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_account_counter", GetID());
		GetDBData(cmd, false);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account_counter", GetID(), false);
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